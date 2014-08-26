// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides the interface between the jspop and an associated
// devised (server).  Basically, an object of this class gets a command
// from the client associated with the server, sends the command to
// the server, and then sends the reply commands and data back to the
// client.  It also keeps track of the current session file directory
// for the devised.

// There is one instance of this class for each devised connected to
// a jspop.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.83  2012/04/30 22:20:18  wenger
// Merged js_data_save_br_0 thru js_data_save_br_1 to trunk.
//
// Revision 1.82.16.3  2012/04/27 16:46:01  wenger
// Cleaned up a bunch of temporary/debug code.
//
// Revision 1.82.16.2  2012/04/23 18:48:25  wenger
// Data download now handles piles (still with dummy data); partway to
// correctly doing Miron's requested change of creating a file that we
// redirect the user's browser to.
//
// Revision 1.82.16.1  2012/04/16 23:26:54  wenger
// The JavaScreen data saving code now sends dummy data from the DEVised
// to the JS client -- needs lots of cleanup, and we don't actually show
// the data in a dialog yet.
//
// Revision 1.82  2009/08/28 15:42:18  wenger
// Drill-down marker is now working; cleaned up version history, since
// 5.9.5 was never released.
//
// Revision 1.81  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.80  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.79  2002/06/17 19:40:15  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.78  2002/05/01 21:28:59  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.77.2.14  2005/11/07 21:25:04  wenger
// Added timestamps to many more items in the JSPoP debug output.
//
// Revision 1.77.2.13  2004/09/29 19:08:35  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.77.2.12  2004/05/12 21:59:59  wenger
// Removed a bunch of temporary stuff from the jspop_debug_0405_br
// branch.
//
// Revision 1.77.2.11  2004/05/12 21:43:58  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.77.2.10.4.6  2004/09/21 19:38:12  wenger
// Misc. cleanup before merging back into 1.7 (DEViseClientSocket.java
// still needs some changes).
//
// Revision 1.77.2.10.4.5  2004/09/10 23:04:23  wenger
// Seem to have fixed the problem of the JSPoP hanging on RedHat 9
// (changed synchronization in the jspop class); still more diagnostic
// output; added config file for local install on trigger (for testing).
//
// Revision 1.77.2.10.4.4  2004/07/01 15:15:49  wenger
// Improved circular log (now always has "-END-" at the temporal end
// of the log); various other debug logging improvements; put the
// sequence of operations in DEViseClientSocket.closeSocket() back
// the way it was.
//
// Revision 1.77.2.10.4.3  2004/05/12 21:27:26  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.77.2.10.4.2  2004/05/10 22:28:51  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.77.2.10.4.1  2004/05/10 19:38:59  wenger
// Lots of new debug code, turned on at compile time; no significant
// functional changes; also has comments about where we might be
// getting hung, based on debug logs.
//
// Revision 1.77.2.10  2003/10/15 21:55:11  wenger
// Added new JAVAC_StopCollab command to fix ambiguity with
// JAVAC_CollabExit; minor improvements to collaboration-related stuff
// in the auto test scripts.
//
// Revision 1.77.2.9  2003/09/23 21:55:12  wenger
// "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.
//
// Revision 1.77.2.8  2003/06/25 21:34:09  wenger
// JSPoP debug log improvements to try to track down the cause of some
// DEVised restarts.
//
// Revision 1.77.2.7  2003/06/17 22:44:39  wenger
// Session opens are now logged in a separate JS usage log; the code
// that generates the usage summary emails is not yet updated to
// use that log.
//
// Revision 1.77.2.6  2002/12/05 21:12:00  wenger
// Added diagnostic output for the number of commands run at each client
// switch.
//
// Revision 1.77.2.5  2002/12/05 20:38:20  wenger
// Removed a bunch of unused (mostly already-commented-out) code to
// make things easier to deal with.
//
// Revision 1.77.2.4  2002/05/22 21:31:50  xuk
// Improve autotest.
//
// Revision 1.77.2.3  2002/05/20 21:21:33  wenger
// Fixed bug 779 (client switching problem with multiple DEViseds).
//
// Revision 1.77.2.2  2002/04/18 17:25:11  wenger
// Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
// temporary session files when the JSPoP and DEViseds are on different
// machines).  Note: JS protocol version is now 11.0.
//
// Revision 1.77.2.1.2.5  2002/04/18 15:40:59  wenger
// Further cleanup of JavaScreen temporary session file code (added
// JAVAC_DeleteTmpSession command) (includes fixing bug 774).
//
// Revision 1.77.2.1.2.4  2002/04/17 20:14:31  wenger
// Implemented new JAVAC_OpenTmpSession command to go along with
// JAVAC_SaveTmpSession (so the JSPoP doesn't need to have any info about
// the path of the temporary session directory relative to the base
// session directory).
//
// Revision 1.77.2.1.2.3  2002/04/17 19:13:55  wenger
// Changed JAVAC_SaveSession command to JAVAC_SaveTmpSession (path is
// now relative to temp session directory, not main session directory).
//
// Revision 1.77.2.1.2.2  2002/04/17 17:45:54  wenger
// DEVised, not JSPoP, now does the actual work of creating or clearing
// the temporary session directory (new command from client to DEVised
// means that communication protocol version is now 11.0).  (Client
// switching is not working yet with this code because I need to change
// how temporary sessions are saved and loaded.)
//
// Revision 1.77.2.1.2.1  2002/04/17 16:15:34  wenger
// Cleaned up unnecessary serverDatas in DEViseServer.java.
//
// Revision 1.77.2.1  2002/04/12 16:08:49  wenger
// Lots of cleanup to the heartbeat checking code -- tested killing a
// client because the heartbeat timeout expired, and because we have
// too many clients.
//
// Revision 1.77  2002/03/12 19:50:04  wenger
// Fixed bug 758 (caused by JAVAC_Set3DConfig not causing a client switch
// but still sending a command to the DEVised); did some cleanup of the
// DEViseClient class.
//
// Revision 1.76  2002/03/01 19:58:53  xuk
// Added new command DEViseCommands.UpdateJS to update JavaScreen after
// a DEViseCommands.Open_Session or DEViseCommands.Close_Session command.
//
// Revision 1.75  2002/01/24 23:03:36  xuk
// *** empty log message ***
//
// Revision 1.74  2001/11/28 21:56:20  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.73  2001/11/19 17:17:03  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.72  2001/11/13 17:57:01  xuk
// Could send command in String[] format, no need to compose a long command string before sending.
//
// Revision 1.71.2.2  2001/11/21 22:11:16  wenger
// Fixed the JSPoP deadlock problem caused by a new client connection
// happening while in the middle of a client switch.
//
// Revision 1.71.2.1  2001/11/13 20:31:35  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.71  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.70  2001/10/30 17:31:14  xuk
// reated DEViseClient object for collaborating clients in jspop.
//
// Revision 1.69.2.2  2001/11/07 17:22:36  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.69.2.1  2001/10/25 22:55:54  wenger
// JSPoP now replies to heartbeat to make it less of a special case.
//
// Revision 1.69  2001/10/24 17:46:07  wenger
// Fixed bug 720 (one client can block others in the JSPoP).  The fix is that
// the JSPoP now has a separate thread to read from each client.
//
// Revision 1.68  2001/10/12 02:07:44  xuk
// ng timestamp-based client ID.
//
// Revision 1.67  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.66  2001/05/11 20:36:08  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.65  2001/04/25 19:41:49  xuk
// Fixed bug 661: when a JS goes into Collaboration mode from Socket mode,
// It will not become an available collaboration leader.
//
// Revision 1.64  2001/04/17 02:30:14  xuk
// *** empty log message ***
//
// Revision 1.63  2001/04/12 18:18:17  wenger
// Added more diagnostic code to the JSPoP.
//
// Revision 1.62  2001/04/12 17:35:43  wenger
// Workaround in place for the collaboration hanging bug (caused by more
// than one server being connected to the same client).
//
// Revision 1.61  2001/04/12 15:54:06  wenger
// Made some minor improvements to the hang checking.
//
// Revision 1.60  2001/04/11 21:16:29  xuk
// A collaboration leader could find out the followers hostname.
//
// Revision 1.59  2001/04/11 21:09:15  wenger
// Added diagnostic output of client.collabInit.
//
// Revision 1.58  2001/04/11 16:49:38  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//
// Revision 1.57  2001/04/06 19:32:14  wenger
// Various cleanups of collaboration code (working on strange hang
// that Miron has seen); added more debug output; turned heartbeat
// back on (it somehow got turned off by accident).
//
// Revision 1.56  2001/04/01 03:53:00  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
// Added cmdSet3DConfig(), process JAVAC_Set3DConfig command.
//
// Revision 1.55  2001/03/20 20:11:39  wenger
// Added more debug output to the JS client and jspop.
//
// Revision 1.54  2001/03/20 17:50:12  xuk
// *** empty log message ***
//
// Revision 1.53  2001/03/05 16:45:26  xuk
// Clear follower's JavaScreen when leader close session.
// Changes in cmdCloseSession().
//
// Revision 1.52  2001/03/04 22:04:27  xuk
// Fixed bugs for JAVAC_CloseSession command in cmdCloseSession().
//
// Revision 1.51  2001/03/03 20:08:20  xuk
// Restore old state if user goes into, then out of, collaboration mode.
// 1.Added cmdSaveSession() to process JAVAC_SaveCurSession command.
// 2.Added cmdReopenSession() to process JAVAC_ReopenSession command.
// 3.Changes in switchClient() to avoid unnecessary sending JAVAC_SaveSession command.
//
// Revision 1.50  2001/02/20 20:02:23  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.49  2001/02/19 20:34:09  xuk
// Added command(s) and GUI so that a collaboration leader can find out who is
// collaborating with it.
//
// Revision 1.48  2001/02/06 16:38:40  xuk
// Change for multiple collaborated JSs in run().
//
// Revision 1.47  2001/01/31 22:24:31  xuk
// Fix bugs for wrong collaboration JS ID.
//
// Revision 1.46  2001/01/30 03:12:43  xuk
// Changes for collaboration JS.
//
// Revision 1.45.2.1  2001/01/31 17:44:02  wenger
// Cron job to check jspop now runs every minute on yola; added more
// diagnostic output to checking; temporarily? increased socket receive
// timeouts to see if this helps on yola; added timestamp to
// JAVAC_CheckPop command.
//
// Revision 1.45  2001/01/08 20:31:53  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.42.4.13  2000/12/27 19:38:34  wenger
// Merged changes from js_restart_improvements thru zero_js_cache_check from
// the trunk onto the js_cgi_br branch.
//
// Revision 1.42.4.12  2000/12/22 16:29:06  wenger
// Fixed bug 629; temporarily disabled heartbeat pending fix for bug 627.
//
// Revision 1.44  2000/12/14 21:41:06  wenger
// More debug info (bytes of data available from devised).
//
// Revision 1.42.4.11  2000/12/07 17:36:45  xuk
// *** empty log message ***
//
// Revision 1.42.4.10  2000/11/22 17:43:58  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.42.4.9  2000/11/08 18:21:38  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.42.4.8  2000/11/02 04:59:48  xuk
// *** empty log message ***
//
// Revision 1.42.4.7  2000/11/01 22:14:18  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.42.4.6  2000/10/25 03:10:07  xuk
// Do not close socket after communication of each command. Modified in run().
//
// Revision 1.42.4.5  2000/10/18 20:28:11  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.43  2000/09/20 19:29:32  wenger
// Removed maximum logins per user from jspop (causes problems because client
// objects are not removed if JS crashes; generally simplified the DEViseUser-
// related code.
//
// Revision 1.42.4.4  2000/10/18 18:29:23  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.42.4.3  2000/10/09 16:35:41  xuk
// In processClientcmd() function, process the Javac_heartBeat command received from JS.
//
// Revision 1.42.4.2  2000/10/02 19:16:26  xuk
// In run() function, disconnect the socket between the client and JS after the communication for each command.
//
// Revision 1.42.4.1  2000/09/01 20:14:28  xuk
// Delete dataPort. In startSocket() function, only provide one port to DEViseCommSocket constructor.
//
// Revision 1.42  2000/07/19 20:11:36  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.41  2000/06/21 18:37:30  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.40  2000/06/15 15:55:22  wenger
// Fixed bug 597 (problem with devised restarts in jspop).
//
// Revision 1.39  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.38  2000/06/05 16:35:07  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.37  2000/04/27 20:15:25  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.36  2000/04/07 22:44:10  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.35  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.34  2000/03/08 21:23:25  wenger
// Fixed bug 568 (crash when opening some sessions in the JavaScreen):
// reset devised timeout in jspop to a better value; JavaScreenCmd now
// deals better with the possibility of a communications failure.
//
// Revision 1.33  2000/03/06 08:09:19  hongyu
// fix the 'invalid image data' bug --- hongyu
//
// Revision 1.32  2000/02/22 10:00:48  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/02/18 22:21:15  wenger
// Various changes to make cron scripts work better with new two-machine
// setup: added -id argument to devise, jspop, jss; updated cron scripts
// that check status of jspop, etc.; improved usage messages of jspop,
// jss, js; improved DEVise.kill script; removed obsolete sections of
// Java code.
//
// Revision 1.30  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.28  2000/02/14 09:26:24  hongyu
// *** empty log message ***
//
// Revision 1.27  2000/01/19 20:41:03  hongyu
// *** empty log message ***
//
// Revision 1.26  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.25  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.24  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.23  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseServer.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

import java.net.*;
import java.io.*;
import java.util.*;

public class DEViseServer implements Runnable, DEViseCheckableThread
{
    private static int _debugLvl = 1;

    private jspop pop = null;

    private Process proc = null;
    private Thread serverThread = null;

    public String hostname = "localhost"; // hostname of jss/devised
    //public int dataPort = 0;
    public int cmdPort = 0, switchPort = 0;

    public int jssport = 0; // port the jss is listening on

    private DEViseCommSocket socket = null;
    private static final int devisedTimeout = 180 * 1000; // milliseconds
    //TEMP -- increased this from 1000 to see if it helps on yola.
    private static final int socketTimeout = 5000; // milliseconds

    // newClient is the client we're going to switch to the next time we
    // switch clients; client is the client we're currently connected to.
    private DEViseClient newClient = null;
    private DEViseClient client = null;

    private String[] serverCmds = null;

    private Vector currentDir = new Vector();
    private String rootDir = "DEViseSession";

    public static final int STATUS_STOPPED = 0, STATUS_RUNNING = 1;
    private int status = STATUS_STOPPED;
    private boolean isValid = false;

    public static final int ACTION_QUIT = 0, ACTION_IDLE = 1, ACTION_WORK = 2;
    private int action = ACTION_IDLE;

    private static int _nextObjectNum = 1;
    private int _objectNum = -1;

    private long _lastRunTime;
    
    private static boolean _createdTmpSessionDir = false;

    // The number of client commands we've run since the last client switch
    // (this is for diagnostics only).
    private int _cmdsSinceSwitch = 0;

    private static String deviseVersion = null;

    public long lastRunTime() { return _lastRunTime; }
    public void intThread() { serverThread.interrupt(); }
    public String thread2Str() { return serverThread.toString(); }
    public boolean isAlive() { return serverThread.isAlive(); }
    public static String getDeviseVersion() { return deviseVersion; }

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    // name is the name of the system that the jss and devised(s) are running
    // on; port is the jssport.
    public DEViseServer(jspop j, String name, int port, int cmdport,
      int imgport)
    {
        if (_debugLvl >= 4) {
            System.out.println("DEViseServer(" + _nextObjectNum +
	      ") constructor");
        }

        pop = j;
        if (name != null) {
            hostname = new String(name);
        }

        cmdPort = cmdport;
        //dataPort = imgport;
        jssport = port;
        isValid = true;

	_objectNum = _nextObjectNum++;
    }

    protected void finalize()
    {
        if (_debugLvl >= 2) {
            System.out.println("DEViseServer(" + _objectNum + ").finalize()");
        }

        // In case this somehow didn't get unregistered before (e.g.,
        // we got stop() call).
        DEViseThreadChecker.getInstance().unregister(this);
    }

    // Returns STATUS_*.
    public int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public void stop()
    {
        if (getStatus() != STATUS_STOPPED) {
            if (serverThread != null) {
                serverThread.stop();
                serverThread = null;
            }

            setStatus(STATUS_STOPPED);

            closeSocket();

	    pop.pn("Removing server in DEViseServer.stop()");
            stopDEVise();
        }
    }

    public void start() throws YException
    {
        pop.pn("Trying to start a DEViseServer ...");

        // connect to the devised
        if (!startSocket()) {
            throw new YException("Can not start DEViseServer");
        }

        // start server Thread but first check if server thread is still active
        if (getStatus() == STATUS_STOPPED) {
            setStatus(STATUS_RUNNING);
            setAction(ACTION_IDLE);
            serverThread = new Thread(this);
	    serverThread.setName("DEViseServer " + _objectNum);
            serverThread.start();
        }

	// Make sure the JSPoP's protocol version is compatible with the
	// devised.
	processClientCmd(DEViseCommands.PROTOCOL_VERSION + " " +
	  DEViseGlobals.PROTOCOL_VERSION);

	// Get the DEVise version (so we can report it to the client
	// if they request it).
	processClientCmd(DEViseCommands.GET_DEVISE_VERSION);

        pop.pn("Successfully started a DEViseServer ...");

	//
	// Have the DEVised create or clear the temporary session directory
	// if we haven't already done it.
	//
	String hostname = "unknown";
	try {
            InetAddress address = InetAddress.getLocalHost();
	    hostname = address.getHostName();
	} catch (UnknownHostException ex) {
	    System.err.println("Unable to get local host: " +
	      ex.getMessage());
	}

	if (!_createdTmpSessionDir) {
	    System.out.println(
	      "Creating or clearing temporary session directory");

	    processClientCmd(DEViseCommands.CREATE_TMP_SESSION_DIR +
	      " " + hostname + " " + pop.getCmdPort());
	    _createdTmpSessionDir = true;
	} else {
	    processClientCmd(DEViseCommands.SET_TMP_SESSION_DIR +
	      " " + hostname + " " + pop.getCmdPort());
	}
    }

    private synchronized void closeSocket()
    {
        if (socket != null) {
            socket.closeSocket();
            socket = null;
        }
    }

    // Connect a socket to the devised.
    private synchronized boolean startSocket()
    {
        if (_debugLvl >= 1) {
            System.out.println("DEViseServer(" + _objectNum +
	      ").startSocket()");
        }

        // close previous connection first if any
        closeSocket();

        currentDir = new Vector();
        currentDir.addElement(new String(rootDir));

        // restart devised
        if (!startDEVise()) {
            pop.pn("Can not start a new devised");
            return false;
        }

        int time = 0, timestep = 1000;

        while (time < devisedTimeout) {
            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            time += timestep;

            try {
                socket = new DEViseCommSocket(hostname, cmdPort,  socketTimeout);

                pop.pn("Successfully connect to devised ...");

                return true;
            } catch (YException e) {
                pop.pn("Can not connect to devised, wait to try again ...");
                closeSocket();
            }
        }

        // can not connect within timeout
        closeSocket();
	pop.pn("Removing server because we can't connect within the timeout");
        stopDEVise();

        pop.pn("Can not connect to devised within timeout");

        return false;
    }

    private void stopDEVise()
    {
        isValid = false;
        pop.removeServer(this);
    }

    private boolean startDEVise()
    {
        return isValid;
    }

    public boolean isAvailable()
    {
        if (newClient == null) {
            return true;
        } else {
            return false;
        }
    }

    public DEViseClient getCurrentClient()
    {
        return client;
    }

    // Set the client to switch to the next time we switch clients.
    public synchronized void setCurrentClient(DEViseClient c)
    {
        if (_debugLvl >= 1) {
            pop.pn("DEViseServer(" + _objectNum +
	      ").setCurrentClient(" + c.getConnectionID() + ") in thread " +
	      Thread.currentThread());
	    System.out.println("  current client is: " +
	      ((client != null) ? (new Long(client.getConnectionID()).toString()) :
	      "null"));
        }

        if (c == null) {
            return;
        }

        if (c.getStatus() == DEViseClient.SERVE) {
	    System.err.println("ERROR!!! New client (" + c.getConnectionID() +
	      ") is already connected to a server!!!");
	    return;
	}

        c.setStatus(DEViseClient.SERVE);
        newClient = c;
        notifyAll();
    }

    // Returns ACTION_*.
    private synchronized int getAction()
    {
        while (action == ACTION_IDLE && newClient == null) {
            try {
	        DEViseThreadChecker.getInstance().unregister(this);
		if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
		      ").getAction() before wait()");
		}
                wait();
		if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
		      ").getAction() after wait()");
		}
	        DEViseThreadChecker.getInstance().register(this);
            } catch (InterruptedException e) {
            }
        }

        if (action == ACTION_QUIT) {
            return action;
        }

        if (client == null && newClient == null) {
            action = ACTION_IDLE;
        } else {
            if (newClient != null) { // need to switch client
                pop.pn("Client switching happened in server running on (" +
		  hostname + ")");
                switchClient(true);
                action = ACTION_WORK;
            } else {
                try {
                    //Thread.sleep(100);
            wait(100);
                } catch (InterruptedException e) {
                }
            }
        }

        return action;
    }

    private synchronized void setAction(int a)
    {
        action = a;
    }

    public void run()
    {
	if (_debugLvl >= 2) {
	    pop.pn("DEViseServer(" + _objectNum + ").run()");
	}

	DEViseThreadChecker.getInstance().register(this);

        int todo;

	//TEMP -- kind of busywaiting?? YES
	//
	// Get and process commands from the client.
	//
        while (true) {
            Date date = new Date();
            _lastRunTime = date.getTime();

            todo = getAction();

            if (todo == ACTION_QUIT) {
                quit();

                break;
            } else if (todo == ACTION_WORK && client != null) {
                // while DEViseServer thread reach here, there is no way to
		// interrupt it until some error happened or it finish
		// processing one client's request
		if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
		      ").run() starting work on a command");
                }

                serverCmds = null;

                String clientCmd = null;

		//
		// Get a single command from the client.
		//
                boolean isEnd = false;
                while (!isEnd) {
                    date = new Date();
                    _lastRunTime = date.getTime();
                    try {

			//
			// If a collaborator just connected, initialize
			// it.
			//
			if ( !client.collabClients.isEmpty() 
		          && client.collabInit) {
			    initializeCollaborator();
			}

			// Get a command from the client.
                        // This method will not block, if no command it just
			// returns null.
			if (_debugLvl >= 4) {
                            pop.pn("DIAG before " +
			      "client.getCmd() in " + serverThread);
			}

			clientCmd = client.getCmd();

			if (_debugLvl >= 4) {
                            pop.pn("DIAG after " +
			      "client.getCmd() in " + serverThread);
			}
                        isEnd = true;
                    } catch (InterruptedIOException e) {
                        // since client.getCmd() will not block, this is
			// meaningless
			if (_debugLvl >= 1) {
		            System.err.println("InterruptedIOException " +
			      e.getMessage() + " in DEViseServer(" +
			      _objectNum + ").run()");
		        }
                    } catch (YException e) {
                        pop.pn("Client communication error1: " + e.getMsg());
                        removeCurrentClient();

                        isEnd = true;
                        clientCmd = null;
                    }
                }

		if (clientCmd != null && _debugLvl >= 1) {
		    pop.pn("Server (" + _objectNum + ") on " +
		      hostname + " got command " + clientCmd +
		      " from client " + client.getConnectionID() +
		      " in thread " + Thread.currentThread());
		}

		if (clientCmd != null) {
		    _cmdsSinceSwitch++;
		}

                if (clientCmd == null) {
                    continue;
                } else {
		    // We've just finished processing this command, so
		    // remove it from the client.
		    // We now have a reference to the command, so remove it
		    // from the client.  Note: I'm not sure why the client
		    // saves the command in the first place.  RKW 2000-11-07.
                    client.removeLastCmd();
                }

		Vector serverDatas = new Vector();

		//
		// Process the command.
                // Commands JAVAC_GetServerState, JAVAC_Abort & JAVAC_Connect
                // already been handled in DEViseClient.
		//
                try {
		    if (!processClientCmd(clientCmd)) {
			
			// close client socket for cgi version
			if (client != null && client.useCgi()) {
			    client.closeSocket();
			    pop.pn("Socket between client and cgi is closed.");
			}
			
      			continue;
                    }
		    processServerCmd(serverDatas);
                } catch (YException e) {
                    pop.pn("DEViseServer failed!");
                    pop.pn(e.getMsg());

                    if (clientCmd.startsWith(DEViseCommands.EXIT)) {
                        removeCurrentClient(false);
                    } else if (clientCmd.startsWith(
		      DEViseCommands.CLOSE_SESSION)) {
                        switchClient();
                    } else {
                        try {
                            client.sendCmd(DEViseCommands.ERROR +
			      " {Communication error occurs while talk to devised}");
                            switchClient();
                        } catch (YException e1) {
                            pop.pn("Client communication error2: " +
			      e1.getMsg());
                            removeCurrentClient(false);
                        }
                    }

                    setAction(ACTION_QUIT);

                    continue;
                }

		if (clientCmd != null && _debugLvl >= 1) {
		    pop.pn("Devised on " + hostname +
		      " is done processsing command " + clientCmd +
		      " from client " + client.getConnectionID() +
		      " in thread " + Thread.currentThread());
		}

		//
		// Send response from server back to the client.
		//
                try {
                    client.sendCmd(serverCmds);
		    
		    client.sendData(serverDatas);
		    pop.pn("Done sending all data to client(s)");
		    
		    if (client.useCgi()) {
			client.closeSocket();
			pop.pn("Socket between client and cgi is closed.");
		    }
                } catch (YException e) {
                    pop.pn("Client communication error3: " + e.getMsg());
                    removeCurrentClient();
		}
		if (_debugLvl >= 4) {
                    pop.pn("DEViseServer(" + _objectNum +
		      ").run() finished work on a command");
                }
            }
        }

        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum + ").run() end");
	}
	DEViseThreadChecker.getInstance().unregister(this);
    }

    // Send a command that does not rely on client state.
    public void sendStatelessCmd(String cmd) throws YException
    {
        if (_debugLvl >= 2) {
            pop.pn("DEViseServer(" + _objectNum +
	      ").sendStatelessCmd(" + cmd + ")");
        }

	//
	// Test for a legal (statelesss) command.
	//
	if (!cmd.startsWith(DEViseCommands.PROTOCOL_VERSION) &&
	  !cmd.startsWith(DEViseCommands.CREATE_TMP_SESSION_DIR) &&
	  !cmd.startsWith(DEViseCommands.DELETE_TMP_SESSION)) {
	    throw new YException("Illegal command (not stateless): " + cmd);
	}


	// We need to *not* save the server commands here because doing
	// so might goof up the client (if any) connected to this server.
	sendCmd(cmd, false);
    }

// ------------------------------------------------------------------------
// Method to process client commands (commands generated by the client,
// or by the JSPoP itself).

    // Returns true if command needs the rest of the "standard" processing,
    // false otherwise.
    private boolean processClientCmd(String clientCmd)
      throws YException
    {
	if (_debugLvl >= 2) {
	    pop.pn("DEViseServer(" + _objectNum +
	      ").processClientCmd(" + clientCmd + ")");
	}

	// Log this command to the usage log.  Note:  client heartbeats
	// and some other commands are consumed in DEViseClient before
	// they get here.
	if (client != null) {
            pop.logUsage(client.hostname + " (" + client.ID + "): " +
	      clientCmd);
	}
	 
        if (clientCmd.startsWith(DEViseCommands.PROTOCOL_VERSION)) {
	    cmdProtocolVersion(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.GET_DEVISE_VERSION)) {
	    cmdGetDeviseVersion(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.EXIT)) {
	    cmdExit();

            // no need to return any response to client
	    //TEMP -- should probably return a response here to fix bug 630.
            return false;

        } else if (clientCmd.startsWith(DEViseCommands.CLOSE_SESSION)) {
	    cmdCloseSession();

        } else if (clientCmd.startsWith(DEViseCommands.GET_SESSION_LIST)) {
	    cmdGetSessionList(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
	    cmdSetDisplaySize(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.OPEN_SESSION)) {
	    cmdOpenSession(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SAVE_CUR_SESSION)) {
	    cmdSaveSession();
	    return false;

        } else if (clientCmd.startsWith(DEViseCommands.REOPEN_SESSION)) {
	    cmdReopenSession();

        } else if (clientCmd.startsWith(
	  DEViseCommands.CREATE_TMP_SESSION_DIR)) {
	    cmdCreateTmpSessionDir(clientCmd);

        } else if (clientCmd.startsWith(DEViseCommands.SET_TMP_SESSION_DIR)) {
	    cmdSetTmpSessionDir(clientCmd);

        } else {
	    cmdClientDefault(clientCmd);
        }

	return true;
    }

// ------------------------------------------------------------------------
// Method to process server commands (commands generated by the server
// (DEVised) in response to a command from the client or JSPoP).  Basically,
// what we do here is pass the command back to the client, and, if necessary,
// get the associated data from the server so we can pass that along to
// the client as well.

    private void processServerCmd(Vector serverDatas) throws YException
    {
        // need to get whatever data is sending over by devised along with
	// the commands since serverCmds always ends with a JAVAC_Error or
	// JAVAC_Fail or JAVAC_Done, so we do not need to check the last
	// command
        if (!serverCmds[serverCmds.length - 1].startsWith(DEViseCommands.DONE)) {
            String tmpcmd = serverCmds[serverCmds.length - 1];
            serverCmds = new String[1];
            serverCmds[0] = tmpcmd;
        }

        for (int i = 0; i < (serverCmds.length - 1); i++) {
            if (serverCmds[i].startsWith(DEViseCommands.UPDATE_VIEW_IMAGE)) {
	        cmdUpdateViewImage(serverCmds[i], serverDatas);
            } else if (serverCmds[i].startsWith(DEViseCommands.UPDATE_GDATA)) {
		cmdUpdateGData(serverCmds[i], serverDatas);
            }
        }
    }

// ------------------------------------------------------------------------
// Methods to process specific client commands.

    private void cmdProtocolVersion(String clientCmd)
      throws YException
    {
        if (!sendCmd(clientCmd)) {
	    // Note: we're exiting here because otherwise we will just keep
	    // trying to start a devised, and failing.  RKW 2001-11-12.
	    System.err.println("JSPoP exiting because of protocol " +
	      "mismatch with devised!!");
	    System.exit(1);
        }
    }

    private void cmdGetDeviseVersion(String clientCmd)
      throws YException
    {
        if (sendCmd(clientCmd)) {
	    String[] args = DEViseGlobals.parseString(serverCmds[0]);
	    if (args == null || args.length < 1) {
		throw new YException(
		  "Ill-formated command received from server \"" +
		  serverCmds[0]);
	    } else {
		if (args[0].equals(DEViseCommands.DEVISE_VERSION)) {
	            if (deviseVersion == null) {
		        deviseVersion = args[1];
		    } else {
		        if (!deviseVersion.equals(args[1])) {
		            System.err.println(
			      "Warning: version of latest DEVised (" + args[1] +
			      ") is not the same as version of first DEVised (" +
			      deviseVersion + ")");
		        }
		    }
		} else {
		    throw new YException("Unexepected command " +
		      serverCmds[0] + " received in response to " + clientCmd);
		}
	    }
        } else {
	    throw new YException("Unable to send command: " + clientCmd);
	}
    }

    public void cmdSaveSession() throws YException
    {
	pop.pn("We send the save_session command.");
	cmdClientDefault(DEViseCommands.SAVE_TMP_SESSION + " {" +
	  client.savedSessionName + "}");
	client.sessionSaved = true;
	client.tmpSessionExists = true;
	pop.pn("We send the close_session command.");
	cmdCloseSession();
	// keep the current session opened
	if (!client.isSessionOpened) {
	    client.isSessionOpened = true;
	}
    }

    public void cmdReopenSession() throws YException
    {
	pop.pn("We send the reopen-session command.");
	sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" + client.savedSessionName + "}");
	client.sessionSaved = false;
	if ( !client.isSessionOpened )
	    client.isSessionOpened = true;
    }

    public void cmdExit() throws YException
    {
	cmdCloseSession();
	cmdDeleteSession();
        removeCurrentClient(false);
    }

    private void cmdCloseSession() throws YException
    {
        client.isClientSwitched = false;
        client.isSwitchSuccessful = false;
        if (client.isSessionOpened) {
            client.isSessionOpened = false;
            sendCmd(DEViseCommands.CLOSE_SESSION);
            currentDir = new Vector();
            currentDir.addElement(rootDir);
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.DONE;
        }

	// also close the collaboration JSs
	if (!client.collabInit) {
	    for (int i = 0; i < client.collabClients.size(); i++) {
		DEViseClient collabClient = 
		    (DEViseClient)client.collabClients.elementAt(i);			
		pop.pn("Sending command " + DEViseCommands.CLOSE_SESSION
		       + " to collabration client" + " " + i);
		collabClient.sendCmd(DEViseCommands.CLOSE_SESSION);
	    }
	} // if (!client.collabInit)
    }

    private void cmdDeleteSession() throws YException
    {
        if (client.tmpSessionExists) {
            sendCmd(DEViseCommands.DELETE_TMP_SESSION + " " +
	      client.savedSessionName);
	    client.tmpSessionExists = false;
	}
    }

    private void cmdGetSessionList(String clientCmd) throws YException
    {
        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 2 &&
          cmds[1].startsWith(rootDir)) {
            if (cmds[1].equals(rootDir)) {
                sendCmd(DEViseCommands.GET_SESSION_LIST);
            } else {
                String p = cmds[1].substring(14);
                sendCmd(DEViseCommands.GET_SESSION_LIST + " {" + p + "}");
            }
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR +
              " {Invalid command: \"" + clientCmd + "\"}";
        }
    }

    private void cmdSetDisplaySize(String clientCmd) throws YException
    {
        boolean error = false;
        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 5) {
            try {
                client.screenDimX = Integer.parseInt(cmds[1]);
                client.screenDimY = Integer.parseInt(cmds[2]);
                client.screenResX = Integer.parseInt(cmds[3]);
                client.screenResY = Integer.parseInt(cmds[4]);
                if (client.screenDimX < 1 || client.screenDimY < 1 ||
		  client.screenResX < 1 || client.screenResY < 1) {
		    throw new NumberFormatException();
	        }
            } catch (NumberFormatException e) {
                error = true;
            }
        } else {
            error = true;
        }

        if (error) {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR + " {Invalid command: \"" +
	      clientCmd + "\"}";
            client.screenDimX = -1;
            client.screenDimY = -1;
            client.screenResX = -1;
            client.screenResY = -1;
        } else {
            sendCmd(clientCmd);
        }
    }

    private void cmdOpenSession(String clientCmd) throws YException
    {
	client.sendCmd(DEViseCommands.UPDATEJS);

        if (client.isSessionOpened) {
            client.isSessionOpened = false;
            sendCmd(DEViseCommands.CLOSE_SESSION);
            currentDir = new Vector();
            currentDir.addElement(rootDir);
        }

        String[] cmds = DEViseGlobals.parseString(clientCmd);
        if (cmds != null && cmds.length == 2 && cmds[1].startsWith(rootDir +
	  "/")) {
            String p = cmds[1].substring(14);

            client.sessionName = p;

            boolean error = false;
            if (client.screenDimX > 0 && client.screenDimY > 0 &&
	      client.screenResX > 0 && client.screenResY > 0) {
                if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
		  client.screenDimX + " " + client.screenDimY + " " +
		  client.screenResX + " " + client.screenResY)) {
                    error = true;
                }
            }

            if (!error) {
                if (sendCmd(DEViseCommands.OPEN_SESSION + " {" +
		  client.sessionName + "}")) {
                    client.isSessionOpened = true;
                } else {
                    // need to clear socket because there might be some
		    // data on data socket
                    int count;
                    try {
                        count = countData();
                    } catch (YException ee) {
                        count = -1;
                        pop.pn(ee.getMessage());
                    }
                    socket.clearSocket(count);
                }
            }
        }

        if (!client.isSessionOpened) {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.ERROR +
	      " {Can not open session \"" + client.sessionName + "\"}";
        }
    }

    private void cmdCreateTmpSessionDir(String clientCmd)
      throws YException
    {
        if (!sendCmd(clientCmd)) {
	    System.err.println("Error requesting creation/clearing of " +
	      "temporary session directory");
        }
    }

    private void cmdSetTmpSessionDir(String clientCmd)
      throws YException
    {
        if (!sendCmd(clientCmd)) {
	    System.err.println("Error setting temporary session directory");
        }
    }

// ------------------------------------------------------------------------
// Method to process all client commands not using the command-specific
// methods.

    private void cmdClientDefault(String clientCmd) throws YException
    {
	if (_debugLvl >= 2) {
	    System.out.println("DEViseServer(" + _objectNum +
	      ").cmdClientDefault(" + clientCmd + ")");
	}

        if (client.isClientSwitched) {
            client.isClientSwitched = false;

            if (client.isSwitchSuccessful) {
                client.isSwitchSuccessful = false;

                boolean error = false;
                if (client.screenDimX > 0 && client.screenDimY > 0) {
                    if (!sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " +
		      client.screenDimX + " " + client.screenDimY + " " +
		      client.screenResX + " " + client.screenResY)) {
                        error = true;
                    //} else {
                    //    pop.pn("Switch error: can not send " +
		    //      DEViseCommands.SET_DISPLAY_SIZE);
                    }
                }

                if (!error) {
                    if (sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" +
		      client.savedSessionName + "}")) {
                        client.isSessionOpened = true;
                    } else {
                        pop.pn("Switch error: Can not send " +
			  DEViseCommands.OPEN_TMP_SESSION + " " +
			  client.savedSessionName);
                    }
                    // need to clear socket because there might be some
		    // useless data on data socket 
		    //socket.clearSocket();
                }

                // need to clear socket because there might be some useless
		// data on data socket
                int count;
                try {
                    count = countData();
                } catch (YException ee) {
                    count = -1;
                    pop.pn(ee.getMessage());
                }
                socket.clearSocket(count);
		// socket.clearSocket(-1);
            } else {
                pop.pn("Switch error: client switch not successful");
            }
        }

        if (client.isSessionOpened) {
            if (!sendCmd(clientCmd)) {
                // need to clear data socket when error happened because
		// there might be some data on data socket
                int count;
                try {
                    count = countData();
		    
                } catch (YException ee) {
                    count = -1;
                    pop.pn(ee.getMessage());
                }
                socket.clearSocket(count);
            }
        } else {
            serverCmds = new String[1];
            serverCmds[0] = DEViseCommands.FAIL +
	      " {Do not have an opened session or Can not open last saved session}";
        }
    }

// ------------------------------------------------------------------------
// Methods to process specific server commands.

    private void cmdUpdateViewImage(String serverCmd, Vector serverDatas)
      throws YException
    {
        String[] cmds = DEViseGlobals.parseString(serverCmd);
        if (cmds != null && cmds.length == 3) {
            try {
                int imgSize = Integer.parseInt(cmds[2]);
                if (imgSize < 1) {
                    throw new NumberFormatException();
                }

                byte[] image = receiveData(imgSize);

                serverDatas.addElement(image);
            } catch (NumberFormatException e1) {
                throw new YException("Invalid image size in command \"" +
		  serverCmd + "\"");
            }
        } else {
            throw new YException("Ill-formated command \"" + serverCmd + "\"");
        }
    }

    private void cmdUpdateGData(String serverCmd, Vector serverDatas)
      throws YException
    {
        String[] cmds = DEViseGlobals.parseString(serverCmd);
        if (cmds != null && cmds.length == 7) {
            try {
                int dataSize = Integer.parseInt(cmds[6]);
                if (dataSize < 1) {
                    throw new NumberFormatException();
                }

                byte[] data = receiveData(dataSize);

                serverDatas.addElement(data);
            } catch (NumberFormatException e1) {
                throw new YException("Invalid GData size in command \"" +
		  serverCmd + "\"");
            }
        } else {
            throw new YException("Ill-formated command \"" + serverCmd + "\"");
        }
    }

// ------------------------------------------------------------------------

    private synchronized void quit()
    {
        closeSocket();

	pop.pn("Removing server in DEViseServer.quit()");
        stopDEVise();

        setStatus(STATUS_STOPPED);
    }

    private synchronized void removeCurrentClient()
    {
        removeCurrentClient(true);
    }

    private synchronized void removeCurrentClient(boolean isClose)
    {
        if (client != null) {
            if (isClose && client.isSessionOpened) {
                try {
                    sendCmd(DEViseCommands.CLOSE_SESSION);
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed.");
                    pop.pn(e.getMsg());

                    //if (!startSocket()) {
                        setAction(ACTION_QUIT);
                    //}
                }
            }

	    // Suspend client before closing it so we don't get the
	    // warning I just added to DEViseClient.close().  RKW 2002-04-12.
            pop.suspendClient(client);
            client.close();
            client = null;
        }
    }

    private synchronized void switchClient()
    {
        switchClient(false);
    }

    // Note: this method is sometimes called with startNewClient false,
    // to just save the state and clean up for the old client (for
    // example, if the client crashes).
    private synchronized void switchClient(boolean startNewClient)
    {
        if (_debugLvl >= 1) {
            pop.pn("DEViseServer(" + _objectNum +
	      ").switchClient(" + startNewClient + ") in thread " +
	      Thread.currentThread());
	}

        if (client != null) {
            if (_debugLvl >= 1) {
	        System.out.println("  Current client is: " + client.ID);
	    }

            if (client.isSessionOpened) {
                client.isClientSwitched = true;
                client.isSwitchSuccessful = false;
                client.isSessionOpened = false;

                try {
		    if (!client.sessionSaved) {
			if (sendCmd(DEViseCommands.SAVE_TMP_SESSION + " {" +
				    client.savedSessionName + "}")) {
			    client.isSwitchSuccessful = true;
			    client.tmpSessionExists = true;
			} else {
			    pop.pn("Can not save session for old client while switching client!");
			}
		    } else {
			client.isSwitchSuccessful = true;
                    }

                    if (!sendCmd(DEViseCommands.CLOSE_SESSION)) {
                        pop.pn("Can not close current session for old client while switching client!");
                    }
                    currentDir = new Vector();
                    currentDir.addElement(rootDir);
                } catch (YException e) {
                    pop.pn("DEViseServer failed");
                    pop.pn(e.getMsg());

                    //if (!startSocket()) {
                        setAction(ACTION_QUIT);
                    //}
                }
	    }

            pop.suspendClient(client);

	    if (_debugLvl >= 1) {
	        System.out.println("Server (" + _objectNum + ") on " +
		  hostname + " ran " + _cmdsSinceSwitch +
		  " commands for client " + client.getConnectionID() +
		  " since the last client switch");
	    }
	    _cmdsSinceSwitch = 0;

            client = null;
        } else {
            if (_debugLvl >= 1) {
	        System.out.println("  No current client");
	    }
	}

        if (startNewClient) {
            client = newClient;
            newClient = null;
            if (client != null) {
                if (_debugLvl >= 1) {
                    pop.pn("DEViseServer(" + _objectNum +
		      ") actually switched to client " +
		      client.getConnectionID());
	        }
                pop.activateClient(client);
            }
        }
    }

    private byte[] receiveData(int size) throws YException
    {
        if (socket == null)
            throw new YException("Invalid communication socket");

        boolean isEnd = false;
        byte[] data = null;
        long time = 0;

        pop.pn("Receiving data from devised(" + hostname + ") of size " + size);
        pop.pn(socket.dataAvailable() + " bytes of data available on socket");
        while (!isEnd) {
            data = socket.receiveData(size);
            pop.pn("Successfully received data from devised(" + hostname +
	      "at " + cmdPort + ") of size " + size);
            isEnd = true;
        }

        return data;
    }

    // Send a command to the server; returns true if the command was
    // successfully processed.
    private boolean sendCmd(String clientCmd) throws YException
    {
        return sendCmd(clientCmd, true);
    }

    // Send a command to the server; returns true if the command was
    // successfully processed.
    private synchronized boolean sendCmd(String clientCmd,
      boolean saveServerCmds) throws YException
    {
        if (clientCmd == null) {
            return true;
	}

        if (socket == null) {
            throw new YException("Invalid communication socket");
        }

        String response = null;
        boolean isEnd = false, isFinish = false, isError = false;
        long time;

        Vector rspbuf = new Vector();

        pop.pn("Sending command to devised(" + hostname + " at " +
	  cmdPort + ") :  \"" + clientCmd + "\"");
        socket.sendCmd(clientCmd, (short)5, 9999);

        isEnd = false;
        while (!isEnd) {
            isFinish = false;
            time = 0;

            while (!isFinish) {
                try {
                    response = socket.receiveCmd();
                    pop.pn("Receive response from devised(" + hostname +
		      " at " + cmdPort + ") :  \"" + response + "\"");
                    isFinish = true;
                } catch (InterruptedIOException e) {
		    if (_debugLvl >= 1) {
		        System.err.println("socket.receiveCmd() " +
			  "interrupted in DEViseServer(" + _objectNum +
			  ").sendCmd()");
		    }
                    time += socketTimeout;
                    if (time > devisedTimeout) {
                        throw new YException("Cannot receive response " +
			  "from devised within timeout");
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Invalid response received from devised");
            } else {
	        if (_debugLvl >= 4) {
                    pop.pn("DIAG Parsing response from devised");
		}
                String[] cmds = DEViseGlobals.parseString(response);
	        if (_debugLvl >= 4) {
                    pop.pn("DIAG Done parsing response from " +
		      "devised");
		}
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated response \"" +
		      response + "\" received from devised");
                } else {
                    //pop.pn("Receive response \"" + response +
		      //"\" from devised");

                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not
		    // the arguments?????
                    for (int j = 1; j < cmds.length; j++) {
                        cmd = cmd + " {" + cmds[j] + "}";
                    }
	            if (_debugLvl >= 4) {
                        pop.pn("DIAG Done removing(?) braces");
		    }

                    if (cmd.startsWith(DEViseCommands.DONE)) {
                        isEnd = true;
                        isError = false;
                    } else if (cmd.startsWith(DEViseCommands.ERROR) ||
		      cmd.startsWith(DEViseCommands.FAIL)) {
                        isEnd = true;
                        isError = true;
                        rspbuf.removeAllElements();
                    }

	            if (_debugLvl >= 4) {
                        pop.pn("DIAG Adding command to rspbuf");
		    }
                    rspbuf.addElement(cmd);
	            if (_debugLvl >= 4) {
                        pop.pn("DIAG Done adding command " +
			  "to rspbuf");
		    }
                }
            }
        }

	if (saveServerCmds) {
            serverCmds = new String[rspbuf.size()];
            for (int i = 0; i < rspbuf.size(); i++) {
                serverCmds[i] = (String)rspbuf.elementAt(i);
            }
	}

        return !isError;
    }

    // Determine the number of bytes on the data socket associated with
    // the commands currently in serverCmds.
    private int countData() throws YException
    {
        int count = 0;
        for (int i = 0; i < (serverCmds.length - 1); i++) {
            if (serverCmds[i].startsWith(DEViseCommands.UPDATE_VIEW_IMAGE)) {
                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                if (cmds != null && cmds.length == 3) {
                    try {
                        int imgSize = Integer.parseInt(cmds[2]);
                        if (imgSize < 1) {
                            throw new NumberFormatException();
                        }

                        count += imgSize;
                    } catch (NumberFormatException e1) {
                        throw new YException("Invalid image size in command \"" + serverCmds[i] + "\" while counting data in the socket");
                    }
                } else {
                    throw new YException("Ill-formated command \"" + serverCmds[i] + "\" while counting data in the socket");
                }
            } else if (serverCmds[i].startsWith(DEViseCommands.UPDATE_GDATA)) {
                String[] cmds = DEViseGlobals.parseString(serverCmds[i]);
                if (cmds != null && cmds.length == 7) {
                    try {
                        int dataSize = Integer.parseInt(cmds[6]);
                        if (dataSize < 1) {
                            throw new NumberFormatException();
                        }

                        count += dataSize;
                    } catch (NumberFormatException e1) {
                        throw new YException("Invalid GData size in command \"" + serverCmds[i] + "\" while counting data in the socket");
                    }
                } else {
                    throw new YException("Ill-formated command \"" + serverCmds[i] + "\" while counting data in the socket");
                }
            }
        }

        return count;
    }

    private void initializeCollaborator() throws YException
    {
        if (_debugLvl >= 1) {
	    pop.pn("DEViseServer(" + _objectNum +
	      ").initializeCollaborator()" + " in thread " +
	      Thread.currentThread());
	}

	//
	// We save and re-open the current session so that the re-opening
	// generates all of the commands needed to initialize a new
	// collaboration follower.
	//
        pop.pn("We send the save-session command.");
	processClientCmd(DEViseCommands.SAVE_TMP_SESSION + " {" +
			 client.savedSessionName + "}");
        // only when isSessionOpened = true, 
	// this save_tmp_session command can be sent out
	if (client.isSessionOpened)
	    client.tmpSessionExists = true;

	pop.pn("We send the close-session command.");
	//server.serverCmds = null;
	processClientCmd(DEViseCommands.CLOSE_SESSION);

	// keep the current session opened
	if ( !client.isSessionOpened ) {
	    client.isSessionOpened = true;
	}
	    
	if (client.tmpSessionExists) {
	    pop.pn("We send the open-session command.");
	    sendCmd(DEViseCommands.OPEN_TMP_SESSION + " {" +
		    client.savedSessionName + "}");
	}

	Vector serverDatas = new Vector();
	processServerCmd(serverDatas);
			    
        client.sendCmd(serverCmds);
	client.sendData(serverDatas);
	pop.pn("Done sending all data to " + "collaboration client");
	pop.pn("  client.collabInit = " + client.collabInit);
    }
}
