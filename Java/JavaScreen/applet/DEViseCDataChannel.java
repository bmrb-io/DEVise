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

    public void run()
    {   
        if (command.equals("")) {
            // do nothing
        } else {
            jsc.startAnim();
            
            try {
                DEViseDebugInfo.println("Sending: " + command);
                cmdSocket.sendCmd(command, DEViseGlobals.API_JAVA);
                boolean isDone = false;
                while (!isDone) {
                    response = cmdSocket.receiveRsp(false);
                    DEViseDebugInfo.println("Received: \n" + response);
                    rsp = parseStr(response);
                    if (rsp == null) {
                        throw new DEViseNetException("Trash Data received from DEVise Server!");
                    } else {
                        if (rsp[rsp.length - 1].equals("JAVAC_Done") ||
                            rsp[rsp.length - 1].equals("JAVAC_Error") ||
                            rsp[rsp.length - 1].equals("JAVAC_Fail")) {
                            isDone = true;
                        }
                        
                        parseRsp();                        
                    }
                }
            } catch (DEViseNetException e) {
                jsc.exitOnError(e.getMessage());
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
            
            if (cmd[0].equals("JAVAC_CreateWindow")) {
                DEViseDebugInfo.print("Retrieving image data for window " + cmd[1] + " ... ");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[6])).intValue());
		        DEViseDebugInfo.println("Finished");
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
            } else if (cmd[0].equals("JAVAC_UpdateGdata")) {
                if ((cmd.length / 2) * 2 == cmd.length) {
                    ; // something is wrong here, but leave it now
                } else {
                    jsc.getCurrentView().updateGData(cmd);
                }
            } else if (cmd[0].equals("JAVAC_UpdateWindow")) {
		        DEViseDebugInfo.print("Retrieving image data for window " + cmd[1] + " ... ");
                byte[] imageData = imgSocket.receiveImg((Integer.valueOf(cmd[2])).intValue());
        		DEViseDebugInfo.println("Finished");
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
                //view.repaint();
            } else if (cmd[0].equals("JAVAC_UpdateRecordValue")) {
                jsc.viewInfo.updateInfo((Float.valueOf(cmd[1])).floatValue());
            } else if (cmd[0].equals("JAVAC_UpdateSessionList")) {
                jsc.updateSessionList(cmd);
            } else if (cmd[0].equals("JAVAC_Done")) {
                if (command.startsWith("JAVAC_OpenSession")) {
                    jsc.jscOpen();
                } else if (command.startsWith("JAVAC_CloseCurrentSession")) {
                    jsc.jscClose();
                } else {
                    ;// do nothing now
                }
                
                return;
            } else if (cmd[0].equals("JAVAC_Error")) {
                DEViseGlobals.showMsg(jsc, cmd[1], "Warning");
                return;
            } else if (cmd[0].equals("JAVAC_Fail")) {
                jsc.exitOnError("DEVise Server Failed(Unrecoverable)!");
                return;
            } else {
                throw new DEViseNetException("Unrecognized commands received from DEVise Server!");
            }
        }
    }        
}
