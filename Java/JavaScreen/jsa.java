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
import  java.applet.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class jsa extends Applet
{
    jsdevisec client = null;
    DEViseCmdSocket cmdSocket = null;
    DEViseImgSocket imgSocket = null;

    public void init()    
    {
        URL url = getDocumentBase();           
        DEViseGlobals.DEVISEHOST = url.getHost();
        showStatus(DEViseGlobals.DEVISEHOST);
                            
        MediaTracker tracker = new MediaTracker(this);
        Vector images = new Vector();
        Image image = null;
        for (int i = 0; i < 4; i++)  {        
            image = getImage(url, "devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }
        
            if (tracker.isErrorID(0)) {
                showStatus("Can not get symbols of DEVise!");
                return;
            }
                                                      
            images.addElement(image);
        }
        
        showStatus("Successfully get symbols of DEVise");
        
        String myID = DEViseGlobals.ERRORID;
        String hostname = "localhost";                            
        try  {
            cmdSocket = new DEViseCmdSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.CMDPORT);
            try {   
                myID = new String(cmdSocket.receiveBytes(DEViseGlobals.IDSIZE));
                
                if (myID == null || myID.length() != DEViseGlobals.IDSIZE || myID.equals(DEViseGlobals.ERRORID)) {
                    showStatus("Connection to DEVise Server is rejected!");
                    destroy();
                    return;
                }
            } catch (DEViseNetException e) {
                showStatus(e.getMessage());
                return;
            }
            
            imgSocket = new DEViseImgSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.IMGPORT);
            try {
                
                imgSocket.sendBytes(myID.getBytes());
                String tempID = new String(imgSocket.receiveBytes(DEViseGlobals.IDSIZE));
                if (tempID == null || tempID.equals(DEViseGlobals.ERRORID) || !tempID.equals(myID)) {
                    showStatus("Can not connect to DEVise Image Server!");
                    destroy();
                    return; 
                } 
                
                String rsp = cmdSocket.receiveRsp(false);
                if (!rsp.equals("JAVAC_Done")) {
                    showStatus("Can not connect to DEVise Image Server!");
                    destroy();
                    return;
                }
            } catch (DEViseNetException e) {
                showStatus("Communication error: " + e.getMessage());
                destroy();
                return;
            }
        }  catch (UnknownHostException e)  {
            showStatus("Can not find " + DEViseGlobals.DEVISEHOST);
            destroy();
            return;
        }  catch (IOException e)  {
            showStatus("Can not open connection to DEVise server at " + DEViseGlobals.DEVISEHOST);
            destroy();
            return;
        }
                
        DEViseGlobals.ISAPPLET = true;
        
        showStatus("Successfully connect to Server command and image socket!");

        client = new jsdevisec(cmdSocket, imgSocket, myID, images); 
    }

    public void start()
    {
        if (client != null && !client.getQuitStatus())
            client.displayMe(true);
    }

    public void stop()
    { 
        if (client != null && !client.getQuitStatus())
            client.displayMe(false);
    }

    public void destroy()
    {
        if (client != null && !client.getQuitStatus()) {
            client.quit();
            client = null;
        } 
        
        try {
            if (imgSocket != null) {
                imgSocket.closeSocket();
                imgSocket = null;
            }
            if (cmdSocket != null) {
                cmdSocket.closeSocket(); 
                cmdSocket = null;
            }
        } catch (DEViseNetException e) {
            showStatus("Can not close socket connection!");
        }    

        super.destroy();
    }
}
