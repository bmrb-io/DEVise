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

import java.applet.*;
import java.awt.*;

public class DEViseJSLoader extends Applet implements Runnable, AppletStub
{
    static final int DEBUG = 0;

    String appletToLoad;
    Label label;
    Thread appletThread;
    Applet realApplet;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.init()");
        }

	// Get "real applet" name from applet parameters.
	appletToLoad = getParameter("applet");
        if (appletToLoad == null) {
	    appletToLoad = "jsb";
	}

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
            Class appletClass = Class.forName(appletToLoad);
            realApplet = (Applet)appletClass.newInstance();
            realApplet.setStub(this);

            remove(label);
            setLayout(new GridLayout(1, 0));
            add(realApplet);
            realApplet.init();
            realApplet.start();
        } catch (Exception e) {
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
