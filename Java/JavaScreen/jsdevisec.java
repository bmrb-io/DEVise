// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
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
// Revision 1.74  2001/01/25 19:40:34  wenger
// Added mode switching capability to the jsb (embedded) form of the client.
//
// Revision 1.73  2001/01/23 22:57:27  xuk
// add SetModeDlg for mode switch.
//
// Revision 1.72  2001/01/23 04:01:47  xuk
// Add "Mode" button and SetCgiUrl dialog for switching between socket and cgi modes.
//
// Revision 1.71  2001/01/08 20:31:58  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.70.4.9  2001/01/05 19:15:46  wenger
// Updated copyright dates.
//
// Revision 1.70.4.8  2001/01/04 21:15:20  wenger
// Added socket/CGI communication mode indicator.
//
// Revision 1.70.4.7  2000/12/29 22:41:39  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.70.4.6  2000/11/22 17:44:01  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.70.4.5  2000/11/21 01:51:35  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.70.4.4  2000/11/08 19:13:52  wenger
// Fixed bug 600 (JS exits even if you click No); fixed crash if you click
// Exit in JS before connecting to a jspop.
//
// Revision 1.70.4.3  2000/10/18 18:29:24  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.70.4.2  2000/10/11 02:59:19  xuk
// Add notifyall() to wake up the dispatcher thread.
//
// Revision 1.70.4.1  2000/10/09 16:33:12  xuk
// Change dispathcer.start() to dispatcher.command = cmd, since D
// EViseCmdDispatcher thread starts only once now.
//
// Revision 1.70  2000/07/20 16:26:08  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.69  2000/07/19 20:11:38  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.68  2000/07/11 16:39:19  venkatan
// *** empty log message ***
//
// Revision 1.67  2000/07/10 12:26:03  venkatan
// *** empty log message ***
//
// Revision 1.66  2000/06/26 16:48:33  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.65  2000/06/22 20:51:25  wenger
// Changed colors and font in drill-down dialog to make text more
// readable.
//
// Revision 1.64  2000/06/12 22:14:39  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.63  2000/05/22 17:52:51  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.62  2000/05/12 20:43:57  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
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

import  java.applet.*;
import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;

public class jsdevisec extends Panel
{
    private YLogGUI debugWindow = null;

    public DEViseCmdDispatcher dispatcher = null;

    public Frame parentFrame = null;
    private boolean isCenterScreen = false;
    public Applet _parentApplet = null;

    public DEViseScreen jscreen = null;

    private Panel topPanel = null;
    private Panel mainPanel = null;

    private Button openButton = new Button("Open");
    private Button closeButton = new Button("Close");
    public  Button stopButton = new Button("Stop");
    private Button restartButton = new Button("Restart");
    //private Button statButton = new Button("Stats");
    private Button setButton = new Button("Option");
    private Button exitButton = new Button("Exit");
    private Button filterButton = new Button("Filter");
    private Button helpButton = new Button("Help");
    private Label commMode = new Label("");
    private Button modeButton = new Button("Mode");
    private Button collabButton = new Button("Collabrate");

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
    private SetCgiUrlDlg setcgiurldlg = null;
    private SetModeDlg setmodedlg = null;
    private CollabDlg collabdlg = null;

    public boolean isSessionOpened = false;

    // This seems to be true if we are in the process of quitting -- avoids
    // re-entering the quit code(?).
    private boolean isQuit = false;

    public int stopNumber = 0;

    public boolean isShowProgramInfo = false;
    public String rootDir = "DEViseSession";
    public String currentDir = "DEViseSession";
    public String currentSession = null;

    public DEViseJSValues jsValues = null;

    // collabrated JS ID
    public int specialID = 0;


	// images[0-9] are the gears; 10 and 11 are "traffic lights"
	//   (devise[0-10].gif).
	// sessionName is non-null only in jsb.
    public jsdevisec(Applet parentApplet, Frame frame, Vector images,
	  String sessionName, DEViseJSValues jv)
    {
	// create the DEViseJSValues object
	jsValues = jv;

	if (jsValues.debug._logEnabled) {
	    jsValues.debug._logger = new DEViseDebugLog(jsValues);
	}

	_parentApplet = parentApplet;

        // frame might be null if JavaScreen is running inside a browser
        parentFrame = frame;
        if (parentFrame == null) {
            parentFrame = new Frame();
            isCenterScreen = true;
        }

        if (jv.debug._debugLevel > 0) {
            debugWindow = new YLogGUI(jv.debug._debugLevel);
        }

        // determine the font size according to JavaScreen size
        int width = jsValues.uiglobals.maxScreenSize.width;
        int height = jsValues.uiglobals.maxScreenSize.height;

        if (width > 800 ) {
            jsValues.uiglobals.font = DEViseFonts.getFont(12,
	      DEViseFonts.SERIF, 0, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(12,
	      DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(12,
	      DEViseFonts.SERIF, 0, 0);
        } else if (width > 640) {
            jsValues.uiglobals.font = DEViseFonts.getFont(10,
	      DEViseFonts.SERIF, 0, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(10,
	      DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(10,
	      DEViseFonts.SERIF, 0, 0);
        } else {
            jsValues.uiglobals.font = DEViseFonts.getFont(8,
	      DEViseFonts.SERIF, 0, 0);
            jsValues.uiglobals.font2 = DEViseFonts.getFont(8,
	      DEViseFonts.SANS_SERIF, 0, 0);
            jsValues.uiglobals.textFont = DEViseFonts.getFont(8,
	      DEViseFonts.SERIF, 0, 0);
        }

        setBackground(jsValues.uiglobals.bg);
        setForeground(jsValues.uiglobals.fg);
        setFont(jsValues.uiglobals.font);
        setLayout(new BorderLayout(2, 2));

        topPanel = new Panel(new BorderLayout(2, 2));
        mainPanel = new Panel(new FlowLayout(FlowLayout.LEFT, 2, 2));

        animPanel = new DEViseAnimPanel(this, images, 100);

        mainPanel.add(animPanel);

		// Note: we're just relying on the file names of the images,
		// and the code that reads them in, to assume that image 9
		// and image 10 are what we need for the traffic light.
        if (images != null && images.size() == 11) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0", this);
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
        if (jsValues.uiglobals.inBrowser) {
            button = new Component[4];
            button[0] = restartButton;
            button[1] = stopButton;
            button[2] = modeButton;
            button[3] = helpButton;
        } else {
            button = new Component[10];
            button[0] = openButton;
            button[1] = closeButton;
            button[2] = stopButton;
            button[3] = restartButton;
            button[4] = setButton;
            button[5] = filterButton;
            button[6] = modeButton;
	    button[7] = collabButton;
            button[8] = helpButton;
            button[9] = exitButton;
        }

        DEViseComponentPanel buttonPanel = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 5,
	  DEViseComponentPanel.ALIGN_LEFT, this);
	  if (jsValues.connection.cgi) {
	      cgiMode();
	  } else {
	      socketMode();
	  }
	  buttonPanel.add(commMode);

        mainPanel.add(buttonPanel);

        viewInfo = new DEViseViewInfo(this, images);

        topPanel.add(mainPanel, BorderLayout.WEST);
        topPanel.add(viewInfo, BorderLayout.EAST);

        if (jsValues.uiglobals.inBrowser) {
            topPanel.setFont(DEViseFonts.getFont(14, DEViseFonts.SERIF, 0, 0));
            topPanel.add(new Label("                       " +
	      DEViseUIGlobals.javaScreenTitle), BorderLayout.CENTER);
        }

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

        jscreen = new DEViseScreen(this);
        Panel screenPanel = new Panel(new FlowLayout(FlowLayout.CENTER, 3, 3));
        screenPanel.setBackground(jsValues.uiglobals.screenBg);
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
                                dispatcher.stop(true);
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
                        checkQuit();
                    }
                });

        modeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			setMode();
                    }
                });

        collabButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            showMsg("You already have a session opened!\nPlease close current session first!");
                            return;
                        }

                        showCollab();
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
	    pn("sessionname: " + sessionName);
            int index = sessionName.lastIndexOf('/');
            if (index > 0) {
                currentDir = currentDir + "/" + sessionName.substring(0, index);
                currentSession = sessionName.substring(index + 1, sessionName.length());
            } else {
                currentSession = sessionName;
            }

            dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + jsValues.uiglobals.screenSize.width + " " + jsValues.uiglobals.screenSize.height + "\n" + DEViseCommands.OPEN_SESSION + " {" + currentDir + "/" + currentSession + "}");
        }
    } // end of constructor

    // print out message to debug window
    // pn() methods add a newline to the end of the string; p() methods don't.
    public void pn(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0) {
            debugWindow.pn(msg, level);
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0) {
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
	jsValues.debug.log("Showing message box: " + msg); 
	mainPanel.setBackground(jsValues.uiglobals.bg_warn);

	// Note: we have two cases here to make things work in the case
	// where we have more than one message box shown at once.  msgbox
	// is a class member so we know whether we have at least one
	// message box showing.
        if (msgbox == null) {
            msgbox = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, jsValues.uiglobals.font, jsValues.uiglobals.bg, jsValues.uiglobals.fg);
            msgbox.open();
            String result = msgbox.getResult();
            msgbox = null;
	    mainPanel.setBackground(jsValues.uiglobals.bg);
	    jsValues.debug.log("Done with message box");
            return result;
        } else {
	    // We only get here if we already have one message box showing,
	    // and need to show another one.
            YMsgBox box = new YMsgBox(parentFrame, isCenterScreen, true, msg, title, style, jsValues.uiglobals.font, jsValues.uiglobals.bg, jsValues.uiglobals.fg);
            box.open();
            String result = box.getResult();
            box = null;
	    mainPanel.setBackground(jsValues.uiglobals.bg);
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
        recorddlg = new RecordDlg(parentFrame, isCenterScreen, msg, this);
        recorddlg.open();
        recorddlg = null;
    }

    public String showViewDialogHelp(String msg){
       
	if(msg == null) {
	   System.out.println("conflict 1");
	   return null;
	   }

        return showMsg(msg, "Help", YMsgBox.YMBXOK);
    }

    public void showServerState(String msg)
    {
        statedlg = new ServerStateDlg(parentFrame, isCenterScreen, msg, this);
        statedlg.open();
        statedlg = null;
    }

    public void showSetting()
    {
        settingdlg = new SettingDlg(this, parentFrame, isCenterScreen);
        settingdlg.open();
        settingdlg = null;
    }

    public void showCollab()
    {
        collabdlg = new CollabDlg(this, parentFrame, isCenterScreen);
        collabdlg.open();
        collabdlg = null;
	dispatcher.start(null);
    }

    public void setCgiUrl()
    {
        setcgiurldlg = new SetCgiUrlDlg(this, parentFrame, isCenterScreen);
        setcgiurldlg.open();
        setcgiurldlg = null;
    }

    public void setMode()
    {
        setmodedlg = new SetModeDlg(this, parentFrame, isCenterScreen);
        setmodedlg.open();
        setmodedlg = null;
    }

    public boolean isShowingMsg()
    {
        if (sessiondlg != null || settingdlg != null || statedlg != null || recorddlg != null || msgbox != null || setcgiurldlg != null) {
            return true;
        } else {
            return false;
        }
    }

    // Check whether we really want to quit, and call destroy() if so.
    public synchronized void checkQuit()
    {
        boolean reallyQuit = true;

	if (specialID == 0) { 
	    if (dispatcher.getStatus() != 0) {
		String result = confirmMsg("JavaScreen still busy talking " +
					   "to server!\nDo you wish to exit anyway?");
		if (result.equals(YMsgBox.YIDNO)) {
		    reallyQuit = false;
		}
	    }
	}
	//TEMP -- do we want to check for an open session here?

	if (reallyQuit) {
            if (jsValues.uiglobals.isApplet) {
                parentFrame.dispose();
		// This will call destroy().
	        _parentApplet.destroy();
            } else {
                destroy();
	    }
	}
    }

    public synchronized boolean getQuitStatus()
    {
        return isQuit;
    }

    public synchronized void destroy()
    {
        if (isQuit) {
            return;
        }

        isQuit = true;

        if (dispatcher != null) {

	    if (specialID != 0 && dispatcher.dispatcherThread != null) {
		dispatcher.dispatcherThread.stop();
		dispatcher.dispatcherThread = null;
	    }

            dispatcher.destroy();
            dispatcher = null;
        }

        if (!jsValues.uiglobals.isApplet) {
            System.exit(0);
	}
    }
    
    public void socketMode()
    {
        commMode.setText("Socket");
    }

    public void cgiMode()
    {
        commMode.setText("CGI");
    }
}

// ------------------------------------------------------------------------

// Dialog to show record values.
class RecordDlg extends Dialog
{
    jsdevisec jsc = null;

    String[] attrs = null;
    Button okButton = new Button("  OK  ");
    private boolean status = false; // true means this dialog is showing

    public RecordDlg(Frame owner, boolean isCenterScreen, String[] data, jsdevisec what)
    {
        super(owner, true);
	
	jsc = what;

        jsc.jsValues.debug.log("Creating RecordDlg");

        attrs = data;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Record Attributes");

        okButton.setBackground(jsc.jsValues.uiglobals.bg);
        okButton.setForeground(jsc.jsValues.uiglobals.fg);
        okButton.setFont(jsc.jsValues.uiglobals.font);

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

        DEViseComponentPanel panel = new DEViseComponentPanel(label,
	  DEViseComponentPanel.LAYOUT_VERTICAL, 0, jsc);
        panel.setBackground(jsc.jsValues.uiglobals.textBg);
        for (int i = 0; i < size; i++) {
            label[i].setAlignment(Label.LEFT);
            label[i].setBackground(jsc.jsValues.uiglobals.textBg);
            label[i].setForeground(jsc.jsValues.uiglobals.textFg);
	}

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
        if (panesize.width > (jsc.jsValues.uiglobals.maxScreenSize.width - 120) || panesize.height > (jsc.jsValues.uiglobals.maxScreenSize.height - 80)) {
            if (panesize.width > (jsc.jsValues.uiglobals.maxScreenSize.width - 120)) {
                panesize.width = jsc.jsValues.uiglobals.maxScreenSize.width - 120;
            }

            if (panesize.height > (jsc.jsValues.uiglobals.maxScreenSize.height - 80)) {
                panesize.height = jsc.jsValues.uiglobals.maxScreenSize.height - 80;
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
        jsc.jsValues.debug.log("Opening RecordDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
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
	what.jsValues.debug.log("Creating SessionDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
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

        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);

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

	//TEMP -- this has too damn many levels of indentation!
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
                                        jsc.dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + jsc.jsValues.uiglobals.screenSize.width + " " + jsc.jsValues.uiglobals.screenSize.height
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
                                    jsc.dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " + jsc.jsValues.uiglobals.screenSize.width + " " + jsc.jsValues.uiglobals.screenSize.height
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
	jsc.jsValues.debug.log("Opening SessionDlg");
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
	jsc.jsValues.debug.log("Closed SessionDlg");
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

	what.jsValues.debug.log("Creating SettingDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JavaScreen Setting");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

        screenX.setBackground(jsc.jsValues.uiglobals.textBg);
        screenX.setForeground(jsc.jsValues.uiglobals.textFg);
        screenX.setFont(jsc.jsValues.uiglobals.textFont);

        screenY.setBackground(jsc.jsValues.uiglobals.textBg);
        screenY.setForeground(jsc.jsValues.uiglobals.textFg);
        screenY.setFont(jsc.jsValues.uiglobals.textFont);

        screenX.setText("" + jsc.jsValues.uiglobals.screenSize.width);
        screenY.setText("" + jsc.jsValues.uiglobals.screenSize.height);

        statButton.setBackground(jsc.jsValues.uiglobals.bg);
        statButton.setForeground(jsc.jsValues.uiglobals.fg);
        statButton.setFont(jsc.jsValues.uiglobals.font);

        if (jsc.jsValues.uiglobals.inBrowser) {
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
    private Button okButton = new Button("   OK   ");

    private boolean status = false; // true means this dialog is showing

    public ServerStateDlg(Frame owner, boolean isCenterScreen, String data, jsdevisec what)
    {
        super(owner, true);

	jsc = what;

	jsc.jsValues.debug.log("Creating ServerStateDlg");

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
        if (list1 != null) {
            for (int i = 0; i < list1.length; i++) {
                serverList.add(list1[i]);
            }
        }
        activeClientList = new java.awt.List(6, false);
        activeClientList.setBackground(jsc.jsValues.uiglobals.textBg);
        activeClientList.setForeground(jsc.jsValues.uiglobals.textFg);
        activeClientList.setFont(jsc.jsValues.uiglobals.textFont);
        if (list2 != null) {
            for (int i = 0; i < list2.length; i++) {
                activeClientList.add(list2[i]);
            }
        }
        suspendClientList = new java.awt.List(6, false);
        suspendClientList.setBackground(jsc.jsValues.uiglobals.textBg);
        suspendClientList.setForeground(jsc.jsValues.uiglobals.textFg);
        suspendClientList.setFont(jsc.jsValues.uiglobals.textFont);
        if (list3 != null) {
            for (int i = 0; i < list3.length; i++) {
                suspendClientList.add(list3[i]);
            }
        }

        okButton.setBackground(jsc.jsValues.uiglobals.bg);
        okButton.setForeground(jsc.jsValues.uiglobals.fg);
        okButton.setFont(jsc.jsValues.uiglobals.font);

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
    public TextField url = new TextField(20);
    public Button setButton = new Button("   Set   ");
    public Button cancelButton = new Button("  Cancel ");    
    private boolean status = false; // true means this dialog is showing

    public SetCgiUrlDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating SetCgiUrlDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting CGI URL");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

        url.setBackground(jsc.jsValues.uiglobals.textBg);
	url.setForeground(jsc.jsValues.uiglobals.textFg);
        url.setFont(jsc.jsValues.uiglobals.textFont);

        url.setText("/cgi-bin/js.cgi");

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

        Button [] button = new Button[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);

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
			jsc.jsValues.connection.cgiURL = url.getText();
                        close();
                    }
                });

        cancelButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			jsc.jsValues.connection.cgi = false;
			jsc.socketMode();			
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

// Dialog for setting socket/cgi mode.
class SetModeDlg extends Dialog
{
    jsdevisec jsc = null;
    public Button socketButton = new Button(" Socket ");
    public Button cgiButton = new Button("   CGI  ");
    private boolean status = false; // true means this dialog is showing

    public SetModeDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating SetModeDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting Socket/CGI Mode");

        socketButton.setBackground(jsc.jsValues.uiglobals.bg);
        socketButton.setForeground(jsc.jsValues.uiglobals.fg);
        socketButton.setFont(jsc.jsValues.uiglobals.font);

        cgiButton.setBackground(jsc.jsValues.uiglobals.bg);
        cgiButton.setForeground(jsc.jsValues.uiglobals.fg);
        cgiButton.setFont(jsc.jsValues.uiglobals.font);

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

        Label label = new Label("Mode Switch:");
        gridbag.setConstraints(label, c);
        add(label);

        gridbag.setConstraints(socketButton, c);
        add(socketButton);
        gridbag.setConstraints(cgiButton, c);
        add(cgiButton);

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

        cgiButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			jsc.jsValues.connection.cgi = true;
			jsc.cgiMode();
                        close();
			jsc.setCgiUrl();
                    }
                });

        socketButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			jsc.jsValues.connection.cgi = false;
			jsc.socketMode();
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
	jsc.jsValues.debug.log("Closed SetModeDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}


// ------------------------------------------------------------------------
// Dialog for setting collabrated JavaScreen ID.
class CollabDlg extends Dialog
{
    jsdevisec jsc = null;
    public TextField screenX = new TextField(4);
    public Button setButton = new Button("   Set   ");
    private boolean status = false; // true means this dialog is showing

    public CollabDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating CollabDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Collabration JavaScreen");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

        screenX.setBackground(jsc.jsValues.uiglobals.textBg);
        screenX.setForeground(jsc.jsValues.uiglobals.textFg);
        screenX.setFont(jsc.jsValues.uiglobals.textFont);
        screenX.setText("1");

        if (jsc.jsValues.uiglobals.inBrowser) {
            screenX.setEditable(false);
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

        c.insets = new Insets(10, 10, 0, 0);
        c.gridwidth = 1;
        Label label1 = new Label("JavaScreen ID:");
        gridbag.setConstraints(label1, c);
        add(label1);

        c.insets = new Insets(10, 0, 0, 5);
        gridbag.setConstraints(screenX, c);
        add(screenX);

        c.insets = new Insets(10, 10, 0, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(setButton, c);
        add(setButton);

        pack();

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (jsc.isSessionOpened) {
                            jsc.showMsg("You already have a session opened!\nPlease close current session first!");
                            return;
                        }

                        try {
                            jsc.specialID = Integer.parseInt(screenX.getText());
			    close();
                        } catch (NumberFormatException e) {
                            jsc.showMsg("Invalid JavaScreen ID specified!");
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
	jsc.jsValues.debug.log("Opening CollabDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
	jsc.jsValues.debug.log("Closed CollabDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}
