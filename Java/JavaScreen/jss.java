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
// Revision 1.5  2000/02/15 20:50:43  hongyu
// *** empty log message ***
//
// Revision 1.4  2000/02/14 09:26:33  hongyu
// *** empty log message ***
//
// Revision 1.3  2000/01/19 20:41:13  hongyu
// *** empty log message ***
//
// Revision 1.2  2000/01/17 07:48:32  hongyu
// *** empty log message ***
//
// Revision 1.1  1999/12/07 23:24:27  hongyu
// *** empty log message ***
//

// ========================================================================


import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jss implements Runnable
{
    private String usage = new String("usage: java jss -server[number] -debug[number] -jssport[number] -jspopport[number] -jspophost[string]");

    private jss jssServer = null;
    private String localHostname = null;
    private ServerSocket jssServerSocket = null;
    private int jssPort = DEViseGlobals.JSSPORT;

    private int jspopPort = DEViseGlobals.JSPOPPORT;
    private String jspopHost = DEViseGlobals.JSPOPHOST;
    private Socket jspopSocket = null;
    private DataOutputStream jspopOS = null;
    private DataInputStream jspopIS = null;

    private int devisedNumber = 1;
    private int debugLevel = 0;
    private Vector deviseds = new Vector();

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

        jss server = new jss(args);
        Thread thread = new Thread(server);
        thread.start();
    }

    public jss(String[] args)
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);

        try {
            InetAddress address = InetAddress.getLocalHost();
            localHostname = address.getHostName();
        } catch (UnknownHostException e) {
            System.out.println("Can not start jss - unknown local host!");
            System.exit(1);
        }

        System.out.println("\nTry to start jss server socket at port" + jssPort + " ...\n");
        try {
            jssServerSocket = new ServerSocket(jssPort);
        } catch (IOException e) {
            System.out.println("Can not start server sokcet at port " + jssPort);
            quit();
        }

        System.out.println("Try to start " + devisedNumber + " devised server ...\n");
        try {
            devised newserver = null;
            for (int i = 0; i < devisedNumber; i++) {
                newserver = new devised();
                deviseds.addElement(newserver);
                System.out.println("Successfully start devised no." + (i + 1) + " out of " + devisedNumber + "\n");
            }
        } catch (YException e) {
            System.out.println(e.getMsg());
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
            connectToJSPOP();
            for (int i = 0; i < deviseds.size(); i++) {
                devised server = (devised)deviseds.elementAt(i);
                sendToJSPOP("JSS_Add " + jssPort + " " + server.cmdPort + " " + server.imgPort);
            }
        } catch (YException e) {
            System.out.println(e.getMessage());
            isListen = false;
        }

        disconnectFromJSPOP();

        while (isListen) {
            Socket socket = null;
            try {
                socket = jssServerSocket.accept();
                hostname = socket.getInetAddress().getHostName();
                if (hostname.equals(jspopHost)) {
                    System.out.println("Connection request from " + hostname + " is accepted ...");
                    try {
                        jspopIS = new DataInputStream(socket.getInputStream());
                        String msg = receiveFromJSPOP();

                        boolean isQuit = false;
                        try {
                            isQuit = processRequest(msg);
                        } catch (YException exp) {
                            System.out.println("Failed to process request from jspop");
                            System.out.println(exp.getMessage());
                            disconnectFromJSPOP();
                        }

                        isListen = !isQuit;
                    } catch (IOException ex) {
                        System.out.println("IO Error while open connection to jspop host " + jspopHost);
                    } catch (YException ex) {
                        System.out.println(ex.getMessage());
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
                    System.out.println("Connection request from " + hostname + " is rejected ...");
                    try {
                        socket.close();
                    } catch (IOException exception) {
                    }
                }
            } catch (IOException e) {
                System.out.println("jss server can not listen on jss socket so it is aborting!");
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
            jspopSocket = new Socket(jspopHost, jspopPort);
            jspopOS = new DataOutputStream(new BufferedOutputStream(jspopSocket.getOutputStream()));
        } catch (UnknownHostException e) {
            throw new YException("Can not find jspop host " + jspopHost);
        } catch (NoRouteToHostException e) {
            throw new YException("Can not find route to jspop host, may caused by an internal firewall");
        } catch (IOException e) {
            throw new YException("IO Error while connecting to jspop host " + jspopHost);
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
            System.out.println("Message successfully sended!");
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

    private synchronized boolean processRequest(String msg) throws YException
    {
        if (msg == null) {
            return false;
        }

        String[] cmd = DEViseGlobals.parseString(msg);
        if (cmd == null) {
            System.out.println("Invalid request received from jspop \"" + msg + "\"");
            return false;
        }

        if (cmd[0].startsWith("JSS_Restart") && cmd.length == 3) {
            try {
                int cmdport = Integer.parseInt(cmd[1]);
                int imgport = Integer.parseInt(cmd[2]);
                for (int i = 0; i < deviseds.size(); i++) {
                    devised server = (devised)deviseds.elementAt(i);
                    if (server.cmdPort == cmdport && server.imgPort == imgport) {
                        server.stop();
                        deviseds.removeElement(server);

                        try {
                            server = new devised();
                        } catch (YException ex) {
                            System.out.println("Can not start new devised");
                            server = null;
                        }

                        if (server != null) {
                            deviseds.addElement(server);
                            connectToJSPOP();
                            sendToJSPOP("JSS_Add " + jssPort + " " + server.cmdPort + " " + server.imgPort);
                            disconnectFromJSPOP();
                        }

                        break;
                    }
                }
            } catch (NumberFormatException e) {
                System.out.println("Invalid request received from jspop \"" + msg + "\"");
            }
        } else if (cmd[0].startsWith("JSS_Quit")) {
            return true;
        } else {
            System.out.println("Invalid request received from jspop \"" + msg + "\"");
        }

        return false;
    }

    private void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-quit")) {
                try {
                    Socket socket = new Socket("localhost", jssPort);
                    DataOutputStream os = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
                    String msg = "JSS_Quit";
                    os.writeInt(msg.length());
                    os.writeBytes(msg);
                    os.flush();
                    os.close();
                    socket.close();
                } catch (UnknownHostException e) {
                    System.out.println("Can not find local jss host");
                } catch (NoRouteToHostException e) {
                    System.out.println("Can not find route to local jss host, may caused by an internal firewall");
                } catch (IOException e) {
                    System.out.println("IO Error while connecting to local jss host");
                }

                System.exit(0);
            } else if (args[i].startsWith("-server")) {
                if (!args[i].substring(7).equals("")) {
                    try {
                        devisedNumber = Integer.parseInt(args[i].substring(7));
                        if (devisedNumber < 1 || devisedNumber > 10) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1 and 10 as the number of devised servers you want to started");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-jssport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        jssPort = Integer.parseInt(args[i].substring(8));
                        if (jssPort < 1024 || jssPort > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number for JSS Server");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-jspopport")) {
                if (!args[i].substring(10).equals("")) {
                    try {
                        jspopPort = Integer.parseInt(args[i].substring(10));
                        if (jspopPort < 1024 || jspopPort > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number of jspop");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        debugLevel = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Please use an integer number as the debug level!");
                        System.exit(1);
                    }
                } else {
                    debugLevel = 1;
                }
            } else if (args[i].startsWith("-jspophost")) {
                if (!args[i].substring(10).equals("")) {
                    jspopHost = args[i].substring(10);
                }
            } else {
                System.out.println("Invalid jss option \"" + args[i] + "\"!");
                System.out.println(usage);
                System.exit(1);
            }
        }
    }
}



class devised
{
    public static String devisedExec = "DEVise.jspop", devisedKill = "DEVise.kill";

    private Process process = null;
    public int cmdPort, imgPort, switchPort;

    public devised() throws YException
    {
        start();
    }

    private synchronized void start() throws YException
    {
        // stop previous devised first if any
        stop();

        // find three free port for new devised
        ServerSocket socket1 = null, socket2 = null, socket3 = null;
        try {
            socket1 = new ServerSocket(0);
            socket2 = new ServerSocket(0);
            socket3 = new ServerSocket(0);
            cmdPort = socket1.getLocalPort();
            imgPort = socket2.getLocalPort();
            switchPort = socket3.getLocalPort();
        } catch (IOException e) {
            throw new YException("Can not find any free port in local host to start a new devised");
        } finally {
            if (socket1 != null) {
                try {
                    socket1.close();
                } catch (IOException e) {
                    System.out.println("Can not close socket at port " + cmdPort);
                }
            }
            if (socket2 != null) {
                try {
                    socket2.close();
                } catch (IOException e) {
                    System.out.println("Can not close socket at port " + imgPort);
                }
            }
            if (socket3 != null) {
                try {
                    socket3.close();
                } catch (IOException e) {
                    System.out.println("Can not close socket at port " + switchPort);
                }
            }
        }

        // start devised
        Runtime currentRT = Runtime.getRuntime();
        try {
            process = currentRT.exec(devised.devisedExec + " -port " + cmdPort + " -imageport " + imgPort + " -switchport " + switchPort);
        } catch (IOException e) {
            throw new YException("IO Error while trying to start a new devised at port " + cmdPort + "," + imgPort + "," + switchPort);
        } catch (SecurityException e) {
            throw new YException("Security Error while trying to start a new devised at port " + cmdPort + "," + imgPort + "," + switchPort);
        }
    }

    public synchronized void stop()
    {
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
            kill = currentRT.exec(devised.devisedKill + " " + cmdPort);

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
            System.out.println("IO Error while trying to kill an old devised at port " + cmdPort + "," + imgPort + "," + switchPort);
        } catch (SecurityException e) {
            System.out.println("Security Error while trying to kill an old devised at port " + cmdPort + "," + imgPort + "," + switchPort);
        } finally {
            kill.destroy();
            kill = null;
        }

        cmdPort = 0;
        imgPort = 0;
        switchPort = 0;
    }
}
