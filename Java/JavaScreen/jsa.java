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

import  java.applet.*;
import  java.awt.event.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class jsa extends Applet
{
    URL baseURL = null;
    String hostname = null;
    
    Button button = null;
    TextArea info = null;
    
    boolean isStarted = false;
    jsdevisec client = null;
    DEViseCmdSocket cmdSocket = null;
    DEViseImgSocket imgSocket = null;

    public void init()
    {
        baseURL = getDocumentBase();
        hostname = baseURL.getHost();
        
        setLayout(new BorderLayout(10, 10));
        
        button = new Button("Start DEVise");
        button.setBackground(UIGlobals.buttonbgcolor);
        button.setForeground(UIGlobals.buttonfgcolor);
        button.setFont(UIGlobals.buttonfont);        
        add(button, BorderLayout.NORTH);
        
        info = new TextArea(5, 40);
        info.setBackground(UIGlobals.textbgcolor);
        info.setForeground(UIGlobals.textfgcolor);
        info.setFont(UIGlobals.textfont);
        add(info, BorderLayout.SOUTH);
        
        //pack();
        //show();
        
        button.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {
                        if (isStarted) {
                            if (client != null && !client.getQuitStatus()) {
                                info.append("Please close DEVise Screen first!\n");
                            } else {
                                closeJS();
                                isStarted = false;
                                button.setLabel("Start DEVise");
                            }
                        } else {
                            startJS();
                            if (isStarted)
                                button.setLabel("Stop DEVise");                            
                        }
                    }
                });               
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

        closeJS();
        isStarted = false;
        
        super.destroy();
    }
    
    public void closeJS()
    {
        try {
            info.append("Start closing socket connection to DEVise Server ...\n");
            if (imgSocket != null) {
                imgSocket.closeSocket();
                imgSocket = null;
            }
            if (cmdSocket != null) {
                cmdSocket.closeSocket();
                cmdSocket = null;
            }
            info.append("Successfully closing socket connection to DEVise Server!\n");
        } catch (YError e) {
            info.append("Can not close socket connection!\n");
        }
    }
    
    public void startJS() 
    {        
        info.append("Trying to load DEVise Symbol ...\n");
        MediaTracker tracker = new MediaTracker(this);
        Vector images = new Vector();
        Image image = null;
        for (int i = 0; i < 4; i++)  {
            image = getImage(baseURL, "devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }

            if (tracker.isErrorID(0)) {
                info.append("Can not load DEVise Symbol!\n");
                return;
            }

            images.addElement(image);
        }
        info.append("Successfully load DEVise Symbol!\n");
        
        isStarted = true;

        String myID = Globals.ERRORID;
        try  {
            info.append("Trying to connect to DEVise CMD Server at " + hostname + " ...\n");        
            cmdSocket = new DEViseCmdSocket(hostname, Globals.CMDPORT);
            try {
                myID = new String(cmdSocket.receiveBytes(Globals.IDSIZE));
                if (myID == null || myID.length() != Globals.IDSIZE || myID.equals(Globals.ERRORID)) {
                    info.append("Can not open connection to DEVise CMD Server!\n");
                    return;
                }
            } catch (YError e) {
                info.append(e.getMessage() + "\n");
                return;
            }
            info.append("Successfully connect to DEVise CMD Server!\n");
            
            info.append("Trying to connect to DEVise IMG Server at " + hostname + " ...\n");
            imgSocket = new DEViseImgSocket(hostname, Globals.IMGPORT);
            try {
                imgSocket.sendBytes(myID.getBytes());
                String tempID = new String(imgSocket.receiveBytes(Globals.IDSIZE));
                if (tempID == null || tempID.equals(Globals.ERRORID) || !tempID.equals(myID)) {
                    info.append("Can not open connection to DEVise IMG Server!\n");
                    return;
                }
                String rsp = cmdSocket.receiveRsp(false);
                if (!rsp.equals("JAVAC_Done")) {
                    info.append("Can not open connection to DEVise Img Server!\n");
                    return;
                } 
                info.append("Successfully connect to DEVise IMG Server!\n");
            } catch (YError e) {
                info.append(e.getMessage() + "\n");
                return;
            }
        }  catch (UnknownHostException e)  {
            info.append("Can not find host " + hostname + "\n");
            return;
        }  catch (IOException e)  {
            info.append("Communication Error -> " + e.getMessage() + "\n");
            return;
        }
        
        Globals.ISAPPLET = true;

        client = new jsdevisec(cmdSocket, imgSocket, myID, images);
    }
}

