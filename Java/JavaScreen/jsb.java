// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$
// Revision 1.6  1999/10/10 08:49:54  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.5  1999/08/03 07:37:46  hongyu
// add support for read animation symbol from JAR file           by Hongyu Yao
//
// Revision 1.4  1999/06/23 20:59:20  wenger
// Added standard DEVise header.
//

// ========================================================================

// jsb.java

import  java.applet.*;
import  java.awt.event.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;


public class jsb extends Applet
{
    URL baseURL = null;

    String sessionName = null;
    int debugLevel = 0;

    Vector images = null;

    TextArea startInfo = null;
    boolean isInit = true;

    jsdevisec jsc = null;

    public void init()
    {
        DEViseGlobals.isApplet = true;
        DEViseGlobals.inBrowser = true;

        isInit = true;

        DEViseGlobals.browser = getAppletContext();

        setLayout(new BorderLayout(0, 10));

        startInfo = new TextArea(8, 50);
        startInfo.setBackground(DEViseGlobals.textBg);
        startInfo.setForeground(DEViseGlobals.textFg);
        startInfo.setFont(DEViseGlobals.textFont);
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

        if (sessionName == null) {
            startInfo.append("Error: No session specified!");
            isInit = false;
            return;
        }

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

            if (images != null) {
                startInfo.append("DEVise animation symbol successfully loaded\n");
            } else {
                isInit = false;
                return;
            }
        }

        if (isInit) {
            remove(startInfo);
            if (jsc == null) {
                jsc = new jsdevisec(null, images, debugLevel, sessionName);
                add(jsc, BorderLayout.CENTER);
            } else {
                if (jsc.getQuitStatus()) {
                    jsc = null;
                    jsc = new jsdevisec(null, images, debugLevel, sessionName);
                    add(jsc, BorderLayout.CENTER);
                } else {
                    //jsc.displayMe(true);
                    setVisible(true);
                }
            }
        }
    }

    public void start()
    {
        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            //jsc.displayMe(true);
            setVisible(true);
        }
    }

    public void stop()
    {
        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            //jsc.displayMe(false);
            setVisible(false);
        }
    }

    public void destroy()
    {
        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.quit();
            jsc = null;
        }

        sessionName = null;
        images = null;

        super.destroy();
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

        //Dimension dim = getMaximumSize();
        //DEViseGlobals.maxScreenSize.width = dim.width - 20;
        //DEViseGlobals.maxScreenSize.height = dim.height - 60;
        DEViseGlobals.maxScreenSize.width = 794;
        DEViseGlobals.maxScreenSize.height = 540;
        DEViseGlobals.minScreenSize.width = 300;
        DEViseGlobals.minScreenSize.height = 240;
        if (DEViseGlobals.maxScreenSize.width < DEViseGlobals.minScreenSize.width) {
            DEViseGlobals.maxScreenSize.width = DEViseGlobals.minScreenSize.width;
        }
        if (DEViseGlobals.maxScreenSize.height < DEViseGlobals.minScreenSize.height) {
            DEViseGlobals.maxScreenSize.height = DEViseGlobals.minScreenSize.height;
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

                DEViseGlobals.screenSize.width = x;
                DEViseGlobals.screenSize.height = y;

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

                DEViseGlobals.rubberBandLimit.width = x;
                DEViseGlobals.rubberBandLimit.height = y;
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
                DEViseGlobals.bg = c;
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
                DEViseGlobals.fg = c;
                startInfo.append("Parameter fgcolor (" + r + ", " + g + ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
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

