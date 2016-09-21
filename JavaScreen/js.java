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

// Top-level class for the JavaScreen running as an application, not
// an applet.  This class parses command-line arguments, etc., and
// then creates a jsdevisec.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.54.6.1  2015/06/19 17:02:57  wenger
// Changed "suggest" to "feedback" as per feedback from Eldon (still working
// on moving the feedback button to the right side).  Added -showallbut
// command-line flag (for debugging) that causes the JS to show the
// Jmol and session-specific buttons.
//
// Revision 1.54  2015/02/18 22:53:47  wenger
// The JavaScreen now reports in the log window how long each command
// takes.  Socket mode can now be turned on in an applet by setting the
// usecgi parameter to 0.  Added the capability to make jar files that
// request all-permissions instead of sandbox.
//
// Revision 1.53  2010/07/15 19:01:13  wenger
// Minor updates to the help page, etc., related to resizing.
//
// Revision 1.52  2010/05/28 19:29:11  wenger
// Changed open session dialog to show BMRB visualization types (with
// numbers) as the "main" value if they are available, with the new
// hidebmrbsess applet parameter and command-line argument to hide the
// file names completely if visualization types are available; added
// histogram visualization types.
//
// Revision 1.51  2007/06/27 17:48:00  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.50  2007/04/20 19:42:36  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.49  2007/02/22 23:20:22  wenger
// Merged the andyd_gui_br thru andyd_gui_br_2 to the trunk.
//
// Revision 1.48  2007/02/20 00:00:19  wenger
// Changed JavaScreen distribution tarfile to have JavaScreen version
// instead of DEVise version; minor cleanup to distribution scripts.
//
// Revision 1.47.6.7  2007/06/15 20:46:09  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.47.6.6  2007/06/15 16:34:42  wenger
// Got JavaScreen toolbar icon images to load correctly from jar files (and
// significantly cleaned up the image loading in general, getting rid of a
// bunch of duplicate code).
//
// Revision 1.47.6.5  2007/04/20 17:00:00  wenger
// Fixed the problem with the JavaScreen buttons showing up with the
// wrong font; improved handling of color arguments.
//
// Revision 1.47.6.4  2007/04/19 21:16:08  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.47.6.3  2007/04/10 22:50:27  wenger
// Undid a bunch of formatting changes to make subsequent merges to the
// trunk easier.
//
// Revision 1.47.6.2  2007/03/16 17:12:47  adayton
// Add UI package
//
// Revision 1.47.6.1  2007/02/13 18:35:15  adayton
// Updated basic colors, as well as images for 'traffic light' and throbber. Also started updating some components to use Swing, including the main application frame. Additional changes to my makefile as well.
//
// Revision 1.47  2005/12/06 20:00:22  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.46.2.2  2003/10/24 23:45:08  wenger
// Non-applet JS now continues without dialog if it can't get the animation
// symbols.
//
// Revision 1.46.2.1  2003/06/17 21:04:57  wenger
// Major improvements to command-line argument processing of all JavaScreen
// programs; we now save the -id value in the JSPoP to use for the usage
// log file; some minor cleanups of the auto test scripts; slight
// clarification of command documentation.
//
// Revision 1.46  2002/03/20 22:04:53  xuk
// Added automatic collaboration functionality.
// Four new parameters:
// collableadername: for collaboration leader to define a collaboration name
// collableaderpass: for collaboration leader to define the password
// collabname: for collab follower to specify the collaboration name
// collabpass: for collab follower to specify the collaboration passwd.
//
// Revision 1.45  2001/10/16 22:14:28  wenger
// Major cleanup of command playback code; fixed bug 711 (problem with
// command log playback).
//
// Revision 1.44  2001/10/10 19:28:25  xuk
// Added display control command line argument/parameter for auto playback.
// For JS application: command-line argument: -playbackdisplayoff to turn off display, default is turning on display;
// For JS applet: parameter playbackdisplay = true|false to turn on|off display.
//
// Revision 1.43  2001/10/05 15:51:35  wenger
// Minor cleanup: fixed usage messages, improved argument checking.
//
// Revision 1.42  2001/10/02 22:02:11  xuk
// Added new command-line option for client log playback.
// Modified checkArguments() to check -clientlog, -autoplayback and playbackoriginal arguments.
//
// Revision 1.41  2001/05/11 20:36:12  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.40  2001/05/07 21:53:26  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
//
// Revision 1.39  2001/01/08 20:31:55  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.38.4.9  2000/12/29 22:41:39  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.38.4.8  2000/12/11 22:14:19  wenger
// Merged chagnes from link_gui_improvements thru js_restart_improvements
// onto the branch, removing imgport code from the restart scripts.
//
// Revision 1.38.4.7  2000/11/29 22:40:57  wenger
// Abstracted common code from jsa and jsb into new DEViseJSApplet
// superclass; other minor cleanups.
//
// Revision 1.38.4.6  2000/11/22 17:44:00  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.38.4.5  2000/11/21 01:51:34  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.38.4.4  2000/11/16 17:30:52  wenger
// Made static variables into static final variables.
//
// Revision 1.38.4.3  2000/11/09 21:43:13  wenger
// js.cgi now uses CGI library, various other cleanups; changed CGI parameter
// names slightly; added -cgi argument to JS usage message.
//
// Revision 1.38.4.2  2000/10/28 19:29:59  xuk
// add argument -cgi cgiURL for cgi communication.
//
// Revision 1.38.4.1  2000/09/01 20:22:28  xuk
// In main() and checkArguments(), delete imgport checking.
//
// Revision 1.38  2000/07/19 20:11:38  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.37  2000/06/26 16:48:32  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.36  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.35  2000/04/05 06:25:40  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.34  2000/03/23 16:26:18  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.33  2000/02/18 22:21:17  wenger
// Various changes to make cron scripts work better with new two-machine
// setup: added -id argument to devise, jspop, jss; updated cron scripts
// that check status of jspop, etc.; improved usage messages of jspop,
// jss, js; improved DEVise.kill script; removed obsolete sections of
// Java code.
//
// Revision 1.32  1999/10/10 08:49:54  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.31  1999/06/23 20:59:20  wenger
// Added standard DEVise header.
//

// ========================================================================

// js.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.util.*;
import java.io.*;
import javax.swing.*;

public class js extends JFrame
{
    private static final String usage = new String(
        "Usage: java JavaScreen.js [options]\n" +
    "  -host[string]: host the jspop is running on (default: localhost)\n" +
    "  -cmdport[number]: port for command socket from jspop (default: " +
      DEViseGlobals.DEFAULTCMDPORT + ")\n" +
    "  -user[string]: name of user (default: " + DEViseGlobals.DEFAULTUSER
      + ")\n" +
    "  -pass[string]: password of user (default: " +
      DEViseGlobals.DEFAULTPASS + ")\n" +
    "  -session[filename]: session to load at startup (default: none)\n" +
    "  -fgcolor[number+number+number]: RGB for JavaScreen foreground\n" +
      "    (default: 0+0+0)\n" +
    "  -bgcolor[number+number+number]: RGB for JavaScreen background\n" +
      "    (default: 247+246+220)\n" +
    "  -sbgcolor[number+number+number]: RGB for JavaScreen screen background\n" +
      "    (default: 229+227+207)\n" +
    "  -rubberbandlimit[widthxheight]: minimum size for rubberband to " +
      "have any effect\n    (default: 4x4)\n" +
    "  -screensize[widthxheight]: window size in pixels\n" +
      "    (default: display width-80xdisplay height-120)\n" +
    "  -log: turn on debug logging (default: off)\n" +
    "  -usecgi: forces the JS to use CGI communication (default: off)\n" +
    "  -cgiurl: URL for CGI communcation (default: none)\n" +
    "  -debug[number]: set debug level (default: 0)\n" +
    "  -clientlog[logfile]: specify log to play back (default: none)\n" +
    "  -autoplayback: automatically start playing back client log file\n" +
      "    (default: false)\n" +
    "  -playbackoriginal: play back client log file at original speed\n" +
      "    (default: false)\n" +
    "  -playbackdisplayoff: turn off display during command log playback\n" +
    "  -hidebmrbsess: hide BMRB session names if the vis type is recognized\n" +
    "  -showallbut: show optional buttions (for debugging)\n" +
    "  -version: print the version and exit\n" +
    "  -usage: print this message and exit");

    // -host[hostname]:
    //      hostname: The IP address of the machine where jspop or DEVise Server
    //                is running, if bland, use the defaults
    //      default: "localhost"
    //      example: "hostbiron.cs.wisc.edu"
    // -cmdport[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    //      example: "cmdport6666"
    // -user[username]:
    //      username: the name of user, if blank, use the defaults
    //      default: 'guest'
    //      example: "-userguest"
    // -pass[password]:
    //      password: the password of the user, if blank, use the defaults,
    //                if username if blank, this field has no effect
    //      default: none
    //      example: "-passnone"
    // -session[filename]:
    //      filename: the name of the session file need to be opened
    //      default: none
    //      example: "-sessionCOD1.ds"
    // -debug[number]:
    //      number: The debug level, no debug information is written if less
    //              than or equals to 0
    //      default: No Debug information is written
    //      example: "-debug0"
    // -bgcolor[number+number+number]
    //      number: RGB values, from 0 to 255 in r,g,b order
    //      default: 247+246+220
    //      example: "-bgcolor64+96+0
    // -fgcolor[number+number+number]
    //      number: RGB values, from 0 to 255 in r,g,b order
    //      default: 0+0+0
    //      example: "-bgcolor255+255+255
    // -sbgcolor[number+number+number]
    //      number: RGB values, from 0 to 255 in r,g,b order
    //      default: 229+227+207
    //      example: "-sbgcolor255+255+255
    // -rubberbandlimit[widthxheight]
    //      width, height: positive integer, rubber band dimension must larger
    //                     than these values before it is actually be considered
    //                     as a rubberband
    //      default: 0x0
    //      example: "-rubberbandlimit0x0"
    // -screensize[widthxheight]
    //      width, height: The assumed physical screen size, can not exceed the
    //                     real physical screen size, mininum is 320x200
    //      default: real physical screen size
    //      example: "-screensize640x480
    //
    public DEViseJSValues jsValues = null;
    public jsdevisec jsc = null;
    private static boolean _showAllButtons = false;

    public js(DEViseJSValues jv)
    {
    jsValues = jv;

        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        jsValues.uiglobals.maxScreenSize.width = dim.width - 80;
        jsValues.uiglobals.maxScreenSize.height = dim.height - 120;
        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        // get the animation symbol images
    jsValues._imageLoadComp = this;
        Vector images = new Vector();
    try {
            for (int i = 0; i < 11; i++) {
            Image image = jsdevisec.loadImage("devise" + i + ".gif",
          jsValues);
                images.addElement(image);
        }
    } catch (Exception ex) {
        System.out.println("Error loading throbber images: " + ex);
    }

        // start JavaScreen
        jsc = new jsdevisec(null, this, images, jsValues, _showAllButtons);
        add(jsc);
        setTitle(DEViseUIGlobals.javaScreenTitle);
        pack();
    jsc.start();

        // reposition JavaScreen so it is in the center of the screen
        Point loc = new Point(0, 0);
        Dimension size = getSize();
        loc.y = loc.y + dim.height / 2 - size.height / 2;
        loc.x = loc.x + dim.width / 2 - size.width / 2;
        if (loc.y < 0) {
            loc.y = 0;
        }
        if (loc.x < 0) {
            loc.x = 0;
        }

        setLocation(loc);

        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);
/*
        System.out.println("Memory: " +
        Runtime.getRuntime().freeMemory() + "/" +
        Runtime.getRuntime().totalMemory());
*/
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.destroy();
            return;
        }

        super.processEvent(event);
    }

    public static void main(String[] args) throws YException
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater " +
          "is needed to run this program\nThe version you used is " +
          version);
            System.exit(1);
        }

    DEViseJSValues jv = new DEViseJSValues();

        checkArguments(args, jv);

        jv.uiglobals.isApplet = false;
    jv.uiglobals.inBrowser = false;
        jv.connection.connectionID = DEViseGlobals.DEFAULTID;

        if (jv.connection.hostname == null) {
      jv.connection.hostname = DEViseGlobals.DEFAULTHOST;
        }
        if (jv.connection.username == null) {
       jv.connection.username = DEViseGlobals.DEFAULTUSER;
        }
        if (jv.connection.password == null) {
       jv.connection.password = DEViseGlobals.DEFAULTPASS;
        }
        if (jv.connection.cmdport < 1024) {
      jv.connection.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        }

        js frame = new js(jv);
        frame.show();
    }

    private static void checkArguments(String[] args, DEViseJSValues jsValues)
      throws YException
    {
        for (int i = 0; i < args.length; i++) {
        StringBuffer argValue = new StringBuffer();

        if (DEViseGlobals.checkArgument(args[i], "-host", true, argValue)) {
            jsValues.connection.hostname = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-cmdport", true,
          argValue)) {
                try {
                    int port = Integer.parseInt(argValue.toString());
                    if (port < 1024 || port > 65535) {
                        throw new NumberFormatException();
                    }
                    jsValues.connection.cmdport = port;
                } catch (NumberFormatException e) {
                    throw new YException("Invalid command port number "
                        + argValue.toString() + " in arguments!\n"
                        + "Command port number must larger than 1023"
                        + " and smaller than 65536!\n");
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-bgcolor", true,
          argValue)) {
                try {
                    jsValues.uiglobals.bg = DEViseGlobals.str2Color(
              argValue.toString());
                } catch (NumberFormatException e) {
            throw new YException("Invalid RGB values specified for"
                        + " bgcolor \"" + argValue.toString() + "\"!\n"
                        + " Please use format \"R+G+B\" where R,G,B must"
                        + " be non-negative integer and smaller than 256!\n");
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-fgcolor",
          true, argValue)) {
                try {
                    jsValues.uiglobals.fg = DEViseGlobals.str2Color(
              argValue.toString());
                } catch (NumberFormatException e) {
                    throw new YException("Invalid RGB values specified for"
                        + " fgcolor \"" + argValue.toString() + "\"!\n"
                        + " Please use format \"R+G+B\" where R,G,B must"
                        + " be non-negative integer and smaller than 256!\n");
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-sbgcolor", true,
          argValue)) {
                try {
                    jsValues.uiglobals.screenBg = DEViseGlobals.str2Color(
              argValue.toString());
                } catch (NumberFormatException e) {
            throw new YException("Invalid RGB values specified for"
                        + " sbgcolor \"" + argValue.toString() + "\"!\n"
                        + " Please use format \"R+G+B\" where R,G,B must"
                        + " be non-negative integer and smaller than 256!\n");
                }


        } else if (DEViseGlobals.checkArgument(args[i], "-rubberbandlimit",
          true, argValue)) {
                try {
                    String[] str = DEViseGlobals.parseStr(
              argValue.toString(), "x");
                    if (str == null || str.length != 2) {
                        throw new NumberFormatException();
                    }

                    int width = Integer.parseInt(str[0]);
                    int height = Integer.parseInt(str[1]);

                    if (width < 0 || height < 0) {
                        throw new NumberFormatException();
                    }

                    jsValues.uiglobals.rubberBandLimit.width = width;
                    jsValues.uiglobals.rubberBandLimit.height = height;
                } catch (NumberFormatException e) {
                    throw new YException("Invalid width or height values"
                        + " specified for rubberbandlimit: \""
                        + argValue.toString() + "\"!\n"
                        + "Please use format \"WxH\" where W, H must be"
                        + " positive integer!\n");
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-screensize",
          true, argValue)) {
                try {
                    String[] str = DEViseGlobals.parseStr(
              argValue.toString(), "x");
                    if (str == null || str.length != 2) {
                        throw new NumberFormatException();
                    }

                    int width = Integer.parseInt(str[0]);
                    int height = Integer.parseInt(str[1]);

                    if (width < 1 || height < 1) {
                        throw new NumberFormatException();
                    }

                    jsValues.uiglobals.screenSize.width = width;
                    jsValues.uiglobals.screenSize.height = height;
                } catch (NumberFormatException e) {
                    throw new YException("Invalid width or height values "
                        + "specified for screen size: \""
                        + argValue.toString() + "\"!\n"
                        + "Please use format \"WxH\" where W, H must be"
                        + " positive integer!\n");
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-user", true,
          argValue)) {
            jsValues.connection.username = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-pass", true,
          argValue)) {
                jsValues.connection.password = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-session", true,
          argValue)) {
                jsValues.session.defaultName = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-debug", false,
          argValue)) {
                if (!argValue.toString().equals("")) {
                    try {
                        jsValues.debug._debugLevel = Integer.parseInt(
              argValue.toString());
                    } catch (NumberFormatException e) {
                        throw new YException("Invalid debug level \""
                            + argValue.toString() + "\" in arguments!\n");
                    }
                } else {
                    jsValues.debug._debugLevel = 1;
                }

        } else if (DEViseGlobals.checkArgument(args[i], "-usage", false,
          argValue)) {
                System.out.println(usage);
                System.exit(0);

        } else if (DEViseGlobals.checkArgument(args[i], "-version", false,
          argValue)) {
                System.out.println(DEViseGlobals.VERSION);
                System.exit(0);

        } else if (DEViseGlobals.checkArgument(args[i], "-log", false,
          argValue)) {
            jsValues.debug._logEnabled = true;

        } else if (DEViseGlobals.checkArgument(args[i], "-usecgi", false,
          argValue)) {
            jsValues.connection.useCgi = true;

        } else if (DEViseGlobals.checkArgument(args[i], "-cgiurl", true,
          argValue)) {
        jsValues.connection.cgiURL = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-clientlog",
          true, argValue)) {
        jsValues.session.clientLogName = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-autoplayback",
          false, argValue)) {
        jsValues.session.autoPlayback = true;

        } else if (DEViseGlobals.checkArgument(args[i],
          "-playbackoriginal", false, argValue)) {
        jsValues.session.playbackOriginal = true;

        } else if (DEViseGlobals.checkArgument(args[i],
          "-playbackdisplayoff", false, argValue)) {
        jsValues.session.playbackDisplay = false;

        } else if (DEViseGlobals.checkArgument(args[i],
          "-collableadername", true, argValue)) {
        jsValues.session.collabLeaderName = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i],
          "-collableaderpass", true, argValue)) {
        jsValues.session.collabLeaderPass = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-collabname",
          true, argValue)) {
        jsValues.session.collabName = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-collabpass",
          true, argValue)) {
        jsValues.session.collabPass = argValue.toString();

        } else if (DEViseGlobals.checkArgument(args[i], "-hidebmrbsess",
          false, argValue)) {
            jsValues.uiglobals._hideBmrbSessionNames = true;

        } else if (DEViseGlobals.checkArgument(args[i], "-showallbut",
          false, argValue)) {
        _showAllButtons = true;

            } else {
        throw new YException("Invalid js option \"" + args[i]
                    + "\" is given!\n" + usage);
            }
        }
    }
}
