import  java.net.*; 
import  java.io.*;
import  java.util.*;

public class DEViseSDataChannel implements Runnable
{
    DEViseCmdServerSocket cmdServer = null;
    DEViseImgSocket imgSocket = null;
    DEViseCmdSocket cmdSocket = null;
    String hostName = null;
    boolean isExit = true;
    boolean isError = false;
    Random rand = new Random();    

    public DEViseSDataChannel(DEViseCmdServerSocket what, Socket socket) throws IOException
    {        
        cmdServer = what;
        cmdSocket = new DEViseCmdSocket(socket);
        hostName = socket.getInetAddress().getHostName();
    }
    
    public void close()
    {
        setStatus(true);
        cmdServer.decreaseCount(this);
           
        try {
            if (imgSocket != null)
                imgSocket.closeSocket();
            if (cmdSocket != null)
                cmdSocket.closeSocket();        
        } catch (DEViseNetException e) {
            // do nothing now
        }
    }
    
    public String getHostName()
    {   
        if (hostName != null)
            return hostName;
        else
            return "No Host";
    }
    
    public synchronized boolean getStatus()
    {
        return isExit;
    }
    
    public synchronized void setStatus(boolean flag)
    {
        isExit = flag;
    }
    
    public synchronized void putImgSocket(Socket what)
    {   
        try  {
            imgSocket = new DEViseImgSocket(what);
        } catch (IOException e) {
            System.out.println("Can not establish image socket connection to " + hostName);
            isError = true;            
        }
        
        //notifyAll();
    }
    
    public void run()
    {   
        if (!cmdServer.increaseCount(this)) {
            try {
                setStatus(true);
                cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);  
                close();
            } catch (DEViseNetException e) {
                System.out.println("Communication Error with " + hostName +" : " + e.getMessage());
            }
        } else {
            try {
                setStatus(false);
                cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);

                //wait for image socket, might be deadlock?
                while (imgSocket == null && !isError) {
                    try {
                        Thread.sleep(200);
                        //this.wait();
                    } catch (InterruptedException e) {
                    }
                }
                
                if (isError) {
                    System.out.println("Can not establish image data connection with client!");
                    cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);
                    close();                
                } else {
                    cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
                    System.out.println("Client connection from " + hostName + " is established.");
                }
            } catch (DEViseNetException e) {
                close();
                System.out.println("Communication Error with " + hostName + " : " + e.getMessage());
            }
        }
        
        while (!getStatus()) {
            try {
                String cmd = cmdSocket.receiveRsp(false);    
                //cmdSocket.sendCmd("Okey", DEViseGlobals.API_ACK);

                if (cmd.equals("Exit")) {
                    close();
                } else if (cmd.equals("Done")) {
                    // Last operation is successful, do nothing
                } else if (cmd.equals("Fail")) {
                    // Last operation is failed
                    System.out.println("Client send failing message!");
                } else if (cmd.startsWith("OpenSession")) {
                    if (!openSession())
                        cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);
                } else if (cmd.startsWith("CloseSession")) {
                    cmdSocket.sendCmd("CloseAll", DEViseGlobals.API_CTL);
                    cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
                } else if (cmd.startsWith("MouseAction_RubberBand")) {
                    if (!MouseAction_RubberBand(cmd))
                        cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);
                } else if (cmd.startsWith("MouseAction_Click")) {
                    if (!MouseAction_Click(cmd))
                        cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);
                } else if (cmd.startsWith("MouseAction_DoubleClick")) {
                    if (!MouseAction_DoubleClick(cmd))
                        cmdSocket.sendCmd("Fail", DEViseGlobals.API_ACK);
                } else {
                }
            } catch (DEViseNetException e) {
                close();
                System.out.println("Communication Error with " + hostName + " : " + e.getMessage());
            }
        }
    }
    
    private boolean MouseAction_RubberBand(String cmd) throws DEViseNetException
    {
        String[] argument = parseStr(cmd);
        String name = argument[1] + "-sample" + String.valueOf((int)(rand.nextFloat()*6) + ".jpg");
        byte[] imageData = getImage(name);
        if (imageData == null)
            return false;
        cmdSocket.sendCmd("UpdateWindow " + argument[1] + " " + imageData.length, DEViseGlobals.API_CTL);
        imgSocket.sendImg(imageData);
        imgSocket.flushStream();
        cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
        return true;
    }
    
    private boolean MouseAction_Click(String cmd) throws DEViseNetException
    {
        String[] argument = parseStr(cmd);
        String value = String.valueOf(rand.nextFloat());
        cmdSocket.sendCmd("UpdateRecordValue " + value, DEViseGlobals.API_CTL);
        cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
        return true;
    }
    
    private boolean MouseAction_DoubleClick(String cmd) throws DEViseNetException
    {
        String[] argument = parseStr(cmd);
        int number = (int)(rand.nextFloat() * 10) + 1;
        String label = "";
        for (int i = 0; i < number; i++)
            label = label + "http://" + argument[1] + ".homepage" + i + ".edu ";
            
        cmdSocket.sendCmd("UpdateGdata " + label, DEViseGlobals.API_CTL);
        cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
        return true;
    }
    
    private boolean openSession() throws DEViseNetException
    {   
        byte[] imageData = null;
        //String rsp = null;        
        for (int i = 0; i < 4; i++) {
            imageData = getImage("view" + (i + 1) + "-sample0.jpg");
            if (imageData == null) 
                return false;
            cmdSocket.sendCmd("CreateWindow view" + (i + 1) +" 0 0 400 300 " + imageData.length, DEViseGlobals.API_CTL);
            //System.out.println("Start sending file ...");
            imgSocket.sendImg(imageData);
            //System.out.println("Finish sending file ...");
            //rsp = cmdSocket.receiveRsp(false);
            //if (!rsp.equals("Done") 
            //    return false;
        }
        
        imgSocket.flushStream();
        cmdSocket.sendCmd("OpenAll", DEViseGlobals.API_CTL);
        cmdSocket.sendCmd("Done", DEViseGlobals.API_ACK);
        return true;
    } 
    
    private byte[] getImage(String name)
    {   
        try {
            //System.out.println("Start read file " + name);
            File file = new File(name);
            if (!file.exists())
                return null;
            
            int fileLength = (int)file.length();
            
            FileInputStream infile = new FileInputStream(file);
            
            byte[] data = new byte[fileLength];
            infile.read(data);
            //System.out.println("Finish read file " + name);
            return data;
        } catch (FileNotFoundException e) {
            System.out.println("Can not find file " + name);
            return null;
        } catch (SecurityException e) {
            System.out.println("Security violation while access file " + name);
            return null;
        } catch (NullPointerException e) {
            System.out.println("Null file name");
            return null;
        } catch (IOException e) {
            System.out.println("IO Error while reading from file " + name);
            return null;
        }                        
    }

    private String [] parseStr(String str)
    {       
        String[] outStr = null;
        
        if (str == null)
            return null;
        
        StringTokenizer token = new StringTokenizer(str);
        Vector tokenList = new Vector();
        while (token.hasMoreTokens()) {
            try {
                tokenList.addElement(token.nextToken());
            } catch (NoSuchElementException e) {
                // this should not be happening
            }
        }
        
        outStr = new String[tokenList.size()];
        for (int i = 0; i < outStr.length; i++) 
            outStr[i] = (String)tokenList.elementAt(i);
            
        return outStr;    
    }    
}
