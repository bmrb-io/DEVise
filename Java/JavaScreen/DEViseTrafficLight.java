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

// ========================================================================

// DEViseTrafficLight.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class DEViseTrafficLight extends JPanel
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
    // YImageCanvas [] label = new YImageCanvas[3];
    String[] c = {"S", "R", "P"};
    String counterString = null;

    public jsdevisec jsc;

    public DEViseTrafficLight(Image offi, Image oni, String s, jsdevisec js)
    throws YException
    {
        this(offi, oni, s, null, js);
    }

    public DEViseTrafficLight(Image offi, Image oni, String s,
                              String align, jsdevisec js) throws YException
    {
        jsc = js;
        onImage = oni;
        offImage = offi;

        for (int i = 0; i < 3; i++) {
            canvas[i] = new YImageCanvas(onImage);
            if (!canvas[i].setImage(offImage)) {
                throw new YException("Invalid Image!");
            }
            // label[i] = new YImageCanvas(c[i], null, jsc.jsValues.uiglobals.bg,
            // jsc.jsValues.uiglobals.fg, 12, 12, 1, 1);
        }
        counterString = s;
        canvas[3] = new YImageCanvas(counterString);

        if (align == null) {
            align = "RIGHT";
        }

        setFont(DEViseFonts.getFont(12, DEViseFonts.SANS_SERIF, 1, 0));
        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);

        JPanel lightPanel = new JPanel();
        // lightPanel.setLayout(new GridLayout(2, 4));
        lightPanel.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
        lightPanel.setFont(DEViseFonts.getFont(12, DEViseFonts.SANS_SERIF, 1, 0));
        lightPanel.setBackground(jsc.jsValues.uiglobals.bg);
        lightPanel.setForeground(jsc.jsValues.uiglobals.fg);

        // for (int i = 0; i < 3; i++) {
        //   lightPanel.add(label[i]);
        // }

        for (int i = 0; i < 3; i++) {
            lightPanel.add(canvas[i]);
        }

        setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
        add(lightPanel); // add the stoplights
        add(canvas[3]); // add the counter

        /* Removed by Andy Dayton
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
                        add(lightPanel);
                    } else {
                        gridbag.setConstraints(panel, c);
                        add(lightPanel);
                        gridbag.setConstraints(canvas[3], c);
                        add(canvas[3]);
                    }
                } else if (align.equals(ALIGN_LEFT)) {
                    add(canvas[3]);
                    add(lightPanel);
                } else if (align.equals(ALIGN_RIGHT)) {
                    add(lightPanel);
                    add(canvas[3]);
                } else {
                throw new YException("Illegal align value: " + align);
                }
        */

    }

    public void updateImage(int status, boolean isOn)
    {
        if (status < STATUS_IDLE || status > STATUS_PROCESSING) {
            return;
        }

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
        counterString = s;
        canvas[3].setString(counterString);
    }
}

