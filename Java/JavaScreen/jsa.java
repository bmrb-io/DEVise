// jsa.java
// last updated on 04/21/99

import  java.applet.*;
import  java.awt.event.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class jsa extends Applet
{
    URL baseURL = null;

    String sessionName = null;
    int debugLevel = 0;

    Vector images = null;

    Button startButton = new Button("Start Java Screen");

    TextArea startInfo = null;

    boolean isInit = true;
    jscframe jsf = null;

    public void init()
    {
        DEViseGlobals.isApplet = true;
        DEViseGlobals.inBrowser = false;

        isInit = true;

        DEViseGlobals.browser = getAppletContext();

        setLayout(new BorderLayout(0, 10));

        add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);

        startButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (jsf == null) {
                            startInfo.append("Start Java Screen ...\n");
                            jsf = new jscframe(images, debugLevel, sessionName);
                            //startButton.setEnabled(false);
                        } else {
                            if (jsf.isQuit()) {
                                startInfo.append("Start Java Screen ...\n");
                                jsf = null;
                                jsf = new jscframe(images, debugLevel, sessionName);
                                //startButton.setEnabled(false);
                            } else {
                                startInfo.append("Java Screen already started!\n");
                            }
                        }
                    }
                });

        startInfo = new TextArea(8, 50);
        startInfo.setBackground(DEViseGlobals.textBg);
        startInfo.setForeground(DEViseGlobals.textFg);
        startInfo.setFont(DEViseGlobals.textFont);
        add(startInfo, BorderLayout.CENTER);

        setVisible(true);

        checkParameters();

        //baseURL = getDocumentBase();
        baseURL = getCodeBase();
        DEViseGlobals.hostname = baseURL.getHost();
        DEViseGlobals.username = DEViseGlobals.DEFAULTUSER;
        DEViseGlobals.password = DEViseGlobals.DEFAULTPASS;

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

    public void start()
    {
        if (sessionName != null) {
            startJS(true);
        } else {
            startJS(false);
        }
    }

    public void stop()
    {
        if (jsf != null && !jsf.isQuit()) {
            jsf.displayMe(false);
        }
    }

    public void destroy()
    {
        closeJS();

        super.destroy();
    }

    public void closeJS()
    {
        if (jsf != null && !jsf.isQuit()) {
            jsf.quit();
            jsf = null;
        }

        sessionName = null;
        images = null;
    }

    public void startJS(boolean flag)
    {
        if (images == null) {
            images = new Vector();

            startInfo.append("Trying to load DEVise animation symbol ...\n");
            MediaTracker tracker = new MediaTracker(this);
            Image image = null;
            for (int i = 0; i < 11; i++)  {
                image = getImage(baseURL, "devise" + i + ".gif");
                tracker.addImage(image, 0);
                try  {
                    tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }

                if (tracker.isErrorID(0)) {
                    startInfo.append("Can not load DEVise animation symbol!\nStarting Java Screen without animation effect!\n");
                    images = null;
                    break;
                }

                images.addElement(image);
            }

            if (images != null)
                startInfo.append("DEVise animation symbol successfully loaded\n");
        }

        startButton.setEnabled(true);

        if (jsf == null) {
            if (flag) {
                startInfo.append("Start Java Screen ...\n");
                jsf = new jscframe(images, debugLevel, sessionName);
                //startButton.setEnabled(false);
            }
        } else {
            if (jsf.isQuit()) {
                if (flag) {
                    startInfo.append("Start new Java Screen ...\n");
                    jsf = null;
                    jsf = new jscframe(images, debugLevel, sessionName);
                    //startButton.setEnabled(false);
                }
            } else {
                jsf.displayMe(true);
            }
        }
    }

    private void checkParameters()
    {
        sessionName = getParameter("session");
        if (sessionName == null) {
            startInfo.append("Parameter session is not specified!\n");
        } else {
            startInfo.append("Parameter session: " + sessionName + " is used\n");
        }

        String debug = getParameter("debug");
        if (debug != null) {
            try {
                debugLevel = Integer.parseInt(debug);
            } catch (NumberFormatException e) {
                debugLevel = 0;
            }
        } else {
            debugLevel = 0;
        }

        String cmdport = getParameter("cmdport");
        if (cmdport != null) {
            try {
                int port = Integer.parseInt(cmdport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.cmdport = port;
                startInfo.append("Parameter cmdport " + port + " is used\n");
            } catch (NumberFormatException e) {
                DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
            }
        } else {
            DEViseGlobals.cmdport = DEViseGlobals.DEFAULTCMDPORT;
        }

        String imgport = getParameter("imgport");
        if (imgport != null) {
            try {
                int port = Integer.parseInt(imgport);
                if (port < 1024 || port > 65535)
                    throw new NumberFormatException();
                DEViseGlobals.imgport = port;
                startInfo.append("Parameter imgport " + port + " is used\n");
            } catch (NumberFormatException e) {
                DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
            }
        } else {
            DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
        }

        String screen = getParameter("screensize");
        if (screen != null) {
            try {
                String[] str = DEViseGlobals.parseStr(screen, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                if (x < 320) {
                    x = 320;
                }

                if (y < 200) {
                    y = 200;
                }

                DEViseGlobals.screenSize.width = x;
                DEViseGlobals.screenSize.height = y;
                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
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

                DEViseGlobals.rubberBandLimit.width = x;
                DEViseGlobals.rubberBandLimit.height = y;
                startInfo.append("Parameter rubber band limit (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {
            }
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
                DEViseGlobals.bg = c;
                startInfo.append("Parameter bgcolor (" + r + ", " + g + ", " + b + ") is used\n");
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
                DEViseGlobals.fg = c;
                startInfo.append("Parameter fgcolor (" + r + ", " + g + ", " + b + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }
    }

    public String[][] getParameterInfo()
    {
        String [][] info = {
               // Parameter Name   Type of Value      Description
                  {"debug",           "int",        "whether or not display debug information"},
                  {"cmdport",         "int",        "jspop command port"},
                  {"imgport",         "int",        "jspop image port"},
                  {"session",        "String",      "session file name"},
                  {"screensize",     "String",      "assumed java screen dimension"},
                  {"rubberbandlimit","String",      "Minimum dimension for a rubberband"},
                  {"bgcolor",        "String",      "RGB values for bgcolor"},
                  {"fgcolor",        "String",      "RGB values for fgcolor"}
               };

        return info;
    }

    public String getAppletInfo()
    {
        return "DEVise Java Screen version 2.0\nBy DEVise Development Group at UW-Madison\nAll rights reserved";
    }
}

class jscframe extends Frame
{
    public jsdevisec jsc = null;

    public jscframe(Vector images, int debugLevel, String sessionName)
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
        
        jsc = new jsdevisec(this, images, debugLevel, sessionName);
        add(jsc);
        setTitle("DEVise JavaScreen");
        pack();

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

        show();
    }

    public boolean isQuit()
    {
        return jsc.getQuitStatus();
    }

    public void quit()
    {
        jsc.quit();
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.quit();

            return;
        }

        super.processEvent(event);
    }

    public void displayMe(boolean isShow)
    {
        if (isShow) {
            if (!isShowing()) {
                setVisible(true);
            }
        } else {
            if (isShowing()) {
                setVisible(false);
            }
        }
    }

}
