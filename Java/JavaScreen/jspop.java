import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private String usage = new String("usage: java jspop -port[port] -server[number] -timeout[time] -userfile[filename] -logfile[filename] -debug[number]");
    // -PORT[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    // -SERVER[number]:
    //      number: The maximum number of active DEVise server, if blank, use defualts
    //      default: 1
    // -TIMEOUT[time]:
    //      time: The timeout value(in seconds) for DEVise Server to start up before abort
    //      default: 60
    // -LOGFILE[filename]:
    //      filename: The name of the file that contain client log information
    //      default: clients.log
    // -USERFILE[filename]:
    //      filename: The name of the file that contain users information
    //      default: users.cfg
    // -DEBUG[number]:
    //      number: The debug level for writing debug information to console
    //      default: No Debug information is written
    //
    private int cmdPort = DEViseGlobals.DEFAULTCMDPORT;
    private int imgPort = DEViseGlobals.DEFAULTIMGPORT;
    private ServerSocket cmdServerSocket = null;
    private ServerSocket imgServerSocket = null;

    public String userFile = new String("users.cfg");
    public String logFile = new String("clients.log");

    public Hashtable users = new Hashtable();
    public Hashtable clients = new Hashtable();
    public Vector servers = new Vector();
    public int maxServer = 1;

    public int serverStartTimeout = 5 * 60 * 1000; // wait for DEVise server to start up for 5 minutes before abort
    public int serverWaitTimeout = 5 * 60 * 1000; // wait for DEVise server response for 5 minutes before abort
    public int jspopWaitTimeout = 5 * 1000; // wait client connection for 5 seconds before disconnect
    public int dispatcherTimestep = 1 * 1000; // do scheduling every 1 seconds
    public int cmdSocketTimeout = 1 * 1000; // interrupt cmdSocket every 1 seconds
    public int imgSocketTimeout = 1 * 1000; // interrupt imgSocket every 1 seconds
    public int logThreadTimestep = 30 * 60 * 1000; // write logfile every 30 minutes

    public DEViseClientDispatcher dispatcher = null;
    public DEViseLogWriter logwriter = null;

    private int IDCount = 0;

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

        YGlobals.YISAPPLET = false;
        YGlobals.YISGUI = false;

        // force java VM to run every object's finalizer on normal or abnormal exit
        //System.runFinalizersOnExit(true);

        jspop popServer = new jspop(args);
        Thread popThread = new Thread(popServer);
        popThread.start();
    }

    public jspop(String[] args)
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);

        YGlobals.start();

        System.out.println("Loading user configuration file and client log file ...\n");
        checkFiles();

        System.out.println("Starting log writer ...\n");
        logwriter = new DEViseLogWriter(this);
        logwriter.startWriter();

        System.out.println("Starting client dispatcher ...\n");
        dispatcher = new DEViseClientDispatcher(this);
        dispatcher.startDispatcher();

        System.out.println("Starting DEVise servers ...\n");
        startServers();

        System.out.println("\nStarting jspop server sockets ...\n");
        try {
            cmdServerSocket = new ServerSocket(cmdPort);
        } catch (IOException e) {
            System.out.println("Can not start server sokcet at port " + cmdPort);
            System.exit(1);
        }

        try {
            imgServerSocket = new ServerSocket(imgPort);
            imgServerSocket.setSoTimeout(jspopWaitTimeout);
        } catch (IOException e) {
            System.out.println("Can not start server socket at port " + imgPort);
            try {
                cmdServerSocket.close();
            } catch (IOException e1) {
            }

            System.exit(1);
        }
    }

    private void startServers()
    {
        DEViseServer newserver = null;

        for (int i = 0; i < maxServer; i++) {
            try {
                newserver = new DEViseServer(this);
                newserver.startServer();
                servers.addElement(newserver);
                System.out.println("Successfully start server " + (i + 1) + " out of " + maxServer + "\n");
            } catch (YException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
        }
    }

    public void run()
    {
        System.out.println("\nJSPOP Server started ...\n");

        boolean isListen = true;
        boolean isTimeout = false;
        Socket socket1 = null;
        Socket socket2 = null;
        DEViseCmdSocket cmd = null;
        DEViseImgSocket img = null;
        String hostname = null;

        while (isListen) {
            try {
                socket1 = cmdServerSocket.accept();
                hostname = socket1.getInetAddress().getHostName();
                System.out.println("Client connection request from " + hostname + " is received ...");

                try {
                    socket2 = imgServerSocket.accept();
                    isTimeout = false;
                } catch (IOException e) {
                    isTimeout = true;
                }

                if (isTimeout) {
                    System.out.println("Can not received image port connection request within timeout!\nClose socket connection to client " + hostname + "!");
                    try {
                        socket1.close();
                    } catch (IOException e) {
                    }
                } else {
                    try {
                        cmd = new DEViseCmdSocket(socket1, 1000);
                        img = new DEViseImgSocket(socket2, 1000);
                        addClient(cmd, img, hostname);
                    } catch (YException e) {
                        YGlobals.Ydebugpn(e.getMessage());
                        System.out.println("Can not create data channel to new socket connection from " + hostname + "!\nClose socket connection to client " + hostname + "!");

                        if (cmd != null) {
                            cmd.closeSocket();
                            cmd = null;
                        }

                        if (img != null) {
                            img.closeSocket();
                            img = null;
                        }
                    }
                }
            } catch (IOException e) {
                System.out.println("JSPOP Server can not listen on port: " + cmdPort + "!\nJSPOP Server is aborted!");
                isListen = false;
            }
        }

        quit();
    }

    private synchronized void quit()
    {
        System.out.println("\nDEVise POP Server stopped ...\n");
        System.exit(0);
    }

    private synchronized void addClient(DEViseCmdSocket cmd, DEViseImgSocket img, String hostname)
    {
        if (cmd == null || img == null || hostname == null)
            return;

        boolean isEnd = false;
        int time = 0;

        while (!isEnd) {
            try {
                String command = cmd.receiveRsp();
                if (command != null && command.startsWith("JAVAC_Connect")) {
                    String[] commands = DEViseGlobals.parseString(command);
                    if (commands != null && commands.length == 4) {
                        try {
                            int id = Integer.parseInt(commands[3]);
                            if (users.containsKey(commands[1])) {
                                DEViseUser u = (DEViseUser)users.get(commands[1]);
                                if (u != null) {
                                    if (commands[2].equals(u.getPassword())) {
                                        if (id != DEViseGlobals.DEFAULTID) {
                                            if (!clients.containsKey(new Integer(id))) {
                                                throw new YException("Invalid connection ID: " + id + "!");
                                            } else {
                                                DEViseClient c = (DEViseClient)clients.get(new Integer(id));
                                                if (!u.equals(c.getUser()) || !hostname.equals(c.getHost())) {
                                                    throw new YException("Invalid connection ID: " + id + "!");
                                                } else {
                                                    c.setSockets(cmd, img);
                                                    YGlobals.Ydebugpn("Old client connected with ID " + id + "!");
                                                }
                                            }
                                        } else {
                                            if (!u.checkLogin()) {
                                                throw new YException("Maximum number of logins has been reached for user " + commands[1] + "!");
                                            }

                                            id = getNewID();
                                            DEViseClient newClient = new DEViseClient(u, cmd, img, hostname, id);
                                            clients.put(newClient.getID(), newClient);
                                            u.addClient(newClient);
                                            dispatcher.addClient(newClient);
                                            YGlobals.Ydebugpn("New client connected with ID " + id + "!");
                                        }

                                        cmd.sendCmd("JAVAC_User " + id);
                                        cmd.sendCmd("JAVAC_Done");
                                        isEnd = true;
                                    } else {
                                        throw new YException("Invalid password: " + commands[2] + "!");
                                    }
                                } else {
                                    throw new YException("Invalid user: " + commands[1] + "!");
                                }
                            } else {
                                throw new YException("Invalid user: " + commands[1] + "!");
                            }
                        } catch (NumberFormatException e) {
                            throw new YException("Invalid connection request: " + command + "!");
                        }
                    } else {
                        throw new YException("Invalid connection request: " + command + "!");
                    }
                } else {
                    throw new YException("Invalid connection request!");
                }
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());

                int id = e.getID();
                if (id == 0) { // Invalid connection request or client check failed
                    try {
                        cmd.sendCmd("JAVAC_Exit {" + e.getMessage() + "}");
                    } catch (YException e1) {
                    }
                } else if (id == 2) { // communication error
                } else { // should not happen
                }

                System.out.println("Connection error: " + e.getMessage() + "!\nClose socket connection to client " + hostname + "!");
                cmd.closeSocket();
                img.closeSocket();
                isEnd = true;
            } catch (InterruptedIOException e) {
                time += cmdSocketTimeout;
                if (time > jspopWaitTimeout) {
                    System.out.println("Can not receive connection request from " + hostname + " within timeout!\nClose socket connection to client " + hostname + "!");
                    cmd.closeSocket();
                    img.closeSocket();
                    isEnd = true;
                }
            }
        }
    }

    public synchronized void removeClient(DEViseClient client)
    {
        if (client != null) {
            client.close();
            clients.remove(client.getID());
        }
    }

    private int getNewID()
    {
        if (IDCount == Integer.MAX_VALUE) {
            IDCount = 0;
        }

        Integer id = new Integer(IDCount);
        while (clients.containsKey(id)) {
            IDCount++;
            if (IDCount == Integer.MAX_VALUE)
                IDCount = 0;
            id = new Integer(IDCount);
        }

        return IDCount++;
    }

    private void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-port")) {
                if (!args[i].substring(5).equals("")) {
                    try {
                        cmdPort = Integer.parseInt(args[i].substring(5));
                        if (cmdPort < 1024 || cmdPort > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid command port number " + args[i].substring(5) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-server")) {
                if (!args[i].substring(7).equals("")) {
                    try {
                        maxServer = Integer.parseInt(args[i].substring(7));
                        if (maxServer < 1 || maxServer > 10)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid number of server " + args[i].substring(7) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-timeout")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        serverStartTimeout = Integer.parseInt(args[i].substring(8));
                        if (serverStartTimeout < 0)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid timeout value of server " + args[i].substring(8) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        YGlobals.YDEBUG = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid debug level " + args[i].substring(6) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                } else {
                    YGlobals.YDEBUG = 1;
                }
            } else if (args[i].startsWith("-logfile")) {
                if (!args[i].substring(8).equals(""))
                     logFile = args[i].substring(8);
            } else if (args[i].startsWith("-userfile")) {
                if (!args[i].substring(9).equals(""))
                     userFile = args[i].substring(9);
            } else {
                System.out.println("Invalid jspop option " + args[i] + " is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }

        /*
        File dir = new File(tmpDir);
        if (!dir.exists()) {
            System.out.println("Specified temporary directory " + tmpDir + " is not exist!");
            System.exit(1);
        }
        if (!dir.isDirectory()) {
            System.out.println("Specified temporary directory " + tmpDir + " is not a directory!");
            System.exit(1);
        }
        if (!dir.canRead() || !dir.canWrite()) {
            System.out.println("Specified temporary directory " + tmpDir + " is not readable or writable!");
            System.exit(1);
        }
        */

        File file = new File(userFile);
        if (!file.exists()) {
            System.out.println("Specified user configuration file " + userFile + " is not exist!");
            System.exit(1);
        }
        if (!file.isFile()) {
            System.out.println("Specified user configuration file " + userFile + " is not a file!");
            System.exit(1);
        }
        if (!file.canRead() || !file.canWrite()) {
            System.out.println("Specified user configuration file " + userFile + " is not readable or writable!");
            System.exit(1);
        }
    }

    private void checkFiles()
    {
        RandomAccessFile uf = null;
        try {
            uf = new RandomAccessFile(userFile, "r");
        } catch (IOException e) {
            System.out.println("Can not open user configuration file " + userFile);
            System.exit(1);
        }

        try {
            String str = uf.readLine();
            while (str != null) {
                str = str.trim();
                // skip comment line
                if (!str.startsWith("#") && !str.equals("")) {
                    String[] line = YGlobals.Yparsestr(str, ":");
                    if (line.length != 5)
                        throw new YException("Invalid line read from user configuration file: " + str);

                    try {
                        DEViseUser user = new DEViseUser(line[0], line[1], Integer.parseInt(line[2]),
                                          Integer.parseInt(line[3]), Long.parseLong(line[4]));
                        users.put(user.getName(), user);
                    } catch (NumberFormatException e) {
                        throw new YException("Invalid user information read from user configuration file: " + str);
                    }
                }

                str = uf.readLine();
            }
        } catch (IOException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println("Can not read from user configuration file " + userFile);
            System.exit(1);
        } catch (YException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println(e.getMessage());
            System.exit(1);
        }

        try {
            uf.close();
        } catch (IOException e) {
        }

        RandomAccessFile lf = null;
        try {
            lf = new RandomAccessFile(logFile, "rw");
        } catch (IOException e) {
            lf = null;
        }

        if (lf != null) {

            try {
                lf.close();
            } catch (IOException e) {
            }
        }
    }
}
