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
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    DEViseImgSocket imgSocket = null;
    DEViseCmdSocket cmdSocket = null;
    Vector jscommands = new Vector();

    String [] commands = null;    
    String command = null;
    String response = null;
    String [] rsp = null;    

    boolean isWaitForCmd = true;
    boolean isFatalError = false;
    
    public DEViseCmdDispatcher(jsdevisec what)    
    {
        if (what == null)
            YDebugInfo.println("Null argument received in DEViseCmdDispatcher construction!");
        
        jsc = what;
        jscreen = what.jscreen;
        imgSocket = jsc.imgSocket;
        cmdSocket = jsc.cmdSocket;
    }  
    
    public synchronized void addCmd(String cmd)
    {
        jscommands.insertElementAt(cmd, jscommands.size());
        /*
        if (isWaitForCmd)
            notifyAll();
        else
            return;
        */
    }
    
    public synchronized boolean getStatus()
    {
        return isWaitForCmd && !isFatalError;
    }
    
    public synchronized boolean getWaitStatus()
    {
        return isWaitForCmd;
    }
    
    public synchronized int getCmdSize()
    {
        return jscommands.size();
    }
    
    public synchronized String getCmd()
    {
        String cmd = (String)jscommands.elementAt(0);
        jscommands.removeElementAt(0);
        return cmd;
    }
    
    public synchronized void clearCmd()
    {
        jscommands.removeAllElements();
    }
    
    public synchronized void setWait(boolean flag)
    {
        isWaitForCmd = flag;
    }
    
    public synchronized void setError(boolean flag)
    {
        isFatalError = flag;
    }
    
    public void run()
    {
        boolean isQuit = false;
        setError(false);
        setWait(true);
        clearCmd();
            
        while (!isQuit) {
            setWait(true);
            
            while (getCmdSize() == 0) {
                try {
                    //setWait(true);
                    //wait();
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                }
            }
            
            setWait(false);
            String cmd = getCmd();
            if (cmd == null) {
                YDebugInfo.println("Null command received in dispatcher!");
            } else if (cmd.equals("ExitDispatcher")) {
                isQuit = true;
            } else {
                commands = parseStr(cmd);                
                if (commands != null) {
                    if (commands.length != 0) {
                        jsc.stopButton.setBackground(Color.red);
                        jsc.animPanel.start(); 
                        
                        for (int i = 0; i < commands.length; i++) {
                            command = commands[i];
                            try {
                                YDebugInfo.println("Sending: " + command);
                                cmdSocket.sendCmd(command, Globals.API_JAVA);
                                boolean isDone = false;
                                while (!isDone) {
                                    response = cmdSocket.receiveRsp(false);
                                    YDebugInfo.println("Received: \n" + response);
                                    rsp = parseStr(response);
                                    if (rsp == null) {
                                        throw new YError("Trash Data received from DEVise Server!");
                                    } else {
                                        String[] lastCmd = Globals.parseStr(rsp[rsp.length - 1], false);
                                        if (lastCmd == null)
                                            throw new YError("Trash Data received from DEVise Server!");
                                            
                                        if (lastCmd.length == 1 && (lastCmd[0].equals("JAVAC_Done") || lastCmd[0].equals("JAVAC_Error") || lastCmd[0].equals("JAVAC_Fail"))) {
                                        /*
                                        if (rsp[rsp.length - 1].equals("JAVAC_Done") || rsp[rsp.length - 1].equals("{JAVAC_Done}")
                                            || rsp[rsp.length - 1].equals("JAVAC_Error") || rsp[rsp.length - 1].equals("{JAVAC_Error}")
                                            || rsp[rsp.length - 1].equals("JAVAC_Fail") || rsp[rsp.length - 1].equals("{JAVAC_Fail}")) {
                                        */    
                                            isDone = true;
                                        }
                                        
                                        parseRsp();
                                    }
                                }
                            } catch (YError e) {
                                String result = UIGlobals.showMsg(jsc, "Fatal Error occurs in the Program\n" + e.getMessage() + "\nPlease press Exit to quit the program!", "Confirm");
                                setError(true);
                            }
                        }
                        
                        jsc.animPanel.stop();
                        jsc.stopButton.setBackground(UIGlobals.buttonbgcolor);
                    }
                }                                                                        
            }                    
        }
    }
    
    private String [] parseStr(String str)
    {       
        String[] outStr = null;
        
        if (str == null)
            return null;
        
        StringTokenizer token = new StringTokenizer(str, "\n", false);
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
    
    private void parseRsp() throws YError
    {
        for (int i = 0; i < rsp.length; i++) {
            String[] cmd = Globals.parseStr(rsp[i], false);
            if (cmd == null) 
                throw new YError("Trash Data received from DEVise Server!");                           
            
            if (cmd[0].equals("JAVAC_CreateWindow")) {
                YDebugInfo.print("Retrieving image data for window " + cmd[1] + " ... ");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[6])).intValue());
                YDebugInfo.println("Finished");
                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);            
                try  {
                    tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }            
                
                if (tracker.isErrorID(0))  
                    throw new YError("Can not build image from image data received!");
                
                String winname = cmd[1];
                int x = (Integer.valueOf(cmd[2])).intValue();
                int y = (Integer.valueOf(cmd[3])).intValue();
                int w = (Integer.valueOf(cmd[4])).intValue();
                int h = (Integer.valueOf(cmd[5])).intValue(); 
                w = w - x;
                h = h - y;
                Rectangle winloc = new Rectangle(x, y, w, h);
                Vector views = new Vector();
                int numOfV = (Integer.valueOf(cmd[7])).intValue();
                if (cmd.length != numOfV * 5 + 8) {
                    // something is wrong here, but leave it now
                    YDebugInfo.println("Received JAVAC_CreateWindow has incorrect format!");
                } else {
                    for (int j = 0; j < numOfV; j++) {
                        String name = cmd[8 + j * 5];
                        Rectangle rect = new Rectangle((Integer.valueOf(cmd[9 + j * 5])).intValue(), (Integer.valueOf(cmd[10 + j * 5])).intValue(),
                                                       (Integer.valueOf(cmd[11 + j * 5])).intValue(), (Integer.valueOf(cmd[12 + j * 5])).intValue());
                        DEViseView v = new DEViseView(jsc, name, rect, null, null);
                        views.addElement(v);
                    }
                } 
                                                  
                DEViseWindow win = new DEViseWindow(jsc, winname, winloc, image, views);

                jscreen.addWindow(win);
            } else if (cmd[0].equals("JAVAC_UpdateGData")) {
                if ((cmd.length / 2) * 2 == cmd.length) {
                    // something is wrong here
                    YDebugInfo.println("Received JAVAC_UpdateGData has incorrect format!");
                } else {
                    ; // do nothing now
                    //jsc.getCurrentView().updateGData(cmd);
                }
            } else if (cmd[0].equals("JAVAC_UpdateWindow")) {
                YDebugInfo.print("Retrieving image data for window " + cmd[1] + " ... ");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[2])).intValue());
                YDebugInfo.println("Finished");
                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);            
                try  {
                    tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }            
                
                if (tracker.isErrorID(0))  
                    throw new YError("Can not build image from image data received!");
                
                jscreen.updateWindow(cmd[1], image);
            } else if (cmd[0].equals("JAVAC_UpdateRecordValue")) {
                //jsc.viewInfo.updateInfo((Float.valueOf(cmd[1])).floatValue());
                jsc.viewInfo.updateInfo(null, cmd[1]);
            } else if (cmd[0].equals("JAVAC_UpdateSessionList")) {
                jsc.updateSessionList(cmd);
            } else if (cmd[0].equals("JAVAC_Done")) {            
                if (command.startsWith("JAVAC_OpenSession")) {
                    jscreen.updateScreen(true);
                } else if (command.startsWith("JAVAC_CloseCurrentSession")) {
                    jsc.updateSessionList(null);
                    jscreen.updateScreen(false);
                } else {
                    ; // do nothing now
                }
                
                return;
            } else if (cmd[0].equals("JAVAC_Error")) {
                UIGlobals.showMsg(jsc, cmd[1], "Warning");
                return;
            } else if (cmd[0].equals("JAVAC_Fail")) {
                //jsc.exitOnError("DEVise Server Failed(Unrecoverable)!");
                throw new YError("DEVise Server Failed (Unrecoverable)!");
            } else {
                throw new YError("Unrecognized commands received from DEVise Server!");
            }
        }
    }            
}
