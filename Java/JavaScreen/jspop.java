import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private String usage = new String("usage: java jspop -PORT[portnum] -WAITTIME[time] -DEBUG -LOG[filename] -ROOTDIR[dir] -MAXSERVER[number] -TIMEOUT[time] -TMPDIR[dir] -USER[filename] -USERLOG[filename]");
    // -PORT[portnum]:
    //      portnum: The command port, if blank, use the defaults
    //      default: 6666
    // -DEBUG:
    //      Write debug information to console
    //      default: No Debug information is written
    // -LOG[filename]:
    //      filename: The log file name, if blank, use the default name 'jspop-defaults.log'
    //      default: No log information is written
    // -ROOTDIR[dir]:
    //      dir: The root directory of DEVise session files, if blank, use defaults
    //      default: current Directory
    // -MAXSERVER[number]:
    //      number: The maximum number of active DEVise server, if blank, use defualts
    //      default: 5
    // -TIMEOUT[time]:
    //      time: The time out value in seconds for idle DEVise Server, if blank, use defaults
    //      default: 0, which means no timeout
    // -TMPDIR[dir]:
    //      dir: The directory for store temporary files
    //      default: current Directory
    // -USER[filename]:
    //      filename: The name of the file that contain user information
    //      default: users.cfg
    // -USERLOG[filename]:
    //      filename: The name of the file that store user log information
    //      default: users.log
    // -WAITTIME[time]:
    //      time: The time wait for DEVise Server started before abort,
    //            in seconds, but will be at least 15 seconds
    //      default: 15
    //
    private String logfilename = new String("jspop-defaults.log");
    private int cmdPort = DEViseGlobals.DEFAULTCMDPORT;
    private File rootDir = null;
    private File tmpDir = null;
    private File usercfgFile = null;
    private File userlogFile = null;
    private int maxServer = 5;
    private long serverTimeout = 0;
    private long serverWaitTimeout = 15;
    private int IDCount = 1;

    private Vector users = new Vector();
    private Vector activeClients = new Vector();
    private Vector suspendClients = new Vector();
    private Vector activeServers = new Vector();

    private ServerSocket serverSocket = null;

    public jspop(String[] args)
    {
        checkArguments(args);

        try {
            serverSocket = new ServerSocket(cmdPort);
        } catch (IOException e) {
            System.out.println("Can not start server sokcet at port " + cmdPort);
            System.exit(1);
        }

        if (YGlobals.ISLOG && logfilename != null) {
            try {
                YGlobals.LogFile = new YLogFile(logfilename);
            } catch (YException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
        } else {
            // create a YLogFile class that does nothing when its methods called
            YGlobals.LogFile = new YLogFile();
        }

        if (YGlobals.ISDEBUG) {
            YGlobals.DebugInfo = new YDebug(YGlobals.ISGUI);
        } else {
            // create a YDebug class that does nothing when its methods called
            YGlobals.DebugInfo = new YDebug();
        }

        DEViseUser user = new DEViseUser(DEViseGlobals.DEFAULTUSER, DEViseGlobals.DEFAULTPASS, 0, 30);
        users.addElement(user);

        checkFiles();

        for (int i = 0; i < maxServer; i++) {
            DEViseServer newserver = null;
            try {
                newserver = new DEViseServer(0, 15);
            } catch (YException e) {
                System.out.println("Can not start Server!");
                System.exit(1);
            }
            activeServers.addElement(newserver);
        }

    }

    public void run()
    {
        boolean isListen = true;

        System.out.println("\nDEVise POP Server started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = serverSocket.accept();
                String name = socket.getInetAddress().getHostName();
                System.out.println("Client connection request from " + name + " is received.");
            }  catch (IOException e)  {
                System.out.println("DEVise POP Server can not listen on port " + cmdPort + "\nDEVise POP CMD Server is aborted!");
                isListen = false;
            }

            startClient(socket);
        }
        System.out.println("\nDEVise POP Server quit ...");
    }

    public synchronized String getStat()
    {
        String newstr = "" + maxServer + " " + activeServers.size();
        return newstr;
    }

    private void startClient(Socket socket)
    {
        try {
            new DEViseClient(this, socket);
        } catch (IOException e) {
            String name = null;
            try {
                name = socket.getInetAddress().getHostName();
                socket.close();
            } catch (IOException e1) {
            }

            YGlobals.printMsg("Can not start new client for " + name);
        }
    }

    public synchronized void removeClient(DEViseClient client)
    {
        if (client == null)
            return;

        if (activeClients.contains(client)) {
            activeClients.removeElement(client);
        }
    }

    public synchronized void suspendClient(DEViseClient client)
    {
        if (client == null)
            return;

        if (activeClients.contains(client)) {
            activeClients.removeElement(client);
        }

        if (!suspendClients.contains(client)) {
            suspendClients.addElement(client);
        }
    }

    public synchronized DEViseUser checkUser(String name, String pass)
    {
        DEViseUser newuser = null;

        for (int i = 0; i < users.size(); i++) {
            DEViseUser tmpuser = (DEViseUser)users.elementAt(i);
            if (name.equals(tmpuser.username) && pass.equals(tmpuser.password)) {
                newuser = tmpuser;
                break;
            }
        }

        return newuser;
    }

    public synchronized boolean checkHost(DEViseUser user, String hostname)
    {
        boolean flag = true;

        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient newclient = (DEViseClient)activeClients.elementAt(i);
            if (newclient.hostname.equals(hostname)) {
                if (newclient.user.equals(user) && user.priority != 2) {
                    flag = false;
                    break;
                }
            }
        }

        return flag;
    }

    public synchronized int checkID(DEViseClient client, int id)
    {
        int newid = DEViseGlobals.ERRORID;

        if (id != DEViseGlobals.ERRORID) {
            boolean flag = true;

            for (int i = 0; i < suspendClients.size(); i++) {
                DEViseClient oldclient = (DEViseClient)suspendClients.elementAt(i);
                if (id == oldclient.connectID && client.user.equals(oldclient.user)) {
                    newid = id;
                    client.lastSavedSession = oldclient.lastSavedSession;
                    break;
                }
            }
        } else {
            newid = getID();
        }

        return newid;
    }

    private synchronized int getID()
    {
        boolean flag = false;

        while (!flag) {
            if(IDCount == Integer.MAX_VALUE)
                IDCount = 1;

            flag = true;

            for (int i = 0; i < activeClients.size(); i++) {
                DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
                if (IDCount == client.connectID) {
                    flag = false;
                    IDCount++;
                    break;
                }
            }

            if (!flag) {
                for (int i = 0; i < activeClients.size(); i++) {
                    DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
                    if (IDCount == client.connectID) {
                        flag = false;
                        IDCount++;
                        break;
                    }
                }
            }
        }

        return IDCount++;
    }

    public synchronized boolean startServer(DEViseClient client)
    {
        if (activeClients.size() < activeServers.size()) {
            for (int i = 0; i < activeServers.size(); i++) {
                DEViseServer oldserver = (DEViseServer)activeServers.elementAt(i);
                if (oldserver.getStatus()) {
                    if (oldserver.isAvailable) {
                        client.deviseImgPort = oldserver.imgPort;
                        client.deviseCmdPort = oldserver.cmdPort;
                        client.server = oldserver;
                        oldserver.isAvailable = false;

                        return true;
                    }
                } else {
                    activeServers.removeElement(oldserver);
                }
            }
        }

        if (activeServers.size() < maxServer || client.user.priority == 2) {
            DEViseServer newserver = null;

            try {
                newserver = new DEViseServer(serverTimeout, serverWaitTimeout);
            } catch (YException e) {
                YGlobals.printMsg(e.getMessage());
                return false;
            }

            client.deviseImgPort = newserver.imgPort;
            client.deviseCmdPort = newserver.cmdPort;
            client.server = newserver;
            newserver.isAvailable = false;
            activeServers.addElement(newserver);

            return true;
        } else {
            return false;
        }
    }

    private void checkArguments(String[] args)
    {
        String userlogfile = new String("users.log");
        String usercfgfile = new String("users.cfg");
        String rdir = new String(".");
        String tdir = new String(".");

        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-ROOTDIR")) {
                if (!args[i].substring(8).equals("")) {
                    rdir = args[i].substring(8);
                }
            } else if (args[i].startsWith("-PORT")) {
                if (!args[i].substring(5).equals("")) {
                    try {
                        cmdPort = Integer.parseInt(args[i].substring(5));
                        if (cmdPort < 1 || cmdPort > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid command port number " + args[i].substring(5) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-MAXSERVER")) {
                if (!args[i].substring(10).equals("")) {
                    try {
                        maxServer = Integer.parseInt(args[i].substring(10));
                        if (maxServer < 1 || maxServer > 25)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid number of server " + args[i].substring(10) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-TIMEOUT")) {
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
            } else if (args[i].startsWith("-WAITTIME")) {
                if (!args[i].substring(9).equals("")) {
                    try {
                        serverWaitTimeout = Long.parseLong(args[i].substring(9));
                        if (serverWaitTimeout < 30)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid timeout value of server " + args[i].substring(10) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-DEBUG")) {
                YGlobals.ISDEBUG = true;
            } else if (args[i].startsWith("-LOG")) {
                YGlobals.ISLOG = true;
                if (!args[i].substring(4).equals(""))
                    logfilename = args[i].substring(4);
            } else if (args[i].startsWith("-USER")) {
                if (!args[i].substring(5).equals(""))
                     usercfgfile = args[i].substring(5);
            } else if (args[i].startsWith("-USERLOG")) {
                if (!args[i].substring(8).equals(""))
                     userlogfile = args[i].substring(8);
            } else if (args[i].startsWith("-TMPDIR")) {
                if (!args[i].substring(7).equals(""))
                     tdir = args[i].substring(7);
            } else {
                System.out.println("Invalid jspop option " + args[i] + " is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }

        /*
        rootDir = new File(rdir);
        if (!rootDir.exists()) {
            System.out.println("Specified session root directory " + rdir + " is not exist!");
            System.exit(1);
        }
        if (!rootDir.isDirectory()) {
            System.out.println("Specified session root directory " + rdir + " is not a directory!");
            System.exit(1);
        }
        if (!rootDir.canRead()) {
            System.out.println("Specified session root directory " + rdir + " is not readable!");
            System.exit(1);
        }
        */

        tmpDir = new File(tdir);
        if (!tmpDir.exists()) {
            System.out.println("Specified temporary directory " + tdir + " is not exist!");
            System.exit(1);
        }
        if (!tmpDir.isDirectory()) {
            System.out.println("Specified temporary directory " + tdir + " is not a directory!");
            System.exit(1);
        }
        if (!tmpDir.canRead() || !tmpDir.canWrite()) {
            System.out.println("Specified temporary directory " + tdir + " is not readable or writable!");
            System.exit(1);
        }

        usercfgFile = new File(usercfgfile);
        if (!usercfgFile.exists()) {
            System.out.println("Specified user configuration file " + usercfgfile + " is not exist!");
            System.exit(1);
        }
        if (!usercfgFile.isFile()) {
            System.out.println("Specified user configuration file " + usercfgfile + " is not a file!");
            System.exit(1);
        }
        if (!usercfgFile.canRead() || !usercfgFile.canWrite()) {
            System.out.println("Specified user configuration file " + usercfgfile + " is not readable or writable!");
            System.exit(1);
        }

        /*
        userlogFile = new File(userlogfile);
        if (!userlogFile.exists()) {
            System.out.println("Specified user configuration file " + userlogfile + " is not exist!");
            System.exit(1);
        }
        if (!userlogFile.isFile()) {
            System.out.println("Specified user configuration file " + userlogfile + " is not a file!");
            System.exit(1);
        }
        if (!userlogFile.canRead() || !userlogFile.canWrite()) {
            System.out.println("Specified user configuration file " + userlogfile + " is not readable or writable!");
            System.exit(1);
        }
        */
    }

    private void checkFiles()
    {
        try {
            RandomAccessFile file = new RandomAccessFile(usercfgFile, "r");
            String str = file.readLine();
            while (str != null) {
                str = str.trim();
                if (!str.equals("")) {
                    String[] line = YGlobals.parseStr(str, ":");
                    if (line.length != 4)
                        throw new YException("Incorrect format of user information: " + str);

                    try {
                        DEViseUser user = new DEViseUser(line[0], line[1], Integer.parseInt(line[2]), Long.parseLong(line[3]));
                        users.addElement(user);
                    } catch (NumberFormatException e) {
                        throw new YException("Incorrect value of user information: " + line[2] + " " + line[3]);
                    }
                }
                str = file.readLine();
            }
        } catch (IOException e) {
            System.out.println("Can not read from file " + usercfgFile.getName());
            System.exit(1);
        } catch (YException e) {
            System.out.println(e.getMessage());
            System.exit(1);
        }
    }

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

        YGlobals.ISAPPLET = false;
        YGlobals.ISGUI = false;

        // force java VM to run every object's finalizer on normal or abnormal exit
        System.runFinalizersOnExit(true);

        jspop popServer = new jspop(args);
        Thread popThread = new Thread(popServer);
        popThread.start();
    }
}
