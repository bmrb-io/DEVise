// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.42  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.41  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.40  1999/07/27 17:11:19  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/06/23 20:59:18  wenger
// Added standard DEVise header.
//

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
    TextField mouseX = new TextField(12), mouseY = new TextField(12);
    //DEViseTrafficLight light = null;

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

        setLayout(new FlowLayout(FlowLayout.LEFT, 2, 12));

        //if (images != null && images.size() == 11) {
        //    try {
        //        light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0");
        //    } catch (YException e) {
        //        light = null;
        //    }
        //}

        //if (light != null)
        //    add(light);

        //add(viewName);

        add(mouseX);
        add(mouseY);
    }

    public void updateInfo(String name, String x, String y)
    {
        if (name == null) {
            //viewName.setText("");
            if (jsc.parentFrame != null) {
                jsc.parentFrame.setTitle(DEViseGlobals.javaScreenTitle);
            }
        } else {
            //viewName.setText(name);
            if (jsc.parentFrame != null) {
                jsc.parentFrame.setTitle(DEViseGlobals.javaScreenTitle + "     \"" + name + "\"");
            }
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
        //viewName.setText("");
        if (jsc.parentFrame != null) {
            jsc.parentFrame.setTitle(DEViseGlobals.javaScreenTitle);
        }

        mouseX.setText("");
        mouseY.setText("");

        validate();
    }

    public void updateImage(int type, int isOn)
    {
        if (jsc.light != null) {
            jsc.light.updateImage(type, isOn);
            jsc.validate();
        }
    }

    public void updateCount(int number)
    {
        if (number < 0 || number > 99 || jsc.light == null)
            return;

        jsc.light.updateCount("" + number);
        jsc.validate();
    }
}
