import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable
{
    private static int Port = 3000;

    private jspop pop = null;

    private static String DEViseExec = new String("DEVise.jspop");
    private Process proc = null;
    private long timeout = 0;
    private long timestep = 2000;

    private String hostname = "localhost";
    private int imgPort = 0, cmdPort = 0, switchPort = 0;
    public DEViseCmdSocket cmdSocket = null;
    public DEViseImgSocket imgSocket = null;

    public Thread serverThread = null;
    private DEViseClient newClient = null;
    private DEViseClient client = null;
    // status = -1, serverThread and DEVise Server quit
    // status = 0, serverThread running
    // status = 1, serverThread wait for new client
    private int status = -1;
    // action = 0, no change of current status
    // action = 1, change client
    // action < 0, quit
    private int action = 0;
    private int tryTime = 0;

    public DEViseServer(jspop j, long time) throws YException
    {
        pop = j;

        if (time < 10)
            timeout = 10 * 1000;
        else
            timeout = time * 1000;

        startServer();
    }

    public synchronized int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public synchronized int getImgPort()
    {
        if (isSocketAlive())
            return imgPort;
        else
            return -1;
    }

    public synchronized int getCmdPort()
    {
        if (isSocketAlive())
            return cmdPort;
        else
            return -1;
    }

    public synchronized int getTryTime()
    {
        return tryTime;
    }

    public synchronized DEViseClient getClient()
    {
        return client;
    }

    private synchronized boolean startSocket()
    {
        if (isSocketAlive()) {
            return true;
        } else if (!isDEViseAlive()) {
            YGlobals.Ydebugpn("Can not start Socket because DEVise server not found!");
            return false;
        }

        long time = 0;
        while (time < timeout) {
            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            time += timestep;

            try {
                cmdSocket = new DEViseCmdSocket(hostname, cmdPort, 1000);
                imgSocket = new DEViseImgSocket(hostname, imgPort, 2000);

                YGlobals.Ydebugpn("Successfully connect to sockets ...");
                return true;
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());
                closeSocket();
            }
        }

        closeSocket();

        YGlobals.Ydebugpn("Can not connect to sockets within " + (timeout/1000) + " seconds!");

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

    private synchronized void closeSocket()
    {
        if (cmdSocket != null)
            cmdSocket.closeSocket();
        if (imgSocket != null)
            imgSocket.closeSocket();

        cmdSocket = null;
        imgSocket = null;
    }

    private synchronized boolean startDEVise()
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

        YGlobals.Ydebugpn("Successfully start devised ...");

        return true;
    }

    public synchronized boolean isDEViseAlive()
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

    public synchronized void startServer() throws YException
    {
        YGlobals.Ydebugpn("Trying to start DEVise Server ...");

        tryTime++;

        // start DEVise server if it is dead
        if (!startDEVise())
            throw new YException("Can not start DEVise Server!", "DEViseServer:startServer");

        // Reconnect sockets if they are dead
        if (!startSocket())
            throw new YException("Can not connect to DEVise Server!", "DEViseServer:startServer");

        // start server Thread but first check if server thread is active
        if (getStatus() < 0) {
            serverThread = new Thread(this);
            serverThread.start();
        }

        tryTime = 0;

        YGlobals.Ydebugpn("Successfully start DEVise Server ...");
    }

    private synchronized void setAction(int a)
    {
        action = a;
        //notifyAll();
    }

    private synchronized int getAction()
    {
        return action;
    }

    public synchronized boolean stopServer()
    {
        if (action == 0 && client == null) {
            action = -1;
            notifyAll();
            return true;
        } else {
            return false;
        }
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

        int old = status;
        while (client == null) {
            status = 1;
            try {
                wait();
            } catch (InterruptedException e) {
            }

            if (action != 0)
                break;
        }

        status = old;
    }

    private String getClientCmd() throws YException
    {
        boolean isEnd = false;
        String cmd = null;

        while (!isEnd) {
            try {
                //YGlobals.Ydebugpn("Starting to receive command!");
                cmd = client.cmdSocket.receiveRsp();
                //YGlobals.Ydebugpn("Received command: " + cmd);
                isEnd = true;
            } catch (InterruptedIOException e) {
                if (getAction() != 0) {
                    throw new YException("Client interrupted!");
                }
            }
        }

        return cmd;
    }

    public void run()
    {
        setStatus(0);
        setAction(0);

        boolean isRunning = true;
        int todo;

        String clientCmd = null;

        YGlobals.Ydebugpn("DEViseServer thread is started ...");

        while (isRunning) {
            waitForClient();
            todo = getAction();

            if (todo < 0) {
                // client is ensured to be NULL at this point
                isRunning = false;
            } else if (todo == 1) {
                boolean flag = true;
                if (client != null && client.getSessionFlag()) {
                    try {
                        String[] cmd = sendCommand("JAVAC_SaveSession {" + client.getLastSavedSession() + "}");
                        if (cmd == null) {
                            throw new YException("Invalid response received from server!", 5);
                        } else {
                            flag = true;

                            if (cmd.length == 1 && cmd[0].equals("JAVAC_Done")) {
                                client.setStage(5);
                            } else {
                                YGlobals.Ydebugpn(cmd[0]);
                            }
                        }
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());

                        closeSocket();

                        flag = false;

                        //try {
                        //    client.cmdSocket.sendCmd("JAVAC_Fail {Server Failed!}");
                        //} catch (YException e) {
                        //}

                        //pop.dispatcher.suspendClient(client, true);
                        //client = null;
                        //continue;
                    }

                    if (!flag) {
                        pop.dispatcher.suspendClient(client, true);
                        pop.dispatcher.suspendClient(newClient, false);
                        client = null;
                        newClient = null;

                        setAction(0);
                        continue;
                    } else {
                        pop.dispatcher.suspendClient(client, false);
                    }
                }

                client = newClient;
                newClient = null;

                setAction(0);
            } else {
                int stage = client.getStage();

                try {
                    if (stage == 0) { // not yet establish connection
                        clientCmd = getClientCmd();
                    } else if (stage == 1) { // clean start, already get client's command and finished processing it
                                             // need to send back response and wait for next instruction
                        if (client.cmdSocket.isEmpty()) { // check if client want to abort
                            String[] cmds = client.getCmds();
                            if (cmds != null) {
                                for (int i = 0; i < cmds.length; i++) {
                                    if (cmds[i] != null && cmds[i].length() != 0) {
                                        client.cmdSocket.sendCmd(cmds[i]);
                                    }
                                }
                            }

                            client.setCmds(null);

                            Vector images = client.getImages();
                            if (images != null) {
                                for (int i = 0; i < images.size(); i++) {
                                    byte[] image = (byte[])images.elementAt(i);
                                    if (image != null && image.length != 0) {
                                        client.imgSocket.sendImg(image);
                                    }
                                    //YGlobals.Ydebugpn("Sending image size " + image.length);
                                }

                                client.setImages(null);
                            }
                        } else { // client want to abort last commands
                            boolean isEnd = false;
                            while (!isEnd) {
                                try {
                                    String cmd = client.cmdSocket.receiveRsp();
                                    isEnd = true;
                                    if (cmd != null && cmd.startsWith("JAVAC_Abort")) {
                                        client.setCmds(null);
                                        client.setImages(null);
                                    } else {
                                        throw new YException("Invalid abort information received from client!", 5);
                                    }
                                } catch (InterruptedIOException e) {
                                }
                            }
                        }

                        clientCmd = getClientCmd();
                    } else if (stage == 2) { // unclean start, not yet get client's command by interruption
                        clientCmd = getClientCmd();
                    } else if (stage == 3) {// unclean start, already get client's command, but server crashed, do did not finish it
                        // if server crashed, I will just send client a JAVAC_Fail error, so both sides closed, so
                        // next time it will be a clean start
                        clientCmd = getClientCmd();
                    } else if (stage == 4) {// unclean start, already get client's command, but server been interrupted so did not finish it
                        // not yet implemented, at this time, I just assume server's action will be interrupted
                        clientCmd = getClientCmd();
                    } else if (stage == 5) { // client been interrupted last time
                        clientCmd = new String("JAVAC_OpenSession {" + client.getLastSavedSession() + "}");
                    } else { // should not happening
                        clientCmd = getClientCmd();
                    }
                } catch (YException e) {
                    int id = e.getID();
                    YGlobals.Ydebugpn(e.getMessage());

                    if (id == 0) { // context switch, client stop
                        if (stage == 0) { // leave it as it is
                        } else {
                            client.setStage(2);
                        }

                        continue;
                    } else if (id == 2 || id == 4) { // socket communication error
                        pop.dispatcher.suspendClient(client, true);
                        client = null;
                        clientCmd = null;
                        continue;
                    } else if (id == 1 || id == 3) { // socket invalid arguments, should not be happening
                        clientCmd = null;
                    } else if (id == 5) { // invalid response received
                        clientCmd = null;
                    } else { // should not be happening
                        clientCmd = null;
                    }
                }

                if (clientCmd == null) { // send JAVAC_Fail and remove client
                    try {
                        client.cmdSocket.sendCmd("JAVAC_Fail {Incorrect command received!}");
                    } catch (YException e) {
                    }

                    pop.dispatcher.suspendClient(client, true);
                    client = null;
                    continue;
                }

                client.setStage(3);

                try {
                    String[] serverCmds = null;
                    Vector images = new Vector();

                    if (clientCmd.startsWith("JAVAC_Exit")) {
                        pop.dispatcher.removeClient(client);
                        client = null;
                        continue;
                    } else if (clientCmd.startsWith("JAVAC_GetStat") || clientCmd.startsWith("JAVAC_Refresh")) {
                        serverCmds = new String[1];
                        serverCmds[0] = new String("JAVAC_Done");
                        client.setCmds(serverCmds);
                        client.setImages(null);
                        client.setStage(1);
                        continue;
                    } else if (clientCmd.startsWith("JAVAC_Abort")) {
                        client.setCmds(null);
                        client.setImages(null);
                        client.setStage(1);
                        continue;
                    } else if (clientCmd.startsWith("JAVAC_Connect")) {
                        String cmds[] = DEViseGlobals.parseString(clientCmd);
                        if (cmds == null || cmds.length != 4) {
                            try {
                                client.cmdSocket.sendCmd("JAVAC_Exit {Incorrect connection received!}");
                                continue;
                            } catch (YException e) {
                                pop.dispatcher.removeClient(client);
                                client = null;
                                continue;
                            }
                        }

                        try {
                            String name = cmds[1];
                            String pass = cmds[2];

                            int id = Integer.parseInt(cmds[3]);
                            if (pop.dispatcher.checkClient(client, name, pass, id) != 0) {
                                throw new NumberFormatException();
                            }

                            serverCmds = new String[2];
                            serverCmds[0] = new String("JAVAC_User " + client.getID());
                            serverCmds[1] = new String("JAVAC_Done");
                            client.setCmds(serverCmds);
                            client.setImages(null);
                            client.setStage(1);
                            continue;
                        } catch (NumberFormatException e) {
                            pop.dispatcher.removeClient(client);
                            client = null;
                            continue;
                        }
                    } else {
                        if (clientCmd.startsWith("JAVAC_SaveCurrentState")) {
                            clientCmd = new String("JAVAC_SaveSession " + client.getLastSavedSession());
                        }

                        serverCmds = sendCommand(clientCmd);

                        for (int i = 0; i < (serverCmds.length - 1); i++) {
                            if (serverCmds[i].startsWith("JAVAC_CreateWindow")) {
                                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                                if (cmds == null || cmds.length < 8)
                                    throw new YException("Ill-formated command " + serverCmds[i] + "!", 5);

                                try {
                                    int imgSize = Integer.parseInt(cmds[6]);
                                    byte[] image = receiveImg(imgSize);
                                    if (image == null) {
                                        throw new NumberFormatException();
                                    }

                                    images.addElement(image);
                                } catch (NumberFormatException e) {
                                    throw new YException("Incorrect image size " + cmds[6] + "!", 5);
                                }
                            } else if (serverCmds[i].startsWith("JAVAC_UpdateWindow")) {
                                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                                if (cmds == null || cmds.length != 3)
                                    throw new YException("Ill-formated command " + serverCmds[i] + "!", 5);

                                try {
                                    int imgSize = Integer.parseInt(cmds[2]);
                                    byte[] image = receiveImg(imgSize);
                                    if (image == null)
                                        throw new NumberFormatException();

                                    images.addElement(image);
                                } catch (NumberFormatException e) {
                                    throw new YException("Incorrect image size " + cmds[2] + "!", 5);
                                }
                            }
                        }

                        if (clientCmd.startsWith("JAVAC_OpenSession")) {
                            client.setSessionFlag(true);
                        } else if (clientCmd.startsWith("JAVAC_CloseCurrentSession")) {
                            client.setSessionFlag(false);
                        }

                        client.setCmds(serverCmds);
                        client.setImages(images);
                        client.setStage(1);
                        continue;
                    }
                } catch (YException e) {
                    YGlobals.Ydebugpn(e.getMessage() + " at " + e.getWhere());

                    //cmdSocket.clearSocket();
                    //imgSocket.clearSocket();
                    closeSocket();

                    try {
                        client.cmdSocket.sendCmd("JAVAC_Fail {Server Failed!}");
                    } catch (YException e1) {
                    }

                    pop.dispatcher.suspendClient(client, true);
                    client = null;
                    continue;
                }
            }
        }

        YGlobals.Ydebugpn("DEViseServer thread is stopped ...");

        quit();
    }

    private synchronized void quit()
    {
        setStatus(-1);
        setAction(0);

        if (isSocketAlive()) {
            closeSocket();
        } else if (isDEViseAlive()) {
            proc.destroy();
            proc = null;
        }

        client = null;
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
                //if (isAbort())
                //    throw new YException("Dispatcher::receiveImg: Aborted!");
            }
        }

        return imgData;
    }

    private String[] sendCommand(String command) throws YException
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