// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
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

// $Id$

// $Log$
// Revision 1.61  2000/05/11 20:59:50  wenger
// More comments.
//
// Revision 1.60  2000/05/11 20:19:34  wenger
// Cleaned up jsdevisec.java and added comments; eliminated
// jsdevisec.lastCursor (not really needed).
//
// Revision 1.59  2000/05/04 15:53:32  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.58  2000/04/27 20:15:25  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.57  2000/04/24 20:22:02  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.56  2000/03/23 16:26:18  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.55  2000/02/23 10:44:02  hongyu
// *** empty log message ***
//
// Revision 1.54  2000/01/19 20:41:12  hongyu
// *** empty log message ***
//
// Revision 1.53  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.52  1999/10/10 09:51:01  hongyu
// *** empty log message ***
//
// Revision 1.51  1999/10/10 08:49:54  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.50  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.49  1999/07/27 17:11:19  hongyu
// *** empty log message ***
//
// Revision 1.47  1999/06/23 20:59:21  wenger
// Added standard DEVise header.
//

// ========================================================================

// jsdevisec.java

import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;

public class jsdevisec extends Panel
{
    private YLogGUI debugWindow = null;
    private int debugLevel = 0;

    public DEViseCmdDispatcher dispatcher = null;

    public Frame parentFrame = null;
    private boolean isCenterScreen = false;
    //public Applet parentApplet = null;

    public DEViseScreen jscreen = null;

    private Button openButton = new Button("Open");
    private Button closeButton = new Button("Close");
    public  Button stopButton = new Button("Stop");
    private Button restartButton = new Button("Restart");
    //private Button statButton = new Button("Stats");
    private Button setButton = new Button("Option");
    private Button exitButton = new Button("Exit");
    private Button filterButton = new Button("Filter");
    private Button helpButton = new Button("Help");

    public DEViseAnimPanel animPanel = null;
    public DEViseViewInfo viewInfo = null;
    public DEViseTrafficLight light = null;

	// Box for displaying messages -- apparently kept as an object member
	// so we can know whether we're displaying a message.
    private YMsgBox msgbox = null;

    public SessionDlg sessiondlg = null;
    private RecordDlg recorddlg = null;
    private ServerStateDlg statedlg = null;
    private SettingDlg settingdlg = null;

    public boolean isSessionOpened = false;
    private boolean isQuit = false;
    public int stopNumber = 0;

    public boolean isShowProgramInfo = false;
    public String rootDir = "DEViseSession";
    public String currentDir = "DEViseSession";
    public String currentSession = null;


	// images[0-9] are the gears; 10 and 11 are "traffic lights"
	//   (devise[0-10].gif).
	// sessionName is non-null only in jsb.
    public jsdevisec(Frame frame, Vector images, int dbgLvl, String sessionName)
    {
        // frame might be null if JavaScreen is running inside a browser
        parentFrame = frame;
        if (parentFrame == null) {
            parentFrame = new Frame();
            isCenterScreen = true;
        }

        debugLevel = dbgLvl;
        if (debugLevel > 0) {
            debugWindow = new YLogGUI(debugLevel);
        }

        // determine the font size according to JavaScreen size
        int width = DEViseUIGlobals.maxScreenSize.width;
        int height = DEViseUIGlobals.maxScreenSize.height;

        if (width > 800 ) {
            DEViseUIGlobals.font = new Font("Serif", Font.PLAIN, 12);
            DEViseUIGlobals.textFont = new Font("Serif", Font.PLAIN, 12);
        } else if (width > 640) {
            DEViseUIGlobals.font = new Font("Serif", Font.PLAIN, 10);
            DEViseUIGlobals.textFont = new Font("Serif", Font.PLAIN, 10);
        } else {
            DEViseUIGlobals.font = new Font("Serif", Font.PLAIN, 8);
            DEViseUIGlobals.textFont = new Font("Serif", Font.PLAIN, 8);
        }

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);
        setLayout(new BorderLayout(2, 2));

        Panel topPanel = new Panel(new BorderLayout(2, 2));
        Panel mainPanel = new Panel(new FlowLayout(FlowLayout.LEFT, 2, 2));

        animPanel = new DEViseAnimPanel(this, images, 100);

        mainPanel.add(animPanel);

		// Note: we're just relying on the file names of the images,
		// and the code that reads them in, to assume that image 9
		// and image 10 are what we need for the traffic light.
        if (images != null && images.size() == 11) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0");
            } catch (YException e) {
                light = null;
            }
        }

        if (light != null) {
            mainPanel.add(light);
        }

		//
		// Add buttons to the panel (only add a subset if we're running
		// in a browser).  Note that we have created all of the buttons
		// even if only some of them will get used.
		//
        Component[] button = null;
        if (DEViseUIGlobals.inBrowser) {
            button = new Component[3];
            button[0] = restartButton;
            button[1] = stopButton;
            button[2] = helpButton;
        } else {
            button = new Component[8];
            button[0] = openButton;
            button[1] = closeButton;
            button[2] = stopButton;
            button[3] = restartButton;
            button[4] = setButton;
            button[5] = filterButton;
            button[6] = helpButton;
            button[7] = exitButton;
        }

        DEViseComponentPanel buttonPanel = new DEViseComponentPanel(button, "Horizontal", 5, 1);

        mainPanel.add(buttonPanel);

        viewInfo = new DEViseViewInfo(this, images);

        topPanel.add(mainPanel, BorderLayout.WEST);
        topPanel.add(viewInfo, BorderLayout.EAST);

        if (DEViseUIGlobals.inBrowser) {
            topPanel.setFont(new Font("Serif", Font.PLAIN, 14));
            topPanel.add(new Label("                       " + DEViseUIGlobals.javaScreenTitle), BorderLayout.CENTER);
        }

		//
		// Constrain the screen width to be within the min and max width,
		// if they are valid; width is set to max if not already set.
		//
        if (DEViseUIGlobals.screenSize.width <= 0) {

            DEViseUIGlobals.screenSize.width =
			  DEViseUIGlobals.maxScreenSize.width;

        } else if (DEViseUIGlobals.screenSize.width <
		  DEViseUIGlobals.minScreenSize.width &&
		  DEViseUIGlobals.screenSize.width > 0) {

            DEViseUIGlobals.screenSize.width =
			  DEViseUIGlobals.minScreenSize.width;

        } else if (DEViseUIGlobals.screenSize.width >
		  DEViseUIGlobals.maxScreenSize.width) {

            DEViseUIGlobals.screenSize.width =
			  DEViseUIGlobals.maxScreenSize.width;

        }

		//
		// Constrain the screen height to be within the min and max height,
		// if they are valid; height is set to max if not already set.
		//
        if (DEViseUIGlobals.screenSize.height <= 0) {

            DEViseUIGlobals.screenSize.height =
			  DEViseUIGlobals.maxScreenSize.height;

        } else if (DEViseUIGlobals.screenSize.height <
		  DEViseUIGlobals.minScreenSize.height &&
		  DEViseUIGlobals.screenSize.height > 0) {

            DEViseUIGlobals.screenSize.height =
			  DEViseUIGlobals.minScreenSize.height;

        } else if (DEViseUIGlobals.screenSize.height >
		  DEViseUIGlobals.maxScreenSize.height) {

            DEViseUIGlobals.screenSize.height =
			  DEViseUIGlobals.maxScreenSize.height;

        }

        jscreen = new DEViseScreen(this);
        Panel screenPanel = new Panel(new FlowLayout(FlowLayout.CENTER, 3, 3));
        screenPanel.setBackground(DEViseUIGlobals.screenBg);
        screenPanel.add(jscreen);

        add(topPanel, BorderLayout.NORTH);
        add(screenPanel, BorderLayout.CENTER);

		//
        // Define event handlers for buttons.
		//
        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            showMsg("You already have a session opened!\nPlease close current session first!");
                            return;
                        }

                        dispatcher.start(DEViseCommands.GET_SESSION_LIST + " {" + currentDir + "}");
                    }
                });

        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!isSessionOpened) {
                            showMsg("You do not have any opened session!");
                            return;
                        }

                        dispatcher.start(DEViseCommands.CLOSE_SESSION);
                    }
                });

        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() != 0) {
                            stopNumber++;
                            if (stopNumber > 1) {
                                dispatcher.stop(true, false);
                            } else {
                                dispatcher.stop();
                            }
                        }
                    }
                });

        restartButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!isSessionOpened) {
                            showMsg("You do not have any opened session!");
                            return;
                        }

                        dispatcher.start(DEViseCommands.OPEN_SESSION + " {" + currentDir + "/" + currentSession + "}");
                    }
                });

        setButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        showSetting();
                    }
                });

        exitButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        quit();
                    }
                });

        filterButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!isSessionOpened) {
                            showMsg("You do not have any opened session!");
                            return;
                        }

                        dispatcher.start(DEViseCommands.RESET_FILTERS);
                    }
                });

        helpButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!isSessionOpened) {
                            showMsg("You do not have any opened session!");
                            return;
                        }

                        jscreen.showAllHelp();
                    }
                });


        isSessionOpened = false;

		//
		// Create the one DEViseCmdDispatcher instance we will have.
		//
        dispatcher = new DEViseCmdDispatcher(this);

		//
		// Open the session if a session name was specified.
		//
        if (sessionName != null) {
            int index = sessionName.lastIndexOf('/');
            if (index > 0) {
                currentDir = currentDir + "/" + sessionName.substring(0, index);
                currentSession = sessionName.substring(index + 1, sessionName.length());
            } else {
                currentSession = sessionName;
            }

            dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + DEViseUIGlobals.screenSize.width + " " + DEViseUIGlobals.screenSize.height + "\n" + DEViseCommands.OPEN_SESSION + " {" + currentDir + "/" + currentSession + "}");
        }
    } // end of constructor

    // print out message to debug window
    // pn() methods add a newline to the end of the string; p() methods don't.
    public void pn(String msg, int level)
    {
        if (debugLevel > 0) {
            debugWindow.pn(msg, level);
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (debugLevel > 0) {
            debugWindow.p(msg, level);
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    // show message in message box
    public String showMsg(String msg, String title, int style)
    {
	// Note: we have two cases here to make things work in the case
	// where we have more than one message box shown at once.  msgbox
	// is a class member so we know whether we have at least one
	// message box showing.
        if (msgbox == null) {
            msgbox = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, DEViseUIGlobals.font, DEViseUIGlobals.bg, DEViseUIGlobals.fg);
            msgbox.open();
            String result = msgbox.getResult();
            msgbox = null;
            return result;
        } else {
	    // We only get here if we already have one message box showing,
	    // and need to show another one.
            YMsgBox box = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, DEViseUIGlobals.font, DEViseUIGlobals.bg, DEViseUIGlobals.fg);
            box.open();
            String result = box.getResult();
            box = null;
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
        if (flag) {
            if (sessiondlg != null) {
                sessiondlg.setSessionList(msg);
            } else {
                sessiondlg = new SessionDlg(this, parentFrame, isCenterScreen, msg);
                sessiondlg.open();
            }
        } else {
            showMsg(msg[0]);
        }
    }

    public void showRecord(String[] msg)
    {
        recorddlg = new RecordDlg(parentFrame, isCenterScreen, msg);
        recorddlg.open();
        recorddlg = null;
    }

    public void showServerState(String msg)
    {
        statedlg = new ServerStateDlg(parentFrame, isCenterScreen, msg);
        statedlg.open();
        statedlg = null;
    }

    public void showSetting()
    {
        settingdlg = new SettingDlg(this, parentFrame, isCenterScreen);
        settingdlg.open();
        settingdlg = null;
    }

    public boolean isShowingMsg()
    {
        if (sessiondlg != null || settingdlg != null || statedlg != null || recorddlg != null || msgbox != null) {
            return true;
        } else {
            return false;
        }
    }

    public synchronized boolean getQuitStatus()
    {
        return isQuit;
    }

    public synchronized void quit()
    {
        if (isQuit)
            return;

        if (dispatcher != null) {
            dispatcher.stop(true, true);
        }

        isQuit = true;
        dispatcher = null;

        if (!DEViseUIGlobals.inBrowser) {
            if (DEViseUIGlobals.isApplet)
                parentFrame.dispose();
            else
                System.exit(0);
        }
    }
}

// ------------------------------------------------------------------------

// Dialog to show record values.
class RecordDlg extends Dialog
{
    String[] attrs = null;
    Button okButton = new Button("  OK  ");
    private boolean status = false; // true means this dialog is showing

    public RecordDlg(Frame owner, boolean isCenterScreen, String[] data)
    {
        super(owner, true);

        attrs = data;

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);

        setTitle("Record Attributes");

        okButton.setBackground(DEViseUIGlobals.bg);
        okButton.setForeground(DEViseUIGlobals.fg);
        okButton.setFont(DEViseUIGlobals.font);

        int size = attrs.length - 1;
        Label[] label = null;
        if (size == 0) {
            label = new Label[1];
            label[0] = new Label("");
        } else {
            label = new Label[size];
            for (int i = 0; i < size; i++) {
                label[i] = new Label(attrs[i + 1]);
            }
        }

        DEViseComponentPanel panel = new DEViseComponentPanel(label, "Vertical", 0);
        for (int i = 0; i < size; i++)
            label[i].setAlignment(Label.LEFT);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(10, 10, 10, 10);
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        gridbag.setConstraints(panel, c);
        add(panel);
        gridbag.setConstraints(okButton, c);
        add(okButton);

        pack();

        Dimension panesize = panel.getPreferredSize();
        if (panesize.width > (DEViseUIGlobals.maxScreenSize.width - 120) || panesize.height > (DEViseUIGlobals.maxScreenSize.height - 80)) {
            if (panesize.width > (DEViseUIGlobals.maxScreenSize.width - 120)) {
                panesize.width = DEViseUIGlobals.maxScreenSize.width - 120;
            }

            if (panesize.height > (DEViseUIGlobals.maxScreenSize.height - 80)) {
                panesize.height = DEViseUIGlobals.maxScreenSize.height - 80;
            }

            ScrollPane pane = new ScrollPane();
            pane.setSize(panesize);
            pane.add(panel);

            removeAll();
            gridbag.setConstraints(pane, c);
            add(pane);
            gridbag.setConstraints(okButton, c);
            add(okButton);

            pack();
        }

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

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
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
class SessionDlg extends Frame
{
    private jsdevisec jsc = null;

    private String sessionName = null;
    private java.awt.List fileList = null;
    private Label label = new Label("Current available sessions in directory    ");
    private Label directory = new Label("");
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    private String[] sessions = null;
    private boolean[] sessionTypes = null;
    private String[] sessionNames = null;

    private boolean status = false; // true means this dialog is showing

    public SessionDlg(jsdevisec what, Frame owner, boolean isCenterScreen, String[] data)
    {
        jsc = what;

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);

        setTitle("JavaScreen Open Dialog");

        label.setFont(new Font("Serif", Font.BOLD, 16));
        //label.setFont(DEViseUIGlobals.font);
        directory.setText("/" + jsc.currentDir);
        directory.setFont(DEViseUIGlobals.font);

        fileList = new java.awt.List(8, false);
        fileList.setBackground(DEViseUIGlobals.textBg);
        //fileList.setBackground(Color.white);
        fileList.setForeground(DEViseUIGlobals.textFg);
        fileList.setFont(DEViseUIGlobals.textFont);

        setSessionList(data);

        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button, "Horizontal", 20);

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
                            if (fileList.getItemCount() > 0) {
                                int idx = fileList.getSelectedIndex();
                                if (idx != -1) {
                                    sessionName = fileList.getItem(idx);
                                    if (sessionName.startsWith("[")) {
                                        String[] name = DEViseGlobals.parseString(sessionName, '[', ']');
                                        if (name[0].equals("..")) {
                                            if (jsc.currentDir.equals(jsc.rootDir)) {
                                                jsc.showMsg("You do not have access to this directory!");
                                                return;
                                            }

                                            int index = jsc.currentDir.lastIndexOf('/');
                                            if (index > 0) {
                                                jsc.currentDir = jsc.currentDir.substring(0, index);
                                            } else {
                                                jsc.showMsg("Invalid current directory \"" + jsc.currentDir + "\"!");
                                                jsc.currentDir = jsc.rootDir;
                                                close();
                                            }
                                        } else {
                                            jsc.currentDir = jsc.currentDir + "/" + name[0];
                                        }

                                        directory.setText("/" + jsc.currentDir);
                                        validate();

                                        //String dir = (jsc.currentDir).substring(14);

                                        jsc.dispatcher.start(DEViseCommands.GET_SESSION_LIST + " {" + jsc.currentDir + "}");
                                    } else {
                                        jsc.currentSession = sessionName;
                                        jsc.dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + DEViseUIGlobals.screenSize.width + " " + DEViseUIGlobals.screenSize.height
                                                                 + "\n" + DEViseCommands.OPEN_SESSION + " {" + jsc.currentDir + "/" + sessionName + "}");
                                        close();
                                    }
                                }
                            }
                        }
                    }
                });

        okButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (fileList.getItemCount() > 0) {
                            int idx = fileList.getSelectedIndex();
                            if (idx != -1) {
                                sessionName = fileList.getItem(idx);
                                if (sessionName.startsWith("[")) {
                                    String[] name = DEViseGlobals.parseString(sessionName, '[', ']');
                                    if (name[0].equals("..")) {
                                        if (jsc.currentDir.equals(jsc.rootDir)) {
                                            jsc.showMsg("You do not have access to this directory!");
                                            return;
                                        }

                                        int index = jsc.currentDir.lastIndexOf('/');
                                        if (index > 0) {
                                            jsc.currentDir = jsc.currentDir.substring(0, index);
                                        } else {
                                            jsc.showMsg("Invalid current directory \"" + jsc.currentDir + "\"!");
                                            jsc.currentDir = jsc.rootDir;
                                            close();
                                        }
                                    } else {
                                        jsc.currentDir = jsc.currentDir + "/" + name[0];
                                    }

                                    directory.setText("/" + jsc.currentDir);
                                    validate();

                                    jsc.dispatcher.start(DEViseCommands.GET_SESSION_LIST + " {" + jsc.currentDir + "}");
                                } else {
                                    jsc.currentSession = sessionName;
                                    jsc.dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + DEViseUIGlobals.screenSize.width + " " + DEViseUIGlobals.screenSize.height
                                                             + "\n" + DEViseCommands.OPEN_SESSION + " {" + jsc.currentDir + "/" + sessionName + "}");
                                    close();
                                }
                            }
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
        sessionTypes = new boolean[number];
        String tmpstr = null;
        for (int i = 0; i < number; i++) {
            sessionNames[i] = sessions[i * 3 + 1];
            tmpstr = sessions[i * 3 + 2];
            if (tmpstr.equals("0")) {
                sessionTypes[i] = true;
            } else {
                sessionTypes[i] = false;
            }
        }

        fileList.removeAll();

        for (int i = 0; i < number; i++) {
            if (sessionTypes[i]) {
                fileList.add(sessionNames[i]);
            } else {
                fileList.add("[" + sessionNames[i] + "]");
            }
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

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;

            jsc.sessiondlg = null;
        }
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting values such as screen size.
class SettingDlg extends Dialog
{
    jsdevisec jsc = null;
    public TextField screenX = new TextField(4);
    public TextField screenY = new TextField(4);
    public Button setButton = new Button("   Set   ");
    public Button statButton = new Button("Request");
    private boolean status = false; // true means this dialog is showing

    public SettingDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

        jsc = what;

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);

        setTitle("JavaScreen Setting");

        setButton.setBackground(DEViseUIGlobals.bg);
        setButton.setForeground(DEViseUIGlobals.fg);
        setButton.setFont(DEViseUIGlobals.font);

        screenX.setBackground(DEViseUIGlobals.textBg);
        screenX.setForeground(DEViseUIGlobals.textFg);
        screenX.setFont(DEViseUIGlobals.textFont);

        screenY.setBackground(DEViseUIGlobals.textBg);
        screenY.setForeground(DEViseUIGlobals.textFg);
        screenY.setFont(DEViseUIGlobals.textFont);

        screenX.setText("" + DEViseUIGlobals.screenSize.width);
        screenY.setText("" + DEViseUIGlobals.screenSize.height);

        statButton.setBackground(DEViseUIGlobals.bg);
        statButton.setForeground(DEViseUIGlobals.fg);
        statButton.setFont(DEViseUIGlobals.font);

        if (DEViseUIGlobals.inBrowser) {
            screenX.setEditable(false);
            screenY.setEditable(false);
            setButton.setEnabled(false);
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
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label label1 = new Label("JavaScreen Size:");
        gridbag.setConstraints(label1, c);
        add(label1);

        c.insets = new Insets(10, 0, 0, 5);
        gridbag.setConstraints(screenX, c);
        add(screenX);

        gridbag.setConstraints(screenY, c);
        add(screenY);

        c.insets = new Insets(10, 10, 0, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(setButton, c);
        add(setButton);

        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label2 = new Label("JSPOP Status:");
        gridbag.setConstraints(label2, c);
        add(label2);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(statButton, c);
        add(statButton);

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
                        if (jsc.isSessionOpened) {
                            jsc.showMsg("You already have a session opened!\nPlease close current session first!");
                            return;
                        }

                        int x = 0, y = 0;

                        try {
                            x = Integer.parseInt(screenX.getText());
                            y = Integer.parseInt(screenY.getText());

                            if (x < DEViseUIGlobals.minScreenSize.width || x > DEViseUIGlobals.maxScreenSize.width
                                || y < DEViseUIGlobals.minScreenSize.height || y > DEViseUIGlobals.maxScreenSize.height) {
                                throw new NumberFormatException();
                            } else {
                                jsc.jscreen.setScreenDim(x, y);
                                close();
                            }
                        } catch (NumberFormatException e) {
                            jsc.showMsg("Invalid screen size specified!\nJavaScreen size must be larger than ("
                                         + DEViseUIGlobals.minScreenSize.width + ", " + DEViseUIGlobals.minScreenSize.height
                                         + ") and smaller than ("
                                         + DEViseUIGlobals.maxScreenSize.width + ", " + DEViseUIGlobals.maxScreenSize.height + ")");
                        }
                    }
                });

        statButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        jsc.dispatcher.start(DEViseCommands.GET_SERVER_STATE);

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
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
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
    private java.awt.List serverList = null, activeClientList = null, suspendClientList = null;
    private Label label1 = new Label("Current active server:");
    private Label label2 = new Label("Current active client:");
    private Label label3 = new Label("Current suspended client:");
    private Button okButton = new Button("   OK   ");

    private boolean status = false; // true means this dialog is showing

    public ServerStateDlg(Frame owner, boolean isCenterScreen, String data)
    {
        super(owner, true);

        String[] list = DEViseGlobals.parseStr(data, " ");
        String[] list1 = null, list2 = null, list3 = null;

        if (list != null && list.length > 3) {
            try {
                int number1 = Integer.parseInt(list[0]);
                if (number1 != 0) {
                    list1 = new String[number1];
                    for (int i = 0; i < number1; i++) {
                        list1[i] = list[i + 1];
                    }
                }

                int number2 = Integer.parseInt(list[number1 + 1]);
                if (number2 != 0) {
                    list2 = new String[number2];
                    for (int i = 0; i < number2; i++) {
                        list2[i] = list[i + 2 + number1];
                    }
                }

                int number3 = Integer.parseInt(list[number1 + number2 + 2]);
                if (number3 != 0) {
                    list3 = new String[number3];
                    for (int i = 0; i < number3; i++) {
                        list3[i] = list[i + 3 + number1 + number2];
                    }
                }
            } catch (NumberFormatException e) {
            }
        }

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);

        setTitle("JSPOP current state");

        label1.setFont(new Font("Serif", Font.BOLD, 16));
        label2.setFont(new Font("Serif", Font.BOLD, 16));
        label3.setFont(new Font("Serif", Font.BOLD, 16));

        serverList = new java.awt.List(4, false);
        serverList.setBackground(DEViseUIGlobals.textBg);
        serverList.setForeground(DEViseUIGlobals.textFg);
        serverList.setFont(DEViseUIGlobals.textFont);
        if (list1 != null) {
            for (int i = 0; i < list1.length; i++) {
                serverList.add(list1[i]);
            }
        }
        activeClientList = new java.awt.List(6, false);
        activeClientList.setBackground(DEViseUIGlobals.textBg);
        activeClientList.setForeground(DEViseUIGlobals.textFg);
        activeClientList.setFont(DEViseUIGlobals.textFont);
        if (list2 != null) {
            for (int i = 0; i < list2.length; i++) {
                activeClientList.add(list2[i]);
            }
        }
        suspendClientList = new java.awt.List(6, false);
        suspendClientList.setBackground(DEViseUIGlobals.textBg);
        suspendClientList.setForeground(DEViseUIGlobals.textFg);
        suspendClientList.setFont(DEViseUIGlobals.textFont);
        if (list3 != null) {
            for (int i = 0; i < list3.length; i++) {
                suspendClientList.add(list3[i]);
            }
        }

        okButton.setBackground(DEViseUIGlobals.bg);
        okButton.setForeground(DEViseUIGlobals.fg);
        okButton.setFont(DEViseUIGlobals.font);

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
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}
