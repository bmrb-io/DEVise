// js.java
// last updated on 04/11/99

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class js extends Frame
{
    private static String usage = new String("usage: java js -host[hostname]"
        + " -cmdport[port] -imgport[port] -user[username] -pass[password]"
        + " -session[filename] -debug[number] -bgcolor[number+number+number]"
        + " -fgcolor[number+number+number] -rubberbandlimit[widthxheight]"
        + " -screensize[widthxheight]");
    // -host[hostname]:
    //      hostname: The IP address of the machine where jspop or DEVise Server
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
    //      number: The debug level, no debug information is written if less
    //              than or equals to 0
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
    private static String sessionName = null;
    private static int debugLevel = 0;

    public jsdevisec jsc = null;

    public js()
    {   
        // determine the "screen size" for JavaScreen
        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        int w = dim.width;
        int h = dim.height;

        if (DEViseGlobals.screenSize.width < 360
            || DEViseGlobals.screenSize.width > (w - 80)) {
            DEViseGlobals.screenSize.width = w - 80;
        }        
        DEViseGlobals.actualScreenSize.width = DEViseGlobals.screenSize.width + 80;

        if (DEViseGlobals.screenSize.height < 240
            || DEViseGlobals.screenSize.height > (h - 120)) {
            DEViseGlobals.screenSize.height = h - 120;
        }        
        DEViseGlobals.actualScreenSize.height = DEViseGlobals.screenSize.height + 120;

        // get the animation symbol images from server
        MediaTracker tracker = new MediaTracker(this);
        Toolkit toolkit = this.getToolkit();
        Vector images = new Vector();
        Image image = null;
        for (int i = 0; i < 11; i++) {
            image = toolkit.getImage("devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }

            if (tracker.isErrorID(0)) {
                YMsgBox box = new YMsgBox(this, true, true, "Can not get JavaScreen "
                    + "animation symbols!\nDo you wish to continue without "
                    + "animation effects?", "Confirm", YMsgBox.YMBXYESNO, null,
                    null, null);
                box.open();
                String result = box.getResult();
                if (result.equals(YMsgBox.YIDYES)) {
                    images = null;
                    break;
                } else {
                    System.exit(1);
                }
            }

            images.addElement(image);
        }

        // start JavaScreen
        jsc = new jsdevisec(this, images, debugLevel, sessionName);
        add(jsc);
        setTitle("DEVise JavaScreen");
        pack();

        // reposition JavaScreen so it is in the center of the screen
        Point loc = new Point(0, 0);
        Dimension size = getSize();
        loc.y = loc.y + h / 2 - size.height / 2;
        loc.x = loc.x + w / 2 - size.width / 2;
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
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\nThe version you used is " + version);
            System.exit(1);
        }

        checkArguments(args);

        DEViseGlobals.isApplet = false;
        DEViseGlobals.inBrowser = false;
        DEViseGlobals.connectionID = DEViseGlobals.DEFAULTID;

        if (DEViseGlobals.hostname == null)
            DEViseGlobals.hostname = DEViseGlobals.DEFAULTHOST;
        if (DEViseGlobals.username == null)
            DEViseGlobals.username = DEViseGlobals.DEFAULTUSER;
        if (DEViseGlobals.password == null)
            DEViseGlobals.password = DEViseGlobals.DEFAULTPASS;
        if (DEViseGlobals.cmdport < 1024)
            DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        if (DEViseGlobals.imgport < 1024)
            DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;

        js frame = new js();
        frame.show();
    }

    private static void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-host")) {
                if (!args[i].substring(5).equals("")) {
                    DEViseGlobals.hostname = args[i].substring(5);
                }
            } else if (args[i].startsWith("-cmdport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.cmdport = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid command port number "
                            + args[i].substring(8) + " in arguments!\n"
                            + "Command port number must larger than 1023"
                            + " and smaller than 65536!\n");
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
                        DEViseGlobals.imgport = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid image port number "
                            + args[i].substring(8) + " in arguments!\n"
                            + "Image port number must larger than 1023"
                            + " and smaller than 65536!\n");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-bgcolor")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        String[] str = DEViseGlobals.parseStr(args[i].substring(8), "+");
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
                        DEViseGlobals.bg = c;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid RGB values specified for"
                            + " bgcolor \"" + args[i].substring(8) + "\"!\n"
                            + " Please use format \"R+G+B\" where R,G,B must"
                            + " be non-negative integer and smaller than 256!\n");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-fgcolor")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        String[] str = DEViseGlobals.parseStr(args[i].substring(8), "+");
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
                        DEViseGlobals.fg = c;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid RGB values specified for"
                            + " fgcolor \"" + args[i].substring(8) + "\"!\n"
                            + " Please use format \"R+G+B\" where R,G,B must"
                            + " be non-negative integer and smaller than 256!\n");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-rubberbandlimit")) {
                if (!args[i].substring(16).equals("")) {
                    try {
                        String[] str = DEViseGlobals.parseStr(args[i].substring(16), "x");
                        if (str == null || str.length != 2) {
                            throw new NumberFormatException();
                        }

                        int width = Integer.parseInt(str[0]);
                        int height = Integer.parseInt(str[1]);

                        if (width < 0 || height < 0) {
                            throw new NumberFormatException();
                        }

                        DEViseGlobals.rubberBandLimit.width = width;
                        DEViseGlobals.rubberBandLimit.height = height;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid width or height values"
                            + " specified for rubberbandlimit: \""
                            + args[i].substring(16) + "\"!\n"
                            + "Please use format \"WxH\" where W, H must be"
                            + " positive integer!\n");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-screensize")) {
                if (!args[i].substring(11).equals("")) {
                    try {
                        String[] str = DEViseGlobals.parseStr(args[i].substring(11), "x");
                        if (str == null || str.length != 2) {
                            throw new NumberFormatException();
                        }

                        int width = Integer.parseInt(str[0]);
                        int height = Integer.parseInt(str[1]);

                        if (width < 1 || height < 1) {
                            throw new NumberFormatException();
                        }

                        DEViseGlobals.screenSize.width = width;
                        DEViseGlobals.screenSize.height = height;
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid width or height values "
                            + "specified for screen size: \""
                            + args[i].substring(11) + "\"!\n"
                            + "Please use format \"WxH\" where W, H must be"
                            + " positive integer!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-user")) {
                if (!args[i].substring(5).equals("")) {
                    DEViseGlobals.username = args[i].substring(5);
                }
            } else if (args[i].startsWith("-pass")) {
                if (!args[i].substring(5).equals("")) {
                    DEViseGlobals.password = args[i].substring(5);
                }
            } else if (args[i].startsWith("-session")) {
                if (!args[i].substring(8).equals("")) {
                    sessionName = args[i].substring(8);
                }
            } else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        debugLevel = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid debug level \""
                            + args[i].substring(6) + "\" in arguments!\n");
                        System.exit(1);
                    }
                } else {
                    debugLevel = 1;
                }
            } else {
                System.out.println("Invalid js option \"" + args[i]
                    + "\" is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }
    }
}
