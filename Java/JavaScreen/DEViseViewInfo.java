// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$

// ========================================================================

// DEViseViewInfo.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseViewInfo extends Panel
{
    private jsdevisec jsc = null;
    Vector images = null;

    TextField viewName = new TextField(16);
    TextField mouseX = new TextField(13), mouseY = new TextField(13);
    DEViseTrafficLight light = null;

    public DEViseViewInfo(jsdevisec what, Vector array)
    {
        jsc = what;
        images = array;

        setBackground(DEViseGlobals.bg);
        setForeground(DEViseGlobals.fg);
        setFont(DEViseGlobals.font);

        viewName.setBackground(DEViseGlobals.textBg);
        viewName.setForeground(DEViseGlobals.textFg);
        viewName.setFont(DEViseGlobals.textFont);
        mouseX.setBackground(DEViseGlobals.textBg);
        mouseX.setForeground(DEViseGlobals.textFg);
        mouseX.setFont(DEViseGlobals.textFont);
        mouseY.setBackground(DEViseGlobals.textBg);
        mouseY.setForeground(DEViseGlobals.textFg);
        mouseY.setFont(DEViseGlobals.textFont);

        setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));

        if (images != null && images.size() == 11) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0");
            } catch (YException e) {
                light = null;
            }
        }

        if (light != null)
            add(light);

        add(viewName);
        add(mouseX);
        add(mouseY);
    }

    public void updateInfo(String name, String x, String y)
    {
        if (name == null) {
            viewName.setText("");
        } else {
            viewName.setText(name);
        }

        if (x == null) {
            mouseX.setText("");
        } else {
            mouseX.setText(x);
        }

        if (y == null) {
            mouseY.setText("");
        } else {
            mouseY.setText(y);
        }

        validate();
    }

    public void updateInfo(String x, String y)
    {
        updateInfo(null, x, y);
    }

    public void updateInfo()
    {
        viewName.setText("");
        mouseX.setText("");
        mouseY.setText("");

        validate();
    }

    public void updateImage(int type, int isOn)
    {
        if (light != null) {
            light.updateImage(type, isOn);
            validate();
        }
    }

    public void updateCount(int number)
    {
        if (number < 0 || number > 99 || light == null)
            return;

        light.updateCount("" + number);
        validate();
    }
}
