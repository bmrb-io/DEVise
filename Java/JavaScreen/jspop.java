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

// ========================================================================

// jspop.java

import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private String usage = new String("usage: java jspop -cmdport[port] -imgport[port] -server[number] -userfile[filename] -logfile[filename] -debug[number]");
    // -CMDPORT[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    // -IMGPORT[port]:
    //      port: The image port, if blank, use the defaults
    //      default: 6688
    // -SERVER[number]:
    //      number: The maximum number of active DEVise server, if blank, use defualts
    //      default: 1
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

    private ServerSocket cmdServerSocket = null;
    private ServerSocket dataServerSocket = null;

    private String userFileName = new String("users.cfg");
    private String logFileName = new String("clients.log");
    private int logLevel = 0;
    private YLogFile logFile = null;
    private int debugLevel = 0;
    private YLogConsole debugConsole = null;
    
    private String localHostname = null;
    private int IDCount = 1;
    private int maxServerNumber = 1;
    private Thread popThread = null;
    private DEViseClientDispatcher dispatcher = null;

    private Hashtable users = new Hashtable();
    private Vector servers = new Vector();
    private Vector clientIDs = new Vector();
    private Vector suspendClients = new Vector();
    private Vector activeClients = new Vector();

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

        jspop popServer = new jspop(args);
        popServer.start();
    }

    public void pn(String msg, int level)
    {
        if (debugLevel > 0) {
            debugConsole.pn(msg, level);
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (debugLevel > 0) {
            debugConsole.p(msg, level);
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void logpn(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.pn(msg, level, true);
        }
    }

    public void logpn(String msg)
    {
        logpn(msg, 1);
    }

    public void logp(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.p(msg, level, true);
        }
    }

    public void logp(String msg)
    {
        logp(msg, 1);
    }

    public jspop(String[] args)
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);
        
        try {
            InetAddress address = InetAddress.getLocalHost();
            localHostname = address.getHostName();
        } catch (UnknownHostException e) {
            System.out.println("Can not start jspop - unknown local host!");
            System.exit(1);
        }
                
        System.out.println("Starting DEViseServer ...\n");
        try {
            DEViseServer newserver = null;
            for (int i = 0; i < maxServerNumber; i++) {
                newserver = new DEViseServer(this, localHostname);
                newserver.start();
                servers.addElement(newserver);
                System.out.println("Successfully start DEViseServer " + (i + 1) + " out of " + maxServerNumber + "\n");
            }
        } catch (YException e) {
            System.out.println(e.getMsg());
            quit(1);
        }

        System.out.println("\nStarting command server socket on " + DEViseGlobals.cmdport + " ...\n");
        try {
            cmdServerSocket = new ServerSocket(DEViseGlobals.cmdport);
        } catch (IOException e) {
            System.out.println("Can not start command server sokcet at port " + DEViseGlobals.cmdport);
            quit(1);
        }

        System.out.println("\nStarting data server socket on " + DEViseGlobals.imgport + " ...\n");
        try {
            dataServerSocket = new ServerSocket(DEViseGlobals.imgport);
            dataServerSocket.setSoTimeout(5000); // wait for data socket connection for 5 seconds before disconnect
        } catch (IOException e) {
            System.out.println("Can not start data server socket at port " + DEViseGlobals.imgport);
            quit(1);
        }

        System.out.println("Starting client dispatcher ...\n");
        dispatcher = new DEViseClientDispatcher(this);
        dispatcher.start();
    }

    public void start()
    {
        popThread = new Thread(this);
        popThread.start();
    }

    // quit() will only be called in jspop server thread or in jspop() initialization
    // and all the stop() or close() methods of other class are synchronized, so there
    // is no need to synchronize quit()
    private void quit(int id)
    {
        System.out.println("Stop jspop server thread...");
        if (popThread != null && Thread.currentThread() != popThread) {
            popThread.stop();
            popThread = null;
        }

        System.out.println("Stop client dispatcher thread...");
        if (dispatcher != null) {
            dispatcher.stop();
            dispatcher = null;
        }

        System.out.println("Stop command server socket...");
        if (cmdServerSocket != null) {
            try {
                cmdServerSocket.close();
            } catch (IOException e) {
            }

            cmdServerSocket = null;
        }

        System.out.println("Stop data server socket...");
        if (dataServerSocket != null) {
            try {
                dataServerSocket.close();
            } catch (IOException e) {
            }

            dataServerSocket = null;
        }

        System.out.println("Stop DEViseServer thread...");
        for (int i = 0; i < servers.size(); i++) {
            DEViseServer server = (DEViseServer)servers.elementAt(i);
            if (server != null) {
                server.stop();
            }
        }
        servers.removeAllElements();

        System.out.println("Close clients...");
        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        suspendClients.removeAllElements();
        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient client = (DEViseClient)activeClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        activeClients.removeAllElements();

        System.out.println("Close log file...");
        if (logFile != null) {
            logFile.close();
            logFile = null;
        }

        System.exit(id);
    }

    public void run()
    {
        System.out.println("\nJSPOP Server started ...\n");

        boolean isListen = true;
        boolean isTimeout = false;
        int quitID = 0;

        Socket socket1 = null;
        Socket socket2 = null;
        DEViseCommSocket socket = null;
        String hostname = null;

        while (isListen) {
            try {
                socket1 = cmdServerSocket.accept();
                hostname = socket1.getInetAddress().getHostName();
                pn("Client connection request from " + hostname + " is received ...");
            } catch (IOException e) {
                System.out.println("JSPOP server can not listen on command socket so it is aborting!");
                quitID = 1;
                break;
            }

            // There may be some serious mismatch could happen here, i.e. the data socket
            // and the command socket are not from the same clients, however, since I am
            // prepare to move to single socket, so I just leave it here, otherwise, should
            // figure out some protocol to prevent that
            try {
                socket2 = dataServerSocket.accept();
                isTimeout = false;
            } catch (InterruptedIOException e) {
                isTimeout = true;
            } catch (IOException e) {
                System.out.println("JSPOP server can not listen on data socket so it is aborting!");
                try {
                    socket1.close();
                } catch (IOException e1) {
                }

                quitID = 1;
                break;
            }

            if (isTimeout) {
                pn("Can not receive data socket connection request within timeout\nClose connection to client from \"" + hostname + "\"");
                try {
                    socket1.close();
                } catch (IOException e) {
                }
            } else {
                try {
                    socket = new DEViseCommSocket(socket1, socket2, 1000);
                    DEViseClient client = new DEViseClient(this, hostname, socket, getID());
                    addClient(client);
                } catch (YException e) {
                    pn(e.getMsg() + "\nClose socket connection to client \"" + hostname + "\"");
                }
            }
        }

        quit(quitID);
    }

    public DEViseUser getUser(String username, String password)
    {
        if (username == null || password == null) {
            return null;
        }

        if (users.containsKey(username)) {
            DEViseUser user = (DEViseUser)users.get(username);
            if (user != null) {
                if (password.equals(user.getPassword())) {
                    return user;
                }
            }

            return null;
        }

        return null;
    }

    public synchronized void addClient(DEViseClient client)
    {
        if (client != null) {
            pn("Client from " + client.getHostname() + " is added ...");
            client.setSuspend();
            suspendClients.addElement(client);
        }
    }

    // this method will only be called in client dispatcher thread
    // this method will also check which client need to be removed and remove it
    public synchronized DEViseClient getNextRequestingClient()
    {
        double time = -1.0, clientTime = 0.0;
        DEViseClient client = null;
        Vector removedClient = new Vector();

        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient newclient = (DEViseClient)suspendClients.elementAt(i);
            if (newclient != null) {
                int status = newclient.getStatus();
                if (status == DEViseClient.CLOSE) { // this client need to be removed
                    removedClient.addElement(newclient);
                    continue;
                } else if (status == DEViseClient.REQUEST) { // only client that are requesting service will be served
                    clientTime = (double)(newclient.getPriority() * newclient.getSuspendTime());

                    if (time < 0.0) {
                        time = clientTime;
                        client = newclient;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            client = newclient;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < removedClient.size(); i++) {
            DEViseClient newclient = (DEViseClient)removedClient.elementAt(i);
            if (newclient != null) {
                Integer id = newclient.getConnectionID();
                if (id != null) {
                    clientIDs.removeElement(id);
                }
                newclient.close();
                suspendClients.removeElement(newclient);
            }
        }
        removedClient.removeAllElements();

        return client;
    }

    public synchronized void activeClient(DEViseClient c)
    {
        if (c != null) {
            suspendClients.removeElement(c);
            activeClients.addElement(c);
            c.setActive();
        }
    }

    public synchronized void suspendClient(DEViseClient c)
    {
        if (c != null) {
            activeClients.removeElement(c);
            suspendClients.addElement(c);
            c.setSuspend();
        }
    }

    public DEViseServer getNextAvailableServer()
    {
        DEViseServer server = null;
        DEViseClient client = null;
        double time = -1.0, clientTime = 0.0;

        for (int i = 0; i < servers.size(); i++) {
            DEViseServer newserver = (DEViseServer)servers.elementAt(i);
            if (newserver != null) {
                if (newserver.getStatus() == 0) {
                    try {
                        newserver.start();
                    } catch (YException e) {
                        pn(e.getMsg());
                        continue;
                    }
                } else {
                    if (!newserver.isAvailable()) {
                        continue;
                    }

                    client = (DEViseClient)newserver.getCurrentClient();
                    if (client == null) { // this server is idle
                        return newserver;
                    }

                    clientTime = (double)client.getActiveTime() / (double)client.getPriority();
                    if (time < 0.0) {
                        time = clientTime;
                        server = newserver;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            server = newserver;
                        }
                    }
                }
            }
        }

        return server;
    }

    public synchronized String getServerState()
    {
        String state = "{";
        DEViseServer server = null;
        DEViseClient client = null;

        state = state + servers.size() + " ";
        for (int i = 0; i < servers.size(); i++) {
            server = (DEViseServer)servers.elementAt(i);
            if (server == null) {
                state = state + "null ";
            } else {
                state = state + server.hostname + " ";
            }
        }

        state = state + activeClients.size() + " ";
        for (int i = 0; i < activeClients.size(); i++) {
            client = (DEViseClient)activeClients.elementAt(i);
            if (client == null) {
                state = state + "null ";
            } else {
                state = state + client.getHostname() + " ";
            }
        }

        state = state + suspendClients.size() + " ";
        for (int i = 0; i < suspendClients.size(); i++) {
            client = (DEViseClient)suspendClients.elementAt(i);
            if (client == null) {
                state = state + "null ";
            } else {
                state = state + client.getHostname() + " ";
            }
        }

        state = state + "}";
        return state;
    }

    private synchronized Integer getID()
    {
        if (IDCount == Integer.MAX_VALUE) {
            IDCount = 1;
        }

        Integer id = new Integer(IDCount);
        while (clientIDs.contains(id)) {
            IDCount++;
            if (IDCount == Integer.MAX_VALUE)
                IDCount = 1;
            id = new Integer(IDCount);
        }

        clientIDs.addElement(id);
        return id;
    }


    private void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-cmdport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.cmdport = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-imgport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.imgport = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-server")) {
                if (!args[i].substring(7).equals("")) {
                    try {
                        maxServerNumber = Integer.parseInt(args[i].substring(7));
                        if (maxServerNumber < 1 || maxServerNumber > 10)
                            throw new NumberFormatException();
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1 and 10 as the maximum number of servers");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        debugLevel = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Please use an integer number as the debug level");
                        System.exit(1);
                    }
                } else {
                    debugLevel = 1;
                }
            } else if (args[i].startsWith("-logfile")) {
                if (!args[i].substring(8).equals("")) {
                     logFileName = args[i].substring(8);
                }
            } else if (args[i].startsWith("-userfile")) {
                if (!args[i].substring(9).equals("")) {
                     userFileName = args[i].substring(9);
                }
            } else {
                System.out.println("Invalid jspop option \"" + args[i] + "\"is given");
                System.out.println(usage);
                System.exit(1);
            }
        }

        logLevel = 1;

        if (DEViseGlobals.cmdport < 1024) {
            DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        }

        if (DEViseGlobals.imgport < 1024) {
            DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
        }

        if (logLevel > 0) {
            logFile = new YLogFile(logFileName, logLevel, true);
        }

        if (debugLevel > 0) {
            debugConsole = new YLogConsole(debugLevel);
        }

        System.out.println("Loading user configuration file ...");
        readCFGFile(userFileName);
    }

    private void readCFGFile(String filename)
    {
        RandomAccessFile uf = null;
        try {
            uf = new RandomAccessFile(filename, "r");
        } catch (IOException e) {
            System.out.println("Invalid user configuration file \"" + filename + "\"");
            System.exit(1);
        }

        try {
            String str = uf.readLine();
            while (str != null) {
                str = str.trim();
                // skip comment line
                if (!str.startsWith("#") && !str.equals("")) {
                    String[] line = DEViseGlobals.parseStr(str, ":");
                    DEViseUser user = new DEViseUser(line);
                    users.put(user.getName(), user);
                }

                str = uf.readLine();
            }
        } catch (IOException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println("Can not read from user configuration file \"" + filename + "\"");
            System.exit(1);
        } catch (YException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println(e.getMsg());
            System.exit(1);
        }

        try {
            uf.close();
        } catch (IOException e) {
        }
    }
}
