import java.net.*;
import java.io.*;

public final class DEViseComm
{
    public DEViseCmdSocket cmdSocket = null;
    public DEViseImgSocket imgSocket = null;    
    public int connectID = DEViseGlobals.ERRORID;
    public String hostname = DEViseGlobals.DEFAULTHOST;
    public int cmdport = DEViseGlobals.DEFAULTCMDPORT;
    public int imgport = 0;
    public boolean isOnline = false;
    private String username = DEViseGlobals.DEFAULTUSER;
    private String password = DEViseGlobals.DEFAULTPASS;
    // timeout value is set in seconds. timout = 0 means no timeout
    public long timeout = 0;
    
    public DEViseComm(String hname, int port, String uname, String pword)        
    {
        if (hname != null) {
            hostname = hname;
        } 
        
        if (port > 0 && port < 65535) {
            cmdport = port;
        }
        
        if (uname != null) {
            username = uname;
        }
        
        if (pword != null) {
            password = pword;
        }
    }
    
    public synchronized void setName(String name)
    { 
        if (name != null)
            username = name;
    }
    
    public synchronized void setPass(String pass)
    {
        if (pass != null)
            password = pass;
    } 
    
    public synchronized String getName()
    {
        return username;
    }
    
    public synchronized String getPass()
    {
        return password;
    }
    
    public synchronized void connect() throws YException
    {   
        if (isOnline)
            return;
            
        boolean isReconnect = false;
        if (connectID != DEViseGlobals.ERRORID) 
            isReconnect = true;
        
        if (isReconnect) 
            YDebug.println("Trying to reconnect to DEVise POP Server...");
        else
            YDebug.println("Trying to connect to DEVise POP Server at " + hostname + " ...\n");
        
        String[] commands = null;
        
        // Try to establish connection to DEVise POP Server
        
        try {
            cmdSocket = new DEViseCmdSocket(hostname, cmdport);
        } catch (UnknownHostException e) {
            throw new YException("Error: Unknown host " + hostname);
        }  catch (IOException e)  {
            throw new YException("Error: Can not connect to host " + hostname + " at port " + cmdport);
        }
                                
        isOnline = true;
        
        try {
            commands = cmdSocket.sendCommand("JAVAC_Connect {" + username + "} {" + password + "} {" + connectID + "}");
        } catch (InterruptedIOException e) {
            throw new YException("Network timeout is reached while trying to connect to server!");
        }
        
        if (commands[0].equals("JAVAC_Done")) { 
            throw new YException("Unexpected end of api commands received from server!");
        } else if (commands[0].startsWith("JAVAC_Fail")) {
            throw new YException("Server Failure: " + commands[0].substring(10));
        } else if (commands[0].startsWith("JAVAC_User")) {
            //YDebug.println("Parse " + commands[0]);
            String[] data = DEViseGlobals.parseString(commands[0]);
            //YDebug.println("Finish parse");
            if (data == null) {
                throw new YException("Error: Incorrectly formatted api commands received from server: " + commands[0]);
            } else {
                try {
                    imgport = Integer.parseInt(data[1]);
                    if (imgport < 1 || imgport > 65535) 
                        throw new NumberFormatException();
                            
                    timeout = Long.parseLong(data[2]);
                
                    connectID = Integer.parseInt(data[3]);
                    if (connectID < 0)
                        throw new NumberFormatException();                        
                } catch (NumberFormatException e) {
                    throw new YException("Error: Invalid value received from DEVise POP Server: " + commands[0]);
                }
            }
        } else {
            throw new YException("Unexpected api commands received from server: " + commands[0]);
        }
        YDebug.println("Successfully connect to DEVise POP Server!");
        
        // Try to establish Image channel with DEVise Server
        
        YDebug.println("Try to connect to DEVise server image channel at port " + imgport + " ...");
        try {
            imgSocket = new DEViseImgSocket(hostname, imgport);
        } catch (UnknownHostException e) {
            throw new YException("Error: Unknown host " + hostname);
        }  catch (IOException e) {
            throw new YException("Error: Can not connect to host " + hostname + " at port " + imgport);
        }
        YDebug.println("Successfully connect to DEVise server image channel!");
        
        if (isReconnect) {
            try {
                commands = cmdSocket.sendCommand("JAVAC_RestoreState");
            } catch (InterruptedIOException e) {
                YDebug.println("Network timeout is reached while sending JAVAC_RestoreState to server!");
            }
            
            if (!commands[0].equals("JAVAC_Done")) {
                if (commands[0].startsWith("JAVAC_Error")) {
                    throw new YException("Warning: " + commands[0].substring(11), 1);
                } else if (commands[0].startsWith("JAVAC_Fail")) {
                    throw new YException("Server Failure: " + commands[0].substring(10));
                } else {
                    throw new YException("Unexpected api commands received from server: " + commands[0]);
                }
            }
        }
    }        
    
    public synchronized void disconnect(boolean flag)
    {  
        if (!isOnline)
            return;
            
        isOnline = false;
        
        if (flag)
            connectID = DEViseGlobals.ERRORID;        
        
        String[] commands = null;        
        if (cmdSocket != null) {
            try {
                if (!flag) {
                    commands = cmdSocket.sendCommand("JAVAC_Suspend");
                    if (!commands[0].equals("JAVAC_Done")) {
                        throw new YException("Unexpected api commands received from server: " + commands[0]);
                    }
                } else {                
                    commands = cmdSocket.sendCommand("JAVAC_Exit");
                    if (!commands[0].equals("JAVAC_Done")) {
                        throw new YException("Unexpected api commands received from server: " + commands[0]);
                    }
                }
            } catch (YException e) {
                YDebug.println(e.getMessage());
            } catch (InterruptedIOException e) {
                YDebug.println("Network timeout value is reached while trying to close connection to server!");
            }
            
            cmdSocket.closeSocket();
            cmdSocket = null;            
        }
        
        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }
    } 
    
    protected void finalize()
    {   
        disconnect(true);
    }
}
