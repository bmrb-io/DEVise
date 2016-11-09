// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class maintains the state for each client (JavaScreen).  It is
// also used to read commands from the client, and send commands and
// data to the client.

// There is one instance of this class for each client (JavaScreen)
// connected to a jspop.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseClient.java

package JavaScreen;

import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.*;
import java.text.*;

public class DEViseClient
{
    private static int _debugLvl = 1;

    jspop pop = null;

    // Client states.  CLOSE means connection is not yet established,
    // or has been closed; REQUEST means JS has sent a command, but
    // the client is not currently assigned to a server; IDLE means the
    // client is not currently assigned to a server, and no command is
    // pending; SERVE means that the client is assigned to a server.
    public static final int CLOSE = 0, REQUEST = 1, IDLE = 2, SERVE = 3;
    private int status = CLOSE;

    public DEViseUser user = null;
    public int ID = 0;
    public String hostname = null;
    public DEViseClientSocket clientSock = null;

    public boolean isSessionOpened = false;
    public boolean isClientSwitched = false;
    public boolean isSwitchSuccessful = false;

    //public String path = "DEViseSession";
    public String sessionName = null;

    // Name of temporary session file (saves state for client switches,
    // etc.).
    public String savedSessionName = null;

    // Whether temporary session file exists.
    public boolean tmpSessionExists = false;

    public int screenDimX = -1;
    public int screenDimY = -1;
    public int screenResX = -1;
    public int screenResY = -1;

    public long lastActiveTime = -1;
    public long lastSuspendTime = -1;

    // Pending commands (will be sent to the devised, unless dealt with
    // otherwise).
    public Vector cmdBuffer = new Vector();

    private boolean cgi; // whether to use cgi instead of socket

    //private String firstCmd = null;

    // Timestamp of most recent command from JS.
    private long heartBeat;

    private static int _objectCount = 0;

    //TEMP -- do these really all have to be public?
    public Vector collabClients = new Vector();
    public boolean collabInit = false;

    public boolean isAbleCollab = false;
    private String collabPass = null;
    private String collabName = null;
    public boolean sessionSaved = false;
    public DEViseClient collabLeader = null;

    public YLogFile logFile = null;

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }


    public DEViseClient(jspop p, String host, DEViseClientSocket cs, int id,
                        boolean cgi)
    {
        pop = p;

        if (_debugLvl >= 2) {
            pop.pn("DEViseClient.DEViseClient(" + host + ", " +
                   id + ", " + cs.getObjectNum() + ") in thread " +
                   Thread.currentThread());
        }

        hostname = host;
        clientSock = cs;
        ID = id;

        savedSessionName = "jstmp_" + ID;

        status = IDLE;

        updateHeartbeat();

        _objectCount++;

        this.cgi = cgi;
    }

    protected void finalize()
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient(" + ID +
                   ").finalize() in thread " + Thread.currentThread());
        }

        _objectCount--;
    }

    public static int getObjectCount() {
        return _objectCount;
    }

    public void setSocket(DEViseClientSocket cs) {
        clientSock = cs;
    }

    public void closeSocket()
    {
        if (clientSock != null) {
            clientSock.closeSocket();
            clientSock = null;
        }
    }

    public void addCollabClient(DEViseClient client, String hostname) {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient.addCollabClient(" + client.ID +
                   ", " + hostname + ")");
        }
        collabClients.addElement(client);
        collabInit = true;
    }

    public void removeCollabClient(DEViseClient client) {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient.removeCollabClient(" +
                   client.ID + ")");
        }

        collabClients.removeElement(client);

        pop.pn("Collaborating JS: " + client.ID + " is removed.");
        // client.closeSocket();
    }


    public void addNewCmd(String cmd) {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient(" + ID +
                   ").addNewCmd(" + cmd + ") in thread " + Thread.currentThread());
        }

        updateHeartbeat();

        boolean shouldCloseSock = useCgi();

        // Note: commands that do not actually require communication with
        // a devised should be directly handled here to avoid unnecessary
        // client switches. RKW 2001-11-09.
        if (cmd.startsWith(DEViseCommands.HEART_BEAT)) {
            // Note: this must be dealt with here so we don't generate
            // unnecessary server switches.
            try {
                if (false) {
                    // For testing only!!!
                    try {
                        Thread.sleep(3 * 1000);
                    } catch (InterruptedException ex) {
                        pop.pn("Sleep interrupted: " +
                               ex.getMessage());
                    }
                }
                // Note: special case here -- this should not be sent to
                // followers if we're in collaboration mode.
                sendCmd(DEViseCommands.DONE, false);
            } catch (YException ex) {
                System.err.println("YException in " +
                                   "DEViseClient.addNewCmd(): " + ex.getMessage());
            }

        } else if (cmd.startsWith(DEViseCommands.CHECK_POP)) {
            try {
                if (pop.getServerCount() >= 1) {
                    sendCmd(DEViseCommands.DONE);
                } else {
                    System.err.println(DEViseCommands.CHECK_POP +
                                       " fails because no servers are connected");
                    sendCmd(DEViseCommands.ERROR);
                }
            } catch (YException ex) {
                System.err.println("Error (" + ex.getMessage() +
                                   ") sending " + DEViseCommands.DONE +
                                   " command in response to " + DEViseCommands.CHECK_POP);
            }

            // Close here because the client exits after getting the reply.
            close();
            shouldCloseSock = false;

        } else if (cmd.startsWith(DEViseCommands.CONNECT)) {
            connect(cmd);

        } else if (cmd.startsWith(DEViseCommands.ASK_COLLAB_LEADER)) {
            askCollabLeader(cmd);

        } else if (cmd.startsWith(DEViseCommands.COLLABORATE)) {
            collaborate(cmd);
            addLogFile(cmd);

        } else if (cmd.startsWith(DEViseCommands.GET_COLLAB_LIST)) {
            getCollabList();
            addLogFile(cmd);

        } else if (cmd.startsWith(DEViseCommands.SET_COLLAB_PASS)) {
            setCollabPassword(cmd);
            addLogFile(cmd);

        } else if (cmd.startsWith(DEViseCommands.SET_3D_CONFIG)) {
            sendCmdToCollaborators(cmd);
            newCommandStd(cmd);

        } else if (cmd.startsWith(DEViseCommands.CLOSE_COLLAB_DLG)) {
            sendCmdToCollaborators(cmd);
            sendCmdToCollaborators(DEViseCommands.DONE);

        } else if (cmd.startsWith(DEViseCommands.HIDE_ALL_VIEW_HELP)) {
            try {
                sendCmdToCollaborators(cmd);
                sendCmd(DEViseCommands.DONE);
            } catch(YException ex) {
                System.err.println("Error sending command to collaborator: " +
                                   ex.getMessage());
            }
            addLogFile(cmd);

        } else {
            newCommandStd(cmd);
            shouldCloseSock = false;
        }

        if (shouldCloseSock) {
            closeSocket();
        }
    }

    private void newCommandStd(String cmd) {
        cmdBuffer.addElement(cmd);

        // add command to logfile
        addLogFile(cmd);

        if (getStatus() != SERVE) {
            setStatus(REQUEST);
        }
        // Note: socket can't be closed here because we have to
        // send the reply.
    }

    private void addLogFile(String cmd)
{   if (pop.clientLog && logFile == null) {
            String time = new Long(ID).toString();
            String logName = "logs/client.log." + time;
            logFile = new YLogFile(logName);
            logFile.pn("# Start of logfile for client " + ID +
                       " at host " + hostname);
        }

        // add command to logfile
        if (pop.clientLog && logFile != null) {
            Date d = new Date();
            long t = d.getTime();
            String timestamp = new Long(t).toString();
            DateFormat dtf = DateFormat.getDateTimeInstance(
                                 DateFormat.MEDIUM, DateFormat.MEDIUM);
            logFile.pn(timestamp + "  # " + dtf.format(d));
            logFile.pn(cmd);
        }
    }


    public boolean useCgi() {
        return cgi;
    }

    public void setCgi(boolean c) {
        cgi = c;
    }

    public int getPriority()
    {
        if (user != null) {
            return user.getPriority();
        } else {
            return 1;
        }
    }

    public long getConnectionID()
    {
        return ID;
    }

    public String getCollabName()
    {
        return collabName;
    }

    public String getHostname()
    {
        return hostname;
    }

    public synchronized void setSuspend()
    {
        // happens during switching
        if (status == REQUEST || ! cmdBuffer.isEmpty()) {
            status = REQUEST;
            return;
        }

        if (status != CLOSE) {
            lastSuspendTime = DEViseGlobals.getCurrentTime();
            lastActiveTime = -1;
            status = IDLE;
        }
    }

    public synchronized void setActive()
    {
        if (status != CLOSE) {
            lastActiveTime = DEViseGlobals.getCurrentTime();
            lastSuspendTime = -1;
            status = SERVE;
        }
    }

    // Get the amount of time this client has been suspended (0 if not
    // suspended).
    public synchronized long getSuspendTime()
    {
        if (lastSuspendTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastSuspendTime;
        }
    }

    // Get the amount of time this client has been active, since it was
    // last suspended (0 if not active).
    public synchronized long getActiveTime()
    {
        if (lastActiveTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastActiveTime;
        }
    }

    // Get the status of the client (CLOSE, etc.).  Note that this function
    // does not merely return the current state -- if the current state
    // is IDLE, it checks the command socket, and if there is a pending
    // command the state becomes REQUEST).
    public synchronized int getStatus()
    {
        if (_debugLvl >= 4) {
            pop.pn("DEViseClient(" + ID +
                   ").getStatus() in thread " + Thread.currentThread());
        }

        if (status == IDLE) {
            try {
                if (!isSocketEmpty() || !cmdBuffer.isEmpty()) {
                    status = REQUEST;
                }
            } catch(YException ex) {
                pop.pn("YException in DEViseClient.getStatus(): " +
                       ex.getMsg());
                close();
            }
        }

        return status;
    }

    public synchronized void setStatus(int s)
    {
        if (status != CLOSE) {
            status = s;
        }
    }

    public void updateHeartbeat()
    {
        if (_debugLvl >= 2) {
            pop.pn("Updating heartbeat for client " + ID);
        }

        Date date = new Date();
        heartBeat = date.getTime();

        if (_debugLvl >= 4) {
            pop.pn("Client " + ID + " heartbeat updated to " +
                   heartBeat + " (" + date + ")");
        }
    }

    // Timestamp of the command we've most recently received.
    public long getHeartbeat()
    {
        return heartBeat;
    }

    public void removeLastCmd()
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient(" + ID +
                   ").removeLastCmd() in thread " + Thread.currentThread());
        }

        if (cmdBuffer.size() > 0) {
            cmdBuffer.removeElementAt(0);
        }
    }

    private synchronized boolean isSocketEmpty() throws YException
    {
        if (status != CLOSE) {
            if (clientSock != null) {
                return !clientSock.hasCommand();
            } else {
                return true;
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    public String getCmd() throws YException, InterruptedIOException
    {
        if (_debugLvl >= 4) {
            pop.pn("DEViseClient(" + ID + ").getCmd()");
        }

        if (getStatus() != CLOSE) {

            if (cmdBuffer.size() == 0) {
                if (_debugLvl >= 4) {
                    pop.pn("Got null command");
                }
                return null;
            }

            try {
                if (_debugLvl >= 6) {
                    pop.pn("DIAG before getting cmdBuffer " +
                           "element 0");
                }

                String command = (String)cmdBuffer.elementAt(0);

                if (_debugLvl >= 6) {
                    pop.pn("DIAG after getting cmdBuffer " +
                           "element 0 (" + command + ")");
                }

                //TEMP -- we don't seem to check argument counts here!!
                if (command != null) {
                    //TEMP -- move to addNewCmd()?
                    //TEMP -- should probably allow JAVAC_Exit through -- at
                    // any rate, make sure client object gets deleted
                    if (!command.startsWith(DEViseCommands.CONNECT) &&
                            user == null) {
                        sendCmd(DEViseCommands.ERROR +
                                " {No user information given}");
                        throw new YException(
                            "Cannot get user information for this client");
                    }

                    //
                    // Commands specifically checked for in this 'if'
                    // require special processing.  All other commands
                    // are "normal".
                    //
                    if (command.startsWith(DEViseCommands.ABORT)) {
                        //TEMP -- move to addNewCmd()?
                        cmdBuffer.removeAllElements();
                    } else if (command.startsWith(DEViseCommands.CLOSE_SESSION)) {
                        //sendCmd(DEViseCommands.UPDATAJS);
                        cmdBuffer.removeAllElements();
                        cmdBuffer.addElement(DEViseCommands.CLOSE_SESSION);
                    } else if (command.startsWith(DEViseCommands.SAVE_CUR_SESSION)) {
                        cmdBuffer.removeElement(command);
                        cmdBuffer.addElement(DEViseCommands.SAVE_CUR_SESSION);
                        sendCmd(DEViseCommands.DONE);

                    } else if (command.startsWith(DEViseCommands.EXIT)) {
                        cmdBuffer.removeAllElements();
                        cmdBuffer.addElement(DEViseCommands.EXIT);

                        // remove collab name from jspop
                        boolean f = pop.collabNames.removeElement(collabName);
                        pop.pn("Remove collab-name " + collabName + ": " + f);
                        sendCmdToCollaborators(DEViseCommands.COLLAB_EXIT);
                        sendCmdToCollaborators(DEViseCommands.DONE);
                        // Note: if we don't remove *all* elements here,
                        // we sometimes end up with an extra JAVAC_CollabExit,
                        // which goofs things up because it gets passed to
                        // the devised, which doesn't know what to do with
                        // it.  RKW 2003-10-15.
                        collabClients.removeAllElements();

                        // Close the socket right away to prevent "Abrupt
                        // end of input stream" errors on normal client
                        // exit.
                        closeSocket();

                    } else if (command.startsWith(DEViseCommands.GET_SERVER_STATE)) {
                        //TEMP -- move to addNewCmd()?
                        String state = DEViseCommands.UPDATE_SERVER_STATE + " " + pop.getServerState();
                        sendCmd(new String[] {state, DEViseCommands.DONE});
                        cmdBuffer.removeAllElements();

                    } else if (command.startsWith(DEViseCommands.DISABLE_COLLAB)) {
                        //TEMP -- move to addNewCmd()?
                        isAbleCollab = false;

                        sendCmdToCollaborators(DEViseCommands.COLLAB_EXIT);
                        sendCmdToCollaborators(DEViseCommands.DONE);

                        collabClients.removeAllElements();
                        sendCmd(DEViseCommands.DONE);
                        cmdBuffer.removeAllElements();

                        // remove collab name from jspop
                        boolean f = pop.collabNames.removeElement(collabName);
                        pop.pn("Remove collab-name: " + f);

                    } else if (command.startsWith(DEViseCommands.STOP_COLLAB)) {
                        //TEMP -- move to addNewCmd()?
                        if (collabLeader != null) {
                            collabLeader.removeCollabClient(this);
                            collabLeader = null;
                        }
                        cmdBuffer.removeAllElements();

                    } else if (command.startsWith(DEViseCommands.GET_POP_VERSION)) {
                        String[] args = DEViseGlobals.parseString(command);
                        if (args != null && args.length == 3) {
                            //TEMP -- move to addNewCmd()?
                            String version = DEViseCommands.POP_VERSION + " " +
                                             DEViseGlobals.VERSION + " " + pop.getPopId() +
                                             " {" + DEViseServer.getDeviseVersion() + "}" +
                                             " " + args[1] + " " + args[2];
                            sendCmd(new String[] {version, DEViseCommands.DONE});
                            cmdBuffer.removeAllElements();
                        } else {
                            sendCmd(DEViseCommands.ERROR +
                                    " {Invalid PoP version request -- bad argument count}");
                            throw new YException(
                                "Invalid PoP version request received from client -- bad argument count (" +
                                args.length + ") in " + args[0]);
                        }

                    } else {
                        //
                        // Send an ACK immediately so that the client
                        // knows that we received the command.
                        //
                        //cmdBuffer.addElement(command);
                        sendCmd(DEViseCommands.ACK);
                    }
                }

                // Close client socket for cgi version (we *must*
                // close the socket, because the CGI script waits
                // for EOF on the socket to know the server is
                // finished).  (Note that we only close the socket
                // here for commands that don't get sent to the devised.
                // For commands that go to the devised, we close the
                // socket in the DEViseServer object.)
                if (useCgi() && cmdBuffer.size() <= 0) {
                    closeSocket();
                    pop.pn("Socket between client and cgi is closed.");
                }

                if (_debugLvl >= 2 && cmdBuffer.size() > 0) {
                    pop.pn("  got command: " +
                           (String)cmdBuffer.elementAt(0));
                }
                if (cmdBuffer.size() > 0) {
                    return (String)cmdBuffer.elementAt(0);
                } else {
                    return null;
                }
            } catch (YException e) {
                //close();
                throw e;
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    // Send a series of commands to the client.
    public synchronized void sendCmd(String[] cmds) throws YException
    {
        for (int i = 0; i < cmds.length; i++) {
            sendCmd(cmds[i], true);
        }
    }

    // Send a single command to the client.
    public synchronized void sendCmd(String cmd) throws YException
    {
        sendCmd(cmd, true);
    }

    // Send a single command to the client.
    public synchronized void sendCmd(String cmd, boolean sendToFollowers)
    throws YException
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient(" + ID + ").sendCmd(" +
                   cmd + ", " + sendToFollowers + ")");
        }

        if (status != CLOSE) {
            if (cmd != null) {

                // Here we send commands to followers firstly (if any)
                // since we may change the JAVAC_CollabState command.
                if (sendToFollowers && !collabClients.isEmpty()) { // also send to collab JS
                    if (collabInit) {
                        // if command = DEViseCommands.ERROR here,
                        // means leader has no session opened.
                        if (! cmd.startsWith(DEViseCommands.ERROR)) {
                            //
                            // Send command only to the most-recently-connected
                            // collaboration client.
                            //
                            DEViseClient client =
                                (DEViseClient)collabClients.lastElement();

                            pop.pn("Sending command to collabration client: " + cmd);
                            client.sendCmd(cmd);
                        }
                    } else {
                        //
                        // Send commands to all collaboration clients.
                        //
                        sendCmdToCollaborators(cmd);
                    }
                }

                // Note: the !sendToFollowers check here is so that if
                // the leader sends a heartbeat during a collab follower
                // initialization, the reply to the heartbeat still gets
                // sent back to the leader.
                if (!collabInit || !sendToFollowers) {
                    //
                    // Send command to "normal" client.
                    //
                    pop.pn("Sending command to client(" + ID + " " + hostname +
                           ") :  \"" + cmd + "\"");
                    clientSock.sendCommand(cmd);
                }
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void sendData(Vector data) throws YException
    {
        if (status != CLOSE) {
            if (data == null) {
                return;
            }

            try {
                for (int i = 0; i < data.size(); i++) {
                    byte[] d = (byte[])data.elementAt(i);
                    if (d != null && d.length > 0) {

                        if (!collabInit) {
                            //
                            // Send data to "normal" client.
                            //
                            pop.pn("Sending data to client(" + ID + " " +
                                   hostname + ") (" + d.length + " bytes)");
                            pop.pn("  First: " + d[0] + "; middle: " +
                                   d[d.length/2] + "; last: " + d[d.length-1]);
                            clientSock.sendData(d);
                            pop.pn("  Done sending data");
                        }

                        if (! collabClients.isEmpty()) { // also send to collab JS
                            if (collabInit) {
                                //
                                // Send command only to the most-recently-
                                // connected collaboration client.
                                //
                                DEViseClient client =
                                    (DEViseClient)collabClients.lastElement();
                                pop.pn("Sending data to collabration client ("
                                       + d.length + " bytes)");
                                pop.pn("  First: " + d[0] + "; middle: " +
                                       d[d.length/2] + "; last: " + d[d.length-1]);
                                client.clientSock.sendData(d);
                                pop.pn("Done sending data");
                            } else {
                                //
                                // Send data to all collaboration clients.
                                //
                                sendDataToCollaborators(d);
                            }
                        }
                    }
                }
            } finally {
                // Note: having collabInit be true seems to be a pretty
                // "dangerous" state, so I want to be *sure* the flag
                // always gets cleared.  RKW 2001-04-06.
                if (collabInit) {
                    collabInit = false;
                }
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    // Close this client.  Note that this method should not be called
    // on active clients.
    public synchronized void close()
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseClient(" + ID + ").close()");
        }

        if (status == CLOSE) {
            return;
        } else if (status == SERVE) {
            //
            // Note: this doesn't seem to be a catastrophic error, but it
            // doesn't shut things down cleanly in the DEViseServer object.
            // RKW 2002-04-11.
            //
            System.err.println(
                "Error: calling DEViseClient.close() on active client " + ID);
        }

        status = CLOSE;

        pop.pn("Close connection to client(" + hostname + ")");

        //
        // Delete our temporary session file.  Note: this should only
        // happen here on an "abnormal" exit.  If the client exits
        // cleanly, the session file should be deleted in the processing
        // of the JAVAC_Exit command.
        //
        if (tmpSessionExists) {
            try {
                pop.sendStatelessCmd(DEViseCommands.DELETE_TMP_SESSION +
                                     " " + savedSessionName);
            } catch (YException ex) {
                // Note: this error isn't too bad a problem, since the
                // temporary session file will get deleted the next time
                // the JSPoP is restarted.  RKW 2002-04-17.
                System.err.println("Error deleting temporary session file: " +
                                   ex.getMessage());
            }
        }

        lastActiveTime = -1;
        lastSuspendTime = -1;
        sessionName = null;
        savedSessionName = null;
        screenDimX = -1;
        screenDimY = -1;
        isSessionOpened = false;
        isClientSwitched = false;
        isSwitchSuccessful = false;

        cmdBuffer = new Vector();

        if (user != null) {
            user = null;
        }

        closeSocket();

        if (logFile != null) {
            logFile.close();
            logFile = null;
        }
    }

    public boolean checkPass(String requestPass)
    {
        if (requestPass.equals(collabPass))
            return true;
        else
            return false;
    }

    private void connect(String command)
    {
        try {
            String[] args = DEViseGlobals.parseString(command);
            if (args != null && args.length == 5) {

                // Make sure the client's protocol version is compatible
                // with the JSPoP's.
                String clientMajPVer = DEViseGlobals.getMajorVersion(args[3]);
                String popMajPVer = DEViseGlobals.getMajorVersion(
                                        DEViseGlobals.PROTOCOL_VERSION);
                if (!clientMajPVer.equals(popMajPVer)) {
                    sendCmd(DEViseCommands.ERROR +
                            " {Expected protocol version " +
                            DEViseGlobals.PROTOCOL_VERSION +
                            "; JavaScreen client has version " + args[3] + "}");
                    throw new YException("Protocol version incompatibility");
                }

                // Now make sure that we have a legal username and password.
                user = pop.getUser(args[1], args[2]);
                if (user != null) {
                    sendCmd(DEViseCommands.USER + " " + ID);
                    sendCmd(DEViseCommands.DONE);
                } else {
                    sendCmd(DEViseCommands.ERROR + " {Invalid login}");
                    throw new YException(
                        "Client send invalid login information");
                }

                if (!args[4].equals("")) {
                    hostname = args[4];
                }
            } else {
                sendCmd(DEViseCommands.ERROR + " {Invalid connection request -- bad argument count}");
                throw new YException(
                    "Invalid connection request received from client -- bad argument count (" +
                    args.length + ") in " + args[0]);
            }
        } catch (YException ex) {
            System.err.println("YException in DEViseClient.connect(): " +
                               ex.getMessage());
        }
    }

    private void askCollabLeader(String command)
    {
        String cmd = null;
        if (useCgi()) {
            cmd = DEViseCommands.ERROR + " {cannot collaborate in CGI mode}";
        } else {
            cmd = pop.getCollabLeaders();
            pop.pn("Sending client list to collaboration JS: " + cmd);
        }

        try {
            sendCmd(cmd);
            sendCmd(DEViseCommands.DONE);
        } catch (YException e) {
            System.err.println("YException " + e.getMessage() +
                               " in DEViseClient.askCollabLeader()");
        }

        cmdBuffer.removeAllElements();
    }

    private void collaborate(String command)
    {
        if (useCgi()) {
            String cmd = DEViseCommands.ERROR +
                         " {cannot collaborate in CGI mode}";
            try {
                sendCmd(cmd);
            } catch (YException ex) {
                System.err.println("YException " + ex.getMessage() +
                                   " in DEViseClient.collaborate()");
            }
        } else {
            pop.setUpCollab(this, command, hostname);
        }
        cmdBuffer.removeAllElements();
    }

    private void getCollabList()
    {
        String cmd = DEViseCommands.COLLAB_STATE;
        for (int index = 0; index < collabClients.size(); index++) {
            DEViseClient client = (DEViseClient)collabClients.elementAt(index);
            cmd += " {" + client.hostname + " (" + client.ID + ")}";
        }

        cmd = cmd.trim();
        try {
            sendCmd(new String[] {cmd, DEViseCommands.DONE});
        } catch (YException e) {
            System.err.println("YException " + e.getMessage() +
                               " in DEViseClient.getCollabList()");
        }
        cmdBuffer.removeAllElements();
    }

    private void setCollabPassword(String command)
    {
        if (useCgi()) {
            String cmd = DEViseCommands.ERROR +
                         " {cannot be a collaboration leader in CGI mode}";
            try {
                sendCmd(cmd);
            } catch (YException ex) {
                System.err.println("YException " + ex.getMessage() +
                                   " in DEViseClient.collaborate()");
            }
        } else {
            String[] cmds = DEViseGlobals.parseString(command);

            try {
                if (pop.collabNames.contains(cmds[1])) {
                    // can not have more than one collab leaders
                    // with the same collab name
                    sendCmd(DEViseCommands.RESET_COLLAB_NAME);
                } else {
                    isAbleCollab = true;
                    collabName = cmds[1];
                    collabPass = cmds[2];
                    pop.collabNames.addElement(collabName);
                }
                sendCmd(DEViseCommands.DONE);
            } catch (YException e) {
                System.err.println("YException " + e.getMessage() +
                                   " in DEViseClient.setCollabPassword()");
            }
        }

        cmdBuffer.removeAllElements();
    }

    private void sendCmdToCollaborators(String cmd)
    {
        for (int index = 0; index < collabClients.size(); index++) {
            DEViseClient client = (DEViseClient)collabClients.elementAt(index);
            pop.pn("Sending command to collabration client " + index);
            try {
                client.sendCmd(cmd);
            } catch (YException ex) {
                System.err.println("Error " + ex +
                                   " sending command to follower");
            }
        }
    }

    private void sendDataToCollaborators(byte[] data)
    {
        for (int index = 0; index < collabClients.size(); index++) {
            DEViseClient client = (DEViseClient)collabClients.elementAt(index);
            pop.pn("Sending data to collabration client" + " " + index +
                   " (" + data.length + " bytes)");
            pop.pn("  First: " + data[0] + "; middle: " +
                   data[data.length/2] + "; last: " + data[data.length-1]);
            try {
                client.clientSock.sendData(data);
            } catch (YException ex) {
                System.err.println("Error " + ex +
                                   " sending data to follower");
            }

            pop.pn("Done sending data");
        }
    }
}
