import  java.net.*;
import  java.io.*;
import  java.util.*;

public class DEVisePOPCmdServerSocket extends ServerSocket implements Runnable
{
    private boolean isListen = true;
    public final int maxClients = 5;
    private Vector clients = new Vector();
    private Vector deviseServer = new Vector();
    private int imgPort[] = new int[maxClients];
    private int cmdPort[] = new int[maxClients];
    private int dataPort[] = new int[maxClients];
    private int currentImgPort = 9000, currentCmdPort = 9000;
    private int isPort[] = new int[maxClients];
    private int portCount = 9000;

    public DEVisePOPCmdServerSocket() throws IOException
    {
        super(Globals.POPCMDPORT);
        for (int i = 0; i < maxClients; i++) {
            imgPort[i] = 9000;
            cmdPort[i] = 9000;
            dataPort[i] = 9000;
            isPort[i] = 0;
        }
    }

    public synchronized int getCmdPort()
    {
        return currentCmdPort;
    }

    public synchronized int getImgPort()
    {
        return currentImgPort;
    }

    public synchronized void setCmdPort(int port)
    {
        currentCmdPort = port;
    }

    public synchronized void setImgPort(int port)
    {
        currentImgPort = port;
    }

    public synchronized Vector getClients()
    {
        return clients;
    }

    public synchronized int getCount()
    {
        return clients.size();
    }

    public synchronized boolean increaseCount(DEVisePOPCmdChannel what)
    {
        //System.out.println("(" + clients.size() + " out of " + maxClients + " channels already been used)");
        if (clients.size() == maxClients) {
            System.out.println("Client connection from " + what.getHostName() + " is rejected because maximum number of allowed clients has been reached!");
            return false;
        } else {
            String name = what.getHostName();
            for (int i = 0; i < clients.size(); i++) {
                if (name.equals(((DEVisePOPCmdChannel)clients.elementAt(i)).getHostName())) {
                    System.out.println("Client connection from " + name + " is rejected because connection already established from that host!");
                    return false;
                }
            }

            boolean isportset = false;
            for (int i = 0; i < maxClients; i++) {
                if (isPort[i] == 2) {
                    isportset = true;
                    setImgPort(imgPort[i]);
                    setCmdPort(cmdPort[i]);
                    isPort[i] = 1;
                    what.setNumber(i);
                    System.out.println("Existing DEVise Server found!");
                    break;
                }
            }

            if (isportset) {
                clients.addElement(what);
                return true;
            } else {
                for (int i = 0; i < maxClients; i++) {
                    if (isPort[i] == 0) {
                        int availportnum = 0;
                        int availport[] = new int[3];
                        System.out.println("Start to search for available port ...");
                        while (availportnum < 3) {
                            try {
                                if (portCount > 9999) {
                                    portCount = 9000;
                                }

                                System.out.println("Try port " + portCount);
                                ServerSocket newsocket = new ServerSocket(portCount);
                                newsocket.close();
                                newsocket = null;
                                System.out.println("Port " + portCount + " is available");
                                availport[availportnum] = portCount;
                                availportnum++;
                                portCount++;
                            } catch (IOException e) {
                                System.out.println("Port " + portCount + " is not available");
                                portCount++;
                            }
                        }

                        System.out.println("Starting new DEVise Server ...");

                        Runtime currentRuntime = Runtime.getRuntime();
                        try {
                            Process newproc = currentRuntime.exec("/p/devise/demo/run/devised -port " + availport[0] + " -switchport " + availport[2] + " -imageport " + availport[1]);
                            try {
                                Thread.sleep(20 * 1000);
                            } catch (InterruptedException e) {
                            }

                            try {
                                int v = newproc.exitValue();
                                System.out.println("DEVise Server Error!");
                                return false;
                            } catch (IllegalThreadStateException e) {
                                System.out.println("Successfully starting DEVise Server!");
                            }

                        } catch (IOException e1) {
                            System.out.println("IOException throws in execute DEVise!\n");
                            return false;
                        } catch (SecurityException e2) {
                            System.out.println("SecurityException throws in execute DEVise!\n");
                            return false;
                        }

                        setCmdPort(availport[0]);
                        cmdPort[i] = availport[0];
                        setImgPort(availport[1]);
                        imgPort[i] = availport[1];
                        isPort[i] = 1;
                        isportset = true;
                        what.setNumber(i);
                        break;
                    }
                }

                if (isportset) {
                    clients.addElement(what);
                    return true;
                } else {
                    System.out.println("Unknown error happened!");
                    return false;
                }
            }
        }
    }

    public synchronized boolean decreaseCount(DEVisePOPCmdChannel what)
    {
        // this condition should not occur
        if (clients.size() == 0) {
            return false;
        } else {
            if (clients.removeElement(what)) {
                isPort[what.getNumber()] = 2;
                System.out.println("Client connection from " + what.getHostName() + " is closed.\n");
                System.out.println("DEVise POP Cmd Server waiting for client connection ...");
                System.out.println("(" + clients.size() + " out of " + maxClients + " channels already been used)\n");
                return true;
            }  else  {
                return false;
            }
        }
    }

    public void run()
    {
        System.out.println("\nDEVise POP CMD Server started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = accept();
                System.out.println("New Client connection request is received.");
            }  catch (IOException e)  {
                System.out.println("DEVise POP CMD Server can not listen on port " + Globals.CMDPORT + "\nDEVise POP CMD Server is aborted!");
                isListen = false;
            }

            try  {
                Thread newThread = new Thread(new DEVisePOPCmdChannel(this, socket));
                newThread.start();
            }  catch  (IOException e)  {
                System.out.println("DEVise POP CMD Server can not make socket connection to client!\n" + e.getMessage());
            }
        }
    }
}

