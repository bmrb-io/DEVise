import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClient
{
    private long totalOnlineTime = 0, totalSuspendTime = 0, lastActiveTime = 0, lastSuspendTime = 0;
    private String[] serverCmds = null;
    private Vector images = null;
    private DEViseUser user = null;
    private int connectID = -5;
    private int stage = 0;
    public DEViseCmdSocket cmdSocket = null;
    public DEViseImgSocket imgSocket = null;
    private String hostname = null;
    private String session = null; 
    private boolean isSessionOpened = false;
    
    public DEViseClient(DEViseCmdSocket s1, DEViseImgSocket s2, String h)
    {                
        cmdSocket = s1;
        imgSocket = s2;
        hostname = h;
    }
    
    public synchronized String getLastSavedSession()
    {
        return session;
    }
    
    public synchronized void setLastSavedSession(String s)
    {
        session = s;
    }
    
    public synchronized void setSessionFlag(boolean flag)
    {
        isSessionOpened = flag;
    }
    
    public synchronized boolean getSessionFlag()
    {
        return isSessionOpened;
    }
    
    public synchronized long getTotalOnlineTime(long time)
    {
        if (lastSuspendTime == 0 && lastActiveTime == 0)
            return totalOnlineTime;
            
        if (lastSuspendTime > lastActiveTime)
            return totalOnlineTime;
        else 
            return totalOnlineTime + (time - lastActiveTime);
    }
    
    public synchronized void setTotalOnlineTime(long time)
    {
        totalOnlineTime = time;
    }
    
    public synchronized long getTotalSuspendTime(long time)
    {
        if (lastSuspendTime == 0 && lastActiveTime == 0)
            return totalSuspendTime;
        
        if (lastSuspendTime > lastActiveTime)
            return totalSuspendTime + (time - lastSuspendTime);
        else
            return totalSuspendTime;
    }
    
    public synchronized void setTotalSuspendTime(long time)
    {
        totalSuspendTime = time;
    }
    
    public synchronized long getActiveTime(long time)
    {
        if (lastSuspendTime < lastActiveTime)
            return (time - lastActiveTime);
        else
            return 0;            
    }
    
    public synchronized long getSuspendTime(long time)
    {
        if (lastSuspendTime > lastActiveTime)
            return (time - lastSuspendTime);
        else
            return 0;
    }
    
    public synchronized void setLastActiveTime(long time)
    {
        if (lastSuspendTime != 0) {
            totalSuspendTime += (time - lastSuspendTime);
        }
        
        lastActiveTime = time;                    
    } 
    
    public synchronized long getLastActiveTime()
    {
        return lastActiveTime;
    }
    
    public synchronized void setLastSuspendTime(long time)
    {
        if (lastActiveTime != 0) {
            totalOnlineTime += (time - lastActiveTime);
        }
        
        lastSuspendTime = time;
    }
    
    public synchronized long getLastSuspendTime()
    {
        return lastSuspendTime;
    }
    
    public synchronized void setStage(int s)
    {
        stage = s;
    }
    
    public synchronized int getStage()
    {
        return stage;
    }
    
    public String getHost()
    {
        return hostname;
    }
    
    public synchronized String[] getCmds()
    {
        return serverCmds;
    }
    
    public synchronized void setCmds(String[] s)
    {
        serverCmds = s;
    }
    
    public synchronized Vector getImages()
    {
        return images;
    }
    
    public synchronized void setImages(Vector i)
    {
        images = i;
    }
    
    public synchronized DEViseUser getUser()
    {
        return user;
    }
    
    public synchronized void setUser(DEViseUser u)
    {
        user = u;
    }
    
    public synchronized int getID()
    {
        return connectID;
    }
    
    public synchronized void setID(int id)
    {
        connectID = id;
    }
    
    public synchronized boolean isRequest()
    {
        if (cmdSocket == null || imgSocket == null)
            return false;
        
        try {
            if (cmdSocket.isEmpty()) 
                return false;
            else 
                return true;
        } catch (YException e) {
            YGlobals.Ydebugpn(e.getMessage());
            close(false);
            return false;
        }
    }
        
    public synchronized void close(boolean flag)
    {            
        if (flag) {
            //totalOnlineTime = getTotalOnlineTime(time);
            //totalSuspendTime = getTotalSuspendTime(time);        
            ; // do something, such as update user information
        }
        
        if (cmdSocket != null) {
            cmdSocket.closeSocket();
            cmdSocket = null;
        }
        
        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }
        
        // client will retain connectID, user, totalOnlineTime and totalSuspendTime
        // and lastSuspendTime and lastActiveTime
        stage = 0;
        isSessionOpened = false;
        serverCmds = null;
        images = null;
    }
}