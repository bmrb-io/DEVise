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


import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jss implements Runnable
{
    private String usage = new String("usage: java jss -server[number] -debug[number]");

    private jss jssServer = null;
    private String localHostname = null;
    private ServerSocket jssServerSocket = null;
    private int jssPort = DEViseGlobals.JSSPORT;
    private String jspopHost = DEViseGlobals.JSPOPHOST;

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
        System.out.println("\nTry to connect to jspop server ...\n");        
        
        
        boolean isListen = true;
        String hostname = null;
        while (isListen) {
            Socket socket = null;
            try {
                socket = jssServerSocket.accept();
                hostname = socket.getInetAddress().getHostName();
                if (hostname.equals(jspopHost)) {
                    System.out.println("Connection request from " + hostname + " is accepted ...");
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

            processRequest(socket);
        }

        quit();
    }

    private synchronized void processRequest(Socket socket)
    {
        if (socket == null) {
            return;
        }
    }

    private void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-server")) {
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
                    int v = process.exitValue();
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
