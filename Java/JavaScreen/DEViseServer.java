// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
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

// $Id$

// $Log$
// Revision 1.39  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.38  2000/06/05 16:35:07  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.37  2000/04/27 20:15:25  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.36  2000/04/07 22:44:10  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.35  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.34  2000/03/08 21:23:25  wenger
// Fixed bug 568 (crash when opening some sessions in the JavaScreen):
// reset devised timeout in jspop to a better value; JavaScreenCmd now
// deals better with the possibility of a communications failure.
//
// Revision 1.33  2000/03/06 08:09:19  hongyu
// fix the 'invalid image data' bug --- hongyu
//
// Revision 1.32  2000/02/22 10:00:48  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/02/18 22:21:15  wenger
// Various changes to make cron scripts work better with new two-machine
// setup: added -id argument to devise, jspop, jss; updated cron scripts
// that check status of jspop, etc.; improved usage messages of jspop,
// jss, js; improved DEVise.kill script; removed obsolete sections of
// Java code.
//
// Revision 1.30  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.28  2000/02/14 09:26:24  hongyu
// *** empty log message ***
//
// Revision 1.27  2000/01/19 20:41:03  hongyu
// *** empty log message ***
//
// Revision 1.26  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.25  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.24  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.23  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseServer.java

import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable
{
    private static int Port = DEViseGlobals.cmdport + 1;

    private jspop pop = null;

    private Process proc = null;
    private Thread serverThread = null;

    public String hostname = "localhost"; // hostname of jss/devised
    public int dataPort = 0, cmdPort = 0, switchPort = 0;

    public int jssport = 0; // port the jss is listening on

    private DEViseCommSocket socket = null;
    private int devisedTimeout = 180 * 1000;
    private int socketTimeout = 1000;

    // newClient is the client we're going to switch to the next time we
    // switch clients; client is the client we're currently connected to.
    private DEViseClient newClient = null;
    private DEViseClient client = null;

    private String[] serverCmds = null;

    private Vector currentDir = new Vector();
    private String rootDir = "DEViseSession";

    public static final int STATUS_STOPPED = 0, STATUS_RUNNING = 1;
    private int status = 0;
    private boolean isValid = false;

    public static final int ACTION_QUIT = 0, ACTION_IDLE = 1, ACTION_WORK = 2;
    private int action = ACTION_IDLE;

    // name is the name of the system that the jss and devised(s) are running
    // on; port is the jssport.
    public DEViseServer(jspop j, String name, int port, int cmdport,
      int imgport)
    {
        pop = j;
        if (name != null) {
            hostname = new String(name);
        }

        cmdPort = cmdport;
        dataPort = imgport;
        jssport = port;
        isValid = true;
    }

    // Returns STATUS_*.
    public synchronized int getStatus()
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

            stopDEVise();
        }
    }

    public void start() throws YException
    {
        pop.pn("Trying to start a DEViseServer ...");

        // start devised and connect to it
        if (!startSocket())
            throw new YException("Can not start DEViseServer");

        // start server Thread but first check if server thread is still active
        if (getStatus() == STATUS_STOPPED) {
            setStatus(STATUS_RUNNING);
            setAction(ACTION_IDLE);
            serverThread = new Thread(this);
            serverThread.start();
        }

        pop.pn("Successfully started a DEViseServer ...");
    }

    private synchronized void closeSocket()
    {
        if (socket != null) {
            socket.closeSocket();
            socket = null;
        }
    }

    private synchronized boolean startSocket()
    {
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
                socket = new DEViseCommSocket(hostname, cmdPort, dataPort,
				  socketTimeout);

                pop.pn("Successfully connect to devised ...");

                return true;
            } catch (YException e) {
                pop.pn("Can not connect to devised, wait to try again ...");
                closeSocket();
            }
        }

        // can not connect within timeout
        closeSocket();
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

    public synchronized boolean isAvailable()
    {
        if (newClient == null) {
            return true;
        } else {
            return false;
        }
    }

    public synchronized DEViseClient getCurrentClient()
    {
        return client;
    }

    // Set the client to switch to the next time we switch clients.
    public synchronized void setCurrentClient(DEViseClient c)
    {
        if (c == null)
            return;

        c.setStatus(DEViseClient.SERVE);
        newClient = c;
        notifyAll();
    }

    // Returns ACTION_*.
    private synchronized int getAction()
    {
        while (action == ACTION_IDLE && newClient == null) {
            try {
                wait();
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
        int todo;
        boolean isRemoveClient = false;

        while (true) {
            todo = getAction();

            if (todo == ACTION_QUIT) {
                quit();

                break;
            } else if (todo == ACTION_WORK && client != null) {
                // while DEViseServer thread reach here, there is no way to
		// interrupt it until some error happened or it finish
		// processing one client's request

                serverCmds = null;

                String clientCmd = null;
                Vector serverDatas = new Vector();

                boolean isEnd = false;
                while (!isEnd) {
                    try {
			// Get a command from the client.
                        // This method will not block, if no command it just
			// returns null.
                        clientCmd = client.getCmd();
                        isEnd = true;
                    } catch (InterruptedIOException e) {
                        // since client.getCmd() will not block, this is
			// meaningless
                    } catch (YException e) {
                        pop.pn("Client communication error");
                        pop.pn(e.getMsg());
                        removeCurrentClient();

                        isEnd = true;
                        clientCmd = null;
                    }
                }

                if (clientCmd == null) {
                    continue;
                } else {
		    // We've just finished processing this command, so
		    // remove it from the client.
                    client.removeLastCmd();
                }

		//
		// Process the command.
                // commands JAVAC_GetServerState, JAVAC_Abort & JAVAC_Connect
                // already been handled in DEViseClient
		//
                try {
		    if (!processClientCmd(clientCmd, isRemoveClient,
		      serverDatas)) {
		        continue;
                    }
		    processServerCmd(serverDatas);
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
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
                            pop.pn("Client communication error");
                            pop.pn(e1.getMsg());
                            removeCurrentClient(false);
                        }
                    }

                    setAction(ACTION_QUIT);

                    continue;
                }

		//
		// Send response from server back to the client.
		//
                try {
                    client.sendCmd(serverCmds);
                    client.sendData(serverDatas);
                    if (isRemoveClient) {
                        isRemoveClient = false;
                        removeCurrentClient(false);
                    }
                } catch (YException e) {
                    pop.pn("Client communication error");
                    pop.pn(e.getMsg());
                    removeCurrentClient();
                }
            }
        }
    }


// ------------------------------------------------------------------------
// Method to process client commands.

    // Returns true if command needs the rest of the "standard" processing,
    // false otherwise.
    private boolean processClientCmd(String clientCmd, boolean isRemoveClient,
      Vector serverDatas) throws YException
    {
        if (clientCmd.startsWith(DEViseCommands.PROTOCOL_VERSION)) {
	    cmdProtocolVersion(clientCmd, isRemoveClient);

        } else if (clientCmd.startsWith(DEViseCommands.EXIT)) {
	    cmdExit();

            // no need to return any response to client
            return false;

        } else if (clientCmd.startsWith(DEViseCommands.CLOSE_SESSION)) {
	    cmdCloseSession();

            // no need to return any response to client
            return false;

        } else if (clientCmd.startsWith(DEViseCommands.GET_SESSION_LIST)) {
	    cmdGetSessionList(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
	    cmdSetDisplaySize(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.OPEN_SESSION)) {
	    cmdOpenSession(clientCmd);

        } else {
	    cmdClientDefault(clientCmd);

        }

	return true;
    }

// ------------------------------------------------------------------------
// Method to process server commands.

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

    private void cmdProtocolVersion(String clientCmd, boolean isRemoveClient)
      throws YException
    {
        if (sendCmd(clientCmd)) {
            if (client.user.addClient(client)) {
                serverCmds = new String[2];
                serverCmds[0] = DEViseCommands.USER + " " +
                client.ID.intValue();
                serverCmds[1] = DEViseCommands.DONE;
            } else {
                serverCmds = new String[1];
                serverCmds[0] = DEViseCommands.ERROR +
                  " {Maximum logins for this user has been reached}";
                //throw new YException(
                  //"No more login is allowed for user \"" +
                  //client.user.getName() + "\"");
                isRemoveClient = true;
            }
        } else {
            isRemoveClient = true;
        }
    }

    private void cmdExit() throws YException
    {
	cmdCloseSession();

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
        if (cmds != null && cmds.length == 3) {
            try {
                client.screenDimX = Integer.parseInt(cmds[1]);
                client.screenDimY = Integer.parseInt(cmds[2]);
                if (client.screenDimX < 1 || client.screenDimY < 1) {
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
        } else {
            sendCmd(clientCmd);
        }
    }

    private void cmdOpenSession(String clientCmd) throws YException
    {
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
            if (client.screenDimX > 0 && client.screenDimY > 0) {
                if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
		  client.screenDimX + " " + client.screenDimY)) {
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

// ------------------------------------------------------------------------
// Method to process all client commands not using the command-specific
// methods.

    private void cmdClientDefault(String clientCmd) throws YException
    {
        if (client.isClientSwitched) {
            client.isClientSwitched = false;

            if (client.isSwitchSuccessful) {
                client.isSwitchSuccessful = false;

                boolean error = false;
                if (client.screenDimX > 0 && client.screenDimY > 0) {
                    if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
		      client.screenDimX + " " + client.screenDimY)) {
                        error = true;
                    //} else {
                    //    pop.pn("Switch error: can not send " +
		    //      DEViseCommands.SET_DISPLAY_SIZE);
                    }
                }

                if (!error) {
                    if (sendCmd(DEViseCommands.OPEN_SESSION + " {" +
		      client.savedSessionName + "}")) {
                        client.isSessionOpened = true;
                    } else {
                        pop.pn("Switch error: Can not send " +
			  DEViseCommands.OPEN_SESSION + " " +
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

/* Not used.  RKW 2000-06-12.
    private Vector findPath(String p)
    {
        Vector ppp = new Vector();
        if (p != null && p.length() > 0) {
            String[] pp = DEViseGlobals.parseStr(p, "/");
            if (pp != null && pp.length > 0) {
                if (pp[0].equals((String)currentDir.elementAt(0))) {
                    int i = 1;
                    for (i = 1; i < pp.length; i++) {
                        if (i < currentDir.size()) {
                            if (!pp[i].equals((String)currentDir.elementAt(i))) {
                                for (int j = i; j < currentDir.size(); j++) {
                                    ppp.addElement("..");
                                }

                                break;
                            }
                        } else {
                            break;
                        }
                    }

                    if (i < pp.length || i >= currentDir.size()) {
                        for (int k = i; k < pp.length; k++) {
                            ppp.addElement(pp[k]);
                        }
                    } else {
                        if (i < currentDir.size()) {
                            for (int k = i; k < currentDir.size(); k++) {
                                ppp.addElement("..");
                            }
                        }
                    }

                    return ppp;
                } else {
                    return null;
                }
            } else {
                return null;
            }
        } else {
            return null;
        }
    }
*/

    private synchronized void quit()
    {
        closeSocket();

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
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    //if (!startSocket()) {
                        setAction(ACTION_QUIT);
                    //}
                }
            }

            client.close();
            pop.suspendClient(client);
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
        if (client != null) {
            if (client.isSessionOpened) {
                client.isClientSwitched = true;
                client.isSwitchSuccessful = false;
                client.isSessionOpened = false;

                try {
                    if (sendCmd(DEViseCommands.SAVE_SESSION + " {" +
		      client.savedSessionName + "}")) {
                        client.isSwitchSuccessful = true;
                    } else {
                        pop.pn("Can not save session for old client while switching client!");
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
            client = null;
        }

        if (startNewClient) {
            client = newClient;
            newClient = null;
            if (client != null) {
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
        while (!isEnd) {
            try {
                data = socket.receiveData(size);
                pop.pn("Successfully received data from devised(" + hostname + "at " + cmdPort + ") of size " + size);
                isEnd = true;
            } catch (InterruptedIOException e) {
                time += socketTimeout;
                if (time > devisedTimeout) {
                    throw new YException("Can not receive response from devised within timeout");
                }
            }
        }

        return data;
    }

    private boolean sendCmd(String clientCmd) throws YException
    {
        if (clientCmd == null)
            return true;

        if (socket == null)
            throw new YException("Invalid communication socket");

        String response = null;
        boolean isEnd = false, isFinish = false, isError = false;
        long time;

        Vector rspbuf = new Vector();

        pop.pn("Sending command to devised(" + hostname + " at " + cmdPort + ") :  \"" + clientCmd + "\"");
        socket.sendCmd(clientCmd);

        isEnd = false;
        while (!isEnd) {
            isFinish = false;
            time = 0;

            while (!isFinish) {
                try {
                    response = socket.receiveCmd();
                    pop.pn("Receive response from devised(" + hostname + " at " + cmdPort + ") :  \"" + response + "\"");
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    time += socketTimeout;
                    if (time > devisedTimeout) {
                        throw new YException("Can not receive response from devised within timeout");
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Invalid response received from devised");
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated response \"" + response + "\" received from devised");
                } else {
                    //pop.pn("Receive response \"" + response + "\" from devised");

                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith(DEViseCommands.DONE)) {
                        isEnd = true;
                        isError = false;
                    } else if (cmd.startsWith(DEViseCommands.ERROR) || cmd.startsWith(DEViseCommands.FAIL)) {
                        isEnd = true;
                        isError = true;
                        rspbuf.removeAllElements();
                    }

                    rspbuf.addElement(cmd);
                }
            }
        }

        serverCmds = new String[rspbuf.size()];
        for (int i = 0; i < rspbuf.size(); i++)
            serverCmds[i] = (String)rspbuf.elementAt(i);

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

}
