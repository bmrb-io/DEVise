// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is just to explicitly set the colors and font for buttons,
// to hopefully fix the problems in some browsers that the buttons don't
// inherit their color from higher-level objects.

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

//import  java.awt.*;
import java.awt.Dimension;

import javax.swing.ImageIcon;
import javax.swing.JButton;

public class DEViseButton extends JButton
{
    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseButton(String name, DEViseJSValues jsValues)
    {
        this(name, jsValues, null);
    }

    //-------------------------------------------------------------------
    public DEViseButton(String name, DEViseJSValues jsValues,
                        String tooltip)
    {
        super(name);

        setFont(jsValues.uiglobals.font);
        setBackground(jsValues.uiglobals.bg);
        setForeground(jsValues.uiglobals.fg);

        if (tooltip != null) {
            setToolTipText(tooltip);
        }
    }

}
