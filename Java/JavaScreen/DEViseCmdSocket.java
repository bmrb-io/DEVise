/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
 */
import  java.io.*;
import  java.net.*;
import  java.lang.*;

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
    }
               
    public DEViseCmdSocket(String hostname, int port) throws UnknownHostException, IOException
    {
        socket = new Socket(hostname, port); 
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        is = new DataInputStream(socket.getInputStream());
    }
/*       
    public synchronized String sendAPICmd(String cmd, boolean header) throws DEViseNetException
    {
        String response = null;
        
        try  {
            sendCmd(cmd, DEViseGlobals.API_CMD);
            response = receiveRsp(header);
        }  catch (DEViseNetException e)  {
            throw new DEViseNetException("(Command: " + cmd + " )\n" + e.getMessage());
        }
        
        return response; 
    }
*/
    public void sendBytes(byte[] data) throws DEViseNetException
    {
        if (data == null)
            return; 
            
        try {
            os.write(data, 0, data.length);    
            os.flush();
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while sending data!");
        }
    }
    
    public byte[] receiveBytes(int howMany) throws DEViseNetException
    {
        if (howMany < 1)
            return null;
        
        byte[] data = new byte[howMany];
        try {
            is.readFully(data);
            return data;
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while receiving data!");
        }
    }
    
    public void sendInt(int data) throws DEViseNetException
    {
        try {
			Integer aInt = new Integer(data);
			String  aStr;
			int		len;
			int		i;

			aStr = aInt.toString();
			len = aStr.length();
			for (i=len; i< DEViseGlobals.SLOTNUMSIZE; ++i)
				aStr += " ";
			os.writeBytes(aStr);
            os.flush();
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while sending integer data!");
        }        
    }
    
    public int receiveInt() throws DEViseNetException
    {
        try {
			byte[] intNum = new byte[DEViseGlobals.SLOTNUMSIZE];
            is.readFully(intNum);
			String aStr = new String(intNum);
			aStr = aStr.substring(0, aStr.indexOf(' '));
			Integer	aInt = new Integer(aStr);
			return aInt.intValue();
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while receiving integer data!");
        }            
    }
    
    public void sendCmd(String cmd, short flag) throws DEViseNetException
    {
        short nelem = 0, size = 0;
        short i;
        
        try  {
            String[] cmdBuffer = DEViseGlobals.parseStr(cmd, true);
            if (cmdBuffer == null)
                throw new DEViseNetException("DEVise Command Socket Error: NULL API Command!");
            
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
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while sending API commands!");
        }
    }
    
    public String receiveRsp(boolean header) throws DEViseNetException
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
                    response = response + (new String(arg, 0, argsize - 1));
                }
                
                if (!isEnd)  {
                    response = response + "\n";
                }
            }
            
            return response;
        }  catch (IOException e)  {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while receiving API responses!");
        }
    }
    
    public void closeSocket() throws DEViseNetException
    {
        try  {                               
            os.close();
            is.close();
            socket.close();
        }  catch (IOException e)  {
            throw new DEViseNetException("DEVise Command Socket Error: Communication Error occured while closing socket connection!");
        }
    }    
}
 
