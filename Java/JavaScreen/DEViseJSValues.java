// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2015
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

// ========================================================================

// DEViseJSValues.java

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
        public boolean useCgi = false;  // using cgi, default false.
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
        /*  public Color bg = new Color(64, 96, 0);*/
        public Color bg = new Color(237, 237, 237);// ededed
        public Color bg_warn = new Color(255, 0, 0);
        /*  public Color fg = Color.white;*/
        public Color fg = Color.black;
        /*  public Color screenBg = new Color(64, 192, 0);*/
        public Color screenBg = new Color(220, 220, 220);
        /*  public Font font = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);*/
        public Font font = DEViseFonts.getFont(14, DEViseFonts.SANS_SERIF, 0, 0);
        public Font font2 = DEViseFonts.getFont(12, DEViseFonts.SANS_SERIF, 0, 0);
        public Color textBg = new Color(255, 240, 230);
        public Color textFg = Color.black;
        /*  public Font textFont = DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0);*/
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
