// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.40  2000/07/26 15:53:49  venkatan
// Hand - Mouse Cursor is used in the 3D view
//
// Revision 1.39  2000/06/26 16:48:33  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.38  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.37  2000/03/23 16:26:18  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.36  1999/10/10 08:49:54  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.35  1999/08/03 07:37:46  hongyu
// add support for read animation symbol from JAR file           by Hongyu Yao
//
// Revision 1.34  1999/06/23 20:59:20  wenger
// Added standard DEVise header.
//

// ========================================================================

// jsa.java

import  java.applet.*;
import  java.awt.event.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class jsa extends Applet
{
    URL baseURL = null;

    String sessionName = null;
    int debugLevel = 0;



    Vector images = null;

    Button startButton = new Button("Click here to start JavaScreen");

    TextArea startInfo = null;

    boolean isInit = true;
    jscframe jsf = null;

    public void init()
    {
        DEViseUIGlobals.isApplet = true;
        DEViseUIGlobals.inBrowser = false;

        isInit = true;

        DEViseUIGlobals.browser = getAppletContext();


        setLayout(new BorderLayout(0, 10));

        add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);

        startButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (jsf == null) {
                            startInfo.append("Start Java Screen ...\n");
                            jsf = new jscframe(images, debugLevel, sessionName);
                            //startButton.setEnabled(false);
                        } else {
                            if (jsf.isQuit()) {
                                startInfo.append("Start Java Screen ...\n");
                                jsf = null;
                                jsf = new jscframe(images, debugLevel, sessionName);
                                //startButton.setEnabled(false);
                            } else {
                                startInfo.append("Java Screen already started!\n");
                            }
                        }
                    }
                });

        startInfo = new TextArea(8, 50);
        startInfo.setBackground(DEViseUIGlobals.textBg);
        startInfo.setForeground(DEViseUIGlobals.textFg);
        startInfo.setFont(DEViseUIGlobals.textFont);
        add(startInfo, BorderLayout.CENTER);

        setVisible(true);

        checkParameters();

        //baseURL = getDocumentBase();
        baseURL = getCodeBase();
        DEViseGlobals.hostname = baseURL.getHost();
        DEViseGlobals.username = DEViseGlobals.DEFAULTUSER;
        DEViseGlobals.password = DEViseGlobals.DEFAULTPASS;

        String version = System.getProperty("java.version");
        String vendor = System.getProperty("java.vendor");
        startInfo.append("The browser you used is: " + vendor + "\n");
        startInfo.append("The Java version your browser supported is: " + version + "\n");
        if (version.compareTo("1.1") < 0)  {
            startInfo.append("Error: Java version 1.1 or greater is needed to run this program\n");
            isInit = false;
            return;
        }
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
        if (jsf != null && !jsf.isQuit()) {
            jsf.displayMe(false);
        }
    }


    public void destroy()
    {
        closeJS();

        super.destroy();
    }

    public void closeJS()
    {
        if (jsf != null && !jsf.isQuit()) {
            jsf.quit();
            jsf = null;
        }

        sessionName = null;
        images = null;
    }

    public void startJS(boolean flag)
    { 
        if (images == null) {
            images = new Vector();

            startInfo.append("Trying to load DEVise animation symbol ...\n");
            MediaTracker tracker = new MediaTracker(this);
            Image image = null;
            String imageName = null;
            byte[] imageData = new byte[2000];
            for (int i = 0; i < 11; i++)  {
                //image = getImage(baseURL, "devise" + i + ".gif");

                // get image from JAR file
                try {
                    imageName = "devise" + i + ".gif";
                    InputStream is = getClass().getResourceAsStream(imageName);
                    BufferedInputStream bis = new BufferedInputStream(is);
                    int count = bis.read(imageData, 0, 2000);
                    //byte idata;
                    //int count = 0;
                    //while ((idata = bis.read()) != -1) {
                    //    imageData[count] = idata;
                    //    count++;
                    //}
                    image = Toolkit.getDefaultToolkit().createImage(imageData, 0, count);
                } catch (IOException e) {
                    startInfo.append("Can not load DEVise animation symbol!\nStarting Java Screen without animation effect!\n");
                    images = null;
                    break;
                }

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

        if (jsf == null) {
            if (flag) {
                startInfo.append("Start Java Screen ...\n");
                jsf = new jscframe(images, debugLevel, sessionName);
                //startButton.setEnabled(false);
            }
        } else {
            if (jsf.isQuit()) {
                if (flag) {
                    startInfo.append("Start new Java Screen ...\n");
                    jsf = null;
                    jsf = new jscframe(images, debugLevel, sessionName);
                    //startButton.setEnabled(false);
                }
            } else {
                jsf.displayMe(true);
            }
        }
	repaint();
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
                debugLevel = Integer.parseInt(debug);
            } catch (NumberFormatException e) {
                debugLevel = 0;
            }
        } else {
            debugLevel = 0;
        }

        String cmdport = getParameter("cmdport");
        if (cmdport != null) {
            try {
                int port = Integer.parseInt(cmdport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.cmdport = port;
                startInfo.append("Parameter cmdport " + port + " is used\n");
            } catch (NumberFormatException e) {
                DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
            }
        } else {
            DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        }

        String imgport = getParameter("imgport");
        if (imgport != null) {
            try {
                int port = Integer.parseInt(imgport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.imgport = port;
                startInfo.append("Parameter imgport " + port + " is used\n");
            } catch (NumberFormatException e) {
                DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
            }
        } else {
            DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
        }

        String screen = getParameter("screensize");
        if (screen != null) {
            try {
                String[] str = DEViseGlobals.parseStr(screen, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                DEViseUIGlobals.screenSize.width = x;
                DEViseUIGlobals.screenSize.height = y;

                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String rsize = getParameter("rubberbandlimit");
        if (rsize != null) {
            try {
                String[] str = DEViseGlobals.parseStr(rsize, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                if (x < 0) {
                    x = 0;
                }

                if (y < 0) {
                    y = 0;
                }

                DEViseUIGlobals.rubberBandLimit.width = x;
                DEViseUIGlobals.rubberBandLimit.height = y;
                startInfo.append("Parameter rubber band limit (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String bg = getParameter("bgcolor");
        if (bg != null) {
            try {
                String[] str = DEViseGlobals.parseStr(bg, "+");
                if (str == null || str.length != 3) {
                    throw new NumberFormatException();
                }

                int r = Integer.parseInt(str[0]);
                int g = Integer.parseInt(str[1]);
                int b = Integer.parseInt(str[2]);
                if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                    throw new NumberFormatException();
                }

                Color c = new Color(r, g, b);
                DEViseUIGlobals.bg = c;
                startInfo.append("Parameter bgcolor (" + r + ", " + g + ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String fg = getParameter("fgcolor");
        if (fg != null) {
            try {
                String[] str = DEViseGlobals.parseStr(fg, "+");
                if (str == null || str.length != 3) {
                    throw new NumberFormatException();
                }

                int r = Integer.parseInt(str[0]);
                int g = Integer.parseInt(str[1]);
                int b = Integer.parseInt(str[2]);
                if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                    throw new NumberFormatException();
                }

                Color c = new Color(r, g, b);
                DEViseUIGlobals.fg = c;
                startInfo.append("Parameter fgcolor (" + r + ", " + g + ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String dl = getParameter("log");
        if (dl != null) {
            try {
		DEViseGlobals.debugLog = (Integer.parseInt(dl) != 0);
            } catch (NumberFormatException e) {
		DEViseGlobals.debugLog = false;
            }
        } else {
	    DEViseGlobals.debugLog = false;
        }
    }

    public String[][] getParameterInfo()
    {
        String [][] info = {
               // Parameter Name   Type of Value      Description
                  {"debug",           "int",        "whether or not display debug information"},
                  {"cmdport",         "int",        "jspop command port"},
                  {"imgport",         "int",        "jspop image port"},
                  {"session",        "String",      "session file name"},
                  {"screensize",     "String",      "assumed java screen dimension"},
                  {"rubberbandlimit","String",      "Minimum dimension for a rubberband"},
                  {"bgcolor",        "String",      "RGB values for bgcolor"},
                  {"fgcolor",        "String",      "RGB values for fgcolor"}
               };

        return info;
    }

    public String getAppletInfo()
    {
        return "DEVise Java Screen version 2.0\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }
}

class jscframe extends Frame
{
    public jsdevisec jsc = null;

    public jscframe(Vector images, int debugLevel, String sessionName)
    {
        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        DEViseUIGlobals.maxScreenSize.width = dim.width - 80;
        DEViseUIGlobals.maxScreenSize.height = dim.height - 120;
        DEViseUIGlobals.minScreenSize.width = 300;
        DEViseUIGlobals.minScreenSize.height = 240;

        jsc = new jsdevisec(this, images, debugLevel, sessionName);
        add(jsc);
        setTitle(DEViseUIGlobals.javaScreenTitle);
        pack();

        Point loc = new Point(0, 0);
        Dimension size = getSize();
        loc.y = loc.y + dim.height / 2 - size.height / 2;
        loc.x = loc.x + dim.width / 2 - size.width / 2;
        if (loc.y < 0)
            loc.y = 0;
        if (loc.x < 0)
            loc.x = 0;

        setLocation(loc);

        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        show();
    }

    public boolean isQuit()
    {
        return jsc.getQuitStatus();
    }

    public void quit()
    {
        jsc.quit();
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.quit();

            return;
        }

        super.processEvent(event);
    }

    public void displayMe(boolean isShow)
    {
        if (isShow) {
            if (!isShowing()) {
                setVisible(true);
            }
        } else {
            if (isShowing()) {
                setVisible(false);
            }
        }
    }


}
