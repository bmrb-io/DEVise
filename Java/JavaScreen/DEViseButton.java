// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006
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

// $Id$

// $Log$
// Revision 1.1.2.1  2006/06/14 16:32:01  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;

public class DEViseButton extends Button
{
    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseButton(String name, DEViseJSValues jsValues)
    {
        super(name);

        setFont(jsValues.uiglobals.font);
        setBackground(jsValues.uiglobals.bg);
        setForeground(jsValues.uiglobals.fg);
    }
}
