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

// Top-level class for JavaScreen applets.  This class contains code that
// was once duplicated in jsa.java and jsb.java.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9  2002/02/28 16:36:03  xuk
// Added "reloadapplet" parameter to load old jsb applet instance into new HTML page.
//
// Revision 1.8  2002/02/22 21:51:10  xuk
// Added an applet parameter to hide all of the applet buttons in browser.
// param name="disablebuttons" value="true|false"
//
// Revision 1.7  2001/10/10 19:28:24  xuk
// Added display control command line argument/parameter for auto playback.
// For JS application: command-line argument: -playbackdisplayoff to turn off display, default is turning on display;
// For JS applet: parameter playbackdisplay = true|false to turn on|off display.
//
// Revision 1.6  2001/10/02 21:59:44  xuk
// Added new parameters for client log playback.
// Modified checkParameter() to check clientlog, autoplayback, playbackoriginal parameters.
// cvs: ----------------------------------------------------------------------
//
// Revision 1.5  2001/05/11 20:36:07  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.4  2001/05/07 21:53:25  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
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
// Revision 1.1.2.4  2001/01/05 19:05:16  wenger
// Fixed the problem with warnings about scrollbars when the JSA is loaded
// into Netscape.
//
// Revision 1.1.2.3  2001/01/05 17:56:25  wenger
// Minor code cleanups.
//
// Revision 1.1.2.2  2000/12/20 18:22:40  wenger
// Fixed some problems with parameters in jsa and jsb.
//
// Revision 1.1.2.1  2000/11/29 22:40:56  wenger
// Abstracted common code from jsa and jsb into new DEViseJSApplet
// superclass; other minor cleanups.
//
//

// ========================================================================

// jsa.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class DEViseJSApplet extends Applet
{
    static final int DEBUG = 0;

    // URL from which this applet was loaded.
    URL baseURL = null;

    Vector images = null;

    TextArea startInfo = null;
    boolean isInit = true;

    public DEViseJSValues jsValues = null;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
	    System.out.println("DEViseJSApplet.init()");
	}

	jsValues = new DEViseJSValues();

        jsValues.uiglobals.isApplet = true;

        isInit = true;

        jsValues.uiglobals.browser = getAppletContext();

        setLayout(new BorderLayout(0, 10));

	// Note: the "\n" here magically gets rid of the scrollbar
	// warnings we've had for ages.  RKW 2001-01-05.
        startInfo = new TextArea("\n", 8, 50);
        startInfo.setBackground(jsValues.uiglobals.textBg);
        startInfo.setForeground(jsValues.uiglobals.textFg);
        startInfo.setFont(jsValues.uiglobals.textFont);
        add(startInfo, BorderLayout.CENTER);

        setVisible(true);

        checkParameters();

        baseURL = getCodeBase();
        jsValues.connection.hostname = baseURL.getHost();
        jsValues.connection.username = DEViseGlobals.DEFAULTUSER;
        jsValues.connection.password = DEViseGlobals.DEFAULTPASS;

        String version = System.getProperty("java.version");
        String vendor = System.getProperty("java.vendor");
        startInfo.append("The browser you used is: " + vendor + "\n");
        startInfo.append("The Java version your browser supported is: " + version + "\n");
        if (version.compareTo("1.1") < 0)  {
            startInfo.append("Error: Java version 1.1 or greater is needed to run this program\n");
            isInit = false;
            return;
        }
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseJSApplet.destroy()");
	}

        images = null;
    }

    protected void checkParameters()
    {
        String cmdport = getParameter("cmdport");
        if (cmdport != null) {
            try {
                int port = Integer.parseInt(cmdport);
                if (port < 1024 || port > 65535) {
                    throw new NumberFormatException();
		}
                jsValues.connection.cmdport = port;
                startInfo.append("Parameter cmdport " + port + " is used\n");
            } catch (NumberFormatException e) {
                jsValues.connection.cmdport = DEViseGlobals.DEFAULTCMDPORT;
            }
        } else {
            jsValues.connection.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        }

        String bg = getParameter("bgcolor");
        if (bg != null) {
            try {
                String[] str = DEViseGlobals.parseStr(bg, "+");
                if (str == null || str.length != 3) {
                    throw new NumberFormatException();
                }

                int r = Integer.parseInt(str[0]);
                int g = Integer.parseInt(str[1]);
                int b = Integer.parseInt(str[2]);
                if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                    throw new NumberFormatException();
                }

                Color c = new Color(r, g, b);
                jsValues.uiglobals.bg = c;
                startInfo.append("Parameter bgcolor (" + r + ", " + g +
		  ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String fg = getParameter("fgcolor");
        if (fg != null) {
            try {
                String[] str = DEViseGlobals.parseStr(fg, "+");
                if (str == null || str.length != 3) {
                    throw new NumberFormatException();
                }

                int r = Integer.parseInt(str[0]);
                int g = Integer.parseInt(str[1]);
                int b = Integer.parseInt(str[2]);
                if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                    throw new NumberFormatException();
                }

                Color c = new Color(r, g, b);
                jsValues.uiglobals.fg = c;
                startInfo.append("Parameter fgcolor (" + r + ", " + g +
		  ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String rsize = getParameter("rubberbandlimit");
        if (rsize != null) {
            try {
                String[] str = DEViseGlobals.parseStr(rsize, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                if (x < 0) {
                    x = 0;
                }

                if (y < 0) {
                    y = 0;
                }

                jsValues.uiglobals.rubberBandLimit.width = x;
                jsValues.uiglobals.rubberBandLimit.height = y;
                startInfo.append("Parameter rubber band limit (" + x + ", " +
		  y + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        jsValues.session.defaultName = getParameter("session");
        if (jsValues.session.defaultName == null) {
            startInfo.append("Parameter session is not specified!\n");
        } else {
            startInfo.append("Parameter session: " +
	      jsValues.session.defaultName + " is used\n");
        }

        String debug = getParameter("debug");
        if (debug != null) {
            try {
                jsValues.debug._debugLevel = Integer.parseInt(debug);
            } catch (NumberFormatException e) {
                jsValues.debug._debugLevel = 0;
            }
        } else {
            jsValues.debug._debugLevel = 0;
        }

        String dl = getParameter("log");
        if (dl != null) {
            try {
		jsValues.debug._logEnabled = (Integer.parseInt(dl) != 0);
            } catch (NumberFormatException e) {
		jsValues.debug._logEnabled = false;
            }
        } else {
	    jsValues.debug._logEnabled = false;
        }

        String useCgi = getParameter("usecgi");
        if (useCgi != null) {
            jsValues.connection.cgi = true;
        } else {
            jsValues.connection.cgi = false;
        }

        String cgiUrl = getParameter("cgiurl");
        if (cgiUrl != null) {
            jsValues.connection.cgiURL = cgiUrl;
        } else {
            jsValues.connection.cgiURL = null;
        }

        String clientLogName = getParameter("clientlog");
        if (clientLogName != null) {
            jsValues.session.clientLogName = clientLogName;
        } else {
            jsValues.session.clientLogName = null;
        }

        String autoPlayback = getParameter("autoplayback");
        if (autoPlayback != null) {
	    if (autoPlayback.equals("true")) {
		jsValues.session.autoPlayback = true;
	    } else {
		jsValues.session.autoPlayback = false;
	    }
        } else {
            jsValues.session.autoPlayback = false;
        }

        String playbackOriginal = getParameter("playbackoriginal");
        if (playbackOriginal != null) {
	    if (playbackOriginal.equals("true")) {
		jsValues.session.playbackOriginal = true;
	    } else {
		jsValues.session.playbackOriginal = false;
	    }
        } else {
            jsValues.session.playbackOriginal = false;
        }

        String playbackDisplay = getParameter("playbackdisplay");
        if (playbackDisplay != null) {
	    if (playbackDisplay.equals("true")) {
		jsValues.session.playbackDisplay = true;
	    } else {
		jsValues.session.playbackDisplay = false;
	    }
        } else {
            jsValues.session.playbackDisplay = true;
        }

        String disableButtons = getParameter("disablebuttons");
        if (disableButtons != null) {
	    if (disableButtons.equals("true")) {
		jsValues.session.disableButtons = true;
	    } else {
		jsValues.session.disableButtons = false;
	    }
        } else {
            jsValues.session.disableButtons = false;
        }

        String reloadString = getParameter("reloadapplet");
        if (reloadString != null) {
	    if (reloadString.equals("true")) {
		jsValues.session.reloadApplet = true;
	    } else {
		jsValues.session.reloadApplet = false;
	    }
        } else {
            jsValues.session.reloadApplet = true;
        }

        String leaderName = getParameter("collableadername");
        if (leaderName != null) {
	    jsValues.session.collabLeaderName = leaderName;
        } else {
            jsValues.session.collabLeaderName = null;
        }

        String leaderPass = getParameter("collableaderpass");
        if (leaderPass != null) {
	    jsValues.session.collabLeaderPass = leaderPass;
        } else {
            jsValues.session.collabLeaderPass = null;
        }

        String collabName = getParameter("collabname");
        if (collabName != null) {
	    jsValues.session.collabName = collabName;
        } else {
            jsValues.session.collabName = null;
        }

        String collabPass = getParameter("collabpass");
        if (collabPass != null) {
	    jsValues.session.collabPass = collabPass;
        } else {
            jsValues.session.collabPass = null;
        }
    }

    public void loadImages()
    {
        if (images == null) {
            images = new Vector();

            startInfo.append("Trying to load DEVise animation symbol ...\n");
            MediaTracker tracker = new MediaTracker(this);
            Image image = null;
            String imageName = null;
	    final int MAX_IMAGE_SIZE = 2000;
            byte[] imageData = new byte[MAX_IMAGE_SIZE];
            for (int i = 0; i < 11; i++)  {
                // get image from JAR file
                try {
                    imageName = "devise" + i + ".gif";
                    InputStream is = getClass().getResourceAsStream(imageName);
                    BufferedInputStream bis = new BufferedInputStream(is);
                    int count = bis.read(imageData, 0, MAX_IMAGE_SIZE);
		    if (count >= MAX_IMAGE_SIZE) {
		        //TEMP -- we could read more here, but I'm just
			// failing now because it's simpler.  RKW 2000-11-29.
			throw new IOException("Image is too large");
		    }
                    image = Toolkit.getDefaultToolkit().createImage(imageData, 0, count);
                } catch (IOException e) {
                    startInfo.append("Cannot load DEVise animation symbol!\nStarting Java Screen without animation effect!\n");
                    images = null;
                    break;
                }

                tracker.addImage(image, 0);
                try  {
                    tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }

                if (tracker.isErrorID(0)) {
                    startInfo.append("Cannot load DEVise animation symbol!\nStarting Java Screen without animation effect!\n");
                    images = null;
                    break;
                }

                images.addElement(image);
            }

            if (images != null) {
                startInfo.append("DEVise animation symbol successfully loaded\n");
            } else {
                isInit = false;
                return;
            }
        }
    }

    public String[][] getParameterInfo()
    {
        String [][] info = {
	  // Parameter Name   Type of Value      Description
	  {"cmdport",         "int",         "jspop command port"},
	  {"bgcolor",         "String",      "RGB values for bgcolor"},
	  {"fgcolor",         "String",      "RGB values for fgcolor"},
	  {"rubberbandlimit", "String",      "Minimum dimension for a rubberband"},
	  {"screensize",      "String",      "assumed java screen dimension"},
	  {"session",         "String",      "session file name"},
	  {"debug",           "int",         "whether or not display debug information"},

	  {"log",             "int",         "whether or not to log debug info"},
	  {"usecgi",          "int",         "whether or not to use CGI communication"},
	  {"cgiurl",          "String",      "URL for CGI communication"}
          };

        return info;
    }

    public String getAppletInfo()
    {
        return "DEVise Java Screen version 2.0\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }
}
