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
// Revision 1.10  1998/08/14 17:48:10  hongyu
// *** empty log message ***
//
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
    String host = null;
    String user = null;
    String pass = null;

    String sessionName = null;

    Vector images = null;

    Button startButton = new Button("Start Java Screen");

    TextArea startInfo = null;

    jsdevisec jsc = null;

    public void init()
    {
        YGlobals.YISAPPLET = true;
        YGlobals.YISGUI = true;
        DEViseGlobals.browser = getAppletContext();

        //baseURL = getDocumentBase();
        baseURL = getCodeBase();
        host = baseURL.getHost();

        setLayout(new BorderLayout(0, 10));
        add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);

        startButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (jsc == null) {
                            startInfo.append("Start Java Screen ...\n");
                            jsc = new jsdevisec(host, user, pass, sessionName, images);
                            //startButton.setEnabled(false);
                        } else {
                            if (jsc.getQuitStatus()) {
                                startInfo.append("Start Java Screen ...\n");
                                jsc = null;
                                jsc = new jsdevisec(host, user, pass, sessionName, images);
                                //startButton.setEnabled(false);
                            } else {
                                startInfo.append("Java Screen already started!\n");
                            }
                        }
                    }
                });

        startInfo = new TextArea(8, 50);
        startInfo.setBackground(DEViseGlobals.textbgcolor);
        startInfo.setForeground(DEViseGlobals.textfgcolor);
        startInfo.setFont(DEViseGlobals.textfont);
        add(startInfo, BorderLayout.CENTER);

        setVisible(true);

        String version = System.getProperty("java.version");
        String vendor = System.getProperty("java.vendor");
        startInfo.append("The browser you used is: " + vendor + "\n");
        startInfo.append("The Java version your browser supported is: " + version + "\n");
        if (version.compareTo("1.1") < 0)  {
            startInfo.append("Error: Java version 1.1 or greater is needed to run this program\n");
            return;
        }

        checkParameters();

        if (user == null)
            user = DEViseGlobals.DEFAULTUSER;
        if (pass == null)
            pass = DEViseGlobals.DEFAULTPASS;

        YGlobals.start();
    }

    public void start()
    {
        if (sessionName != null) {
            startJS(true);
        } else {
            startJS(false);
        }
    }

    public void stop()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.displayMe(false);
        }
    }

    public void destroy()
    {
        closeJS();

        super.destroy();
    }

    public void closeJS()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.quit();
            jsc = null;
        }

        sessionName = null;
        images = null;
        user = null;
        pass = null;
    }

    public void startJS(boolean flag)
    {
        if (images == null) {
            images = new Vector();

            startInfo.append("Trying to load DEVise animation symbol ...\n");
            MediaTracker tracker = new MediaTracker(this);
            Image image = null;
            for (int i = 0; i < 11; i++)  {
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
        }

        startButton.setEnabled(true);

        if (jsc == null) {
            if (flag) {
                startInfo.append("Start Java Screen ...\n");
                jsc = new jsdevisec(host, user, pass, sessionName, images);
                //startButton.setEnabled(false);
            }
        } else {
            if (jsc.getQuitStatus()) {
                if (flag) {
                    startInfo.append("Start new Java Screen ...\n");
                    jsc = null;
                    jsc = new jsdevisec(host, user, pass, sessionName, images);
                    //startButton.setEnabled(false);
                }
            } else {
                jsc.displayMe(true);
            }
        }
    }

    private void checkParameters()
    {
        sessionName = getParameter("session");
        if (sessionName == null) {
            startInfo.append("Parameter session is not specified!\n");
        } else {
            startInfo.append("Parameter session: " + sessionName + " is used\n");
        }

        String debug = getParameter("debug");
        if (debug != null) {
            try {
                YGlobals.YDEBUG = Integer.parseInt(debug);
            } catch (NumberFormatException e) {
            }
        }
        
        String cmdport = getParameter("cmdport");
        if (cmdport != null) {
            try {
                int port = Integer.parseInt(cmdport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.CMDPORT = port;
                startInfo.append("Parameter cmdport " + port + " is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String imgport = getParameter("imgport");
        if (imgport != null) {
            try {
                int port = Integer.parseInt(imgport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.IMGPORT = port;
                startInfo.append("Parameter imgport " + port + " is used\n");
            } catch (NumberFormatException e) {
            }
        }
    }

    public String[][] getParameterInfo()
    {
        String [][] info = {
               // Parameter Name   Type of Value      Description
               //   {"debug",           "int",        "whether or not display debug information"},
               //   {"cmdport",            "int",        "jspop command port"},
               //   {"imgport",            "int",        "jspop image port"},
                  {"session",        "String",      "session file name"},
               };

        return info;
    }

    public String getAppletInfo()
    {
        return "DEVise Java Screen version 2.0\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }
}

