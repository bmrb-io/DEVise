import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClientDispatcher implements Runnable
{
    private jspop pop = null;
    private int timestep = 100;

    private Thread dispatcher = null;
    // status = -1, dispatcher is stopped
    // status = 0, dispatcher is idle
    // status = 1, dispatcher is doing stuffs
    private int status = -1;
    // action = 0, stop dispatcher
    // action = 1, dispatcher normal
    private int action = 1;

    private Vector activeClients = new Vector();
    private Vector suspendClients = new Vector();

    public DEViseClientDispatcher(jspop j)
    {
        pop = j;

        if (pop.dispatcherTimestep > timestep) {
            timestep = pop.dispatcherTimestep;
        }
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    public synchronized int getAction()
    {
        return action;
    }

    public synchronized void startDispatcher()
    {
        if (status < 0) {
            action = 1;
            status = 0;
            dispatcher = new Thread(this);
            dispatcher.start();
        }
    }

    public synchronized void stopDispatcher()
    {
        if (status < 0) {
            return;
        } else {
            action = 0;
            notifyAll();
        }
    }

    private synchronized void waitForSignal()
    {
        status = 0;

        try {
            wait(timestep);
        } catch (InterruptedException e) {
        }
    }

    public void run()
    {
        DEViseClient client = null;
        DEViseServer server = null;

        while (getAction() > 0) {
            waitForSignal();

            //YGlobals.Ydebugpn("active: " + activeClients.size() + " suspend: " + suspendClients.size());

            if (getAction() > 0) {
                client = getNextClient();

                if (client != null) {
                    server = getNextServer();
                    if (server != null) {
                        if (server.setClient(client)) {
                            // do we need to wait for finishing switching?
                            activeClient(client);
                        }
                    }
                }
            }
        }
    }

    // To prevent deadlock with pop.removeClient, this funciton is not synchronized
    public void addClient(DEViseClient client)
    {
        suspendClients.addElement(client.getID());
    }

    private synchronized DEViseClient getNextClient()
    {
        for (int i = 0; i < suspendClients.size(); i++) {
            Integer id = (Integer)suspendClients.elementAt(i);
            DEViseClient client = (DEViseClient)pop.clients.get(id);

            int code = client.isRequest();
            if (code == 1 ) {
                return client;
            } else if (code == -1) {
                removeClient(client);
            } else {
                ; // do nothing
            }
        }

        return null;
    }

    public synchronized void removeClient(DEViseClient client)
    {
        if (client != null) {
            activeClients.removeElement(client.getID());
            suspendClients.removeElement(client.getID());
            pop.removeClient(client);
        }
    }

    public synchronized void activeClient(DEViseClient client)
    {
        if (client != null) {
            client.setActive();
            suspendClients.removeElement(client.getID());
            activeClients.addElement(client.getID());
        }
    }

    public synchronized void suspendClient(DEViseClient client)
    {
        if (client != null) {
            client.setSuspend();
            activeClients.removeElement(client.getID());
            suspendClients.addElement(client.getID());
        }
    }

    private DEViseServer getNextServer()
    {
        DEViseServer newserver = null, server = null;
        long time = 0;

        for (int i = 0; i < pop.servers.size(); i++) {
            server = (DEViseServer)pop.servers.elementAt(i);
            if (server.isAvailable()) {
                return server;
            } else {
                if (server.getActiveTime() > time) {
                    time = server.getActiveTime();
                    newserver = server;
                }
            }
        }

        return newserver;
    }
}

