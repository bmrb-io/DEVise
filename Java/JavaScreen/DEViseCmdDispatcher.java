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

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    private jsdevisec jsc = null;

    private Thread dispatcherThread = null;

    private String[] commands = null;

    private DEViseCommSocket commSocket = null;

    private DEViseHeartbeat _heartbeat = null;

    public String errMsg = null;

    // status = 0, dispatcher is not running
    // status = 1, dispatcher is running
    private int status = 0;

    // isOnline = true, successfully established connection to server, i.e.
    //                  get a valid connection ID
    // isOnline = false, connection to server is not established
    private boolean isOnline = false;

    // ADD COMMENT -- what does this mean?
    private boolean isAbort = false;

    private URL cgiURL = null;
    //TEMP -- this doesn't seem to get set to null when we're done
    //TEMP -- this perhaps shouldn't be an object variable
    private DataInputStream cgiInput = null;

    private static final boolean _debug = false;

    //TEMP -- we shouldn't really need this *and* isOnline, but I don't
    // want to sort that out right now.  RKW 2000-10-18.
    private boolean _connectedAlready = false;

    public DEViseCmdDispatcher(jsdevisec what)
    {
        jsc = what;
    }

    // This must *not* be a syncronized method, to avoid deadlock between
    // the GUI and command-processing threads (via
    // DEViseCanvas.checkMousePos().
    public int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int arg)
    {
        status = arg;
        jsc.jscreen.reEvaluateMousePosition();
    }

    // Changed this to not check whether we have a socket, since we may
    // now have a "virtual" connection even if a socket isn't open.
    // RKW 2000-10-18.
    public synchronized boolean getOnlineStatus()
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

    private synchronized void setAbortStatus(boolean flag)
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
        if (getStatus() != 0) {
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
			String result = jsc.confirmMsg(errMsg + "\n \nDo you wish to try again?");
			if (result.equals(YMsgBox.YIDNO)) {
			    String result2 = jsc.confirmMsg("\nDo you wish to use CGI?");
			    if (result2.equals(YMsgBox.YIDNO)) {    
				jsc.animPanel.stop();
				jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
				setStatus(0);
				return;
			    } else {
				jsc.cgiMode();
				jsc.jsValues.connection.cgi = true;
				isEnd = true;
			    }
			}
		    } else {
			isEnd = true;
		    }
		}
	    }
	} else { // in cgi mode, close the socket
	    if (commSocket != null) {
		commSocket.closeSocket();
		commSocket = null;
		jsc.pn("Socket has been closed.");
	    }	
	}	
	

	// If we don't have a connection yet, prepend a connection request
	// command to whatever was passed in.

        if (!_connectedAlready) {
            cmd = DEViseCommands.CONNECT + " {" + jsc.jsValues.connection.username +
	      "} {" + jsc.jsValues.connection.password + "} {" +
	      DEViseGlobals.PROTOCOL_VERSION + "}\n" + cmd;
	      _connectedAlready = true;

	      // Start the heartbeat thrad.
	      //TEMP _heartbeat = new DEViseHeartbeat(this);
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

    public synchronized void destroy()
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
                if (jsc.jsValues.connection.cgi) {
                    // cgi routing
                    cgi_sendCommand(DEViseCommands.EXIT);
                } else {
                    socketSendCom(DEViseCommands.EXIT);
                }
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
            commSocket = new DEViseCommSocket(jsc.jsValues.connection.hostname, jsc.jsValues.connection.cmdport);
	    jsc.pn("Socket connection set up!");
            return true;
        } catch (YException e) {
            errMsg = e.getMessage() + " in " + e.getWhere();
	    jsc.pn("Socket connection turned down!");
            disconnect();
            return false;
        }
    }

    private synchronized void disconnect()
    {
        if (commSocket != null) {
            commSocket.closeSocket();
            commSocket = null;
        }

        isOnline = false;
        isAbort = false;
        jsc.jsValues.connection.connectionID = DEViseGlobals.DEFAULTID;
    }

    public synchronized void run()
    {
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
		        if (jsc.jsValues.connection.cgi) {
		            // cgi routing
			    cgi_processCmd(commands[i]);
		        } else {
			    processCmd(commands[i]);
		        }
		    } catch (YException e1) {
			jsc.showMsg(e1.getMsg());
			disconnect();
		    }
		} else if (commands[i].startsWith(DEViseCommands.HEART_BEAT)) {
		    try {
			jsc.pn("Sending: \"" + commands[i] + "\"");
			if (jsc.jsValues.connection.cgi) {
			    // cgi routing
			    cgi_sendCommand(commands[i]);
			} else {
			    socketSendCom(commands[i]);
			}
		    } catch (YException e1) {
			jsc.showMsg(e1.getMsg());
			disconnect();
		    }
		} else if (commands[i].startsWith(DEViseCommands.OPEN_SESSION)) {
		    jsc.jscreen.updateScreen(false);
		    if (jsc.jsValues.connection.cgi) {
		        // cgi routing
			cgi_processCmd(commands[i]);
		    } else {
			processCmd(commands[i]);
		    }
		} else {
		    if (jsc.jsValues.connection.cgi) {
		        // cgi routing
			cgi_processCmd(commands[i]);
		    } else {
			processCmd(commands[i]);
		    }
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
		    if (jsc.jsValues.connection.cgi) {
		        // cgi routing
			cgi_sendCommand(DEViseCommands.CLOSE_SESSION);
		    } else {
			socketSendCom(DEViseCommands.CLOSE_SESSION);
		    }
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

    private void processCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.processCmd(" +
	      command + ")");
        }

        // sending command to server, and wait until server finish processing and
        // returned a list of commands
        String[] rsp = sendCommand(command);

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
    private void processReceivedCommand(String command, String response)
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
            if (command.startsWith(DEViseCommands.OPEN_SESSION)) {
                jsc.jscreen.updateScreen(true);
            }

        } else if (args[0].equals(DEViseCommands.FAIL)) {
            jsc.showMsg(response);
            jsc.jscreen.updateScreen(false);

        } else if (args[0].equals(DEViseCommands.ERROR)) {
            // this command will guaranteed to be the last
            if (!command.startsWith(DEViseCommands.GET_SESSION_LIST)) {
                jsc.showMsg(response);
            } else {
                jsc.showSession(new String[] {response}, false);
            }

        } else if (args[0].equals(DEViseCommands.UPDATE_SERVER_STATE)) {
            if (args.length != 2) {
                throw new YException(
                  "Ill-formated command received from server \"" +
                  response + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.showServerState(args[1]);

        } else if (args[0].equals(DEViseCommands.CREATE_VIEW)) {
	    createView(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_VIEW_IMAGE)) {
	    updateViewImage(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_GDATA)) {
	    updateGData(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_SESSION_LIST)) {
	    // Number of arguments is variable.
            jsc.showSession(args, true);

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
	    imageData = cgi_receiveData(imageSize);
        } else {
	    imageData = receiveData(imageSize);
        }

        MediaTracker tracker = new MediaTracker(jsc);
        Toolkit kit = jsc.getToolkit();
        Image image = kit.createImage(imageData);
        tracker.addImage(image, 0);
        try {
            tracker.waitForID(0);
        }  catch (InterruptedException e) {
        }

        if (tracker.isErrorID(0)) {
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
	    gdata = cgi_receiveData(gdataSize);
        } else {
	    gdata= receiveData(gdataSize);
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
        if (args.length < 5 || args.length > 6) {
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
            if (args.length == 6) {
               format = args[5];
            } 

            jsc.jscreen.updateViewDataRange(viewname, viewaxis, min, max,
              format);
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
            int id = Integer.parseInt(args[1]);
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

    private byte[] receiveData(int size) throws YException
    {
	if (_debug) {
	    System.out.println("DEViseCmdDispatcher.receiveData(" + size +
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
	jsc.pn("  Last byte = " + imgData[imgData.length - 1]);
	jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Successfully received data (" + size +
	  ") from socket ...");

        // turn off the receive light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, false);

        if (imgData == null) {
            throw new YException("Invalid response received from server", "DEViseCmdDispatcher::receiveData()", 1);
        }

        return imgData;
    }

    // Send a command and receive any responses.
    private String[] sendCommand(String command) throws YException
    {
	if (_debug) {
	    System.out.println("DEViseCmdDispatcher.sendCommand(" +
	      command + ")");
	}

        String response = null;
        boolean isEnd = false, isFinish = false;
        Vector rspbuf = new Vector();

        // turn on the 'send' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, true);
        // sending command to server, and expect an immediate response of "JAVAC_Ack"
        jsc.pn("Sending: \"" + command + "\"");
	socketSendCom(command);
	
        // turn off the 'send' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, false);

        // turn on the counter
        jsc.viewInfo.updateCount(0);

        // turn on the 'receive' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);

        // wait to receive the response from server

	// isEnd is true when we have finished receiving *all* commands.
        while (!isEnd) {
	    // isFinish is true when we have finished receiving the current
	    // command.
            isFinish = false;

            while (!isFinish) {
                try {
                    response = commSocket.receiveCmd();
                    jsc.pn("Receive: \"" + response + "\"");
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    if (getAbortStatus()) {
	                //TEMP -- what about message type????
                        commSocket.sendCmd(DEViseCommands.ABORT);
                        setAbortStatus(false);
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Unexpected response received from server", "DEViseCmdDispatcher::sendCommand()", 1);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command received from server \"" + response + "\"", "DEViseCmdDispatcher::sendCommand()", 2);
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

    private void socketSendCom(String command) throws YException 
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.socketSendCom(" +
	      command + ")");
        }

	// send the command
	if (command.startsWith(DEViseCommands.CONNECT)) {
            commSocket.sendCmd(command, DEViseGlobals.API_JAVA_WID,
	      jsc.jsValues.connection.connectionID);
        } else {
            commSocket.sendCmd(command, DEViseGlobals.API_JAVA,
	      jsc.jsValues.connection.connectionID);
	}
    }

    private void cgi_processCmd(String command) throws YException
    {
        // sending command to server, and wait until server finish processing and
        // returned a list of commands
        String[] rsp = cgi_sendCommand(command);

        // turn on the 'process' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING, true);

        for (int i = 0; i < rsp.length; i++) {
            // adjust the counter
            jsc.viewInfo.updateCount(rsp.length - 1 - i);

	    jsc.pn("Processing cgi-command (" + (rsp.length - 1 - i) + ") " +
	      rsp[i]);
            jsc.jsValues.debug.log("Processing cgi-command (" +
	      (rsp.length - 1 - i) + ") " + rsp[i]);

	    processReceivedCommand(command, rsp[i]);

	    jsc.pn("  Done with command " + rsp[i]);
	    jsc.pn("  Free mem: " + Runtime.getRuntime().freeMemory() +
	      " Total mem: " + Runtime.getRuntime().totalMemory());
	    jsc.jsValues.debug.log("  Done with command " + rsp[i]);
        }

        // turn off the 'process' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING, false);

        System.gc();

    }


    private String[] cgi_sendCommand(String command)
      throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.cgi_sendCommand(" +
	      command + ")");
        }

        Vector rspbuf = new Vector();
	URLConnection cgiConn = null;
        DataOutputStream cgiOutput = null;

        // turn on the 'send' light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, true);
        // sending command to server, and expect an immediate response of "JAVAC_Ack"
        jsc.pn("Sending cgi command: \"" + command + "\"");
	try {
	    cgiURL = new URL("http", jsc.jsValues.connection.hostname,
			     jsc.jsValues.connection.cgiURL);

	    //cgiURL = new URL(jsc.jsValues.connection.cgiURL);
	    cgiConn = cgiURL.openConnection();
	    cgiConn.setDoOutput(true);
	    cgiOutput = new DataOutputStream(cgiConn.getOutputStream());

	    String[] cmdBuffer = DEViseGlobals.parseString(command, true);

	    // nelem is the number of "elements" (arguments); size is
	    // the total size of the arguments plus the length value
	    // for each argument.
	    int nelem, size = 0;

	    nelem = cmdBuffer.length;
            
	    for (int i = 0; i < nelem; i++)  {
		// Note: for some reason, having nulls in the string
		// sent to the CGI script causes problems when the JS
		// is run as an applet (but not when it's run as an
		// application), even though the CGI script seems to
		// receive everything correctly.  RKW 2000-12-21.

		//TEMPTEMP -- appending a null (how it was previously) goofs
		// up CGI mode when running in netscape; not appending
		// anything totally goofs things up.  RKW 2000-12-21.
		cmdBuffer[i] = cmdBuffer[i] + "_";//TEMPTEMP
		// Note: the "2" here is for the length value (short).
                size = size + 2 + cmdBuffer[i].length();
            }

	    cgiOutput.writeBytes("cmdport=" + jsc.jsValues.connection.cmdport);
	    cgiOutput.writeBytes("&msgtype=" + DEViseGlobals.API_JAVA_WID);
	    cgiOutput.writeBytes("&id=" + jsc.jsValues.connection.connectionID);
	    cgiOutput.writeBytes("&nelem=" + nelem);
	    cgiOutput.writeBytes("&size=" + size);
	    
            for (int i = 0; i < nelem; i++) {
                cgiOutput.writeBytes("&len" + i + "=" + cmdBuffer[i].length());
		cgiOutput.writeBytes("&arg" + i + "=" + cmdBuffer[i]);
            }
	    
	    cgiOutput.flush();

	    jsc.pn("Cgi command has been sent");

	} catch(MalformedURLException excpt) {
	    jsc.pn("Badly formed URL: " + excpt);
	} catch(IOException excpt) {
	    jsc.pn("Failed I/O: " + excpt);
	} catch(Exception ex) {
	    System.err.println("Exception sending cgi command: " +
	      ex.getMessage());
	    throw new YException(ex.getMessage());
	}

	// turn off the 'send' light
	jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING, false);

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

	    // Kludgey fix for bug 630.  RKW 2000-12-22.
	    if (DEViseCommands.EXIT.equals(command)) {
	        isEnd = true;
		isFinish = true;
		response = DEViseCommands.DONE;
	    }

	    while (!isFinish) {
		try {
		    //TEMP -- problems here if cgiConn is null (exception
		    // was thrown and caught earlier).
		    response = cgi_receiveCmd(cgiConn);
		    if (response == "cgi no response") {
			jsc.pn("CGI no response");
			return null;
		    }

		    jsc.pn("Receive cgi command: \"" + response + "\"");
		    isFinish = true;

		} catch (InterruptedIOException e) {
		    if (getAbortStatus()) {
		        cgi_sendCommand(DEViseCommands.ABORT);
		        setAbortStatus(false);
		    }
	        }
	    }

	    if (response == null || response.length() == 0) {
	        throw new YException(
		  "Unexpected response received from server",
		  "DEViseCmdDispatcher::sendCommand()", 1);
	    } else {
	        String[] cmds = DEViseGlobals.parseString(response);
	        if (cmds == null || cmds.length == 0) {
		    throw new YException(
		      "Ill-formated command received from server \"" +
		      response + "\"",
		      "DEViseCmdDispatcher::sendCommand()", 2);
		} else {
		    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the
		    // arguments.
		    for (int j = 1; j < cmds.length; j++) {
		        cmd = cmd + " {" + cmds[j] + "}";
		    }

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
	for (int i = 0; i < rspbuf.size(); i++) {
	    rspstr[i] = (String)rspbuf.elementAt(i);
        }

        // turn off the 'receive' light
	jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, false);

	return rspstr;
    }

    private synchronized String cgi_receiveCmd(URLConnection cgiConn)
      throws YException, InterruptedIOException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.cgi_receiveCmd()");
        }

        int msgType = 0, numberOfElement = 0, totalSize = 0;
	int cmdId = 0, cgiFlag = 0;
        byte[] dataRead = null;
        int numberRead = 0;
        
      	try {
             cgiInput = new DataInputStream(cgiConn.getInputStream());

	     if (dataRead == null) {
               dataRead = new byte[10];
               numberRead = 0;
             }

             int b;
             for (int i = numberRead; i < 10; i++) {
		 b = cgiInput.read();
                 if (b < 0) {
		     //TEMP -- is this safe?
		     return "cgi no response";
		     //throw new YException(
		     //"Abrupt end of input stream reached",
		     //"cgi_receiveCmd()");
                 }

                 dataRead[numberRead] = (byte)b;
                 numberRead++;
             }

             msgType = DEViseGlobals.toUshort(dataRead);
	     cmdId = DEViseGlobals.toUshort(dataRead, 2);
	     cgiFlag = DEViseGlobals.toUshort(dataRead, 4);
             numberOfElement = DEViseGlobals.toUshort(dataRead, 6);
             totalSize = DEViseGlobals.toUshort(dataRead, 8);

             dataRead = null;

             if (numberOfElement <= 0 || totalSize <= 0) { 
		 throw new YException(
		   "Invalid control information received", "cgi_receiveCmd()");
             }
            
             if (dataRead == null) {
                dataRead = new byte[totalSize];
                numberRead = 0;
             }

             for (int i = numberRead; i < totalSize; i++) {
                b = cgiInput.read();

                if (b < 0) {
		    throw new YException(
		      "Abrupt end of input stream reached",
		      "cgi_receiveCmd()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }
        } catch (InterruptedIOException e) {
	    System.err.println("InterruptedIOException in cgi_receiveCmd: " + e.getMessage());
            throw e;
        } catch (IOException e) {
	    System.err.println("IOException in cgi_receiveCmd: " + e.getMessage());
            throw new YException("Error occurs while reading from " +
	      "input stream in cgi_receiveCmd()", "cgi_receiveCmd()");
        }

        int argsize = 0;
        int pastsize = 0;
        String response = new String("");
        for (int i = 0; i < numberOfElement; i++) {
            if (totalSize < pastsize + 2) {
                throw new YException("Inconsistant data received",
		"cgi_receiveCmd()");
            }

            argsize = DEViseGlobals.toUshort(dataRead, pastsize);
            pastsize += 2;

            if (totalSize < pastsize + argsize) {
                throw new YException("Inconsistant data received",
		  "cgi_receiveCmd()");
            }

            // use argsize - 1 instead of argsize is to skip the string ending '\0'
            // use one space to seperate different parts of the response command
            response += new String(dataRead, pastsize, argsize - 1) + " ";
            pastsize += argsize;
        }

        // resetData();

        return response;
    }

    private byte[] cgi_receiveData(int size) throws YException
    {
        byte[] imgData = null;
        byte[] dataRead = null;
        int numberRead = 0;

        // turn on the receive light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);

        jsc.pn("Trying to receive cgi-data (" + size + ")...");
	//jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Trying to receive cgi-data (" + size +
	  ") from socket ...");

        // simply give up receiving if input size is invalid
        if (size <= 0) {
            return null;
        }

        try {
            if (dataRead == null) {
                dataRead = new byte[size];
                numberRead = 0;
            }

	    //TEMP -- make this into a method?
            int offset = 0;
	    int bytesRemaining = size;
	    while (bytesRemaining > 0) {
		try {
	            int bytesRead = cgiInput.read(dataRead, offset,
		      bytesRemaining);
		    offset += bytesRead;
		    bytesRemaining -= bytesRead;
		} catch(InterruptedIOException e) {
		    // Note: this may happen if the data transfer is too slow.
		    jsc.jsValues.debug.log("Error reading data: " +
		      e.getMessage());
		    try {
		        Thread.sleep(100); // wait for some more data on socket
		    } catch (InterruptedException ex) {
		    }
		}
	    }

	    imgData = dataRead;

        } catch (IOException e) {
            throw new YException(
	      "Error occurs while reading from input stream",
	      "cgi_receiveData()");
        }


        jsc.pn("Successfully received cgi ata (" + size + ")...");
	jsc.pn("  Last byte = " + imgData[imgData.length - 1]);
        jsc.jsValues.debug.log("Successfully received data (" + size +
	  ") from socket ...");

        // turn off the receive light
        jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, false);

        if (imgData == null) {
            throw new YException("Invalid response received from CGI",
	      "cgi_receiveData()", 1);
        }

        return imgData;
    }
}
