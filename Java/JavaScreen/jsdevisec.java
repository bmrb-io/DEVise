// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the top-level GUI components (buttons, dialog
// boxes, status display, etc.) of the JavaScreen.

// There is one instance of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// ========================================================================

// jsdevisec.java

package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import java.awt.Dialog.ModalityType;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;
import  java.lang.*;
import  org.jmol.viewer.JmolConstants;
import  javax.swing.*;
import  javax.swing.table.*;
import  javax.swing.event.ListSelectionListener;
import  javax.swing.event.ListSelectionEvent;

import JavaScreen.DEViseButton;
import JavaScreen.DEViseCanvas2D;
import  JavaScreen.UI.*;

public class jsdevisec extends JPanel
{
    static final int DEBUG = 0;

    private YLogGUI debugWindow = null;

    public DEViseCmdDispatcher dispatcher = null;

    public JFrame parentFrame = null;
    private boolean isCenterScreen = false;
    public DEViseJSApplet _parentApplet = null;

    public DEViseScreen jscreen = null;

    public DEViseMouseCursor mouseCursor = null;

    // Canvas in which a drill-down event has just occurred.
    public DEViseCanvas drillDownCanvas = null;

    // These are the beginnings of restructuring the UI into a UI branch
    private DEViseUIManager uiManager;
    private DEViseMenuPanel menuPanel = null;
    private DEViseStatusPanel statusPanel;
    private DEViseStatusMessage statusMessage;
    public DEViseToolBar toolBar;

    private DEViseMainButtons _mainButtons = null;

    public  DEViseButton stopButton = null;

    private Label commMode = new Label("");
    private Color commModeNormalColor = Color.gray;

    public DEViseAnimPanel animPanel = null;
    public DEViseViewInfo viewInfo = null;
    public DEViseTrafficLight light = null;

    // Box for displaying messages -- apparently kept as an object member
    // so we can know whether we're displaying a message.
    private YMsgBox msgbox = null;

    public SessionDlg sessiondlg = null;
    public RecordDlg recorddlg = null;
    public ServerStateDlg statedlg = null;
    private SettingDlg settingdlg = null;
    private DynMovieDlg dynMovieDlg = null;
    private SetCgiUrlDlg setcgiurldlg = null;
    private SetLogFileDlg setlogfiledlg = null;
    private CollabSelectDlg collabSelectDlg = null;
    public CollabIdDlg collabIdDlg = null;
    public CollabPassDlg collabpassdlg = null;
    public EnterCollabPassDlg entercollabpassdlg = null;
    public CollabStateDlg collabstatedlg = null;
    public ShowUrlDlg showUrlDlg = null;
    //TEMP public AboutDlg aboutDlg = null;

    private DEViseJmolMenuButton jmolButton;
    private DEViseSessionMenuButton sessionMenuButton;

    public boolean isSessionOpened = false;
    // Visualization type for BMRB sessions.
    public String _currentVisType = null;

    // This variable will tell us if a CollabIdDlg is open (only 1
    // should be open at a time)
    public boolean isCollabIdDlgOpened = false;

    // This seems to be true if we are in the process of quitting -- avoids
    // re-entering the quit code(?).
    private boolean isQuit = false;

    public int stopNumber = 0;

    public boolean isShowProgramInfo = false;
    public String rootDir = "DEViseSession";
    public String currentDir = "DEViseSession";
    public String previousDir = rootDir;
    public String currentSession = null;

    public DEViseJSValues jsValues = null;

    // collaborated JS ID
    // -1: normal JS
    // 0: before the first round connection
    // >0: collaborated JS ID
    public int specialID = -1;

    public String collabPass = new String(DEViseGlobals.DEFAULTPASS);

    public boolean sessionSaved = false;

    public boolean collabinterrupted = false;

    // message buffer for logging
    public Vector msgBuffer = new Vector();

    // enable/disable collaboration
    public boolean isCollab = false;

    // for command log playback
    //TEMP -- these seem to duplicate values in DEViseJSValues.Session
    public boolean isPlayback = false;
    public String logFileName = null;
    public boolean isDisplay = true;
    public boolean isOriginal = true;
    private DEVisePlayback playback = null;

    private int oldScreenWidth = -1;
    private int oldScreenHeight = -1;
    private int oldScreenRes = -1;

    // Turning this on sets various GUI components to different colors
    // so we can see what is what.
    private boolean FUNKY_COLORS = false;

    private DEViseScreenResizeHandler _resizeHandler =
        new DEViseScreenResizeHandler(this);

    private int drillDownX;

    private int drillDownY;

    private DEViseCanvas2D dCanvas2D;

    //---------------------------------------------------------------------
    // Constructor.
    // Note:  showAllButtons is basically for debugging.
    public jsdevisec(Applet parentApplet, JFrame frame, Vector images,
                     DEViseJSValues jv, boolean showAllButtons)
    {
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("In jsdevisec constructor");
        }

        // Handle resizes of this component.
        addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                _resizeHandler.handleResize();
            }
        });

        // create the DEViseJSValues object
        jsValues = jv;

        if (jsValues.debug._logEnabled) {
            jsValues.debug._logger = new DEViseDebugLog(jsValues);
        }

        _parentApplet = (DEViseJSApplet)parentApplet;

        // frame might be null if JavaScreen is running inside a browser
        if (frame != null) {
            parentFrame = frame;
        } else {
            parentFrame = new JFrame();
            isCenterScreen = true;
        }

        if (jv.debug._debugLevel > 0) {
            System.out.println("Creating new debug window");
            debugWindow = new YLogGUI(jv.debug._debugLevel);
        }

        // determine the font size according to JavaScreen size
        int width = jsValues.uiglobals.maxScreenSize.width;
        int height = jsValues.uiglobals.maxScreenSize.height;

        if (width > 800) {
            jsValues.uiglobals.font = DEViseFonts.getFont(14,
                                      DEViseFonts.SANS_SERIF, 1, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(14,
                                       DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(14,
                                          DEViseFonts.SERIF, 0, 0);
        } else if (width > 640) {
            jsValues.uiglobals.font = DEViseFonts.getFont(12,
                                      DEViseFonts.SANS_SERIF, 1, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(12,
                                       DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(12,
                                          DEViseFonts.SERIF, 0, 0);
        } else {
            jsValues.uiglobals.font = DEViseFonts.getFont(10,
                                      DEViseFonts.SANS_SERIF, 1, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(10,
                                       DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(10,
                                          DEViseFonts.SERIF, 0, 0);
        }

        // set attributes for the main frame
        setBackground(jsValues.uiglobals.bg);
        setForeground(jsValues.uiglobals.fg);
        setFont(jsValues.uiglobals.font);
        setLayout(new BorderLayout(1, 1));

        // mouse cursors
        mouseCursor = new DEViseMouseCursor(jsValues);

        // throbber
        animPanel = new DEViseAnimPanel(this, images, 100);

        // main menu buttons
        _mainButtons = new DEViseMainButtons(this);
        stopButton = _mainButtons.getStopButton();
        Component[] buttons = _mainButtons.getButtons();
        DEViseComponentPanel buttonPanel = new DEViseComponentPanel(buttons,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 6,
                DEViseComponentPanel.ALIGN_LEFT, this);
        if (jsValues.connection.useCgi) {
            cgiMode();
        } else {
            socketMode();
        }
        buttonPanel.add(commMode);
        jmolButton = new DEViseJmolMenuButton(jsValues);
        buttonPanel.add(jmolButton);
        if (!showAllButtons) {
            jmolButton.hide();
        }

        sessionMenuButton = new DEViseSessionMenuButton(this);
        buttonPanel.add(sessionMenuButton);
        if (!showAllButtons) {
            sessionMenuButton.hide();
        }

        //Adding restart button
        JButton restartBtn = new JButton();
        try {
            ImageIcon icon = new ImageIcon(loadImage("resources/toolbar_icons/refresh.png", jsValues));
            restartBtn.setIcon(icon);
        } catch(Exception e) {
            System.err.println("Error creating restart button : " + e);
        }
        restartBtn.setBackground(jsValues.uiglobals.bg);
        restartBtn.setForeground(jsValues.uiglobals.fg);
        restartBtn.setBorderPainted(false);
        restartBtn.setContentAreaFilled(false);
        restartBtn.setOpaque(false);
        restartBtn.setMargin(new Insets(0, 0, 0, 0));
        restartBtn.setToolTipText("Restart Session");
        restartBtn.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                restartSession();
            }
        });
        buttonPanel.add(restartBtn, 0);

        // viewInfo contains the process counter
        viewInfo = new DEViseViewInfo(this, images);

        //
        // Constrain the screen width to be within the min and max width,
        // if they are valid; width is set to max if not already set.
        //
        if (jsValues.uiglobals.screenSize.width <= 0) {

            jsValues.uiglobals.screenSize.width =
                jsValues.uiglobals.maxScreenSize.width;

        } else if (jsValues.uiglobals.screenSize.width <
                   jsValues.uiglobals.minScreenSize.width &&
                   jsValues.uiglobals.screenSize.width > 0) {

            jsValues.uiglobals.screenSize.width =
                jsValues.uiglobals.minScreenSize.width;

        } else if (jsValues.uiglobals.screenSize.width >
                   jsValues.uiglobals.maxScreenSize.width) {

            jsValues.uiglobals.screenSize.width =
                jsValues.uiglobals.maxScreenSize.width;
        }

        //
        // Constrain the screen height to be within the min and max height,
        // if they are valid; height is set to max if not already set.
        //
        if (jsValues.uiglobals.screenSize.height <= 0) {

            jsValues.uiglobals.screenSize.height =
                jsValues.uiglobals.maxScreenSize.height;

        } else if (jsValues.uiglobals.screenSize.height <
                   jsValues.uiglobals.minScreenSize.height &&
                   jsValues.uiglobals.screenSize.height > 0) {

            jsValues.uiglobals.screenSize.height =
                jsValues.uiglobals.minScreenSize.height;

        } else if (jsValues.uiglobals.screenSize.height >
                   jsValues.uiglobals.maxScreenSize.height) {

            jsValues.uiglobals.screenSize.height =
                jsValues.uiglobals.maxScreenSize.height;
        }

        //
        // Determine the screen resolution.
        //
        Toolkit kit = Toolkit.getDefaultToolkit();
        jsValues.uiglobals.screenRes = kit.getScreenResolution();

        jscreen = new DEViseScreen(this);

        // Note: we're just relying on the file names of the images,
        // and the code that reads them in, to assume that image 9
        // and image 10 are what we need for the traffic light.
        if (images != null && images.size() == 11) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(9),
                                               (Image)images.elementAt(10), "0", this);
            } catch (YException e) {
                light = null;
            }
        }

        // Right now the DEViseStatusMessage class isn't implemented. Eventually
        // it will contain text-based status info about communication mode,
        // loading status, etc.
        statusMessage = new DEViseStatusMessage();
        statusMessage.setText("Select cursor/zoom in XY/3D rotate");


        // The menuPanel contains the throbber and main application menus.
        // Right now the background color is manually set using a value
        // from jsValues. Eventually this will hopefully be set using
        // Java Swing Look and Feel.
        menuPanel = new DEViseMenuPanel(animPanel, buttonPanel,
                                        _mainButtons.getFeedbackButton(),
                                        jsValues.session.disableButtons);
        menuPanel.setBackground(jsValues.uiglobals.bg);
        menuPanel.inheritBackground(); // causes children to inherit bg color
        if (jsValues.uiglobals.inBrowser) {
            menuPanel.setFont(DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0));
        }

        // The statusPanel is at the bottom of the interface and holds status
        // information. Right now the background color is manually set using a
        // value from jsValues. Eventually this will hopefully be set using
        // Java Swing Look and Feel.
        statusPanel = new DEViseStatusPanel(light, statusMessage, viewInfo);
        statusPanel.setBackground(jsValues.uiglobals.bg);
        statusPanel.inheritBackground(); // causes children to inherit bg color

        // ToolBar
        toolBar = new DEViseToolBar(jsValues, mouseCursor);
        toolBar.setBackground(jsValues.uiglobals.bg);


        // Add the main panels to the parent JPanel (this)
        add(menuPanel, BorderLayout.NORTH);
        add(statusPanel, BorderLayout.SOUTH);

        // Note: leftPanel and rightPanel exist only to carry the background
        // color around the sides of the jscreen, for a more unified look.
        // I got rid of the screenPanel partly because that goofed up
        // the fix for the jsb layout problem I just ran into.  wenger
        // 2007-04-19
        JPanel leftPanel = new JPanel();
        leftPanel.setBackground(jsValues.uiglobals.bg);
        add(leftPanel, BorderLayout.WEST);

        leftPanel.add(toolBar);

        JPanel rightPanel = new JPanel();
        rightPanel.setPreferredSize(new Dimension(3, 3));
        rightPanel.setBackground(jsValues.uiglobals.bg);
        add(rightPanel, BorderLayout.EAST);

        add(jscreen, BorderLayout.CENTER);

        isSessionOpened = false;

        //
        // Create the one DEViseCmdDispatcher instance we will have.
        //
        dispatcher = new DEViseCmdDispatcher(this);

        if (FUNKY_COLORS) {
            setBackground(Color.gray);
            // topPanel.setBackground(Color.magenta);
            // mainPanel.setBackground(Color.blue);
            menuPanel.setBackground(Color.red);
            buttonPanel.setBackground(Color.blue);
            animPanel.setBackground(Color.green);
            light.setBackground(Color.yellow);
            viewInfo.setBackground(Color.black);
            jscreen.setBackground(Color.orange);
            statusPanel.setBackground(Color.pink);
        }
    } // end of constructor

// ------------------------------------------------------------------------
// public methods
// ------------------------------------------------------------------------

    // Note: this method must be called after the sizes of the GUI
    // components have been established.
    public void start()
    {
        jscreen.setScreenDim(jscreen.getWidth(), jscreen.getHeight());

        //
        // Open the session if a session name was specified.
        //
        String sessionName = jsValues.session.defaultName;
        if (sessionName != null) {
            int index = sessionName.lastIndexOf('/');
            if (index > 0) {
                currentDir = currentDir + "/" + sessionName.substring(0, index);
                currentSession = sessionName.substring(index + 1, sessionName.length());
            } else {
                currentSession = sessionName;
            }

            if (jsValues.uiglobals._hideBmrbSessionNames) {
                _currentVisType = DEViseUtils.getVisType(currentSession);
            }

            String cmd = DEViseCommands.SET_DISPLAY_SIZE + " " +
                         jsValues.uiglobals.screenSize.width + " " +
                         jsValues.uiglobals.screenSize.height + " " +
                         jsValues.uiglobals.screenRes + " " +
                         jsValues.uiglobals.screenRes + "\n" +
                         DEViseCommands.OPEN_SESSION + " {" + currentDir + "/" +
                         currentSession + "}";

            // For collaboration leader, set collaboration name
            if (jsValues.session.collabLeaderName != null) {
                cmd = cmd + "\n" + DEViseCommands.SET_COLLAB_PASS +
                      " {" + jsValues.session.collabLeaderName +
                      "} {" + jsValues.session.collabLeaderPass + "}";
                isCollab = true;
                collabModeL();
            }

            dispatcher.start(cmd);
        } else {
            // for collaboration leader, set collaboration name
            if (jsValues.session.collabLeaderName != null) {
                String cmd = DEViseCommands.SET_COLLAB_PASS +
                             " {" + jsValues.session.collabLeaderName +
                             "} {" + jsValues.session.collabLeaderPass + "}";
                dispatcher.start(cmd);
                isCollab = true;
                collabModeL();
            }

            // for automatic collaboration
            if (jsValues.session.collabName != null) {
                String flag = new Integer(2).toString();
                String id = new Integer(0).toString();
                String cmd = DEViseCommands.COLLABORATE +
                             " {" + flag + "} {" + id + "} {" +
                             jsValues.session.collabName + "} {" +
                             jsValues.session.collabPass + "}";
                // temp specialID
                specialID = 0;
                dispatcher.start(cmd);
            }
        }

        if (jsValues.session.autoPlayback) {
            logFileName = jsValues.session.clientLogName;
            isOriginal = jsValues.session.playbackOriginal;
            isDisplay = jsValues.session.playbackDisplay;
            logPlayBack();
        }
    }

    public void refreshAllData(boolean doHome)
    {
        String cmd = DEViseCommands.REFRESH_DATA;
        cmd += doHome ? " 1" : " 0";
        dispatcher.start(cmd);
    }

    public void restartSession()
    {
        if (currentSession == null && !jsValues.uiglobals.inBrowser) {
            showMsg("You do not have any current session!");
            return;
        }
        dispatcher.start(DEViseCommands.OPEN_SESSION + " {" +
                         currentDir + "/" + currentSession + "}");
    }

    public void closeSession()
    {
        if (!isSessionOpened) {
            showMsg("You do not have any open session!");
            return;
        }
        _currentVisType = null;
        dispatcher.start(DEViseCommands.CLOSE_SESSION);
    }

    public void getSessionList()
    {
        // If we are a follower in collaboration mode
        if (specialID != -1) {
            showMsg("Cannot 'Open' while following.");
            return;
        }

        dispatcher.start(DEViseCommands.GET_SESSION_LIST + " {" +
                         currentDir + "}");
    }

    public void openSession(String fullSessionName)
    {
        if (jsValues.uiglobals._hideBmrbSessionNames) {
            _currentVisType = DEViseUtils.getVisType(fullSessionName);
        }

        jscreen.setScreenDim(jscreen.getWidth(), jscreen.getHeight());

        String command = "";
        if (isSessionOpened) {
            command += DEViseCommands.CLOSE_SESSION + "\n";
        }

        command += DEViseCommands.SET_DISPLAY_SIZE + " " +
                   jsValues.uiglobals.screenSize.width + " " +
                   jsValues.uiglobals.screenSize.height + " " +
                   jsValues.uiglobals.screenRes + " " +
                   jsValues.uiglobals.screenRes + "\n" +
                   DEViseCommands.OPEN_SESSION + " {" +
                   fullSessionName + "}";
        dispatcher.start(command);
    }

    public void resetFilters()
    {
        if (!isSessionOpened) {
            showMsg("You do not have any open session!");
            return;
        }
        dispatcher.start(DEViseCommands.RESET_FILTERS);
    }

    public void stopCommand()
    {
        // If we are a follower in collaboration mode
        if (specialID != -1) {
            showMsg("Cannot 'Stop' while following.\n" +
                    "To stop, choose 'Quit Following' from 'Collaborate'.");
            return;
        }

        if (dispatcher.getStatus() != DEViseCmdDispatcher.STATUS_IDLE) {
            stopNumber++;
            if (stopNumber > 1) {
                dispatcher.stop(true);
            } else {
                dispatcher.stop();
            }
        }
    }

    public void showViewHelp()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.viewHelp()");
        }
        if (!isSessionOpened) {
            showMsg("You do not have any open session!");
            return;
        }

        // Don't call showAllHelp() until we know that the dispatcher is
        // not busy.  This is to avoid lock-ups caused by calling
        // showAllHelp() twice before getting a response
        while(dispatcher.getStatus() != DEViseCmdDispatcher.STATUS_IDLE) {}

        jscreen.showAllHelp();
    }

    public void hideViewHelp()
    {
        jscreen.hideAllHelp();

        // Don't call hideAllHelp() until we know that the dispatcher is
        // not busy.  This is to avoid lock-ups caused by calling
        // showAllHelp() twice before getting a response
        while(dispatcher.getStatus() != DEViseCmdDispatcher.STATUS_IDLE) {}

        dispatcher.start(DEViseCommands.HIDE_ALL_VIEW_HELP);
    }

    public void hideDebug() {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.hideDebug()");
        }
        if (debugWindow != null) {
            debugWindow.setVisible(false);
        }
    }

    public void showDebug() {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showDebug()");
        }
        if (debugWindow != null) {
            debugWindow.setVisible(true);
        }
    }

    public void showJmol(DEViseCanvas3DJmol canvas)
    {
        Runnable doShowJmol = new DoShowJmol(canvas);
        SwingUtilities.invokeLater(doShowJmol);
    }

    private class DoShowJmol implements Runnable {
        private DEViseCanvas3DJmol _canvas;

        DoShowJmol(DEViseCanvas3DJmol canvas) {
            _canvas = canvas;
        }

        public void run() {
            jmolButton.show(_canvas);
            toolBar.setJmolCanvas(_canvas);
            validate();
        }
    }

    public void hideJmol()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.hideJmol()");
        }
        Runnable doHideJmol = new Runnable() {
            public void run() {
                jmolButton.hide();
                toolBar.setJmolCanvas(null);
                validate();
            }
        };

        SwingUtilities.invokeLater(doHideJmol);
    }

    public void createSessionMenuItem(String menuType, String menuName,
                                      String label, String cmd)
    {
        sessionMenuButton.addMenuItem(menuType, menuName, label, cmd);
    }

    public void hideSessionMenu()
    {
        Runnable doHideSessionMenu = new Runnable() {
            public void run() {
                sessionMenuButton.hide();
                validate();
            }
        };

        SwingUtilities.invokeLater(doHideSessionMenu);
    }

    public void showDocument(String url, boolean showDialog)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showDocument()");
        }
        pn("Showing URL: <" + url + ">");
        if (_parentApplet != null) {
            _parentApplet.showDocument(url, "_blank",
                                       showDialog ? this : null);
        } else {
            System.out.println("Can't show document " + url +
                               " because not an applet");
        }
    }

    public void showHelpInBrowser()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showHelpInBrowser()");
        }
        if (_parentApplet != null) {
            _parentApplet.showHelpInBrowser();
        } else {
            System.out.println("Can't show help in browser " +
                               "because not an applet");
        }
    }

    /*TEMP
        public void showAbout()
        {
            //TEMP -- version, copyright, web site

        //TEMP!!
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
          (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
          !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
              " calls jsdevisec.showAbout()");
        }
            aboutDlg = new AboutDlg(this, parentFrame, isCenterScreen);
            aboutDlg.open();
            aboutDlg = null;
        }
    TEMP*/

    public void showFeedbackInBrowser()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showFeedbackInBrowser()");
        }
        if (_parentApplet != null) {
            _parentApplet.showFeedbackInBrowser(currentSession);
        } else {
            System.out.println("Can't show feedback page in browser " +
                               "because not an applet");
        }
    }

    // print out message to debug window
    // pn() methods add a newline to the end of the string; p() methods don't.
    public void pn(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0 && debugWindow != null) {
            debugWindow.pn(msg, level);
        } else {
            msgBuffer.addElement(msg);
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0 && debugWindow != null) {
            debugWindow.p(msg, level);
        } else {
            msgBuffer.addElement(msg);
        }
    }

    public void pnStackTrace(Throwable throwable)
    {
        pn("Stack trace:");
        StackTraceElement ste[] = throwable.getStackTrace();
        for (int index = 0; index < ste.length; index++) {
            pn("  " + ste[index].toString());
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    // Shows or hides the log window, based on what the menu item is.
    public void setLog(MenuItem logMenuItem)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.setLog()");
        }
        String label = logMenuItem.getLabel();

        if (label.equals(DEViseMainButtons.displayLogStr)) {
            jsValues.debug._debugLevel = 1;
            debugWindow = new YLogGUI(jsValues.debug._debugLevel);
            showDebug();

            showProperty("java.version");
            showProperty("java.vendor");
            showProperty("java.class.version");
            pn("");

            showProperty("java.runtime.version");
            showProperty("java.runtime.name");
            pn("");

            showProperty("java.vm.version");
            showProperty("java.vm.vendor");
            pn("");

            showProperty("os.arch");
            showProperty("os.name");
            showProperty("os.version");
            pn("");

            pn("JavaScreen version: " + DEViseGlobals.VERSION);
            pn("Protocol version: " + DEViseGlobals.PROTOCOL_VERSION);
            pn("Jmol version: " + JmolConstants.version);
            pn("");

            /* Print all system properties
            Properties props = System.getProperties();
            Enumeration names = props.propertyNames();
            while (names.hasMoreElements()) {
            String name = (String)names.nextElement();
                showProperty(name);
            }
            pn("");
            */

            for (int i = 0; i < msgBuffer.size(); i++) {
                String msg = (String)msgBuffer.elementAt(i);
                pn(msg);
            }

            msgBuffer.removeAllElements();
            logMenuItem.setLabel(DEViseMainButtons.closeLogStr);

        } else {
            jsValues.debug._debugLevel = 0;
            hideDebug();
            debugWindow = null;
            logMenuItem.setLabel(DEViseMainButtons.displayLogStr);
        }

    }

    // Show the given property in the log window.
    public void showProperty(String propertyName)
    {
        try {
            String propValue = System.getProperty(propertyName);
            pn(propertyName + ": " + propValue);
        } catch (Exception ex) {
            pn("Exception (" + ex + ") showing property " + propertyName);
        }
    }

    // show message in message box
    public String showMsg(String msg, String title, int style)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 3 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 2 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showMsg()");
        }

        jsValues.debug.log("Showing message box: " + msg);
        /*  mainPanel.setBackground(jsValues.uiglobals.bg_warn);*/

        // Note: we have two cases here to make things work in the case
        // where we have more than one message box shown at once.  msgbox
        // is a class member so we know whether we have at least one
        // message box showing.
        if (msgbox == null) {
            msgbox = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, jsValues.uiglobals.font, jsValues.uiglobals.bg, jsValues.uiglobals.fg);
            msgbox.open();
            String result = msgbox.getResult();
            msgbox = null;
            /*      mainPanel.setBackground(jsValues.uiglobals.bg);*/
            jsValues.debug.log("Done with message box");

            return result;
        } else {
            // We only get here if we already have one message box showing,
            // and need to show another one.
            YMsgBox box = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, jsValues.uiglobals.font, jsValues.uiglobals.bg, jsValues.uiglobals.fg);
            box.open();
            String result = box.getResult();
            box = null;
            /*      mainPanel.setBackground(jsValues.uiglobals.bg);*/
            jsValues.debug.log("Done with message box");
            return result;
        }
    }

    public String showMsg(String msg)
    {
        return showMsg(msg, null, YMsgBox.YMBXOK);
    }

    public String confirmMsg(String msg)
    {
        return showMsg(msg, "Confirm", YMsgBox.YMBXYESNO);
    }

    // flag is apparently set to false if there is an error
    public void showSession(String[] msg, boolean flag)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showSession()");
        }
        if (flag) {
            if (sessiondlg != null) {
                sessiondlg.setSessionList(msg);
            } else {
                sessiondlg = new SessionDlg(this, parentFrame, isCenterScreen, msg);
                sessiondlg.open();
            }
        } else {
            showMsg(msg[0]);
            // Set the session directory back to the previous one and
            // update it on the session dialog.
            currentDir = previousDir;
            if (sessiondlg != null) {
                sessiondlg.setDirectory();
            }
        }
    }

    public void showClientList(String msg)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showClientList()");
        }
        if(isCollabIdDlgOpened == false) {
            try {
                collabIdDlg = new CollabIdDlg(this, parentFrame,
                                              isCenterScreen, msg);
                collabIdDlg.open();
            } catch (YException ex) {
                System.err.println(ex.getMessage());
            }
            collabIdDlg = null;
        }
    }

    public void showCollabPass()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showCollabPass()");
        }
        collabpassdlg = new CollabPassDlg(this, parentFrame, isCenterScreen);
        collabpassdlg.open();
        collabpassdlg = null;
    }

    public void displayID()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.displayID()");
        }
        String idstr = new Integer(jsValues.connection.connectionID).toString();
        showMsg("My client ID is:  " + idstr);
    }

    public void disableCollab()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.disableCollab()");
        }
        String command = new String();
        command = DEViseCommands.DISABLE_COLLAB;
        dispatcher.start(command);
    }

    public void enterCollabPass()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.enterCollabPass()");
        }
        entercollabpassdlg = new EnterCollabPassDlg(this, parentFrame, isCenterScreen);
        entercollabpassdlg.open();
        entercollabpassdlg = null;
    }

    public void showCollabState(String msg)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showCollabState()");
        }
        collabstatedlg = new CollabStateDlg(this, parentFrame, isCenterScreen, msg);
        collabstatedlg.open();
        if (specialID == -1 && ! jsValues.session.autoPlayback)
            collabstatedlg = null;
    }

    public void setDrillDownMarkerCoordinates(int xCoord, int yCoord, DEViseCanvas2D canvas2D) {
        drillDownX = xCoord;
        drillDownY = yCoord;
        dCanvas2D  = canvas2D;
    }

    public void showRecord(String[] msg)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showRecord()");
        }

        // Don't drill-down twice w/o user re-selecting drill-down mode.
        // toolBar.setNormal();
        recorddlg = RecordDlg.getInstance();
        if(recorddlg != null) {
            recorddlg.getOkButton().doClick(); //automatically closes an open dialog so that a new dialog can be opened
        }
        recorddlg = new RecordDlg(parentFrame, isCenterScreen, msg, this);
        dCanvas2D.drawDrillDownMark(drillDownX, drillDownY);
        recorddlg.setModalityType(ModalityType.MODELESS);
        recorddlg.open();
        if (specialID == -1 && ! jsValues.session.autoPlayback)
            recorddlg = null;
    }

    public String showViewDialogHelp(String msg) {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showViewDialogHelp()");
        }

        if(msg == null) {
            System.out.println("conflict 1");
            return null;
        }

        return showMsg(msg, "Help", YMsgBox.YMBXOK);
    }

    public void showServerState(String msg)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showServerState()");
        }
        statedlg = new ServerStateDlg(parentFrame, isCenterScreen, msg, this);
        statedlg.open();
        if (specialID == -1 && ! jsValues.session.autoPlayback)
            statedlg = null;
    }

    public void showSetting()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showSetting()");
        }
        settingdlg = new SettingDlg(this, parentFrame, isCenterScreen);
        settingdlg.open();
        settingdlg = null;
    }

    public boolean showDynamicsMovieDlg()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showDynamicsMovieDlg()");
        }
        dynMovieDlg = new DynMovieDlg(this, parentFrame,
                                      isCenterScreen);
        dynMovieDlg.open();
        boolean result = dynMovieDlg.isOk;
        dynMovieDlg = null;
        return result;
    }

    public void setCgiUrl()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.setCgiUrl()");
        }
        setcgiurldlg = new SetCgiUrlDlg(this, parentFrame, isCenterScreen);
        setcgiurldlg.open();
        setcgiurldlg = null;
    }

    // set the logfile name for command log playback
    public void setLogFile()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.setLogFile()");
        }
        setlogfiledlg = new SetLogFileDlg(this, parentFrame, isCenterScreen);
        setlogfiledlg.open();
        setlogfiledlg = null;
    }

    // command log playback
    public void logPlayBack()
    {
        playback = new DEVisePlayback(this, dispatcher, logFileName);
    }

    public void showCollab()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showCollab()");
        }
        collabSelectDlg = new CollabSelectDlg(this, parentFrame, isCenterScreen);
        collabSelectDlg.open();
        collabSelectDlg = null;
    }

    public boolean isShowingMsg()
    {
        if (sessiondlg != null || settingdlg != null || statedlg != null ||
                recorddlg != null || msgbox != null || setcgiurldlg != null ||
                dynMovieDlg != null) {
            return true;
        } else {
            return false;
        }
    }

    // Check whether we really want to quit, and call destroy() if so.
    // (We get here if the "Exit" button is pressed.)
    public synchronized void checkQuit()
    {
        boolean reallyQuit = true;

        if (specialID == -1) {
            if (dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                String result = confirmMsg("JavaScreen still busy talking " +
                                           "to server!\nDo you wish to exit anyway?");
                if (result.equals(YMsgBox.YIDNO)) {
                    reallyQuit = false;
                }
            }
        }
        //TEMP -- do we want to check for an open session here?

        if (reallyQuit) {
            destroy();
        }
    }

    public synchronized boolean getQuitStatus()
    {
        return isQuit;
    }

    public void destroy()
    {
        if (DEBUG >= 1) {
            System.out.println("jsdevisec.destroy()");
        }
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.destroy()");
        }
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            System.out.println("Thread count before jsdevisec.destroy(): " +
                               Thread.activeCount());
        }

        if (isQuit) {
            return;
        }

        isQuit = true;

        // Note: this must be done before destroying the dispatcher;
        // otherwise we get null pointer errors because we get window
        // events after the dispatcher has been destroyed.
        // parentFrame.dispose();
        parentFrame.setVisible(false);  //-- Fix for bug 1030  -- kancherla

        if (debugWindow != null) {
            debugWindow.dispose();
            debugWindow = null;
        }

        if (dispatcher != null) {
            if (specialID != -1 && dispatcher.dispatcherThread != null) {
                dispatcher.dispatcherThread.stop();
                dispatcher.dispatcherThread = null;
            }
            dispatcher.destroy();
            dispatcher = null;
        }

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("After jsdevisec.destroy()");
        }

        if (!jsValues.uiglobals.isApplet) {
            System.exit(0);
        }
    }

    public void socketMode()
    {
        commMode.setForeground(commModeNormalColor);
        commMode.setText("Socket");
    }

    public void setDisplayMode(int mode)
    {
        dispatcher.start(DEViseCommands.SET_DISPLAY_MODE + " " + mode);
    }

    // Go out of collaboration leader mode.  (This is called with isLeader
    // false if we are sort of halfway to being a leader.)
    public void collabModeUnlead(boolean isLeader)
    {
        isCollab = false;
        if (jsValues.connection.useCgi) {
            cgiMode();
        } else {
            socketMode();
        }
        if (isLeader) disableCollab();
        jsValues.session.collabLeaderPass = DEViseGlobals.DEFAULT_COLLAB_PASS;
    }

    // Go into collaboration leader mode.
    public void collabModeL()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.collabModeL()");
        }
        commMode.setForeground(commModeNormalColor);
        commMode.setText("Collab (L)");
    }

    // Go into collaboration follower mode.
    public void collabModeF()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.collabModeF()");
        }
        commMode.setForeground(commModeNormalColor);
        commMode.setText("Collab (F)");
    }

    public void cgiMode()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.cgiMode()");
        }
        commMode.setForeground(commModeNormalColor);
        commMode.setText("CGI");
    }

    public void disconnectedMode()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.disconnectedMode()");
        }
        commMode.setForeground(Color.red);
        commMode.setText("Disconnected");
    }

    public void playbackMode()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.playbackMode()");
        }
        commMode.setForeground(commModeNormalColor);
        commMode.setText("Playback");
    }

    public synchronized void collabInit(int leaderId)
    {
        pn("jsdevisec.collabInit(" + leaderId + " )");

        //TEMP We really should do this here (on the assumption that saving
        // an existing session will be taken care of by the JSPoP).
        // jscreen.updateScreen(false);

        //TEMP We really shouldn't set specialID until here, in case we
        // didn't succeed in collaborating, but not setting it earlier
        // somehow goofs things up.  RKW 2001-11-12.

        specialID = leaderId;

        collabModeF();
    }

    public synchronized void collabQuit()
    {
        pn("Quit from collaboration mode.");

        dispatcher.clearStatus();
        if ( !dispatcher.dispatcherThread.isInterrupted() )
            dispatcher.dispatcherThread.interrupt();
        animPanel.stop();
        stopButton.setBackground(jsValues.uiglobals.bg);
        jscreen.updateScreen(false);

        // go back to normal mode
        specialID = -1;
        socketMode();
    }

    // Restore any pre-collaboration session.
    public synchronized void restorePreCollab()
    {
        //TEMP -- this should really be taken care of by the DEViseClient
        // object in the JSPoP, but I don't want to make too many changes
        // right now.  RKW 2001-11-12.
        if (sessionSaved) {
            isSessionOpened = true;
            dispatcher.start(DEViseCommands.REOPEN_SESSION);
            sessionSaved = false;
        }
    }

    public void setDisplaySize(String command)
    {
        String[] cmds = DEViseGlobals.parseString(command);

        oldScreenWidth = jsValues.uiglobals.screenSize.width;
        oldScreenHeight = jsValues.uiglobals.screenSize.height;
        oldScreenRes = jsValues.uiglobals.screenRes;

        jsValues.uiglobals.screenSize.width = Integer.parseInt(cmds[1]);
        jsValues.uiglobals.screenSize.height = Integer.parseInt(cmds[2]);
        jsValues.uiglobals.screenRes = Integer.parseInt(cmds[3]);
    }

    public void restoreDisplaySize()
    {
        jsValues.uiglobals.screenSize.width = oldScreenWidth;
        jsValues.uiglobals.screenSize.height = oldScreenHeight;
        jsValues.uiglobals.screenRes = oldScreenRes;
    }

    public void showUrlMsg(String url)
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
                (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
                 !SwingUtilities.isEventDispatchThread())) {
            System.out.println(Thread.currentThread() +
                               " calls jsdevisec.showUrlMsg()");
        }
        showUrlDlg = new ShowUrlDlg(url, this, parentFrame, isCenterScreen);
        showUrlDlg.open();
        showUrlDlg = null;
    }

    public static Image loadImage(String imagePath, DEViseJSValues jsValues)
    throws YException, IOException
    {
        if (DEBUG >= 1) {
            System.out.println("jsdevisec.loadImage(" + imagePath + ")");
        }

        // Seems like we need the MediaTracker stuff in here to wait
        // for images to load -- if we don't do that, the images don't
        // seem to work right.  wenger 2007-06-15.
        if (jsValues._tracker == null) {
            jsValues._tracker = new MediaTracker(jsValues._imageLoadComp);
        }

        Image image = null;

        Toolkit tk = jsValues._imageLoadComp.getToolkit();

        InputStream is = jsValues._imageLoadComp.getClass().
                         getResourceAsStream(imagePath);
        BufferedInputStream bis = new BufferedInputStream(is);
        final int MAX_IMAGE_SIZE = 20000;
        byte[] imageData = new byte[MAX_IMAGE_SIZE];
        int count = bis.read(imageData, 0, MAX_IMAGE_SIZE);
        if (count >= MAX_IMAGE_SIZE) {
            //TEMP -- we could read more here, but I'm just
            // failing now because it's simpler.  RKW 2000-11-29.
            throw new IOException("Image is too large");
        }

        image = tk.createImage(imageData, 0, count);
        jsValues._tracker.addImage(image, 0);

        try  {
            jsValues._tracker.waitForID(0);
        }  catch (InterruptedException e)  {
            YException ex =  new YException("MediaTracker interrupted " +
                                            "while waiting for image " + imagePath);
            System.err.println(ex);
            throw ex;
        }

        if (jsValues._tracker.isErrorID(0)) {
            YException ex =  new YException("MediaTracker error " +
                                            "on image " + imagePath);
            System.err.println(ex);
            throw ex;
        }

        return image;
    }

    public DEViseStatusPanel getStatusPanel() {
        return statusPanel;
    }
}

// ------------------------------------------------------------------------

// Dialog to show record values (drill-down).
class RecordDlg extends Dialog
{
    private static RecordDlg instance = null;

    jsdevisec jsc = null;

    private DEViseButton okButton;

    private boolean status = false; // true means this dialog is showing
    private JTable table;
    String[] urls;

    public RecordDlg(Frame owner, boolean isCenterScreen, String[] data,
                     jsdevisec what)
    {
        super(owner, true);

        setResizable(true);

        jsc = what;

        okButton = new DEViseButton("  OK  ", jsc.jsValues);

        jsc.jsValues.debug.log("Creating RecordDlg");

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Record Attributes");

        // -1 here because first item in data is the command name.
        int attrCount = data.length - 1;

        //
        // Split strings from DEVise into attribute and value.
        //
        String[] attributes = new String[attrCount];
        String[] values = new String[attrCount];
        for (int i = 0; i < attrCount; i++) {
            //TEMP: eliminate this line when bug 1023 is fixed
            jsc.pn("DIAG data: <" + data[i+1] + ">");//TEMP
            String delimiter = ": ";
            int tempIndex = data[i+1].indexOf(delimiter);
            if (tempIndex != -1) {
                attributes[i] = new String(data[i+1].substring(0, tempIndex));
                values[i] = new String(data[i+1].substring(tempIndex +
                                       delimiter.length(), data[i+1].length()));
            } else {
                attributes[i] = new String(data[i+1]);
                values[i] = new String("");
            }
            //TEMP: eliminate this line when bug 1023 is fixed
            jsc.pn("  DIAG attribute: <" + attributes[i] + ">");//TEMP
            //TEMP: eliminate this line when bug 1023 is fixed
            jsc.pn("  DIAG value: <" + values[i] + ">");//TEMP
        }

        //
        // Figure out how many attributes to show.
        //
        int attrsToShow = 0;
        String noDDMarker = "_nodd";
        for (int i = 0; i < attrCount; i++) {
            if (!attributes[i].endsWith(noDDMarker)) {
                attrsToShow++;
            }
        }

        //
        // Generate the array of data for the JTable.
        //
        String[][] swingData = null;
        int attrNum = 0;
        if (attrsToShow > 0) {
            swingData = new String[attrsToShow][2];
            urls = new String[attrsToShow];
            for (int i = 0; i < attrCount; i++) {
                if (!attributes[i].endsWith(noDDMarker)) {

                    // Figure out if this is a URL, save the "plain" URL
                    // string if so.
                    String urlMarker = "_url";
                    if (attributes[i].endsWith(urlMarker)) {
                        // Strip off "_url" from the attribute name.
                        int suffix = attributes[i].indexOf(urlMarker);
                        swingData[attrNum][0] = attributes[i].substring(0,
                                                suffix);
                        swingData[attrNum][1] = str2Description(values[i]);
                        urls[attrNum] = str2Url(values[i]);
                    } else {
                        swingData[attrNum][0] = attributes[i];
                        swingData[attrNum][1] = values[i];
                        urls[attrNum] = null;
                    }
                    attrNum++;
                }
            }
        }

        // set layout manager
        BorderLayout layout = new BorderLayout();
        layout.setVgap(10);
        setLayout(layout);

        String[] columnNames = {"Attribute", "Value"};
        table = new JTable(swingData, columnNames);

        // Make the value column right-justified (see JS bug 981) -- making
        // *everything* justified for now, not just numbers...
        DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
        renderer.setHorizontalAlignment(JLabel.RIGHT);
        TableColumn tColumn = table.getColumnModel().getColumn(1);
        tColumn.setCellRenderer(renderer);

        table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        table.setCellSelectionEnabled(true);

        table.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent me) {
                if (table.getSelectedColumn() == 1 &&
                        urls[table.getSelectedRow()] != null) {
                    jsc.showDocument(urls[table.getSelectedRow()], true);
                }
            }
        });

        // Pack the second column of the table (make the column widths
        // fit the widths of the attributes and values).
        int margin = 2;
        for (int column = 0; column < table.getColumnCount(); column++) {
            packColumn(table, column, margin);
        }

        Dimension panesize = table.getPreferredSize();
        removeAll();

        JScrollPane pane = new JScrollPane(table);
        if(panesize.height > 600) panesize.height = 600;
        if(panesize.width > 800) panesize.width = 800;
        pane.setPreferredSize(panesize);

        add(pane, BorderLayout.CENTER);
        add(okButton, BorderLayout.SOUTH);
        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                if (jsc.drillDownCanvas != null) {
                    jsc.drillDownCanvas.eraseDrillDownMark();
                    jsc.drillDownCanvas = null;
                }
                close();
                // close the dialog in followers
                if (jsc.specialID == -1 && jsc.isCollab) {
                    try {
                        jsc.dispatcher.sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                    } catch (YException ex) {
                        System.out.println(ex.getMessage());
                    }
                }
            }
        });
        instance = this;
    }

    public static RecordDlg getInstance() {
        return instance;
    }

    public DEViseButton getOkButton() {
        return okButton;
    }

    //
    // The next two methods split up a string of the form "{description}{url}".
    // If the braces aren't there, both methods return the whole string.
    //

    private String str2Description(String value) {
        //TEMP: eliminate this line when bug 1023 is fixed
        jsc.pn("    DIAG str2Description(" + value + ")");//TEMP
        String desc = value;
        if (value.startsWith("{")) {
            int end = value.indexOf('}', 1);
            if (end > 0) {
                desc = value.substring(1, end);
            }
        }
        //TEMP: eliminate this line when bug 1023 is fixed
        jsc.pn("      DIAG desc <" + desc + ">");//TEMP
        return "<html><u>" + desc + "</u></html>";
    }

    private String str2Url(String value) {
        String url = value;
        if (value.startsWith("{")) {
            // Skip past the first {...} section (the description).
            int end = value.indexOf('}', 1);
            int beginning = value.indexOf('{', end);
            if (beginning > 0) {
                beginning++; // skip over '{'
                end = value.indexOf('}', beginning);
                if (end > 0) {
                    url = value.substring(beginning, end);
                }
            }
        }
        return url;
    }

    // Sets the preferred width of the visible column specified
    // by vColIndex. The column will be just wide enough
    // to show the column head and the widest cell in the column.
    // margin pixels are added to the left and right
    // (resulting in an additional width of 2*margin pixels).
    private void packColumn(JTable table, int vColIndex, int margin) {
        TableModel model = table.getModel();
        DefaultTableColumnModel colModel =
            (DefaultTableColumnModel)table.getColumnModel();
        TableColumn col = colModel.getColumn(vColIndex);
        int width = 0;

        // Get width of column header
        TableCellRenderer renderer = col.getHeaderRenderer();
        if (renderer == null) {
            renderer = table.getTableHeader().getDefaultRenderer();
        }
        Component comp = renderer.getTableCellRendererComponent(
                             table, col.getHeaderValue(), false, false, 0, 0);
        width = comp.getPreferredSize().width;

        // Get maximum width of column data
        for (int r=0; r<table.getRowCount(); r++) {
            renderer = table.getCellRenderer(r, vColIndex);
            comp = renderer.getTableCellRendererComponent(
                       table, table.getValueAt(r, vColIndex), false, false,
                       r, vColIndex);
            width = Math.max(width, comp.getPreferredSize().width);
        }

        // Add margin
        width += 2*margin;

        // Set the width
        col.setPreferredWidth(width);
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening RecordDlg");
        status = true;
        setVisible(true);
    }

    public void close() // Synchronization causes deadlock
    {
        if (status) {
            dispose();
            status = false;
        }
        jsc.jsValues.debug.log("Closed RecordDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog to show available session files and allow the user to select
// one.
class SessionDlg extends JFrame
{
    private jsdevisec jsc = null;

    private String sessionName = null;
    private java.awt.List fileList = null;
    private Vector fileInfo;
    private Label label = new Label("Current available sessions in directory    ");
    private Label directory = new Label("");
    private DEViseButton okButton;
    private DEViseButton cancelButton;
    private String[] sessions = null;
    private String[] sessionNames = null;

    private boolean isShowing = false; // whether this dialog is showing

    private boolean selectionMade = false;

    private class FileDirInfo {
        public boolean isDir;
        public String filename;
    }

    public SessionDlg(jsdevisec what, Frame owner, boolean isCenterScreen, String[] data)
    {
        what.jsValues.debug.log("Creating SessionDlg");

        jsc = what;

        okButton = new DEViseButton("OK", jsc.jsValues);
        cancelButton = new DEViseButton("Cancel", jsc.jsValues);

        getContentPane().setBackground(jsc.jsValues.uiglobals.bg);
        getContentPane().setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JavaScreen Open Dialog");

        label.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));
        //label.setFont(jsc.jsValues.uiglobals.font);
        directory.setText("/" + jsc.currentDir);
        directory.setFont(jsc.jsValues.uiglobals.font);

        fileList = new java.awt.List(8, false);
        fileList.setBackground(jsc.jsValues.uiglobals.textBg);
        //fileList.setBackground(Color.white);
        fileList.setForeground(jsc.jsValues.uiglobals.textFg);
        fileList.setFont(jsc.jsValues.uiglobals.textFont);

        setSessionList(data);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = okButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.bg);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.BOTH;
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        c.insets = new Insets(5, 10, 0, 10);
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(0, 10, 5, 10);
        gridbag.setConstraints(directory, c);
        add(directory);
        c.insets = new Insets(5, 10, 5, 10);
        gridbag.setConstraints(fileList, c);
        add(fileList);
        gridbag.setConstraints(panel, c);
        add(panel);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        fileList.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent event)
            {
                if (event.getClickCount() > 1) {
                    dirOrFileSelected();
                }
            }
        });

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                dirOrFileSelected();
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });
    }

    private void dirOrFileSelected()
    {
        if (!selectionMade) {
            selectionMade = true;
            int idx;
            if ((fileList.getItemCount() > 0) &&
                    ((idx = fileList.getSelectedIndex()) != -1)) {
                FileDirInfo fdi = (FileDirInfo)fileInfo.elementAt(idx);

                if (fdi.isDir) {
                    if (fdi.filename.equals("..")) { // go up a directory
                        jsc.previousDir = jsc.currentDir;
                        if (jsc.currentDir.equals(jsc.rootDir)) {
                            jsc.showMsg("You do not have access to this directory!");
                            return;
                        }

                        // Remove the last element from the currentDir string.
                        int index = jsc.currentDir.lastIndexOf('/');
                        if (index > 0) {
                            jsc.previousDir = jsc.currentDir;
                            jsc.currentDir = jsc.currentDir.substring(0, index);
                        } else {
                            jsc.showMsg("Invalid current directory \"" +
                                        jsc.currentDir + "\"!");
                            jsc.previousDir = jsc.currentDir;
                            jsc.currentDir = jsc.rootDir;
                            close();
                        }
                    } else {
                        jsc.previousDir = jsc.currentDir;
                        jsc.currentDir = jsc.currentDir + "/" + fdi.filename;
                    }

                    directory.setText("/" + jsc.currentDir);
                    validate();
                    jsc.dispatcher.start(DEViseCommands.GET_SESSION_LIST +
                                         " {" + jsc.currentDir + "}");
                } else { // a file
                    jsc.currentSession = fdi.filename;
                    jsc.openSession(jsc.currentDir + "/" + fdi.filename);
                    close();
                }
            }
        }
    }

    public void setSessionList(String[] data)
    {
        if (data == null) {
            data = new String[4];
            data[0] = new String("");
            data[1] = new String(" ");
            data[2] = new String("0");
            data[3] = new String("0");
        }

        sessions = data;

        // need to correct for num < 1
        int number = (sessions.length - 1) / 3;
        sessionNames = new String[number];
        // Whether this is a file (as opposed to a directory).
        boolean[] isDirs = new boolean[number];
        String tmpstr = null;
        for (int i = 0; i < number; i++) {
            sessionNames[i] = sessions[i * 3 + 1];
            tmpstr = sessions[i * 3 + 2];
            isDirs[i] = !tmpstr.equals("0");
        }

        fileList.removeAll();
        fileInfo = new Vector();

        for (int i = 0; i < number; i++) {
            // Workaround for DEVise/JS bug 933.
            if (sessionNames[i].endsWith("acdd")) {
                continue;
            }
            FileDirInfo fdi = new FileDirInfo();
            if (isDirs[i]) {
                fdi.isDir = true;
                fdi.filename = sessionNames[i];
                fileList.add("[" + sessionNames[i] + "]");
            } else {
                fdi.isDir = false;
                fdi.filename = sessionNames[i];
                String visType = DEViseUtils.getVisType(sessionNames[i]);
                if (!visType.equals("")) {
                    String filename = sessionNames[i];
                    sessionNames[i] = visType;
                    if (!jsc.jsValues.uiglobals._hideBmrbSessionNames) {
                        sessionNames[i] += " (" + filename + ")";
                    }
                }
                fileList.add(sessionNames[i]);
            }
            fileInfo.add(fdi);
        }

        validate();

        selectionMade = false;
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening SessionDlg");
        isShowing = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (isShowing) {
            dispose();

            isShowing = false;

            jsc.sessiondlg = null;
        }
        jsc.jsValues.debug.log("Closed SessionDlg");
    }

    public void setDirectory()
    {
        directory.setText("/" + jsc.currentDir);
        validate();
    }
}

// ------------------------------------------------------------------------

// Dialog for setting values such as screen size.
class SettingDlg extends Dialog
{
    jsdevisec jsc = null;

    private TextField screenX = new TextField(4);
    private TextField screenY = new TextField(4);
    private DEViseButton setButton;
    private DEViseButton statButton;
    private DEViseButton meButton;
    private DEViseButton collabButton;
    private DEViseButton cancelButton;
    private boolean status = false; // true means this dialog is showing

    public SettingDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating SettingDlg");

        jsc = what;

        setButton = new DEViseButton("   Set   ", jsc.jsValues);
        statButton = new DEViseButton("Request", jsc.jsValues);
        meButton = new DEViseButton("Request", jsc.jsValues);
        collabButton = new DEViseButton("Request", jsc.jsValues);
        cancelButton = new DEViseButton("Close", jsc.jsValues);

        //
        // Get the version info from the JSPoP (send the command, and
        // wait for it to finish or fail).
        //
        jsc.dispatcher.start(DEViseCommands.GET_POP_VERSION + " {0} {0}");
        while (jsc.dispatcher.getStatus() != DEViseCmdDispatcher.STATUS_IDLE) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                System.err.println("InterruptedException: " + ex);
            }
        }

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JavaScreen Settings");

        screenX.setBackground(jsc.jsValues.uiglobals.textBg);
        screenX.setForeground(jsc.jsValues.uiglobals.textFg);
        screenX.setFont(jsc.jsValues.uiglobals.textFont);

        screenY.setBackground(jsc.jsValues.uiglobals.textBg);
        screenY.setForeground(jsc.jsValues.uiglobals.textFg);
        screenY.setFont(jsc.jsValues.uiglobals.textFont);

        screenX.setText("" + jsc.jsValues.uiglobals.screenSize.width);
        screenY.setText("" + jsc.jsValues.uiglobals.screenSize.height);

        if (jsc.specialID == -1) {
            statButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            statButton.setBackground(Color.red);
        }

        if (jsc.specialID == -1) {
            collabButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            collabButton.setBackground(Color.red);
        }

        if (jsc.jsValues.uiglobals.inBrowser) {
            screenX.setEditable(false);
            screenY.setEditable(false);
            setButton.setEnabled(false);
        }

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        // JavaScreen (client) version.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label label1 = new Label("JavaScreen Version:");
        gridbag.setConstraints(label1, c);
        add(label1);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        Label version = new Label(DEViseGlobals.VERSION);
        gridbag.setConstraints(version, c);
        add(version);

        // Jmol version.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelJmol1 = new Label("Jmol Version:");
        gridbag.setConstraints(labelJmol1, c);
        add(labelJmol1);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        Label labelJmol2 = new Label(JmolConstants.version);
        gridbag.setConstraints(labelJmol2, c);
        add(labelJmol2);

        // JSPoP version.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelPopVer = new Label("JSPoP Version:");
        gridbag.setConstraints(labelPopVer, c);
        add(labelPopVer);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        Label labelPopVerValue = new Label(jsc.dispatcher.getPopVersion());
        gridbag.setConstraints(labelPopVerValue, c);
        add(labelPopVerValue);

        // DEVise version.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelDevVer = new Label("DEVise Version:");
        gridbag.setConstraints(labelDevVer, c);
        add(labelDevVer);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        Label labelDevVerValue = new Label(jsc.dispatcher.getDeviseVersion());
        gridbag.setConstraints(labelDevVerValue, c);
        add(labelDevVerValue);

        // JSPoP ID.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelPopID = new Label("JSPoP ID:");
        gridbag.setConstraints(labelPopID, c);
        add(labelPopID);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        Label labelPopIDValue = new Label(jsc.dispatcher.getPopID());
        gridbag.setConstraints(labelPopIDValue, c);
        add(labelPopIDValue);

        // Is CGI?
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelIsCgi = new Label("Is CGI:");
        gridbag.setConstraints(labelIsCgi, c);
        add(labelIsCgi);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        String isCgiValue =
            jsc.dispatcher.getIsCgi() != 0 ? "yes" : "no";
        Label labelIsCgiValue = new Label(isCgiValue);
        gridbag.setConstraints(labelIsCgiValue, c);
        add(labelIsCgiValue);

        // Is mod_perl?
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label labelIsModPerl = new Label("Is mod_perl:");
        gridbag.setConstraints(labelIsModPerl, c);
        add(labelIsModPerl);

        c.insets = new Insets(10, 0, 0, 5);
        c.gridwidth = GridBagConstraints.REMAINDER;
        String isModPerlValue =
            jsc.dispatcher.getIsModPerl() != 0 ? "yes" : "no";
        Label labelIsModPerlValue = new Label(isModPerlValue);
        gridbag.setConstraints(labelIsModPerlValue, c);
        add(labelIsModPerlValue);

        // JavaScreen size.
        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label label2 = new Label("JavaScreen Size:");
        gridbag.setConstraints(label2, c);
        add(label2);

        c.insets = new Insets(10, 0, 0, 5);
        gridbag.setConstraints(screenX, c);
        add(screenX);

        gridbag.setConstraints(screenY, c);
        add(screenY);

        c.insets = new Insets(10, 10, 0, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(setButton, c);
        add(setButton);

        // JSPoP status.
        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label3 = new Label("JSPOP Status:");
        gridbag.setConstraints(label3, c);
        add(label3);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(statButton, c);
        add(statButton);

        // My ID.
        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label5 = new Label("My ID:");
        gridbag.setConstraints(label5, c);
        add(label5);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(meButton, c);
        add(meButton);

        // Collaboration status.
        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label4 = new Label("Collaboration Status:");
        gridbag.setConstraints(label4, c);
        add(label4);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(collabButton, c);
        add(collabButton);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(cancelButton, c);
        add(cancelButton);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                int x = 0, y = 0;

                try {
                    x = Integer.parseInt(screenX.getText());
                    y = Integer.parseInt(screenY.getText());

                    if (x < jsc.jsValues.uiglobals.minScreenSize.width || x > jsc.jsValues.uiglobals.maxScreenSize.width
                            || y < jsc.jsValues.uiglobals.minScreenSize.height || y > jsc.jsValues.uiglobals.maxScreenSize.height) {
                        throw new NumberFormatException();
                    } else {
                        jsc.jscreen.setScreenDim(x, y);
                        close();
                    }
                } catch (NumberFormatException e) {
                    jsc.showMsg("Invalid screen size specified!\nJavaScreen size must be larger than ("
                                + jsc.jsValues.uiglobals.minScreenSize.width + ", " + jsc.jsValues.uiglobals.minScreenSize.height
                                + ") and smaller than ("
                                + jsc.jsValues.uiglobals.maxScreenSize.width + ", " + jsc.jsValues.uiglobals.maxScreenSize.height + ")");
                }
            }
        });
        if (jsc.specialID == -1) {
            statButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    jsc.dispatcher.start(DEViseCommands.GET_SERVER_STATE);

                    close();
                }
            });
        }

        if (jsc.specialID == -1) {
            collabButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    jsc.dispatcher.start(DEViseCommands.GET_COLLAB_LIST);
                    close();
                }
            });
        }

        meButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
                jsc.displayID();
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });

    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening SettingDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed SettingDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for displaying server state.
class ServerStateDlg extends Dialog
{
    public jsdevisec jsc = null;

    private java.awt.List serverList = null, activeClientList = null, suspendClientList = null;
    private Label label1 = new Label("Current active server:");
    private Label label2 = new Label("Current active client:");
    private Label label3 = new Label("Current suspended client:");
    private DEViseButton okButton;

    private boolean status = false; // true means this dialog is showing

    // For the format of the data string, see jspop.getServerState().
    public ServerStateDlg(Frame owner, boolean isCenterScreen, String data, jsdevisec what)
    {
        super(owner, true);

        jsc = what;

        okButton = new DEViseButton("   OK   ", jsc.jsValues);

        jsc.jsValues.debug.log("Creating ServerStateDlg");

        String[] serverInfo = null, activeClientInfo = null,
                 suspClientInfo = null;

        String[] list = DEViseGlobals.parseString(data);
        int listIndex = 0;

        if (list != null && list.length > 3) {
            try {
                int serverCount = Integer.parseInt(list[listIndex++]);
                if (serverCount != 0) {
                    serverInfo = new String[serverCount];
                    for (int i = 0; i < serverCount; i++) {
                        String[] subList = DEViseGlobals.parseStr(
                                               list[listIndex++], " ");
                        serverInfo[i] = subList[0] + " " + subList[1];
                    }
                }

                listIndex++; // skip total clients value
                int activeClientCount = Integer.parseInt(list[listIndex++]);
                if (activeClientCount != 0) {
                    activeClientInfo = new String[activeClientCount];
                    for (int i = 0; i < activeClientCount; i++) {
                        String[] subList = DEViseGlobals.parseStr(
                                               list[listIndex++], " ");
                        activeClientInfo[i] = subList[0] + " " + subList[1];
                    }
                }

                int suspClientCount = Integer.parseInt(list[listIndex++]);
                if (suspClientCount != 0) {
                    suspClientInfo = new String[suspClientCount];
                    for (int i = 0; i < suspClientCount; i++) {
                        String[] subList = DEViseGlobals.parseStr(
                                               list[listIndex++], " ");
                        suspClientInfo[i] = subList[0] + " " + subList[1];
                    }
                }
            } catch (NumberFormatException e) {
            }
        }

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JSPOP current state");

        label1.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));
        label2.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));
        label3.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));

        serverList = new java.awt.List(4, false);
        serverList.setBackground(jsc.jsValues.uiglobals.textBg);
        serverList.setForeground(jsc.jsValues.uiglobals.textFg);
        serverList.setFont(jsc.jsValues.uiglobals.textFont);
        if (serverInfo != null) {
            for (int i = 0; i < serverInfo.length; i++) {
                serverList.add(serverInfo[i]);
            }
        }
        activeClientList = new java.awt.List(6, false);
        activeClientList.setBackground(jsc.jsValues.uiglobals.textBg);
        activeClientList.setForeground(jsc.jsValues.uiglobals.textFg);
        activeClientList.setFont(jsc.jsValues.uiglobals.textFont);
        if (activeClientInfo != null) {
            for (int i = 0; i < activeClientInfo.length; i++) {
                activeClientList.add(activeClientInfo[i]);
            }
        }
        suspendClientList = new java.awt.List(6, false);
        suspendClientList.setBackground(jsc.jsValues.uiglobals.textBg);
        suspendClientList.setForeground(jsc.jsValues.uiglobals.textFg);
        suspendClientList.setFont(jsc.jsValues.uiglobals.textFont);
        if (suspClientInfo != null) {
            for (int i = 0; i < suspClientInfo.length; i++) {
                suspendClientList.add(suspClientInfo[i]);
            }
        }

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.BOTH;
        c.insets = new Insets(5, 10, 5, 10);
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        gridbag.setConstraints(label1, c);
        add(label1);
        gridbag.setConstraints(serverList, c);
        add(serverList);
        gridbag.setConstraints(label2, c);
        add(label2);
        gridbag.setConstraints(activeClientList, c);
        add(activeClientList);
        gridbag.setConstraints(label3, c);
        add(label3);
        gridbag.setConstraints(suspendClientList, c);
        add(suspendClientList);

        gridbag.setConstraints(okButton, c);
        add(okButton);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();

                // close the dialog in followers
                if (jsc.specialID == -1 && jsc.isCollab) {
                    try {
                        jsc.dispatcher.sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                    } catch (YException ex) {
                        System.out.println(ex.getMessage());
                    }
                }
            }
        });

    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening ServerStateDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed ServerStateDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting cgi URL.
class SetCgiUrlDlg extends Dialog
{
    jsdevisec jsc = null;
    private TextField url = new TextField(20);
    private DEViseButton setButton;
    private DEViseButton cancelButton;
    private boolean status = false; // true means this dialog is showing

    public SetCgiUrlDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating SetCgiUrlDlg");

        jsc = what;

        setButton = new DEViseButton("   Set   ", jsc.jsValues);
        cancelButton = new DEViseButton("  Cancel ", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting CGI URL");

        url.setBackground(jsc.jsValues.uiglobals.textBg);
        url.setForeground(jsc.jsValues.uiglobals.textFg);
        url.setFont(jsc.jsValues.uiglobals.textFont);

        url.setText(jsc.jsValues.connection.cgiURL);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        c.gridwidth = 1;
        Label label = new Label("CGI URL:");
        gridbag.setConstraints(label, c);
        add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(url, c);
        add(url);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.bg);

        gridbag.setConstraints(panel, c);
        add(panel);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                jsc.jsValues.connection.useCgi = true;
                jsc.cgiMode();
                jsc.jsValues.connection.cgiURL = url.getText();
                close();
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening SetCgiUrlDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed SetCgiUrlDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting logfile name for playback.
class SetLogFileDlg extends Dialog
{
    private jsdevisec jsc = null;
    private TextField file = new TextField(30);
    private Checkbox display = new Checkbox("Display", true);
    private Checkbox original = new Checkbox("Original Rate", true);
    private DEViseButton setButton;
    private DEViseButton cancelButton;
    private boolean status = false; // true means this dialog is showing

    public SetLogFileDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating SetLogFileDlg");

        jsc = what;

        setButton = new DEViseButton("   Play   ", jsc.jsValues);
        cancelButton = new DEViseButton("  Cancel ", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting Logfile Name");

        file.setBackground(jsc.jsValues.uiglobals.textBg);
        file.setForeground(jsc.jsValues.uiglobals.textFg);
        file.setFont(jsc.jsValues.uiglobals.textFont);

        display.setBackground(jsc.jsValues.uiglobals.textBg);
        display.setForeground(jsc.jsValues.uiglobals.textFg);
        display.setFont(jsc.jsValues.uiglobals.textFont);

        original.setBackground(jsc.jsValues.uiglobals.textBg);
        original.setForeground(jsc.jsValues.uiglobals.textFg);
        original.setFont(jsc.jsValues.uiglobals.textFont);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        c.gridwidth = 1;
        Label label = new Label("Logfile URL:");
        gridbag.setConstraints(label, c);
        add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(file, c);
        add(file);

        Checkbox [] checkbox = new Checkbox[2];
        checkbox[0] = display;
        checkbox[1] = original;
        DEViseComponentPanel panel1 = new DEViseComponentPanel(checkbox,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel1.setBackground(jsc.jsValues.uiglobals.bg);

        gridbag.setConstraints(panel1, c);
        add(panel1);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel2 = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel2.setBackground(jsc.jsValues.uiglobals.bg);

        gridbag.setConstraints(panel2, c);
        add(panel2);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                jsc.logFileName = file.getText();

                File file = new File(jsc.logFileName);
                if(!file.exists()) {
                    jsc.showMsg("File not found: " + jsc.logFileName);
                }
                else {
                    jsc.isDisplay = display.getState();
                    jsc.isOriginal = original.getState();
                    close();
                    jsc.logPlayBack();
                }
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    //TEMP -- do we need this?
    public void open()
    {
        jsc.jsValues.debug.log("Opening SetLogFileDlg");
        status = true;
        setVisible(true);
    }

    //TEMP -- do we need this?
    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed SetLogFileDlg");
    }
}

// ------------------------------------------------------------------------

// Dialog to select collaboration state.
class CollabSelectDlg extends Dialog
{
    jsdevisec jsc = null;

    private DEViseButton collabButton;
    private DEViseButton endButton;
    private DEViseButton enCollabButton;
    private DEViseButton disCollabButton;
    private DEViseButton cancelButton;
    private boolean status = false; // true means this dialog is showing

    public CollabSelectDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating CollabSelectDlg");

        jsc = what;

        collabButton = new DEViseButton("Become Follower", jsc.jsValues);
        endButton = new DEViseButton("Quit Following", jsc.jsValues);
        enCollabButton = new DEViseButton("Become Leader", jsc.jsValues);
        disCollabButton = new DEViseButton("Quit Leading", jsc.jsValues);
        cancelButton = new DEViseButton("Cancel", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JavaScreen Collaboration");

        if ((jsc.specialID == -1) && (!jsc.isCollab)) {
            collabButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            collabButton.setBackground(Color.red);
        }

        if (jsc.specialID != -1) {
            endButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            endButton.setBackground(Color.red);
        }

        if ((jsc.specialID == -1) && (!jsc.isCollab)) {
            enCollabButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            enCollabButton.setBackground(Color.red);
        }

        if ((jsc.specialID == -1) && (jsc.isCollab)) {
            disCollabButton.setBackground(jsc.jsValues.uiglobals.bg);
        } else {
            disCollabButton.setBackground(Color.red);
        }

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        Label label = new Label("Collaboration Setting:");
        gridbag.setConstraints(label, c);
        add(label);

        gridbag.setConstraints(collabButton, c);
        add(collabButton);
        gridbag.setConstraints(endButton, c);
        add(endButton);
        gridbag.setConstraints(enCollabButton, c);
        add(enCollabButton);
        gridbag.setConstraints(disCollabButton, c);
        add(disCollabButton);
        gridbag.setConstraints(cancelButton, c);
        add(cancelButton);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        if ((jsc.specialID == -1) && (!jsc.isCollab)) {
            collabButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    String command = new String();

                    // if switch from "socket" mode, save current
                    // session.
                    // TEMP: this should really be taken care of in
                    // the JSPoP, so we don't save and close the session
                    // unless we have actually succeeded in getting
                    // into collaboration mode.  RKW 2001-11-12.
                    // (If we start collaboration and then cancel,
                    // we've lost our pre-collaboration session.)
                    if (jsc.isSessionOpened) {
                        jsc.jscreen.updateScreen(false);
                        jsc.sessionSaved = true;
                        command = DEViseCommands.SAVE_CUR_SESSION + "\n";
                    }

                    // if already in "collaboration" mode,
                    // send JAVAC_CollabExit to exit from previous collaboration
                    if (jsc.specialID != -1) {
                        // Note: the GUI doesn't seem to let you get to
                        // here, so I'm not sure whether this works.
                        // RKW 2001-11-13.
                        if (!jsc.dispatcher.dispatcherThread.isInterrupted()) {
                            jsc.collabinterrupted = true;
                            jsc.dispatcher.dispatcherThread.interrupt();
                        }
                        jsc.specialID = -1;
                    }

                    command = command + DEViseCommands.ASK_COLLAB_LEADER;
                    jsc.dispatcher.start(command);

                    close();
                }
            });
        }

        if (jsc.specialID != -1) {
            endButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    jsc.socketMode();
                    jsc.specialID = -1;
                    jsc.collabinterrupted = true;
                    jsc.dispatcher.dispatcherThread.interrupt();

                    jsc.animPanel.stop();
                    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
                    jsc.jscreen.updateScreen(false);
                    jsc.dispatcher.clearStatus();

                    jsc.restorePreCollab();

                    close();
                }
            });
        }

        if ((jsc.specialID == -1) && (!jsc.isCollab)) {
            enCollabButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    close();
                    jsc.showCollabPass();
                }
            });
        }

        if ((jsc.specialID == -1) && (jsc.isCollab)) {
            disCollabButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    close();
                    jsc.collabModeUnlead(true);
                }
            });
        }

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });

    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabSelectDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed CollabSelectDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------
// Dialog for setting collabrated JavaScreen ID.
class CollabIdDlg extends JFrame
{
    private jsdevisec jsc = null;

    private java.awt.List clientList = null;
    private Label label = new Label("Current active clients: ");
    private DEViseButton okButton;
    private DEViseButton cancelButton;
    private String[] clients = null;
    private boolean emptyList = false;

    private boolean status = false; // true means this dialog is showing

    public CollabIdDlg(jsdevisec what, Frame owner, boolean isCenterScreen,
                       String data) throws YException
    {
        what.jsValues.debug.log("Creating CollabIdDlg");

        jsc = what;

        okButton = new DEViseButton("OK", jsc.jsValues);
        cancelButton = new DEViseButton("Cancel", jsc.jsValues);

        getContentPane().setBackground(jsc.jsValues.uiglobals.bg);
        getContentPane().setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Collaboration JavaScreen Dialog");

        label.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));

        clientList = new java.awt.List(8, false);
        clientList.setBackground(jsc.jsValues.uiglobals.textBg);
        clientList.setForeground(jsc.jsValues.uiglobals.textFg);
        clientList.setFont(jsc.jsValues.uiglobals.textFont);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = okButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.bg);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        c.insets = new Insets(5, 10, 0, 10);
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 10, 5, 10);
        gridbag.setConstraints(clientList, c);
        add(clientList);
        gridbag.setConstraints(panel, c);
        add(panel);

        setClientList(data);
        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        clientList.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent event)
            {
                if (event.getClickCount() > 1) {
                    startCollab();
                }
            }
        });

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                startCollab();
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
                jsc.dispatcher.clearStatus();
                jsc.restorePreCollab();
            }
        });

    }

    public void setClientList(String data) throws YException
    {
        clients = DEViseGlobals.parseString(data);

        //clientList.removeAll();

        for (int i = 1; i <= (clients.length-1)/4; i++) {
            String list = new String();
            list = clients[(i-1)*4+1] + clients[(i-1)*4+2]
                   + clients[(i-1)*4+3] + clients[(i-1)*4+4];
            clientList.add(list);
        }

        if (clientList.getItemCount() <= 0) {
            clientList.add("No available collaboration leader.");
            emptyList = true;
        } else {
            validate();
        }
    }

    private void startCollab() {
        if (!emptyList) {
            int idx = clientList.getSelectedIndex();
            if (idx != -1) {
                String list = clientList.getItem(idx);
                int n = list.indexOf(' ');
                String clientID = list.substring(0,n);
                jsc.specialID = Integer.parseInt(clientID);

                close();
                jsc.enterCollabPass();
            }
        } else { // no available collaboration leader
            // go back to normal mode
            jsc.collabQuit();
            jsc.restorePreCollab();
            close();
        }
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabIdDlg");
        status = true;
        jsc.isCollabIdDlgOpened = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();
            status = false;
            jsc.isCollabIdDlgOpened = false;
            jsc.collabIdDlg = null;
        }
        jsc.jsValues.debug.log("Closed CollabIdDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting password for collaboration (the *leader* does this).
class CollabPassDlg extends Dialog
{
    private jsdevisec jsc = null;

    private TextField pass = new TextField(20);
    private TextField name = new TextField(20);
    private DEViseButton setButton;
    private DEViseButton cancelButton;
    private boolean status = false; // true means this dialog is showing

    public CollabPassDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating CollabPassDlg");

        jsc = what;

        setButton = new DEViseButton("Set", jsc.jsValues);
        cancelButton = new DEViseButton("Cancel", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting Collaboration Name & Password");

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        pass.setText(jsc.jsValues.session.collabLeaderPass);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.bg);

        c.gridwidth = 1;
        Label label1 = new Label("Collaboration Name:");
        gridbag.setConstraints(label1, c);
        add(label1);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(name, c);
        add(name);

        c.gridwidth = 1;
        Label label2 = new Label("Collaboration Passwd:");
        gridbag.setConstraints(label2, c);
        add(label2);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(pass, c);
        add(pass);

        gridbag.setConstraints(panel, c);
        add(panel);


        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                jsc.isCollab = true;
                jsc.collabModeL();

                jsc.jsValues.session.collabLeaderPass =
                    pass.getText();
                jsc.jsValues.session.collabLeaderName =
                    name.getText();
                String command = new String();
                command = DEViseCommands.SET_COLLAB_PASS +
                          " {" + jsc.jsValues.session.collabLeaderName + "}" +
                          " {" + jsc.jsValues.session.collabLeaderPass + "}";
                close();
                jsc.dispatcher.start(command);
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });

    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabPassDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed CollabPassDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting password for collaboration (the *follower* does this).
class EnterCollabPassDlg extends Dialog
{
    private jsdevisec jsc = null;

    private TextField pass = new TextField(20);
    private DEViseButton setButton;
    //public DEViseButton cancelButton = new DEViseButton("Cancel", jsValues);
    private boolean status = false; // true means this dialog is showing

    public EnterCollabPassDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating EnterCollabPassDlg");

        jsc = what;

        setButton = new DEViseButton("Set", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Reading Collaboration Password");

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        pass.setText(DEViseGlobals.DEFAULT_COLLAB_PASS);

        DEViseButton [] button = new DEViseButton[1];
        button[0] = setButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);


        c.gridwidth = 1;
        Label label = new Label("Enter Collaboration Password:");
        gridbag.setConstraints(label, c);
        add(label);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(pass, c);
        add(pass);
        gridbag.setConstraints(panel, c);
        add(panel);


        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                jsc.collabPass = pass.getText();

                String id = new Integer(jsc.specialID).toString();
                String flag = new Integer(1).toString();
                String command = DEViseCommands.COLLABORATE +
                                 " {" +  flag + "} {" + id + "} {} {" +
                                 jsc.collabPass + "}";

                jsc.dispatcher.start(command);
                close();
            }
        });
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabPassDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed CollabPassDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------
// Dialog for displaying collaborating state.
class CollabStateDlg extends Dialog
{
    private jsdevisec jsc = null;

    private java.awt.List collabList = null;
    private Label label = new Label("Current collaborating followers: ");
    private DEViseButton closeButton;
    private String[] followers = null;

    private boolean status = false; // true means this dialog is showing

    public CollabStateDlg(jsdevisec what, Frame owner, boolean isCenterScreen, String data)
    {
        super(owner, true);
        what.jsValues.debug.log("Creating CollabStateDlg");

        jsc = what;

        closeButton = new DEViseButton("Close", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Collaboration State Dialog");

        label.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));

        collabList = new java.awt.List(8, false);
        collabList.setBackground(jsc.jsValues.uiglobals.textBg);
        collabList.setForeground(jsc.jsValues.uiglobals.textFg);
        collabList.setFont(jsc.jsValues.uiglobals.textFont);

        DEViseButton [] button = new DEViseButton[1];
        button[0] = closeButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        c.insets = new Insets(5, 10, 0, 10);
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 10, 5, 10);
        gridbag.setConstraints(collabList, c);
        add(collabList);
        gridbag.setConstraints(panel, c);
        add(panel);

        pack();

        setCollabList(data);

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        closeButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
                // close the dialog in followers
                if (jsc.specialID == -1 && jsc.isCollab) {
                    try {
                        jsc.dispatcher.sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                    } catch (YException ex) {
                        System.out.println(ex.getMessage());
                    }
                }
            }
        });
    }

    public void setCollabList(String data)
    {
        followers = DEViseGlobals.parseString(data);

        for (int i = 1; i < followers.length; i++) {
            collabList.add(" " + i + ":" + " " + followers[i]);
        }

        validate();
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabStateDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();
            status = false;
        }
        jsc.jsValues.debug.log("Closed CollabStateDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------
// Dialog to confirm that users really want to generate edited dynamics
// movies.  Note: this should eventually get extended to allow the user
// to modify the movie parameters.
class DynMovieDlg extends Dialog
{
    private jsdevisec jsc = null;

    private Label msg;
    private DEViseButton okButton;
    private DEViseButton cancelButton;

    private boolean status = false; // true means this dialog is showing
    public boolean isOk = false;

    public DynMovieDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating DynMovieDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Create Edited Dynamics Movie");

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        msg = new Label(
            "Creating edited dynamics movie -- this may take several minutes.");

        c.gridwidth = 1;
        c.gridx = 0;
        c.gridx = 0;
        c.ipadx = 20;
        c.ipady = 20;
        gridbag.setConstraints(msg, c);
        add(msg);

        okButton = new DEViseButton("OK", jsc.jsValues);
        cancelButton = new DEViseButton("Cancel", jsc.jsValues);

        DEViseButton [] button = new DEViseButton[2];
        button[0] = okButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.bg);

        c.gridx = 0;
        c.gridy = 1;
        c.ipadx = 10;
        c.ipady = 10;
        gridbag.setConstraints(panel, c);
        add(panel);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                isOk = true;
                close();
            }
        });

        cancelButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });

    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening CollabPassDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed DynMovieDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------
// Dialog for showing URL that browser should display
class ShowUrlDlg extends Dialog
{
    private jsdevisec jsc = null;
    private Label msg1 = new Label("Your browser should show the document " +
                                   "listed below in a");
    private Label msg2 = new Label("new window or tab; if not, please " +
                                   "paste the URL into your browser.");
    private TextField urlText = new TextField(50);
    private DEViseButton okButton;
    private boolean status = false; // true means this dialog is showing

    public ShowUrlDlg(String url, jsdevisec what, Frame owner,
                      boolean isCenterScreen)
    {
        super(owner, true);

        what.jsValues.debug.log("Creating ShowUrlDlg");

        jsc = what;

        okButton = new DEViseButton("   OK   ", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Showing Web Document");

        urlText.setBackground(jsc.jsValues.uiglobals.textBg);
        urlText.setForeground(jsc.jsValues.uiglobals.textFg);
        urlText.setFont(jsc.jsValues.uiglobals.textFont);
        urlText.setText(url);
        urlText.setEditable(false);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.fill = GridBagConstraints.HORIZONTAL;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(5, 5, 0, 5);
        c.ipadx = 0;
        c.ipady = 0;
        c.gridwidth = 1;

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridx = 0;
        c.gridy = 0;
        msg1.setAlignment(Label.CENTER);
        gridbag.setConstraints(msg1, c);
        add(msg1);

        c.insets = new Insets(0, 5, 5, 5);
        c.gridy = 1;
        msg2.setAlignment(Label.CENTER);
        gridbag.setConstraints(msg2, c);
        add(msg2);

        Label label = new Label("URL:");
        c.insets = new Insets(10, 5, 5, 5);
        c.gridwidth = 1;
        c.gridx = 0;
        c.gridy = 2;
        gridbag.setConstraints(label, c);
        add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridx = 1;
        c.gridy = 2;
        gridbag.setConstraints(urlText, c);
        add(urlText);

        DEViseButton [] button = new DEViseButton[1];
        button[0] = okButton;
        DEViseComponentPanel panel2 = new DEViseComponentPanel(button,
                DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel2.setBackground(jsc.jsValues.uiglobals.bg);

        c.gridx = 0;
        c.gridy = 3;
        gridbag.setConstraints(panel2, c);
        add(panel2);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        okButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent event)
            {
                close();
            }
        });
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
        jsc.jsValues.debug.log("Opening ShowUrlDlg");
        status = true;
        setVisible(true);
        toFront();
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
        jsc.jsValues.debug.log("Closed ShowUrlDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

/*TEMP
// ------------------------------------------------------------------------
// Dialog for showing information about the JavaScreen.
class AboutDlg extends Dialog
{
    private jsdevisec jsc = null;
    private DEViseButton closeButton;
    private boolean status = false; // true means this dialog is showing

    public AboutDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        jsc = what;
    jsc.jsValues.debug.log("Creating AboutDlg");

        closeButton = new DEViseButton("  Close  ", jsc.jsValues);

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("About DEVise JavaScreen");

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);

        c.fill = GridBagConstraints.BOTH;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.insets = new Insets(10, 10, 10, 10);

        DEViseButton [] button = new DEViseButton[1];
        button[0] = closeButton;
        DEViseComponentPanel panel2 = new DEViseComponentPanel(button,
      DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);
        panel2.setBackground(jsc.jsValues.uiglobals.bg);

        gridbag.setConstraints(panel2, c);
        add(panel2);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

    //TEMP -- do we need this?
        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
            close();
                    }
                });
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    public void open()
    {
    jsc.jsValues.debug.log("Opening AboutDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
    jsc.jsValues.debug.log("Closed AboutDlg");
    }
}
TEMP*/
