import  java.io.*;
import  java.net.*;
import  java.util.*;

// Socket used to send DEVise API commands and receive response between
// DEViseServer and DEViseClient
public class DEViseCmdSocket
{   
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
        
    public DEViseCmdSocket(Socket arg) throws IOException
    {
        socket = arg;        
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        is = new DataInputStream(socket.getInputStream()); 
        try {
            socket.setSoTimeout(DEViseGlobals.DEFAULTCMDSOCKETTIMEOUT);
        } catch (SocketException e) {
            YDebug.println(e.getMessage());
        }
    }
               
    public DEViseCmdSocket(String hostname, int port) throws UnknownHostException, IOException
    {
        socket = new Socket(hostname, port); 
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        is = new DataInputStream(socket.getInputStream());
        try {
            socket.setSoTimeout(DEViseGlobals.DEFAULTCMDSOCKETTIMEOUT);
        } catch (SocketException e) {
            YDebug.println(e.getMessage());
        }
    }

    public void sendBytes(byte[] data) throws YException
    {
        if (data == null)
            return; 
            
        try {
            os.write(data, 0, data.length);    
            os.flush();
        } catch (IOException e) {
            throw new YException("DEVise Command Socket Error: Communication Error occured while sending data!");
        }
    }
    
    public byte[] receiveBytes(int howMany) throws YException, InterruptedIOException
    {
        if (howMany < 1)
            return null;
        
        byte[] data = new byte[howMany];
        try {
            is.readFully(data);
            return data;
        } catch (IOException e) {
            throw new YException("DEVise Command Socket Error: Communication Error occured while receiving data!");
        }
    }
   
    public void sendInt(int data) throws YException
    {
        try {
            os.writeInt(data);
            os.flush();
        } catch (IOException e) {
            throw new YException("DEVise Command Socket Error: Communication Error occured while sending integer data!");
        }        
    }
    
    public int receiveInt() throws YException, InterruptedIOException
    {
        try {
            return is.readInt();
        } catch (IOException e) {
            throw new YException("DEVise Command Socket Error: Communication Error occured while receiving integer data!");
        }            
    }
    
    public String[] sendCommand(String cmd) throws YException, InterruptedIOException
    {
        return sendCommand(cmd, DEViseGlobals.API_JAVA);
    }
    
    public String[] sendCommand(String cmd, short flag) throws YException, InterruptedIOException
    {
        String[] commands = null;
        String rsp = null;
        boolean isEnd = false;
        Vector rspbuf = new Vector();
        
        sendCmd(cmd, flag);
        while (!isEnd) {
            rsp = receiveRsp();
            commands = DEViseGlobals.parseStr(rsp);
            if (commands == null) {
                throw new YException("DEVise Command Socket Error: Invalid response received from DEVise POP Server!");
            } else {
                for (int i = 0; i < commands.length; i++) {                    
                    if (commands[i].startsWith("JAVAC_")) {
                        if (commands[i].equals("JAVAC_Done") || commands[i].startsWith("JAVAC_Error") || commands[i].startsWith("JAVAC_Fail")) {
                            if (i == commands.length - 1) {
                                isEnd = true;
                            } else {
                                throw new YException("DEVise API error: Incorrectly formatted api received from server: " + rsp);
                            }
                        }
                        
                        rspbuf.addElement(commands[i]);                            
                    } else {
                        throw new YException("DEVise API error: Unrecognized api received from server: " + commands[i]);
                    }
                }
            }
        }
        
        if (rspbuf.size() == 0) {
            throw new YException("DEVise API error: Invalid response received from server!");
        } else {
            String[] rspstr = new String[rspbuf.size()];
            for (int i = 0; i < rspbuf.size(); i++)
                rspstr[i] = (String)rspbuf.elementAt(i);
            
            return rspstr;
        }
    }
    
    public void sendCmd(String cmd) throws YException
    {
        sendCmd(cmd, DEViseGlobals.API_JAVA);
    }
        
    public void sendCmd(String cmd, short flag) throws YException
    {
        short nelem = 0, size = 0;
        short i;
        
        YDebug.println("Sending command: " + cmd);
        
        try  {
            String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);
            if (cmdBuffer == null)
                throw new YException("DEVise Command Socket Error: NULL or Error Formatted API Command!");
            
            nelem = (short)cmdBuffer.length;
            for (i = 0; i < nelem; i++)  {
                cmdBuffer[i] = cmdBuffer[i] + "\u0000";
                size = (short)(size + 2 + cmdBuffer[i].length());
            }
                    
            os.writeShort(flag);
            os.writeShort(nelem);
            os.writeShort(size);
            
            for (i = 0; i < nelem; i++)  {
                os.writeShort((short)cmdBuffer[i].length());
                os.writeBytes(cmdBuffer[i]);
            } 
            
            os.flush();
        }  catch (IOException e)  {
            throw new YException("DEVise Command Socket Error: Communication Error occured while sending API commands!");
        }
    }
    
    public String receiveRsp() throws YException, InterruptedIOException
    {
        return receiveRsp(false);
    }
    
    public String receiveRsp(boolean header) throws YException, InterruptedIOException
    {
        String response = new String("");
        short flag = 0, nelem = 0, size = 0, i;
        boolean isEnd = false;
        
        try  {
            while (!isEnd)  {        
                flag = is.readShort();
                switch (flag)  {
                case DEViseGlobals.API_CMD:
                    if (header)
                        response = response + "(CMD) -> ";                    
                    isEnd = true;
                    break;
                case DEViseGlobals.API_ACK:
                    if (header)
                        response = response + "(ACK) -> ";                    
                    isEnd = true;
                    break;
                case DEViseGlobals.API_NAK:
                    if (header)
                        response = response + "(NAK) -> ";                    
                    isEnd = true;
                    break;
                case DEViseGlobals.API_CTL:
                    if (header)
                        response = response + "(CTL) -> ";                    
                    break;
                case DEViseGlobals.API_JAVA:
                    if (header)
                        response = response + "(JAV) -> ";
                    isEnd = true;
                    break;
                default:
                    if (header)
                        response = response + "(   ) -> ";                    
                    break;
                }
                    
                nelem = is.readShort();
                size = is.readShort();            
                short argsize;
                byte[] arg;
                for (i = 0; i < nelem; i++)  {
                    if (i != 0) 
                        response = response + " ";
                    argsize = is.readShort();
                    arg = new byte[argsize];
                    is.readFully(arg);
                    
                    // use argsize - 1 instead of argsize is to skip the NULL '\0'
                    String rsp = new String(arg, 0, argsize - 1);
                    
                    if (i == 0) {
                        if (rsp.startsWith("{") && rsp.endsWith("}")) {
                            String[] tmprsp = DEViseGlobals.parseString(rsp);
                            if (tmprsp != null && tmprsp.length == 1) 
                                rsp = tmprsp[0];
                        }
                    }
                    
                    response = response + rsp;
                }
                
                if (!isEnd)  {
                    response = response + "\n";
                }
            }
            
            YDebug.println("Receiving response: " + response);
            
            return response;
        }  catch (IOException e)  {
            throw new YException("DEVise Command Socket Error: Communication Error occured while receiving API responses!");
        }
    }
    
    public void closeSocket()
    {
        try  {                               
            os.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Command Socket Error: Communication Error occured while closing socket connection!");
        }
        
        try {
            is.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Command Socket Error: Communication Error occured while closing socket connection!");
        }
           
        try {
            socket.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Command Socket Error: Communication Error occured while closing socket connection!");
        }
        
        os = null;
        is = null;
        socket = null;
    }    
}
