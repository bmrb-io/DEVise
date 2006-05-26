// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Create button and corresponding menu for Jmol functions.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2006/05/23 18:17:50  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.util.*;
import  java.text.*;
import  java.awt.*;
import  java.awt.event.*;

public class DEViseJmolMenuButton extends Button
{
    private static final boolean DEBUG = false;

    private PopupMenu jmolMenu;
    private Button thisButton;
    private DEViseCanvas3DJmol jmolCanvas;

    //-------------------------------------------------------------------
    public DEViseJmolMenuButton()
    {
        super("Jmol");

	thisButton = this;

	jmolMenu = new PopupMenu();

	MenuItem treeMenuItem = new MenuItem("Selection trees...");
	treeMenuItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent event)
		{
		    jmolCanvas.showTrees();
		}
	    });
	jmolMenu.add(treeMenuItem);

	add(jmolMenu);
	addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent event)
		{
		    jmolMenu.show(thisButton,
		      DEViseMainButtons.MENU_X_OFFSET,
		      DEViseMainButtons.MENU_Y_OFFSET);
		}
	    });
    }

    //-------------------------------------------------------------------
    public void show(DEViseCanvas3DJmol canvas)
    {
	jmolCanvas = canvas;
        super.show();
    }
}
