// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to display the status of the JavaScreen with
// the "traffic lights" and command counter.

// There is one instace of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9  2001/01/08 20:31:53  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.8.4.1  2000/11/21 01:51:33  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.8  2000/06/12 22:13:57  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.7  2000/05/22 17:52:49  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.6  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.5  2000/03/23 16:26:15  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.4  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseTrafficLight.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;

public class DEViseTrafficLight extends Panel
{
    public static final String ALIGN_TOP = "TOP";
    public static final String ALIGN_BOTTOM = "BOTTOM";
    public static final String ALIGN_LEFT = "LEFT";
    public static final String ALIGN_RIGHT = "RIGHT";

    public static final int STATUS_IDLE = 0;
    public static final int STATUS_SENDING = 1;
    public static final int STATUS_RECEIVING = 2;
    public static final int STATUS_PROCESSING = 3;

    Image onImage = null, offImage = null;
    YImageCanvas [] canvas = new YImageCanvas[4];
    YImageCanvas [] label = new YImageCanvas[3];
    String[] c = {"S", "R", "P"};
    String string = null;

    public jsdevisec jsc;

    public DEViseTrafficLight(Image offi, Image oni, String s, jsdevisec js)
      throws YException
    {
        this(offi, oni, s, null, js);
    }

    public DEViseTrafficLight(Image offi, Image oni, String s, String align, jsdevisec js)
      throws YException
    {
	jsc = js;
	
        onImage = oni;
        offImage = offi;

        for (int i = 0; i < 3; i++) {
            canvas[i] = new YImageCanvas(onImage);
            if (!canvas[i].setImage(offImage)) {
                throw new YException("Invalid Image!");
	    }
            label[i] = new YImageCanvas(c[i], null, jsc.jsValues.uiglobals.bg,
	      jsc.jsValues.uiglobals.fg, 12, 12, 1, 1);
        }
        string = s;
        canvas[3] = new YImageCanvas(string);

        if (align == null) {
            align = "RIGHT";
        }

        setFont(DEViseFonts.getFont(14, DEViseFonts.MONOSPACED, 1, 0));
        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);

        Panel panel = new Panel();
        panel.setLayout(new GridLayout(2, 4));
        panel.setFont(DEViseFonts.getFont(14, DEViseFonts.MONOSPACED, 1, 0));
        panel.setBackground(jsc.jsValues.uiglobals.bg);
        panel.setForeground(jsc.jsValues.uiglobals.fg);

        for (int i = 0; i < 3; i++) {
            panel.add(label[i]);
	}
        for (int i = 0; i < 3; i++) {
            panel.add(canvas[i]);
	}

	if (align.equals(ALIGN_TOP) || align.equals(ALIGN_BOTTOM)) {
            // set new gridbag layout
            GridBagLayout gridbag = new GridBagLayout();
            GridBagConstraints c = new GridBagConstraints();
            //c.gridx = GridBagConstraints.RELATIVE;
            //c.gridy = GridBagConstraints.RELATIVE;
            c.gridwidth = GridBagConstraints.REMAINDER;
            c.gridheight = 1;
            c.fill = GridBagConstraints.NONE;
            c.insets = new Insets(0, 0, 0, 0);
            c.ipadx = 0;
            c.ipady = 0;
            c.anchor = GridBagConstraints.CENTER;
            //c.weightx = 1.0;
            //c.weighty = 1.0;

            setLayout(gridbag);

	    if (align.equals(ALIGN_TOP)) {
                gridbag.setConstraints(canvas[3], c);
                add(canvas[3]);
                gridbag.setConstraints(panel, c);
                add(panel);
            } else {
                gridbag.setConstraints(panel, c);
                add(panel);
                gridbag.setConstraints(canvas[3], c);
                add(canvas[3]);
            }
        } else if (align.equals(ALIGN_LEFT)) {
            add(canvas[3]);
            add(panel);
        } else if (align.equals(ALIGN_RIGHT)) {
            add(panel);
            add(canvas[3]);
        } else {
	    throw new YException("Illegal align value: " + align);
        }
    }

    public void updateImage(int status, boolean isOn)
    {
        if (status < STATUS_IDLE || status > STATUS_PROCESSING)
            return;

        if (status == STATUS_IDLE) {
            for (int i = 0; i < 3; i++) {
                canvas[i].setImage(offImage);
            }
        } else {
            if (isOn) {
                canvas[status - 1].setImage(onImage);
            } else {
                canvas[status - 1].setImage(offImage);
            }
        }
    }

    public void updateCount(String s)
    {
        string = s;
        canvas[3].setString(string);
    }
}

