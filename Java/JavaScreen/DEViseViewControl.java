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
// Revision 1.15  1998/08/14 17:48:08  hongyu
// *** empty log message ***
//
// Revision 1.11  1998/06/11 15:07:47  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseViewControl extends Panel
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    Vector images = null;

    public TextField screenX = new TextField(4);
    public TextField screenY = new TextField(4);
    public Button setButton = new Button("Set Size");
    Label screenSizeTitle = new Label("");
    YImageCanvas canvas = null;
    DEViseTrafficLight light = null;

    boolean isEditable = true;

    public DEViseViewControl(jsdevisec what, Vector array)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        // the length of array should be 3
        images = array;

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        //setLayout(new GridLayout(1, 0));
        setLayout(new BorderLayout());

        screenX.setText("" + jscreen.getScreenDim().width);
        screenY.setText("" + jscreen.getScreenDim().height);

        Component[] component = new Component[4];
        component[0] = screenSizeTitle;
        component[1] = screenX;
        component[2] = screenY;
        component[3] = setButton;
        ComponentPanel panel = new ComponentPanel(component, "Vertical", 5);

        boolean isCanvas = false;
        /*
        if (images != null && images.size() > 4) {
            try {
                canvas = new YImageCanvas((Image)images.elementAt(4));
                isCanvas = true;
            } catch (YException e) {
                canvas = null;
            }
        }

        Panel lowPanel = new Panel();
        if (isCanvas) {
            lowPanel.add(canvas);
        }
        */

        isCanvas = false;
        if (images != null && images.size() > 6) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(5), (Image)images.elementAt(6), "0", "BOTTOM");
                isCanvas = true;
            } catch (YException e) {
                light = null;
            }
        }

        if (isCanvas) {
            add(light, BorderLayout.NORTH);
        }

        add(panel, BorderLayout.SOUTH);

        setButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    Dimension newDim = new Dimension((Integer.valueOf(screenX.getText())).intValue(), (Integer.valueOf(screenY.getText())).intValue());
                    if (newDim.width > (int)(1.0 * DEViseGlobals.SCREENSIZE.width)
                        || newDim.height > (int)(1.0 * DEViseGlobals.SCREENSIZE.height)
                        || newDim.width < (int)(0.5 * DEViseGlobals.SCREENSIZE.width)
                        || newDim.height < (int)(0.5 * DEViseGlobals.SCREENSIZE.height)) {
                        YGlobals.Yshowmsg(jsc, "Your actual screen size is " + DEViseGlobals.SCREENSIZE.width
                                         + " x " + DEViseGlobals.SCREENSIZE.height
                                         + "\nYour Java Screen size can not exceed 100% of your actual screen size"
                                         + "\n and it can not be less than 50% of your actual screen size!");
                    } else {
                        jscreen.setScreenDim(newDim);
                    }
                }
            });
    }

    public void updateControl(boolean flag)
    {
        isEditable = flag;

        screenX.setEditable(isEditable);
        screenY.setEditable(isEditable);
        setButton.setEnabled(isEditable);

        validate();
    }

    // type = 0, idle
    // type = 1, sending
    // type = 2, waiting
    // type = 3, receiving
    // type = 4, processing
    public void updateImage(int type, int isOn)
    {
        if (!jsc.inBrowser) {
            if (light != null) {
                light.updateImage(type, isOn);
                validate();
            }
        } else {
            if (jsc.light != null) {
                jsc.light.updateImage(type, isOn);
                jsc.validate();
            }
        }
    }

    public void updateCount(int number)
    {
        if (number < 0 || number > 99)
            return;

        if (!jsc.inBrowser) {
            if (light != null) {
                light.updateCount("" + number);
                validate();
            }
        } else {
            if (jsc.light != null) {
                jsc.light.updateCount("" + number);
                jsc.validate();
            }
        }
    }
}
