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

// Top-level class for JavaScreen applet with its window within the
// web browser.  This class parses the applet parameters, etc., and
// then creates a jsdevisec.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.16  2002/02/28 16:37:03  xuk
// Keep old jsb applet instance when reloading a new HTML page.
//
// Revision 1.15  2001/05/11 20:36:13  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.14  2001/05/07 21:53:26  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
//
// Revision 1.13  2001/02/20 20:02:25  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.12.2.1  2001/02/05 22:02:11  wenger
// Fixed bugs 639 and 640 and other problems associated with destroying
// and re-starting the JavaScreen applets.
//
// Revision 1.12  2001/01/08 20:31:57  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.11.4.8  2001/01/05 19:15:45  wenger
// Updated copyright dates.
//
// Revision 1.11.4.7  2001/01/05 16:35:31  wenger
// JS applets now deal correctly with stop/start calls from browser.
//
// Revision 1.11.4.6  2000/12/29 22:41:39  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.11.4.5  2000/11/29 22:40:57  wenger
// Abstracted common code from jsa and jsb into new DEViseJSApplet
// superclass; other minor cleanups.
//
// Revision 1.11.4.4  2000/11/22 22:01:05  wenger
// Added 'usecgi' and 'cgiurl' parameters to the JSA and JSB, added other
// files to help test this.
//
// Revision 1.11.4.3  2000/11/22 17:44:00  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.11.4.2  2000/11/21 01:51:35  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.11.4.1  2000/09/01 20:24:45  xuk
// In checkParameters(), delete imgports checking.
//
// Revision 1.11  2000/06/26 16:48:33  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.10  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.9  2000/03/23 16:26:18  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.8  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.7  1999/10/10 09:51:01  hongyu
// *** empty log message ***
//
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

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.io.*;
import  java.util.*;

public class jsb extends DEViseJSApplet
{
    static final int DEBUG = 0;

    jsdevisec jsc = null;

    public static jsb app = null;

    private DEViseJSTimer timer = null;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("jsb.init()");
        }

        jsValues.uiglobals.inBrowser = true;

        if (jsValues.session.defaultName == null) {
            startInfo.append("Error: No session specified!");
            isInit = false;
            return;
        }

	loadImages();

        if (isInit) {
            remove(startInfo);
            if (jsc == null) {
                jsc = new jsdevisec(this, null, images, jsValues);
                add(jsc, BorderLayout.CENTER);
            } else {
                if (jsc.getQuitStatus()) {
                    jsc = null;
                    jsc = new jsdevisec(this, null, images, jsValues);
                    add(jsc, BorderLayout.CENTER);
                } else {
                    //jsc.displayMe(true);
                    setVisible(true);
                }
            }
        }

	// reloading applet instance
	if (app == null) {
	    app = this;
	} else if (jsc != null && !jsValues.session.reloadApplet) {
	    jsc.jsValues.uiglobals.maxScreenSize.width = jsValues.uiglobals.maxScreenSize.width;
	    jsc.jsValues.uiglobals.maxScreenSize.height = jsValues.uiglobals.maxScreenSize.height;
	    jsc.jsValues.uiglobals.screenSize.width = jsValues.uiglobals.maxScreenSize.width;
	    jsc.jsValues.uiglobals.screenSize.height = jsValues.uiglobals.maxScreenSize.height;
	    jsc.jscreen.setScreenDim(jsValues.uiglobals.maxScreenSize.width,
				     jsValues.uiglobals.maxScreenSize.height);
	    jsc.restartSession();
	}
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.start()");
	}

        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            setVisible(true);
        }
	if (jsc != null) {
	    jsc.showDebug();
	}

	if (timer != null) {
	    timer.stopped = true;
	}
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.stop()");
	}

        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            setVisible(false);
        }
	if (jsc != null) {
	    jsc.hideDebug();
	}

	if (timer == null) 
	    timer = new DEViseJSTimer(this);
	timer.stopped = false;
	timer.start();
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.destroy()");
	}

        if (jsc != null && !jsc.getQuitStatus()) {
            jsc.destroy();
            jsc = null;
        }

        super.destroy();
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

                jsValues.uiglobals.maxScreenSize.width = x;
                jsValues.uiglobals.maxScreenSize.height = y;

                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {

            }
        }

        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        if (jsValues.uiglobals.maxScreenSize.width > 0 && jsValues.uiglobals.maxScreenSize.height > 0) {
            jsValues.uiglobals.maxScreenSize.width -= 6;
            jsValues.uiglobals.maxScreenSize.height -= 60;
        } else {
            jsValues.uiglobals.maxScreenSize.width = 640;
            jsValues.uiglobals.maxScreenSize.height = 480;
        }

        if (jsValues.uiglobals.maxScreenSize.width < jsValues.uiglobals.minScreenSize.width) {
            jsValues.uiglobals.maxScreenSize.width = jsValues.uiglobals.minScreenSize.width;
        }
        if (jsValues.uiglobals.maxScreenSize.height < jsValues.uiglobals.minScreenSize.height) {
            jsValues.uiglobals.maxScreenSize.height = jsValues.uiglobals.minScreenSize.height;
        }
    }
}


class DEViseJSTimer implements Runnable
{
    private static final int DEBUG = 0;
    private static final int INTERVAL = 60 * 60 * 1000; //one hour 
    private jsb applet = null;
    private Thread thread = null;
    public static boolean stopped = false;

    public DEViseJSTimer(jsb a) {
        applet = a;

	thread = new Thread(this);
    }

    public void start() 
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.start()");
	}

	thread.start();
    }

    public void run()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.run()");
	}

	try {
	    Thread.sleep(INTERVAL);

	    if (stopped) {
		stopped = false;
		System.out.println("stop timer.");
	    } else {
		applet.destroy();
		System.out.println("destroy applet.");
	    }

	    stop();
	} catch (InterruptedException e) {
	}
    }

    public void stop()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.stop()");
	}
        thread.stop();
    }
}









