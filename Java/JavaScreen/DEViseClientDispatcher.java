import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClientDispatcher implements Runnable
{
    private jspop pop = null;
    
    private Vector activeUsers = null;
    private Vector activeServers = null;
    private Vector activeClients = new Vector();
    private Vector suspendClients = new Vector();
    private int maxServer = 0;
    
    private long timeout = 1000;
    private Thread dispatcher = null;
    
    private int IDCount = 0;
    
    public DEViseClientDispatcher(jspop j, Vector u, Vector s)
    {
        pop = j;
        activeUsers = u;
        activeServers = s;
        maxServer = activeServers.size();        
    }
    
    private int getNewID()
    {
        return IDCount++;
    }
    
    public synchronized void addClient(DEViseClient client)
    {
        client.setLastSuspendTime(getCurrentTime());
        suspendClients.addElement(client);
    }
    
    public synchronized void removeClient(DEViseClient client)
    {
        client.setLastSuspendTime(getCurrentTime());
        
        client.close(true);
        
        activeClients.removeElement(client);
        suspendClients.removeElement(client);
    }
    
    public synchronized void suspendClient(DEViseClient client, boolean flag)
    {   
        client.setLastSuspendTime(getCurrentTime());

        if (flag) {
            client.close(false);
        }
        
        activeClients.removeElement(client);
        suspendClients.addElement(client);
    }
    
    public synchronized int checkClient(DEViseClient client, String name, String pass, int id)
    {
        if (name == null || pass == null)
            return 1;
        
        // Check whether the user name is valid
        DEViseUser user = null;
        for (int i = 0; i < activeUsers.size(); i++) {
            user = (DEViseUser)activeUsers.elementAt(i);
            if (user.equals(name, pass))
                break;
            else 
                user = null;
        }
        
        if (user == null)
            return 2;
        
        client.setUser(user);
        
        // check whether the connect id is correct
        if (id == DEViseGlobals.DEFAULTID) {
            client.setID(getNewID());
            client.setLastSavedSession(client.getHost() + id);
            return 0;
        } else {
            DEViseClient oldclient = null;
            for (int i = 0; i < suspendClients.size(); i++) {
                oldclient = (DEViseClient)suspendClients.elementAt(i);
                if (id == client.getID()) {
                    // need more work
                    break;
                } else {
                    oldclient = null;
                }
            }
            
            if (oldclient == null) {
                return 3;
            } else {
                client.setID(id);
                client.setTotalOnlineTime(oldclient.getTotalOnlineTime(getCurrentTime()));
                client.setTotalSuspendTime(oldclient.getTotalSuspendTime(getCurrentTime()));
                client.setLastSavedSession(oldclient.getLastSavedSession());
                client.setLastActiveTime(getCurrentTime());
                suspendClients.removeElement(oldclient);
                activeClients.removeElement(oldclient);
                return 0;
            }
        }
    }
    
    private synchronized void waitForSignal()
    {
        try {
            wait(timeout);
        } catch (InterruptedException e) {
        }
    }
    
    public synchronized void startDispatcher()
    {
        dispatcher = new Thread(this);
        dispatcher.start();    
    }
    
    public void run()
    {
        DEViseClient client = null;
        DEViseServer server = null;
        while(true) {
            waitForSignal();
            client = getNextSuspendClient();
            
            if (client != null) {                
                server = getNextAvailableServer();
                if (server == null)
                    continue;
                DEViseClient c = server.getClient();
                server.setClient(client);
                client.setLastActiveTime(getCurrentTime());
                activeClients.addElement(client);
                suspendClients.removeElement(client);
                if (c != null) {
                    c.setLastSuspendTime(getCurrentTime());
                    activeClients.removeElement(c);
                    suspendClients.addElement(c);
                }                                            
            }            
        }
    }
    
    private synchronized DEViseClient getNextSuspendClient()
    {
        DEViseClient client = null, lastClient = null;
        long time = 0, currentTime = getCurrentTime();        
        
        for (int i = 0; i < suspendClients.size(); i++) {
            client = (DEViseClient)suspendClients.elementAt(i);
            if (client.getSuspendTime(currentTime) > time) {
                if (client.isRequest()) {
                    time = client.getSuspendTime(currentTime);
                    lastClient = client;
                } else {
                    client = null;
                }
            }               
        }
        
        return lastClient;
    }
    
    private synchronized DEViseServer getNextAvailableServer()
    {    
        DEViseServer lastServer = null, server = null;
        long time = 0, currentTime = getCurrentTime();
        
        for (int i = 0; i < activeServers.size(); i++) {
            server = (DEViseServer)activeServers.elementAt(i);
            if (server.isSocketAlive()) {
                DEViseClient client = server.getClient();
                if (client == null) {
                    return server;
                } else {
                    if (client.getActiveTime(currentTime) > time) {
                        time = client.getActiveTime(currentTime);
                        lastServer = server;
                    }    
                }
            } else {
                try {
                    server.startServer();
                    return server;
                } catch (YException e) {
                    YGlobals.Ydebugpn(e.getMessage());
                }                                
            }
        }
        
        return lastServer;
    }
    
    public long getCurrentTime()
    {
        return System.currentTimeMillis();
    }
}

