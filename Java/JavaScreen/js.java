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

// ========================================================================

// js.java

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
