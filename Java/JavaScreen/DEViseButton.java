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

// $Id$

// $Log$
// Revision 1.5  2015/07/09 20:02:44  wenger
// Merged aditya_merge_br_0 thru aditya_merge_br_3 to trunk.
//
// Revision 1.4.14.3  2015/07/09 19:00:13  wenger
// Final cleanup of Aditya's changes.
//
// Revision 1.4.14.2  2015/07/08 20:57:11  wenger
// Merged aditya1_br_7 thru aditya1_br_8 to aditya_merge_br.
//
// Revision 1.4.14.1  2015/07/07 14:32:15  wenger
// Merged aditya1_br_3 thru aditya1_br_4 to aditya_merge_br.
//
// Revision 1.4.12.3  2015/01/26 21:05:18  kancherla
// THe constructor is no longer needed
//
// Revision 1.4.12.2  2014/12/29 19:32:58  kancherla
// *** empty log message ***
//
// Revision 1.4.12.1  2014/12/29 19:00:36  kancherla
// *** empty log message ***
//
// Revision 1.4.18.2  2015/07/20 21:18:21  wenger
// Moved Feedback button to the right side of the JavaScreen.
//
// Revision 1.4.18.1  2015/06/18 21:34:09  wenger
// First cut at the "Suggest" button and related HTML form.  Also, a few
// other changes to the menu buttons.  Fixed version in JavaScreen
// help page.
//
// Revision 1.4  2011/08/26 15:37:34  wenger
// Merged js_button_fix_br_0 thru js_button_fix_br_1 to trunk.
//
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
