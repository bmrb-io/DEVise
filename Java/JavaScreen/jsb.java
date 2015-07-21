// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
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
// Revision 1.24.34.1  2015/06/19 17:02:57  wenger
// Changed "suggest" to "feedback" as per feedback from Eldon (still working
// on moving the feedback button to the right side).  Added -showallbut
// command-line flag (for debugging) that causes the JS to show the
// Jmol and session-specific buttons.
//
// Revision 1.24  2008/01/24 20:30:53  wenger
// Merged js_ie_fix_br_0 thru js_ie_fix_br_1 to the trunk.
//
// Revision 1.23  2008/01/22 20:02:38  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron); I tried backporting
// my fix to the pre-toolbar version of the JS, but it doesn't work for
// some reason (I suspect that some of the other cleanups since then
// also affect the fix).  Note that this commit has a bunch of temporary
// code still in place; I want to get a working version into CVS ASAP.
//
// Revision 1.22.14.1  2008/01/22 22:11:21  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron) in pre-toolbar version
// of the JavaScreen (the problem before was that I forgot to make
// jsdevisec.destroy() non-synchronized).
//
// Revision 1.22  2007/04/20 19:42:36  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.21.6.1  2007/04/19 21:16:09  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.21  2005/12/06 20:00:23  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.20  2003/01/13 19:23:45  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.19  2002/06/17 19:40:16  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.18.2.7  2003/06/26 20:18:04  wenger
// Added closeSession() and openSession() methods to the JS applet for
// Wavelet-IDR.
//
// Revision 1.18.2.6  2003/03/28 17:21:30  wenger
// Made JS invisibility destruction timeout configurable; fixed some other
// problems with loading and re-loading the applet.
//
// Revision 1.18.2.5  2003/01/28 15:29:29  wenger
// Made refreshAllData() and restartSession() methods in DEViseJSLoader,
// so that the Wavelet-IDR JavaScript code can call them.
//
// Revision 1.18.2.4  2002/10/03 17:59:18  wenger
// JS applet instance re-use now reloads the session -- that's what
// Wavelet-IDR needs for their web page to work right.
//
// Revision 1.18.2.3  2002/08/19 16:51:37  wenger
// Applet instances are now re-used according to URL if the "reloadapplet"
// flag is false.
//
// Revision 1.18.2.2  2002/08/16 21:56:56  wenger
// Fixed bug 807 (reload twice in Netscape goofs up JS applets); fixed
// various other problems with destroying hidden applets.
//
// Revision 1.18.2.1  2002/05/08 16:39:45  wenger
// JSB applet allows startup without loading a session (for collaboration
// follower); collabf.html web page doesn't load a session.
//
// Revision 1.18  2002/03/07 17:18:31  xuk
// Destroy jsa applet when it's invisible longer than one hour.
//
// Revision 1.17  2002/03/05 23:01:24  xuk
// Resize JavaScreen after reloading jsb in browser.
// Added DEViseJSTimer class in jsb.java to destroy applet after the applet
// has not been touched for an hour.
//
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

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
            System.out.println("jsb.init()");
        }

        jsValues.uiglobals.inBrowser = true;

	loadImages();

        if (isInit) {
            remove(startInfo);
            if (jsc == null || jsc.getQuitStatus()) {
                jsc = new jsdevisec(this, null, images, jsValues, false);
                add(jsc, BorderLayout.CENTER);
	    } else {
                //jsc.displayMe(true);
                setVisible(true);
	    }
        }
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.start()");
	}

	jsc.start();

        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            setVisible(true);
        }
	if (jsc != null) {
	    jsc.showDebug();
	}

	super.start();
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.stop()");
	}

        if (jsc != null) {
            jsc.destroy();
            jsc = null;
        }

	super.stop();
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.destroy()");
	}

        if (jsc != null) {
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

    public void refreshAllData(boolean doHome)
    {
        if (DEBUG >= 1) {
	    System.out.println("jsb.refreshAllData(" + doHome + ")");
	}

	jsc.refreshAllData(doHome);
    }


    public void restartSession()
    {
        if (DEBUG >= 1) {
	    System.out.println("jsb.restartSession()");
	}

	jsc.restartSession();
    }

    public void closeSession()
    {
        if (DEBUG >= 1) {
	    System.out.println("jsb.closeSession()");
	}

	jsc.closeSession();
    }

    public void openSession(String fullSessionName)
    {
        if (DEBUG >= 1) {
	    System.out.println("jsb.openSession(" + fullSessionName + ")");
	}

	jsc.openSession(fullSessionName);
    }
}
