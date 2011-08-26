// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2011
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
// Revision 1.3.30.3  2011/08/25 21:35:53  wenger
// Hopefully final cleanup of the JavaScreen embedded button fixes.
//
// Revision 1.3.30.2  2011/06/07 18:00:56  wenger
// More cleanup of no-longer-used code (including some test code I added
// temporarily).
//
// Revision 1.3.30.1  2011/06/03 23:10:50  wenger
// Working on getting embedded buttons in the JS working again -- big
// change so far is getting rid of the paint() method in DEViseScreen
// -- I think it was an error that that ever existed.  Lots of test/debug
// code in place right now as I play around with getting buttons to work.
//
// Revision 1.3  2007/04/20 19:42:34  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.2.2.1  2007/03/16 17:12:46  adayton
// Add UI package
//
// Revision 1.2  2006/06/23 19:52:40  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.1.2.1  2006/06/14 16:32:01  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

/*import  java.awt.*;*/
import javax.swing.*;

public class DEViseButton extends JButton
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
