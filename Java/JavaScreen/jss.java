// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jss implements Runnable
{
    private String usage = new String(
        "Usage: java JavaScreen.jss [options]\n" +
        "  -id[string]: ID for ps (default: none)\n" +
        "  -server[number]: number of servers to start (default: 1)\n" +
        "  -devisescript[filename]: script to use to start servers\n" +
        "    (default: DEVise.jspop)\n" +
        "  -debug[number]: debug output level (default: 0)\n" +
        "  -jssport[number]: port jss listens on (default: " +
        DEViseGlobals.JSSPORT + ")\n" +
        "  -jspopport[number]: port to use to connect to jspop (default: " +
        DEViseGlobals.JSPOPPORT + ")\n" +
        "  -jspophost[string]: host jspop is running on (default: " +
        DEViseGlobals.JSPOPHOST + ")\n" +
        "  -quit: tell another jss to quit (default: no)\n" +
        "  -usage: print this message");

    private jss jssServer = null;
    private String localHostname = null;
    private ServerSocket jssServerSocket = null;
    private int jssPort = DEViseGlobals.JSSPORT;

    private int jspopPort = DEViseGlobals.JSPOPPORT;
    private String jspopHost = DEViseGlobals.JSPOPHOST;
    private InetAddress jspopAddress = null;
    private Socket jspopSocket = null;
    private DataOutputStream jspopOS = null;
    private DataInputStream jspopIS = null;

    private String idStr = null;
    private String devisedScript = "DEVise.jspop";

    private int devisedNumber = 1;
    private int debugLevel = 0;
    private Vector deviseds = new Vector();

    public static void main(String[] args) throws YException
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.err.println("Error: Java version 1.1 or greater is " +
                               "needed to run this program\n" + "The version you used is " +
                               version + "\n");
            System.exit(0);
        }

        jss server = new jss(args);
        Thread thread = new Thread(server);
        thread.setName("JSS server");
        thread.start();
    }

    public jss(String[] args) throws YException
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);

        try {
            InetAddress address = InetAddress.getLocalHost();
            localHostname = address.getHostName();
        } catch (UnknownHostException e) {
            throw new YException("Can not start jss - unknown local host! (" +
                                 e + ")");
        }

        System.out.println("\nTry to start jss server socket at port" +
                           jssPort + " ...\n");
        try {
            jssServerSocket = new ServerSocket(jssPort);
        } catch (IOException e) {
            System.err.println("Can not start server socket at port " +
                               jssPort + "(" + e + ")");
            quit();
        }

        System.out.println("Trying to start " + devisedNumber +
                           " devised server ...\n");
        try {
            devised newserver = null;
            for (int i = 0; i < devisedNumber; i++) {
                newserver = new devised(idStr, devisedScript);
                deviseds.addElement(newserver);
                System.out.println("Successfully start devised no." +
                                   (i + 1) + " out of " + devisedNumber + "\n");
            }
            System.out.println("  Successfully started DEVise server(s)");
        } catch (YException e) {
            System.err.println("Error starting devised: " + e);
            quit();
        }

        jssServer = this;
    }

    public synchronized void quit()
    {
        System.out.println("Start closing jss server ...");

        disconnectFromJSPOP();

        System.out.println("Try to close existing devised server ...");
        while (deviseds.size() > 0) {
            devised server = (devised)deviseds.elementAt(0);
            if (server != null) {
                server.stop();
                server = null;
            }
            deviseds.removeElementAt(0);
        }

        System.out.println("Try to close jss server socket ...");
        if (jssServerSocket != null) {
            try {
                jssServerSocket.close();
            } catch (IOException e) {
            }
            jssServerSocket = null;
        }

        System.out.println("Quit jss ...");
        System.exit(0);
    }

    public void run()
    {
        System.out.println("\njss server started ...\n");

        boolean isListen = true;
        String hostname = null;

        System.out.println("\nTry to connect to jspop server " + jspopHost + " at port " + jspopPort + " ...\n");
        try {
            for (int i = 0; i < deviseds.size(); i++) {
                connectToJSPOP();
                devised server = (devised)deviseds.elementAt(i);
                sendToJSPOP(DEViseCommands.S_ADD + " " + jssPort + " " +
                            server.cmdPort);
                disconnectFromJSPOP();
            }
        } catch (YException e) {
            System.err.println("Error connecting to JSPoP: " + e);
            isListen = false;
        }

        disconnectFromJSPOP();

        while (isListen) {
            Socket socket = null;
            try {
                socket = jssServerSocket.accept();
                InetAddress addr = socket.getInetAddress();
                hostname = addr.getHostName();
                if (addr.getHostAddress().equals(jspopAddress.getHostAddress())) {
                    System.out.println("Connection request from " + hostname + " is accepted ...");
                    try {
                        jspopIS = new DataInputStream(socket.getInputStream());
                        String msg = receiveFromJSPOP();

                        boolean isQuit = false;
                        try {
                            isQuit = processRequest(msg);
                        } catch (YException exp) {
                            System.err.println("Failed to process request " +
                                               "from jspop (" + exp + ")");
                            disconnectFromJSPOP();
                        }

                        isListen = !isQuit;
                    } catch (IOException ex) {
                        System.err.println("IO Error (" + ex +
                                           ") while open connection to jspop host " +
                                           jspopHost);
                    } catch (YException ex) {
                        System.err.println(
                            "Error while opening JSPoP connection: " + ex);
                    }

                    if (jspopIS != null) {
                        try {
                            jspopIS.close();
                        } catch (IOException ex) {
                        }
                        jspopIS = null;
                    }

                    if (socket != null) {
                        try {
                            socket.close();
                        } catch (IOException ex) {
                        }
                        socket = null;
                    }
                } else {
                    System.out.println("JSPOP Host is " + jspopAddress.getHostName() + "\nConnection request from " + hostname + " is rejected ...");
                    try {
                        socket.close();
                    } catch (IOException exception) {
                    }
                }
            } catch (IOException e) {
                System.err.println("jss server cannot listen on jss " +
                                   "socket so it is aborting! (" + e + ")");
                break;
            }
        }

        quit();
    }

    private void disconnectFromJSPOP()
    {
        if (jspopOS != null) {
            try {
                jspopOS.close();
            } catch (IOException e) {
            }
            jspopOS = null;
        }

        if (jspopSocket != null) {
            try {
                jspopSocket.close();
            } catch (IOException e) {
            }
            jspopSocket = null;
        }
    }

    private void connectToJSPOP() throws YException
    {
        disconnectFromJSPOP();

        try {
            if (jspopAddress == null) {
                jspopAddress = InetAddress.getByName(jspopHost);
            }

            jspopSocket = new Socket(jspopAddress, jspopPort);
            jspopOS = new DataOutputStream(new BufferedOutputStream(
                                               jspopSocket.getOutputStream()));
        } catch (UnknownHostException e) {
            throw new YException("Can not find jspop host " + jspopHost +
                                 "(" + e + ")");
        } catch (NoRouteToHostException e) {
            throw new YException("Can not find route to jspop host, "
                                 + "may caused by an internal firewall (" + e + ")");
        } catch (IOException e) {
            throw new YException("IO Error (" + e +
                                 ") while connecting to jspop host " + jspopHost);
        }
    }

    private void sendToJSPOP(String msg) throws YException
    {
        if (jspopOS == null) {
            throw new YException("Null jspop output stream!", "jss:sendToJSPOP()");
        }

        if (msg == null) {
            throw new YException("Null message!", "jss:sendToJSPOP()");
        }

        try {
            System.out.println("Try to send msg \"" + msg + "\" to JSPOP server ...");
            jspopOS.writeInt(msg.length());
            jspopOS.writeBytes(msg);
            jspopOS.flush();
            System.out.println("Message successfully sent!");
        } catch (IOException e) {
            throw new YException("IO Error while send message to JSPOP", "jss:sendToJSPOP()");
        }
    }

    private String receiveFromJSPOP() throws YException
    {
        if (jspopIS == null) {
            throw new YException("Null jspop input stream!", "jss:receiveFromJSPOP()");
        }

        try {
            System.out.println("Try to receive data from JSPOP server ...");
            int length = jspopIS.readInt();
            byte[] data = new byte[length];
            jspopIS.readFully(data);
            String msg = new String(data);
            System.out.println("Message \"" + msg + "\" successfully received from JSPOP!");
            return msg;
        } catch (IOException e) {
            throw new YException("IO Error while receive message from JSPOP", "jss:receiveFromJSPOP()");
        }
    }

    // Returns true iff the JSS should quit.
    private synchronized boolean processRequest(String msg) throws YException
    {
        if (msg == null) {
            return false;
        }

        String[] cmd = DEViseGlobals.parseString(msg);
        if (cmd == null) {
            System.err.println("Can't parse request received from jspop: <"
                               + msg + ">");
            return false;
        }

        if (cmd[0].equals(DEViseCommands.S_RESTART)) {
            if (cmd.length == 2) {
                try {
                    int cmdport = Integer.parseInt(cmd[1]);
                    for (int i = 0; i < deviseds.size(); i++) {
                        devised server = (devised)deviseds.elementAt(i);
                        if (server.cmdPort == cmdport) {
                            server.stop();
                            deviseds.removeElement(server);

                            try {
                                server = new devised(idStr, devisedScript);
                            } catch (YException ex) {
                                System.err.println("Cannot start new devised");
                                server = null;
                            }

                            if (server != null) {
                                deviseds.addElement(server);
                                connectToJSPOP();
                                //TEMP -- get rid of imgPort here!
                                sendToJSPOP(DEViseCommands.S_ADD + " " +
                                            jssPort + " " + server.cmdPort);
                                disconnectFromJSPOP();
                            }

                            break;
                        }
                    }
                } catch (NumberFormatException e) {
                    System.err.println("NumberFormatException: " +
                                       e.getMessage());
                    System.err.println("Invalid request received from jspop: <"
                                       + msg + ">");
                }
            } else {
                System.err.println("Wrong number of arguments in command: <"
                                   + msg + ">");
            }
        } else if (cmd[0].equals(DEViseCommands.S_QUIT)) {
            return true;
        } else {
            System.err.println("Can't recognize command received from jspop: <"
                               + msg + ">");
        }

        return false;
    }

    private void checkArguments(String[] args) throws YException
    {
        for (int i = 0; i < args.length; i++) {
            StringBuffer argValue = new StringBuffer();

            if (DEViseGlobals.checkArgument(args[i], "-quit", false,
                                            argValue)) {
                try {
                    Socket socket = new Socket("localhost", jssPort);
                    DataOutputStream os = new DataOutputStream(
                        new BufferedOutputStream(socket.getOutputStream()));
                    String msg = DEViseCommands.S_QUIT;
                    os.writeInt(msg.length());
                    os.writeBytes(msg);
                    os.flush();
                    os.close();
                    socket.close();
                } catch (UnknownHostException e) {
                    System.err.println("Cannot find local jss host");
                } catch (NoRouteToHostException e) {
                    System.err.println("Cannot find route to local jss host, may caused by an internal firewall");
                } catch (IOException e) {
                    System.err.println("IO Error while connecting to local jss host");
                }

                System.exit(0);

            } else if (DEViseGlobals.checkArgument(args[i], "-server", true,
                                                   argValue)) {
                try {
                    devisedNumber = Integer.parseInt(argValue.toString());
                    if (devisedNumber < 1 || devisedNumber > 100) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Please use a positive integer " +
                                         "between 1 and 100 as the number of devised servers " +
                                         "you want to started");
                }

            } else if (DEViseGlobals.checkArgument(args[i], "-jssport", true,
                                                   argValue)) {
                try {
                    jssPort = Integer.parseInt(argValue.toString());
                    if (jssPort < 1024 || jssPort > 65535) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Please use a positive integer " +
                                         "between 1024 and 65535 as the port number for " +
                                         "the JSS");
                }

            } else if (DEViseGlobals.checkArgument(args[i], "-jspopport",
                                                   true, argValue)) {
                try {
                    jspopPort = Integer.parseInt(argValue.toString());
                    if (jspopPort < 1024 || jspopPort > 65535) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Please use a positive integer " +
                                         "between 1024 and 65535 as the port number of jspop");
                }

            } else if (DEViseGlobals.checkArgument(args[i], "-debug", false,
                                                   argValue)) {
                if (!argValue.toString().equals("")) {
                    try {
                        debugLevel = Integer.parseInt(argValue.toString());
                    } catch (NumberFormatException e) {
                        throw new YException("Please use an integer as " +
                                             "the debug level!");
                    }
                } else {
                    debugLevel = 1;
                }

            } else if (DEViseGlobals.checkArgument(args[i], "-jspophost",
                                                   true, argValue)) {
                jspopHost = argValue.toString();
                System.out.println("JSPOP host is " + jspopHost);

            } else if (DEViseGlobals.checkArgument(args[i], "-id", true,
                                                   argValue)) {
                idStr = argValue.toString();

            } else if (DEViseGlobals.checkArgument(args[i], "-devisescript",
                                                   true, argValue)) {
                devisedScript = argValue.toString();

            } else if (DEViseGlobals.checkArgument(args[i], "-usage", false,
                                                   argValue)) {
                System.out.println(usage);
                System.exit(0);

            } else {
                throw new YException("Invalid jss option \"" + args[i] +
                                     "\"!\n" + usage);
            }
        }
    }
}



class devised
{
    public static final String devisedKill = "DEVise.kill";

    private Process process = null;
    public int cmdPort, imgPort, switchPort;

    public devised(String idStr, String startScript) throws YException
    {
        //System.out.println("I am in devised");
        start(idStr, startScript);
    }

    private synchronized void start(String idStr, String startScript) throws YException
    {
        // stop previous devised first if any
        //System.out.println("I am in devised-start");
        stop();

        // find two free ports for new devised
        ServerSocket socket1 = null, socket3 = null;
        try {
            //System.out.println("Try to find 2 available ports ... ");
            socket1 = new ServerSocket(0);
            socket3 = new ServerSocket(0);
            cmdPort = socket1.getLocalPort();
            // Note: -1 means that the devised shouldn't listen on an
            // image port.
            imgPort = -1;
            switchPort = socket3.getLocalPort();
        } catch (IOException e) {
            System.err.println(e.getMessage());
            throw new YException("Can not find any free port in local host to start a new devised");
        } finally {
            if (socket1 != null) {
                try {
                    socket1.close();
                } catch (IOException e) {
                    System.err.println("Cannot close socket at port " +
                                       cmdPort);
                }
            }
            if (socket3 != null) {
                try {
                    socket3.close();
                } catch (IOException e) {
                    System.err.println("Cannot close socket at port " +
                                       switchPort);
                }
            }
        }

        // start devised
        Runtime currentRT = Runtime.getRuntime();
        try {
            String execStr = startScript;
            if (idStr != null) execStr += " -id " + idStr;
            execStr += " -port " + cmdPort + " -imageport " + imgPort +
                       " -switchport " + switchPort;
            System.out.println("Try to start command \"" + execStr + "\" ...");
            process = currentRT.exec(execStr);
            System.out.println("  Sucessfully started DEVise server");
        } catch (IOException e) {
            throw new YException("IO Error (" + e.getMessage() +
                                 ") while trying to start a new devised at port " + cmdPort +
                                 "," + imgPort + "," + switchPort);
        } catch (SecurityException e) {
            throw new YException("Security Error (" + e.getMessage() +
                                 ") while trying to start a new devised at port " + cmdPort +
                                 "," + imgPort + "," + switchPort);
        }
    }

    public synchronized void stop()
    {
        //System.out.println("I am in devised-stop");
        if (process != null) {
            try {
                int v = process.exitValue();
            } catch (IllegalThreadStateException e) {
                process.destroy();
            }

            process = null;
        }

        Runtime currentRT = Runtime.getRuntime();
        Process kill = null;
        try {
            //System.out.println("I am start to kill");
            kill = currentRT.exec(devised.devisedKill + " -port " + cmdPort);
            //System.out.println("kill finished");
            boolean stillWorking = true;
            while (stillWorking) {
                try {
                    int v = kill.exitValue();
                    stillWorking = false;
                } catch (IllegalThreadStateException e) {
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException exception) {
                    }
                }
            }
        } catch (IOException e) {
            System.err.println("IO Error (" + e.getMessage() +
                               ") while trying to kill an old devised at port " + cmdPort +
                               "," + imgPort + "," + switchPort);
        } catch (SecurityException e) {
            System.err.println("Security Error (" + e.getMessage() +
                               ") while trying to kill an old devised at port " + cmdPort +
                               "," + imgPort + "," + switchPort);
        } finally {
            if (kill != null) kill.destroy();
            kill = null;
        }

        //System.out.println("leaving stop");
        cmdPort = 0;
        imgPort = 0;
        switchPort = 0;
    }
}
