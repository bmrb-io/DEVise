import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private String usage = new String("usage: java jspop -port[port] -server[number] -timeout[time] -logfile[filename] -tmpdir[dir] -debug");
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
    //      filename: The name of the file that contain user log information
    //      default: users.cfg
    // -TMPDIR[dir]:
    //      dir: The directory for store temporary files
    //      default: current Directory
    // -DEBUG:
    //      Write debug information to console
    //      default: No Debug information is written
    //
    private int cmdPort = DEViseGlobals.DEFAULTCMDPORT;
    private int imgPort = DEViseGlobals.DEFAULTIMGPORT;
    private String tmpDir = new String(".");
    private String userFile = new String("users.cfg");
    private Vector users = new Vector();
    private Vector servers = new Vector();

    public int maxServer = 1;
    public long serverTimeout = 60;

    private ServerSocket cmdServerSocket = null;
    private ServerSocket imgServerSocket = null;

    public DEViseClientDispatcher dispatcher = null;

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
        YGlobals.YISLOG = false;

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

        System.out.println("Loading user configuration file ...\n");
        checkFiles();

        System.out.println("Starting DEVise server ...\n");
        startServers();

        System.out.println("Start client dispatcher ...\n");
        dispatcher = new DEViseClientDispatcher(this, users, servers);
        dispatcher.startDispatcher();

        System.out.println("\nStarting server socket ...\n");
        try {
            cmdServerSocket = new ServerSocket(cmdPort);
        } catch (IOException e) {
            System.out.println("Can not start server sokcet at port " + cmdPort);
            System.exit(1);
        }

        try {
            imgServerSocket = new ServerSocket(imgPort);
        } catch (IOException e) {
            System.out.println("Can not start server socket at port " + imgPort);
            try {
                cmdServerSocket.close();
            } catch (IOException e1) {
            }

            System.exit(1);
        }
    }

    public void run()
    {
        System.out.println("DEVise JSPOP Server started ...\n");

        boolean isListen = true;
        while (isListen) {
            Socket socket1 = null;
            Socket socket2 = null;
            DEViseCmdSocket cmd = null;
            DEViseImgSocket img = null;
            DEViseClient client = null;
            
            try {
                socket1 = cmdServerSocket.accept();
                socket2 = imgServerSocket.accept();
                
                try {
                    String name = socket1.getInetAddress().getHostName();
                    System.out.println("Client connection request from " + name + " is received ...");
                    
                    cmd = new DEViseCmdSocket(socket1, 1000);
                    img = new DEViseImgSocket(socket2, 2000);
                    client = new DEViseClient(cmd, img, name);
                } catch (YException e) {
                    YGlobals.Ydebugpn(e.getMessage());
                    
                    if (cmd != null) {
                        cmd.closeSocket();
                        cmd = null;
                    }
                    
                    if (img != null) {
                        img.closeSocket();
                        img = null;
                    }                                                                 
                } 
                                        
                dispatcher.addClient(client);
            } catch (IOException e) {
                System.out.println("DEVise POP Server can not listen on port: " + cmdPort + " or " + imgPort + "\nDEVise JSPOP Server is aborted!");
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
                        maxServer = Integer.parseInt(args[i].substring(10));
                        if (maxServer < 1 || maxServer > 10)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid number of server " + args[i].substring(10) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-timeout")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        serverTimeout = Long.parseLong(args[i].substring(8));
                        if (serverTimeout < 0)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid timeout value of server " + args[i].substring(10) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-debug")) {
                YGlobals.YISDEBUG = true;
            } else if (args[i].startsWith("-logfile")) {
                if (!args[i].substring(8).equals(""))
                     userFile = args[i].substring(5);
            } else if (args[i].startsWith("-tmpdir")) {
                if (!args[i].substring(7).equals(""))
                     tmpDir = args[i].substring(7);
            } else {
                System.out.println("Invalid jspop option " + args[i] + " is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }

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
        RandomAccessFile file = null;
        RandomAccessFile bakfile = null;

        try {
            file = new RandomAccessFile(userFile, "rw");
        } catch (IOException e) {
            System.out.println("Can not open file " + userFile);
            System.exit(1);
        }

        try {
            bakfile = new RandomAccessFile(userFile + ".bak", "rw");
        } catch (IOException e) {
            System.out.println("Can not open file " + userFile + ".bak");
            System.exit(1);
        }

        try {
            String str = file.readLine();
            while (str != null) {
                str = str.trim();
                bakfile.writeBytes(str + "\n");
                // skip comment line
                if (!str.startsWith("#")) {
                    if (!str.equals("")) {
                        String[] line = YGlobals.Yparsestr(str, ":");
                        //YGlobals.Ydebugpn("length is " + line.length);
                        if (line.length != 8)
                            throw new YException("Incorrect format of user information: " + str);

                        try {
                            DEViseUser user = new DEViseUser(line[0], line[1], Integer.parseInt(line[2]),
                                              Integer.parseInt(line[3]), Long.parseLong(line[4]), Long.parseLong(line[5]),
                                              line[6], Long.parseLong(line[7]));
                            users.addElement(user);
                        } catch (NumberFormatException e) {
                            throw new YException("Incorrect value of user information: " + str);
                        }
                    }
                }

                str = file.readLine();
            }
        } catch (IOException e) {
            System.out.println("Can read from file " + userFile + " or write to file " + userFile + ".bak");
            System.exit(1);
        } catch (YException e) {
            try {
                file.close();
            } catch (IOException e1) {
            }

            try {
                bakfile.close();
            } catch (IOException e1) {
            }

            System.out.println(e.getMessage());

            System.exit(1);
        }

        try {
            file.close();
        } catch (IOException e1) {
        }

        try {
            bakfile.close();
        } catch (IOException e1) {
        }
    }

    private void startServers()
    {
        DEViseServer newserver = null;

        for (int i = 0; i < maxServer; i++) {
            try {
                newserver = new DEViseServer(this, serverTimeout);
                servers.addElement(newserver);
                System.out.println("Successfully start server " + (i + 1) + " out of " + maxServer + "\n");
            } catch (YException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
        }
    }
}
