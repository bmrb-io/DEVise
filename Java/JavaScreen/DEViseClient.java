import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseClient implements Runnable
{
    jspop pop = null;
    DEViseCmdSocket jsCmdSocket = null, deviseCmdSocket = null;
    public Thread clientThread = null;
    
    public String hostname = null;
    public int connectID;
    public String lastSavedSession = null;
    public DEViseUser user = null;
    public DEViseServer server = null;   
    public int deviseImgPort, deviseCmdPort;
    
    boolean status = true;
    String errorMsg = null;
        
    public DEViseClient(jspop what, Socket socket) throws IOException
    {
        pop = what;
        jsCmdSocket = new DEViseCmdSocket(socket);
        hostname = socket.getInetAddress().getHostName();
        clientThread = new Thread(this);
        clientThread.start();
    }
    
    public synchronized void close(boolean isQuit)
    {
        status = false;                
        
        try {
            if (jsCmdSocket != null) {
                jsCmdSocket.sendCmd("JAVAC_Done");
            }        
        } catch (YException e) {
            YGlobals.printMsg(e.getMessage());
        }
        
        try {
            if (deviseCmdSocket != null) {
                if (!isQuit) {
                    try {
                        if (lastSavedSession == null)
                            lastSavedSession = user.username + connectID;
                            
                        String[] rsp = deviseCmdSocket.sendCommand("JAVAC_SaveSession {" + lastSavedSession + "}");
                    } catch (InterruptedIOException e1) {
                        YGlobals.printMsg("Timeout while sending JAVAC_SaveSession!");
                    }
                }
                    
                deviseCmdSocket.sendCmd("JAVAC_Exit");
            }        
        } catch (YException e) {
            YGlobals.printMsg(e.getMessage());
        }
        
        if (deviseCmdSocket != null) {
            deviseCmdSocket.closeSocket();   
            deviseCmdSocket = null;
        }
        
        if (jsCmdSocket != null) {
            jsCmdSocket.closeSocket();
            jsCmdSocket = null;
        } 
        
        if (server != null) 
            server.isAvailable = true;
                
        if (isQuit) 
            pop.removeClient(this);
        else
            pop.suspendClient(this);
    }
    
    public void run()
    {
        try {
            setStatus(true);
            
            String request = null;
            try {
                request = jsCmdSocket.receiveRsp();
            } catch (InterruptedIOException e) {
                throw new YException("Can not receive response from " + hostname + " within timeout value!");
            }
            
            if (request.startsWith("JAVAC_Connect")) {
                String[] data = DEViseGlobals.parseString(request);
                if (data == null)
                    throw new YException("Invalid connection request received from client!");
                
                String username = data[1];
                String password = data[2];
                int id;
                try {
                    id = Integer.parseInt(data[3]);
                } catch (NumberFormatException e) {
                    throw new YException("Invalid connection request received from client: " + request);
                }
                
                if (validateClient(username, password, id)) {
                    jsCmdSocket.sendCmd("JAVAC_User " + deviseImgPort + " " + user.timeout + " " + connectID);
                    jsCmdSocket.sendCmd("JAVAC_Done");
                } else {
                    jsCmdSocket.sendCmd("JAVAC_Fail {" + errorMsg + "}");
                    close(true);
                }
            } else {
                throw new YException("Invalid connection request received from client " + hostname + ": " + request);
            } 
            
            String command = null;
            String[] cmd = null;
            String[] rsp = null;
            while (getStatus()) {
                try {
                    command = jsCmdSocket.receiveRsp();
                } catch (InterruptedIOException e) {
                //    String result = YGlobals.showMsg(jsc, "Can not receive response from client " + hostname + " within timeout!"
                //                    + "\nDo you wish to continue to wait?", "Confirm", YGlobals.MBXYESNO);
                //    if (!result.equals(YGlobals.IDYES)) 
                        close(true);
                        
                    continue;
                }
                
                cmd = DEViseGlobals.parseString(command);
                if (cmd == null)
                    throw new YException("Incorrect command received from client!");
                
                try {                
                    if (cmd[0].startsWith("JAVAC_Exit")) {
                        close(true);
                    } else if (cmd[0].startsWith("JAVAC_Suspend")) {
                        close(false);
                    } else if (cmd[0].startsWith("JAVAC_SaveCurrentState")) {
                        if (lastSavedSession != null) {
                            rsp = deviseCmdSocket.sendCommand("JAVAC_SaveSession " + lastSavedSession);
                        } else {
                            lastSavedSession = user.username + connectID + ".ds";
                            rsp = deviseCmdSocket.sendCommand("JAVAC_SaveSession " + lastSavedSession);
                        }
                    } else if (cmd[0].startsWith("JAVAC_RestoreState")) {
                        if (lastSavedSession != null) {
                            rsp = deviseCmdSocket.sendCommand("JAVAC_OpenSession " + lastSavedSession);
                        } else {
                            rsp = new String[1];
                            rsp[0] = new String("JAVAC_Error {Can not find Saved Session!}");
                        }    
                    } else {
                        if (command.startsWith("JAVAC")) {
                            rsp = deviseCmdSocket.sendCommand(command);
                        } else {
                            rsp = deviseCmdSocket.sendCommand(command, DEViseGlobals.API_CMD);
                        }
                    }
                } catch (InterruptedIOException e1) {
                    rsp = new String[1];
                    rsp[0] = new String("JAVAC_Fail {Can not receive response from DEVise server within timeout!}");
                }
                
                if (rsp != null) {
                    for (int i = 0; i < rsp.length; i++) 
                        jsCmdSocket.sendCmd(rsp[i]); 
                }
                
                rsp = null;
            }            
        } catch (YException e) {
            YGlobals.printMsg(e.getMessage() + "\n");
            close(true);
        }                  
    }
    
    public synchronized boolean getStatus()
    {
        return status;
    }
    
    public synchronized void setStatus(boolean s)
    {
        status = s;
    } 
    
    public synchronized boolean validateClient(String name, String pass, int id)
    {   
        user = pop.checkUser(name, pass);
        
        if (user == null) {
            errorMsg = new String("Connection rejected! Invalid username or Incorrect password received!");
            YGlobals.printMsg(errorMsg);
            return false;
        }
        
        if (!pop.checkHost(user, hostname)) {
            errorMsg = new String("Connection rejected! You can not start more than one client!");
            YGlobals.printMsg(errorMsg);
            return false;
        }
        
        connectID = pop.checkID(this, id);
        if (connectID == DEViseGlobals.ERRORID) {
            errorMsg = new String("Connection rejected! Invalid connection ID received!");
            YGlobals.printMsg(errorMsg);
            return false;
        }
        
        if (!pop.startServer(this)) {
            connectID = DEViseGlobals.ERRORID;
            if (user.priority == 2) {
                errorMsg = new String("Connection rejected! Can not start DEVise server within timeout!");
                YGlobals.printMsg(errorMsg);
                return false;
            } else {
                errorMsg = new String("Connection rejected! All connection are used! Please try again later!");
                YGlobals.printMsg(errorMsg);
                return false;
            }
        } 
        
        try {
            deviseCmdSocket = new DEViseCmdSocket("localhost", deviseCmdPort);
        } catch (UnknownHostException e) {
            YGlobals.printMsg("Can not open socket connection to DEVise Server");
            errorMsg = new String("Connection rejected! Can not talk to DEVise Server!");
            return false;
        } catch (IOException e) {
            YGlobals.printMsg("Can not open socket connection to DEVise Server");
            errorMsg = new String("Connection rejected! Can not talk to DEVise Server!");
            return false;
        }
        
        return true;
    }
}                  