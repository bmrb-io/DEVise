// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2003
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------
//TEMPTEMP -- do we even need this file anymore???

// This applet loads the applet specified in the appletToLoad
// string. Used to print a short message that Java Screen is 
// loading, when the network connection is slow.

// Note: we seem to need all of the threads to get the "loading" message
// to show up properly.  wenger 2003-03-27.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.10  2008/01/24 20:30:53  wenger
// Merged js_ie_fix_br_0 thru js_ie_fix_br_1 to the trunk.
//
// Revision 1.9  2008/01/22 20:02:38  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron); I tried backporting
// my fix to the pre-toolbar version of the JS, but it doesn't work for
// some reason (I suspect that some of the other cleanups since then
// also affect the fix).  Note that this commit has a bunch of temporary
// code still in place; I want to get a working version into CVS ASAP.
//
// Revision 1.8  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.7  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.6  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.5.2.11  2003/07/23 21:00:10  wenger
// Oops!  Needed closeSession() and openSession() wrapper methods in
// DEViseJSLoader.
//
// Revision 1.5.2.10  2003/05/02 17:16:16  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.5.2.9  2003/03/28 17:21:29  wenger
// Made JS invisibility destruction timeout configurable; fixed some other
// problems with loading and re-loading the applet.
//
// Revision 1.5.2.8  2003/01/28 15:29:28  wenger
// Made refreshAllData() and restartSession() methods in DEViseJSLoader,
// so that the Wavelet-IDR JavaScript code can call them.
//
// Revision 1.5.2.7  2002/12/18 15:19:04  wenger
// Disabled (at least temporarily) the applet no-reload feature in the
// JavaScreen code (because it just causes problems on Mozilla).
//
// Revision 1.5.2.6  2002/12/17 23:15:01  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.5.2.5  2002/11/25 21:29:35  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.5.2.4  2002/10/03 17:59:18  wenger
// JS applet instance re-use now reloads the session -- that's what
// Wavelet-IDR needs for their web page to work right.
//
// Revision 1.5.2.3  2002/08/19 16:51:37  wenger
// Applet instances are now re-used according to URL if the "reloadapplet"
// flag is false.
//
// Revision 1.5.2.2  2002/08/16 21:56:56  wenger
// Fixed bug 807 (reload twice in Netscape goofs up JS applets); fixed
// various other problems with destroying hidden applets.
//
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
import java.util.*;

public class DEViseJSLoader extends Applet implements Runnable, AppletStub
{
    static final int DEBUG = 0;

    String appletToLoad;
    protected Label label;
    Thread _loaderThread;
    DEViseJSApplet realApplet;

    boolean _reload = true;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.init()");
        }

	ShowLoadingMessage show = new ShowLoadingMessage(this);
	show.start();

	// Get "real applet" name from applet parameters.
	appletToLoad = getParameter("applet");
        if (appletToLoad == null) {
	    appletToLoad = "JavaScreen.jsb";
	}

        // Force reloadApplet to always be true for now, since the "no-reload"
	// feature doesn't work in Mozilla anyhow.  RKW 2002-12-18.
	/*TEMP?
	// Whether reload a new applet instance for jsb.
	String reloadString = getParameter("reloadapplet");
        if (reloadString == null) {
	    _reload = true;
	} else if (reloadString.equals("false")) {
	    _reload = false;
	} else {
	    _reload = true;
	}
        TEMP?*/
	_reload = true;//TEMP?
    }

    public void run()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.run()");
        }

        if (appletToLoad == null) return;

	String url = getDocumentBase().toString();

        try {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
            }

	    realApplet = getAppletInstance(appletToLoad, url, _reload);

	    realApplet.setStub(this);

	    remove(label);
	    setLayout(new GridLayout(1, 0));
	    add(realApplet);
	    // Note: we still have to do this even if we're re-using
	    // an existing applet instance. RKW 2002-08-05.
	    realApplet.init();
	    realApplet.start();

	} catch (Exception e) {
	    System.err.println("Error loading applet: " + e.getMessage());
            label.setText("Error loading applet.");
        }
        validate();

	if (DEViseGlobals.DEBUG_THREADS >= 1) {
	    DEViseUtils.printAllThreads("Thread " + _loaderThread + " ending");
	}
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.start()");
        }

        if (_loaderThread == null || realApplet == null ||
	  realApplet.wasDestroyed()) {
            _loaderThread = new Thread(this);
            _loaderThread.setName("JS loader");
            _loaderThread.start();
	    if (DEViseGlobals.DEBUG_THREADS >= 1) {
		DEViseUtils.printAllThreads("Starting thread " + _loaderThread);
	    }
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

	// Note: if reload is true, we'll never re-use this "real" applet
	// instance (unless the html page is changed to have reload be
	// false when we come back to it).  Therefore, we'll go ahead
	// and destroy the real applet so we don't have lots of "zombie"
	// instances cluttering things up.  RKW 2002-11-25.
	if (_reload) {
	    realApplet.destroy();
	    realApplet = null;
	}

	super.destroy();
    }

    public void appletResize(int width, int height)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSLoader.resize()");
        }

        resize(width, height);
    }

    // Force the applet to refresh all data in its current session.
    public void refreshAllData(boolean doHome)
    {
        realApplet.refreshAllData(doHome);
    }

    // Force the applet to reload its current session.
    public void restartSession()
    {
        realApplet.restartSession();
    }

    // Close the current session.
    public void closeSession()
    {
        realApplet.closeSession();
    }

    // Open the given session.
    public void openSession(String fullSessionName)
    {
        realApplet.openSession(fullSessionName);
    }

    //
    // Figure out whether there's an existing instance of the applet
    // that we may be able to re-use.
    // Note: this assumes that the actual HTML code referenced
    // by the URL hasn't changed since we loaded the associated
    // applet (or at least hasn't changed in a way such as
    // referencing a different version of the applet).  RKW
    // 2002-08-19.
    //
    private static DEViseJSApplet getAppletInstance(String appletName,
      String url, boolean reload) throws ClassNotFoundException,
      InstantiationException, IllegalAccessException
    {
        DEViseJSApplet applet = null;

	DEViseJSApplet tmpAppl = DEViseJSApplet.getInstance(url);

        if (reload || tmpAppl == null) {
	    Class appletClass = Class.forName(appletName);
	    applet = (DEViseJSApplet)appletClass.newInstance();

	    System.out.println("New instance (" + applet.getInstanceNum() + ")");
	} else {
	    applet = tmpAppl;

	    System.out.println("Old instance (" +
	      applet.getInstanceNum() + ")");

	    // Force the old applet to reload the session (this
	    // behavior is needed for Wavelet-IDR).
	    applet.restartSession();
	}

	return applet;
    }

    class ShowLoadingMessage implements Runnable
    {
	DEViseJSLoader _loader;
	Thread _thread;

        public ShowLoadingMessage(DEViseJSLoader loader)
        {
	    _loader = loader;
        }

        public void start()
        {
	    _thread = new Thread(this);
	    _thread.setName("ShowLoadingMessage");
	    _thread.start();
        }

        public void run()
        {
            _loader.label = new Label(
	      " *** Please wait - Loading Applet for DEVise JavaScreen. ***  ");
            _loader.label.setFont(new Font("Helvetica", Font.BOLD, 16));
            _loader.label.setForeground(new Color(0.4f, 0.3f, 0.6f));
            _loader.label.setBackground(new Color(0.9f, 0.9f, 0.9f));
 
            _loader.add(_loader.label);
        }
    }
}
