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

// This applet loads the applet specified in the appletToLoad
// string. Used to print a short message that Java Screen is 
// loading, when the network connection is slow.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5.2.1  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.5  2002/02/28 16:37:03  xuk
// Keep old jsb applet instance when reloading a new HTML page.
//
// Revision 1.4  2001/05/11 20:36:08  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.3  2001/02/20 20:02:23  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.2.2.1  2001/02/05 22:02:10  wenger
// Fixed bugs 639 and 640 and other problems associated with destroying
// and re-starting the JavaScreen applets.
//
// Revision 1.2  2001/01/08 20:31:52  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.1.2.6  2001/01/05 19:15:45  wenger
// Updated copyright dates.
//
// Revision 1.1.2.5  2001/01/05 16:35:21  wenger
// JS applets now deal correctly with stop/start calls from browser.
//
// Revision 1.1.2.4  2000/12/29 22:41:38  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.1.2.3  2000/12/28 20:30:48  wenger
// Modified DEViseJSLoader.java to handle both the jsa and jsb, and eliminated
// DEViseJSBLoader.java; eliminated makejsa and makejsb and incorporated
// their functionality into the Makefile.
//
// Revision 1.1.2.2  2000/12/28 18:51:02  wenger
// Fixed formatting.
//
// Revision 1.1.2.1  2000/12/21 22:15:28  wenger
// Added proper header with RCS Id, etc.
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.applet.*;
import java.awt.*;

public class DEViseJSLoader extends Applet implements Runnable, AppletStub
{
    static final int DEBUG = 0;

    String appletToLoad;
    Label label;
    Thread appletThread;
    Applet realApplet;

    jsb app = null;
    boolean reload = true;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.init()");
        }

	// Get "real applet" name from applet parameters.
	appletToLoad = getParameter("applet");
        if (appletToLoad == null) {
	    appletToLoad = "JavaScreen.jsb";
	}

	// Whether reload a new applet instance for jsb.
	String reloadString = getParameter("reloadapplet");
        if (reloadString == null) {
	    reload = true;
	} else if (reloadString.equals("false")) 
	    reload = false;
	else
	    reload = true;
	
        label = new Label(
	  " *** Please wait - Loading Applet for DEVise JavaScreen. ***  ");
        label.setFont(new Font("Helvetica", Font.BOLD, 16));
        label.setForeground(new Color(0.4f, 0.3f, 0.6f));
        label.setBackground(new Color(0.9f, 0.9f, 0.9f));

        add(label);
    }

    public void run()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.run()");
        }

        if (appletToLoad == null) return;

        try {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
            }

	    if (reload) {
		System.out.println("New instance 1");

		Class appletClass = Class.forName(appletToLoad);
		realApplet = (Applet)appletClass.newInstance();
		realApplet.setStub(this);

		remove(label);
		setLayout(new GridLayout(1, 0));
		add(realApplet);
		realApplet.init();
		realApplet.start();
	    } else { // not reload, but still check 
		     // whether there is an old instance
		app = jsb.app; 
		
		if (app == null) {
		    System.out.println("New instance 2");

		    Class appletClass = Class.forName(appletToLoad);
		    realApplet = (Applet)appletClass.newInstance();
		    realApplet.setStub(this);

		    remove(label);
		    setLayout(new GridLayout(1, 0));
		    add(realApplet);
		    realApplet.init();
		    realApplet.start();
		} else {
		    System.out.println("Old instance");
		    
		    realApplet = (Applet)app;
		    realApplet.setStub(this);
		    
		    remove(label);
		    setLayout(new GridLayout(1, 0));
		    add(realApplet);
		    realApplet.init();
		    realApplet.start();
		}
	    }
	} catch (Exception e) {
	    System.err.println("Error loading applet: " + e.getMessage());
            label.setText("Error loading applet.");
        }
        validate();
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.start()");
        }

        if (appletThread == null) {
            appletThread = new Thread(this);
            appletThread.setName("JS loader");
            appletThread.start();
        } else {
	    realApplet.start();
	}
    }

    public void stop()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSLoader.stop()");
	}

	realApplet.stop();
    }

    public void destroy()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSLoader.destroy()");
	}

	realApplet.destroy();
    }

    public void appletResize(int width, int height)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.resize()");
        }

        resize(width, height);
    }
}
