// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
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

    public Canvas canvas = new Canvas();
    public Debug  debug = new Debug();
    public GData  gdata = new GData();
    //Fonts  fonts;
    public Connection connection = new Connection(); 
    public UIGlobals uiglobals = new UIGlobals();
}
