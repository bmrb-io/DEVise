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
// Revision 1.2  1998/06/11 15:07:45  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    public jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    DEViseImgSocket imgSocket = null;
    DEViseCmdSocket cmdSocket = null;

    // status = 0, dispatcher running    
    // status = 1, dispatcher wait for command
    // status = 2, dispatcher process command
    // status = 3, dispatcher quit 
    // status = 4, dispatcher quit because of unrecoverable error    
    private int status;
    private Vector jscommands = new Vector();
    public Counter counter = null;
    private Thread counterThread = null;
        
    public DEViseCmdDispatcher(jsdevisec what)    
    {        
        jsc = what;
        jscreen = jsc.jscreen;
        imgSocket = jsc.jscomm.imgSocket;
        cmdSocket = jsc.jscomm.cmdSocket;
        counter = new Counter(this, jsc.jscomm.timeout);
        counterThread = new Thread(counter);
        counterThread.start();
    }
    
    public synchronized void startTimer()
    {
        if (!counterThread.isAlive()) {
            counterThread = new Thread(counter);
            counterThread.start();
        }
    }  
    
    public synchronized void addCmd(String rsp)
    {   
        if (status < 3) {
            jscommands.insertElementAt(rsp, jscommands.size());
            notifyAll();
        } else {
            YDebug.println("Warning: Can not insert command into pool because dispatcher is already quit!");
        }
    }
    
    public synchronized String getCmd()
    {
        while (jscommands.size() == 0) {
            status = 1; 
            try { 
                wait();
            } catch (InterruptedException e) {
            }
        }
        
        status = 0;
        counter.reset();
        String cmd = (String)jscommands.elementAt(0);
        jscommands.removeElementAt(0);
        return cmd;
    }
    
    public synchronized void clearCmd()
    {
        jscommands.removeAllElements();
    }
    
    public synchronized int getStatus()
    {
        return status;
    }
    
    public synchronized void setStatus(int arg)
    {
        status = arg;
    }
    
    public void run()
    {
        String cmd = null;
        String [] commands = null;    
        String command = null;
        String[] response = null;
        
        setStatus(0);
        startTimer();
        
        while (getStatus() == 0) {
            cmd = getCmd();
            
            if (cmd == null) {
                YDebug.println("Null command received from command pool!");            
            } else if (cmd.equals("ExitDispatcher")) {
                setStatus(3);
                YDebug.println("Receive command dispatcher quit signal...");
            } else if (cmd.equals("Timeout")) {
                setStatus(2);
                
                jsc.suspend(true);
                
                setStatus(0);
            } else {
                commands = DEViseGlobals.parseStr(cmd);
                
                if (commands == null) {
                    YDebug.println("Invalid command received from command pool: " + cmd);
                } else if (commands.length == 0) {
                    YDebug.println("Zero length command received from command pool: " + cmd);
                } else {                    
                    try {
                        setStatus(2);
                        jsc.animPanel.start();
                        
                        for (int i = 0; i < commands.length; i++) {
                            command = commands[i];
                                                
                            YDebug.println("Command Dispatcher is processing command: " + command);
                            
                            response = cmdSocket.sendCommand(command);
                            parseRsp(command, response);
                        
                            YDebug.println("Command Dispatcher finish processing command: " + command);
                        }
                        
                        jsc.animPanel.stop();
                        setStatus(0);
                    } catch (YException e) {                            
                        setStatus(4);
                        jsc.animPanel.stop();
                        String result = YGlobals.showMsg(jsc, "Program encounter unrecoverable error\n" + e.getMessage() 
                                            + "\nDo you wish to continue(Y) or restart(N)?", "Confirm", YGlobals.MBXYESNO);                                                
                        if (result.equals(YGlobals.IDYES)) {
                            clearCmd();
                            setStatus(0);
                        } else {
                            jsc.suspend(false);
                        }
                    } catch (InterruptedIOException e) {
                        jsc.animPanel.stop();
                        
                        String result = YGlobals.showMsg(jsc, "Network timeout is reached while sending command: " + command
                                        + "\nDo you wish to continue next command(Y) or restart(N)?", "Confirm", YGlobals.MBXYESNO);                          
                        if (result.equals(YGlobals.IDYES)) {
                            setStatus(0);
                        } else {
                            setStatus(4);
                            jsc.suspend(false);                        
                        }                            
                    }
                }
            }
        }
        
        clearCmd();
        
        if (getStatus() == 3) {
            YDebug.println("Command dispatcher is quitted!");
        } else if (getStatus() == 4) {
            YDebug.println("Command dispatcher is quitted because of unrecoverable error!");
        }
    }
    
    private void parseRsp(String command, String[] response) throws YException, InterruptedIOException
    {   
        String[] rsp = null;
        
        for (int i = 0; i < response.length; i++) {
            rsp = DEViseGlobals.parseString(response[i]);
            if (rsp == null)
                throw new YException("Incorrectly formatted command received from server: " + response[i]);
            
            try {    
                if (rsp[0].equals("JAVAC_CreateWindow")) {
                    if (rsp.length < 8)
                        throw new YException("Incorrectly formatted command received from server: " + response[i]);
                        
                    YDebug.println("Retrieving image data for window " + rsp[1] + " ... ");
                    byte[] imageData = imgSocket.receiveImg((Integer.valueOf(rsp[6])).intValue());
                    YDebug.println("Successfully retrieve image data");
                    MediaTracker tracker = new MediaTracker(jsc);
                    Toolkit kit = jsc.getToolkit();
                    Image image = kit.createImage(imageData);
                    tracker.addImage(image, 0);            
                    try  {
                        tracker.waitForID(0);
                    }  catch (InterruptedException e)  {
                    }            
                    
                    if (tracker.isErrorID(0))  
                        throw new YException("Can not create image from the data received from server!");
                    
                    String winname = rsp[1];
                    int x = (Integer.valueOf(rsp[2])).intValue();
                    int y = (Integer.valueOf(rsp[3])).intValue();
                    int w = (Integer.valueOf(rsp[4])).intValue();
                    int h = (Integer.valueOf(rsp[5])).intValue(); 
                    w = w - x;
                    h = h - y;
                    Rectangle winloc = new Rectangle(x, y, w, h);
                    Vector views = new Vector();
                    int numOfV = (Integer.valueOf(rsp[7])).intValue();
                    if (rsp.length != numOfV * 5 + 8) {
                        throw new YException("Incorrectly formatted command receive from server: " + response[i]);
                    } else {
                        for (int j = 0; j < numOfV; j++) {
                            String name = rsp[8 + j * 5];
                            Rectangle rect = new Rectangle((Integer.valueOf(rsp[9 + j * 5])).intValue(), (Integer.valueOf(rsp[10 + j * 5])).intValue(),
                                                           (Integer.valueOf(rsp[11 + j * 5])).intValue(), (Integer.valueOf(rsp[12 + j * 5])).intValue());
                            DEViseView v = new DEViseView(jsc, name, rect, null, null);
                            views.addElement(v);
                        }
                    } 
                                                      
                    DEViseWindow win = new DEViseWindow(jsc, winname, winloc, image, views);
            
                    jscreen.addWindow(win);
                } else if (rsp[0].equals("JAVAC_UpdateWindow")) {
                    YDebug.println("Retrieving image data for window " + rsp[1] + " ... ");
                    byte[] imageData = imgSocket.receiveImg((Integer.valueOf(rsp[2])).intValue());
                    YDebug.println("Successfully retrieve image data");
                    MediaTracker tracker = new MediaTracker(jsc);
                    Toolkit kit = jsc.getToolkit();
                    Image image = kit.createImage(imageData);
                    tracker.addImage(image, 0);            
                    try {
                        tracker.waitForID(0);
                    }  catch (InterruptedException e) {
                    }            
                    
                    if (tracker.isErrorID(0))  
                        throw new YException("Can not create image from the data received from server!");
                    
                    jscreen.updateWindow(rsp[1], image);
                } else if (rsp[0].equals("JAVAC_UpdateSessionList")) {
                    jsc.updateSessionList(rsp);
                } else if (rsp[0].equals("JAVAC_Done")) {            
                    if (command.startsWith("JAVAC_OpenSession")) {
                        jsc.isSessionOpened = true;
                        jscreen.updateScreen(true);
                    } else if (command.startsWith("JAVAC_CloseCurrentSession")) {
                        jsc.isSessionOpened = false;
                        jscreen.updateScreen(false);
                    } else {
                    }
                } else if (rsp[0].equals("JAVAC_Error")) {
                    if (command.startsWith("JAVAC_GetSessionList")) {
                        jsc.updateSessionList(null);
                    }
                    
                    YGlobals.showMsg(jsc, rsp[1], "Warning");
                } else if (rsp[0].equals("JAVAC_Fail")) {
                    if (command.startsWith("JAVAC_GetSessionList")) {
                        jsc.updateSessionList(null);
                    }
                    
                    throw new YException(rsp[1]);
                } else {
                    YGlobals.showMsg(jsc, "Unsupported command received from server: " + response[i]);
                }
            } catch (NumberFormatException e) {
                throw new YException("Incorrect value received from server: " + response);
            }
        }    
    }            
} 


class Counter implements Runnable
{
    DEViseCmdDispatcher dispatcher = null;
    long time = 0;
    long timeout = 0;
              
    public Counter(DEViseCmdDispatcher arg, long to) 
    {
        dispatcher = arg;
        timeout = to * 60; // timeout specified in minutes
    }
    
    public synchronized void reset()
    {
        time = 0;
    }
    
    // advance in one second step
    public synchronized void increaseTime()
    {
        time++;
    }
    
    public synchronized boolean checkTimeout()
    {
        if (timeout <= 0) {
            time = 0;
            return false;
        }
        
        if (timeout > time)
            return false;
        else
            return true;
    }
    
    public synchronized long getTime()
    {
        return time;
    }
    
    public void run()
    { 
        reset();
        
        while (!checkTimeout()) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
            }
            
            increaseTime();
        }
        
        if (dispatcher.getStatus() > 2) {
            dispatcher.jsc.suspend(true);
        } else {
            dispatcher.addCmd("Timeout");
        }
    }
}
