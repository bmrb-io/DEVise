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
// Revision 1.136  2002/02/21 18:35:19  wenger
// Added jsdevisec.restartSession method so Wavelet-IDR JavaScript code
// can force a session restart.
//
// Revision 1.135  2001/12/17 23:18:08  wenger
// Fixed problem preventing JSB from starting.
//
// Revision 1.134  2001/11/29 17:54:36  xuk
// Added "Option", "Filter", "Playback" and "Collaboration" buttons for JSB version
//
// Revision 1.133  2001/11/28 21:56:21  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.132  2001/11/27 18:14:31  xuk
// Return error message to JS, when there is no devised running on JSPoP side.
//
// Revision 1.131  2001/11/19 22:20:38  xuk
// Changes for String[] format socket/commands.
//
// Revision 1.130  2001/11/19 17:17:03  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.129.2.2  2001/11/19 21:04:02  wenger
// Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
// Squid to be able to force DEVise to re-read all data and update the
// visualization; did some cleanup of JavaScreenCmd.C.
//
// Revision 1.129.2.1  2001/11/13 20:31:35  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.129  2001/11/07 23:09:28  wenger
// Merged changes from js_no_reconnect_br_2 thru js_no_reconnect_br_3.
// (Mostly installation-related improvements.)
//
// Revision 1.128  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.127  2001/11/06 16:27:20  xuk
// Reset collaboration follower's screen size and resolution to deal with different screen size from leaders.
//
// Revision 1.126  2001/11/02 19:38:38  xuk
// Added "Collaborate" and "Playback" buttons on menu.
//
// Revision 1.125  2001/10/30 17:35:43  xuk
// Created DEViseClient object for collaborating clients in jspop.
// 1. Deleted showCollab();
// 2. In setModeDlg class, modified socketButton and collabButton actions;
// 3. In enterCollabPassDlg class, sends JAVAC_Collaborate {ID} {passwd} to
// jspop, don't disconnect socket.
//
// Revision 1.124.2.1  2001/11/07 17:22:37  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.124  2001/10/24 22:15:33  wenger
// More collaboration-related fixes.
//
// Revision 1.123  2001/10/19 19:28:55  xuk
// Added playback indicator.
//
// Revision 1.122  2001/10/18 15:15:26  xuk
// Fixed bug 714: can't switch out of collabration mode in Netscape.
//
// Revision 1.121  2001/10/17 15:54:25  wenger
// Fixed bug 712 (JS collaboration doesn't work); various other code
// cleanup.
//
// Revision 1.120  2001/10/16 22:14:28  wenger
// Major cleanup of command playback code; fixed bug 711 (problem with
// command log playback).
//
// Revision 1.119  2001/10/12 02:05:56  xuk
// Using timestamp-based client ID.
// Expanded specialID to long.
//
// Revision 1.118  2001/10/10 19:28:24  xuk
// Added display control command line argument/parameter for auto playback.
// For JS application: command-line argument: -playbackdisplayoff to turn off display, default is turning on display;
// For JS applet: parameter playbackdisplay = true|false to turn on|off display.
//
// Revision 1.117.2.3  2001/11/07 20:13:20  wenger
// Fixed bug 725 (-cgiurl command line argument and parameter were not
// used by the CGI GUI).
//
// Revision 1.117  2001/10/05 20:00:26  xuk
// Fixed bug 701: command log playwork can work for URL and simple file path.
//
// Revision 1.116  2001/10/02 22:00:58  xuk
// Added command-line opition for client log playback.
// Modified jsdevisec() for auto playback.
//
// Revision 1.115  2001/09/28 18:53:55  xuk
// JavaScreen command log playback.
// Added isPlayback, isDisplay, isOriginal, logFileName variables;
// Added setLogFile() and logPlayback() methods;
// Modified setModeDlg() to add "Playback" button.
//
// Revision 1.114  2001/09/27 19:52:33  wenger
// Fixed bug 688 (problem dealing with links in session directories);
// improved JS error handling for session open; eliminated a bunch of
// duplicate code in the JS.
//
// Revision 1.113  2001/09/10 21:20:09  xuk
// Solve the client disconnection problem.
// Added disconnectedMode() method to indicate disconnected mode of JavaScreen.
//
// Revision 1.112  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.111  2001/08/17 19:01:37  wenger
// Cleaned up formatting.
//
// Revision 1.110  2001/05/11 20:36:14  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.109  2001/05/07 21:53:26  wenger
// Found and fixed bug 670; jss checks for /tmp.X1-lock before starting
// Xvfb.
//
// Revision 1.108  2001/04/25 19:41:49  xuk
// Fixed bug 661: when a JS goes into Collaboration mode from Socket mode,
// It will not become an available collaboration leader.
//
// Revision 1.107  2001/04/24 01:46:17  xuk
// Fixed bug 658: restoring pre-collaboration session, when leader exits.
//
// Revision 1.106  2001/04/21 20:52:53  xuk
// Added functionality to disable collaboration.
// Added isCollab variable.
// Added disableCollab() function.
//
// Revision 1.105  2001/04/20 20:27:05  xuk
// Fixed bug 659: if no leaders are available for ollaboration, the selection dialog will not be shown.
//
// Revision 1.104  2001/04/20 19:57:54  xuk
// Added JavaScreen version in "Option" dialog.
//
// Revision 1.103  2001/04/18 16:04:53  wenger
// Changed the JSA version of the JavaScreen to have two rows of buttons
// because we're running out of space; changed JS version to 4.1.
//
// Revision 1.102  2001/04/17 17:49:17  wenger
// Button to show or hide debug log is now also in the JSB version.
//
// Revision 1.101  2001/04/17 15:51:36  xuk
// Added the functionality of close debug window.
//
// Revision 1.100  2001/04/17 02:33:49  xuk
// Added functionality to display logging info under the non-dubug mode.
// 1. added msgBuffer to store all the logging info.
// 2. added "Logs" button.
// 3. added setLog() function.
// 4. changed pn(), p() functions.
//
// Revision 1.99  2001/04/12 15:29:59  xuk
// When collaboration leader exits, followers don't exit; instead they go back to n
// ormal mode.
// Added collabQuit() function.
//
// Revision 1.98  2001/04/11 21:16:29  xuk
// A collaboration leader could find out the followers hostname.
//
// Revision 1.97  2001/04/05 16:14:15  xuk
// Fixed bugs for JSPoP status query.
// Modified serverStateDlg() to display JSPoP status correctly.
//
// Revision 1.96  2001/03/25 20:34:44  xuk
// Fixed bug for Collaboration Dlag.
//
// Revision 1.95  2001/03/19 23:37:53  xuk
// Fixed bugs.
//
// Revision 1.94  2001/03/19 23:10:32  xuk
// Fixed bug for no available JavaScreen for collaboration.
//
// Revision 1.93  2001/03/08 21:10:13  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.92  2001/03/05 16:47:01  xuk
// *** empty log message ***
//
// Revision 1.91  2001/03/04 22:02:59  xuk
// Disabled some buttons when JS in "collaboration" mode.
//
// Revision 1.90  2001/03/03 20:03:04  xuk
// Restore old state if user goes into, then out of, collaboration mode.
// 1.When "Start Collaboration" button is pressed while a normal session is open, send JAVAC_SaveCurSession command to JSpop.
// 2.When "Socket" button is pressed, after out of collaboration mode, send JAVAC_ReopenSession command to JSpop.
// 3.Added sessionSaved to remember a user goes into collaboration mode while a normal sesseion is open.
//
// Revision 1.89  2001/02/23 17:41:42  xuk
// Added machine name and session name on the client list sent to collaboration JS.
//
// Revision 1.88  2001/02/23 03:48:50  xuk
// Fixed bugs: "follower" JS doesn't reponse the "Collaboration Status" request.
//
// Revision 1.87  2001/02/21 17:44:33  xuk
// Added the collaboration security features.
// When a user opens a session normally, the default will be that their
// connection is not available for collaboration;
// User can set password for collaboration:
// 	Added collabPass (default = "guest");
// 	Added CollabPassDlg() and showCollabPass() for set collaboration passwd;
// Enter password for collaboration:
// 	Added enterCollabPass() and EnterCollabPassDlg().
//
// Revision 1.86  2001/02/20 20:02:26  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.85  2001/02/19 20:30:23  xuk
// Added command(s) and GUI so that a collaboration leader can find out who is
// collaborating with it.
// Changes in settingDlg(), added "collaboration stauts" and "cancel" buttons;
// Added showCollabState(), show collaboration info.
//
// Revision 1.84  2001/02/16 23:39:39  xuk
// Added "Cancel" button in "Mode" menu.
//
// Revision 1.83  2001/02/16 17:51:16  xuk
// Added new command and GUI for collaboration JS to collect active client ID list.
// Added showClientList();
// Changed CollabIdDlg() for new GUI.
//
// Revision 1.82  2001/02/15 03:21:35  xuk
// Fixed bugs for collaboration JavaScreen.
//
// Revision 1.81  2001/02/12 02:46:57  xuk
// JavaScreen can prevent from collaboration.
// Added isAbleCollab; Changes in setModeDlg().
//
// Revision 1.79  2001/02/12 02:44:06  xuk
// Made the JavaScreen can switch between collaboration and socket modes.
//
// Revision 1.78  2001/02/08 18:06:12  xuk
// Merged the "Collaboration" button into "Mode" button to save menu bar space.
//
// Revision 1.77  2001/02/08 17:45:02  xuk
// Added collaboration mode indicator.  collabMode()
//
// Revision 1.76  2001/01/30 03:08:34  xuk
// Fix bugs for collaboration.
//
// Revision 1.75  2001/01/30 03:00:09  xuk
// Add "Collaboration" button for collaboration JavaScreen.
// Add specialID for collaborated JS ID.
//
// Revision 1.74.2.1  2001/02/05 22:02:11  wenger
// Fixed bugs 639 and 640 and other problems associated with destroying
// and re-starting the JavaScreen applets.
//
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

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.applet.*;
import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;
import  java.lang.*;

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
    private Button setButton = new Button("Option");
    private Button exitButton = new Button("Exit");
    private Button filterButton = new Button("Filter");
    private Button helpButton = new Button("Help");
    private Label commMode = new Label("");
    private Button modeButton = new Button("Mode");
    private Button collabButton = new Button("Collaborate");
    private Button playbackButton = new Button("Playback");
    private final String displayLogStr = "Show Log";
    private final String closeLogStr = "Hide Log";
    private Button logButton = new Button(displayLogStr);

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
    private SetLogFileDlg setlogfiledlg = null;
    private SetModeDlg setmodedlg = null;
    private CollabSelectDlg collabSelectDlg = null;
    public CollabIdDlg collabIdDlg = null;
    public CollabPassDlg collabpassdlg = null;
    public EnterCollabPassDlg entercollabpassdlg = null;
    public CollabStateDlg collabstatedlg = null;

    public boolean isSessionOpened = false;

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

    // images[0-9] are the gears; 10 and 11 are "traffic lights"
    //   (devise[0-10].gif).
    public jsdevisec(Applet parentApplet, Frame frame, Vector images,
      DEViseJSValues jv)
    {
	// create the DEViseJSValues object
	jsValues = jv;

	String sessionName = jv.session.defaultName;

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
	    System.out.println("Creating new debug window");
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
            button = new Component[9];
            button[0] = restartButton;
            button[1] = stopButton;
            button[2] = setButton;
            button[3] = filterButton;
            button[4] = modeButton;
	    button[5] = collabButton;
	    button[6] = playbackButton;
            button[7] = logButton;
            button[8] = helpButton;
        } else {
            button = new Component[12];
            button[0] = openButton;
            button[1] = closeButton;
            button[2] = stopButton;
            button[3] = restartButton;
            button[4] = setButton;
            button[5] = filterButton;
            button[6] = modeButton;
	    button[7] = collabButton;
	    button[8] = playbackButton;
	    button[9] = logButton;
            button[10] = helpButton;
            button[11] = exitButton;
        }

        DEViseComponentPanel buttonPanel = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 6,
	  DEViseComponentPanel.ALIGN_LEFT, this);
	if (jsValues.connection.cgi) {
	    cgiMode();
	} else {
	    socketMode();
	}
	buttonPanel.add(commMode);

	pn("disable buttons = " + jsValues.session.disableButtons);
	if (! jsValues.session.disableButtons)
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

	//
	// Determine the screen resolution.
	//
        Toolkit kit = Toolkit.getDefaultToolkit();
	jsValues.uiglobals.screenRes = kit.getScreenResolution();

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
			restartSession();
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
			showCollab();
                    }
                });	

        playbackButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			setLogFile();
                    }
                });	

        logButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			setLog();
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

            dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " +
	      jsValues.uiglobals.screenSize.width + " " +
	      jsValues.uiglobals.screenSize.height + " " +
	      jsValues.uiglobals.screenRes + " " +
	      jsValues.uiglobals.screenRes + "\n" +
	      DEViseCommands.OPEN_SESSION + " {" + currentDir + "/" +
	      currentSession + "}");
        }

	if (jv.session.autoPlayback) {
	    logFileName = jv.session.clientLogName;
	    isOriginal = jv.session.playbackOriginal;
	    isDisplay = jv.session.playbackDisplay;
	    logPlayBack();
	}
    } // end of constructor

    public void refreshAllData(boolean doHome)
    {
	String cmd = DEViseCommands.REFRESH_DATA;
	cmd += doHome ? " 1" : " 0";
        dispatcher.start(cmd);
    }

    public void restartSession()
    {
        if (!isSessionOpened) {
	    showMsg("You do not have any opened session!");
	    return;
	}
	dispatcher.start(DEViseCommands.OPEN_SESSION + " {" +
	  currentDir + "/" + currentSession + "}");
    }

    public void hideDebug() {
	if (debugWindow != null) {
            debugWindow.setVisible(false);
	}
    }

    public void showDebug() {
	if (debugWindow != null) {
            debugWindow.setVisible(true);
	}
    }

    // print out message to debug window
    // pn() methods add a newline to the end of the string; p() methods don't.
    public void pn(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0 && debugWindow != null) {
            debugWindow.pn(msg, level);
        } else 
	    msgBuffer.addElement(msg);
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (jsValues.debug._debugLevel > 0 && debugWindow != null) {
            debugWindow.p(msg, level);
        } else
	    msgBuffer.addElement(msg);
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void setLog()
    {
	String label = logButton.getLabel();
	
	if (label.equals(displayLogStr)) {
	    jsValues.debug._debugLevel = 1;
	    debugWindow = new YLogGUI(jsValues.debug._debugLevel);
	    showDebug();
	
	    for (int i = 0; i < msgBuffer.size(); i++) {
		String msg = (String)msgBuffer.elementAt(i);
		pn(msg);
	    }
	    
	    msgBuffer.removeAllElements();
	    logButton.setLabel(closeLogStr);
	
	} else {
	    jsValues.debug._debugLevel = 0;
	    hideDebug();
	    debugWindow = null;
	    logButton.setLabel(displayLogStr);
	}

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
	try {
            collabIdDlg = new CollabIdDlg(this, parentFrame,
	      isCenterScreen, msg);
            collabIdDlg.open();
	} catch (YException ex) {
	    System.err.println(ex.getMessage());
	}
	collabIdDlg = null;
    }

    public void showCollabPass()
    {
        collabpassdlg = new CollabPassDlg(this, parentFrame, isCenterScreen);
        collabpassdlg.open();
	collabpassdlg = null;
    }

    public void displayID()
    {
	String idstr = new Integer(jsValues.connection.connectionID).toString();
	showMsg("My client ID is:  " + idstr);
    }

    public void disableCollab()
    {
	String command = new String();
	command = DEViseCommands.DISABLE_COLLAB;
	dispatcher.start(command);
    }

    public void enterCollabPass()
    {
        entercollabpassdlg = new EnterCollabPassDlg(this, parentFrame, isCenterScreen);
        entercollabpassdlg.open();
	entercollabpassdlg = null;
    }

    public void showCollabState(String msg)
    {
        collabstatedlg = new CollabStateDlg(this, parentFrame, isCenterScreen, msg);
        collabstatedlg.open();
	collabstatedlg = null;        
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

    public void setCgiUrl()
    {
        setcgiurldlg = new SetCgiUrlDlg(this, parentFrame, isCenterScreen);
        setcgiurldlg.open();
        setcgiurldlg = null;
    }

    // set the logfile name for command log playback
    public void setLogFile()
    {
        setlogfiledlg = new SetLogFileDlg(this, parentFrame, isCenterScreen);
        setlogfiledlg.open();
        setlogfiledlg = null;
    }

    // command log playback
    public void logPlayBack()
    {
	playback = new DEVisePlayback(this, dispatcher, logFileName);
    }


    public void setMode()
    {
        setmodedlg = new SetModeDlg(this, parentFrame, isCenterScreen);
        setmodedlg.open();
        setmodedlg = null;
    }

    public void showCollab()
    {
        collabSelectDlg = new CollabSelectDlg(this, parentFrame, isCenterScreen);
        collabSelectDlg.open();
        collabSelectDlg = null;
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
    // (We get here if the "Exit" button is pressed.)
    public synchronized void checkQuit()
    {
        boolean reallyQuit = true;

	if (specialID == -1) { 
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
            destroy();
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
	
	// Note: this must be done before destroying the dispatcher;
	// otherwise we get null pointer errors because we get window
	// events after the dispatcher has been destroyed.
	parentFrame.dispose();

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

        if (!jsValues.uiglobals.isApplet) {
	    System.exit(0);
	}
    }
    
    public void socketMode()
    {
	commMode.setForeground(Color.white);
        commMode.setText("Socket");
    }

    public void collabMode()
    {
	commMode.setForeground(Color.white);
        commMode.setText("Collaboration");
    }

    public void cgiMode()
    {
	commMode.setForeground(Color.white);
        commMode.setText("CGI");
    }

    public void disconnectedMode()
    {
	commMode.setForeground(Color.red);
        commMode.setText("Disconnected");
    }

    public void playbackMode()
    {
	commMode.setForeground(Color.white);
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
        // specialID = leaderId;

	collabMode();
    }

    public synchronized void collabQuit()
    {
	pn("Quit from collaboration mode.");

	dispatcher.setStatus(0);
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
        int idx;
        if ((fileList.getItemCount() > 0) &&
          ((idx = fileList.getSelectedIndex()) != -1)) {
            sessionName = fileList.getItem(idx);

            if (sessionName.startsWith("[")) { // a directory
                String[] name = DEViseGlobals.parseString(sessionName,
		  '[', ']');
	        if (name[0].equals("..")) { // go up a directory
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
	            jsc.currentDir = jsc.currentDir + "/" + name[0];
	        }

                directory.setText("/" + jsc.currentDir);
	        validate();
	        jsc.dispatcher.start(DEViseCommands.GET_SESSION_LIST + " {" +
	          jsc.currentDir + "}");
            } else { // a file
                jsc.currentSession = sessionName;
	        jsc.dispatcher.start(DEViseCommands.SET_DISPLAY_SIZE + " " +
	          jsc.jsValues.uiglobals.screenSize.width + " " +
	          jsc.jsValues.uiglobals.screenSize.height + " " +
	          jsc.jsValues.uiglobals.screenRes + " " +
	          jsc.jsValues.uiglobals.screenRes + "\n" +
	          DEViseCommands.OPEN_SESSION + " {" +
	          jsc.currentDir + "/" + sessionName + "}");
                close();
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

    public TextField screenX = new TextField(4);
    public TextField screenY = new TextField(4);
    public Button setButton = new Button("   Set   ");
    public Button statButton = new Button("Request");
    public Button meButton = new Button("Request");
    public Button collabButton = new Button("Request");
    public Button cancelButton = new Button("Cancel");
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

        meButton.setBackground(jsc.jsValues.uiglobals.bg);
        meButton.setForeground(jsc.jsValues.uiglobals.fg);
        meButton.setFont(jsc.jsValues.uiglobals.font);

	if (jsc.specialID == -1) {
	    collabButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    collabButton.setBackground(Color.red);
	}
	    
        collabButton.setForeground(jsc.jsValues.uiglobals.fg);
        collabButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

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

        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label3 = new Label("JSPOP Status:");
        gridbag.setConstraints(label3, c);
        add(label3);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(statButton, c);
        add(statButton);

        c.insets = new Insets(10, 10, 10, 0);
        c.gridwidth = 1;
        Label label5 = new Label("My ID:");
        gridbag.setConstraints(label5, c);
        add(label5);

        c.insets = new Insets(10, 0, 10, 10);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(meButton, c);
        add(meButton);

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
    private Button okButton = new Button("   OK   ");

    private boolean status = false; // true means this dialog is showing

    public ServerStateDlg(Frame owner, boolean isCenterScreen, String data, jsdevisec what)
    {
        super(owner, true);

	jsc = what;

	jsc.jsValues.debug.log("Creating ServerStateDlg");

	String[] list = DEViseGlobals.parseString(data);
	String[] list1 = null, list2 = null, list3 = null;
	
	if (list != null && list.length > 3) {
            try {
                int number1 = Integer.parseInt(list[0]);
                if (number1 != 0) {
                    list1 = new String[number1];
		    String[] temp_list = DEViseGlobals.parseStr(list[1], " ");
                    for (int i = 0; i < number1; i++) {
                        list1[i] = temp_list[2*i] + " " + temp_list[2*i+1];
                    }
                }

                int number2 = Integer.parseInt(list[3]);
                if (number2 != 0) {
                    list2 = new String[number2];
		    String[] temp_list = DEViseGlobals.parseStr(list[4], " ");
                    for (int i = 0; i < number2; i++) {
                        list2[i] = temp_list[2*i] + " " + temp_list[2*i+1];
                    }
                }

                int number3 = Integer.parseInt(list[5]);
                if (number3 != 0) {
                    list3 = new String[number3];
		    for (int i = 0; i < number3; i++) {
			String[] temp_list = DEViseGlobals.parseStr(list[6+i], " ");
                        list3[i] = temp_list[0] + " " + temp_list[1];
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


// Dialog for setting logfile name for playback.
class SetLogFileDlg extends Dialog
{
    jsdevisec jsc = null;
    public TextField file = new TextField(30);
    public Checkbox display = new Checkbox("Is Display", true);
    public Checkbox original = new Checkbox("Original Rate", true);
    public Button setButton = new Button("   Set   ");
    public Button cancelButton = new Button("  Cancel ");    
    private boolean status = false; // true means this dialog is showing

    public SetLogFileDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating SetLogFileDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting Logfile Name");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

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

        gridbag.setConstraints(panel1, c);
        add(panel1);

        Button [] button = new Button[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel2 = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);

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
			jsc.isDisplay = display.getState();
			jsc.isOriginal = original.getState();
			close();
			jsc.logPlayBack();
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
	jsc.jsValues.debug.log("Opening SetLogFileDlg");
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
	jsc.jsValues.debug.log("Closed SetLogFileDlg");
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
    public Button socketButton = new Button("Socket");
    public Button cgiButton = new Button("CGI");
    public Button cancelButton = new Button("Cancel");
    private boolean status = false; // true means this dialog is showing

    public SetModeDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating SetModeDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting JavaScreen Mode");

        socketButton.setBackground(jsc.jsValues.uiglobals.bg);
        socketButton.setForeground(jsc.jsValues.uiglobals.fg);
        socketButton.setFont(jsc.jsValues.uiglobals.font);

	if (jsc.specialID == -1) {
	    cgiButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    cgiButton.setBackground(Color.red);
	}
        cgiButton.setForeground(jsc.jsValues.uiglobals.fg);
        cgiButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

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

	if (jsc.specialID == -1) {
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
	}

        socketButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
			jsc.jsValues.connection.cgi = false;
			jsc.socketMode();
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
	jsc.jsValues.debug.log("Opening SetModeDlg");
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

// Dialog to select collaboration state.
class CollabSelectDlg extends Dialog
{
    jsdevisec jsc = null;

    public Button collabButton = new Button("Start Collaboration");
    public Button endButton = new Button("Finish Collaboration");
    public Button enCollabButton = new Button("Enable Collaboration");
    public Button disCollabButton = new Button("Disable Collaboration");
    public Button cancelButton = new Button("Cancel");
    private boolean status = false; // true means this dialog is showing

    public CollabSelectDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating CollabSelectDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("JavaScreen Collaboration");

	if (jsc.specialID == -1) {
	    collabButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    collabButton.setBackground(Color.red);
	}
        collabButton.setForeground(jsc.jsValues.uiglobals.fg);
        collabButton.setFont(jsc.jsValues.uiglobals.font);

	if (jsc.specialID != -1) {
	    endButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    endButton.setBackground(Color.red);
	}
        endButton.setForeground(jsc.jsValues.uiglobals.fg);
        endButton.setFont(jsc.jsValues.uiglobals.font);

	if ((jsc.specialID == -1) && (!jsc.isCollab)) {
	    enCollabButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    enCollabButton.setBackground(Color.red);
	}
        enCollabButton.setForeground(jsc.jsValues.uiglobals.fg);
        enCollabButton.setFont(jsc.jsValues.uiglobals.font);

	if ((jsc.specialID == -1) && (jsc.isCollab)) {
	    disCollabButton.setBackground(jsc.jsValues.uiglobals.bg);
	} else {
	    disCollabButton.setBackground(Color.red);
	}
        disCollabButton.setForeground(jsc.jsValues.uiglobals.fg);
        disCollabButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

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

	if (jsc.specialID == -1) {
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
			jsc.dispatcher.setStatus(0);
			
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
			jsc.isCollab = true;
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
			jsc.isCollab = false;
			close();
			jsc.disableCollab();
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
class CollabIdDlg extends Frame
{
    private jsdevisec jsc = null;

    private java.awt.List clientList = null;
    private Label label = new Label("Current active clients: ");
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    private String[] clients = null;
    private boolean emptyList = false;

    private boolean status = false; // true means this dialog is showing

    public CollabIdDlg(jsdevisec what, Frame owner, boolean isCenterScreen,
      String data) throws YException
    {
	what.jsValues.debug.log("Creating CollabIdDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Collaboration JavaScreen Dialog");

        label.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));

        clientList = new java.awt.List(8, false);
        clientList.setBackground(jsc.jsValues.uiglobals.textBg);
        clientList.setForeground(jsc.jsValues.uiglobals.textFg);
        clientList.setFont(jsc.jsValues.uiglobals.textFont);

        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
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
	            jsc.dispatcher.setStatus(0);
		    jsc.restorePreCollab();
                }
            });

    }

    public void setClientList(String data) throws YException
    {
      	clients = DEViseGlobals.parseString(data);	

        //clientList.removeAll();

        for (int i = 1; i <= (clients.length-1)/3; i++) {
	    String list = new String(); 
	    list = clients[(i-1)*3+1] + clients[(i-1)*3+2] 
		   + clients[(i-1)*3+3];
	    clientList.add(list);
	}

	if (clientList.getItemCount() <= 0) {
	    // jsc.showMsg("No available JavaScreen for collaboration.");
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
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();
            status = false;
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

// Dialog for setting password for collaboration.
class CollabPassDlg extends Dialog
{
    jsdevisec jsc = null;

    public TextField pass = new TextField(20);
    public Button setButton = new Button("Set");
    public Button cancelButton = new Button("Cancel");
    private boolean status = false; // true means this dialog is showing

    public CollabPassDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating CollabPassDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Setting Collaboration Password");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

        cancelButton.setBackground(jsc.jsValues.uiglobals.bg);
        cancelButton.setForeground(jsc.jsValues.uiglobals.fg);
        cancelButton.setFont(jsc.jsValues.uiglobals.font);

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

	pass.setText(jsc.collabPass);

        Button [] button = new Button[2];
        button[0] = setButton;
        button[1] = cancelButton;
        DEViseComponentPanel panel = new DEViseComponentPanel(button,
	  DEViseComponentPanel.LAYOUT_HORIZONTAL, 20, jsc);


        c.gridwidth = 1;
        Label label = new Label("Set Collaboration Password:");
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

			String command = new String();
			command = DEViseCommands.SET_COLLAB_PASS + " {" + jsc.collabPass + "}";
			jsc.dispatcher.start(command);
			
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
	jsc.jsValues.debug.log("Closed CollabPassDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}

// ------------------------------------------------------------------------

// Dialog for setting password for collaboration.
class EnterCollabPassDlg extends Dialog
{
    jsdevisec jsc = null;

    public TextField pass = new TextField(20);
    public Button setButton = new Button("Set");
    //public Button cancelButton = new Button("Cancel");
    private boolean status = false; // true means this dialog is showing

    public EnterCollabPassDlg(jsdevisec what, Frame owner, boolean isCenterScreen)
    {
        super(owner, true);

	what.jsValues.debug.log("Creating EnterCollabPassDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Reading Collaboration Password");

        setButton.setBackground(jsc.jsValues.uiglobals.bg);
        setButton.setForeground(jsc.jsValues.uiglobals.fg);
        setButton.setFont(jsc.jsValues.uiglobals.font);

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

	pass.setText(DEViseGlobals.DEFAULTPASS);

        Button [] button = new Button[1];
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
			String command = DEViseCommands.COLLABORATE + " {" + 
			    id + "} {" + jsc.collabPass + "}";

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
class CollabStateDlg extends Frame
{
    private jsdevisec jsc = null;

    private java.awt.List collabList = null;
    private Label label = new Label("Current collaborating followers: ");
    private Button closeButton = new Button("Close");
    private String[] followers = null;

    private boolean status = false; // true means this dialog is showing

    public CollabStateDlg(jsdevisec what, Frame owner, boolean isCenterScreen, String data)
    {
	what.jsValues.debug.log("Creating CollabStateDlg");

        jsc = what;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        setTitle("Collaboration State Dialog");

        label.setFont(DEViseFonts.getFont(16, DEViseFonts.SERIF, 1, 0));

        collabList = new java.awt.List(8, false);
        collabList.setBackground(jsc.jsValues.uiglobals.textBg);
        collabList.setForeground(jsc.jsValues.uiglobals.textFg);
        collabList.setFont(jsc.jsValues.uiglobals.textFont);

        Button [] button = new Button[1];
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
            jsc.collabIdDlg = null;
        }
	jsc.jsValues.debug.log("Closed CollabStateDlg");
    }

    // true means this dialog is showing
    public synchronized boolean getStatus()
    {
        return status;
    }
}
