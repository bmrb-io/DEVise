// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
// Revision 1.4  1998/07/09 17:38:40  hongyu
// *** empty log message ***
//
// Revision 1.3  1998/06/11 15:07:51  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class js extends Frame
{
    private static String usage = new String("usage: java js -host[hostname]"
        + " -cmdport[port] -imgport[port] -user[username] -pass[password] -session[filename]"
        + " -debug[number] -bgcolor[number+number+number] -fgcolor[number+number+number]"
        + " -rubberbandlimit[widthxheight] -screensize[widthxheight]");
    // -host[hostname]:
    //      hostname: The IP address of the machine where jspop or Devise Server
    //                is running, if bland, use the defaults
    //      default: "localhost"
    //      example: "hostbiron.cs.wisc.edu"
    // -cmdport[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    //      example: "cmdport6666"
    // -imgport[port]:
    //      port: The img port, if blank, use the defaults
    //      default: 6688
    //      example: "-imgport6688"
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
    //      number: The debug level, no debug information is written if less than or equals to 0
    //      default: No Debug information is written
    //      example: "-debug0"
    // -bgcolor[number+number+number]
    //      number: RGB values, from 0 to 255 in r,g,b order
    //      default: 64+96+0
    //      example: "-bgcolor64+96+0
    // -fgcolor[number+number+number]
    //      number: RGB values, from 0 to 255 in r,g,b order
    //      default: 255+255+255
    //      example: "-bgcolor255+255+255
    // -rubberbandlimit[widthxheight]
    //      width, height: positive integer, rubber band dimension must larger
    //                     than these values before it is actually be considered as
    //                     a rubberband
    //      default: 0x0
    //      example: "-rubberbandlimit0x0"
    // -screensize[widthxheight]
    //      width, height: The assumed physical screen size, can not exceed the real
    //                     physical screen size, mininum is 320x200
    //      default: real physical screen size
    //      example: "-screensize640x480
    //
    private static String hostname = null;
    private static String sessionName = null;
    private static String username = null;
    private static String password = null;

    public jsdevisec jsc = null;

    public js()
    {
        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension screen = kit.getScreenSize();
        if (DEViseGlobals.SCREENSIZE.width < 320 || DEViseGlobals.SCREENSIZE.width > screen.width) {
            DEViseGlobals.SCREENSIZE.width = screen.width;
        }

        if (DEViseGlobals.SCREENSIZE.height < 200 || DEViseGlobals.SCREENSIZE.height > screen.height) {
            DEViseGlobals.SCREENSIZE.height = screen.height;
        }

        int width = DEViseGlobals.SCREENSIZE.width;
        int height = DEViseGlobals.SCREENSIZE.height;

        jsc = new jsdevisec(hostname, username, password, sessionName, null, this);
        add(jsc);
        setTitle("DEVise Java Screen");
        pack();

        Point loc = new Point(0, 0);
        Dimension size = getSize();
        loc.y = loc.y + height / 2 - size.height / 2;
        loc.x = loc.x + width / 2 - size.width / 2;
        if (loc.y < 0)
            loc.y = 0;
        if (loc.x < 0)
            loc.x = 0;

        setLocation(loc);

        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.quit();
            return;
        }

        super.processEvent(event);
    }

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version);
            System.exit(1);
        }

        // force java VM to run every object's finalizer on normal or abnormal exit
        //System.runFinalizersOnExit(true);

        checkArguments(args);

        YGlobals.YISAPPLET = false;
        YGlobals.YISGUI = true;

        YGlobals.start();

        if (hostname == null)
            hostname = DEViseGlobals.DEFAULTHOST;
        if (username == null)
            username = DEViseGlobals.DEFAULTUSER;
        if (password == null)
            password = DEViseGlobals.DEFAULTPASS;

        js frame = new js();
        frame.show();
    }

    private static void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-host")) {
                if (!args[i].substring(5).equals("")) {
                    hostname = args[i].substring(5);
                }
            } else if (args[i].startsWith("-cmdport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.CMDPORT = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid command port number " + args[i].substring(8) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-imgport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.IMGPORT = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid image port number " + args[i].substring(8) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-bgcolor")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        String[] str = YGlobals.Yparsestr(args[i].substring(8), "+");
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
                        YGlobals.YMSGBGCOLOR = c;
                        DEViseGlobals.uibgcolor = c;
                        DEViseGlobals.buttonbgcolor = c;
                        DEViseGlobals.dialogbgcolor = c;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid RGB values specified for bgcolor: " + args[i].substring(8) + "!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-fgcolor")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        String[] str = YGlobals.Yparsestr(args[i].substring(8), "+");
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
                        YGlobals.YMSGFGCOLOR = c;
                        DEViseGlobals.uifgcolor = c;
                        DEViseGlobals.buttonfgcolor = c;
                        DEViseGlobals.dialogfgcolor = c;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid RGB values specified for fgcolor: " + args[i].substring(8) + "!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-rubberbandlimit")) {
                if (!args[i].substring(16).equals("")) {
                    try {
                        String[] str = YGlobals.Yparsestr(args[i].substring(16), "x");
                        if (str == null || str.length != 2) {
                            throw new NumberFormatException();
                        }

                        int width = Integer.parseInt(str[0]);
                        int height = Integer.parseInt(str[1]);

                        if (width < 0 || height < 0) {
                            throw new NumberFormatException();
                        }

                        DEViseGlobals.RubberBandLimit.width = width;
                        DEViseGlobals.RubberBandLimit.height = height;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid width or height values specified for rubberbandlimit: " + args[i].substring(16) + "!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-screensize")) {
                if (!args[i].substring(11).equals("")) {
                    try {
                        String[] str = YGlobals.Yparsestr(args[i].substring(11), "x");
                        if (str == null || str.length != 2) {
                            throw new NumberFormatException();
                        }

                        int width = Integer.parseInt(str[0]);
                        int height = Integer.parseInt(str[1]);

                        if (width < 1 || height < 1) {
                            throw new NumberFormatException();
                        }

                        DEViseGlobals.SCREENSIZE.width = width;
                        DEViseGlobals.SCREENSIZE.height = height;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid width or height values specified for screen size: " + args[i].substring(11) + "!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-user")) {
                if (!args[i].substring(5).equals("")) {
                    username = args[i].substring(5);
                }
            } else if (args[i].startsWith("-pass")) {
                if (!args[i].substring(5).equals("")) {
                    password = args[i].substring(5);
                }
            } else if (args[i].startsWith("-session")) {
                if (!args[i].substring(8).equals("")) {
                    sessionName = args[i].substring(8);
                }
            } else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        YGlobals.YDEBUG = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid debug level " + args[i].substring(6) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                } else {
                    YGlobals.YDEBUG = 1;
                }
            } else {
                System.out.println("Invalid js option " + args[i] + " is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }
    }
}
