// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
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

public final class DEViseJSValues
{
    public final class Canvas {
	public int lastKey = KeyEvent.VK_UNDEFINED;
	public DEViseCanvas sourceCanvas = null;

	// I think this is true when the mouse button is down in a canvas
	// -- signals to do non-shaded drawing of spheres in 3D canvases.
	// RKW 2000-11-21.
	public boolean isInteractive = false;

	// Total number of crystal repaints.
	public int crystalPaintCount = 0;
    }

    public final class Debug {
	public int _debugLevel = 0;

	public boolean _logEnabled = false;
    	public DEViseDebugLog _logger = null;
	public void log(String msg) {
	    if (_logger != null) {
	        _logger.send(msg);
	    }
	}
    }

    public final class GData {
	public Font defaultFont = null;
	public int _gdCount = 0;
    }

    public final class Connection {
	public int cmdport = 0;
	public int connectionID = DEViseGlobals.DEFAULTID; // for client
	public String username = null, password = null, hostname = null;
	public boolean helpBox = false;
	public boolean cgi = false;  // using cgi, default false.
	public String cgiURL = null; // URL for cgi.
    }

    public final class UIGlobals {
	public boolean isApplet = false, inBrowser = false;
	public AppletContext browser = null;
	public Dimension maxScreenSize = new Dimension(0, 0);
	public Dimension minScreenSize = new Dimension(0, 0);
	public Dimension screenSize = new Dimension(0, 0);
	public int screenRes = -1;
	public Dimension rubberBandLimit = new Dimension(4, 4);
	public Color bg = new Color(64, 96, 0);
	public Color bg_warn = new Color(255, 0, 0);
	public Color fg = Color.white;
	public Color screenBg = new Color(64, 192, 0);
	public Font font = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);
	public Font font2 = DEViseFonts.getFont(12, DEViseFonts.SANS_SERIF, 0, 0);
	public Color textBg = new Color(255, 240, 230);
	public Color textFg = Color.black;
	public Font textFont = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);
    }

    public final class Session {
        public String defaultName = null;
	public String clientLogName = null;
	public boolean autoPlayback = false;
	public boolean playbackOriginal = false;
	public boolean playbackDisplay = true;
    }

    public Canvas canvas = new Canvas();
    public Debug  debug = new Debug();
    public GData  gdata = new GData();
    //Fonts  fonts;
    public Connection connection = new Connection(); 
    public UIGlobals uiglobals = new UIGlobals();
    public Session session = new Session();
}
