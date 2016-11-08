// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Create button and corresponding menu for session-specific menu items.

// Note: in the future, we may want to use this feature to replace
// "selection views" -- in that case, maybe cmd would be the JavaScreen
// command to be sent to the JSPoP when the menu item is selected.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4.14.1  2015/06/19 17:02:57  wenger
// Changed "suggest" to "feedback" as per feedback from Eldon (still working
// on moving the feedback button to the right side).  Added -showallbut
// command-line flag (for debugging) that causes the JS to show the
// Jmol and session-specific buttons.
//
// Revision 1.4  2012/05/10 21:35:53  wenger
// Fixed a bug in the session menu buttons that prevented the menu
// commands from getting updated properly (this caused the dynamics
// movies to always be generated with residue 5 selected).
//
// Revision 1.3  2012/04/30 22:20:18  wenger
// Merged js_data_save_br_0 thru js_data_save_br_1 to trunk.
//
// Revision 1.2.4.2  2012/04/30 20:39:37  wenger
// (Hopefully final) cleanup.
//
// Revision 1.2.4.1  2012/04/27 21:55:22  wenger
// Added dialog with the URL that's shown when we try to get the browser
// to show a document.
//
// Revision 1.2  2011/10/10 20:44:06  wenger
// Merged js_button_fix_br_1 thru js_button_fix_br_2 to trunk.
//
// Revision 1.1.2.3  2011/10/10 19:27:03  wenger
// More cleanup of session-specific menus: we now properly get rid of
// any session-specific menu items when we close a session.
//
// Revision 1.1.2.2  2011/10/06 15:52:04  wenger
// Some more cleanup of the "session-specific" menu code.
//
// Revision 1.1.2.1  2011/10/05 23:44:51  wenger
// Early version of "session-specific" menu working -- only works for
// showing URLs at this point.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.util.*;
import  java.text.*;
import  java.awt.*;
import  java.awt.event.*;
import  javax.swing.*;

public class DEViseSessionMenuButton extends DEViseButton
  implements ActionListener, ItemListener
{
    private static final int DEBUG = 0;

    private jsdevisec _jsc;
    private PopupMenu _sessionMenu;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseSessionMenuButton(jsdevisec jsc)
    {
        super("", jsc.jsValues);

    _jsc = jsc;
    setText("(undefined)");

    addActionListener(this);
    }

    //-------------------------------------------------------------------
    // Note: if we already have a menu item with the given label, this
    // won't do anything.
    public void addMenuItem(String menuType, String menuName,
      String label, String cmd)
    {
    if (DEBUG >= 1) {
        System.out.println("DEViseSessionMenuButton.addMenuItem(" +
          menuType + ", " + menuName + ", " + label + ", " + cmd + ")");
    }

    // Need to add more types here eventually...
        if (!menuType.equals("url_menu")) {
        System.err.println("Unsupported session menu type: " +
          menuType);
        return;
    }

    if (_sessionMenu == null) {
        _sessionMenu = new PopupMenu();
        add(_sessionMenu);
        setText(menuName);
    } else {
        if (!getLabel().equals(menuName)) {
        System.err.println("Menu name conflict: " + getLabel() +
          " vs. " + menuName);
            return;
        }
    }

    MenuItem menuItem = findMenuItem(label);
    if (menuItem == null) {
        menuItem = new MenuItem(label);
        menuItem.addActionListener(this);
        _sessionMenu.add(menuItem);
    }
    menuItem.setActionCommand(cmd);

    show();
    }

    //-------------------------------------------------------------------
    public void hide()
    {
    setText("");
    if (_sessionMenu != null) {
        remove(_sessionMenu);
    }
    _sessionMenu = null;
        super.hide();
    }

    //-------------------------------------------------------------------
    public void actionPerformed(ActionEvent event)
    {
    if (DEBUG >= 1) {
        System.out.println("DEViseSessionMenuButton.actionPerformed(" +
          event.getActionCommand() + ")");
    }

    if (event.getActionCommand().equals("")) {
        return;
    }

    if (event.getActionCommand().equals(getText())) {
        _sessionMenu.show(this, DEViseMainButtons.MENU_X_OFFSET,
          DEViseMainButtons.MENU_Y_OFFSET);

    } else {
        // Note: this is very specific to BMRB dynamics movies.
        if (event.getActionCommand().contains("&residues=")) {
            if (!_jsc.showDynamicsMovieDlg()) {
                // User clicked "Cancel".
                return;
            }
        }
        _jsc.showDocument(event.getActionCommand(), true);
    }

    }

    //-------------------------------------------------------------------
    // Not used now -- kept in case we add checkbox items.
    public void itemStateChanged(ItemEvent event)
    {
    if (DEBUG >= 1) {
        System.out.println("DEViseSessionMenuButton.itemStateChanged(" +
          event.paramString() + ")");
    }
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private MenuItem findMenuItem(String label)
    {
        for (int index = 0; index < _sessionMenu.getItemCount(); index++) {
        MenuItem item = _sessionMenu.getItem(index);
        if (label.equals(item.getLabel())) {
            return item;
        }
    }

    return null;
    }
}
