// DEViseUser.java
// last updated on 04/16/99

import java.util.*;

public class DEViseUser
{
    private String username = null;
    private String password = null;
    private int priority;
    private int maxLogin;

    private Hashtable currentClients = new Hashtable();

    public DEViseUser(String[] data) throws YException
    {
        if (data == null) {
            throw new YException("Invalid user data: \"NULL\"");
        }

        if (data.length != 4) {
            throw new YException("Incorrect number of items in user data");
        }

        try {
            username = data[0];
            password = data[1];
            priority = Integer.parseInt(data[2]);
            maxLogin = Integer.parseInt(data[3]);
            if (priority < 1 || maxLogin < 1 || username == null || password == null)
                throw new NumberFormatException();
        } catch (NumberFormatException e) {
            throw new YException("Incorrect data format in user data");
        }
    }

    public String getName()
    {
        return username;
    }

    public String getPassword()
    {
        return password;
    }

    public int getPriority()
    {
        return priority;
    }

    public int getMaxLogin()
    {
        return maxLogin;
    }

    public synchronized int getCurrentLogin()
    {
        return currentClients.size();
    }

    public synchronized boolean addClient(DEViseClient client)
    {
        if (client == null)
            return true;

        if (currentClients.contains(client)) {
            return true;
        }

        if (currentClients.size() < maxLogin) {
            currentClients.put(client.getConnectionID(), client);
            return true;
        } else {
            return false;
        }
    }

    public synchronized void removeClient(DEViseClient client)
    {
        if (client == null)
            return;

        currentClients.remove(client.getConnectionID());
    }

    public synchronized void removeClient(Integer id)
    {
        if (id == null)
            return;

        currentClients.remove(id);
    }

    public synchronized boolean containsClient(DEViseClient client)
    {
        if (client == null)
            return false;

        return currentClients.contains(client);
    }

    public synchronized boolean containsClient(Integer id)
    {
        if (id == null)
            return false;

        return currentClients.containsKey(id);
    }

    public synchronized Hashtable getAllClients()
    {
        return currentClients;
    }

    public boolean equals(DEViseUser user)
    {
        if (user == null)
            return false;

        if (username.equals(user.getName()) && password.equals(user.getPassword()))
            return true;
        else
            return false;
    }
}