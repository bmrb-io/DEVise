import java.util.*;

public class DEViseUser
{
    private String username = DEViseGlobals.DEFAULTUSER;
    private String password = DEViseGlobals.DEFAULTPASS;
    private int priority;
    private int maxLogin;
    // In milliseconds, 50 years would be 1.5768 * 10^12 milliseconds
    private long totalOnlineTime; // In milliseconds, user's total online time
    private long lastOnlineTime; // In milliseconds, user's last online time
    private long lastLoginTime; // In milliseconds since 01/01/1970 midnight GMT
    private String lastLoginAddress; // Internet IP address

    private Vector currentClients = new Vector();

    public DEViseUser(String name, String pass, int pr, int max, long tot, long llt, String lla, long lot) throws YException
    {
        if (name == null || pass == null || lla == null)
            throw new YException("DEViseUser::constructor: Null User name or password or hostname!");

        if (pr < 0 || pr > 10 || max < 1 || max > 100 || tot < 0
            || lot < 0 || llt < 0)
            throw new YException("DEViseUser::constructor: Invalid arguments!\n"
                     +"Priority: " + pr + " MaxLogin: " + max + " TotalOnlineTime: " + tot
                     + " lastOnlineTime: " + lot + " lastLoginTime: " + llt);
        
        StringTokenizer stk = new StringTokenizer(name);
        if (stk.countTokens() == 0)
            throw new YException("DEViseUser::constructor: User name can not be empty!");
        else 
            username = name;
        
        stk = new StringTokenizer(pass);
        if (stk.countTokens() == 0)
            password = new String("");
        else
            password = pass;
            
        stk = new StringTokenizer(lla);
        if (stk.countTokens() == 0)
            lastLoginAddress = new String("");
        else 
            lastLoginAddress = lla;
                
        priority = pr;
        maxLogin = max;
        totalOnlineTime = tot;
        lastOnlineTime = lot;
        lastLoginTime = llt;
    }

    public String getName()
    {
        return username;
    }

    public String getPass()
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

    public synchronized long getTOT()
    {
        return totalOnlineTime;
    }

    public synchronized String getTOTS()
    {
        return null;
    }

    public synchronized void setTOT(long tot)
    {
        totalOnlineTime = tot;
    }

    public synchronized void addTOT(long t)
    {
        totalOnlineTime += t;
    }

    public synchronized long getLOT()
    {
        return lastOnlineTime;
    }

    public synchronized String getLOTS()    
    {
        return null;
    }

    public synchronized void setLOT(long lot)
    {
        lastOnlineTime = lot;
    }

    public synchronized String getLLT()
    {
        return null;
    }

    public synchronized void setLLT()
    {
    }

    public synchronized String getLLH()
    {
        return lastLoginAddress;
    }

    public synchronized void setLLH(String lla)
    {
        lastLoginAddress = lla;
    }

    public synchronized void addClient(DEViseClient client)
    {
        currentClients.addElement(client);
    }
    
    public synchronized void removeClient(DEViseClient client)
    {
        currentClients.removeElement(client);
    }
    
    public synchronized Vector getAllClients()
    {
        return currentClients;
    }
    
    public synchronized String getInfo()
    {
        return null;
    }

    public boolean equals(DEViseUser user)
    {   
        if (user == null)
            return false;
            
        if (username.equals(user.username) && password.equals(user.password))
            return true;
        else
            return false;
    }
    
    public boolean equals(String name, String pass)
    {
        if (name == null || pass == null)
            return false;
        
        if (username.equals(name) && password.equals(pass))
            return true;
        else
            return false;
    }
}