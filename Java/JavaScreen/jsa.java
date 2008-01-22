// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2007
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
// Revision 1.50  2007/04/20 19:42:36  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.49  2007/02/22 23:20:22  wenger
// Merged the andyd_gui_br thru andyd_gui_br_2 to the trunk.
//
// Revision 1.48.6.2  2007/04/19 21:16:09  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.48.6.1  2007/02/13 18:35:15  adayton
// Updated basic colors, as well as images for 'traffic light' and throbber. Also started updating some components to use Swing, including the main application frame. Additional changes to my makefile as well.
//
// Revision 1.48  2005/12/06 20:00:22  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.47  2003/01/13 19:23:45  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.46.2.4  2003/06/26 20:18:04  wenger
// Added closeSession() and openSession() methods to the JS applet for
// Wavelet-IDR.
//
// Revision 1.46.2.3  2003/01/28 15:29:29  wenger
// Made refreshAllData() and restartSession() methods in DEViseJSLoader,
// so that the Wavelet-IDR JavaScript code can call them.
//
// Revision 1.46.2.2  2002/10/03 17:59:18  wenger
// JS applet instance re-use now reloads the session -- that's what
// Wavelet-IDR needs for their web page to work right.
//
// Revision 1.46.2.1  2002/08/16 21:56:56  wenger
// Fixed bug 807 (reload twice in Netscape goofs up JS applets); fixed
// various other problems with destroying hidden applets.
//
// Revision 1.46  2002/03/07 17:18:31  xuk
// Destroy jsa applet when it's invisible longer than one hour.
//
// Revision 1.45  2001/05/11 20:36:13  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.44  2001/05/07 21:53:26  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
//
// Revision 1.43  2001/02/20 20:02:25  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.42.2.1  2001/02/05 22:02:10  wenger
// Fixed bugs 639 and 640 and other problems associated with destroying
// and re-starting the JavaScreen applets.
//
// Revision 1.42  2001/01/08 20:31:56  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
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

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import  java.awt.event.*;
import  java.io.*;
import  java.util.*;
import	javax.swing.*;

public class jsa extends DEViseJSApplet
{
    static final int DEBUG = 0;

    Button startButton = new Button("Click here to start JavaScreen");

    jscframe jsf = null;
    boolean started = false;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("jsa.init()");
	}

        jsValues.uiglobals.inBrowser = false;

        add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);

        startButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    startJS();
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
	    loadImages();

            startButton.setEnabled(true);

            if (jsValues.session.defaultName != null) {
	        startJS();
            }

	    repaint();

	    started = true;
	}

	super.start();
    }

    public void stop()
    {
        if (DEBUG >= 0/*TEMPTEMP 1*/) {
            System.out.println("jsa.stop()");
	}

	jsf.destroy();//TEMPTEMP?

        //TEMPTEMP? if (jsf != null && !jsf.isQuit()) {
            //TEMPTEMP? jsf.displayMe(false);
        //TEMPTEMP? }

	super.stop();
    }

    public void destroy()
    {
        if (DEBUG >= 0/*TEMPTEMP 1*/) {
            System.out.println("jsa.destroy()");
	}

        //TEMPTEMP? if (jsf != null && !jsf.isQuit()) {
        if (jsf != null) {//TEMPTEMP?
            jsf.destroy();
            jsf = null;
        }
	startButton.setEnabled(false);

        super.destroy();
    }

    private void startJS()
    {
        if (DEBUG >= 1) {
	    System.out.println("jsa.startJS()");
	}
	
	if (jsf == null) {
	    startInfo.append("Start Java Screen ...\n");
	    jsf = new jscframe(this, images, jsValues);
	    //startButton.setEnabled(false);
        } else {
	    // Note: we get here if the user clicks the start
	    // button when a JS already exists.  RKW 2001-01-05.
	    if (jsf.isQuit()) {
	        startInfo.append("Start new Java Screen ...\n");
		jsf = null;
		jsf = new jscframe(this, images, jsValues);
		//startButton.setEnabled(false);
	    } else {
	        startInfo.append("Java Screen already started!\n");
	    }
        }
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

    public void refreshAllData(boolean doHome)
    {
        if (DEBUG >= 1) {
	    System.out.println("jsa.refreshAllData(" + doHome + ")");
	}

	jsf.jsc.refreshAllData(doHome);
    }

    public void restartSession()
    {
        if (DEBUG >= 1) {
	    System.out.println("jsa.restartSession()");
	}

	jsf.jsc.restartSession();
    }

    public void closeSession()
    {
        if (DEBUG >= 1) {
	    System.out.println("jsa.closeSession()");
	}

	jsf.jsc.closeSession();
    }

    public void openSession(String fullSessionName)
    {
        if (DEBUG >= 1) {
	    System.out.println("jsa.openSession(" + fullSessionName + ")");
	}

	jsf.jsc.openSession(fullSessionName);
    }
}

class jscframe extends JFrame
{
    static final int DEBUG = 0;

    public jsdevisec jsc = null;

    public jscframe(Applet parentApplet, Vector images,
      DEViseJSValues jsValues)
    {
        if (DEBUG >= 1) {
            System.out.println("jscframe constructor");
	}

        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        jsValues.uiglobals.maxScreenSize.width = dim.width - 80;
        jsValues.uiglobals.maxScreenSize.height = dim.height - 120;
        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        jsc = new jsdevisec(parentApplet, this, images, jsValues);
        add(jsc);
        setTitle(DEViseUIGlobals.javaScreenTitle);
        pack();
	jsc.start();

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
	if (jsc == null) {
	    return true;
	} else {
            return jsc.getQuitStatus();
	}
    }

    public void destroy()
    {
        if (DEBUG >= 0/*TEMPTEMP 1*/) {
	    System.out.println("jscframe.destroy()");
	}

        jsc.destroy();//TEMPTEMP -- got null pointer here when destroying jsa
	jsc = null;
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
        if (DEBUG >= 1) {
            System.out.println("jscframe.displayMe(" + isShow + ")");
	}

        if (isShow) {
            if (!isShowing()) {
                setVisible(true);
            }
	    if (jsc != null) {
	        jsc.showDebug();
	    }
        } else {
            if (isShowing()) {
                setVisible(false);
            }
	    if (jsc != null) {
	        jsc.hideDebug();
	    }
        }
    }
}
