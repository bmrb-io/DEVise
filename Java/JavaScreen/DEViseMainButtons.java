// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Create the buttons and associated menus for the main JavaScreen window.
// I'm switching to menus for most things so that we don't have so darn
// many buttons; I moved the code into this new class because there's
// so much stuff in jsdevisec already.  Kent Wenger (wenger) 2003-12-03.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.4.1  2006/05/23 18:17:50  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//
// Revision 1.2  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.1.2.2  2003/12/22 22:47:15  wenger
// JavaScreen support for print color modes is now in place.
//
// Revision 1.1.2.1  2003/12/03 19:31:09  wenger
// Changed most buttons in the JavaScreen GUI to menus (to save space
// in preparation for adding new functionality).
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js;
package JavaScreen;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

public class DEViseMainButtons
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    public static int MENU_X_OFFSET = 10;
    public static int MENU_Y_OFFSET = 10;

    private jsdevisec _js = null;
    
    private Component[] _buttons = null;

    // Buttons that bring up menus.
    private Button sessionMenuButton = new Button("Session");
    //TEMP private Button sessionMenuButton = new Button("Cursor Mode");
    private Button viewMenuButton = new Button("View");
    private Button helpMenuButton = new Button("Help");

    // Menus.
    private PopupMenu sessionPM = new PopupMenu();
    //TEMP private PopupMenu cursorModePM = new PopupMenu();
    private PopupMenu viewPM = new PopupMenu();
    private PopupMenu helpPM = new PopupMenu();
    private Menu displayModeMenu = new PopupMenu("Display Mode");

    // Buttons that don't bring up menus.
    public  Button stopButton = new Button("Stop");

    // Menu items.
    private MenuItem openMenuItem = new MenuItem("Open...");
    private MenuItem closeMenuItem = new MenuItem("Close");
    private MenuItem restartMenuItem = new MenuItem("Restart");
    private MenuItem filterMenuItem = new MenuItem("Reset Filters");
    private MenuItem exitMenuItem = new MenuItem("Exit");

    private MenuItem normalDisplayMenuItem = new MenuItem("Normal");
    private MenuItem colorPrintDisplayMenuItem = new MenuItem("Color print");
    private MenuItem bwPrintDisplayMenuItem = new MenuItem("B/w print");

    private MenuItem setMenuItem = new MenuItem("Option...");
    private MenuItem modeMenuItem = new MenuItem("Mode...");
    private MenuItem collabMenuItem = new MenuItem("Collaborate...");
    private MenuItem playbackMenuItem = new MenuItem("Playback...");

    public static final String displayLogStr = "Show Log";
    public static final String closeLogStr = "Hide Log";
    private MenuItem logMenuItem = new MenuItem(displayLogStr);

    private MenuItem jsHelpMenuItem = new MenuItem("JavaScreen Help...");
    private MenuItem viewHelpMenuItem = new MenuItem("View Help...");
    private MenuItem aboutMenuItem = new MenuItem("About JavaScreen...");

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Constructor.
     * @param The jsdevisec object this object will be associated with.
     */
    public DEViseMainButtons(jsdevisec js)
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseMainButtons.DEViseMainButtons()");
	}

	_js = js;

        createButtons();
	addActions();
    }

    //-------------------------------------------------------------------
    /**
     * Get an array containing the buttons created in this object.
     * @return An array of Components (the buttons).
     */
    public Component[] getButtons()
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseMainButtons.getButtons()");
	}

	return _buttons;
    }

    //-------------------------------------------------------------------
    /**
     * Get the stop button (this is kind of a kludgey way to allow other
     * parts of the code to change its color).
     * @return The stop button.
     */
    public Button getStopButton()
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseMainButtons.getStopButton()");
	}

	return stopButton;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    /**
     * Create the buttons.
     */
    private void createButtons()
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseMainButtons.createButtons()");
	}

        _buttons = new Component[4];
        _buttons[0] = sessionMenuButton;
        _buttons[1] = viewMenuButton;
        _buttons[2] = stopButton;
        _buttons[3] = helpMenuButton;

	// Set up session menu.
        if (!_js.jsValues.uiglobals.inBrowser) {
	    sessionPM.add(openMenuItem);
	    sessionPM.add(closeMenuItem);
	}
	sessionPM.add(restartMenuItem);
	sessionPM.add(filterMenuItem);
        if (!_js.jsValues.uiglobals.inBrowser) {
	    sessionPM.add(exitMenuItem);
	}
	sessionMenuButton.add(sessionPM);

	// Set up display mode menu.
	displayModeMenu.add(normalDisplayMenuItem);
	displayModeMenu.add(colorPrintDisplayMenuItem);
	displayModeMenu.add(bwPrintDisplayMenuItem);

	// Set up view menu.
	viewPM.add(displayModeMenu);
	viewPM.add(setMenuItem);
	viewPM.add(modeMenuItem);
	viewPM.add(collabMenuItem);
	viewPM.add(playbackMenuItem);
	viewPM.add(logMenuItem);
	viewMenuButton.add(viewPM);

	// Set up help menu.
	//TEMP helpPM.add(jsHelpMenuItem);
	helpPM.add(viewHelpMenuItem);
	//TEMP helpPM.add(aboutMenuItem);
	helpMenuButton.add(helpPM);

    }

    //-------------------------------------------------------------------
    /**
     * Add action listeners to the buttons and menu items.
     */
    private void addActions()
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseMainButtons.addActions()");
	}

        sessionMenuButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
			sessionPM.show(sessionMenuButton, MENU_X_OFFSET,
			  MENU_Y_OFFSET);
                }
            });

        viewMenuButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
			viewPM.show(viewMenuButton, MENU_X_OFFSET,
			  MENU_Y_OFFSET);
                }
            });

        helpMenuButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
			helpPM.show(helpMenuButton, MENU_X_OFFSET,
			  MENU_Y_OFFSET);
                }
            });

        openMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.getSessionList();
                }
            });

        closeMenuItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent event)
	        {
	            _js.closeSession();
	        }
	    });


        restartMenuItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent event)
	        {
	            _js.restartSession();
	        }
	    });

        filterMenuItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent event)
	        {
		    _js.resetFilters();
	        }
            });

        exitMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    _js.checkQuit();
                }
            });

        normalDisplayMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setDisplayMode(0);
                }
            });

        colorPrintDisplayMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setDisplayMode(1);
                }
            });

        bwPrintDisplayMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setDisplayMode(2);
                }
            });

        setMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    _js.showSetting();
                }
            });

        modeMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setMode();
                }
            });

        collabMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.showCollab();
                }
            });	

        playbackMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setLogFile();
                }
            });	

        logMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.setLog(logMenuItem);
                }
            });

        stopButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
	            _js.stopCommand();
                }
            });

        viewHelpMenuItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
		    _js.viewHelp();
                }
            });
    }
}

// ========================================================================
