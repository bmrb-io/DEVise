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
// Revision 1.5  1998/07/09 17:38:40  hongyu
// *** empty log message ***
//
// Revision 1.4  1998/06/23 17:53:25  wenger
// Improved some error messages (see bug 368).
//
// Revision 1.3  1998/06/11 15:07:52  wenger
// Added standard header to Java files.
//
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
    int port;
    String sessionName = null;
    
    TextArea startInfo = null;
    
    boolean isStarted = false;
    jsdevisec jsc = null;
    DEViseComm jscomm = null;

    public void init()
    {
        YGlobals.ISAPPLET = true;

        //baseURL = getDocumentBase();
        baseURL = getCodeBase();
        hostname = baseURL.getHost();
        
        startInfo = new TextArea(8, 50);
        startInfo.setBackground(DEViseGlobals.textbgcolor);
        startInfo.setForeground(DEViseGlobals.textfgcolor);
        startInfo.setFont(DEViseGlobals.textfont);
        add(startInfo);
        
        String version = System.getProperty("java.version");
        String vendor = System.getProperty("java.vendor");
        startInfo.append("The browser you used is: " + vendor + "\n");
        startInfo.append("The Java version your browser supported is: " + version + "\n");
        if (version.compareTo("1.1") < 0)  {
            startInfo.append("Error: Java version 1.1 or greater is needed to run this program\n");
            return; 
        }
        
        // create a YLogFile class that does nothing when its methods called
        YGlobals.LogFile = new YLogFile();
        
        if (YGlobals.ISDEBUG) {
            YGlobals.DebugInfo = new YDebug(true);
        } else {
            // create a YDebug class that does nothing when its methods called
            YGlobals.DebugInfo = new YDebug();
        }                
                        
        checkParameters();
        
        startJS();
    }

    public void start()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.displayMe(true);
            YDebug.setVisible(true);
        }
    }

    public void stop()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.displayMe(false); 
            YDebug.setVisible(false);
        }
    }

    public void destroy()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.quit();
            closeJS();
            jsc = null;
        }
        
        super.destroy();
    }
    
    public void closeJS()
    {
        isStarted = false;        
        jscomm.disconnect(true);
    }
    
    public void startJS() 
    {        
        startInfo.append("Trying to load DEVise animation symbol ...\n");        
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
                startInfo.append("Can not load DEVise animation symbol!\nStarting Java Screen without animation effect!\n");
                images = null;
                break;
            }

            images.addElement(image);
        } 
        if (images != null)
            startInfo.append("DEVise animation symbol successfully loaded\n");
        
        jscomm = new DEViseComm(hostname, port, null, null);
        
        if (sessionName != null) {
            startInfo.append("Try to make socket connection to server ...\n");
            try {
                jscomm.connect();
            } catch (YException e) {
                startInfo.append(e.getMessage());
                return;
            }
        }
        
        jsc = new jsdevisec(jscomm, images, sessionName);
        
        isStarted = true;
    } 
    
    private void checkParameters()
    {
        String str = null;
        str = getParameter("port");
        if (str != null) {
            try {
                port = Integer.parseInt(str);
                if (port < 1 || port > 65535) 
                    throw new NumberFormatException();
            } catch (NumberFormatException e) {
                port = DEViseGlobals.DEFAULTCMDPORT;
                startInfo.append("Incorrect port value specified! Default value is used\n");
            }
        } else {
            startInfo.append("Parameter port is not specified! Default value is used\n");
        }
        
        sessionName = getParameter("session");
        if (sessionName == null) {
            startInfo.append("Parameter session is not specified!\n");
        }
        
        String debug = getParameter("debug");
        if (debug.equals("true")) {
            YGlobals.ISDEBUG = true;
            startInfo.append("Debug Enabled!\n");
        } else {
            YGlobals.ISDEBUG = false;
        }
    }
    
    public String[][] getParameterInfo()
    {
        String [][] info = {
               // Parameter Name   Type of Value      Description
               //   {"debug",           "int",        "whether or not display debug information"},
                  {"port",            "int",        "jspop command port"},
                  {"session",        "String",      "session file name"},
               };
               
        return info;
    }
    
    public String getAppletInfo()
    {
        return "DEVise Java Screen version 2.0\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }    
}

