import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable
{
    private static int Port = 3000;
    private static String DEViseExec = new String("DEVise.jspop");

    private jspop pop = null;
    private int serverStartTimeout = 10 * 1000;
    private int serverStartTimestep = 2000;
    private int serverWaitTimeout = 60 * 1000;
    private int cmdSocketTimeout = 200;
    private int imgSocketTimeout = 200;
    // when is current client been serviced
    private long clientStartTime = 0;

    private Process proc = null;
    private String hostname = "localhost";
    private int imgPort = 0, cmdPort = 0, switchPort = 0;
    private DEViseCmdSocket cmdSocket = null;
    private DEViseImgSocket imgSocket = null;

    private Thread serverThread = null;

    private DEViseClient newClient = null;
    private DEViseClient client = null;
    // status = -1, serverThread is stopped
    // status = 0, serverThread wait for new client and idle
    // status = 1, serverThread is working with new client
    private int status = -1;
    // action = -1, stop server thread
    // action = 0, no change of current status
    // action = 1, change client
    private int action = 0;
    // number of times trying to start a new DEVise Server but failed consecutively
    private int tryTime = 0;

    public DEViseServer(jspop j) throws YException
    {
        pop = j;

        if (pop.serverStartTimeout > serverStartTimeout)
            serverStartTimeout = pop.serverStartTimeout;
        if (pop.serverWaitTimeout > serverWaitTimeout)
            serverWaitTimeout = pop.serverWaitTimeout;
        if (pop.cmdSocketTimeout > cmdSocketTimeout)
            cmdSocketTimeout = pop.cmdSocketTimeout;
        if (pop.imgSocketTimeout > imgSocketTimeout)
            imgSocketTimeout = pop.imgSocketTimeout;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int set)
    {
        status = set;
    }

    public synchronized int getAction()
    {
        return action;
    }

    private synchronized void setAction(int set)
    {
        action = set;
    }

    public synchronized DEViseClient getClient()
    {
        return client;
    }

    public synchronized void setClient(DEViseClient c)
    {
        action = 1;
        newClient = c;
        notifyAll();
    }

    private synchronized void waitForClient()
    {
        if (action != 0)
            return;

        while (client == null) {
            status = 0;

            try {
                wait();
            } catch (InterruptedException e) {
            }

            if (action != 0)
                return;
        }
    }

    public synchronized void stopServer()
    {
        action = -1;
        notifyAll();
    }

    public synchronized void startServer() throws YException
    {
        YGlobals.Ydebugpn("Trying to start server ...");

        tryTime++;

        // Reconnect sockets if they are dead
        if (!startSocket())
            throw new YException("Can not connect to DEVise Server!", "DEViseServer:startServer");

        // start server Thread but first check if server thread is still active
        if (status < 0) {
            serverThread = new Thread(this);
            serverThread.start();
        }

        tryTime = 0;

        YGlobals.Ydebugpn("Successfully start server ...");
    }

    private void stopDEVise()
    {
        if (isDEViseAlive()) {
            return;
        } else {
            proc.destroy();
            proc = null;
        }
    }

    private boolean startDEVise()
    {
        if (isDEViseAlive())
            return true;

        closeSocket();

        cmdPort = DEViseServer.getPort();
        imgPort = DEViseServer.getPort();
        switchPort = DEViseServer.getPort();

        YGlobals.Ydebugpn("Successfully found three available port " + cmdPort + " " + imgPort + " " + switchPort + " ...");

        Runtime currentRT = Runtime.getRuntime();
        try {
            proc = currentRT.exec(DEViseServer.DEViseExec + " -port " + cmdPort + " -imageport " + imgPort + " -switchport " + switchPort);
        } catch (IOException e1) {
            YGlobals.Ydebugpn("IO Error while trying to start a new DEVise Server!");
            return false;
        } catch (SecurityException e2) {
            YGlobals.Ydebugpn("Security Error while trying to start a new DEVise Server!");
            return false;
        }

        YGlobals.Ydebugpn("Successfully start DEVise server ...");

        return true;
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

    private synchronized void closeSocket()
    {
        if (cmdSocket != null) {
            cmdSocket.closeSocket();
            cmdSocket = null;
        }

        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }
    }

    private synchronized boolean startSocket()
    {
        if (isSocketAlive()) {
            return true;
        }

        if (!isDEViseAlive()) {
            // start DEVise server if it is dead
            if (!startDEVise()) {
                YGlobals.Ydebugpn("Can not start DEVise Server!");
                return false;
            }
        }

        int time = 0;
        while (time < serverStartTimeout) {
            try {
                Thread.sleep(serverStartTimestep);
            } catch (InterruptedException e) {
            }

            time += serverStartTimestep;

            try {
                cmdSocket = new DEViseCmdSocket(hostname, cmdPort, cmdSocketTimeout);
                imgSocket = new DEViseImgSocket(hostname, imgPort, imgSocketTimeout);

                YGlobals.Ydebugpn("Successfully connect to DEVise server ...");
                return true;
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());
                closeSocket();
            }
        }

        closeSocket();

        YGlobals.Ydebugpn("Can not connect to DEVise server within " + (serverStartTimeout / 1000) + " seconds!");

        return false;
    }

    public synchronized boolean isSocketAlive()
    {
        if (isDEViseAlive()) {
            if (cmdSocket != null && imgSocket != null) {
                return true;
            } else {
                closeSocket();
                return false;
            }
        } else {
            closeSocket();
            return false;
        }
    }

    public synchronized boolean isAvailable()
    {
        if (client != null) {
            return false;
        } else {
            startSocket();
            return true;
        }
    }

    public long getActiveTime()
    {
        long time = YGlobals.getTime();
        return time - clientStartTime;
    }

    private synchronized void quit()
    {
        closeSocket();
        stopDEVise();

        client = null;
        newClient = null;
        tryTime = 0;
        status = -1;
        action =  0;
    }

    private synchronized void removeClient()
    {
        if (client == null)
            return;

        if (client.isSessionOpened) {
            client.isSessionOpened = false;

            String[] cmds = null;
            try {
                cmds = sendCmd("JAVAC_SaveSession {" + client.savedSessionName + "}");
                if (cmds != null && cmds.length == 1) {
                    if (cmds[0].startsWith("JAVAC_Done")) {
                        client.isSessionSaved = true;
                        client.insertCmd("JAVAC_OpenSession {" + client.savedSessionName + "}");
                    } else {
                    }
                } else { // something wrong with the server
                    YGlobals.Ydebugpn("Something wrong with the server while saving session!");
                    closeSocket();
                    startSocket();
                    return;
                }

                cmds = sendCmd("JAVAC_CloseCurrentSession");
                if (cmds != null && cmds.length == 1) {
                    if (cmds[0].startsWith("JAVAC_Done")) {
                    } else {
                    }
                } else { // something wrong with the server
                    YGlobals.Ydebugpn("Something wrong with the server while closing session!");
                    closeSocket();
                    startSocket();
                }
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());
                closeSocket();
                startSocket();
            }
        }
    }

    public void run()
    {
        setStatus(0);
        setAction(0);

        boolean isRunning = true;
        int todo;

        YGlobals.Ydebugpn("Server thread is started ...");

        while (isRunning) {
            waitForClient();
            todo = getAction();

            if (todo < 0) {
                quit();

                isRunning = false;
            } else if (todo == 1) {
                removeClient();

                // when is current client been serviced
                clientStartTime = YGlobals.getTime();

                client = newClient;
                newClient = null;

                setAction(0);
            } else {
                boolean isEnd = false;

                try {
                    // check whether client already send over commands, will not block
                    while (!isEnd) {
                        try {
                            client.receiveCmd(false);
                            isEnd = true;
                        } catch (InterruptedIOException e) {
                        }
                    }

                    // check whether client command buffer is empty, will block
                    if (client.getCmd() == null) {
                        isEnd = false;
                        while (!isEnd) {
                            try {
                                client.receiveCmd(true);
                                isEnd = true;
                            } catch (InterruptedIOException e) {
                            }
                        }
                    }
                } catch (YException e) {
                    YGlobals.Ydebugpn(e.getMessage());
                    removeClient();

                    pop.dispatcher.suspendClient(client);
                    client.closeSocket();
                    client = null;
                    continue;
                }

                // get client command if any
                String clientCmd = client.getCmd();
                if (clientCmd == null)
                    continue;

                String[] serverCmds = null;
                Vector images = new Vector();

                // convert client command to command that DEVise server can understand
                if (clientCmd.startsWith("JAVAC_SaveCurrentState")) {
                    clientCmd = new String("JAVAC_SaveSession {" + client.savedSessionName + "}");
                }

                // processing client command
                if (clientCmd.startsWith("JAVAC_Exit")) {
                    // js or jsa will make sure that the session is closed before this
                    pop.dispatcher.removeClient(client);
                    client = null;
                    continue;
                } else if (clientCmd.startsWith("JAVAC_GetStat") || clientCmd.startsWith("JAVAC_Refresh")) {
                    // not yet implemented
                    serverCmds = new String[1];
                    serverCmds[0] = new String("JAVAC_Done");

                    // erase this command from client command buffer
                    client.removeCmd();
                } else {
                    try {
                        serverCmds = sendCmd(clientCmd);
                        if (serverCmds != null && serverCmds.length > 0) {
                        } else {
                            throw new YException("Invalid response received from DEVise server!", 5);
                        }
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());
                        int id = e.getID();

                        if (id == 1) { // should not happen
                        } else if (id == 2) { // cmd socket communication error
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {CMD socket communication error with DEVise server!}"});
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                pop.dispatcher.suspendClient(client);
                                client.closeSocket();
                                client = null;
                            }

                            closeSocket();
                            startSocket();
                        } else if (id == 5) { // cmd socket invalid response
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {CMD socket communication error with DEVise server!}"});
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                pop.dispatcher.suspendClient(client);
                                client.closeSocket();
                                client = null;
                            }

                            closeSocket();
                            startSocket();
                        } else if (id == 7) { // cmd socket interrupted
                        } else { // should not happen
                        }

                        continue;
                    }

                    // erase this command from client command buffer since DEVise server has successfully processing this command
                    client.removeCmd();

                    try {
                        for (int i = 0; i < (serverCmds.length - 1); i++) {
                            if (serverCmds[i].startsWith("JAVAC_CreateWindow")) {
                                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                                if (cmds != null && cmds.length >= 8) {
                                    try {
                                        int imgSize = Integer.parseInt(cmds[6]);
                                        byte[] image = receiveImg(imgSize);
                                        if (image == null) {
                                            throw new YException("Can not read image data of size " + imgSize + "!", 6);
                                        }

                                        images.addElement(image);
                                    } catch (NumberFormatException e) {
                                        throw new YException("Invalid image size in create window command: " + cmds[6] + "!", 5);
                                    }
                                } else {
                                    throw new YException("Ill-formated create window command: " + serverCmds[i] + "!", 5);
                                }
                            } else if (serverCmds[i].startsWith("JAVAC_UpdateWindow")) {
                                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                                if (cmds != null && cmds.length == 3) {
                                    try {
                                        int imgSize = Integer.parseInt(cmds[2]);
                                        byte[] image = receiveImg(imgSize);
                                        if (image == null) {
                                            throw new YException("Can not read image data of size " + imgSize + "!", 6);
                                        }

                                        images.addElement(image);
                                    } catch (NumberFormatException e) {
                                        throw new YException("Invalid image size in update window command: " + cmds[2] + "!", 5);
                                    }
                                } else {
                                    throw new YException("Ill-formated update window command: " + serverCmds[i] + "!", 5);
                                }
                            }
                        }
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());
                        int id = e.getID();

                        if (id == 3) { // should not happen
                        } else if (id == 4) { // img socket communication error
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {IMG socket communication error with DEVise server!}"});
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                pop.dispatcher.suspendClient(client);
                                client.closeSocket();
                                client = null;
                            }

                            closeSocket();
                            startSocket();
                        } else if (id == 5) { // cmd socket invalid response
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {CMD socket communication error with DEVise server!}"});
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                pop.dispatcher.suspendClient(client);
                                client.closeSocket();
                                client = null;
                            }

                            closeSocket();
                            startSocket();
                        } else if (id == 6) { // img socket invalid response
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {IMG socket communication error with DEVise server!}"});
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                pop.dispatcher.suspendClient(client);
                                client.closeSocket();
                                client = null;
                            }

                            closeSocket();
                            startSocket();
                        } else if (id == 8) { // img socket interrupted
                        } else { // should not happen
                        }

                        continue;
                    }

                    if (clientCmd.startsWith("JAVAC_OpenSession")) {
                        client.isSessionOpened = true;
                    } else if (clientCmd.startsWith("JAVAC_CloseCurrentSession")) {
                        client.isSessionOpened = false;
                    } else if (clientCmd.startsWith("JAVAC_SaveSession")) {
                        client.isSessionSaved = true;
                    }
                }

                // sending results back to clients
                try {
                    client.sendCmd(serverCmds);
                    client.sendImg(images);
                } catch (YException e) {
                    YGlobals.Ydebugpn(e.getMessage());

                    removeClient();
                    pop.dispatcher.suspendClient(client);
                    client.closeSocket();
                    client = null;
                    continue;
                }
            }
        }

        YGlobals.Ydebugpn("Server thread is stopped ...");
    }

    private byte[] receiveImg(int size) throws YException
    {
        if (imgSocket == null)
            throw new YException("Null IMG socket!", 4);

        boolean isEnd = false;
        byte[] imgData = null;

        while (!isEnd) {
            try {
                imgData = imgSocket.receiveImg(size);
                isEnd = true;
            } catch (InterruptedIOException e) {
                //if (getAction() > 0)
                //    throw new YException("Aborted receiving image data!", 8);
            }
        }

        return imgData;
    }

    private String[] sendCmd(String command) throws YException
    {
        if (cmdSocket == null)
            throw new YException("Null CMD socket!", 2);

        String response = null;
        // isEnd is used to detect that JAVAC_Done or JAVAC_Error or JAVAC_Fail is received
        boolean isEnd = false;
        // isFinish is used to detect InterruptedIOException
        boolean isFinish = false;
        Vector rspbuf = new Vector();

        cmdSocket.sendCmd(command);

        while (!isEnd) {
            isFinish = false;

            while (!isFinish) {
                try {
                    response = cmdSocket.receiveRsp();
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    //if (getAction() > 0)
                    //    throw new YException("Aborted receiving command!", 7);
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Null response received for client command " + command, 5);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command " + response, 5);
                } else {
                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith("JAVAC_")) {
                        if (cmd.startsWith("JAVAC_Done") || cmd.startsWith("JAVAC_Error")
                            || cmd.startsWith("JAVAC_Fail") || cmd.startsWith("JAVAC_Exit")) {
                            isEnd = true;
                        }

                        rspbuf.addElement(cmd);
                    } else {
                        throw new YException("Unrecognized command " + response, 5);
                    }
                }
            }
        }

        if (rspbuf.size() > 0) {
            String[] rspstr = new String[rspbuf.size()];
            for (int i = 0; i < rspbuf.size(); i++)
                rspstr[i] = (String)rspbuf.elementAt(i);

            return rspstr;
        } else {
            throw new YException("Null response received for client command: " + command, 5);
        }
    }

    public synchronized static int getPort()
    {
        if (Port == DEViseGlobals.DEFAULTCMDPORT || Port == DEViseGlobals.DEFAULTIMGPORT)
            Port++;

        if (Port > 60000)
            Port = 3000;

        while (true) {
            try {
                ServerSocket socket = new ServerSocket(Port);
                socket.close();
                return Port++;
            } catch (IOException e) {
                Port++;

                if (Port == DEViseGlobals.DEFAULTCMDPORT || Port == DEViseGlobals.DEFAULTIMGPORT)
                    Port++;

                if (Port > 60000)
                    Port = 3000;
            }
        }
    }
}