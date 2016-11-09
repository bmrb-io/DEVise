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

// Top-level class for JavaScreen applets.  This class contains code that
// was once duplicated in jsa.java and jsb.java.

// ------------------------------------------------------------------------

// ========================================================================

// jsa.java

package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;
import  java.applet.AppletContext;

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

    public int getInstanceNum() {
        return _instanceNum;
    }

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

        startInfo = new TextArea("\n", 8, 50,
                                 TextArea.SCROLLBARS_VERTICAL_ONLY);
        startInfo.setBackground(jsValues.uiglobals.textBg);
        startInfo.setForeground(jsValues.uiglobals.textFg);
        startInfo.setFont(jsValues.uiglobals.textFont);

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

    public void showFeedbackInBrowser(String sessionFile)
    {
        String archive = getParameter("archive");

        String firstElement;
        int firstComma = archive.indexOf(",");
        if (firstComma != -1) {
            firstElement = archive.substring(0, firstComma);
        } else {
            firstElement = archive;
        }

        String feedbackUrl;
        int lastSlash = firstElement.lastIndexOf("/");
        if (lastSlash != -1) {
            feedbackUrl = firstElement.substring(0, lastSlash) + "/" +
                          "html/feedback/javascreen_feedback.html";
        } else {
            feedbackUrl = "html/feedback/javascreen_feedback.html";
        }

        if (sessionFile != null) {
            feedbackUrl += "?session_file=" + sessionFile;
        }

        showDocument(feedbackUrl, "_blank", null);
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

        jsValues.connection.useCgi = true;
        String useCgi = getParameter("usecgi");
        if (useCgi != null) {
            if (Integer.parseInt(useCgi) != 0 ) {
                jsValues.connection.useCgi = true;
            } else {
                jsValues.connection.useCgi = false;
            }
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
