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

// ========================================================================

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
