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

// Top-level class for JavaScreen applet with its window outside the
// web browser.  This class parses the applet parameters, etc., and
// then creates a jsdevisec.

// ------------------------------------------------------------------------

// ========================================================================

// jsa.java

package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import  java.awt.event.*;
import  java.io.*;
import  java.util.*;
import  javax.swing.*;

public class jsa extends DEViseJSApplet
{
    static final int DEBUG = 0;

    Button startButton = new Button("Click here to start/restart JavaScreen");
    Label startLabel =
        new Label("(DEVise data visualization will appear in a new window)",
                  Label.CENTER);

    jscframe jsf = null;
    boolean started = false;

    public void init()
    {
        super.init();

        if (DEBUG >= 1) {
            System.out.println("jsa.init()");
        }

        jsValues.uiglobals.inBrowser = false;

        //add(startButton, BorderLayout.NORTH);
        startButton.setEnabled(false);
        add(startLabel, BorderLayout.NORTH);

        startButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                startJS();
            }
        });
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.start()");
        }

        if (started) {
            if (jsf != null && !jsf.isQuit()) {
                jsf.displayMe(true);
            }
        } else {
            loadImages();

            //startButton.setEnabled(true);

            //if (jsValues.session.defaultName != null) {
            startJS();
            //}

            repaint();

            started = true;
        }

        super.start();
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.stop()");
        }

        if (jsf != null) {
            jsf.destroy();
            jsf = null;
        }

        super.stop();
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.destroy()");
        }

        if (jsf != null) {
            jsf.destroy();
            jsf = null;
        }
        startButton.setEnabled(false);

        super.destroy();
    }

    private void startJS()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.startJS()");
        }

        if (jsf == null) {
            startInfo.append("Start Java Screen ...\n");
            jsf = new jscframe(this, images, jsValues);
            //startButton.setEnabled(false);
        } else {
            // Note: we get here if the user clicks the start
            // button when a JS already exists.  RKW 2001-01-05.
            if (jsf.isQuit()) {
                startInfo.append("Start new Java Screen ...\n");
                jsf = null;
                jsf = new jscframe(this, images, jsValues);
                //startButton.setEnabled(false);
            } else {
                startInfo.append("Java Screen already started!\n");
            }
        }
    }

    protected void checkParameters()
    {
        super.checkParameters();

        String screen = getParameter("screensize");
        if (screen != null) {
            try {
                String[] str = DEViseGlobals.parseStr(screen, "x");
                if (str == null || str.length != 2) {
                    throw new NumberFormatException();
                }

                int x = Integer.parseInt(str[0]);
                int y = Integer.parseInt(str[1]);

                jsValues.uiglobals.screenSize.width = x;
                jsValues.uiglobals.screenSize.height = y;

                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {
            }
        }
    }

    public void refreshAllData(boolean doHome)
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.refreshAllData(" + doHome + ")");
        }

        jsf.jsc.refreshAllData(doHome);
    }

    public void restartSession()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.restartSession()");
        }

        jsf.jsc.restartSession();
    }

    public void closeSession()
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.closeSession()");
        }

        jsf.jsc.closeSession();
    }

    public void openSession(String fullSessionName)
    {
        if (DEBUG >= 1) {
            System.out.println("jsa.openSession(" + fullSessionName + ")");
        }

        jsf.jsc.openSession(fullSessionName);
    }
}

class jscframe extends JFrame
{
    static final int DEBUG = 0;

    public jsdevisec jsc = null;

    public jscframe(Applet parentApplet, Vector images,
                    DEViseJSValues jsValues)
    {
        if (DEBUG >= 1) {
            System.out.println("jscframe constructor");
        }

        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension dim = kit.getScreenSize();
        jsValues.uiglobals.maxScreenSize.width = dim.width - 80;
        jsValues.uiglobals.maxScreenSize.height = dim.height - 120;
        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        jsc = new jsdevisec(parentApplet, this, images, jsValues, false);
        add(jsc);
        setTitle(DEViseUIGlobals.javaScreenTitle);
        pack();
        jsc.start();

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

        show();
    }

    public boolean isQuit()
    {
        if (jsc == null) {
            return true;
        } else {
            return jsc.getQuitStatus();
        }
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jscframe.destroy()");
        }

        if (jsc != null) {
            jsc.destroy();
            jsc = null;
        }
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            jsc.destroy();

            return;
        }

        super.processEvent(event);
    }

    public void displayMe(boolean isShow)
    {
        if (DEBUG >= 1) {
            System.out.println("jscframe.displayMe(" + isShow + ")");
        }

        if (isShow) {
            if (!isShowing()) {
                setVisible(true);
            }
            if (jsc != null) {
                jsc.showDebug();
            }
        } else {
            if (isShowing()) {
                setVisible(false);
            }
            if (jsc != null) {
                jsc.hideDebug();
            }
        }
    }
}
