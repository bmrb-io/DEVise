import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable
{
    private static int Port = 3000;
    private static String DEViseExec = new String("DEVise.jspop");

    private jspop pop = null;
    private int serverStartTimeout = 10 * 1000;
    private int serverStartTimestep = 2 * 1000;
    private int serverWaitTimeout = 30 * 1000;
    private int cmdSocketTimeout = 500;
    private int imgSocketTimeout = 500;
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
    // code = -1, no response received from DEVise server
    // code = 0, only "JAVAC_Done" is received
    // code = 1, "JAVAC_Done" is received, along with other commands
    // code = 2, "JAVAC_Error" or "JAVAC_Fail" is received
    private int deviseResponseCode = -1;

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

    public synchronized boolean setClient(DEViseClient c)
    {
        if (action != 0) {
            notifyAll();
            return false;
        } else {
            action = 1;
            newClient = c;
            notifyAll();
            return true;
        }
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

    public synchronized boolean stopServer()
    {
        if (action != 0) {
            notifyAll();
            return false;
        } else {
            action = -1;
            notifyAll();
            return true;
        }
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
        if (!isDEViseAlive()) {
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

        int time = 0;
        while (time < serverStartTimeout) {
            try {
                Thread.sleep(serverStartTimestep);
            } catch (InterruptedException e) {
            }

            if (!isDEViseAlive()) {
                // start DEVise server if it is dead
                if (!startDEVise()) {
                    YGlobals.Ydebugpn("Can not start DEVise Server!");
                    return false;
                }
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
        deviseResponseCode = -1;
    }

    private synchronized void restartServer()
    {
        closeSocket();
        stopDEVise();

        deviseResponseCode = -1;
        startSocket();
    }

    private synchronized void removeClient()
    {
        if (client != null) {
            pop.dispatcher.removeClient(client);
            client = null;
        }
    }

    private synchronized void suspendClient(boolean isClose)
    {
        if (client == null)
            return;

        client.isSwitched = true;

        if (!isClose) {
            pop.dispatcher.suspendClient(client);
            client.isSessionOpened = false;
            client = null;
            return;
        }

        if (client.isSessionOpened) {
            client.isSessionOpened = false;
            client.isSwitchSuccessed = false;

            String[] cmds = null;
            try {
                cmds = sendCmd("JAVAC_SaveSession {" + client.savedSessionName + "}");
                if (deviseResponseCode == 0) {
                    client.isSwitchSuccessed = true;
                    client.isSessionSaved = true;
                } else { // something wrong with the server
                    YGlobals.Ydebugpn("Can not saving session while switch client!");
                }

                cmds = sendCmd("JAVAC_CloseCurrentSession");
                if (deviseResponseCode != 0) {
                    YGlobals.Ydebugpn("Can not close session while switch client!");
                }
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());

                restartServer();
            }
        }

        pop.dispatcher.suspendClient(client);
        client = null;
    }

    // I am going to use a simple error handling scheme here
    //
    // 1. client: if anything wrong with network(YException with id = 1,2,3,4,5,6),
    //            id = 1, invalid argument in sendCmd
    //            id = 2, cmd socket communication error
    //            id = 3, invalid argument in receiveImg or sendImg
    //            id = 4, img socket communication error
    //            id = 5, invalid response from cmd socket
    //            id = 6, invalid response from img socket
    //
    //            I am going to kill it and throw it away
    //
    // 2. server: if anything wrong with network(YException with id = 1,2,3,4,5,6),
    //            id = 1, invalid argument in sendCmd
    //            id = 2, cmd socket communication error
    //            id = 3, invalid argument in receiveImg or sendImg
    //            id = 4, img socket communication error
    //            id = 5, invalid response from cmd socket
    //            id = 6, invalid response from img socket
    //
    //            I am going to kill it and restart a new server
    //
    public void run()
    {
        setStatus(0);
        setAction(0);

        boolean isRunning = true;
        boolean isFirstTime = true; // used to gurantee that at least one command is executed
        int todo;

        YGlobals.Ydebugpn("Server thread is started ...");

        while (isRunning) {
            waitForClient();
            todo = getAction();

            if (todo < 0) {
                quit();

                isRunning = false;
            } else if (todo == 1) {
                // save current opened session and close it if any
                suspendClient(true);

                // when is current client been serviced
                clientStartTime = YGlobals.getTime();
                isFirstTime = true;
                deviseResponseCode = -1;

                client = newClient;
                newClient = null;

                setAction(0);
            } else {
                boolean isEnd = false;

                try {
                    // check whether or not client already send over commands, will not block
                    while (!isEnd) {
                        try {
                            client.receiveCmd(false);
                            isEnd = true;
                        } catch (InterruptedIOException e) {
                        }
                    }

                    // check whether client command buffer is empty, will block
                    isEnd = false;
                    if (client.getCmd() == null) { // this usually will not happen in the first time, because this client is
                                                   // been serverd just because it has some command in its buffer, however,
                                                   // if client send over "JAVAC_Abort", this situation might be happening
                        // at this point, server is execute at least one client command, either "JAVAC_Abort"
                        // or some other command
                        isFirstTime = false;

                        while (!isEnd) {
                            try {
                                client.receiveCmd(true);
                                isEnd = true;
                            } catch (InterruptedIOException e) {
                                if (getAction() != 0)
                                    isEnd = true;
                            }
                        }
                    }

                    if (getAction() != 0 && !isFirstTime) {
                        continue;
                    }
                } catch (YException e) {
                    // The only thing could happen here is id = 2
                    YGlobals.Ydebugpn(e.getMessage());
                    removeClient();
                    continue;
                }

                // get client command if any
                String clientCmd = client.getCmd();
                isFirstTime = false;

                // the only situation this clientCmd will be null is that client send
                // "JAVAC_Abort" over
                if (clientCmd == null)
                    continue;

                // erase the command from client command buffer, so if any error happens,
                // this command will be losted
                client.removeCmd();
                
                // convert client command to command that DEVise server can understand
                if (clientCmd.startsWith("JAVAC_SaveCurrentState")) {
                    clientCmd = new String("JAVAC_SaveSession {" + client.savedSessionName + "}");
                }

                String[] serverCmds = null;
                Vector images = new Vector();
                // processing client command
                //if (clientCmd.startsWith("JAVAC_Exit")) {
                //    // js or jsa will make sure that the session is closed before this
                //    removeClient();
                //    continue;
                if (clientCmd.startsWith("JAVAC_GetStat")) {
                    // not yet implemented
                } else if (clientCmd.startsWith("JAVAC_Abort")) {
                    continue;
                } else {
                    // send command to DEVise server and receive response commands
                    try {
                        if (clientCmd.startsWith("JAVAC_CloseCurrentSession") || clientCmd.startsWith("JAVAC_Exit")) {
                            if (client.isSessionOpened) {
                                client.isSessionOpened = false;
                                client.isSwitched = false;
                                client.isSwitchSuccessed = false;
                                client.isSessionSaved = false;

                                serverCmds = sendCmd("JAVAC_CloseCurrentSession");
                            } else {
                                client.isSwitched = false;
                                client.isSwitchSuccessed = false;
                                client.isSessionSaved = false;

                                serverCmds = new String[1];
                                serverCmds[0] = new String("JAVAC_Done");
                                deviseResponseCode = 0;
                            }

                            if (clientCmd.startsWith("JAVAC_Exit")) {
                                removeClient();
                                continue;
                            }
                        } else if (clientCmd.startsWith("JAVAC_OpenLastSavedState")) {
                            client.isSessionOpened = false;
                            client.isSwitched = false;
                            client.isSwitchSuccessed = false;

                            if (client.isSessionSaved) {
                                clientCmd = "JAVAC_SetDisplaySize " + client.dimx + " " + client.dimy;
                                serverCmds = sendCmd(clientCmd);
                                if (deviseResponseCode == 0) {
                                    clientCmd = "JAVAC_OpenSession {" + client.savedSessionName + "}";
                                    serverCmds = sendCmd(clientCmd);
                                    if (deviseResponseCode != 2) {
                                        client.isSessionOpened = true;
                                    }
                                }
                            } else {
                                serverCmds = new String[1];
                                serverCmds[0] = new String("JAVAC_Error {Last saved session not found!}");
                                deviseResponseCode = 2;
                            }
                        } else if (clientCmd.startsWith("JAVAC_GetSessionList")) {
                            serverCmds = sendCmd(clientCmd);
                        } else if (clientCmd.startsWith("JAVAC_OpenSession")) {
                            client.isSessionOpened = false;
                            client.isSessionSaved = false;
                            client.isSwitched = false;
                            client.isSwitchSuccessed = false;

                            serverCmds = sendCmd(clientCmd);
                            if (deviseResponseCode != 2) {
                                client.isSessionOpened = true;
                            }
                        } else if (clientCmd.startsWith("JAVAC_SetDisplaySize")) {
                            serverCmds = sendCmd(clientCmd);
                            if (deviseResponseCode == 0) {
                                String[] cmds = DEViseGlobals.parseString(clientCmd);
                                if (cmds != null && cmds.length == 3) {
                                    try {
                                        client.dimx = Integer.parseInt(cmds[1]);
                                        client.dimy = Integer.parseInt(cmds[2]);
                                    } catch (NumberFormatException e) {
                                        serverCmds = new String[1];
                                        serverCmds[0] = "JAVAC_Error {Invalid screen sized specified!}";
                                        deviseResponseCode = 2;
                                    }
                                } else {
                                    serverCmds = new String[1];
                                    serverCmds[0] = "JAVAC_Error {Incorrect command to set screen size!}";
                                    deviseResponseCode = 2;
                                }
                            }
                        } else {
                            if (client.isSwitched && !client.isSessionOpened) {
                                client.isSwitched = false;

                                if (client.isSwitchSuccessed) {
                                    client.isSwitchSuccessed = false;

                                    serverCmds = sendCmd("JAVAC_SetDisplaySize " + client.dimx + " " + client.dimy);
                                    if (deviseResponseCode == 0) {
                                        serverCmds = sendCmd("JAVAC_OpenSession {" + client.savedSessionName + "}");
                                        if (deviseResponseCode != 2) {
                                            client.isSessionOpened = true;
                                            // need to clear img socket because we do not want to return these data
                                            try {
                                                if (imgSocket == null)
                                                    throw new YException("Null img Socket!", 4);

                                                imgSocket.clearSocket();
                                            } catch (YException e1) {
                                                YGlobals.Ydebugpn(e1.getMessage());
                                                try {
                                                    client.sendCmd(new String[] {"JAVAC_LastSavedState 1"});
                                                    suspendClient(true);
                                                } catch (YException e2) {
                                                    YGlobals.Ydebugpn(e2.getMessage());
                                                    removeClient();
                                                }

                                                restartServer();

                                                continue;
                                            }

                                            serverCmds = sendCmd(clientCmd);
                                        } else {
                                            serverCmds = new String[1];
                                            serverCmds[0] = new String("JAVAC_LastSavedState 1");
                                            deviseResponseCode = 0;
                                        }
                                    } else {
                                        serverCmds = new String[1];
                                        serverCmds[0] = new String("JAVAC_LastSavedState 1");
                                        deviseResponseCode = 0;
                                    }
                                } else {
                                    serverCmds = new String[1];
                                    deviseResponseCode = 0;
                                    if (client.isSessionSaved) {
                                        serverCmds[0] = new String("JAVAC_LastSavedState 1");
                                    } else {
                                        serverCmds[0] = new String("JAVAC_LastSavedState 0");
                                    }
                                }
                            } else if (client.isSessionOpened && !client.isSwitched) {
                                serverCmds = sendCmd(clientCmd);
                            } else {
                                serverCmds = new String[1];
                                serverCmds[0] = new String("JAVAC_Error {Incorrect client state detected!}");
                                deviseResponseCode = 2;
                            }
                        }
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());
                        int id = e.getID();

                        if (id == 7) { // DEVise server timeout
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {Can not receive response from DEVise server!}"});
                                suspendClient(false);
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                removeClient();
                            }

                            restartServer();
                        } else { // DEVise server cmd socket problem or anything should not be happening
                            try {
                                client.sendCmd(new String[] {"JAVAC_Error {Communication error while receiving response from DEVise server!}"});
                                suspendClient(false);
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());
                                removeClient();
                            }

                            restartServer();
                        }

                        continue;
                    }

                    // check the response from DEVise server
                    if (deviseResponseCode == 0) {
                        try {
                            // check whether or not client send over "JAVAC_Abort"
                            isEnd = false;
                            while (!isEnd) {
                                try {
                                    client.receiveCmd(false);
                                    isEnd = true;
                                } catch (InterruptedIOException e) {
                                }
                            }

                            if (client.getCmd() != null && client.getCmd().startsWith("JAVAC_Abort")) {
                                client.removeCmd();
                                suspendClient(true);
                            } else {
                                client.sendCmd(serverCmds);
                            }
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());
                            removeClient();
                        }

                        continue; // do not need to do anything
                    } else if (deviseResponseCode == 2) {
                        try {
                            // check whether or not client send over "JAVAC_Abort"
                            isEnd = false;
                            while (!isEnd) {
                                try {
                                    client.receiveCmd(false);
                                    isEnd = true;
                                } catch (InterruptedIOException e) {
                                }
                            }

                            if (client.getCmd() != null && client.getCmd().startsWith("JAVAC_Abort")) {
                                client.removeCmd();
                            } else {
                                client.sendCmd(serverCmds);
                            }

                            suspendClient(true);
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());
                            removeClient();
                        }

                        // need to clear up img socket, may not necessary
                        try {
                            if (imgSocket == null)
                                throw new YException("Null img socket!", 4);

                            imgSocket.clearSocket();
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());

                            restartServer();
                        }

                        continue;
                    } else {
                        try {
                            // check whether or not client send over "JAVAC_Abort"
                            isEnd = false;
                            while (!isEnd) {
                                try {
                                    client.receiveCmd(false);
                                    isEnd = true;
                                } catch (InterruptedIOException e) {
                                }
                            }

                            if (client.getCmd() != null && client.getCmd().startsWith("JAVAC_Abort")) {
                                client.removeCmd();
                                suspendClient(true);

                                // need to clear up img socket
                                try {
                                    if (imgSocket == null)
                                        throw new YException("Null img socket!", 4);

                                    imgSocket.clearSocket();
                                } catch (YException e1) {
                                    YGlobals.Ydebugpn(e1.getMessage());

                                    restartServer();
                                }

                                continue;
                            }
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());
                            removeClient();

                            // need to clear up img socket
                            try {
                                if (imgSocket == null)
                                    throw new YException("Null img socket!", 4);

                                imgSocket.clearSocket();
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());

                                restartServer();
                            }

                            continue;
                        }

                        // need to check whether or not there is image data in image socket
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
                                } else if (serverCmds[i].startsWith("JAVAC_UpdateGData")) {
                                    String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                                    if (cmds != null && cmds.length == 7) {
                                        try {
                                            int imgSize = Integer.parseInt(cmds[6]);
                                            byte[] image = receiveImg(imgSize);
                                            if (image == null) {
                                                throw new YException("Can not read image data of size " + imgSize + "!", 6);
                                            }

                                            images.addElement(image);
                                        } catch (NumberFormatException e) {
                                            throw new YException("Invalid GData size in update GData command: " + cmds[6] + "!", 5);
                                        }
                                    } else {
                                        throw new YException("Ill-formated update GData command: " + serverCmds[i] + "!", 5);
                                    }
                                }
                            }
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());
                            int id = e.getID();

                            if (id == 8) {
                                try {
                                    client.sendCmd(new String[] {"JAVAC_Error {Can not receive image data from DEVise server!}"});
                                    suspendClient(false);
                                } catch (YException e1) {
                                    YGlobals.Ydebugpn(e1.getMessage());
                                    removeClient();
                                }

                                restartServer();
                            } else {
                                try {
                                    client.sendCmd(new String[] {"JAVAC_Error {Communication error while receiving image data from DEVise server!}"});
                                    suspendClient(false);
                                } catch (YException e1) {
                                    YGlobals.Ydebugpn(e1.getMessage());
                                    removeClient();
                                }

                                restartServer();
                            }

                            continue;
                        }
                    }

                    try {
                        // check whether or not client send over "JAVAC_Abort"
                        isEnd = false;
                        while (!isEnd) {
                            try {
                                client.receiveCmd(false);
                                isEnd = true;
                            } catch (InterruptedIOException e) {
                            }
                        }

                        if (client.getCmd() != null && client.getCmd().startsWith("JAVAC_Abort")) {
                            client.removeCmd();
                        } else {
                            client.sendCmd(serverCmds);
                            client.sendImg(images);
                        }
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());
                        removeClient();
                    }
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
        int time = 0;

        while (!isEnd) {
            try {
                imgData = imgSocket.receiveImg(size);
                isEnd = true;
            } catch (InterruptedIOException e) {
                time += imgSocketTimeout;
                if (time > serverWaitTimeout) {
                    throw new YException("Can not receive response from DEVise server", 8);
                }
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

        int time = 0;

        deviseResponseCode = -1;

        cmdSocket.sendCmd(command);

        while (!isEnd) {
            isFinish = false;

            while (!isFinish) {
                try {
                    response = cmdSocket.receiveRsp();
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    time += cmdSocketTimeout;
                    if (time > serverWaitTimeout) {
                        throw new YException("Can not receive response from DEVise server!", 7);
                    }
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
                        if (cmd.startsWith("JAVAC_Done")) {
                            isEnd = true;
                            deviseResponseCode = 0;
                        } else if (cmd.startsWith("JAVAC_Error") || cmd.startsWith("JAVAC_Fail")) {
                            isEnd = true;
                            deviseResponseCode = 2;
                            rspbuf.removeAllElements();
                        }

                        rspbuf.addElement(cmd);
                    } else {
                        throw new YException("Unrecognized command " + response, 5);
                    }
                }
            }
        }

        if (rspbuf.size() > 0) {
            if (rspbuf.size() > 1 && deviseResponseCode == 0) {
                deviseResponseCode = 1;
            }

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
        if (Port == DEViseGlobals.CMDPORT || Port == DEViseGlobals.IMGPORT)
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

                if (Port == DEViseGlobals.CMDPORT || Port == DEViseGlobals.IMGPORT)
                    Port++;

                if (Port > 60000)
                    Port = 3000;
            }
        }
    }
}