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

// Command dispatcher.  This class sends commands from the JavaScreen
// to the jspop/devised, and processes the commands that are sent
// back.

// There is one instance of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.106  2001/10/30 17:20:13  xuk
// Created DEViseClient object for collaborating clients in jspop.
// 1. Modified sockSendCmd(), no difference between normal and collaborating modes;
// 2. Modified start(), no difference between normal and collaborating modes;
// 	                now JAVAC_Connect command has no collab_passwd argument;
// 			enable heartbeat in collaboration mode;
// 3. Modified run(), in collaboration mode, break loop when specialID == 1;
// 4. Modified sendRcvCommand(), in collaboration mode, when current thread is interrupted, sends JAVAC_CollabExit command to jspop;
//
// Revision 1.105  2001/10/25 21:35:41  wenger
// Added heartbeat count to heartbeat command (for debugging); other minor
// cleanup and debug code additions.
//
// Revision 1.104  2001/10/22 18:38:23  wenger
// A few more cleanups to the previous fix.
//
// Revision 1.103  2001/10/17 15:54:24  wenger
// Fixed bug 712 (JS collaboration doesn't work); various other code
// cleanup.
//
// Revision 1.102  2001/10/16 22:14:27  wenger
// Major cleanup of command playback code; fixed bug 711 (problem with
// command log playback).
//
// Revision 1.101  2001/10/12 02:07:21  xuk
// Using timestamp-based client ID.
// In user(), id has been expanded to long.
//
// Revision 1.100  2001/10/05 19:26:33  xuk
// Fixed bug 698: manipulating 3D command playback.
//
// Revision 1.99  2001/09/28 18:54:56  xuk
// JavaScreen command log playback.
// Modified run() mothod to deal with playback.
//
// Revision 1.98  2001/09/27 19:52:25  wenger
// Fixed bug 688 (problem dealing with links in session directories);
// improved JS error handling for session open; eliminated a bunch of
// duplicate code in the JS.
//
// Revision 1.97  2001/09/10 21:15:48  xuk
// Solve the client disconnection problem.
// Added reconnect() method to keep trying to reestablish the connection to the JSPoP every 10 seconds.
// In sockSendCmd() and sendRcvCmd() methods check whether the connection is disabled.
//
// Revision 1.96  2001/05/11 20:36:05  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.95  2001/05/03 16:24:36  xuk
// Added multiply factor for displaying mouse postion.
// Changed viewDataArea() function to parse factor argument.
//
// Revision 1.94  2001/04/25 19:41:43  xuk
// Fixed bug 661: when a JS goes into Collaboration mode from Socket mode,
// It will not become an available collaboration leader.
//
// Revision 1.93  2001/04/24 01:46:18  xuk
// Fixed bug 658: restoring pre-collaboration session, when leader exits.
//
// Revision 1.92  2001/04/12 15:29:25  xuk
// When collaboration leader exits, followers don't exit; instead they go back to normal mode.
//
// Revision 1.91  2001/04/11 21:16:29  xuk
// A collaboration leader could find out the followers hostname.
//
// Revision 1.90  2001/04/06 19:32:14  wenger
// Various cleanups of collaboration code (working on strange hang
// that Miron has seen); added more debug output; turned heartbeat
// back on (it somehow got turned off by accident).
//
// Revision 1.89  2001/04/05 16:12:35  xuk
// Fixed bugs for JSPoP status query.
// Only process JAVAC_UpdataSeverState command for normal JS.
//
// Revision 1.88  2001/04/01 03:51:17  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.87  2001/03/20 20:11:38  wenger
// Added more debug output to the JS client and jspop.
//
// Revision 1.86  2001/03/20 17:49:45  xuk
// Added collaboration for 3D Views.
//
// Revision 1.85  2001/03/19 23:10:31  xuk
// Fixed bug for no available JavaScreen for collaboration.
//
// Revision 1.84  2001/03/09 20:24:36  wenger
// Merged changes from no_collab_br_3 thru no_collab_br_4 from the branch
// to the trunk; updated linux and solaris dependencies.
//
// Revision 1.83  2001/03/08 21:10:12  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.82  2001/03/05 16:46:48  xuk
// Clear follower's JavaScreen when leader close session.
// Changes in processReceivedCommand() ro process JAVAC_CloseSession Command.
//
// Revision 1.81  2001/03/03 20:12:07  xuk
// Restore old state if user goes into, then out of, collaboration mode.
//
// Revision 1.80  2001/02/22 17:11:56  xuk
// Close collaboration JSs when "leader" JS exits.
// In processReceivedCommand(), process JAVAC_CollabExit command.
// Don't display "open session" dialogue for collaboration JSs.
// In processReceivedCommand(), changes for JAVAC_UpdateSessionList command.
//
// Revision 1.79  2001/02/21 17:50:36  xuk
// Added the collaboration security features.
// Changes in run() for setting collaboration password in JAVAC_Connect command.
//
// Revision 1.78  2001/02/20 20:02:21  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.77  2001/02/19 20:33:44  xuk
// Added command(s) and GUI so that a collaboration leader can find out who is
// collaborating with it.
// In processReceivedCommand(), process JAVAC_COLLAB_STATE command.
//
// Revision 1.76  2001/02/16 17:53:55  xuk
// Added new command and GUI for collect active client ID list.
// Changed processReceivedCmd() to process new command JAVAC_Clients;
// Changed run() for the first round collaboration connection, the thread will not loop forever.
//
// Revision 1.75  2001/02/15 03:21:35  xuk
// Fixed bugs for collaboration JavaScreen.
//
// Revision 1.74  2001/02/12 02:51:12  xuk
// JavaScreen can prevent from being collaborated.
// Changes in run(), adding one collaboration status field for Java_Connect Command
//
// Revision 1.73  2001/02/11 20:53:16  xuk
// Made the JS can switch between collaboration and socket modes.
//
// Revision 1.72  2001/01/31 22:23:55  xuk
// Modify processReceivedCommand(), for wrong collaboration JS ID. Stop current thread when receives JAVAC_ERROR command from jspop.
//
// Revision 1.71  2001/01/30 03:03:28  xuk
// Add collabration function. Mainly changes are in run(), socketSendCom(), destroy().
//
// Revision 1.70.2.10  2001/03/09 17:20:12  wenger
// Fixed bug 643 (problem with JAVAC_Exit command in CGI mode).
//
// Revision 1.70.2.9  2001/03/08 21:23:23  wenger
// Added note about socket closing.
//
// Revision 1.70.2.8  2001/02/23 19:56:41  wenger
// Moved the details of client-side CGI communication into its own class;
// combined separate socket and CGI code that largely duplicated each other.
//
// Revision 1.70.2.7  2001/02/20 15:55:35  wenger
// JavaScreen client now checks the availability of a CGI connection
// before prompting the user, if opening a socket fails.
//
// Revision 1.70.2.6  2001/02/19 19:58:56  wenger
// Found and fixed bug 642.
//
// Revision 1.70.2.5  2001/02/05 22:36:29  wenger
// Changed all socket recieve timeouts to 5 sec. because of problems on
// SGIs at BMRB.
//
// Revision 1.70.2.4  2001/02/05 22:02:09  wenger
// Fixed bugs 639 and 640 and other problems associated with destroying
// and re-starting the JavaScreen applets.
//
// Revision 1.70.2.3  2001/02/05 17:13:14  wenger
// Finally implemented a way to timeout on the Socket constructor that
// works in Netscape (wrote my own version of Thread.join() because
// Netscape's doesn't work right).
//
// Revision 1.70.2.2  2001/02/01 18:38:07  wenger
// Turned the JavaScreen heartbeat back on.
//
// Revision 1.70.2.1  2001/01/31 21:02:39  wenger
// Added timeout to socket creation in the client (currently set to 5
// sec.); improved GUI for socket failure.
//
// Revision 1.70  2001/01/23 22:58:16  xuk
// fix bugs for mode switch.
//
// Revision 1.69  2001/01/23 04:06:43  xuk
// In start() funtion, close socket after switching to cgi mode.
//
// Revision 1.68  2001/01/08 20:31:50  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.65.4.34  2001/01/05 19:15:44  wenger
// Updated copyright dates.
//
// Revision 1.65.4.33  2001/01/04 21:15:19  wenger
// Added socket/CGI communication mode indicator.
//
// Revision 1.65.4.32  2000/12/29 22:41:28  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.65.4.31  2000/12/22 17:26:43  wenger
// Implemented kludgey fix for bug 630.
//
// Revision 1.65.4.30  2000/12/22 16:29:06  wenger
// Fixed bug 629; temporarily disabled heartbeat pending fix for bug 627.
//
// Revision 1.65.4.29  2000/12/21 23:28:26  wenger
// Got CGI mode working with applet, minor cleanup still needed; more debug
// output in DEViseCommSocket, jspop, and js.cgi; comments updated for
// new command format that always includes ID and CGI flag.
//
// Revision 1.65.4.28  2000/12/19 22:50:30  wenger
// Fixed CGI code for current command format (always sending ID and CGI flag).
//
// Revision 1.65.4.27  2000/12/07 17:28:18  xuk
// Sending command ID and cgi flag with every command.
//
// Revision 1.65.4.26  2000/11/22 22:00:34  wenger
// Made some small cleanups to DEViseCmdDispatcher.java.
//
// Revision 1.65.4.25  2000/11/22 17:43:57  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.65.4.24  2000/11/21 20:58:19  xuk
// *** empty log message ***
//
// Revision 1.65.4.23  2000/11/21 20:55:34  xuk
// Include ID and cgiFlag in every command sent out.
//
// Revision 1.65.4.22  2000/11/21 01:51:31  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.65.4.21  2000/11/16 15:19:23  wenger
// Temporarily turned off heartbeat from the JavaScreen because of client
// switching.
//
// Revision 1.65.4.20  2000/11/15 18:43:36  wenger
// Re-did formatting fix.
//
// Revision 1.65.4.19  2000/11/15 04:48:23  xuk
// Clean up the mess caused by last cvs update
//
// Revision 1.65.4.18  2000/11/10 17:24:10  wenger
// Cleaned up format of run() method.
//
// Revision 1.65.4.17  2000/11/10 16:19:41  wenger
// JavaScreen sends message type (API_JAVA_WID) to cgi script.
//
// Revision 1.65.4.16  2000/11/10 15:50:27  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.65.4.15  2000/11/09 21:43:12  wenger
// js.cgi now uses CGI library, various other cleanups; changed CGI parameter
// names slightly; added -cgi argument to JS usage message.
//
// Revision 1.65.4.14  2000/11/09 16:52:56  xuk
// In cgi version, sends cmdport to cgi script.
//
// Revision 1.65.4.13  2000/11/08 20:23:48  wenger
// Improved formatting of cgi_sendCommand(), cgi_receiveCmd(), and
// cgi_receiveData().
//
// Revision 1.65.4.12  2000/11/08 19:13:51  wenger
// Fixed bug 600 (JS exits even if you click No); fixed crash if you click
// Exit in JS before connecting to a jspop.
//
// Revision 1.65.4.11  2000/11/02 23:44:47  xuk
// Add cgi stuffs.
// Add new functions for cgi communication:
// cgi_processCmd(), cgi_sendCommand(), cgi_receiveCmd(), cgi_receiveData().
//
// Revision 1.65.4.10  2000/10/25 03:08:53  xuk
// 1. Do not close socket after the communication of each command. Set connect in start(), not in socketSendcom(). delete disconnect() in run() and processCmd().
// 2. in socketSendCom(), only send ID in Java_Connect command.
//
// Revision 1.65.4.9  2000/10/23 19:59:11  xuk
// Get rid of DEViseCmdDispathcer.ID. Using DEViseGlobals.connectionID instead.
//
// Revision 1.65.4.8  2000/10/18 20:28:10  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.67  2000/09/13 17:38:58  wenger
// Cleaned up command parsing code.
//
// Revision 1.66  2000/09/12 20:51:22  wenger
// Did some cleanup of the command-related code, better error messages from JSS.
//
// Revision 1.65.4.7  2000/10/18 18:29:22  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.65.4.6  2000/10/16 03:52:40  xuk
// *** empty log message ***
//
// Revision 1.65.4.5  2000/10/11 02:58:25  xuk
// Add wait() in the while loop of run() function. Not occupy CPU.
//
// Revision 1.65.4.4  2000/10/10 04:48:19  xuk
// Fix the bugs of Stop button color and gears in the upper left corner: jsc.animPanel.start() and jsc.stopButton.setBackground(Color.red) in run() function.
//
// Revision 1.65.4.3  2000/10/09 16:29:43  xuk
// 1. Make the thread running for ever. Add a while(true) loop in run() function.
// 2. Send Java_HeartBeat command every minute in run() function.
//
// Revision 1.65.4.2  2000/10/02 19:11:54  xuk
// Add socketSendCom() function to set up a new socket between JS and J
// SPoP whenever sending out a command;
// Also disconnect the socket after the communication for each command.
// After sending out the "JAVAC_CONNECT" command, receive the ID from JSPoP in sendCommand().
//
// Revision 1.65.4.1  2000/09/01 20:07:22  xuk
// In connect() function, only provide one port to DEViseCommSocket constructor.
//
// Revision 1.65  2000/07/20 22:38:26  venkatan
// Mouse Location Format display:
// 1. Both X and Y axis formats are recognised.
// 2. "-" is recognised for "" String.
// 3. %.0f is now recognised.
//
// Revision 1.64  2000/07/20 16:26:06  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.63  2000/07/20 15:42:59  wenger
// Fixed bug 603 (GData errors caused by problems in new parser); eliminated
// old parser.
//
// Revision 1.62  2000/07/19 20:11:36  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.61  2000/07/14 21:13:07  wenger
// Speeded up 3D GData processing by a factor of 2-3: improved the parser
// used for GData; eliminated Z sorting for bonds-only 3D views; eliminated
// DEViseAtomTypes for atoms used only to define bond ends; reduced string-
// based processing; eliminated various unused variables, etc.
//
// Revision 1.60  2000/07/11 16:39:18  venkatan
// *** empty log message ***
//
// Revision 1.59  2000/07/10 12:26:02  venkatan
// *** empty log message ***
//
// Revision 1.58  2000/06/26 16:48:31  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.57  2000/06/12 22:13:55  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.56  2000/05/11 20:19:33  wenger
// Cleaned up jsdevisec.java and added comments; eliminated
// jsdevisec.lastCursor (not really needed).
//
// Revision 1.55  2000/05/04 15:53:33  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.54  2000/04/27 20:15:24  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.53  2000/04/27 15:56:54  wenger
// Added some comments and requests for comments.
//
// Revision 1.52  2000/04/24 20:21:59  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.51  2000/04/05 15:42:22  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.50  2000/04/05 06:25:38  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.49  2000/04/03 21:21:22  wenger
// When new GData arrives for a view, we remove the old GData from the
// view and call the garbage collector before constructing the new
// GData objects, so that the old GData objects are at least hopefully
// destroyed before the new ones are created.
//
// Revision 1.48  2000/04/03 05:29:35  hongyu
// *** empty log message ***
//
// Revision 1.47  2000/03/31 19:29:04  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.46  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.45  2000/02/23 21:12:12  hongyu
// *** empty log message ***
//
// Revision 1.44  2000/02/16 15:15:31  wenger
// Temporary change so that the JavaScreen accepts the current form
// of the JAVAC_DrawCursor command.
//
// Revision 1.43  2000/02/16 08:53:57  hongyu
// *** empty log message ***
//
// Revision 1.42  1999/12/10 15:30:12  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.41  1999/10/10 08:49:51  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.40  1999/09/24 17:11:46  hongyu
// adding support for 3-d molecule view
//
// Revision 1.39  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/08/17 06:15:16  hongyu
// *** empty log message ***
//
// Revision 1.37  1999/08/03 05:56:48  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.36  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.35  1999/06/23 20:59:15  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCmdDispatcher.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    private static final int SOCK_REC_TIMEOUT = 5000; // milliseconds
    private static final int SOCK_CONST_TIMEOUT = 5000; // milliseconds

    private jsdevisec jsc = null;

    public Thread dispatcherThread = null;

    private String[] commands = null;

    public DEViseCommSocket commSocket = null;

    private DEViseHeartbeat _heartbeat = null;

    public String errMsg = null;

    // status = 0, dispatcher is not running
    // status = 1, dispatcher is running
    private int status = 0;

    // isOnline = true, successfully established connection to server, i.e.
    //                  get a valid connection ID
    // isOnline = false, connection to server is not established
    public boolean isOnline = false;

    // ADD COMMENT -- what does this mean?
    private boolean isAbort = false;

    private DEViseCommCgi _commCgi = null;

    private static final boolean _debug = false;

    //TEMP -- we shouldn't really need this *and* isOnline, but I don't
    // want to sort that out right now.  RKW 2000-10-18.
    public boolean _connectedAlready = false;

    public DEViseCmdDispatcher(jsdevisec what)
    {
        jsc = what;
    }

    // This must *not* be a synchronized method, to avoid deadlock between
    // the GUI and command-processing threads (via
    // DEViseCanvas.checkMousePos().
    public int getStatus()
    {
        return status;
    }

    public synchronized void setStatus(int arg)
    {
        status = arg;
        jsc.jscreen.reEvaluateMousePosition();
    }

    // Changed this to not check whether we have a socket, since we may
    // now have a "virtual" connection even if a socket isn't open.
    // RKW 2000-10-18.
    // Note: this must *not* be synchronized (fixes bug 642).  RKW 2001-02-19.
    public boolean getOnlineStatus()
    {
        return isOnline;
    }

    private synchronized void setOnlineStatus(boolean flag)
    {
        isOnline = flag;
    }

    public synchronized boolean getAbortStatus()
    {
        return isAbort;
    }

    public synchronized void setAbortStatus(boolean flag)
    {
        isAbort = flag;
    }

    // it is assumed that status = 0 while method start() is called
    // it is also assumed that while status = 0, dispatcher thread is not running

    // Initiate a command from the JavaScreen to the jspop or devised.
    // Note that cmd may actually contain more than one command (separated
    // by newlines).
    public synchronized void start(String cmd)
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.start(" + cmd + ")");
        }

        if (getStatus() != 0) {
            if (!cmd.startsWith(DEViseCommands.HEART_BEAT))
		jsc.showMsg("JavaScreen is busy working\nPlease try again later");
            return;
        }
	
        setStatus(1);
        jsc.animPanel.start();
        jsc.stopButton.setBackground(Color.red);
        jsc.stopNumber = 0;

	if (jsc.jsValues.connection.cgi == false) { // not using cgi, default case
	    // If we don't have a socket to the jspop, attempt to create one.
	    if (commSocket == null) {
		boolean isEnd = false;
		while (!isEnd) {
		    if (!connect()) {
			if (testCgi()) {
			    jsc.showMsg("Direct socket " +
			      "connection is not available (may be blocked " +
			      "by firewall).  Continuing in CGI mode.");
			    jsc.cgiMode();
			    jsc.jsValues.connection.cgi = true;
			    isEnd = true;
			} else {
			    jsc.showMsg("Connection to JSPoP is not " +
			      "currently available");
			    jsc.animPanel.stop();
			    jsc.stopButton.setBackground(
			      jsc.jsValues.uiglobals.bg);
			    setStatus(0);
			    return;
			}
		    } else {
			isEnd = true;
		    }
		}
	    }
	} else { // in cgi mode, close the socket
	    //TEMPTEMP -- is this the right place to do this?  does
	    // the socket get closed after replies are received, or isn't
	    // it closed until the next command is sent????
	    if (commSocket != null) {
		commSocket.closeSocket();
		commSocket = null;
		jsc.pn("Socket has been closed.");
	    }	
	}	

	// for collabration JavaScreen	
	if (jsc.specialID != -1) {
	    jsc.pn("We entered one collabration JavaScreen.");
        }

	// If we don't have a connection yet, prepend a connection request
	// command to whatever was passed in.

	if (!_connectedAlready) {
	    cmd = DEViseCommands.CONNECT + " {" +
		jsc.jsValues.connection.username + "} {" +
		jsc.jsValues.connection.password + "} {" +
		DEViseGlobals.PROTOCOL_VERSION + "}\n" + cmd; 
	    
	    _connectedAlready = true;

	    // for JS switched back from collaboration
	    if ((jsc.specialID == -1) && (jsc.sessionSaved)) {
		jsc.isSessionOpened = true;
		jsc.sessionSaved = false;
		cmd = cmd + "\n" + DEViseCommands.REOPEN_SESSION;
	    }

	    // Start the heartbeat thread.
	    // if (jsc.specialID == -1) {
	    _heartbeat = new DEViseHeartbeat(this);
	    // }
        }

	commands = DEViseGlobals.parseStr(cmd);
	if (commands == null || commands.length == 0) {
	    jsc.showMsg("Invalid command: \"" + cmd + "\"");
	    jsc.animPanel.stop();
	    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
	    setStatus(0);
	    return;
	}

	jsc.jscreen.setLastAction(cmd);

	// Note: command(s) will actually be sent by the run() method
	// of this class.
        dispatcherThread = new Thread(this);
        dispatcherThread.start();
    }

    public void stop()
    {
        stop(false);
    }

    // Note: this method doesn't really seem to do much, because it never
    // seems to get called with isDisconnect true.  RKW 2000-12-29.
    public void stop(boolean isDisconnect)
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.stop(" +
	      isDisconnect + ")");
        }

        if (isDisconnect) {
            if (getStatus() != 0) {
                String result = jsc.confirmMsg("Abort request already send to the server!\nAre you so impatient that you want to close the connection right away?");

                if (result.equals(YMsgBox.YIDNO)) {
                    return;
                } else {
                    if (dispatcherThread != null) {
                        dispatcherThread.stop();
                        dispatcherThread = null;
                    }

                    disconnect();

                    jsc.animPanel.stop();
                    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
                    jsc.jscreen.updateScreen(false);

                    setStatus(0);

                    return;
                }
            } else {
                return;
            }
        }

        if (getStatus() != 0) {
           setAbortStatus(true);
         }

	return;
    }

    // Note: this must *not* be synchronized (fixes bug 642).  RKW 2001-02-19.
    public void destroy()
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.destroy()");
        }

	//
	// Destroy the heartbeat thread -- we *must* make sure that no
	// heartbeats are sent after the JAVAC_Exit command is sent.
	//
	if (_heartbeat != null) {
            _heartbeat.stop();
            _heartbeat = null;
	}

	// send a Java_Collab_Exit if we are collaborating.
	if (jsc.specialID != -1) {
	    try {
                jsc.pn("Sending: \"" + DEViseCommands.COLLAB_EXIT +"\"");
		commSocket.sendCmd(DEViseCommands.COLLAB_EXIT, 
				   DEViseGlobals.API_JAVA, 
				   jsc.jsValues.connection.connectionID);
            } catch (YException e) {
                jsc.showMsg(e.getMsg());
            }
	}

	//
	// Send a JAVAC_Exit command if we're connected.
	//
        if (getOnlineStatus()) {
	    try {
                if (_debug) {
                    System.out.println("Sending JAVAC_Exit command in " +
		      "DEViseCmdDispatcher.destroy()");
		}

                jsc.pn("Sending: \"" + DEViseCommands.EXIT +"\"");
                sendCmd(DEViseCommands.EXIT);
            } catch (YException e) {
                jsc.showMsg(e.getMsg());
            }
        }

	//
	// Kill the dispatcher thread and disconnect.
	//
	if (getStatus() != 0 && dispatcherThread != null) {
	    dispatcherThread.stop();
	    dispatcherThread = null;
	}
        disconnect();

	//
	// Set the GUI state appropriately.
	//
        jsc.animPanel.stop();
	jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
	jsc.jscreen.updateScreen(false);

	//
	// Set the status appropriately.
	//
        if (getStatus() != 0) {
            setAbortStatus(true);
        }
	setStatus(0);
    }

    private synchronized boolean connect()
    {
        try {
            commSocket = new DEViseCommSocket(jsc.jsValues.connection.hostname,
	      jsc.jsValues.connection.cmdport, SOCK_REC_TIMEOUT,
	      SOCK_CONST_TIMEOUT);
	    jsc.pn("Socket connection set up!");
            return true;
        } catch (YException e) {
	    System.err.println(e.getMessage() + " (in " + e.getWhere() + ")");
            errMsg = e.getMessage();
	    jsc.pn("Socket connection turned down!");
            disconnect();
            return false;
        }
    }

    // for disconnected client
    private synchronized boolean reconnect()
    {
	/*
	try {
	    if (jsc.jsValues.connection.cgi) {
		String response = null;
		_commCgi = null;
		_commCgi = new DEViseCommCgi(jsc.jsValues);
		jsc.pn("Sending Check_PoP command...");
		_commCgi.sendCmd(DEViseCommands.CHECK_POP);

		Thread.sleep(3000);

		response = _commCgi.receiveCmd(true);
		jsc.pn("Received response: " + response);
		if (response.equals("Connection disabled")) {
		    return false;
		} else {
		    return true;
		}
	    }   
	} catch (YException e) {
	    return false;
	} catch (InterruptedIOException e) {
	    return false;
	} catch (InterruptedException e) {
	    return false;
	}
	*/

        try {
            commSocket = new DEViseCommSocket(jsc.jsValues.connection.hostname,
	      jsc.jsValues.connection.cmdport, SOCK_REC_TIMEOUT,
	      SOCK_CONST_TIMEOUT);
	    jsc.pn("Socket connection set up!");
            return true;
        } catch (YException e) {
            return false;
        }
    }

    public synchronized void disconnect()
    {
        if (commSocket != null) {
            commSocket.closeSocket();
            commSocket = null;
        }

	_connectedAlready = false;

        isOnline = false;
        isAbort = false;
        jsc.jsValues.connection.connectionID = DEViseGlobals.DEFAULTID;
    }

    public synchronized void run()
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.run(" + commands[0] + ")");
        }

      if (jsc.specialID == -1) { // for formal JS
        try {
	    for (int i = 0; i < commands.length; i++) {
		if (getAbortStatus()) {
		    setAbortStatus(false);
		    break;
		}

		if (commands[i].length() == 0) {
		    continue;
		} else if (!commands[i].startsWith(DEViseCommands.JS_PREFIX)) {
		    jsc.pn("Invalid command: " + commands[i]);
		    continue;
		}

		if (commands[i].startsWith(DEViseCommands.CLOSE_SESSION)) {
		    jsc.jscreen.updateScreen(false);
		    try {
		        processCmd(commands[i]);
		    } catch (YException e1) {
			jsc.showMsg(e1.getMsg());
			disconnect();
		    }
		} else if (commands[i].startsWith(DEViseCommands.HEART_BEAT)) {
		    try {
			jsc.pn("Sending: \"" + commands[i] + "\"");
                        sendCmd(commands[i]);
		    } catch (YException e1) {
			jsc.showMsg(e1.getMsg());
			disconnect();
		    }
		} else if (commands[i].startsWith(DEViseCommands.OPEN_SESSION)) {
		    jsc.jscreen.updateScreen(false);
		    processCmd(commands[i]);
		} else {
		    processCmd(commands[i]);
		}
	    }

	    // Note: this is the "standard" place where the GUI gets
	    // changed to indicate that the command is finished.
	    jsc.animPanel.stop();
	    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
	} catch (YException e) {
	    jsc.animPanel.stop();
	    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);

	    // turn off the counter and the traffic light
	    jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_IDLE, false);
	    jsc.viewInfo.updateCount(0);

	    // user pressed the stop button
	    switch (e.getID()) {
	    case 0: // low level communication error
		jsc.showMsg(e.getMsg());
		jsc.jscreen.updateScreen(false);
		disconnect();
		break;
	    case 1: // invalid response from server
	    case 2: // ill-formated command received from server
		jsc.showMsg(e.getMsg());
		jsc.jscreen.updateScreen(false);
		try {
		    jsc.pn("Sending: \"" + DEViseCommands.CLOSE_SESSION +
			   "\"");
                    sendCmd(DEViseCommands.CLOSE_SESSION);
		} catch (YException e1) {
		    jsc.showMsg(e1.getMsg());
		    disconnect();
		}
		break;
	    default:
		jsc.showMsg(e.getMsg());
		jsc.jscreen.updateScreen(false);
		disconnect();
		break;
	    }
	}

        setAbortStatus(false);
        setStatus(0);
      }
      // Collabration JavaScreen, waiting for incoming commands
      else {
	    try {
		processCmd(commands[0]); // for the connect command
		while (jsc.specialID != -1) {
		    processCmd(null);
		}
	    }
	    catch (YException e) {
	    }
      }

      if (_debug) {
	  System.out.println("  Done with DEViseCmdDispatcher.run(" +
			     commands[0] + ")");
      }
    }

    // Send a command to the server, wait for the replies, and process
    // them.
    private void processCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.processCmd(" +
	      command + ")");
        }

        // sending command to server, and wait until server finish processing and
        // returned a list of commands
        String[] rsp = sendRcvCmd(command);

        // turn on the 'process' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING, true);

        for (int i = 0; i < rsp.length; i++) {
            // adjust the counter
            jsc.viewInfo.updateCount(rsp.length - 1 - i);

	    jsc.pn("Processing command (" + (rsp.length - 1 - i) + ") " +
	      rsp[i]);
            jsc.jsValues.debug.log("Processing command (" +
	      (rsp.length - 1 - i) + ") " + rsp[i]);

	    processReceivedCommand(command, rsp[i]);

	    jsc.pn("  Done with command " + rsp[i]);
	    jsc.pn("  Free mem: " + Runtime.getRuntime().freeMemory() +
	      " Total mem: " + Runtime.getRuntime().totalMemory());
	    jsc.jsValues.debug.log("  Done with command " + rsp[i]);
        }

	_commCgi = null;

        // turn off the 'process' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING, false);

        System.gc();

	//disconnect();

        //System.out.println("Memory: " +
	    //Runtime.getRuntime().freeMemory() + "/" +
	    //Runtime.getRuntime().totalMemory());
    }

    // command is the command we sent; response is the command we got
    // in response.
    private synchronized void processReceivedCommand(String command, String response)
      throws YException
    {
	if (_debug) {
	    System.out.println("DEViseCmdDispatcher.processReceivedCommand(" +
	      command + ", " + response + ")");
	}

        String[] args = DEViseGlobals.parseString(response);
        if (args == null || args.length < 1) {
            throw new YException(
              "Ill-formated command received from server \"" +
              response + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        if (args[0].equals(DEViseCommands.DONE)) {
            // this command will guaranteed to be the last
	    if (command != null) {
		if (command.startsWith(DEViseCommands.OPEN_SESSION)) {
		    jsc.jscreen.updateScreen(true);
		}
	    }

        } else if (args[0].equals(DEViseCommands.FAIL)) {
            jsc.showMsg(response);
            jsc.jscreen.updateScreen(false);
	    
        } else if (args[0].equals(DEViseCommands.CLOSE_SESSION)) {
            // this command is for collaboration JS
            jsc.jscreen.updateScreen(false);

        } else if (args[0].equals(DEViseCommands.ERROR)) {
            // this command will guaranteed to be the last
	    
	    if (jsc.specialID != -1) {
                // a collaborator
		jsc.showMsg(response);
		jsc.specialID = -1;
		jsc.socketMode();
		jsc.animPanel.stop();
		jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
		jsc.jscreen.updateScreen(false);
		dispatcherThread.interrupt();
		setStatus(0);
	    } else {
		if (!command.startsWith(DEViseCommands.GET_SESSION_LIST)) {
		    jsc.showMsg(response);
		} else {
		    jsc.showSession(new String[] {response}, false);
		}
	    }

        } else if (args[0].equals(DEViseCommands.CLIENTS)) {
            jsc.showClientList(response);

        } else if (args[0].equals(DEViseCommands.COLLAB_EXIT)) {
	    jsc.collabQuit();

        } else if (args[0].equals(DEViseCommands.COLLAB_STATE)) {
            if (jsc.specialID == -1) { // only for normal JS	    
		jsc.showCollabState(response);
            }

        } else if (args[0].equals(DEViseCommands.UPDATE_SERVER_STATE)) {
            if (args.length != 2) {
                throw new YException(
                  "Ill-formated command received from server \"" +
                  response + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            if (jsc.specialID == -1) { // only for normal JS
		jsc.showServerState(args[1]);
            }

        } else if (args[0].equals(DEViseCommands.CREATE_VIEW)) {
	    createView(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_VIEW_IMAGE)) {
	    updateViewImage(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_GDATA)) {
	    updateGData(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_SESSION_LIST)) {
	    // Number of arguments is variable.
	    if (jsc.specialID == -1) { // only for normal JS 
		jsc.showSession(args, true);
            }

        } else if (args[0].equals(DEViseCommands.DRAW_CURSOR)) {
	    drawCursor(response, args);

        } else if (args[0].equals(DEViseCommands.ERASE_CURSOR)) {
            if (args.length != 3) {
                throw new YException(
                  "Ill-formated command received from server \"" + response +
                  "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.removeCursor(args[1], args[2]);

        } else if (args[0].equals(DEViseCommands.UPDATE_RECORD_VALUE)) {
	    // Number of arguments is variable.
            jsc.showRecord(args);

        } else if (args[0].equals(DEViseCommands.VIEW_DATA_AREA)) {
	    viewDataArea(response, args);

        } else if (args[0].equals(DEViseCommands.DELETE_VIEW)) {
            if (args.length != 2) {
                throw new YException(
                  "Ill-formated command received from server \"" + response +
                  "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.removeView(args[1]);

        } else if (args[0].equals(DEViseCommands.DELETE_CHILD_VIEWS)) {
            if (args.length != 2) {
                throw new YException(
                  "Ill-formated command received from server \"" + response +
                  "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.removeChildViews(args[1]);

        } else if (args[0].equals(DEViseCommands.USER)) {
	    user(args);

        } else if (args[0].equals(DEViseCommands.SHOW_VIEW_HELP)) {
            if (args.length != 3) {
                throw new YException(
                  "Ill-formated command received from server \"" + response +
                  "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            // Modified - Ven
            if (!jsc.jsValues.connection.helpBox) {
               jsc.jscreen.showHelpMsg(args[1], args[2]);
            } else {
               jsc.showViewDialogHelp(args[2]);
               jsc.jsValues.connection.helpBox = false ;
            }

        } else if (args[0].equals(DEViseCommands.SET_3D_CONFIG)) {
            // this command is for collaboration JS
            jsc.jscreen.collab3DView(args);

        } else {
            throw new YException("Unsupported command (" + response +
              ")received from server", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    private void createView(String command, String[] args) throws YException
    {
        if (args.length < 25) {
            throw new YException(
              "Ill-formated command received from server \"" +
              command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        String parentname = args[2];
        String piledname = args[3];
        try {
            int x = Integer.parseInt(args[4]);
            int y = Integer.parseInt(args[5]);
            int w = Integer.parseInt(args[6]);
            int h = Integer.parseInt(args[7]);
            float z = (Float.valueOf(args[8])).floatValue();
            int dim = Integer.parseInt(args[9]);
            Rectangle viewloc = new Rectangle(x, y, w, h);
            x = Integer.parseInt(args[10]);
            y = Integer.parseInt(args[11]);
            w = Integer.parseInt(args[12]);
            h = Integer.parseInt(args[13]);
            Rectangle dataloc = new Rectangle(x, y, w, h);

            int bg, fg;
            Color color = DEViseUIGlobals.convertColor(args[14]);
            if (color != null) {
                fg = color.getRGB();
            } else {
                throw new NumberFormatException();
            }
            color = DEViseUIGlobals.convertColor(args[15]);
            if (color != null) {
                bg = color.getRGB();
            } else {
                throw new NumberFormatException();
            }
            //int bg = (Color.white).getRGB();
            //int fg = (Color.black).getRGB();

            String xtype = args[16], ytype = args[17];

            // Mouse movement grid -- not yet used.
            float gridx = (Float.valueOf(args[18])).floatValue();
            float gridy = (Float.valueOf(args[19])).floatValue();

            int rb = Integer.parseInt(args[20]);
            int cm = Integer.parseInt(args[21]);
            int dd = Integer.parseInt(args[22]);
            int ky = Integer.parseInt(args[23]);

            String viewtitle = args[24];
            int dtx = 0, dty = 0;
            Font dtf = null;

            int dvinfo = 1; // whether to show mouse location
            if (args.length == 26) {
                // When viewtitle is empty, then arg 25 is the show mouse
                // location - dvinfo

                if (args[25].equals("")) {
                    dvinfo = 1;
                } else{
                    dvinfo = Integer.parseInt(args[25]);
                }
            }

            if (viewtitle.length() > 0) {
                if (args.length < 31 || args.length > 32) {
                    throw new YException(
                      "Ill-formated command received from server \"" +
                      command + "\"",
                      "DEViseCmdDispatcher::processCmd()", 2);
                }

                dtx = Integer.parseInt(args[25]);
                dty = Integer.parseInt(args[26]);

                int dtff;
                if (args[27].equals("")) {
                    dtff = 0;
                } else {
                    dtff = Integer.parseInt(args[27]);
                }

                int dtfs;
                if (args[28].equals("")) {
                    dtfs = 14;
                } else {
                    dtfs = Integer.parseInt(args[28]);
                }

                int dtb;
                if (args[29].equals("")) {
                    dtb = 0;
                } else {
                    dtb = Integer.parseInt(args[29]);
                }

                int dti;
                if (args[30].equals("")) {
                    dti = 0;
                } else {
                    dti = Integer.parseInt(args[30]);
                }

                if (args.length == 32) {
                    if(args[31].equals("")){
                        dvinfo = 1;
                    } else {
                        dvinfo = Integer.parseInt(args[31]);
                    }
                }

                dtf = DEViseUIGlobals.getFont(dtfs, dtff, dtb, dti);
                if (dtf != null) {
                    Toolkit tk = Toolkit.getDefaultToolkit();
                    FontMetrics fm = tk.getFontMetrics(dtf);

                    int ac = fm.getAscent();
                    int dc = fm.getDescent();
                    int ld = fm.getLeading();

                    ac = ac + ld / 2;
                    dc = dc + ld / 2;

                    int sh = fm.getHeight();
                    int sw = fm.stringWidth(viewtitle);

                    int height = 0, width = 0;

                    dty = dty + height / 2 + ac - sh / 2;
                    dtx = dtx + width / 2 - sw / 2;
                }
            }

            DEViseView view = new DEViseView(jsc, parentname, viewname,
              piledname, viewtitle, viewloc, z, dim, bg, fg, dataloc,
              xtype, ytype, gridx, gridy, rb, cm, dd, ky);
            if (view != null) {
                view.viewDTFont = dtf;
                view.viewDTX = dtx + view.viewLocInCanvas.x;
                view.viewDTY = dty + view.viewLocInCanvas.y;
                view.isViewInfo = (dvinfo == 1);
            }

            jsc.jscreen.addView(view);
        } catch (NumberFormatException e) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

    }

    private void updateViewImage(String command, String[] args)
      throws YException
    {
        if (args.length != 3) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        int imageSize;
        try {
            imageSize = Integer.parseInt(args[2]);
            if (imageSize <= 0) {
                throw new YException(
                  "Ill-formated command received from server \"" +
                  command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }
        } catch (NumberFormatException e) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

	byte[] imageData;
	if (jsc.jsValues.connection.cgi) {
	    imageData = _commCgi.receiveData(imageSize);
        } else {
	    imageData = sockReceiveData(imageSize);
        }

        MediaTracker tracker = new MediaTracker(jsc);
        Toolkit kit = jsc.getToolkit();
        Image image = kit.createImage(imageData);
	final int imageId = 0;
        tracker.addImage(image, imageId);
        try {
            tracker.waitForID(imageId);
        }  catch (InterruptedException e) {
	    System.err.println("tracker.waitForID() failed: " +
	      e.getMessage());
	    jsc.jsValues.debug.log("tracker.waitForID() failed: " +
	      e.getMessage());
        }

        if (tracker.isErrorID(imageId)) {
            throw new YException("Invalid image data for view \"" +
              viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        jsc.jscreen.updateViewImage(viewname, image);
    }

    private void updateGData(String command, String[] args) throws YException
    {
        if (args.length != 7) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        DEViseView view = jsc.jscreen.getView(viewname);

        // Remove the old GData from the view and hopefully free it.
        //        DEViseView view = jsc.jscreen.getView(viewname);
        //if (view != null) {
        //    view.removeAllGData();
        //    System.gc();
        //}

        float xm, xo, ym, yo;
        int gdataSize;
        try {
            xm = (Float.valueOf(args[2])).floatValue();
            xo = (Float.valueOf(args[3])).floatValue();
            ym = (Float.valueOf(args[4])).floatValue();
            yo = (Float.valueOf(args[5])).floatValue();
            gdataSize = Integer.parseInt(args[6]);
            if (gdataSize <= 0) {
                throw new YException(
                  "Ill-formated command received from server \"" +
                  command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }
        } catch (NumberFormatException e) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

//          System.out.println("Free memory(before new GData): " +
//            Runtime.getRuntime().freeMemory() + "/" +
//            Runtime.getRuntime().totalMemory());

        byte[] gdata;
	if (jsc.jsValues.connection.cgi) {
	    gdata = _commCgi.receiveData(gdataSize);
        } else {
	    gdata= sockReceiveData(gdataSize);
        }

        String gdataStr = new String(gdata);
        if (gdataStr.equals("\u0004")) {
            jsc.jscreen.updateGData(viewname, null);
        } else {
            // This is used to handle the case when JSPoP sending
            // all the GData in one command (I know currently devised
            // is sending one
            // GData per command) and separate them use \x04.
            String[] GData = DEViseGlobals.parseStr(gdataStr, "\u0004",
              false);
            if (GData == null) {
                throw new YException(
                  "Invalid GData received for view \"" + viewname +
                  "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            Vector gdList = new Vector();
            for (int j = 0; j < GData.length; j++) {
                if (GData[j] == null) {
                    throw new YException(
                      "Invalid GData received for view \"" + viewname +
                      "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                // Split the GData into records.
                String[] results = DEViseGlobals.parseStr(GData[j]);
                if (results == null || results.length == 0) {
                    throw new YException(
                      "Invalid GData received for view \"" + viewname +
                      "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jsValues.gdata.defaultFont = null;

                for (int k = 0; k < results.length; k++) {
                    DEViseGData data = null;
                    //jsc.pn("Received gdata is: \"" + results[k] + "\"");
                    try {
                        data = new DEViseGData(jsc, view, results[k], xm,
                          xo, ym, yo);
                    } catch (YException e1) {
                        //throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                        throw new YException(e1.getMsg(), 2);
                    }

                    gdList.addElement(data);
                    results[k] = null;
                }

                if (_debug) {
                    System.out.println("number of new gdata: " +
                      results.length);
                }

                jsc.jscreen.updateGData(viewname, gdList);
                results = null;
                gdList = null;
            }
        }

        gdata = null;

//          System.gc();

//          System.out.println("Free memory(after new GData): " +
//              Runtime.getRuntime().freeMemory() + "/" +
//              Runtime.getRuntime().totalMemory());
    }

    private void drawCursor(String command, String[] args) throws YException
    {
        if (args.length != 14) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            String cursorName = args[1];
            String viewname = args[2];

            int x0 = Integer.parseInt(args[3]);
            int y0 = Integer.parseInt(args[4]);
            int w = Integer.parseInt(args[5]);
            int h = Integer.parseInt(args[6]);
            String move = args[7];
            String resize = args[8];
            Rectangle rect = new Rectangle(x0, y0, w, h);
            float gridx = (Float.valueOf(args[9])).floatValue();
            float gridy = (Float.valueOf(args[10])).floatValue();
            int isedge = Integer.parseInt(args[11]);
            Color color = DEViseUIGlobals.convertColor(args[12]);
            //TEMP int type = Integer.parseInt(args[13]);
            int type = 0;//TEMP

            DEViseCursor cursor = null;
            try {
                cursor = new DEViseCursor(jsc, cursorName, viewname,
                  rect, move, resize, gridx, gridy, isedge, type, color);
            } catch (YException e1) {
		jsc.showMsg(e1.getMsg());		
		throw new YException("Invalid cursor data received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.updateCursor(viewname, cursor);
        } catch (NumberFormatException e) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    private void viewDataArea(String command, String[] args) throws YException
    {
        if (args.length < 5 || args.length > 7) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            String viewname = args[1];
            String viewaxis = args[2];
            float min = (Float.valueOf(args[3])).floatValue();
            float max = (Float.valueOf(args[4])).floatValue();

            // Ven - for mouse display format string
            String format = null;
            if (args.length >= 6) {
		format = args[5];
            } 
	    
	    float factor = 1;
	    if (args.length == 7) {
		factor = (Float.valueOf(args[6])).floatValue();
	    }

            jsc.jscreen.updateViewDataRange(viewname, viewaxis, min, max,
              format, factor);
        } catch (NumberFormatException e) {
            throw new YException(
              "Ill-formated command received from server \"" + command +
              "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    private void user(String[] args) throws YException
    {
        if (args.length != 2) {
            throw new YException(
              "Invalid connection ID received from server",
              "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            long id = Long.parseLong(args[1]);
            if (id < 0 && id != DEViseGlobals.DEFAULTID) {
                throw new NumberFormatException();
            } else {
                jsc.jsValues.connection.connectionID = id;
                setOnlineStatus(true);
            }
        } catch (NumberFormatException e) {
            throw new YException(
              "Invalid connection ID received from server",
              "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    private byte[] sockReceiveData(int size) throws YException
    {
	if (_debug) {
	    System.out.println("DEViseCmdDispatcher.sockReceiveData(" + size +
	      ")");
	}

        byte[] imgData = null;

        // turn on the receive light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);

        jsc.pn("Trying to receive data (" + size + ") from socket ...");
	jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Trying to receive data (" + size +
	  ") from socket ...");
	jsc.jsValues.debug.log("  Bytes available: " + commSocket.dataAvailable());

        imgData = commSocket.receiveData(size);

        jsc.pn("Successfully received data (" + size + ") from socket ...");
	jsc.pn("  First: " + imgData[0] + "; middle: " +
	  imgData[imgData.length/2] + "; last: " + imgData[imgData.length-1]);
	jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Successfully received data (" + size +
	  ") from socket ...");
	jsc.jsValues.debug.log("  First: " + imgData[0] + "; middle: " +
	  imgData[imgData.length/2] + "; last: " + imgData[imgData.length-1]);

        // turn off the receive light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, false);

        if (imgData == null) {
            throw new YException("Invalid response received from server",
	      "DEViseCmdDispatcher::sockReceiveData()", 1);
        }

        return imgData;
    }

    // Send a command and receive any responses.
    private String[] sendRcvCmd(String command) throws YException
    {
        return sendRcvCmd(command, true);
    }

    // Send a command and receive any responses.
    private String[] sendRcvCmd(String command, boolean expectResponse)
      throws YException
    {
	if (_debug) {
	    System.out.println("DEViseCmdDispatcher.sendRcvCmd(" +
	      command + ", " + expectResponse + ")");
	}

        Vector rspbuf = new Vector();

        if ( (jsc.specialID == -1 && command != null) || 
	     command != null ) { 
	    // turn on the 'send' light
	    jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, true);
	    // sending command to server, and expect an immediate response
            // of "JAVAC_Ack"
	    jsc.pn("Sending: \"" + command + "\"");

            if (jsc.jsValues.connection.cgi) {
                _commCgi = new DEViseCommCgi(jsc.jsValues);
                _commCgi.sendCmd(command);
            } else {
	        sockSendCmd(command);
            }

	    // turn off the 'send' light
	    jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, false);
	} else { // for collabration JS
	    jsc.pn("We are waiting at null command...");
	}

        // turn on the counter
        jsc.viewInfo.updateCount(0);

        // turn on the 'receive' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);

        // wait to receive the response from server
        String response = null;

	// isEnd is true when we have finished receiving *all* commands.
        boolean isEnd = false;
        while (!isEnd) {
	    // isFinish is true when we have finished receiving the current
	    // command.
            boolean isFinish = false;

            while (!isFinish) {
                try {
	            if (jsc.jsValues.connection.cgi) {
                        response = _commCgi.receiveCmd(expectResponse);
                        if (response == "cgi no response") {
                            jsc.pn("CGI no response");
                            return null;
                        }
                    } else {
                        response = commSocket.receiveCmd();
                    }

                    jsc.pn("Receive: \"" + response + "\"");

		    if (response.equals("Connection disabled")) {
			commSocket = null;
	
			jsc.disconnectedMode();		
			
			boolean end = false;
			while (!end) {  
			    if (!reconnect()) {
				jsc.pn("###### Reconnecting failed.");
				try {
				    Thread.sleep(10000);
				} catch (InterruptedException e) {
				}	

			    } else {
				end = true;
				jsc.pn("###### Reconnecting successed.");
				if (jsc.jsValues.connection.cgi) {
				    jsc.cgiMode();
				} else {
				    jsc.socketMode();
				}
			    }
			}

			jsc.pn("Resending command: " + command);
			if (jsc.jsValues.connection.cgi) {
			    _commCgi = null;
			    _commCgi = new DEViseCommCgi(jsc.jsValues);
			    _commCgi.sendCmd(command);
			} else {
			    sockSendCmd(command);
			}

			continue;
		    }

                    isFinish = true;
                } catch (InterruptedIOException e) {
		    if (jsc.collabinterrupted) {
			jsc.collabinterrupted = false;
			commSocket.sendCmd(DEViseCommands.COLLAB_EXIT, 
					   DEViseGlobals.API_JAVA, 
					   jsc.jsValues.connection.connectionID);
			jsc.pn("Sent out Collab_Exit command after interrupt.");
			return null;
		    }
                    if (getAbortStatus()) {
			// switch out of collaboration mode
			//if (jsc.specialID != -1) {
			//  commSocket.sendCmd(DEViseCommands.COLLAB_EXIT);
			//} 
			//else 
			if (jsc.jsValues.connection.cgi) {
                            sendRcvCmd(DEViseCommands.ABORT);
                        } else {
                            commSocket.sendCmd(DEViseCommands.ABORT);
                        }
                        setAbortStatus(false);
                    }
		}
            }

            if (response == null || response.length() == 0) {
                throw new YException("Unexpected response received from server",
                  "DEViseCmdDispatcher::sendRcvCmd()", 1);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command received from server \"" + response + "\"", "DEViseCmdDispatcher::sendRcvCmd()", 2);
                } else {
                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith(DEViseCommands.JS_PREFIX)) {
                        if (cmd.startsWith(DEViseCommands.ACK)) {
                            jsc.animPanel.setActiveImageNumber(5);
                        } else {

                            if (cmd.startsWith(DEViseCommands.DONE) ||
			      cmd.startsWith(DEViseCommands.ERROR) ||
			      cmd.startsWith(DEViseCommands.FAIL)) {
                                isEnd = true;
                            }

                            rspbuf.addElement(cmd);

                            jsc.viewInfo.updateCount(rspbuf.size());
                        }
                    } else {
                	throw new YException("Unsupported command (" +
			  response + ")received from server",
		          "DEViseCmdDispatcher::processCmd()", 2);
                    }
                }
            }
        }

        String[] rspstr = new String[rspbuf.size()];
        for (int i = 0; i < rspbuf.size(); i++)
            rspstr[i] = (String)rspbuf.elementAt(i);

        // turn off the 'receive' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, false);

        return rspstr;
    }

    private void sendCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sendCmd(" +
              command + ")");
        }

        if (jsc.jsValues.connection.cgi) {
            // cgi routing -- we must send *and* receive for the CGI
	    // to work right.
	    sendRcvCmd(command, false);
        } else {
            sockSendCmd(command);
        }
    }

    // Send a command in socket mode.
    // Note: I don't really like having this be a public method, because
    // if we call it from another class we're bypassing part of the "normal"
    // command processing.  However, because of the way Kai has implemented
    // collaboration, it has to be public for now.  RKW 2001-03-08.
    public synchronized void sockSendCmd(String command) throws YException 
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sockSendCmd(" +
	      command + ")");
        }

	if ( !commSocket.isAvailable() ) {
	    commSocket = null;
	    jsc.disconnectedMode();

	    boolean end = false;
	    while (!end) {
		if (!reconnect()) {
		    jsc.pn("###### Reconnecting failed.");
		    try {
			Thread.sleep(10000);
		    } catch (InterruptedException e) {
		    }
		} else {
		    end = true;
		    jsc.pn("###### Reconnecting successed.");
		    jsc.socketMode();    
		}
	    }

	    jsc.pn("Send command:" + command);

	    commSocket.sendCmd(command, DEViseGlobals.API_JAVA, jsc.jsValues.connection.connectionID);
	    return;
	}

	// send the command
	commSocket.sendCmd(command, DEViseGlobals.API_JAVA,
			   jsc.jsValues.connection.connectionID);

	if (!commSocket.isOpen()) {
	    commSocket = null;
	    jsc.disconnectedMode();
	    boolean end = false;
	    while (!end) {
		if (!reconnect()) {
		    jsc.pn("###### Reconnecting failed.");
		    try {
			Thread.sleep(10000);
		    } catch (InterruptedException e) {
		    }
		} else {
		    end = true;
		    jsc.pn("###### Reconnecting successed.");
		    jsc.socketMode();
		}
	    }

	    jsc.pn("Send command:" + command);

	    commSocket.sendCmd(command, DEViseGlobals.API_JAVA, jsc.jsValues.connection.connectionID);
	    return;
	}
    }

    // Test whether a CGI connection to the JSPoP works.  Returns true if
    // it does.
    boolean testCgi() {
        if (_debug) {
	    System.out.println("DEViseCmdDispatcher.testCgi()");
	}

	boolean result = true;

	try {
            jsc.jsValues.connection.cgi = true;
            String[] response = sendRcvCmd(DEViseCommands.CHECK_POP);
            jsc.jsValues.connection.cgi = false;
            _commCgi = null;
	    if (response == null || response.length != 1 ||
	      !response[0].equals(DEViseCommands.DONE))
	    {
		String msg = "Received incorrect response from the jspop: ";
		if (response != null) {
	            for (int index = 0; index < response.length; index++) {
	                msg += "<" + response[index] + "> ";
	            }
		}
		throw new YException(msg);
	    }
	} catch (YException ex) {
	    System.err.println("CGI connection not available: " +
	      ex.getMessage());
	    result = false;
	}

	return result;
    }
}
