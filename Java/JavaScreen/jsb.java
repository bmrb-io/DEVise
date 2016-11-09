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

// Top-level class for JavaScreen applet with its window within the
// web browser.  This class parses the applet parameters, etc., and
// then creates a jsdevisec.

// ------------------------------------------------------------------------

// ========================================================================

// jsb.java

package JavaScreen;

import  java.awt.*;
import  java.io.*;
import  java.util.*;

public class jsb extends DEViseJSApplet
{
    static final int DEBUG = 0;

    jsdevisec jsc = null;

    public void init()
    {
        super.init();

        if (DEBUG >= 1) {
            System.out.println("jsb.init()");
        }

        jsValues.uiglobals.inBrowser = true;

        loadImages();

        if (isInit) {
            remove(startInfo);
            if (jsc == null || jsc.getQuitStatus()) {
                jsc = new jsdevisec(this, null, images, jsValues, false);
                add(jsc, BorderLayout.CENTER);
            } else {
                //jsc.displayMe(true);
                setVisible(true);
            }
        }
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.start()");
        }

        jsc.start();

        if (isInit && jsc != null && !jsc.getQuitStatus()) {
            setVisible(true);
        }
        if (jsc != null) {
            jsc.showDebug();
        }

        super.start();
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.stop()");
        }

        if (jsc != null) {
            jsc.destroy();
            jsc = null;
        }

        super.stop();
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.destroy()");
        }

        if (jsc != null) {
            jsc.destroy();
            jsc = null;
        }

        super.destroy();
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

                jsValues.uiglobals.maxScreenSize.width = x;
                jsValues.uiglobals.maxScreenSize.height = y;

                startInfo.append("Parameter screen size (" + x + ", " + y + ") is used\n");
            } catch (NumberFormatException e) {

            }
        }

        jsValues.uiglobals.minScreenSize.width = 300;
        jsValues.uiglobals.minScreenSize.height = 240;

        if (jsValues.uiglobals.maxScreenSize.width > 0 && jsValues.uiglobals.maxScreenSize.height > 0) {
            jsValues.uiglobals.maxScreenSize.width -= 6;
            jsValues.uiglobals.maxScreenSize.height -= 60;
        } else {
            jsValues.uiglobals.maxScreenSize.width = 640;
            jsValues.uiglobals.maxScreenSize.height = 480;
        }

        if (jsValues.uiglobals.maxScreenSize.width < jsValues.uiglobals.minScreenSize.width) {
            jsValues.uiglobals.maxScreenSize.width = jsValues.uiglobals.minScreenSize.width;
        }
        if (jsValues.uiglobals.maxScreenSize.height < jsValues.uiglobals.minScreenSize.height) {
            jsValues.uiglobals.maxScreenSize.height = jsValues.uiglobals.minScreenSize.height;
        }
    }

    public void refreshAllData(boolean doHome)
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.refreshAllData(" + doHome + ")");
        }

        jsc.refreshAllData(doHome);
    }


    public void restartSession()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.restartSession()");
        }

        jsc.restartSession();
    }

    public void closeSession()
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.closeSession()");
        }

        jsc.closeSession();
    }

    public void openSession(String fullSessionName)
    {
        if (DEBUG >= 1) {
            System.out.println("jsb.openSession(" + fullSessionName + ")");
        }

        jsc.openSession(fullSessionName);
    }
}
