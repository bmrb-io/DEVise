// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$
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
    private static String DEViseExec = new String("DEVise.jspop");
    private static String DEViseKill = new String("DEVise.kill");
    private static int Port = DEViseGlobals.cmdport + 1;
    public static int QUIT = 0, IDLE = 1, WORK = 2;

    private jspop pop = null;

    private Process proc = null;
    private Thread serverThread = null;

    public String hostname = "localhost";
    private int dataPort = 0, cmdPort = 0, switchPort = 0;
    private DEViseCommSocket socket = null;
    private int devisedTimeout = 180 * 1000;
    private int socketTimeout = 1000;

    private DEViseClient newClient = null;
    private DEViseClient client = null;
    private String[] serverCmds = null;

    private Vector currentDir = new Vector();
    private String rootDir = "DEViseSession";

    private int status = 0;

    private int action = DEViseServer.IDLE;

    public DEViseServer(jspop j, String name)
    {
        pop = j;
        if (name != null) {
            hostname = new String(name);
        }
    }

    public synchronized static int getPort()
    {
        if (Port == DEViseGlobals.cmdport || Port == DEViseGlobals.imgport)
            Port++;

        if (Port > 60000)
            Port = DEViseGlobals.cmdport + 1;

        while (true) {
            try {
                ServerSocket s = new ServerSocket(Port);
                s.close();
                return Port++;
            } catch (IOException e) {
                Port++;

                if (Port == DEViseGlobals.cmdport || Port == DEViseGlobals.imgport)
                    Port++;

                if (Port > 60000)
                    Port = DEViseGlobals.cmdport + 1;
            }
        }
    }

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
        if (getStatus() != 0) {
            if (serverThread != null) {
                serverThread.stop();
                serverThread = null;
            }

            setStatus(0);

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
        if (getStatus() == 0) {
            setStatus(1);
            setAction(DEViseServer.IDLE);
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

        int time = 0, timestep = 5000;

        while (time < devisedTimeout) {
            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            time += timestep;

            try {
                socket = new DEViseCommSocket(hostname, cmdPort, dataPort, 1000);

                pop.pn("Successfully connect to devised ...");

                return true;
            } catch (YException e) {
                closeSocket();
            }
        }

        // can not connect within timeout
        closeSocket();
        stopDEVise();

        pop.pn("Can not connect to devised within timeout");

        return false;
    }

    private boolean isDEViseAlive()
    {
        if (proc == null)
            return false;

        try {
            int v = proc.exitValue();
            proc = null;
            return false;
        } catch (IllegalThreadStateException e) {
            return true;
        }
    }

    private void stopDEVise()
    {
        if (isDEViseAlive()) {
            proc.destroy();
            proc = null;
        }

        Runtime currentRT = Runtime.getRuntime();
        try {
            proc = currentRT.exec(DEViseServer.DEViseKill + " " + cmdPort);
        } catch (IOException e) {
            pop.pn("IO Error while trying to kill an old devised!");
        } catch (SecurityException e) {
            pop.pn("Security Error while trying to kill an old devised!");
        }
    }

    private boolean startDEVise()
    {
        // stop previous devised first
        stopDEVise();

        // need more work to make it stable
        cmdPort = getPort();
        dataPort = getPort();
        switchPort = getPort();

        Runtime currentRT = Runtime.getRuntime();
        try {
            proc = currentRT.exec(DEViseServer.DEViseExec + " -port " + cmdPort + " -imageport " + dataPort + " -switchport " + switchPort);
        } catch (IOException e) {
            pop.pn("IO Error while trying to start a new devised");
            return false;
        } catch (SecurityException e) {
            pop.pn("Security Error while trying to start a new devised");
            return false;
        }

        pop.pn("Successfully start devised ...");

        return true;
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

    public synchronized void setCurrentClient(DEViseClient c)
    {
        if (c == null)
            return;

        c.setStatus(DEViseClient.SERVE);
        newClient = c;
        notifyAll();
    }

    private synchronized int getAction()
    {
        while (action == DEViseServer.IDLE && newClient == null) {
            try {
                wait();
            } catch (InterruptedException e) {
            }
        }

        if (action == DEViseServer.QUIT) {
            return action;
        }

        if (client == null && newClient == null) {
            action = DEViseServer.IDLE;
        } else {
            if (newClient != null) { // need to switch client
                pop.pn("Client switching happened in server running on (" + hostname + ")");
                switchClient(true);
                action = DEViseServer.WORK;
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

        while (true) {
            todo = getAction();

            if (todo == DEViseServer.QUIT) {
                quit();

                break;
            } else if (todo == DEViseServer.WORK && client != null) {
                // while DEViseServer thread reach here, there is no way to interrupt it until
                // some error happened or it finish processing one client's request

                serverCmds = null;

                String clientCmd = null;
                Vector serverDatas = new Vector();

                boolean isEnd = false;
                while (!isEnd) {
                    try {
                        // this method will not block, if no command it just return null
                        clientCmd = client.getCmd();
                        isEnd = true;
                    } catch (InterruptedIOException e) {
                        // since client.getCmd() will not block, so this is meaningless
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
                    client.removeLastCmd();
                }

                // commands JAVAC_GetServerState, JAVAC_Abort and JAVAC_Connect
                // already been handled in DEViseClient
                try {
                    if (clientCmd.startsWith("JAVAC_Exit")) {
                        client.isClientSwitched = false;
                        client.isSwitchSuccessful = false;
                        if (client.isSessionOpened) {
                            client.isSessionOpened = false;
                            sendCmd("JAVAC_CloseCurrentSession");
                            currentDir = new Vector();
                            currentDir.addElement(rootDir);
                        }

                        removeCurrentClient(false);

                        // no need to return any response to client
                        continue;
                    } else if (clientCmd.startsWith("JAVAC_CloseCurrentSession")) {
                        client.isClientSwitched = false;
                        client.isSwitchSuccessful = false;
                        if (client.isSessionOpened) {
                            client.isSessionOpened = false;
                            sendCmd("JAVAC_CloseCurrentSession");
                            currentDir = new Vector();
                            currentDir.addElement(rootDir);
                        }

                        // no need to return any response to client
                        continue;
                    } else if (clientCmd.startsWith("JAVAC_GetSessionList")) {
                        String[] cmds = DEViseGlobals.parseString(clientCmd);
                        if (cmds != null && cmds.length == 2 && cmds[1].startsWith(rootDir)) {
                            Vector path = findPath(cmds[1]);
                            if (path != null) {
                                if (path.size() > 0) {
                                    for (int i = 0; i < path.size(); i++) {
                                        String p = (String)path.elementAt(i);
                                        sendCmd("JAVAC_GetSessionList {" + p + "}");
                                        if (p.equals("..")) {
                                            currentDir.removeElementAt(currentDir.size() - 1);
                                        } else {
                                            currentDir.addElement(p);
                                        }
                                    }
                                } else {
                                    sendCmd("JAVAC_GetSessionList");
                                }

                            } else {
                                serverCmds = new String[1];
                                serverCmds[0] = "JAVAC_Error {Invalid command: \"" + clientCmd + "\"}";
                            }
                        } else {
                            serverCmds = new String[1];
                            serverCmds[0] = "JAVAC_Error {Invalid command: \"" + clientCmd + "\"}";
                        }
                    } else if (clientCmd.startsWith("JAVAC_SetDisplaySize")) {
                        boolean error = false;
                        String[] cmds = DEViseGlobals.parseString(clientCmd);
                        if (cmds != null && cmds.length == 3) {
                            try {
                                client.screenDimX = Integer.parseInt(cmds[1]);
                                client.screenDimY = Integer.parseInt(cmds[2]);
                                if (client.screenDimX < 1 || client.screenDimY < 1)
                                    throw new NumberFormatException();
                            } catch (NumberFormatException e) {
                                error = true;
                            }
                        } else {
                            error = true;
                        }

                        if (error) {
                            serverCmds = new String[1];
                            serverCmds[0] = "JAVAC_Error {Invalid command: \"" + clientCmd + "\"}";
                            client.screenDimX = -1;
                            client.screenDimY = -1;
                        } else {
                            sendCmd(clientCmd);
                        }
                    } else if (clientCmd.startsWith("JAVAC_OpenSession")) {
                        if (client.isSessionOpened) {
                            client.isSessionOpened = false;
                            sendCmd("JAVAC_CloseCurrentSession");
                            currentDir = new Vector();
                            currentDir.addElement(rootDir);
                        }

                        String[] cmds = DEViseGlobals.parseString(clientCmd);
                        if (cmds != null && cmds.length == 2 && cmds[1].startsWith(rootDir)) {
                            Vector path = findPath(cmds[1]);
                            if (path != null && path.size() > 0) {
                                for (int i = 0; i < path.size() - 1; i++) {
                                    String p = (String)path.elementAt(i);
                                    sendCmd("JAVAC_GetSessionList {" + p + "}");
                                    if (p.equals("..")) {
                                        currentDir.removeElementAt(currentDir.size() - 1);
                                    } else {
                                        currentDir.addElement(p);
                                    }
                                }
                                /*
                                client.path = (String)currentDir.elementAt(0);
                                for (int i = 1; i < currentDir.size(); i++) {
                                    client.path = client.path + "/" + (String)currentDir.elementAt(i);
                                }
                                */
                                client.sessionName = (String)path.lastElement();

                                boolean error = false;
                                if (client.screenDimX > 0 && client.screenDimY > 0) {
                                    if (!sendCmd("JAVAC_SetDisplaySize " + client.screenDimX + " " + client.screenDimY)) {
                                        error = true;
                                    }
                                }

                                if (!error) {
                                    if (sendCmd("JAVAC_OpenSession {" + client.sessionName + "}")) {
                                        client.isSessionOpened = true;
                                    } else {
                                        // need to clear socket because there might be some data on data socket
                                        socket.clearSocket();
                                    }
                                }
                            }
                        }

                        if (!client.isSessionOpened) {
                            serverCmds = new String[1];
                            serverCmds[0] = "JAVAC_Error {Can not open session \"" + client.sessionName + "\"}";
                        }
                    } else {
                        if (client.isClientSwitched) {
                            client.isClientSwitched = false;

                            if (client.isSwitchSuccessful) {
                                client.isSwitchSuccessful = false;

                                Vector path = findPath(client.path);
                                if (path != null) {
                                    for (int i = 0; i < path.size(); i++) {
                                        String p = (String)path.elementAt(i);
                                        sendCmd("JAVAC_GetSessionList {" + p + "}");
                                        if (p.equals("..")) {
                                            currentDir.removeElementAt(currentDir.size() - 1);
                                        } else {
                                            currentDir.addElement(p);
                                        }
                                    }

                                    boolean error = false;
                                    if (client.screenDimX > 0 && client.screenDimY > 0) {
                                        if (!sendCmd("JAVAC_SetDisplaySize " + client.screenDimX + " " + client.screenDimY)) {
                                            error = true;
                                        } else {
                                            pop.pn("Switch error: can not send JAVAC_SetDisplaySize");
                                        }
                                    }

                                    if (!error) {
                                        if (sendCmd("JAVAC_OpenSession {" + client.savedSessionName + "}")) {
                                            client.isSessionOpened = true;
                                        } else {
                                            pop.pn("Switch error: Can not send JAVAC_OpenSession " + client.savedSessionName);
                                        }

                                        // need to clear socket because there might be some useless data on data socket
                                        socket.clearSocket();
                                    }
                                } else {
                                    pop.pn("Switch error: path is null");
                                }
                            } else {
                                pop.pn("Switch error: client switch not successful");
                            }
                        }

                        if (client.isSessionOpened) {
                            if (!sendCmd(clientCmd)) {
                                // need to clear data socket when error happened because there might be some data on data socket
                                socket.clearSocket();
                            }
                        } else {
                            serverCmds = new String[1];
                            serverCmds[0] = "JAVAC_Fail {Do not have an opened session or Can not open last saved session}";
                        }
                    }

                    // need to get whatever data is sending over by devised along with the commands
                    // since serverCmds always ends with a JAVAC_Error or JAVAC_Fail or JAVAC_Done, so we do not need to check the last command
                    if (!serverCmds[serverCmds.length - 1].startsWith("JAVAC_Done")) {
                        String tmpcmd = serverCmds[serverCmds.length - 1];
                        serverCmds = new String[1];
                        serverCmds[0] = tmpcmd;
                    }

                    for (int i = 0; i < (serverCmds.length - 1); i++) {
                        if (serverCmds[i].startsWith("JAVAC_UpdateViewImage")) {
                            String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                            if (cmds != null && cmds.length == 3) {
                                try {
                                    int imgSize = Integer.parseInt(cmds[2]);
                                    if (imgSize < 1) {
                                        throw new NumberFormatException();
                                    }

                                    byte[] image = receiveData(imgSize);

                                    serverDatas.addElement(image);
                                } catch (NumberFormatException e1) {
                                    throw new YException("Invalid image size in command \"" + serverCmds[i] + "\"");
                                }
                            } else {
                                throw new YException("Ill-formated command \"" + serverCmds[i] + "\"");
                            }
                        } else if (serverCmds[i].startsWith("JAVAC_UpdateGData")) {
                            String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                            if (cmds != null && cmds.length == 7) {
                                try {
                                    int dataSize = Integer.parseInt(cmds[6]);
                                    if (dataSize < 1) {
                                        throw new NumberFormatException();
                                    }

                                    byte[] data = receiveData(dataSize);

                                    serverDatas.addElement(data);
                                } catch (NumberFormatException e1) {
                                    throw new YException("Invalid GData size in command \"" + serverCmds[i] + "\"");
                                }
                            } else {
                                throw new YException("Ill-formated command \"" + serverCmds[i] + "\"");
                            }
                        }
                    }
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    if (clientCmd.startsWith("JAVAC_Exit")) {
                        removeCurrentClient(false);
                    } else if (clientCmd.startsWith("JAVAC_CloseCurrentSession")) {
                        switchClient();
                    } else {
                        try {
                            client.sendCmd("JAVAC_Error {Communication error occurs while talk to devised}");
                            switchClient();
                        } catch (YException e1) {
                            pop.pn("Client communication error");
                            pop.pn(e1.getMsg());
                            removeCurrentClient(false);
                        }
                    }

                    if (!startSocket()) {
                        setAction(DEViseServer.QUIT);
                    }

                    continue;
                }

                try {
                    client.sendCmd(serverCmds);
                    client.sendData(serverDatas);
                } catch (YException e) {
                    pop.pn("Client communication error");
                    pop.pn(e.getMsg());
                    removeCurrentClient();
                }
            }
        }
    }

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

    private synchronized void quit()
    {
        closeSocket();

        stopDEVise();

        setStatus(0);
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
                    sendCmd("JAVAC_CloseCurrentSession");
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    if (!startSocket()) {
                        setAction(DEViseServer.QUIT);
                    }
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

    private synchronized void switchClient(boolean flag)
    {
        if (client != null) {
            if (client.isSessionOpened) {
                client.isClientSwitched = true;
                client.isSwitchSuccessful = false;
                client.isSessionOpened = false;

                try {
                    if (sendCmd("JAVAC_SaveSession {" + client.savedSessionName + "}")) {
                        client.isSwitchSuccessful = true;
                    } else {
                        pop.pn("Can not save session for old client while switching client!");
                    }

                    if (!sendCmd("JAVAC_CloseCurrentSession")) {
                        pop.pn("Can not close current session for old client while switching client!");
                    }
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    if (!startSocket()) {
                        setAction(DEViseServer.QUIT);
                    }
                }
            }

            pop.suspendClient(client);
            client = null;
        }

        if (flag) {
            client = newClient;
            newClient = null;
            if (client != null) {
                pop.activeClient(client);
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
                pop.pn("Successfully received data from devised(" + hostname + ") of size " + size);
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

        pop.pn("Sending command to devised(" + hostname + ") :  \"" + clientCmd + "\"");
        socket.sendCmd(clientCmd);

        isEnd = false;
        while (!isEnd) {
            isFinish = false;
            time = 0;

            while (!isFinish) {
                try {
                    response = socket.receiveCmd();
                    pop.pn("Receive response from devised(" + hostname + ") :  \"" + response + "\"");
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
                    pop.pn("Receive response \"" + response + "\" from devised");

                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith("JAVAC_Done")) {
                        isEnd = true;
                        isError = false;
                    } else if (cmd.startsWith("JAVAC_Error") || cmd.startsWith("JAVAC_Fail")) {
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
}
