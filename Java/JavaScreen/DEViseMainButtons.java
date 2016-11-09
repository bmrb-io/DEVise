// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2015
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

// ========================================================================

package JavaScreen;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import JavaScreen.UI.*;

public class DEViseMainButtons
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    public static int MENU_X_OFFSET = 10;
    public static int MENU_Y_OFFSET = 10;

    private jsdevisec _js = null;

    private Component[] _buttons = null;

    // DEViseButton that bring up menus.
    private DEViseButton sessionMenuButton;
    //TEMP private DEViseButton sessionMenuButton = new DEViseButton("Cursor Mode");
    private DEViseButton viewMenuButton;
    private DEViseButton helpMenuButton;

    // Menus.
    private PopupMenu sessionPM = new PopupMenu();
    //TEMP private PopupMenu cursorModePM = new PopupMenu();
    private PopupMenu viewPM = new PopupMenu();
    private PopupMenu helpPM = new PopupMenu();
    private Menu displayModeMenu = new PopupMenu("Display Mode");
    private Menu commModeMenu = new PopupMenu("Communication Mode");

    // DEViseButton that don't bring up menus.
    public  DEViseButton stopButton;
    public  DEViseButton feedbackButton;

    // Menu items.
    private MenuItem openMenuItem = new MenuItem("Open/Switch...");
    private MenuItem closeMenuItem = new MenuItem("Close");
    //private MenuItem restartMenuItem = new MenuItem("Restart");
    private MenuItem filterMenuItem = new MenuItem("Reset Axis Ranges");
    private MenuItem exitMenuItem = new MenuItem("Exit");

    private MenuItem normalDisplayMenuItem = new MenuItem("Normal");
    private MenuItem colorPrintDisplayMenuItem = new MenuItem("Color print");
    private MenuItem bwPrintDisplayMenuItem = new MenuItem("B/w print");

    private MenuItem setMenuItem = new MenuItem("Settings...");

    private MenuItem socketModeMenuItem = new MenuItem("Socket");
    private MenuItem cgiModeMenuItem = new MenuItem("CGI...");

    private MenuItem collabMenuItem = new MenuItem("Collaborate...");
    private MenuItem playbackMenuItem = new MenuItem("Playback...");

    public static final String displayLogStr = "Show Log...";
    public static final String closeLogStr = "Hide Log";
    private MenuItem logMenuItem = new MenuItem(displayLogStr);

    private MenuItem showViewHelpMenuItem = new MenuItem("Show View Help");
    private MenuItem hideViewHelpMenuItem = new MenuItem("Hide View Help");
    private MenuItem aboutMenuItem = new MenuItem("About JavaScreen...");

    private MenuItem jsHelpBrowserMenuItem = new MenuItem(
        "JavaScreen Help (in browser window)...");
    private MenuItem jsHelpInternalMenuItem = new MenuItem(
        "JavaScreen Help (in Java window)...");
    private MenuItem visHelpMenuItem = new MenuItem(
        "Help about this visualization...");

    // Dialogs
    private DEViseHtmlWindow helpWindow;

    private static double RESIZE_FACTOR = 1.25;

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
    public DEViseButton getStopButton()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseMainButtons.getStopButton()");
        }

        return stopButton;
    }

    /**
     * Get the feedback button (so that it can be stuck at the far ride
     * side of the JavaScreen).
     * @return The feedback button.
     */
    public DEViseButton getFeedbackButton()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseMainButtons.getFeedbackButton()");
        }

        return feedbackButton;
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
        sessionMenuButton = new DEViseButton("Session", _js.jsValues);
        viewMenuButton = new DEViseButton("Tools", _js.jsValues);
        helpMenuButton = new DEViseButton("Help", _js.jsValues);
        stopButton = new DEViseButton("Stop", _js.jsValues,
                                      "Stop the current operation");
        feedbackButton = new DEViseButton("Feedback", _js.jsValues,
                                          "Go to a form to give feedback");

        _buttons = new Component[5];
        _buttons[0] = sessionMenuButton;
        _buttons[1] = viewMenuButton;
        _buttons[2] = stopButton;
        _buttons[3] = helpMenuButton;
        _buttons[4] = feedbackButton;

        // Set up session menu.
        if (!_js.jsValues.uiglobals.inBrowser) {
            sessionPM.add(openMenuItem);
            sessionPM.add(closeMenuItem);
        }
        //sessionPM.add(restartMenuItem); -- Not needed since we are adding a refresh button -- kancherla
        sessionPM.add(filterMenuItem);
        if (!_js.jsValues.uiglobals.inBrowser) {
            sessionPM.add(exitMenuItem);
        }
        sessionMenuButton.add(sessionPM);

        // Set up display mode menu.
        displayModeMenu.add(normalDisplayMenuItem);
        displayModeMenu.add(colorPrintDisplayMenuItem);
        displayModeMenu.add(bwPrintDisplayMenuItem);

        // Set up communication mode menu.
        commModeMenu.add(socketModeMenuItem);
        commModeMenu.add(cgiModeMenuItem);

        // Set up view menu.
        viewPM.add(displayModeMenu);
        viewPM.add(setMenuItem);
        viewPM.add(commModeMenu);
        viewPM.add(collabMenuItem);
        viewPM.add(playbackMenuItem);
        viewPM.add(logMenuItem);
        viewMenuButton.add(viewPM);

        // Set up help menu.
        helpPM.add(jsHelpBrowserMenuItem);
        //TEMP helpPM.add(visHelpMenuItem);
        helpPM.add(showViewHelpMenuItem);
        helpPM.add(hideViewHelpMenuItem);
        helpPM.add(jsHelpInternalMenuItem);
        //TEMP helpPM.add(aboutMenuItem);
        helpMenuButton.add(helpPM);
        if (_js._parentApplet == null) {
            jsHelpBrowserMenuItem.setEnabled(false);
            feedbackButton.setEnabled(false);
        }
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

        /*restartMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.restartSession();
            }
        });*/

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

        socketModeMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.jsValues.connection.useCgi = false;
                _js.socketMode();

            }
        });

        cgiModeMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.setCgiUrl();
            }
        });

        setMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.showSetting();
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

        jsHelpBrowserMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.showHelpInBrowser();
            }
        });

        jsHelpInternalMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                String htmlText = DEViseHtmlWindow.getTextFromFile(
                                      _js.jsValues, DEViseGlobals.JS_HELP_URL);
                helpWindow = new DEViseHtmlWindow(
                    "DEVise JavaScreen help", htmlText);
            }
        });

        showViewHelpMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.showViewHelp();
            }
        });

        hideViewHelpMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.hideViewHelp();
            }
        });

        /*TEMP
                aboutMenuItem.addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent event)
                        {
                    _js.showAbout();
                        }
                    });
        TEMP*/

        feedbackButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                _js.showFeedbackInBrowser();
            }
        });
    }
}

// ========================================================================
