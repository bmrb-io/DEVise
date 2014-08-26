// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains the main() method for the jspop.  The main this
// class does, besides various startup stuff, is to listen for new
// connections from JavaScreens.  It also maintains lists of all of
// the clients (JavaScreens) and servers (DEViseServers/deviseds).


// There is one instance of this class for a given jspop process.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.82  2009/08/28 15:59:39  wenger
// Minor code cleanups.
//
// Revision 1.81  2005/12/06 20:00:24  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.80  2003/01/13 19:23:45  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.79  2002/06/17 19:40:20  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.78  2002/05/01 21:29:00  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.77.2.20  2005/11/07 21:25:04  wenger
// Added timestamps to many more items in the JSPoP debug output.
//
// Revision 1.77.2.19  2004/09/29 19:08:37  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.77.2.18  2004/05/12 22:00:00  wenger
// Removed a bunch of temporary stuff from the jspop_debug_0405_br
// branch.
//
// Revision 1.77.2.17  2004/05/12 21:43:59  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.77.2.16.4.10  2004/09/29 18:16:48  wenger
// (Hopefully) final cleanup of the jspop_debug_0405_br branch -- some
// changes to DEViseClientSocket, and a little more debug output in
// jspop.
//
// Revision 1.77.2.16.4.9  2004/09/21 19:38:13  wenger
// Misc. cleanup before merging back into 1.7 (DEViseClientSocket.java
// still needs some changes).
//
// Revision 1.77.2.16.4.8  2004/09/10 23:04:24  wenger
// Seem to have fixed the problem of the JSPoP hanging on RedHat 9
// (changed synchronization in the jspop class); still more diagnostic
// output; added config file for local install on trigger (for testing).
//
// Revision 1.77.2.16.4.7  2004/09/08 16:52:32  wenger
// More diagnostics -- committing basically the code that reproduced
// the hanging problem on 2004-09-08 (just some comments added).
//
// Revision 1.77.2.16.4.6  2004/09/03 19:00:52  wenger
// More diagnostic output and debug comments; version is now 5.2.2x3.
//
// Revision 1.77.2.16.4.5  2004/09/03 17:26:54  wenger
// More diagnostic output to try to figure out exactly where the deadlock
// is happening.
//
// Revision 1.77.2.16.4.4  2004/06/29 15:16:20  wenger
// Fixed some problems with the JSPoP debug code.
//
// Revision 1.77.2.16.4.3  2004/05/12 21:27:27  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.77.2.16.4.2  2004/05/10 22:28:52  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.77.2.16.4.1  2004/05/10 19:39:00  wenger
// Lots of new debug code, turned on at compile time; no significant
// functional changes; also has comments about where we might be
// getting hung, based on debug logs.
//
// Revision 1.77.2.16  2003/10/28 21:56:32  wenger
// Moved determination of JSPoP client hostname to DEViseClientSocket
// thread (out of main jspop thread).
//
// Revision 1.77.2.15  2003/10/28 20:41:37  wenger
// Added debug code (including that used to find bug 889).
//
// Revision 1.77.2.14  2003/10/15 21:55:12  wenger
// Added new JAVAC_StopCollab command to fix ambiguity with
// JAVAC_CollabExit; minor improvements to collaboration-related stuff
// in the auto test scripts.
//
// Revision 1.77.2.13  2003/09/23 22:16:55  wenger
// JSPoP prints version and arguments to log file at startup.
//
// Revision 1.77.2.12  2003/09/23 21:55:13  wenger
// "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.
//
// Revision 1.77.2.11  2003/06/25 21:34:10  wenger
// JSPoP debug log improvements to try to track down the cause of some
// DEVised restarts.
//
// Revision 1.77.2.10  2003/06/17 22:44:39  wenger
// Session opens are now logged in a separate JS usage log; the code
// that generates the usage summary emails is not yet updated to
// use that log.
//
// Revision 1.77.2.9  2003/06/17 21:04:57  wenger
// Major improvements to command-line argument processing of all JavaScreen
// programs; we now save the -id value in the JSPoP to use for the usage
// log file; some minor cleanups of the auto test scripts; slight
// clarification of command documentation.
//
// Revision 1.77.2.8  2003/03/13 19:40:23  wenger
// Fixed bug 866 (JSPoP fails on bad collaboration name from follower).
//
// Revision 1.77.2.7  2002/12/05 20:38:20  wenger
// Removed a bunch of unused (mostly already-commented-out) code to
// make things easier to deal with.
//
// Revision 1.77.2.6  2002/11/05 20:02:28  wenger
// Fixed bug 831 (JSPoP can't respond if stuck sending data); incremented
// DEVise and JavaScreen versions.
//
// Revision 1.77.2.5  2002/06/17 17:30:37  wenger
// Added a bunch more error reporting and put timestamps on check_pop logs
// to try to diagnose JSPoP restarts.
//
// Revision 1.77.2.4  2002/04/19 20:50:49  xuk
// Add new testings in autotest: enforced client switching, restore pre-collab
// states for JS;
// Split autotest scripts into 2 parts.
//
// Revision 1.77.2.3  2002/04/18 17:25:11  wenger
// Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
// temporary session files when the JSPoP and DEViseds are on different
// machines).  Note: JS protocol version is now 11.0.
//
// Revision 1.77.2.2.2.2  2002/04/18 15:41:00  wenger
// Further cleanup of JavaScreen temporary session file code (added
// JAVAC_DeleteTmpSession command) (includes fixing bug 774).
//
// Revision 1.77.2.2.2.1  2002/04/17 17:45:56  wenger
// DEVised, not JSPoP, now does the actual work of creating or clearing
// the temporary session directory (new command from client to DEVised
// means that communication protocol version is now 11.0).  (Client
// switching is not working yet with this code because I need to change
// how temporary sessions are saved and loaded.)
//
// Revision 1.77.2.2  2002/04/16 17:59:40  wenger
// Added command-line argument for client kill interval to the JSPoP.
//
// Revision 1.77.2.1  2002/04/12 16:08:50  wenger
// Lots of cleanup to the heartbeat checking code -- tested killing a
// client because the heartbeat timeout expired, and because we have
// too many clients.
//
// Revision 1.77  2002/03/20 22:12:25  xuk
// Added automatic collaboration functionality.
// Added collabNames vector, findClientByCollabName() method;
// Modified setUpCollab() and getCollabLeaders() methods.
//
// Revision 1.76  2002/01/24 23:03:36  xuk
// *** empty log message ***
//
// Revision 1.75  2001/11/28 21:56:21  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.74  2001/11/19 17:17:03  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.73  2001/11/13 17:57:01  xuk
// Could send command in String[] format, no need to compose a long command string before sending.
//
// Revision 1.72.2.2  2001/11/21 22:11:16  wenger
// Fixed the JSPoP deadlock problem caused by a new client connection
// happening while in the middle of a client switch.
//
// Revision 1.72.2.1  2001/11/13 20:31:36  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.72  2001/11/07 22:31:30  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.71  2001/11/07 19:27:21  xuk
// Garbage collection for temporary session files.
// In jspop(), create a subdir for each jspop. Delete temporary session files.
//
// Revision 1.70  2001/11/06 16:27:20  xuk
// Reset collaboration follower's screen size and resolution to deal with different screen size from leaders.
//
// Revision 1.69  2001/10/30 17:39:15  xuk
// reated DEViseClient object for collaborating clients in jspop.
// 1. In processFirstCommand(), no need to check collaboration flag;
// 2. Deleted onCollab();
// 3. Changed sendCollabLeaders() to getCollabLeaders(), which is called by
// DEViseClient object when getting JAVAC_AskCollabLeader command;
// 4. Modified setUpCollab(), which is called by DEViseClient object when
// getting JAVAC_Collaborate command.
//
// Revision 1.68.2.2  2001/11/07 17:22:37  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.68.2.1  2001/10/29 00:05:15  wenger
// Minor debug output fix.
//
// Revision 1.68  2001/10/24 22:15:33  wenger
// More collaboration-related fixes.
//
// Revision 1.67  2001/10/24 20:49:40  wenger
// Fix to previous fix, which is also a workaround to bug 722.
//
// Revision 1.66  2001/10/24 18:53:11  wenger
// Fixed bug 719 (JSPoP deals with invalid client IDs better).
//
// Revision 1.65  2001/10/24 17:46:07  wenger
// Fixed bug 720 (one client can block others in the JSPoP).  The fix is that
// the JSPoP now has a separate thread to read from each client.
//
// Revision 1.64  2001/10/22 18:38:24  wenger
// A few more cleanups to the previous fix.
//
// Revision 1.63  2001/10/22 18:06:50  wenger
// Fixed bug 718 (client sockets never deleted in JSPoP).
//
// Revision 1.62  2001/10/19 18:39:48  wenger
// Changed socket receive timeout to 1000 millisec as emergency fix for
// JSPoP restart problem.
//
// Revision 1.61  2001/10/12 19:12:42  wenger
// Changed client ID generation to avoid any chance of duplicates;
// updated command format documentation.
//
// Revision 1.60  2001/10/12 01:47:32  xuk
// Using timestamp-based client ID.
// 1. Got rid of _nextClientId;
// 2. Modified getID() method, now client ID is from current timestamp;
// 3. In handleCommection(), onCollab(), findClientById(), getNextRequestingClient() all id has been expanded to long int.
//
// Revision 1.59  2001/10/05 15:51:35  wenger
// Minor cleanup: fixed usage messages, improved argument checking.
//
// Revision 1.58  2001/09/28 19:41:46  xuk
// *** empty log message ***
//
// Revision 1.57  2001/09/28 19:38:06  xuk
// *** empty log message ***
//
// Revision 1.56  2001/09/28 19:00:07  xuk
// JavaScreen command log playback.
// Added clientLog variable to indicated whether record command log for each DEViseClient object;
// Modified checkArguments() to check a new command-line argument: -clientlog
// -clientlogon: turn on command log functionality
// -clientlogoff: turn off command log functionality
//
// Revision 1.55  2001/09/10 21:23:03  xuk
// Solve the client disconnection problem.
//
// Revision 1.54  2001/08/21 18:37:16  wenger
// JSPoP now responds to JAVAC_CheckPop with JAVAC_Error if no deviseds
// are connected to it; fixed up redirection of stderr in sh scripts.
//
// Revision 1.53  2001/05/11 20:36:15  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.52  2001/04/25 19:41:50  xuk
// Fixed bug 661: when a JS goes into Collaboration mode from Socket mode,
// It will not become an available collaboration leader.
//
// Revision 1.51  2001/04/12 18:18:17  wenger
// Added more diagnostic code to the JSPoP.
//
// Revision 1.50  2001/04/11 21:16:28  xuk
// A collaboration leader could find out the followers hostname.
//
// Revision 1.49  2001/04/11 16:49:39  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//
// Revision 1.48  2001/04/06 19:32:15  wenger
// Various cleanups of collaboration code (working on strange hang
// that Miron has seen); added more debug output; turned heartbeat
// back on (it somehow got turned off by accident).
//
// Revision 1.47  2001/03/19 23:10:31  xuk
// Fixed bug for no available JavaScreen for collaboration.
//
// Revision 1.46  2001/03/04 22:03:04  xuk
// Disabled some buttons when JS in "collaboration" mode.
//
// Revision 1.45  2001/03/03 20:14:02  xuk
// Restore old state if user goes into, then out of, collaboration mode.
//
// Revision 1.44  2001/02/23 17:41:41  xuk
// Added machine name and session name on the client list sent to collaboration JS.
//
// Revision 1.43  2001/02/23 16:07:46  xuk
// Don't display client's ID on the collaboration side, when it disables collaboration.
// Changes in OnCollab().
//
// Revision 1.42  2001/02/21 17:49:37  xuk
// Added the collaboration security features.
// Changes in OnCollab() for checking collaboration password.
//
// Revision 1.41  2001/02/20 20:02:26  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.40  2001/02/16 17:56:38  xuk
// Added new command and GUI for collect active client ID list.
// Changed OnCollab() sends JAVAC_Clients command after receives the first round collaboration connection request.
//
// Revision 1.39  2001/02/12 02:54:17  xuk
// JavaScreen can prevent from being collaborated.
// Changes in OnCollab(), check collaboration status with client.
//
// Revision 1.38  2001/02/06 16:33:45  xuk
// Change for multiple collaborate JSs.
//
// Revision 1.37  2001/01/31 22:19:02  xuk
// Modify onCollab(), for wrong collaboration JS ID. Send JAVAC_ERROR to client.
//
// Revision 1.36  2001/01/30 03:04:46  xuk
// Add collabration function onCollab().
//
// Revision 1.35.2.3  2001/02/13 23:36:31  wenger
// Don't explicitly request garbage collection as often, because it
// uses up loads of CPU on SGI machines.
//
// Revision 1.35.2.2  2001/02/05 22:36:32  wenger
// Changed all socket recieve timeouts to 5 sec. because of problems on
// SGIs at BMRB.
//
// Revision 1.35.2.1  2001/01/31 17:44:03  wenger
// Cron job to check jspop now runs every minute on yola; added more
// diagnostic output to checking; temporarily? increased socket receive
// timeouts to see if this helps on yola; added timestamp to
// JAVAC_CheckPop command.
//
// Revision 1.35  2001/01/27 00:11:15  wenger
// Minor improvements to diagnostic output.
//
// Revision 1.34  2001/01/26 22:21:39  wenger
// Fixed up the jspop because Kai's last commit undid my previous changes.
//
// Revision 1.33  2001/01/23 22:58:54  xuk
// *** empty log message ***
//
// Revision 1.32  2001/01/22 17:08:13  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.31  2001/01/08 20:31:58  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.29.4.14  2000/12/21 23:28:28  wenger
// Got CGI mode working with applet, minor cleanup still needed; more debug
// output in DEViseCommSocket, jspop, and js.cgi; comments updated for
// new command format that always includes ID and CGI flag.
//
// Revision 1.29.4.13  2000/12/07 17:31:16  xuk
// Maintain a vertor of active sockets.
// Active sockets are checked in getNextRequiringClient() and assigned to porper client object.
//
// Revision 1.29.4.12  2000/11/22 17:44:01  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.29.4.11  2000/11/15 18:52:05  wenger
// ID temporarily rolls over at Short.MAX_VALUE instead of Integer.MAX_VALUE
// because we are only sending shorts on the socket.
//
// Revision 1.29.4.10  2000/11/10 15:50:38  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.29.4.9  2000/11/08 18:21:39  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.29.4.8  2000/11/02 04:30:07  xuk
// *** empty log message ***
//
// Revision 1.29.4.7  2000/11/01 22:14:18  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.29.4.6  2000/10/23 20:02:42  xuk
// Close oldest client object when the number of client objects hits maxClient in getID();
// Add command-line argument "maxclient" for maximum number of client objects (default is DEViseGlobals.DEFAULTMAXCLIENT).
//
// Revision 1.29.4.5  2000/10/18 20:28:12  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.30  2000/09/12 20:51:29  wenger
// Did some cleanup of the command-related code, better error messages from JSS.
//
// Revision 1.29.4.4  2000/10/09 16:38:22  xuk
// 1. Add runTime variable to remember the latest time receiving a command from JS.
// 2. In run() fucntion, every one minute check all DEViseClient objects to remove the ones didn't received commands from JS over 10 minutes. Also, whenever receives a command from JS, update the runTime variable of corresponding DEViseClient object.
//
// Revision 1.29.4.3  2000/10/02 20:15:42  xuk
// Modify run() function to switch client when old JS comes in.
//
// Revision 1.29.4.2  2000/10/02 19:15:04  xuk
// Set up a new socket between JS and DEViseClient for each command sent from JS.
// Get the JS id from command. If id=-1, construct a new DEViseClient object, and find an idle DEViseServer for it. Otherwise, find the client from running DEViseServers according to the id.
//
// Revision 1.29.4.1  2000/09/01 20:20:14  xuk
// 1. delete dataServerSocket.
// 2. in constructor jspop(), delete start data server socket part.
// 3. in quit(), delete close data server socket part.
// 4. in run(), delete listen to data socket part.
//
// Revision 1.29  2000/06/05 16:35:08  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.28  2000/04/05 15:42:24  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.27  2000/04/05 06:25:40  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.26  2000/03/30 19:14:36  wenger
// Improved error messages for socket creation failures.
//
// Revision 1.25  2000/03/23 16:26:19  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.24  2000/02/22 10:00:48  hongyu
// *** empty log message ***
//
// Revision 1.23  2000/02/18 22:21:17  wenger
// Various changes to make cron scripts work better with new two-machine
// setup: added -id argument to devise, jspop, jss; updated cron scripts
// that check status of jspop, etc.; improved usage messages of jspop,
// jss, js; improved DEVise.kill script; removed obsolete sections of
// Java code.
//
// Revision 1.22  2000/02/15 20:50:42  hongyu
// *** empty log message ***
//
// Revision 1.21  2000/02/14 10:45:23  hongyu
// *** empty log message ***
//
// Revision 1.20  2000/02/14 09:26:32  hongyu
// *** empty log message ***
//
// Revision 1.19  2000/01/19 20:41:13  hongyu
// *** empty log message ***
//
// Revision 1.18  1999/08/03 05:56:51  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.17  1999/06/23 20:59:21  wenger
// Added standard DEVise header.
//

// ========================================================================

// jspop.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private static final String usage = new String(
      "Usage: java JavaScreen.jspop [options]\n" +
      "  -id<string>: ID for ps (default: none)\n" +
      "  -cmdport<number>: port for command socket from devised (default: " +
      DEViseGlobals.DEFAULTCMDPORT + ")\n" +
      "  -userfile<filename>: file containing user info (default: users.cfg)\n" +
      "  -logfile<filename>: client log info file (default: clients.log)\n" +
      "  -clientlog[on|off]: turn on|off client command logs (default: on)\n" + 
      "  -jspopport<number>: port on which jspop listens for jss connections\n    (default: " + DEViseGlobals.JSPOPPORT + ")\n" +
      "  -maxclient<number>: maximum number of client objects (default: " +
      DEViseGlobals.DEFAULTMAXCLIENT + ")\n" +
      "  -killint<number>: client kill interval in hours (<= 0 to disable)\n" +
      "    (default: 24.0)\n" +
      "  -usage: print this message");
    // -CMDPORT[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    // -IMGPORT[port]:
    //      port: The image port, if blank, use the defaults
    //      default: 6688
    // -LOGFILE[filename]:
    //      filename: The name of the file that contain client log information
    //      default: clients.log
    // -USERFILE[filename]:
    //      filename: The name of the file that contain users information
    //      default: users.cfg
    // -DEBUG[number]:
    //      number: The debug level for writing debug information to console
    //      default: No Debug information is written
    //

    private static final int SOCK_REC_TIMEOUT = 100 * 1000; // milliseconds

    private ServerSocket cmdServerSocket = null;
    //private ServerSocket dataServerSocket = null;

    private String userFileName = new String("users.cfg");
    private String logFileName = new String("clients.log");
    private int logLevel = 0;
    private YLogFile logFile = null;
    private static int _debugLvl = 1;
    private YLogConsole debugConsole = null;

    private Thread popThread = null;

    private JssHandler jssHandler = null;
    private int jspopPort = DEViseGlobals.JSPOPPORT;
    private DEViseClientDispatcher dispatcher = null;

    private Hashtable users = new Hashtable();
    private Vector servers = new Vector();

    private int _lastClientId = 0;

    // Clients that are not connected to a devised.
    private Vector suspendClients = new Vector();

    // Clients that are connected to a devised.
    private Vector activeClients = new Vector();

    // Sockets that are connected to a JS.
    private Vector activeSockets = new Vector();

    // Maximum number of client objects allowed.
    private int maxClient = DEViseGlobals.DEFAULTMAXCLIENT;

    // Number of milliseconds we can go without getting a client heartbeat
    // before we kill that client.
    private int _killInterval = DEViseGlobals.KILLINTERVAL;

    private int _popCmdPort = 0;

    private String _popId = null;
    private CircularLog _usageLog = null;

    private DEViseThreadChecker _threadChecker = null;
    private DEViseJspopDebug _debugThread = null;

    // enable client log or not
    public boolean clientLog = true;

    // saving temporary session files in one directory
    public String sessionDir = null;

    // all collaboration names
    public Vector collabNames = new Vector();

    //----------------------------------------------------------------------

    public static void main(String[] args) throws YException
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.err.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

	// Print JSPoP version information and arguments.
	System.out.println("JSPoP version " + DEViseGlobals.VERSION);
	System.out.println("Arguments:");
	for (int index = 0; index < args.length; index++) {
	    System.out.println("  <" + args[index] + ">");
	}

        jspop popServer = new jspop(args);
        popServer.start();
    }

    // ------------------------------------------------------------------
    // Set the debug level for this class.
    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    //----------------------------------------------------------------------

    public void pn(String msg, int level)
    {
        debugConsole.pn(msg, level);
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        debugConsole.p(msg, level);
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void logpn(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.pn(msg, level, true);
        }
    }

    public void logpn(String msg)
    {
        logpn(msg, 1);
    }

    public void logp(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.p(msg, level, true);
        }
    }

    public void logp(String msg)
    {
        logp(msg, 1);
    }

    public void logUsage(String msg)
    {
	if (_usageLog != null) {
            _usageLog.logMsg(msg);
	}
    }

    //----------------------------------------------------------------------

    public int getCmdPort()
    {
        return _popCmdPort;
    }

    public String getPopId()
    {
        return _popId;
    }

    //----------------------------------------------------------------------

    public jspop(String[] args) throws YException
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);
        System.out.println("\nmaxclient value is " + maxClient);

	String usageLogFile = "logs/jspop.usage." + _popId;
	try {
	    // Note: we should probably eventually have command-line arguments
	    // to specify the sizes here.  Size 0 means don't rotate
	    // this log.
	    _usageLog = new CircularLog(usageLogFile, 0, 1000);
	    _usageLog.setAppendTime(true);
	} catch (IOException ex) {
	    System.err.println("Unable open file: " + usageLogFile +
	      "; " + ex);
	    throw new YException("Unable to create usage log");
	}

        pn("\nStarting command server socket on " + _popCmdPort + " ...\n");
        try {
            cmdServerSocket = new ServerSocket(_popCmdPort);
        } catch (IOException e) {
            System.err.println("Can not start command server socket at port " + _popCmdPort);
	    System.err.println(e.getMessage());
            quit(1);
        }

        pn("\nStarting JSS handler ...\n");
        try {
            jssHandler = new JssHandler(this, jspopPort);
            jssHandler.start();
        } catch (YException e) {
            System.err.println("Can not start jss handler");
            System.err.println(e.getMessage());
            quit(1);
        }

	_threadChecker = new DEViseThreadChecker();
	_debugThread = new DEViseJspopDebug();
	
        pn("Starting client dispatcher ...\n");
        dispatcher = new DEViseClientDispatcher(this);
        dispatcher.start();
    }

    //----------------------------------------------------------------------

    public void start()
    {
        popThread = new Thread(this);
	popThread.setName("jspop");
        popThread.start();
    }

    // quit() will only be called in jspop server thread or in jspop()
    // initialization and all the stop() or close() methods of other
    // class are synchronized, so there is no need to synchronize quit()
    private synchronized void quit(int exitVal)
    {
        if (_debugLvl >= 2) {
            pn("jspop.quit()");
        }

	//
	// Stop the various threads.
	//

        pn("Stop jspop server thread...");
        if (popThread != null && Thread.currentThread() != popThread) {
            popThread.stop();
            popThread = null;
        }

        pn("Stop client dispatcher thread...");
        if (dispatcher != null) {
            dispatcher.stop();
            dispatcher = null;
        }

        pn("Stop jss handler thread ...");
        if (jssHandler != null) {
            jssHandler.stop();
            jssHandler = null;
        }

	//
	// Close sockets.
	//

        pn("Stop command server socket...");
        if (cmdServerSocket != null) {
            try {
                cmdServerSocket.close();
            } catch (IOException e) {
            }

            cmdServerSocket = null;
        }
	
	//TEMP -- why is this commented out?
        /*
        System.out.println("Stop DEViseServer thread...");
        for (int i = 0; i < servers.size(); i++) {
            DEViseServer server = (DEViseServer)servers.elementAt(i);
            if (server != null) {
                server.stop();
            }
        }
        servers.removeAllElements();
        */

	//
	// Close all clients.
	//

        pn("Close clients...");
        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        suspendClients.removeAllElements();

        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient client = (DEViseClient)activeClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        activeClients.removeAllElements();

	//
	// Close the log file.
	//
        pn("Close log file...");
        if (logFile != null) {
            logFile.close();
            logFile = null;
        }

        System.exit(exitVal);
    }

    //----------------------------------------------------------------------

    // This basically listens for new connections from JavaScreens.
    // When a new connection is made, it creates a new DEViseClient object
    // corresponding to the JavaScreen that just connected.
    public void run()
    {
        pn("\nJSPOP Server started ...\n");

        int quitID = 0;

        Socket socket1 = null;

        while (true) {
	    if (_debugLvl >= 1) pn("Starting jspop.run loop");
            try {
		// Note: probably *everything* here after the accept() call
		// should be in a separate thread for each connection.
		// wenger 2003-10-28.
                socket1 = cmdServerSocket.accept();
	        if (_debugLvl >= 1) pn("Socket connection; client " +
		  "host not yet known");
            } catch (IOException e) {
                pn("JSPOP server can not listen on command socket so " +
		  "it is aborting!");
                quitID = 1;
                break;
            }

	    if (_debugLvl >= 1) pn("  Before handleConnection()");
	    handleConnection(socket1);
	  
	    // client count?
	    if (getClientCount() >= maxClient) {
	        if (_debugLvl >= 1) pn("  Before killOldestClient()");
	        killOldestClient();
	    }

	    //
	    // Kill off any clients that we haven't gotten heartbeats from
	    // for a while.
	    //
	    if (_debugLvl >= 1) pn("  Before checkHeartbeats()");
            checkHeartbeats();
	    if (_debugLvl >= 1) pn("  After checkHeartbeats()");

        } // while

        quit(quitID);
    }

    // Send a stateless command (one that has nothing to do with client
    // state) to a randomly-selected server.
    public void sendStatelessCmd(String cmd) throws YException
    {
        if (_debugLvl >= 2) {
	    pn("jspop.sendStatelessCmd(" + cmd + ")");
	}

	// Randomly pick a server.
	int serverCount = servers.size();
	int serverNum = (int)((new Random()).nextFloat() * serverCount);
	serverNum %= serverCount; // make *sure* it's legal

	// Send the command.
	DEViseServer server = (DEViseServer)servers.elementAt(serverNum);
	server.sendStatelessCmd(cmd);
    }

    private synchronized void handleConnection(Socket socket1)
    {
        if (_debugLvl >= 2) {
            pn("jspop.handleConnection() in thread " +
              Thread.currentThread());
        }

        try {
            //
            // If we get to here, we've connected on both the
            // command socket and data socket, so create a new
            // DEViseClient corresponding to the connection.
            //
	    DEViseClientSocket clientSock = new DEViseClientSocket(socket1,
	      SOCK_REC_TIMEOUT, this);

	    activeSockets.addElement(clientSock);
	    if (_debugLvl >= 1) {
	        pn(activeSockets.size() + " active client sockets in JSPoP");
	    }
        } catch (YException e) {
	    pn("\nEstablishing client connection failed: " + e.getMsg());
        }

        if (_debugLvl >= 2) {
            pn("DIAG done with jspop.handleConnection() " +
	      "in thread " + Thread.currentThread());
        }
    }

    // Check the heartbeat times of all clients; if we haven't gotten a
    // heartbeat for _killInterval milliseconds, kill that client object (on
    // the assumption that the actual applet has died, or we've lost
    // communication with it).
    private synchronized void checkHeartbeats()
    {
	if (_debugLvl >= 2) {
	    pn("jspop.checkHeartbeats()");
	}

	long presentTime = (new Date()).getTime();

	boolean killedAClient = false;

	//
	// Create a list of all clients so we can check both active and
	// suspended clients without having duplicate code.
	//
	Vector allClients = new Vector(activeClients.size() +
	  suspendClients.size());

        for (int index = 0; index < activeClients.size(); index++) {
	    allClients.addElement(activeClients.elementAt(index));
	}

        if (_debugLvl >= 4) {
	    pn("DIAG before adding to suspended clients list");
	}
        for (int index = 0; index < suspendClients.size(); index++) {
	    allClients.addElement(suspendClients.elementAt(index));
	}
        if (_debugLvl >= 4) {
	    pn("DIAG after adding to suspended clients list");
	}

	//
	// Okay, now go through the list and kill off any clients that
	// haven't sent a heartbeat for a long time
	// (_killInterval milliseconds).
	//
	for (int index = 0; index < allClients.size(); index++) {
	    DEViseClient client =
	      (DEViseClient)allClients.elementAt(index);
	    long sinceHeartbeat = presentTime-client.getHeartbeat();
	    if (_killInterval > 0 && sinceHeartbeat > _killInterval) {
		pn("Killing client " + client.getConnectionID() +
		  " because heartbeat interval is too long");
		killedAClient = true;
		killClient(client);
	    }
	}

	//
	// Note: killed clients will actually be removed from the client
	// list in getNextRequestingClient().  We don't *really* need
	// to call this here, because it will be called later, but it's
	// kind of weird to kill off a client and then still have it in
	// the clients list.  RKW 2002-04-11.
	//
        if (killedAClient) {
	    getNextRequestingClient();
	}
    }

    //----------------------------------------------------------------------
    // Close the client that has been inactive for the longest period of
    // time.
    private synchronized void killOldestClient()
    {
        if (_debugLvl >= 1) {
	    pn("jspop.killOldestClient()");
	}

	//
	// Note: we are checking both the active and suspended clients lists
	// here in case of the special case where maxclients is equal to
	// the number of servers.  If that's the case, and a new client
	// connects, the new client is the only client in the suspended
	// list, so if we only check that list, the new client will be
	// closed immediately.  RKW 2000-11-08.
	//
	DEViseClient oldestClient = null;
	long oldestHeartbeat = Long.MAX_VALUE;

	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getHeartbeat() < oldestHeartbeat) {
		    oldestClient = tmpClient;
		    oldestHeartbeat = oldestClient.getHeartbeat();
		}
	    }
        }

        if (_debugLvl >= 4) pn("DIAG jspop 4210");
	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getHeartbeat() < oldestHeartbeat) {
		    oldestClient = tmpClient;
		    oldestHeartbeat = oldestClient.getHeartbeat();
		}
	    }
        }
        if (_debugLvl >= 4) pn("DIAG jspop 4290");

	if (oldestClient != null) {
	    pn("Killing client " +
	      oldestClient.getConnectionID() +
	      " because maxclients limit has been reached");
	    killClient(oldestClient);

	    //
	    // Note: killed clients will actually be removed from the client
	    // list in getNextRequestingClient().  We don't *really* need
	    // to call this here, because it will be called later, but it's
	    // kind of weird to kill off a client and then still have it in
	    // the clients list.  RKW 2002-04-11.
	    //
	    getNextRequestingClient();
	}
    }

    //----------------------------------------------------------------------

    // Get the DEViseUser object corresponding to the given key and password.
    public DEViseUser getUser(String username, String password)
    {
        if (username == null || password == null) {
            return null;
        }

        if (users.containsKey(username)) {
            DEViseUser user = (DEViseUser)users.get(username);
            if (user != null) {
                if (password.equals(user.getPassword())) {
                    return user;
                }
            }

            return null;
        }

        return null;
    }

    private synchronized void addClient(DEViseClient client)
    {
        if (client != null) {
            pn("Client from " + client.getHostname() + " is added ...");
            client.setSuspend();
            if (_debugLvl >= 4) {
                pn("DIAG before adding to suspendClients");
	    }
            suspendClients.addElement(client);
            if (_debugLvl >= 4) {
                pn("DIAG after adding to suspendClients");
	    }
        }
    }

    public synchronized int getClientCount()
    {
        return suspendClients.size() + activeClients.size();
    }

    private synchronized DEViseClient findClientById(long id)
    {
	if (_debugLvl >= 2) {
	    pn("findClientById(" + id + ") in thread " +
	      Thread.currentThread());
	}

	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getConnectionID() == id) {
		    return tmpClient;
		}
	    }
	}

	if (_debugLvl >= 4) {
	    pn("Before checking suspended clients list");
        }
	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getConnectionID() == id) {
		    return tmpClient;
		}
	    }
	}
	if (_debugLvl >= 4) {
	    pn("After checking suspended clients list");
        }

        return null;
    }

    private synchronized DEViseClient findClientByCollabName(String name)
    {
	if (_debugLvl >= 2) {
	    pn("findClientByCollabName(" + name + ") in thread " +
	      Thread.currentThread());
	}

	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null) {
		String collabName = tmpClient.getCollabName();
		pn("get collabname = " + collabName);
		if (collabName != null)
		    if (collabName.equals(name)) {
			return tmpClient;
		}
	    }
	}

	if (_debugLvl >= 4) {
	    pn("Before checking suspended clients list");
        }
	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null) {
		String collabName = tmpClient.getCollabName();
		pn("get collabname = " + collabName);
		if (collabName != null)
		    if (collabName.equals(name)) {
			return tmpClient;
		}
	    }
	}
	if (_debugLvl >= 4) {
	    pn("After checking suspended clients list");
        }

        return null;
    }

    //----------------------------------------------------------------------

    // Get the client, if any, that has been waiting longest for a command
    // to be serviced (length of time multiplied by client priority).
    // This also removes any closed clients.
    //
    // this method will only be called in client dispatcher thread
    // this method will also check which client need to be removed and remove it

    // TEMP -- we should probably re-think this method because of the
    // new DEViseClientSocket class.  RKW 2001-10-24.
    public synchronized DEViseClient getNextRequestingClient()
    {
        if (_debugLvl >= 2) {
	    pn("jspop.getNextRequestingClient() in thread " +
	      Thread.currentThread());
	}

	//
	// Check all sockets for input; if there is input, read the command
	// and add it to the appropriate client.  (This changes the client's
	// state to REQUEST unless it's already connected to a server.)
	//
	try {
	    if (_debugLvl >= 4) {
	        pn("DIAG before checking sockets");
	    }
	    for (int i = 0; i < activeSockets.size(); i++) {
	        if (_debugLvl >= 4) pn("DIAG jspop 1010");
		DEViseClientSocket clientSock =
		  (DEViseClientSocket)activeSockets.elementAt(i);
	        if (_debugLvl >= 4) pn("DIAG jspop 1020");
	    
		// Note: socket will never be null here; socket.is is null
		// if the socket has been closed. RKW 2001-10-22.
		if (clientSock.isOpen()) {
	            if (_debugLvl >= 4) pn("DIAG jspop 1030");
		    if (clientSock.hasCommand()) {
	                if (_debugLvl >= 4) pn("DIAG jspop 1040");
			String cmd = clientSock.getCommand();

			int id = clientSock.getCmdId();
			int cgi = clientSock.getCmdCgiFlag();
			clientSock.clearCommand();

		        processCommand(clientSock, cmd, id, cgi);
		    }
	            if (_debugLvl >= 4) pn("DIAG jspop 1045");
		} else { // !socket.isOpen
		    if (_debugLvl >= 2) {
		        pn("Removing client socket " +
			  clientSock.getObjectNum() +
			  " from active sockets list");
		    }
		    activeSockets.removeElement(clientSock);
		}
	    } // for
	    if (_debugLvl >= 4) {
	        pn("DIAG after checking sockets");
	    }
        } catch (YException e) {
            pn("Exception checking sockets " + e);
        }    

	//
	// Find the suspended client that's been waiting the longest time
	// (weighted by client priority).
	//
        float time = -1.0F, clientTime = 0.0F;
        DEViseClient client = null;
        Vector removedClient = new Vector();

	if (_debugLvl >= 4) pn("DIAG jspop 1110");
        for (int i = 0; i < suspendClients.size(); i++) {
	    if (_debugLvl >= 4) pn("DIAG jspop 1120");
            DEViseClient newclient = (DEViseClient)suspendClients.elementAt(i);
	    if (_debugLvl >= 4) pn("DIAG jspop 1121");
            if (newclient != null) {
	        if (_debugLvl >= 4) pn("DIAG jspop 1130");
                int status = newclient.getStatus();
	        if (_debugLvl >= 4) pn("DIAG jspop 1131");
                if (status == DEViseClient.CLOSE) {
		    // this client need to be removed
	            if (_debugLvl >= 4) pn("DIAG jspop 1140");
                    removedClient.addElement(newclient);
	            if (_debugLvl >= 4) pn("DIAG jspop 1141");
                    continue;
		} else if (status == DEViseClient.REQUEST) {
		    //} else if (status == DEViseClient.REQUEST || 
		    //   ! newclient.cmdBuffer.isEmpty()) {
		    // only clients that are requesting service will be served
	            if (_debugLvl >= 4) pn("DIAG jspop 1150");
                    clientTime = (float)(newclient.getPriority() *
					 newclient.getSuspendTime());
	            if (_debugLvl >= 4) pn("DIAG jspop 1151");
		    
		    // Keep track of the client that's been waiting the
		    // longest (accounting for priority).
                    if (time < 0.0) {
                        time = clientTime;
                        client = newclient;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            client = newclient;
                        }
                    }
	            if (_debugLvl >= 4) pn("DIAG jspop 1160");
                }
            }
        }
	if (_debugLvl >= 4) pn("DIAG jspop 1190");

	//
	// Remove any closed clients.
	//
	if (removedClient.size() > 0) {
            for (int i = 0; i < removedClient.size(); i++) {
                DEViseClient newclient =
		  (DEViseClient)removedClient.elementAt(i);
                if (newclient != null) {
                    if (_debugLvl >= 1) {
		        pn("Removing client " + newclient.getConnectionID());
	            }
                    suspendClients.removeElement(newclient);
	            if (_debugLvl >= 4) pn("DIAG jspop 1250");
                }
            }
            removedClient.removeAllElements();

            // Objects don't seem to get reliably garbage collected without
            // this.  RKW 2000-11-07.
            System.gc();

            if (_debugLvl >= 1) {
                pn("jspop state in getNextRequestingClient: " +
		  getServerState(true));
	    }
	}

	if (_debugLvl >= 4) {
	    pn("DIAG at end of getNextRequestingClient(); " +
	      "client is " + ((client != null) ?
	      (new Long(client.getConnectionID()).toString()) : "null"));
	}

        return client;
    }

    public synchronized void activateClient(DEViseClient c)
    {
        if (_debugLvl >= 2) {
	    pn("jspop.activateClient(" +
	      c.getConnectionID() + ")");
	}

        if (c != null) {
	    if (_debugLvl >= 4) pn("DIAG jspop 2110");
            if (!suspendClients.removeElement(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is not in suspended clients list");
	    }
	    if (_debugLvl >= 4) pn("DIAG jspop 2120");

	    if (activeClients.contains(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is already in active clients list");
	    } else {
                activeClients.addElement(c);
	    }
            c.setActive();
        }
    }

    public synchronized void suspendClient(DEViseClient c)
    {
        if (_debugLvl >= 2) {
	    pn("jspop.suspendClient(" +
	      c.getConnectionID() + ")");
	}

        if (c != null) {
            if (!activeClients.removeElement(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is not in active clients list");
	    }

	    if (_debugLvl >= 4) pn("DIAG jspop 3110");
	    if (suspendClients.contains(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is already in suspended clients list");
	    } else {
                suspendClients.addElement(c);
	    }
	    if (_debugLvl >= 4) pn("DIAG jspop 3120");
            c.setSuspend();
        }
    }

    //----------------------------------------------------------------------

    public synchronized void addServer(String name, int port, int cmdport,
      int imgport)
    {
        DEViseServer newserver = new DEViseServer(this, name, port,
	  cmdport, imgport);
        servers.addElement(newserver);

        try {
            newserver.start();
        } catch (YException e) {
	    pn("Removing server because startup failed");
            removeServer(newserver);
            pn(e.getMsg());
        }
    }

    public synchronized void removeServer(DEViseServer server)
    {
        if (server == null) {
            return;
        }

        servers.removeElement(server);

        try {
            Socket socket = new Socket(server.hostname, server.jssport);
            DataOutputStream os = new DataOutputStream(
	      new BufferedOutputStream(socket.getOutputStream()));
            String msg = DEViseCommands.S_RESTART + " " + server.cmdPort;
            pn("Trying to send restart request to " +
	      server.hostname + " ...");
            os.writeInt(msg.length());
            os.writeBytes(msg);
            os.flush();
            os.close();
            socket.close();
            System.out.println("Message \"" + msg +
	      "\" successfully sent to JSS server at " + server.hostname);
        } catch (UnknownHostException e) {
            pn("Can not find jss host " + server.hostname);
        } catch (NoRouteToHostException e) {
            pn("Can not find route to jss host " + server.hostname +
	      ", may caused by an internal firewall");
        } catch (IOException e) {
            pn("IO Error while connecting to jss host " + server.hostname);
        }
    }

    // Get the server that has been inactive for the longest time (accounting
    // for client priority).
    public synchronized DEViseServer getNextAvailableServer()
    {
        if (_debugLvl >= 2) {
	    pn("jspop.getNextAvailableServer() in thread " +
	      Thread.currentThread());
	}

        DEViseServer server = null;
        DEViseClient client = null;
        float time = -1.0F, clientTime = 0.0F;

        for (int i = 0; i < servers.size(); i++) {
            DEViseServer newserver = (DEViseServer)servers.elementAt(i);
            if (newserver != null) {
                if (newserver.getStatus() == DEViseServer.STATUS_STOPPED) {
                    try {
                        newserver.start();
                    } catch (YException e) {
	                pn("Removing server because restart failed");
                        removeServer(newserver);
                        pn(e.getMsg());
                        continue;
                    }
                } else {
                    if (!newserver.isAvailable()) {
                        continue;
                    }

                    client = (DEViseClient)newserver.getCurrentClient();
                    if (client == null) { // this server is idle
                        server =  newserver;
			break;
                    }

		    // Keep track of the server that's been inactive the
		    // longest (accounting for client priority).
                    clientTime = (float)client.getActiveTime() /
		      (float)client.getPriority();
                    if (time < 0.0) {
                        time = clientTime;
                        server = newserver;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            server = newserver;
                        }
                    }
                }
            }
        }

	if (_debugLvl >= 4) {
	    String name = ((server != null) ? server.hostname : "null");
	    pn("DIAG server is " + name);
	}

        return server;
    }

    //----------------------------------------------------------------------
    public String getServerState()
    {
        return getServerState(false);
    }

    // State string contains the following info (if not verbose):
    //   - number of servers
    //   - hostname and client ID of each server
    //   - total number of client objects
    //   - number of active clients
    //   - ID and hostname of each active client
    //   - number of suspended clients
    //   - ID and hostname of each suspended client, and whether the client's
    //     command buffer is empty
    public synchronized String getServerState(boolean verbose)
    {
        String state = "{";

        state += servers.size() + " ";
	if (verbose) state += " servers: ";
        for (int i = 0; i < servers.size(); i++) {
	    state += "{";
            DEViseServer server = (DEViseServer)servers.elementAt(i);
            if (server == null) {
                state = state + "null ";
            } else {
                state = state + server.hostname + " ";
		DEViseClient client = server.getCurrentClient();
		if (client == null) {
		    state += "null";
		} else {
		    state += client.getConnectionID();
		}
            }
	    state += "} ";
        }

	int totalClients = activeClients.size() + suspendClients.size();
	state += totalClients + " ";
	if (verbose) state += "total clients ";

        state = state + activeClients.size() + " ";
	if (verbose) state += "active clients: ";
        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient client = (DEViseClient)activeClients.elementAt(i);
	    state += "{" + client.getConnectionID() + " ";
            if (client == null) {
                state = state + "null";
            } else {
                state = state + client.getHostname();
            }
	    state += "} ";
        }

        if (_debugLvl >= 4) pn("DIAG jspop 4110");
        state = state + suspendClients.size() + " ";
	if (verbose) state += "suspended clients: ";
        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
	    state += "{" + client.getConnectionID() + " ";
            if (client == null) {
                state = state + "null";
            } else {
                state = state + client.getHostname();
		state = state + " " + client.getStatus() + " " + client.cmdBuffer.isEmpty();
 
            }
	    state += "} ";
        }
        if (_debugLvl >= 4) pn("DIAG jspop 4190");

	if (verbose) {
	    state += DEViseClient.getObjectCount() + " client objects ";
	}

        state = state + "}";
        return state;
    }

    //----------------------------------------------------------------------
    public int getServerCount()
    {
        return servers.size();
    }

    //----------------------------------------------------------------------
    private DEViseClient createClient(String clientHost,
      DEViseClientSocket clientSock, boolean cgi)
    {
	DEViseClient client = new DEViseClient(this, clientHost, clientSock,
	  getID(), cgi);

	// Note: a new client gets added into the suspended clients list;
	// eventually it gets moved from there to the active clients list.
	addClient(client);

        pn("jspop state in createClient: " + getServerState(true));

	return client;
    }

    //----------------------------------------------------------------------

    // Get a unique ID for a client.
    private synchronized int getID()
    {
	// we use timestamp as client's id now.
	// xuk 10/10/01
	Date date = new Date();
	long time = date.getTime();

	// Make this fit into an int.
	time /= 100; // truncate to tenths of seconds
	time %= Integer.MAX_VALUE;

	int id = (int)time;

	//
	// Make sure there's no chance of duplicate IDs.
	//
	if (id <= _lastClientId) {
	  id = _lastClientId + 1;
	}

	_lastClientId = id;
	
	if (_debugLvl >= 1) {
	    pn("  New client ID: " + id);
	}

	return id;
    }

    //----------------------------------------------------------------------

    // Check command-line arguments and set variables accordingly.
    private void checkArguments(String[] args) throws YException
    {
        for (int i = 0; i < args.length; i++) {
	    StringBuffer argValue = new StringBuffer();

	    if (DEViseGlobals.checkArgument(args[i], "-cmdport", true,
	      argValue)) {
                try {
                    int port = Integer.parseInt(argValue.toString());
                    if (port < 1024 || port > 65535) {
                        throw new NumberFormatException();
                    }
                    _popCmdPort = port;
                } catch (NumberFormatException e) {
                    throw new YException("Please use a positive integer " +
		      "between 1024 and 65535 as the cmdport number");
                }

	    } else if (DEViseGlobals.checkArgument(args[i], "-jspopport", true,
	      argValue)) {
                try {
                    jspopPort = Integer.parseInt(argValue.toString());
                    if (jspopPort < 1024 || jspopPort > 65535) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Please use a positive integer " +
		      "between 1024 and 65535 as the jspop port number");
                }

	    } else if (DEViseGlobals.checkArgument(args[i], "-logfile", true,
	      argValue)) {
		// Note: nothing ever seems to get logged to this file.
		// RKW 2000-11-07.
                logFileName = argValue.toString();

	    } else if (DEViseGlobals.checkArgument(args[i], "-userfile", true,
	      argValue)) {
                userFileName = argValue.toString();

	    } else if (DEViseGlobals.checkArgument(args[i], "-id", true,
	      argValue)) {
		_popId = argValue.toString();

	    } else if (DEViseGlobals.checkArgument(args[i], "-maxclient",
	      true, argValue)) {
                try {
		    maxClient = Integer.parseInt(argValue.toString());
		    if (maxClient < 1) {
		        System.err.println("Warning: illegal maxclient value ("
			  + maxClient + "); using default");
		        maxClient = DEViseGlobals.DEFAULTMAXCLIENT;
		    }
                } catch (NumberFormatException e) {
	            throw new YException("Please specify a numerical value " +
		      "for the -maxclient argument");
                }

	    } else if (DEViseGlobals.checkArgument(args[i], "-clientlog",
	      true, argValue)) {
	        // enable client log files?
		String s = argValue.toString();
		if (s.equals("on")) {
		    clientLog = true;
		} else if (s.equals("off")) {
		    clientLog = false;
	        } else {
		    throw new YException("Please specify 'on' or 'off' for " +
		      "the -clientlog argument");
		}

	    } else if (DEViseGlobals.checkArgument(args[i], "-killint", true,
	      argValue)) {
                try {
		    float hours = Float.valueOf(argValue.toString()).
		      floatValue();
		    if (hours < 0.0) {
		        // Infinite time.
		        _killInterval = 0;
		    } else {
		        // Hours to milliseconds conversion.
		        _killInterval = (int)(hours * 60 * 60 * 1000);
		    }
                } catch (NumberFormatException e) {
	            throw new YException("Please specify a numerical " +
		      "value for the -killint argument");
                }

	    } else if (DEViseGlobals.checkArgument(args[i], "-usage", false,
	      argValue)) {
                System.out.println(usage);
                System.exit(0);

            } else {
                throw new YException("Invalid jspop option \"" + args[i] +
		  "\"is given\n" + usage);
            }
        }

        logLevel = 1;

        if (_popCmdPort < 1024) {
            _popCmdPort = DEViseGlobals.DEFAULTCMDPORT;
        }
        if (logLevel > 0) {
            logFile = new YLogFile(logFileName, logLevel, true);
        }

        debugConsole = new YLogConsole(100);

        System.out.println("Loading user configuration file ...");
        readCFGFile(userFileName);
    }

    // Read user configuration file.
    private void readCFGFile(String filename)
    {
        RandomAccessFile uf = null;
        try {
            uf = new RandomAccessFile(filename, "r");
        } catch (IOException e) {
            System.out.println("Invalid user configuration file \"" +
	      filename + "\"");
            System.exit(1);
        }

        try {
            String str = uf.readLine();
            while (str != null) {
                str = str.trim();
                // skip comment line
                if (!str.startsWith("#") && !str.equals("")) {
                    String[] line = DEViseGlobals.parseStr(str, ":");
                    DEViseUser user = new DEViseUser(line);
                    users.put(user.getName(), user);
                }

                str = uf.readLine();
            }
        } catch (IOException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println("Can not read from user configuration file \""
	      + filename + "\"");
            System.exit(1);
        } catch (YException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println(e.getMsg());
            System.exit(1);
        }

        try {
            uf.close();
        } catch (IOException e) {
        }
    }

    private void processCommand(DEViseClientSocket clientSock,
      String cmd, long id, int flag) throws YException
    {
        if (_debugLvl >= 1) {
	    pn("jspop.processCommand(" + cmd + ")");
	}

	boolean cgi;
	if (flag == 1) {
	    cgi = true;
	    pn("Receiving command CGI.");
	} else {
	    cgi  = false;
	    pn("Receiving command via socket.");
	}
	
	pn("Received command from client(" + id + ") :  \"" + cmd + "\"");		
	if (id == DEViseGlobals.DEFAULTID) { // new JS
	    pn("New client");
	    DEViseClient client = createClient(clientSock.getHostname(),
	      clientSock, cgi);
	    client.addNewCmd(cmd);
	} else { // old JS
	    pn("Existing client");
	    // Vector methods -- if so, what's the other thread holding
	    // the lock?  Hmm -- it pretty much looks like the client
	    // dispatcher thread must get stuck here or in
	    // DEViseClient.sendCmd() -- with a heartbeat there just
	    // isn't much place for it to get stuck.
	    DEViseClient client = findClientById(id);
	    if (client != null) {
		// set cgi flag; added for mode changing
		client.setCgi(cgi); 
		client.setSocket(clientSock);
		client.addNewCmd(cmd);

	        if ((cmd.startsWith(DEViseCommands.SAVE_CUR_SESSION)) &&
	          (client.isAbleCollab)) {
	            // disable collaboration
		    // Why? -- RKW 2001-11-09.
		    client.isAbleCollab = false;
		    pn("Disable collaboration." + client.isAbleCollab);
	        }
	    } else {
		clientSock.sendCommand(DEViseCommands.ERROR +
				       " {Client connection is invalid.  Please exit " +
				       "the JavaScreen.}");
		clientSock.closeSocket();
		throw new YException("No client for ID: " + id);
	    }
	}
    }

    // Get a list of possible collaboration leaders 
    public synchronized String getCollabLeaders()
    {
        if (_debugLvl >= 4) pn("DIAG jspop 5110");
	String command = new String(DEViseCommands.CLIENTS);

	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null && tmpClient.isAbleCollab) {
		command = command + " {" + tmpClient.getConnectionID() +
		    "} {  " + tmpClient.getCollabName() + "} {  " + 
		    tmpClient.hostname + ":}" + " { " +
		    tmpClient.sessionName + "}";
	    }
	}

        if (_debugLvl >= 4) pn("DIAG jspop 5120");
	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null && tmpClient.isAbleCollab) {
		command = command + " {" + tmpClient.getConnectionID() +
		    "} {  " + tmpClient.getCollabName() + "} {  " + 
		    tmpClient.hostname + ":}" +
		    " { " + tmpClient.sessionName + "}";
	    }
	}
        if (_debugLvl >= 4) pn("DIAG jspop 5130");

	command = command.trim();

	return (command);
    }

    // Set up the client as a collaboration follower.
    public void setUpCollab(DEViseClient client, String cmd, String hostname) 
    {
        if (_debugLvl >= 1) {
            pn("jspop.setUpCollab(" + client + ", " + cmd +
	      ", " + hostname + ")");
        }

	DEViseClient leaderClient = null;
	long id = 0;

	String[] cmds = DEViseGlobals.parseString(cmd);
	int flag = new Integer(cmds[1]).intValue();
	
	if (flag == 2) { // use collab-name to identify leader
	    String name = cmds[3];
	    leaderClient = findClientByCollabName(name);
	    if (leaderClient != null) {
	        id = leaderClient.getConnectionID();
	    }
	    pn("We got collabortion name: " + name);
	    pn("We got collabortion id: " + id);
	} else {         // use id to identify leader
	    id = new Long(cmds[2]).longValue();
	    pn("We got collaboration id: " + id);
	    // find the proper client;
	    leaderClient = findClientById(id);
	}

	try {
	    if (leaderClient != null) {
		// check for enable collaboration status
		if (leaderClient.isAbleCollab) {
		    
		    String collabPass = new String(cmds[4]);
		    pn("We got collaboration passwd: " + collabPass);
		    
		    if (leaderClient.checkPass(collabPass)) {
			// TEMP -- if you have > 1 server, might this cause
			// an unnecessary client switch, if you happen to get
			// the server that's connected to the other client??
			// RKW 2001-10-23.
			leaderClient.addCollabClient(client, hostname);
			client.collabLeader = leaderClient;

			//TEMP -- ideally, the JSPoP would initiate the
			// saving of the pre-collaboration session in here
			// somewhere; however, for now this is still done
			// by the client. RKW 2001-11-13.

			client.sendCmd(DEViseCommands.INIT_COLLAB + " " + id);

			client.sendCmd(DEViseCommands.SET_DISPLAY_SIZE + " " + 
				       leaderClient.screenDimX + " " + 
				       leaderClient.screenDimY+ " " + 
				       leaderClient.screenResX+ " " + 
				       leaderClient.screenResY);
			client.sendCmd(DEViseCommands.DONE);
			
			// Make sure the leader client has a server so it
			// can save and reload the current session to generate
			// the commands and data to initialize the collaborator.
			DEViseServer server = getNextAvailableServer();
			if (server != null && 
			    (server.getCurrentClient()) != leaderClient) {
			    server.setCurrentClient(leaderClient);
			}
		    } else { // wrong passwd
			pn("Incorrect password.");
			client.sendCmd(DEViseCommands.ERROR +
				       " {Incorrect password. Please try again.}");
		    }
		} else { // disable collaboration
		    pn("Disabled to collaborate with client.");
		    client.sendCmd(DEViseCommands.ERROR +
				   " {Client " + id + " does not allow collaboration.}");
		}
	    } else {
		pn("No client is found.");
		client.sendCmd(DEViseCommands.ERROR + " {Incorrect client id or collaboration name. Please try again.}");
	    }
        } catch (YException e) {
            pn(e.getMsg());
        }
    }

    // Cleanly kill the specified client, whether it's active or suspended.
    private synchronized void killClient(DEViseClient client)
    {
	if (_debugLvl >= 2) {
	    pn("jspop.killClient(" +
	      client.getConnectionID() + ")");
	}

        if (activeClients.contains(client)) {
	    if (_debugLvl >= 4) {
	        pn("  Killing active client");
	    }

	    DEViseServer server = findServerOfClient(client);
	    if (server != null) {
	        try {
	            server.cmdExit();
	        } catch (YException e) {
	            System.err.println("Error closing client " +
		      client.getConnectionID() + ": " + e.getMessage());
	        }
	    } else {
	        System.err.println(
		  "Error: no server found for client in active clients list!");
	    }
	} else {
	    if (_debugLvl >= 4) {
	        pn("  Killing suspended client");
	    }

	    client.close();
	}
    }

    // Find the server (if any) of the given client.
    DEViseServer findServerOfClient(DEViseClient client)
    {
        DEViseServer result = null;

        for (int index = 0; index < servers.size(); index++) {
	    DEViseServer server = (DEViseServer)servers.elementAt(index);
	    if (server.getCurrentClient() == client) {
		result = server;
	        break;
	    }
	}

	return result;
    }
}
