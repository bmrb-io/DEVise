import java.net.*;
import java.io.*;

class DEViseServer
{
    private static int portCount = 10000;
    public int imgPort, cmdPort, switchPort;
    public long timeout = 0, waitTimeout = 15;
    Process proc = null;
    public boolean isAvailable = false;
     
    public DEViseServer(long t1, long t2) throws YException 
    {
        timeout = t1;
        waitTimeout = t2;
        
        YGlobals.printMsg("Trying to start a new DEVise Server ...\n");
        
        int availPort = 0;
        int[] port = new int[3];
        
        while (availPort < 3) {
            int newport = DEViseServer.getPortCount();                        
                
            if (tryPort(newport)) {
                port[availPort] = newport;
                availPort++;
            }            
        }
        
        imgPort = port[1];
        cmdPort = port[0];
        switchPort = port[2];
        
        Runtime currentRT = Runtime.getRuntime();
        try {
            proc = currentRT.exec("devised.jspop -port " + cmdPort + " -imageport " + imgPort + " -switchport " + switchPort);
            
            try {
                Thread.sleep(waitTimeout * 1000);
            } catch (InterruptedException e) {
            }
            
            try {
                int v = proc.exitValue();
                throw new YException("DEVise Server aborted!");
            } catch (IllegalThreadStateException e) {
            }
        } catch (IOException e1) {
            throw new YException("IO Error while trying to start a new DEVise Server!");
        } catch (SecurityException e2) {
            throw new YException("Security Error while trying to start a new DEVise Server!");
        }
        
        isAvailable = true;
        
        YGlobals.printMsg("New DEVise Server successfully started!\n");    
    }
    
    public synchronized static int getPortCount()
    {        
        if (portCount > 20000)
            portCount = 10000;
        
        int port = portCount;
        portCount++;
        
        return port;
    }
    
    public synchronized boolean tryPort(int port)
    {
        try {
            ServerSocket socket = new ServerSocket(port);
            socket.close();
            return true;
        } catch (IOException e) {
            return false;
        }
    }                    
}