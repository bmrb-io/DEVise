import java.util.*;

public class DEViseUser
{
    private String username = null;
    private String password = null;
    private int priority;
    private int maxLogin;
    // In milliseconds, 50 years would be 1.5768 * 10^12 milliseconds
    private long totalOnlineTime; // In milliseconds, user's total online time

    private Hashtable currentClients = new Hashtable();

    public DEViseUser(String name, String pass, int pr, int max, long tot) throws YException
    {
        if (name == null || pass == null)
            throw new YException("Null User name or password!", "DEViseUser:constructor");

        if (pr < 0 || max < 1 || tot < 0)
            throw new YException("Invalid arguments: priority " + pr + " or maxlogin " + max + " or total online time " + tot + "!",
                                 "DEViseUser:constructor");

        StringTokenizer stk = new StringTokenizer(name);
        if (stk.countTokens() == 0)
            throw new YException("User name must contain at least one non-whitespace character!", "DEViseUser:constructor");
        else
            username = name;

        stk = new StringTokenizer(pass);
        if (stk.countTokens() == 0)
            password = new String("");
        else
            password = pass;

        priority = pr;
        maxLogin = max;
        totalOnlineTime = tot;
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

    public synchronized int getLogin()
    {
        return currentClients.size();
    }

    public synchronized boolean checkLogin()
    {
        if (currentClients.size() < maxLogin) {
            return true;
        } else {
            return false;
        }
    }

    public synchronized long getTOT()
    {
        return totalOnlineTime;
    }

    public synchronized void setTOT(long tot)
    {
        totalOnlineTime = tot;
    }

    public synchronized void addTOT(long t)
    {
        totalOnlineTime += t;
    }

    public synchronized void addClient(DEViseClient client)
    {
        if (client == null)
            return;

        currentClients.put(client.getID(), client);
    }

    public synchronized void removeClient(DEViseClient client)
    {
        if (client == null)
            return;

        currentClients.remove(client.getID());
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