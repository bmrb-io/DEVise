// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Top-level class for JavaScreen applet with its window outside the
// web browser.  This class parses the applet parameters, etc., and
// then creates a jsdevisec.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.41.2.11  2001/01/05 19:15:45  wenger
// Updated copyright dates.
//
// Revision 1.41.2.10  2001/01/05 17:56:25  wenger
// Minor code cleanups.
//
// Revision 1.41.2.9  2001/01/05 16:35:31  wenger
// JS applets now deal correctly with stop/start calls from browser.
//
// Revision 1.41.2.8  2000/12/29 22:41:39  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.41.2.7  2000/12/20 18:22:50  wenger
// Fixed some problems with parameters in jsa and jsb.
//
// Revision 1.41.2.6  2000/11/29 22:40:57  wenger
// Abstracted common code from jsa and jsb into new DEViseJSApplet
// superclass; other minor cleanups.
//
// Revision 1.41.2.5  2000/11/22 22:01:04  wenger
// Added 'usecgi' and 'cgiurl' parameters to the JSA and JSB, added other
// files to help test this.
//
// Revision 1.41.2.4  2000/11/22 19:59:48  wenger
// Formatting cleanup.
//
// Revision 1.41.2.3  2000/11/22 17:44:00  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.41.2.2  2000/11/21 01:51:35  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.41.2.1  2000/09/01 20:23:58  xuk
// In checkParameters(), delete imgports checking.
//
// Revision 1.41  2000/08/21 15:30:19  wenger
// Made 'start JavaScreen' button label clearer; changed html files to use
// new loader message jar file.
//
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
import  java.awt.*;
import  java.awt.event.*;
import  java.io.*;
import  java.util.*;

public class jsa extends DEViseJSApplet
{
    static final int DEBUG = 0;

    Button startButton = new Button("Click here to start JavaScreen");

    jscframe jsf = null;
    boolean started = false;

    public void init()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.init()");
	}

	super.init();
	final Applet applet = this;

        jsValues.uiglobals.inBrowser = false;

        add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);

        startButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    if (jsf == null) {
                        startInfo.append("Start Java Screen ...\n");
                        jsf = new jscframe(applet, images, sessionName,
			  jsValues);
                        //startButton.setEnabled(false);
                    } else {
			// Note: we get here if the user clicks the start
			// button when a JS already exists.  RKW 2001-01-05.
                        if (jsf.isQuit()) {
                            startInfo.append("Start Java Screen ...\n");
                            jsf = null;
                            jsf = new jscframe(applet, images, sessionName,
			      jsValues);
                            //startButton.setEnabled(false);
                        } else {
                            startInfo.append("Java Screen already started!\n");
                        }
                    }
                }
            });
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.start()");
	}

	if (started) {
            if (jsf != null && !jsf.isQuit()) {
                jsf.displayMe(true);
	    }
	} else {
            if (sessionName != null) {
                startJS(true);
            } else {
                startJS(false);
            }
	    started = true;
	}
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.stop()");
	}

        if (jsf != null && !jsf.isQuit()) {
            jsf.displayMe(false);
        }
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.destroy()");
	}

        if (jsf != null && !jsf.isQuit()) {
            jsf.destroy();
            jsf = null;
        }

        sessionName = null;
        images = null;

        super.destroy();
    }

    public void startJS(boolean flag)
    { 
        if (DEBUG >= 1) {
            System.out.println("jsa.startJS()");
	}

	loadImages();

        startButton.setEnabled(true);

        if (jsf == null) {
            if (flag) {
                startInfo.append("Start Java Screen ...\n");
                jsf = new jscframe(this, images, sessionName, jsValues);
                //startButton.setEnabled(false);
            }
        } else {
            if (jsf.isQuit()) {
                if (flag) {
                    startInfo.append("Start new Java Screen ...\n");
                    jsf = null;
                    jsf = new jscframe(this, images, sessionName, jsValues);
                    //startButton.setEnabled(false);
                }
            } else {
                jsf.displayMe(true);
            }
        }
	repaint();
    }

    protected void checkParameters()
    {
        super.checkParameters();

        String screen = getParameter("screensize");
        if (screen != null) {
            try {
                String[] str = DEViseGlobals.parseStr(screen, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                jsValues.uiglobals.screenSize.width = x;
                jsValues.uiglobals.screenSize.height = y;

                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }
    }
}

class jscframe extends Frame
{
    public jsdevisec jsc = null;

    public jscframe(Applet parentApplet, Vector images, String sessionName,
      DEViseJSValues jsValues)
    {
        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        jsValues.uiglobals.maxScreenSize.width = dim.width - 80;
        jsValues.uiglobals.maxScreenSize.height = dim.height - 120;
        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        jsc = new jsdevisec(parentApplet, this, images, sessionName, jsValues);
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

    public void destroy()
    {
        jsc.destroy();
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.destroy();

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
