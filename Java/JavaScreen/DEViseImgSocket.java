// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
// Revision 1.12  1998/06/11 15:07:46  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import  java.io.*;
import  java.net.*;

// Socket used to send and receive Image data between DEViseServer and DEViseClient
public class DEViseImgSocket
{   
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
    //private BufferedOutputStream os = null;
        
    public DEViseImgSocket(Socket arg) throws IOException
    {
        socket = arg;        
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        //os = new BufferedOutputStream(socket.getOutputStream(), 512 * 1024);
        is = new DataInputStream(socket.getInputStream());
        try {
            socket.setSoTimeout(DEViseGlobals.DEFAULTIMGSOCKETTIMEOUT);
        } catch (SocketException e) {
            YDebug.println(e.getMessage());
        }
    }
               
    public DEViseImgSocket(String hostname, int port) throws UnknownHostException, IOException
    {
        socket = new Socket(hostname, port); 
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        //os = new BufferedOutputStream(socket.getOutputStream(), 512 * 1024);
        is = new DataInputStream(socket.getInputStream());
        try {
            socket.setSoTimeout(DEViseGlobals.DEFAULTIMGSOCKETTIMEOUT);
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
            throw new YException("DEVise Image Socket Error: Communication Error occured while sending data!");
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
            throw new YException("DEVise Image Socket Error: Communication Error occured while receiving data!");
        }
    }
  
    public void sendInt(int data) throws YException
    {
        try {
            os.writeInt(data);
            os.flush();
        } catch (IOException e) {
            throw new YException("DEVise Image Socket Error: Communication Error occured while sending integer data!");
        }        
    }
    
    public int receiveInt() throws YException, InterruptedIOException
    {
        try {
            return is.readInt();
        } catch (IOException e) {
            throw new YException("DEVise Image Socket Error: Communication Error occured while receiving integer data!");
        }            
    }
    
    public void sendImg(byte[] imageData) throws YException
    {   
        try {
            os.write(imageData, 0, imageData.length);
            os.flush();
        } catch (IOException e) {
            throw new YException("DEVise Image Socket Error: Communication Error occured while sending image data!");
        }
    }
    
    public byte[] receiveImg(int imageSize) throws YException, InterruptedIOException
    {   
        byte[] imageData = new byte[imageSize];
        
        try {
            is.readFully(imageData);
        } catch (IOException e) {
            throw new YException("DEVise Image Socket Error: Communication Error occured while receiving image data!");
        }
        
        return imageData;
    }  

    public void closeSocket()
    {
        try  {                               
            os.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Image Socket Error: Communication Error occured while closing socket connection!");
        }
        
        try {
            is.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Image Socket Error: Communication Error occured while closing socket connection!");
        }
           
        try {
            socket.close();
        }  catch (IOException e)  {
            YDebug.println("DEVise Image Socket Error: Communication Error occured while closing socket connection!");
        }
        
        os = null;
        is = null;
        socket = null;
    }    
}
 
