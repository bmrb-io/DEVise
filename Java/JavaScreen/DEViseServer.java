// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides the interface between the jspop and an associated
// devised (server).  Basically, an object of this class gets a command
// from the client associated with the server, sends the command to
// the server, and then sends the reply commands and data back to the
// client.  It also keeps track of the current session file directory
// for the devised.

// There is one instance of this class for each devised connected to
// a jspop.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseServer.java

package JavaScreen;

import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable, DEViseCheckableThread
{
    private static int _debugLvl = 1;

    private jspop pop = null;

    private Process proc = null;
    private Thread serverThread = null;

    public String hostname = "localhost"; // hostname of jss/devised
    //public int dataPort = 0;
    public int cmdPort = 0, switchPort = 0;

    public int jssport = 0; // port the jss is listening on

    private DEViseCommSocket socket = null;
    private static final int devisedTimeout = 180 * 1000; // milliseconds
    //TEMP -- increased this from 1000 to see if it helps on yola.
    private static final int socketTimeout = 5000; // milliseconds

    // newClient is the client we're going to switch to the next time we
    // switch clients; client is the client we're currently connected to.
    private DEViseClient newClient = null;
    private DEViseClient client = null;

    private String[] serverCmds = null;

    private Vector currentDir = new Vector();
    private String rootDir = "DEViseSession";

    public static final int STATUS_STOPPED = 0, STATUS_RUNNING = 1;
    private int status = STATUS_STOPPED;
    private boolean isValid = false;

    public static final int ACTION_QUIT = 0, ACTION_IDLE = 1, ACTION_WORK = 2;
    private int action = ACTION_IDLE;

    private static int _nextObjectNum = 1;
    private int _objectNum = -1;

    private long _lastRunTime;

    private static boolean _createdTmpSessionDir = false;

    // The number of client commands we've run since the last client switch
    // (this is for diagnostics only).
    private int _cmdsSinceSwitch = 0;

    private static String deviseVersion = null;

    public long lastRunTime() {
        return _lastRunTime;
    }
    public void intThread() {
        serverThread.interrupt();
    }
    public String thread2Str() {
        return serverThread.toString();
    }
    public boolean isAlive() {
        return serverThread.isAlive();
    }
    public static String getDeviseVersion() {
        return deviseVersion;
    }

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    // name is the name of the system that the jss and devised(s) are running
    // on; port is the jssport.
    public DEViseServer(jspop j, String name, int port, int cmdport,
                        int imgport)
    {
        if (_debugLvl >= 4) {
            System.out.println("DEViseServer(" + _nextObjectNum +
                               ") constructor");
        }

        pop = j;
        if (name != null) {
            hostname = new String(name);
        }

        cmdPort = cmdport;
        //dataPort = imgport;
        jssport = port;
        isValid = true;

        _objectNum = _nextObjectNum++;
    }

    protected void finalize()
    {
        if (_debugLvl >= 2) {
            System.out.println("DEViseServer(" + _objectNum + ").finalize()");
        }

        // In case this somehow didn't get unregistered before (e.g.,
        // we got stop() call).
        DEViseThreadChecker.getInstance().unregister(this);
    }

    // Returns STATUS_*.
    public int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public void stop()
    {
        if (getStatus() != STATUS_STOPPED) {
            if (serverThread != null) {
                serverThread.stop();
                serverThread = null;
            }

            setStatus(STATUS_STOPPED);

            closeSocket();

            pop.pn("Removing server in DEViseServer.stop()");
            stopDEVise();
        }
    }

    public void start() throws YException
    {
        pop.pn("Trying to start a DEViseServer ...");

        // connect to the devised
        if (!startSocket()) {
            throw new YException("Can not start DEViseServer");
        }

        // start server Thread but first check if server thread is still active
        if (getStatus() == STATUS_STOPPED) {
            setStatus(STATUS_RUNNING);
            setAction(ACTION_IDLE);
            serverThread = new Thread(this);
            serverThread.setName("DEViseServer " + _objectNum);
            serverThread.start();
        }

        // Make sure the JSPoP's protocol version is compatible with the
        // devised.
        processClientCmd(DEViseCommands.PROTOCOL_VERSION + " " +
                         DEViseGlobals.PROTOCOL_VERSION);

        // Get the DEVise version (so we can report it to the client
        // if they request it).
        processClientCmd(DEViseCommands.GET_DEVISE_VERSION);

        pop.pn("Successfully started a DEViseServer ...");

        //
        // Have the DEVised create or clear the temporary session directory
        // if we haven't already done it.
        //
        String hostname = "unknown";
        try {
            InetAddress address = InetAddress.getLocalHost();
            hostname = address.getHostName();
        } catch (UnknownHostException ex) {
            System.err.println("Unable to get local host: " +
                               ex.getMessage());
        }

        if (!_createdTmpSessionDir) {
            System.out.println(
                "Creating or clearing temporary session directory");

            processClientCmd(DEViseCommands.CREATE_TMP_SESSION_DIR +
                             " " + hostname + " " + pop.getCmdPort());
            _createdTmpSessionDir = true;
        } else {
            processClientCmd(DEViseCommands.SET_TMP_SESSION_DIR +
                             " " + hostname + " " + pop.getCmdPort());
        }
    }

    private synchronized void closeSocket()
    {
        if (socket != null) {
            socket.closeSocket();
            socket = null;
        }
    }

    // Connect a socket to the devised.
    private synchronized boolean startSocket()
    {
        if (_debugLvl >= 1) {
            System.out.println("DEViseServer(" + _objectNum +
                               ").startSocket()");
        }

        // close previous connection first if any
        closeSocket();

        currentDir = new Vector();
        currentDir.addElement(new String(rootDir));

        // restart devised
        if (!startDEVise()) {
            pop.pn("Can not start a new devised");
            return false;
        }

        int time = 0, timestep = 1000;

        while (time < devisedTimeout) {
            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            time += timestep;

            try {
                socket = new DEViseCommSocket(hostname, cmdPort,  socketTimeout);

                pop.pn("Successfully connect to devised ...");

                return true;
            } catch (YException e) {
                pop.pn("Can not connect to devised, wait to try again ...");
                closeSocket();
            }
        }

        // can not connect within timeout
        closeSocket();
        pop.pn("Removing server because we can't connect within the timeout");
        stopDEVise();

        pop.pn("Can not connect to devised within timeout");

        return false;
    }

    private void stopDEVise()
    {
        isValid = false;
        pop.removeServer(this);
    }

    private boolean startDEVise()
    {
        return isValid;
    }

    public boolean isAvailable()
    {
        if (newClient == null) {
            return true;
        } else {
            return false;
        }
    }

    public DEViseClient getCurrentClient()
    {
        return client;
    }

    // Set the client to switch to the next time we switch clients.
    public synchronized void setCurrentClient(DEViseClient c)
    {
        if (_debugLvl >= 1) {
            pop.pn("DEViseServer(" + _objectNum +
                   ").setCurrentClient(" + c.getConnectionID() + ") in thread " +
                   Thread.currentThread());
            System.out.println("  current client is: " +
                               ((client != null) ? (new Long(client.getConnectionID()).toString()) :
                                "null"));
        }

        if (c == null) {
            return;
        }

        if (c.getStatus() == DEViseClient.SERVE) {
            System.err.println("ERROR!!! New client (" + c.getConnectionID() +
                               ") is already connected to a server!!!");
            return;
        }

        c.setStatus(DEViseClient.SERVE);
        newClient = c;
        notifyAll();
    }

    // Returns ACTION_*.
    private synchronized int getAction()
    {
        while (action == ACTION_IDLE && newClient == null) {
            try {
                DEViseThreadChecker.getInstance().unregister(this);
                if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
                           ").getAction() before wait()");
                }
                wait();
                if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
                           ").getAction() after wait()");
                }
                DEViseThreadChecker.getInstance().register(this);
            } catch (InterruptedException e) {
            }
        }

        if (action == ACTION_QUIT) {
            return action;
        }

        if (client == null && newClient == null) {
            action = ACTION_IDLE;
        } else {
            if (newClient != null) { // need to switch client
                pop.pn("Client switching happened in server running on (" +
                       hostname + ")");
                switchClient(true);
                action = ACTION_WORK;
            } else {
                try {
                    //Thread.sleep(100);
                    wait(100);
                } catch (InterruptedException e) {
                }
            }
        }

        return action;
    }

    private synchronized void setAction(int a)
    {
        action = a;
    }

    public void run()
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum + ").run()");
        }

        DEViseThreadChecker.getInstance().register(this);

        int todo;

        //TEMP -- kind of busywaiting?? YES
        //
        // Get and process commands from the client.
        //
        while (true) {
            Date date = new Date();
            _lastRunTime = date.getTime();

            todo = getAction();

            if (todo == ACTION_QUIT) {
                quit();

                break;
            } else if (todo == ACTION_WORK && client != null) {
                // while DEViseServer thread reach here, there is no way to
                // interrupt it until some error happened or it finish
                // processing one client's request
                if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
                           ").run() starting work on a command");
                }

                serverCmds = null;

                String clientCmd = null;

                //
                // Get a single command from the client.
                //
                boolean isEnd = false;
                while (!isEnd) {
                    date = new Date();
                    _lastRunTime = date.getTime();
                    try {

                        //
                        // If a collaborator just connected, initialize
                        // it.
                        //
                        if ( !client.collabClients.isEmpty()
                                && client.collabInit) {
                            initializeCollaborator();
                        }

                        // Get a command from the client.
                        // This method will not block, if no command it just
                        // returns null.
                        if (_debugLvl >= 4) {
                            pop.pn("DIAG before " +
                                   "client.getCmd() in " + serverThread);
                        }

                        clientCmd = client.getCmd();

                        if (_debugLvl >= 4) {
                            pop.pn("DIAG after " +
                                   "client.getCmd() in " + serverThread);
                        }
                        isEnd = true;
                    } catch (InterruptedIOException e) {
                        // since client.getCmd() will not block, this is
                        // meaningless
                        if (_debugLvl >= 1) {
                            System.err.println("InterruptedIOException " +
                                               e.getMessage() + " in DEViseServer(" +
                                               _objectNum + ").run()");
                        }
                    } catch (YException e) {
                        pop.pn("Client communication error1: " + e.getMsg());
                        removeCurrentClient();

                        isEnd = true;
                        clientCmd = null;
                    }
                }

                if (clientCmd != null && _debugLvl >= 1) {
                    pop.pn("Server (" + _objectNum + ") on " +
                           hostname + " got command " + clientCmd +
                           " from client " + client.getConnectionID() +
                           " in thread " + Thread.currentThread());
                }

                if (clientCmd != null) {
                    _cmdsSinceSwitch++;
                }

                if (clientCmd == null) {
                    continue;
                } else {
                    // We've just finished processing this command, so
                    // remove it from the client.
                    // We now have a reference to the command, so remove it
                    // from the client.  Note: I'm not sure why the client
                    // saves the command in the first place.  RKW 2000-11-07.
                    client.removeLastCmd();
                }

                Vector serverDatas = new Vector();

                //
                // Process the command.
                // Commands JAVAC_GetServerState, JAVAC_Abort & JAVAC_Connect
                // already been handled in DEViseClient.
                //
                try {
                    if (!processClientCmd(clientCmd)) {

                        // Close client socket for cgi version (we *must*
                        // close the socket, because the CGI script waits
                        // for EOF on the socket to know the server is
                        // finished).
                        if (client != null && client.useCgi()) {
                            client.closeSocket();
                            pop.pn("Socket between client and cgi is closed.");
                        }

                        continue;
                    }
                    processServerCmd(serverDatas);
                } catch (YException e) {
                    pop.pn("DEViseServer failed!");
                    pop.pn(e.getMsg());

                    if (clientCmd.startsWith(DEViseCommands.EXIT)) {
                        removeCurrentClient(false);
                    } else if (clientCmd.startsWith(
                                   DEViseCommands.CLOSE_SESSION)) {
                        switchClient();
                    } else {
                        try {
                            client.sendCmd(DEViseCommands.ERROR +
                                           " {Communication error occurs while talk to devised}");
                            switchClient();
                        } catch (YException e1) {
                            pop.pn("Client communication error2: " +
                                   e1.getMsg());
                            removeCurrentClient(false);
                        }
                    }

                    setAction(ACTION_QUIT);

                    continue;
                }

                if (clientCmd != null && _debugLvl >= 1) {
                    pop.pn("Devised on " + hostname +
                           " is done processsing command " + clientCmd +
                           " from client " + client.getConnectionID() +
                           " in thread " + Thread.currentThread());
                }

                //
                // Send response from server back to the client.
                //
                try {
                    client.sendCmd(serverCmds);

                    client.sendData(serverDatas);
                    pop.pn("Done sending all data to client(s)");

                    if (client.useCgi()) {
                        client.closeSocket();
                        pop.pn("Socket between client and cgi is closed.");
                    }
                } catch (YException e) {
                    pop.pn("Client communication error3: " + e.getMsg());
                    removeCurrentClient();
                }
                if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
                           ").run() finished work on a command");
                }
            }
        }

        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum + ").run() end");
        }
        DEViseThreadChecker.getInstance().unregister(this);
    }

    // Send a command that does not rely on client state.
    public void sendStatelessCmd(String cmd) throws YException
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum +
                   ").sendStatelessCmd(" + cmd + ")");
        }

        //
        // Test for a legal (statelesss) command.
        //
        if (!cmd.startsWith(DEViseCommands.PROTOCOL_VERSION) &&
                !cmd.startsWith(DEViseCommands.CREATE_TMP_SESSION_DIR) &&
                !cmd.startsWith(DEViseCommands.DELETE_TMP_SESSION)) {
            throw new YException("Illegal command (not stateless): " + cmd);
        }


        // We need to *not* save the server commands here because doing
        // so might goof up the client (if any) connected to this server.
        sendCmd(cmd, false);
    }

// ------------------------------------------------------------------------
// Method to process client commands (commands generated by the client,
// or by the JSPoP itself).

    // Returns true if command needs the rest of the "standard" processing,
    // false otherwise.
    private boolean processClientCmd(String clientCmd)
    throws YException
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum +
                   ").processClientCmd(" + clientCmd + ")");
        }

        // Log this command to the usage log.  Note:  client heartbeats
        // and some other commands are consumed in DEViseClient before
        // they get here.
        if (client != null) {
            pop.logUsage(client.hostname + " (" + client.ID + "): " +
                         clientCmd);
        }

        if (clientCmd.startsWith(DEViseCommands.PROTOCOL_VERSION)) {
            cmdProtocolVersion(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.GET_DEVISE_VERSION)) {
            cmdGetDeviseVersion(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.EXIT)) {
            cmdExit();

            // no need to return any response to client
            //TEMP -- should probably return a response here to fix bug 630.
            return false;

        } else if (clientCmd.startsWith(DEViseCommands.CLOSE_SESSION)) {
            cmdCloseSession();

        } else if (clientCmd.startsWith(DEViseCommands.GET_SESSION_LIST)) {
            cmdGetSessionList(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
            cmdSetDisplaySize(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.OPEN_SESSION)) {
            cmdOpenSession(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SAVE_CUR_SESSION)) {
            cmdSaveSession();
            return false;

        } else if (clientCmd.startsWith(DEViseCommands.REOPEN_SESSION)) {
            cmdReopenSession();

        } else if (clientCmd.startsWith(
                       DEViseCommands.CREATE_TMP_SESSION_DIR)) {
            cmdCreateTmpSessionDir(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SET_TMP_SESSION_DIR)) {
            cmdSetTmpSessionDir(clientCmd);

        } else {
            cmdClientDefault(clientCmd);
        }

        return true;
    }

// ------------------------------------------------------------------------
// Method to process server commands (commands generated by the server
// (DEVised) in response to a command from the client or JSPoP).  Basically,
// what we do here is pass the command back to the client, and, if necessary,
// get the associated data from the server so we can pass that along to
// the client as well.

    private void processServerCmd(Vector serverDatas) throws YException
    {
        // need to get whatever data is sending over by devised along with
        // the commands since serverCmds always ends with a JAVAC_Error or
        // JAVAC_Fail or JAVAC_Done, so we do not need to check the last
        // command
        if (!serverCmds[serverCmds.length - 1].startsWith(DEViseCommands.DONE)) {
            String tmpcmd = serverCmds[serverCmds.length - 1];
            serverCmds = new String[1];
            serverCmds[0] = tmpcmd;
        }

        for (int i = 0; i < (serverCmds.length - 1); i++) {
            if (serverCmds[i].startsWith(DEViseCommands.UPDATE_VIEW_IMAGE)) {
                cmdUpdateViewImage(serverCmds[i], serverDatas);
            } else if (serverCmds[i].startsWith(DEViseCommands.UPDATE_GDATA)) {
                cmdUpdateGData(serverCmds[i], serverDatas);
            }
        }
    }

// ------------------------------------------------------------------------
// Methods to process specific client commands.

    private void cmdProtocolVersion(String clientCmd)
    throws YException
    {
        if (!sendCmd(clientCmd)) {
            // Note: we're exiting here because otherwise we will just keep
            // trying to start a devised, and failing.  RKW 2001-11-12.
            System.err.println("JSPoP exiting because of protocol " +
                               "mismatch with devised!!");
            System.exit(1);
        }
    }

    private void cmdGetDeviseVersion(String clientCmd)
    throws YException
    {
        if (sendCmd(clientCmd)) {
            String[] args = DEViseGlobals.parseString(serverCmds[0]);
            if (args == null || args.length < 1) {
                throw new YException(
                    "Ill-formated command received from server \"" +
                    serverCmds[0]);
            } else {
                if (args[0].equals(DEViseCommands.DEVISE_VERSION)) {
                    if (deviseVersion == null) {
                        deviseVersion = args[1];
                    } else {
                        if (!deviseVersion.equals(args[1])) {
                            System.err.println(
                                "Warning: version of latest DEVised (" + args[1] +
                                ") is not the same as version of first DEVised (" +
                                deviseVersion + ")");
                        }
                    }
                } else {
                    throw new YException("Unexepected command " +
                                         serverCmds[0] + " received in response to " + clientCmd);
                }
            }
        } else {
            throw new YException("Unable to send command: " + clientCmd);
        }
    }

    public void cmdSaveSession() throws YException
    {
        pop.pn("We send the save_session command.");
        cmdClientDefault(DEViseCommands.SAVE_TMP_SESSION + " {" +
                         client.savedSessionName + "}");
        client.sessionSaved = true;
        client.tmpSessionExists = true;
        pop.pn("We send the close_session command.");
        cmdCloseSession();
        // keep the current session opened
        if (!client.isSessionOpened) {
            client.isSessionOpened = true;
        }
    }

    public void cmdReopenSession() throws YException
    {
        pop.pn("We send the reopen-session command.");
        sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" + client.savedSessionName + "}");
        client.sessionSaved = false;
        if ( !client.isSessionOpened )
            client.isSessionOpened = true;
    }

    public void cmdExit() throws YException
    {
        cmdCloseSession();
        cmdDeleteSession();
        removeCurrentClient(false);
    }

    private void cmdCloseSession() throws YException
    {
        client.isClientSwitched = false;
        client.isSwitchSuccessful = false;
        if (client.isSessionOpened) {
            client.isSessionOpened = false;
            sendCmd(DEViseCommands.CLOSE_SESSION);
            currentDir = new Vector();
            currentDir.addElement(rootDir);
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.DONE;
        }

        // also close the collaboration JSs
        if (!client.collabInit) {
            for (int i = 0; i < client.collabClients.size(); i++) {
                DEViseClient collabClient =
                    (DEViseClient)client.collabClients.elementAt(i);
                pop.pn("Sending command " + DEViseCommands.CLOSE_SESSION
                       + " to collabration client" + " " + i);
                collabClient.sendCmd(DEViseCommands.CLOSE_SESSION);
            }
        } // if (!client.collabInit)
    }

    private void cmdDeleteSession() throws YException
    {
        if (client.tmpSessionExists) {
            sendCmd(DEViseCommands.DELETE_TMP_SESSION + " " +
                    client.savedSessionName);
            client.tmpSessionExists = false;
        }
    }

    private void cmdGetSessionList(String clientCmd) throws YException
    {
        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 2 &&
                cmds[1].startsWith(rootDir)) {
            if (cmds[1].equals(rootDir)) {
                sendCmd(DEViseCommands.GET_SESSION_LIST);
            } else {
                String p = cmds[1].substring(14);
                sendCmd(DEViseCommands.GET_SESSION_LIST + " {" + p + "}");
            }
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR +
                            " {Invalid command: \"" + clientCmd + "\"}";
        }
    }

    private void cmdSetDisplaySize(String clientCmd) throws YException
    {
        boolean error = false;
        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 5) {
            try {
                client.screenDimX = Integer.parseInt(cmds[1]);
                client.screenDimY = Integer.parseInt(cmds[2]);
                client.screenResX = Integer.parseInt(cmds[3]);
                client.screenResY = Integer.parseInt(cmds[4]);
                if (client.screenDimX < 1 || client.screenDimY < 1 ||
                        client.screenResX < 1 || client.screenResY < 1) {
                    throw new NumberFormatException();
                }
            } catch (NumberFormatException e) {
                error = true;
            }
        } else {
            error = true;
        }

        if (error) {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR + " {Invalid command: \"" +
                            clientCmd + "\"}";
            client.screenDimX = -1;
            client.screenDimY = -1;
            client.screenResX = -1;
            client.screenResY = -1;
        } else {
            sendCmd(clientCmd);
        }
    }

    private void cmdOpenSession(String clientCmd) throws YException
    {
        client.sendCmd(DEViseCommands.UPDATEJS);

        if (client.isSessionOpened) {
            client.isSessionOpened = false;
            sendCmd(DEViseCommands.CLOSE_SESSION);
            currentDir = new Vector();
            currentDir.addElement(rootDir);
        }

        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 2 && cmds[1].startsWith(rootDir +
                "/")) {
            String p = cmds[1].substring(14);

            client.sessionName = p;

            boolean error = false;
            if (client.screenDimX > 0 && client.screenDimY > 0 &&
                    client.screenResX > 0 && client.screenResY > 0) {
                if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
                             client.screenDimX + " " + client.screenDimY + " " +
                             client.screenResX + " " + client.screenResY)) {
                    error = true;
                }
            }

            if (!error) {
                if (sendCmd(DEViseCommands.OPEN_SESSION + " {" +
                            client.sessionName + "}")) {
                    client.isSessionOpened = true;
                } else {
                    // need to clear socket because there might be some
                    // data on data socket
                    int count;
                    try {
                        count = countData();
                    } catch (YException ee) {
                        count = -1;
                        pop.pn(ee.getMessage());
                    }
                    socket.clearSocket(count);
                }
            }
        }

        if (!client.isSessionOpened) {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR +
                            " {Can not open session \"" + client.sessionName + "\"}";
        }
    }

    private void cmdCreateTmpSessionDir(String clientCmd)
    throws YException
    {
        if (!sendCmd(clientCmd)) {
            System.err.println("Error requesting creation/clearing of " +
                               "temporary session directory");
        }
    }

    private void cmdSetTmpSessionDir(String clientCmd)
    throws YException
    {
        if (!sendCmd(clientCmd)) {
            System.err.println("Error setting temporary session directory");
        }
    }

// ------------------------------------------------------------------------
// Method to process all client commands not using the command-specific
// methods.

    private void cmdClientDefault(String clientCmd) throws YException
    {
        if (_debugLvl >= 2) {
            System.out.println("DEViseServer(" + _objectNum +
                               ").cmdClientDefault(" + clientCmd + ")");
        }

        if (client.isClientSwitched) {
            client.isClientSwitched = false;

            if (client.isSwitchSuccessful) {
                client.isSwitchSuccessful = false;

                boolean error = false;
                if (client.screenDimX > 0 && client.screenDimY > 0) {
                    if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
                                 client.screenDimX + " " + client.screenDimY + " " +
                                 client.screenResX + " " + client.screenResY)) {
                        error = true;
                        //} else {
                        //    pop.pn("Switch error: can not send " +
                        //      DEViseCommands.SET_DISPLAY_SIZE);
                    }
                }

                if (!error) {
                    if (sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" +
                                client.savedSessionName + "}")) {
                        client.isSessionOpened = true;
                    } else {
                        pop.pn("Switch error: Can not send " +
                               DEViseCommands.OPEN_TMP_SESSION + " " +
                               client.savedSessionName);
                    }
                    // need to clear socket because there might be some
                    // useless data on data socket
                    //socket.clearSocket();
                }

                // need to clear socket because there might be some useless
                // data on data socket
                int count;
                try {
                    count = countData();
                } catch (YException ee) {
                    count = -1;
                    pop.pn(ee.getMessage());
                }
                socket.clearSocket(count);
                // socket.clearSocket(-1);
            } else {
                pop.pn("Switch error: client switch not successful");
            }
        }

        if (client.isSessionOpened) {
            if (!sendCmd(clientCmd)) {
                // need to clear data socket when error happened because
                // there might be some data on data socket
                int count;
                try {
                    count = countData();

                } catch (YException ee) {
                    count = -1;
                    pop.pn(ee.getMessage());
                }
                socket.clearSocket(count);
            }
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.FAIL +
                            " {Do not have an opened session or Can not open last saved session}";
        }
    }

// ------------------------------------------------------------------------
// Methods to process specific server commands.

    private void cmdUpdateViewImage(String serverCmd, Vector serverDatas)
    throws YException
    {
        String[] cmds = DEViseGlobals.parseString(serverCmd);
        if (cmds != null && cmds.length == 3) {
            try {
                int imgSize = Integer.parseInt(cmds[2]);
                if (imgSize < 1) {
                    throw new NumberFormatException();
                }

                byte[] image = receiveData(imgSize);

                serverDatas.addElement(image);
            } catch (NumberFormatException e1) {
                throw new YException("Invalid image size in command \"" +
                                     serverCmd + "\"");
            }
        } else {
            throw new YException("Ill-formated command \"" + serverCmd + "\"");
        }
    }

    private void cmdUpdateGData(String serverCmd, Vector serverDatas)
    throws YException
    {
        String[] cmds = DEViseGlobals.parseString(serverCmd);
        if (cmds != null && cmds.length == 7) {
            try {
                int dataSize = Integer.parseInt(cmds[6]);
                if (dataSize < 1) {
                    throw new NumberFormatException();
                }

                byte[] data = receiveData(dataSize);

                serverDatas.addElement(data);
            } catch (NumberFormatException e1) {
                throw new YException("Invalid GData size in command \"" +
                                     serverCmd + "\"");
            }
        } else {
            throw new YException("Ill-formated command \"" + serverCmd + "\"");
        }
    }

// ------------------------------------------------------------------------

    private synchronized void quit()
    {
        closeSocket();

        pop.pn("Removing server in DEViseServer.quit()");
        stopDEVise();

        setStatus(STATUS_STOPPED);
    }

    private synchronized void removeCurrentClient()
    {
        removeCurrentClient(true);
    }

    private synchronized void removeCurrentClient(boolean isClose)
    {
        if (client != null) {
            if (isClose && client.isSessionOpened) {
                try {
                    sendCmd(DEViseCommands.CLOSE_SESSION);
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed.");
                    pop.pn(e.getMsg());

                    //if (!startSocket()) {
                    setAction(ACTION_QUIT);
                    //}
                }
            }

            // Suspend client before closing it so we don't get the
            // warning I just added to DEViseClient.close().  RKW 2002-04-12.
            pop.suspendClient(client);
            client.close();
            client = null;
        }
    }

    private synchronized void switchClient()
    {
        switchClient(false);
    }

    // Note: this method is sometimes called with startNewClient false,
    // to just save the state and clean up for the old client (for
    // example, if the client crashes).
    private synchronized void switchClient(boolean startNewClient)
    {
        if (_debugLvl >= 1) {
            pop.pn("DEViseServer(" + _objectNum +
                   ").switchClient(" + startNewClient + ") in thread " +
                   Thread.currentThread());
        }

        if (client != null) {
            if (_debugLvl >= 1) {
                System.out.println("  Current client is: " + client.ID);
            }

            if (client.isSessionOpened) {
                client.isClientSwitched = true;
                client.isSwitchSuccessful = false;
                client.isSessionOpened = false;

                try {
                    if (!client.sessionSaved) {
                        if (sendCmd(DEViseCommands.SAVE_TMP_SESSION + " {" +
                                    client.savedSessionName + "}")) {
                            client.isSwitchSuccessful = true;
                            client.tmpSessionExists = true;
                        } else {
                            pop.pn("Can not save session for old client while switching client!");
                        }
                    } else {
                        client.isSwitchSuccessful = true;
                    }

                    if (!sendCmd(DEViseCommands.CLOSE_SESSION)) {
                        pop.pn("Can not close current session for old client while switching client!");
                    }
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    //if (!startSocket()) {
                    setAction(ACTION_QUIT);
                    //}
                }
            }

            pop.suspendClient(client);

            if (_debugLvl >= 1) {
                System.out.println("Server (" + _objectNum + ") on " +
                                   hostname + " ran " + _cmdsSinceSwitch +
                                   " commands for client " + client.getConnectionID() +
                                   " since the last client switch");
            }
            _cmdsSinceSwitch = 0;

            client = null;
        } else {
            if (_debugLvl >= 1) {
                System.out.println("  No current client");
            }
        }

        if (startNewClient) {
            client = newClient;
            newClient = null;
            if (client != null) {
                if (_debugLvl >= 1) {
                    pop.pn("DEViseServer(" + _objectNum +
                           ") actually switched to client " +
                           client.getConnectionID());
                }
                pop.activateClient(client);
            }
        }
    }

    private byte[] receiveData(int size) throws YException
    {
        if (socket == null)
            throw new YException("Invalid communication socket");

        boolean isEnd = false;
        byte[] data = null;
        long time = 0;

        pop.pn("Receiving data from devised(" + hostname + ") of size " + size);
        pop.pn(socket.dataAvailable() + " bytes of data available on socket");
        while (!isEnd) {
            data = socket.receiveData(size);
            pop.pn("Successfully received data from devised(" + hostname +
                   "at " + cmdPort + ") of size " + size);
            isEnd = true;
        }

        return data;
    }

    // Send a command to the server; returns true if the command was
    // successfully processed.
    private boolean sendCmd(String clientCmd) throws YException
    {
        return sendCmd(clientCmd, true);
    }

    // Send a command to the server; returns true if the command was
    // successfully processed.
    private synchronized boolean sendCmd(String clientCmd,
                                         boolean saveServerCmds) throws YException
    {
        if (clientCmd == null) {
            return true;
        }

        if (socket == null) {
            throw new YException("Invalid communication socket");
        }

        String response = null;
        boolean isEnd = false, isFinish = false, isError = false;
        long time;

        Vector rspbuf = new Vector();

        pop.pn("Sending command to devised(" + hostname + " at " +
               cmdPort + ") :  \"" + clientCmd + "\"");
        socket.sendCmd(clientCmd, (short)5, 9999);

        isEnd = false;
        while (!isEnd) {
            isFinish = false;
            time = 0;

            while (!isFinish) {
                try {
                    response = socket.receiveCmd();
                    pop.pn("Receive response from devised(" + hostname +
                           " at " + cmdPort + ") :  \"" + response + "\"");
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    if (_debugLvl >= 1) {
                        System.err.println("socket.receiveCmd() " +
                                           "interrupted in DEViseServer(" + _objectNum +
                                           ").sendCmd()");
                    }
                    time += socketTimeout;
                    if (time > devisedTimeout) {
                        throw new YException("Cannot receive response " +
                                             "from devised within timeout");
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Invalid response received from devised");
            } else {
                if (_debugLvl >= 4) {
                    pop.pn("DIAG Parsing response from devised");
                }
                String[] cmds = DEViseGlobals.parseString(response);
                if (_debugLvl >= 4) {
                    pop.pn("DIAG Done parsing response from " +
                           "devised");
                }
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated response \"" +
                                         response + "\" received from devised");
                } else {
                    //pop.pn("Receive response \"" + response +
                    //"\" from devised");

                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not
                    // the arguments?????
                    for (int j = 1; j < cmds.length; j++) {
                        cmd = cmd + " {" + cmds[j] + "}";
                    }
                    if (_debugLvl >= 4) {
                        pop.pn("DIAG Done removing(?) braces");
                    }

                    if (cmd.startsWith(DEViseCommands.DONE)) {
                        isEnd = true;
                        isError = false;
                    } else if (cmd.startsWith(DEViseCommands.ERROR) ||
                               cmd.startsWith(DEViseCommands.FAIL)) {
                        isEnd = true;
                        isError = true;
                        rspbuf.removeAllElements();
                    }

                    if (_debugLvl >= 4) {
                        pop.pn("DIAG Adding command to rspbuf");
                    }
                    rspbuf.addElement(cmd);
                    if (_debugLvl >= 4) {
                        pop.pn("DIAG Done adding command " +
                               "to rspbuf");
                    }
                }
            }
        }

        if (saveServerCmds) {
            serverCmds = new String[rspbuf.size()];
            for (int i = 0; i < rspbuf.size(); i++) {
                serverCmds[i] = (String)rspbuf.elementAt(i);
            }
        }

        return !isError;
    }

    // Determine the number of bytes on the data socket associated with
    // the commands currently in serverCmds.
    private int countData() throws YException
    {
        int count = 0;
        for (int i = 0; i < (serverCmds.length - 1); i++) {
            if (serverCmds[i].startsWith(DEViseCommands.UPDATE_VIEW_IMAGE)) {
                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                if (cmds != null && cmds.length == 3) {
                    try {
                        int imgSize = Integer.parseInt(cmds[2]);
                        if (imgSize < 1) {
                            throw new NumberFormatException();
                        }

                        count += imgSize;
                    } catch (NumberFormatException e1) {
                        throw new YException("Invalid image size in command \"" + serverCmds[i] + "\" while counting data in the socket");
                    }
                } else {
                    throw new YException("Ill-formated command \"" + serverCmds[i] + "\" while counting data in the socket");
                }
            } else if (serverCmds[i].startsWith(DEViseCommands.UPDATE_GDATA)) {
                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                if (cmds != null && cmds.length == 7) {
                    try {
                        int dataSize = Integer.parseInt(cmds[6]);
                        if (dataSize < 1) {
                            throw new NumberFormatException();
                        }

                        count += dataSize;
                    } catch (NumberFormatException e1) {
                        throw new YException("Invalid GData size in command \"" + serverCmds[i] + "\" while counting data in the socket");
                    }
                } else {
                    throw new YException("Ill-formated command \"" + serverCmds[i] + "\" while counting data in the socket");
                }
            }
        }

        return count;
    }

    private void initializeCollaborator() throws YException
    {
        if (_debugLvl >= 1) {
            pop.pn("DEViseServer(" + _objectNum +
                   ").initializeCollaborator()" + " in thread " +
                   Thread.currentThread());
        }

        //
        // We save and re-open the current session so that the re-opening
        // generates all of the commands needed to initialize a new
        // collaboration follower.
        //
        pop.pn("We send the save-session command.");
        processClientCmd(DEViseCommands.SAVE_TMP_SESSION + " {" +
                         client.savedSessionName + "}");
        // only when isSessionOpened = true,
        // this save_tmp_session command can be sent out
        if (client.isSessionOpened)
            client.tmpSessionExists = true;

        pop.pn("We send the close-session command.");
        //server.serverCmds = null;
        processClientCmd(DEViseCommands.CLOSE_SESSION);

        // keep the current session opened
        if ( !client.isSessionOpened ) {
            client.isSessionOpened = true;
        }

        if (client.tmpSessionExists) {
            pop.pn("We send the open-session command.");
            sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" +
                    client.savedSessionName + "}");
        }

        Vector serverDatas = new Vector();
        processServerCmd(serverDatas);

        client.sendCmd(serverCmds);
        client.sendData(serverDatas);
        pop.pn("Done sending all data to " + "collaboration client");
        pop.pn("  client.collabInit = " + client.collabInit);
    }
}
