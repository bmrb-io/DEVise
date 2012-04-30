// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2012
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
// Revision 1.24.12.3  2012/04/30 20:39:37  wenger
// (Hopefully final) cleanup.
//
// Revision 1.24.12.2  2012/04/27 21:55:22  wenger
// Added dialog with the URL that's shown when we try to get the browser
// to show a document.
//
// Revision 1.24.12.1  2012/04/26 22:57:31  wenger
// Created the DrillDown::AdjustFilterForCountMapping() method, and
// DataDownload uses DrillDown::GAttrLinkFollower() to remove
// duplicate code; removed some debug/test code.
//
// Revision 1.24  2010/06/02 15:12:05  wenger
// The JSA startInfo text object is no longer displayed if the applet
// height is less than 50 pixels (Eldon asked me to get rid of this).
//
// Revision 1.23  2010/05/28 19:29:11  wenger
// Changed open session dialog to show BMRB visualization types (with
// numbers) as the "main" value if they are available, with the new
// hidebmrbsess applet parameter and command-line argument to hide the
// file names completely if visualization types are available; added
// histogram visualization types.
//
// Revision 1.22  2010/04/21 17:10:10  wenger
// Merged devise_dist_rest_1003_br_0 thru devise_dist_rest_1003_br_1 to trunk.
//
// Revision 1.21.8.1  2010/03/29 18:17:47  wenger
// Got things to work as an applet with the latest Jmol version -- needed
// some more initialization in creating the JmolViewer object.  Added
// the jsdevisec.pnStackTrace() method, since we can't get a Java
// console with the lab's Firefox setup.
//
// Revision 1.21  2008/09/24 21:46:51  wenger
// Partially(?) fixed DEVise/JS bug 973: there is now a minimum delay
// before the JS stop button goes back to normal when communicating
// with the server, so the user won't fail to notice.
//
// Revision 1.20  2008/02/20 20:22:10  wenger
// JavaScreen now defaults to showing help in browser window (to
// get around problems on Mac with the Java window); added links
// and images to the help page because of this change.
//
// Revision 1.19  2008/01/24 20:30:53  wenger
// Merged js_ie_fix_br_0 thru js_ie_fix_br_1 to the trunk.
//
// Revision 1.18  2008/01/22 20:02:38  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron); I tried backporting
// my fix to the pre-toolbar version of the JS, but it doesn't work for
// some reason (I suspect that some of the other cleanups since then
// also affect the fix).  Note that this commit has a bunch of temporary
// code still in place; I want to get a working version into CVS ASAP.
//
// Revision 1.17  2007/06/27 17:47:58  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.16.6.1  2008/01/22 22:11:21  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron) in pre-toolbar version
// of the JavaScreen (the problem before was that I forgot to make
// jsdevisec.destroy() non-synchronized).
//
// Revision 1.16  2007/04/20 19:42:35  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.15  2007/02/23 16:57:41  wenger
// Changed applet parameter names for colors from "fgcolor" and "bgcolor"
// to "fgcolor2" and "bgcolor2" so existing web pages don't override
// the colors.
//
// Revision 1.14.2.3  2007/06/15 20:46:09  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.14.2.2  2007/06/15 16:34:42  wenger
// Got JavaScreen toolbar icon images to load correctly from jar files (and
// significantly cleaned up the image loading in general, getting rid of a
// bunch of duplicate code).
//
// Revision 1.14.2.1  2007/04/19 21:16:08  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.14  2006/09/18 17:01:39  wenger
// Made relative URLs work in the "clickable links in drill-down dialog"
// feature in the JavaScreen -- the JavaScreen now turns relative URLs
// into absolute ones before passing them to AppletContext.showDocument().
//
// Revision 1.13  2006/08/31 19:03:36  wenger
// Added "clickable URL in JavaScreen drill-down dialog" feature -- if
// attribute name ends in "_url" the JS considers it a URL.
//
// Revision 1.12  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.11  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.10.2.10  2003/06/26 20:18:04  wenger
// Added closeSession() and openSession() methods to the JS applet for
// Wavelet-IDR.
//
// Revision 1.10.2.9  2003/03/28 17:21:29  wenger
// Made JS invisibility destruction timeout configurable; fixed some other
// problems with loading and re-loading the applet.
//
// Revision 1.10.2.8  2003/01/28 15:29:28  wenger
// Made refreshAllData() and restartSession() methods in DEViseJSLoader,
// so that the Wavelet-IDR JavaScript code can call them.
//
// Revision 1.10.2.7  2002/12/18 15:19:03  wenger
// Disabled (at least temporarily) the applet no-reload feature in the
// JavaScreen code (because it just causes problems on Mozilla).
//
// Revision 1.10.2.6  2002/12/17 23:15:01  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.10.2.5  2002/11/22 21:55:07  wenger
// More fixes for stupid scrollbar warnings.
//
// Revision 1.10.2.4  2002/11/05 19:31:49  wenger
// Debug code for applet-reuse problems.
//
// Revision 1.10.2.3  2002/10/03 17:59:17  wenger
// JS applet instance re-use now reloads the session -- that's what
// Wavelet-IDR needs for their web page to work right.
//
// Revision 1.10.2.2  2002/08/19 16:51:36  wenger
// Applet instances are now re-used according to URL if the "reloadapplet"
// flag is false.
//
// Revision 1.10.2.1  2002/08/16 21:56:56  wenger
// Fixed bug 807 (reload twice in Netscape goofs up JS applets); fixed
// various other problems with destroying hidden applets.
//
// Revision 1.10  2002/03/20 22:04:49  xuk
// Added automatic collaboration functionality.
// Four new parameters:
// collableadername: for collaboration leader to define a collaboration name
// collableaderpass: for collaboration leader to define the password
// collabname: for collab follower to specify the collaboration name
// collabpass: for collab follower to specify the collaboration passwd.
//
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
import	java.applet.AppletContext;

public abstract class DEViseJSApplet extends Applet
{
    static final int DEBUG = 0;

    String documentUrl = null;

    // Maps URLs to applet instances.
    static Hashtable _instances = new Hashtable();

    Vector images = null;

    TextArea startInfo = null;
    boolean isInit = true;

    public DEViseJSValues jsValues = null;

    // How many applet instances exist.
    private static int _instanceCount = 0;

    // The instance number of this applet.
    private int _instanceNum = _instanceCount++;

    public int getInstanceNum() { return _instanceNum; }

    private DEViseJSTimer timer = null;
    
    private boolean _destroyed = false;

    public void init()
    {
	super.init();

        if (DEBUG >= 1) {
	    System.out.println("DEViseJSApplet.init()");
	    System.out.println("  Instance number " + _instanceNum);
	}

        documentUrl = stripUrl(getDocumentBase().toString());
	_instances.put(documentUrl, this);

	jsValues = new DEViseJSValues();

        jsValues.uiglobals.isApplet = true;

        isInit = true;

        jsValues.uiglobals.browser = getAppletContext();

        setLayout(new BorderLayout(0, 10));

	int appletHt = -1;
	String height = getParameter("height");
	if (height != null) {
	    try {
	        appletHt = Integer.parseInt(height);
	    } catch (NumberFormatException ex) {
	        appletHt = -1;
	    }
	}

	// Note: the "\n" here magically gets rid of the scrollbar
	// warnings we've had for ages.  RKW 2001-01-05.
        startInfo = new TextArea("\n", 8, 50,
	  TextArea.SCROLLBARS_VERTICAL_ONLY);
        startInfo.setBackground(jsValues.uiglobals.textBg);
        startInfo.setForeground(jsValues.uiglobals.textFg);
        startInfo.setFont(jsValues.uiglobals.textFont);
	// Option to hide the startInfo here added at Eldon's request
	// for BMRB.  wenger 2010-06-02.
	if (appletHt >= 50) {
            add(startInfo, BorderLayout.CENTER);
	}

        setVisible(true);

        checkParameters();

        URL codeBaseURL = getCodeBase();
        jsValues.connection.hostname = codeBaseURL.getHost();
        jsValues.connection.username = DEViseGlobals.DEFAULTUSER;
        jsValues.connection.password = DEViseGlobals.DEFAULTPASS;
	jsValues.connection.documentBase = getDocumentBase();
	jsValues.connection.codeBase = getCodeBase();

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

    // Note: if jsc is not null, we show a dialog with the URL.
    public void showDocument(String url, String targetWindow, jsdevisec jsc)
    {
        try {
	    String newUrl = null;
	    if (url.indexOf(":/") >= 0) {
	    	// URL is absolute
		newUrl = url;
	    } else {
	    	// URL is relative
		String tmpBase = getDocumentBase().toString();
		int lastSlash = tmpBase.lastIndexOf("/");
		newUrl = tmpBase.substring(0, lastSlash) + "/" + url;
	    }
            getAppletContext().showDocument(new URL(newUrl), targetWindow);
	    if (jsc != null) {
	        jsc.showUrlMsg(url);
	    }
        } catch (MalformedURLException ex) {
            System.err.println("Bad URL: " + ex);
        }
    }

    public void showHelpInBrowser()
    {
	String archive = getParameter("archive");

	String firstElement;
	int firstComma = archive.indexOf(",");
	if (firstComma != -1) {
	    firstElement = archive.substring(0, firstComma);
	} else {
	    firstElement = archive;
	}

	String helpUrl;
	int lastSlash = firstElement.lastIndexOf("/");
	if (lastSlash != -1) {
	    helpUrl = firstElement.substring(0, lastSlash) + "/" +
	      DEViseGlobals.JS_HELP_URL;
	} else {
	    helpUrl = DEViseGlobals.JS_HELP_URL;
	}

        showDocument(helpUrl, "_blank", null);
    }


    public void start()
    {
        if (timer != null) {
	    timer.stop();
	}
    }

    public void stop()
    {
        if (DEBUG >= 1) {
                System.out.println("DEViseJSApplet(" + _instanceNum +
                  ").stop()");
        }

        if (timer == null) {
	    timer = new DEViseJSTimer(this, jsValues.uiglobals.visTimeout);
	}
	timer.start();
    }

    // This method is used to destroy the applet from the DEViseJSTimer
    // thread.  If we don't go through here, destroy() deadlocks when it
    // tries to destroy the timer thread.
    public void destroyFromTimer()
    {
	// Note: we need to set destroyed to true here so that if we do
	// back/forward quickly in the browser, DEViseJSLoader realizes
	// that it can't just restart the existing applet. wenger 2003-03-28.
	_destroyed = true;
        timer = null;
	destroy();
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseJSApplet(" + _instanceNum +
	      ").destroy()");
	}
	
	if (timer != null) {
	    timer.stop();
	    timer = null;
	}

	if (documentUrl != null) {
	    _instances.remove(documentUrl);
	}

	documentUrl = null;
        images = null;
	startInfo = null;
	jsValues = null;

	_destroyed = true;
    }

    public boolean wasDestroyed()
    {
        return _destroyed;
    }

    public static DEViseJSApplet getInstance(String url)
    {
	if (DEBUG >= 2) {
	    System.out.println("DEViseJSApplet.getInstance(" + url + ")");
	}
	System.out.println("We have " + _instances.size() +
	  " applet instances in this JVM");
	if (DEBUG >= 2) {
	    System.out.println("URLs:");
	    Enumeration urls = _instances.keys();
	    while (urls.hasMoreElements()) {
		System.out.println("  " + urls.nextElement());
	    }
	}
	url = stripUrl(url);
        DEViseJSApplet instance = (DEViseJSApplet)_instances.get(url);
	if (DEBUG >= 2) {
	    System.out.println("  returning: " + ((instance != null) ?
	      new Integer(instance.getInstanceNum()).toString() : "null"));
	}
	return instance;
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

        String bg = getParameter("bgcolor2");
        if (bg != null) {
            try {
                jsValues.uiglobals.bg = DEViseGlobals.str2Color(bg);
                startInfo.append("Parameter bgcolor (" +
		  jsValues.uiglobals.bg + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String fg = getParameter("fgcolor2");
        if (fg != null) {
            try {
                jsValues.uiglobals.fg = DEViseGlobals.str2Color(bg);
                startInfo.append("Parameter fgcolor (" +
		  jsValues.uiglobals.fg + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }

        String sbg = getParameter("sbgcolor");
        if (sbg != null) {
            try {
                jsValues.uiglobals.screenBg = DEViseGlobals.str2Color(sbg);
                startInfo.append("Parameter sbgcolor (" +
		  jsValues.uiglobals.screenBg + ") is used\n");
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

        String hideBmrbSess = getParameter("hidebmrbsess");
        if (hideBmrbSess != null) {
            jsValues.uiglobals._hideBmrbSessionNames =
	      !hideBmrbSess.equals("0");
        } else {
            jsValues.uiglobals._hideBmrbSessionNames = false;
        }

	// Force reloadApplet to always be true for now, since the "no-reload"
	// feature doesn't work in Mozilla anyhow.  RKW 2002-12-18.
	/*TEMP?
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
	TEMP?*/
        jsValues.session.reloadApplet = true;//TEMP?


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

	String visTimeout = getParameter("vis_timeout");
	if (visTimeout != null) {
            try {
	        jsValues.uiglobals.visTimeout = Integer.parseInt(visTimeout);
            } catch (NumberFormatException e) {
	    	// Use the existing value.
	    }
	}

        String minWaitTime = getParameter("minwaittime");
        if (minWaitTime != null) {
            try {
                int value = Integer.parseInt(minWaitTime);
                jsValues.uiglobals.minWaitTime = value;
                startInfo.append("Parameter minwaittime " + value +
		  " is used\n");
            } catch (NumberFormatException e) {
                jsValues.uiglobals.minWaitTime =
		  DEViseGlobals.DEFAULT_MIN_WAIT_TIME;
            }
        } else {
            jsValues.uiglobals.minWaitTime =
	      DEViseGlobals.DEFAULT_MIN_WAIT_TIME;
        }
    }

    public void loadImages()
    {
        // get the animation symbol images 
	jsValues._imageLoadComp = this;
        images = new Vector();
	try {
            for (int i = 0; i < 11; i++) {
                Image image = jsdevisec.loadImage("devise" + i + ".gif",
		  jsValues);
                images.addElement(image);
            }

            startInfo.append("DEVise animation symbol successfully loaded\n");

	} catch (Exception ex) {
	    System.err.println("Error loading images: " + ex);
            isInit = false;
	}
    }

    public String[][] getParameterInfo()
    {
        String [][] info = {
	  // Parameter Name   Type of Value      Description
	  {"cmdport",         "int",         "jspop command port"},
	  {"bgcolor2",        "String",      "RGB values for bgcolor"},
	  {"fgcolor2",        "String",      "RGB values for fgcolor"},
	  {"sbgcolor",        "String",      "RGB values for screen bgcolor"},
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
        return "DEVise Java Screen version " + DEViseGlobals.VERSION +
	  "\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }

    // Strip the question mark (if any) and everything after it from the
    // given URL.
    private static String stripUrl(String url)
    {
        int qLoc = url.indexOf('?');
	if (qLoc >= 0) {
	    url = url.substring(0, qLoc);
	}

	return url;
    }

    // Force the applet to refresh all data in its current session.
    public abstract void refreshAllData(boolean doHome);

    // Force the applet to reload its current session.
    public abstract void restartSession();

    // Close the current session.
    public abstract void closeSession();

    // Open the given session.
    public abstract void openSession(String fullSessionName);
}
