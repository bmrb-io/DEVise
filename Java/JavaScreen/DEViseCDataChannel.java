import  java.net.*; 
import  java.io.*;
import  java.util.*;
import  java.awt.*;

public class DEViseCDataChannel implements Runnable
{
    jsdevisec jsc = null;
    DEViseImgSocket imgSocket = null;
    DEViseCmdSocket cmdSocket = null;
    Thread dataChannel = null;
    String command = null;
    String response = null;
    String [] rsp = null;

    public DEViseCDataChannel(jsdevisec what, DEViseCmdSocket socket1, DEViseImgSocket socket2)
    {        
        jsc = what;
        cmdSocket = socket1;
        imgSocket = socket2;
    }
    
    public boolean start(String arg)
    {   
        if (arg == null) {
            return false;
        }
        
        command = arg;
        response = null; 
        rsp = null;
        
        if (dataChannel == null)  {
            dataChannel = new Thread(this);
            dataChannel.start();
        }
        
        return true;
    }
    
    public void stop()
    {
        if (dataChannel != null && dataChannel.isAlive())  {
            //DEViseDebugInfo.println("I am in stop");
            dataChannel.stop();
            //DEViseDebugInfo.println("I am in stop again");
        }
        
        command = null;                        
        dataChannel = null;
    }                                                     

    public void quit()
    {
        String result = DEViseGlobals.showMsg(jsc, "Do you really want to quit?", "Confirm", DEViseGlobals.OP_YESNO);
        if (result.equals(DEViseGlobals.NO_OP))  {
            return;
        }
                
        //stop();

        DEViseDebugInfo.closefile();
           
        try {
            cmdSocket.sendCmd("Exit", DEViseGlobals.API_ACK);            
            if (cmdSocket != null)
                cmdSocket.closeSocket();        
            if (imgSocket != null)
                imgSocket.closeSocket();
        } catch (DEViseNetException e) {
            System.exit(1);
        }
        
        System.exit(0);
    }
    
    public void exitOnError(String msg)
    {
        String result = DEViseGlobals.showMsg(jsc, "Error occurs in the Program\n" + msg + "\nDo you want to exit the program?", "Confirm", DEViseGlobals.OP_YESNO);
        if (result.equals(DEViseGlobals.NO_OP))  {
            return;
        }
        
        //stop();

        DEViseDebugInfo.closefile();
           
        try {
            if (imgSocket != null)
                imgSocket.closeSocket();
            if (cmdSocket != null)
                cmdSocket.closeSocket();        
        } catch (DEViseNetException e) {
            System.exit(1);
        }
        
        System.exit(1);
    }
            
    public void run()
    {   
        if (command.equals("")) {
            // do nothing
        } else {
            jsc.startAnim();
            
            try {
                DEViseDebugInfo.println("Sending: " + command);
                cmdSocket.sendCmd(command, DEViseGlobals.API_ACK);
                //DEViseDebugInfo.println("Sended!\nReceiving Response ...");                
                response = cmdSocket.receiveRsp(false);
                DEViseDebugInfo.println("Received: \n" + response);
                rsp = parseStr(response);
                if (rsp == null) {
                    // do nothing now
                } else {
                    if (!rsp[rsp.length - 1].equals("Done")) {
                        exitOnError("DEVise Server Failed!");
                    } else {
                        parseRsp();
                    }
                }
            } catch (DEViseNetException e) {
                exitOnError(e.getMessage());
            }
            
            jsc.stopAnim();             
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
    
    private void parseRsp() throws DEViseNetException
    {
        for (int i = 0; i < rsp.length; i++) {
            String[] cmd = DEViseGlobals.parseStr(rsp[i], false);
            if (cmd == null) 
                throw new DEViseNetException("Trash Data received from DEVise Server!");                           
            
            if (cmd[0].equals("CreateWindow")) {
                DEViseDebugInfo.println("Start receiving image data for window " + cmd[1] + " ...");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[6])).intValue());
		DEViseDebugInfo.println("Finish receiving image data for window " + cmd[1]);
                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);            
                try  {
                	tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }            
                
                if (tracker.isErrorID(0))  
            	    throw new DEViseNetException("Can not build image from image data received!");
            	
            	DEViseImageView view = null;
                try  {
                    Rectangle loc = new Rectangle((Integer.valueOf(cmd[2])).intValue(), (Integer.valueOf(cmd[3])).intValue(),
                                                  (Integer.valueOf(cmd[4])).intValue(), (Integer.valueOf(cmd[5])).intValue());                    
                    view = new DEViseImageView(jsc, cmd[1], loc, image);
                }  catch (DEViseException e) {
                    throw new DEViseNetException("Can not open view!" + e.getMessage());
                }            
                 
                jsc.addView(view);
            } else if (cmd[0].equals("OpenAll")) {
                jsc.jscOpen();
            } else if (cmd[0].equals("UpdateGdata")) {
                String[] label = new String[cmd.length - 1];
                for (i = 0; i < cmd.length - 1; i++) 
                    label[i] = cmd[i + 1];                       
                jsc.viewControl.updateControl(label); 
            } else if (cmd[0].equals("UpdateWindow")) {
		DEViseDebugInfo.println("Start receiving image data for window " + cmd[1] + " ...");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[2])).intValue());
		DEViseDebugInfo.println("Finish receiving image data for window " + cmd[1]);
                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);            
                try  {
                	tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }            
                
                if (tracker.isErrorID(0))  
            	    throw new DEViseNetException("Can not build image from image data received!");
            	
                DEViseImageView view = jsc.getCurrentView();
                view.setNewImage(image);
                view.repaint();
            } else if (cmd[0].equals("UpdateRecordValue")) {
                jsc.viewInfo.updateInfo((Float.valueOf(cmd[1])).floatValue());
            } else if (cmd[0].equals("Done")) {
                return;
            } else if (cmd[0].equals("CloseAll")) {
                jsc.jscClose();
            } else {
                throw new DEViseNetException("Unrecognized commands received from DEVise Server!");                
            }
        }
    }        
}
