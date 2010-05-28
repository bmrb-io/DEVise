// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains a number of variables used by the JavaScreen.
// The purpose of this class is to avoid problems caused by client-side
// static variables in DEViseGlobals.java, etc.  Since static variables
// are shared by all objects in a JVM, this sometimes caused problems
// within Netscape.

// Note: all members are non-static.

// There is one instance of this class for each instance of the JavaScreen.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.20  2010/04/21 17:10:10  wenger
// Merged devise_dist_rest_1003_br_0 thru devise_dist_rest_1003_br_1 to trunk.
//
// Revision 1.19.4.1  2010/03/29 18:17:47  wenger
// Got things to work as an applet with the latest Jmol version -- needed
// some more initialization in creating the JmolViewer object.  Added
// the jsdevisec.pnStackTrace() method, since we can't get a Java
// console with the lab's Firefox setup.
//
// Revision 1.19  2009/08/28 15:59:39  wenger
// Minor code cleanups.
//
// Revision 1.18  2008/09/24 21:46:51  wenger
// Partially(?) fixed DEVise/JS bug 973: there is now a minimum delay
// before the JS stop button goes back to normal when communicating
// with the server, so the user won't fail to notice.
//
// Revision 1.17  2007/06/27 17:47:59  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.16  2007/02/22 23:20:18  wenger
// Merged the andyd_gui_br thru andyd_gui_br_2 to the trunk.
//
// Revision 1.15.2.2  2007/06/15 20:46:09  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.15.2.1  2007/02/13 18:35:10  adayton
// Updated basic colors, as well as images for 'traffic light' and throbber. Also started updating some components to use Swing, including the main application frame. Additional changes to my makefile as well.
//
// Revision 1.15  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.14.4.1  2006/03/31 22:41:15  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.14  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.13  2002/06/17 19:40:14  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.12.2.2  2003/03/28 17:21:30  wenger
// Made JS invisibility destruction timeout configurable; fixed some other
// problems with loading and re-loading the applet.
//
// Revision 1.12.2.1  2002/05/08 21:16:51  wenger
// Changed some collaboration GUI for more clarity, updated instructions
// accordingly; default collaboration password is now "" as workaround
// for problem filling in default in GUI on some platforms.
//
// Revision 1.12  2002/03/20 22:06:36  xuk
// Added automatic collaboration functionality.
// Four new global variables:
// collabLeaderName: for collaboration leader to define a collaboration name
// collabLeaderPass: for collaboration leader to define the password
// collabName: for collab follower to specify the collaboration name
// collabPass: for collab follower to specify the collaboration passwd.
//
// Revision 1.11  2002/02/28 16:35:59  xuk
// Added "reloadapplet" parameter to load old jsb applet instance into new HTML page.
//
// Revision 1.10  2002/02/22 21:51:10  xuk
// Added an applet parameter to hide all of the applet buttons in browser.
// param name="disablebuttons" value="true|false"
//
// Revision 1.9  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.8.2.1  2001/11/07 17:22:36  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.8  2001/10/12 02:06:37  xuk
// Using timestamp-based client ID.
// Expanded connection.connectionID to long.
//
// Revision 1.7  2001/10/10 19:28:19  xuk
// Added display control command line argument/parameter for auto playback.
// For JS application: command-line argument: -playbackdisplayoff to turn off display, default is turning on display;
// For JS applet: parameter playbackdisplay = true|false to turn on|off display.
//
// Revision 1.6  2001/10/02 21:57:48  xuk
// Added command-line option for command log playback.
// Added clientLogName, for client log URL;
// Added autoPlayback, for automatically playback;
// Added playbackOriginal, for auto playback in original rate.
//
// Revision 1.5  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.4  2001/05/11 20:36:08  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.3  2001/05/07 21:53:25  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
//
// Revision 1.2  2001/01/08 20:31:52  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.1.2.3  2000/11/29 22:40:56  wenger
// Abstracted common code from jsa and jsb into new DEViseJSApplet
// superclass; other minor cleanups.
//
// Revision 1.1.2.2  2000/11/22 17:43:58  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//

// ========================================================================

// DEViseJSValues.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.URL;

public final class DEViseJSValues
{
//TEMPTOOLBAR -- add something in here to save what toolbar mode we're in?
    public final class CanvasValues {
	public int lastKey = KeyEvent.VK_UNDEFINED;
	public DEViseCanvas sourceCanvas = null;

	// I think this is true when the mouse button is down in a canvas
	// -- signals to do non-shaded drawing of spheres in 3D canvases.
	// RKW 2000-11-21.
	public boolean isInteractive = false;

	// Total number of crystal repaints.
	public int crystalPaintCount = 0;
    }

    public final class DebugValues {
	public int _debugLevel = 0;

	public boolean _logEnabled = false;
    	public DEViseDebugLog _logger = null;
	public void log(String msg) {
	    if (_logger != null) {
	        _logger.send(msg);
	    }
	}
    }

    public final class GDataValues {
	public Font defaultFont = null;
	public int _gdCount = 0;
    }

    public final class ConnectionValues {
	public int cmdport = 0;
	public int connectionID = DEViseGlobals.DEFAULTID; // for client
	public String username = null, password = null, hostname = null;
	public boolean helpBox = false;
	public boolean cgi = false;  // using cgi, default false.
	public String cgiURL = null; // URL for cgi.
	public URL documentBase = null;
	public URL codeBase = null;
    }

    public final class UIGlobals {
	public boolean isApplet = false, inBrowser = false;
	public AppletContext browser = null;
	public Dimension maxScreenSize = new Dimension(0, 0);
	public Dimension minScreenSize = new Dimension(0, 0);
	public Dimension screenSize = new Dimension(0, 0);
	public int screenRes = -1;
	public Dimension rubberBandLimit = new Dimension(4, 4);
/*	public Color bg = new Color(64, 96, 0);*/
	public Color bg = new Color(247, 246, 220);
	public Color bg_warn = new Color(255, 0, 0);
/*	public Color fg = Color.white;*/
	public Color fg = Color.black;
/*	public Color screenBg = new Color(64, 192, 0);*/
	public Color screenBg = new Color(229, 227, 207);
/*	public Font font = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);*/
	public Font font = DEViseFonts.getFont(14, DEViseFonts.SANS_SERIF, 0, 0);
	public Font font2 = DEViseFonts.getFont(12, DEViseFonts.SANS_SERIF, 0, 0);
	public Color textBg = new Color(255, 240, 230);
	public Color textFg = Color.black;
/*	public Font textFont = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);*/
	public Font textFont = DEViseFonts.getFont(12, DEViseFonts.MONOSPACED, 0, 0);

	// Timeout (in minutes) for killing the client if it's invisible.
	public int visTimeout = 60;

	// Minimum time (in milliseconds) the stop button will be "busy" after
	// we've sent a command to the server (see bug 973).
	public long minWaitTime = DEViseGlobals.DEFAULT_MIN_WAIT_TIME;
	public boolean _hideBmrbSessionNames = false;
    }

    public final class SessionValues {
        public String defaultName = null;
	public String clientLogName = null;
	public boolean autoPlayback = false;
	public boolean playbackOriginal = false;
	public boolean playbackDisplay = true;
	public boolean disableButtons = false;
	public boolean reloadApplet = true;
	public String collabLeaderName = null;
	public String collabLeaderPass = DEViseGlobals.DEFAULT_COLLAB_PASS;
	public String collabName = null;
	public String collabPass = null;
    }

    public CanvasValues canvas = new CanvasValues();
    public DebugValues  debug = new DebugValues();
    public GDataValues  gdata = new GDataValues();
    //Fonts  fonts;
    public ConnectionValues connection = new ConnectionValues(); 
    public UIGlobals uiglobals = new UIGlobals();
    public SessionValues session = new SessionValues();

    // for loading images for icons, etc.
    public Component _imageLoadComp = null;
    public MediaTracker _tracker = null;
}
