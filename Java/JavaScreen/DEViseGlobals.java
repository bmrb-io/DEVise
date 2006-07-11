// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains a number of constants and utility functions.
// Note: all members are static.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.76  2006/06/29 21:01:20  wenger
// Merged devise_jmol_br_2 thru devise_jmol_br_3 to the trunk.
//
// Revision 1.75  2006/06/23 19:52:41  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.74  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.73  2006/05/25 20:14:48  wenger
// Merged V1_8b0_br_0 thru V1_8b0_br_1 to the trunk.
//
// Revision 1.72.2.18  2006/06/29 18:10:13  wenger
// Fixed the bug that moving the data selection cursor reset
// what was *displayed* in Jmol, not just what was highlighted.
//
// Revision 1.72.2.17  2006/06/27 20:49:29  wenger
// Added menu option to highlight by color instead of selection
// halo; slight cleanup of how the Jmol popup menu is generated;
// other minor cleanups.
//
// Revision 1.72.2.16  2006/06/23 17:08:02  wenger
// Implemented setting Jmol menu checkboxes to the right state;
// tried help stuff, but both the HelpDialog and AboutDialog
// generate security violations when running as an applet.
//
// Revision 1.72.2.15  2006/06/15 19:54:30  wenger
// Most (but not all) Jmol menus in the JS now working; still need to
// update the JS's Jmol menus as the state of Jmol changes, etc.;
// fixed a null pointer exception if closing coordinate session after
// destroying the tree dialog.
//
// Revision 1.72.2.14  2006/06/14 18:35:28  wenger
// Changed Jmol menus to use string constants so subsequent comparisons
// are safe (don't have to worry about typos causing mismatches).
//
// Revision 1.72.2.13  2006/06/14 16:32:02  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//
// Revision 1.72.2.12  2006/06/09 19:22:18  wenger
// Went back to using a JmolPanel embedded in the JavaScreen, instead of
// a top-level Jmol object in its own frame (doesn't work in browser because
// of security restrictions).
//
// Revision 1.72.2.11  2006/06/08 19:52:58  wenger
// Initial phase of having Jmol in its own window, mainly so we get the
// menus for free.  Still needs lots of cleanup.
//
// Revision 1.72.2.10  2006/06/06 21:48:21  wenger
// Added Jmol menus to the JavaScreen Jmol menu button (only the View
// menu is functional at this time).
//
// Revision 1.72.2.9  2006/05/26 21:19:23  wenger
// Jmol popup menus now working.
//
// Revision 1.72.2.8  2006/05/22 18:59:56  wenger
// We now handle destruction of the Jmol tree window much better.
//
// Revision 1.72.2.7  2006/05/16 15:02:40  wenger
// Fixed (I think) tree GUI problems on Mozilla (really, pre-1.5 JVMs)
// (the tree GUI window was not showing up).
//
// Revision 1.72.2.6  2006/05/12 20:15:39  wenger
// Both structure trees are now in a single window.
//
// Revision 1.72.2.5  2006/05/02 16:51:33  wenger
// Added "J" to versions so we know it's the Jmol branch.
//
// Revision 1.72.2.4  2006/04/14 15:27:17  wenger
// Atom selection GUI now feeds back into Jmol; default selection
// from DEVise data view is now fed into the atom selection GUI;
// selecting entire molecule works.  We still need a few more
// features.
//
// Revision 1.72.2.3  2006/04/12 14:08:29  wenger
// Changed JS version string.
//
// Revision 1.72.2.2  2006/02/23 22:08:40  wenger
// Added flag for whether or not 3D views should use Jmol.
//
// Revision 1.72.2.1  2005/12/29 21:19:20  wenger
// Improved Jmol integration into the JavaScreen -- the Jmol
// viewer isn't constantly destroyed and constructed; also a
// bunch of other improvements in the Jmol-related DEViseCanvas
// code.
//
// Revision 1.72  2005/12/06 22:53:00  wenger
// Updated DEVise and JavaScreen versions for the trunk (now 1.9).
//
// Revision 1.71.2.1  2005/12/06 22:49:33  wenger
// Updated DEVise and JavaScreen versions for the 1.8 branch.
//
// Revision 1.71  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.70  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.69  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.68  2002/06/17 19:40:14  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.67  2002/05/01 21:28:58  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.66.2.24  2005/11/01 18:53:52  wenger
// Changed DEVise version to 1.7.20x1, JavaScreen version to 5.5.3x1.
//
// Revision 1.66.2.23  2004/09/29 19:57:25  wenger
// Changed version to 5.5.2 (from 5.5.2x4 on the jspop_debug_0405_br branch).
//
// Revision 1.66.2.22  2004/09/29 19:08:35  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.66.2.21  2004/05/12 21:43:57  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.66.2.20.4.5  2004/09/29 18:16:48  wenger
// (Hopefully) final cleanup of the jspop_debug_0405_br branch -- some
// changes to DEViseClientSocket, and a little more debug output in
// jspop.
//
// Revision 1.66.2.20.4.4  2004/09/03 19:00:52  wenger
// More diagnostic output and debug comments; version is now 5.2.2x3.
//
// Revision 1.66.2.20.4.3  2004/07/01 17:56:19  wenger
// Updated version.
//
// Revision 1.66.2.20.4.2  2004/06/29 15:16:20  wenger
// Fixed some problems with the JSPoP debug code.
//
// Revision 1.66.2.20.4.1  2004/05/10 22:28:51  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.66.2.20  2003/12/22 22:47:14  wenger
// JavaScreen support for print color modes is now in place.
//
// Revision 1.66.2.19  2003/12/03 19:31:07  wenger
// Changed most buttons in the JavaScreen GUI to menus (to save space
// in preparation for adding new functionality).
//
// Revision 1.66.2.18  2003/10/28 20:41:37  wenger
// Added debug code (including that used to find bug 889).
//
// Revision 1.66.2.17  2003/10/24 23:46:08  wenger
// Non-applet JS now continues without dialog if it can't get the animation
// symbols.
//
// Revision 1.66.2.16  2003/10/15 21:55:10  wenger
// Added new JAVAC_StopCollab command to fix ambiguity with
// JAVAC_CollabExit; minor improvements to collaboration-related stuff
// in the auto test scripts.
//
// Revision 1.66.2.15  2003/09/23 21:55:11  wenger
// "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.
//
// Revision 1.66.2.14  2003/06/17 21:04:56  wenger
// Major improvements to command-line argument processing of all JavaScreen
// programs; we now save the -id value in the JSPoP to use for the usage
// log file; some minor cleanups of the auto test scripts; slight
// clarification of command documentation.
//
// Revision 1.66.2.13  2003/05/02 17:16:16  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.66.2.12  2002/12/17 18:08:22  wenger
// Fixed bug 844 (slow rubberband line drawing in child views).
// Note: this fix involves some pretty signficant changes to how the
// canvas is painted and rubberband lines are drawn, so it needs some
// testing.
//
// Revision 1.66.2.11  2002/12/05 20:38:20  wenger
// Removed a bunch of unused (mostly already-commented-out) code to
// make things easier to deal with.
//
// Revision 1.66.2.10  2002/11/25 21:29:34  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.66.2.9  2002/11/05 20:02:28  wenger
// Fixed bug 831 (JSPoP can't respond if stuck sending data); incremented
// DEVise and JavaScreen versions.
//
// Revision 1.66.2.8  2002/07/17 15:08:24  wenger
// Incremented JS version to 5.1.
//
// Revision 1.66.2.7  2002/05/20 21:21:33  wenger
// Fixed bug 779 (client switching problem with multiple DEViseds).
//
// Revision 1.66.2.6  2002/05/08 21:16:50  wenger
// Changed some collaboration GUI for more clarity, updated instructions
// accordingly; default collaboration password is now "" as workaround
// for problem filling in default in GUI on some platforms.
//
// Revision 1.66.2.5  2002/04/18 17:25:10  wenger
// Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
// temporary session files when the JSPoP and DEViseds are on different
// machines).  Note: JS protocol version is now 11.0.
//
// Revision 1.66.2.4.2.1  2002/04/17 17:45:54  wenger
// DEVised, not JSPoP, now does the actual work of creating or clearing
// the temporary session directory (new command from client to DEVised
// means that communication protocol version is now 11.0).  (Client
// switching is not working yet with this code because I need to change
// how temporary sessions are saved and loaded.)
//
// Revision 1.66.2.4  2002/04/15 14:44:27  wenger
// Changed kill interval to 24 hours (how long with no heartbeat before we
// kill a client).
//
// Revision 1.66.2.3  2002/04/12 16:08:49  wenger
// Lots of cleanup to the heartbeat checking code -- tested killing a
// client because the heartbeat timeout expired, and because we have
// too many clients.
//
// Revision 1.66.2.2  2002/04/04 21:21:16  xuk
// Fixed bug 768: collaboration follower can close dialog automatically.
// PROTOCOL_VERSION = 10.3.
//
// Revision 1.66.2.1  2002/04/03 23:33:51  xuk
// Fixed bug 766.
//
// Revision 1.66  2002/03/07 17:18:31  xuk
// Destroy jsa applet when it's invisible longer than one hour.
//
// Revision 1.65  2002/03/06 18:56:50  wenger
// Changed JavaScreen protocol version from 11.0 to 10.1 (to allow backwards
// compatibility with previous DEVised, because the new JAVAC_UpdateJS
// command is never sent to the DEVised).
//
// Revision 1.64  2002/03/01 20:00:08  xuk
// Added new command DEViseCommands.UpdateJS, now protocol number 11.0
//
// Revision 1.63  2002/01/28 23:51:59  wenger
// Updated the DEVised to support drawing axis labels on the JS side:
// changed JAVAC_ViewDataArea command; leaves blank areas for axis
// labels (temporarily disabled until the JS end is done); protocol
// version is now 10.0 -- JS code accepts but ignores new arguments.
//
// Revision 1.62  2001/12/13 21:35:32  wenger
// Added flexibility to enable/disable mouse location display individually
// for X and Y axes (needed for peptide-cgi session improvements requested
// by John Markley).
//
// Revision 1.61  2001/11/28 21:56:19  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.60  2001/11/19 17:17:02  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.59.2.3  2001/11/19 21:04:02  wenger
// Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
// Squid to be able to force DEVise to re-read all data and update the
// visualization; did some cleanup of JavaScreenCmd.C.
//
// Revision 1.59.2.2  2001/11/14 19:05:17  wenger
// Should have changed protocol version to 9.1 before, when I added the
// JAVAC_InitCollaboration command.
//
// Revision 1.59.2.1  2001/11/13 20:31:35  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.59  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.58.2.2  2001/11/07 17:22:36  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.58.2.1  2001/10/28 18:13:18  wenger
// Made msgType and cmdId private in DEViseCommSocket; other minor cleanups.
//
// Revision 1.58  2001/10/12 02:05:08  xuk
// Using timestamp-based client ID.
// 1. PROTOCOL_VERSION = 8.0;
// 2. DEFAULTID has been expanded to long;
// 3. Added toUlong() method to parse long int from network.
//
// Revision 1.57  2001/09/12 20:34:29  wenger
// Incremented JS version to 4.4; various fixes because of install problems
// on pumori.
//
// Revision 1.56  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.55  2001/05/21 17:15:19  wenger
// Changed JS version to 4.3 because of package.
//
// Revision 1.54  2001/05/11 20:36:07  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.53  2001/05/03 16:28:01  xuk
// Changed version to 6.2
//
// Revision 1.52  2001/04/20 21:05:11  wenger
// Changed 3D version of JAVAC_ShowRecords to use the previously-defined
// JAVAC_ShowRecords3D command; DEVise accepts that command but doesn't
// but doesn't actually return records yet; updated JavaScreen version;
// updated command documentation and protocol version.
//
// Revision 1.51  2001/04/18 16:04:53  wenger
// Changed the JSA version of the JavaScreen to have two rows of buttons
// because we're running out of space; changed JS version to 4.1.
//
// Revision 1.50  2001/04/01 03:51:18  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.49  2001/02/21 17:46:39  xuk
// Deleted DISABLECOLLAB and ENABLECOLLAB.
//
// Revision 1.48  2001/02/12 02:49:02  xuk
// Added DISABLECOLLAB and ENABLECOLLAB. Prevent JS from collaboration.
//
// Revision 1.47  2001/01/30 03:05:50  xuk
// Add API_JAVA_CID = 9, for collabration JavaScreen.
//
// Revision 1.46  2001/01/22 17:08:12  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.45  2001/01/08 20:31:51  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.44.2.11  2000/12/22 17:32:10  wenger
// Changed JS version to 4.0 and protocol version to 5.0.
//
// Revision 1.44.2.10  2000/12/07 17:36:45  xuk
// *** empty log message ***
//
// Revision 1.44.2.9  2000/11/22 17:43:58  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.44.2.8  2000/11/21 01:51:32  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.44.2.7  2000/11/10 15:50:38  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.44.2.6  2000/11/09 21:43:13  wenger
// js.cgi now uses CGI library, various other cleanups; changed CGI parameter
// names slightly; added -cgi argument to JS usage message.
//
// Revision 1.44.2.5  2000/11/01 22:14:17  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.44.2.4  2000/10/28 19:30:52  xuk
// add global variables cgi and cgiURL for cgi communication.
//
// Revision 1.44.2.3  2000/10/23 19:57:39  xuk
// Add time constants for checking the death of JS.
// Add constant of default maximum number of clients.
//
// Revision 1.44.2.2  2000/10/02 19:18:28  xuk
// Add a new message type API_JAVA_JS=8, which is used for the command sent from JS to JSPoP including a ID of JS.
//
// Revision 1.44.2.1  2000/09/01 20:10:45  xuk
// Remove the imgport global variable.
//
// Revision 1.44  2000/08/07 17:15:39  wenger
// Removed the 'control-drag' to move a cursor without talking to the devised
// feature because it could end up causing an incorrect state; added
// workaround for bug 607 (problems with control-drag); documented X-only zoom
// in keyboard help; improved JavaScreen makefile; fixed up makejsa.
//
// Revision 1.43  2000/07/27 14:46:33  wenger
// Updated JS end of protocol version to 4.2.
//
// Revision 1.42  2000/07/20 15:42:59  wenger
// Fixed bug 603 (GData errors caused by problems in new parser); eliminated
// old parser.
//
// Revision 1.41  2000/07/14 21:13:08  wenger
// Speeded up 3D GData processing by a factor of 2-3: improved the parser
// used for GData; eliminated Z sorting for bonds-only 3D views; eliminated
// DEViseAtomTypes for atoms used only to define bond ends; reduced string-
// based processing; eliminated various unused variables, etc.
//
// Revision 1.40  2000/07/10 12:26:03  venkatan
// *** empty log message ***
//
// Revision 1.39  2000/06/26 16:48:32  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.38  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.37  2000/04/07 22:43:14  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.36  2000/04/05 15:42:23  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.35  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.34  2000/02/23 21:12:13  hongyu
// *** empty log message ***
//
// Revision 1.33  2000/02/16 15:16:15  wenger
// Changed default jspophost to localhost; changed overall JS version to 3.2.
//
// Revision 1.32  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/01/17 07:50:17  hongyu
// *** empty log message ***
//
// Revision 1.30  1999/12/10 15:30:13  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.29  1999/12/08 15:34:12  wenger
// Changed version to 3.1.
//
// Revision 1.28  1999/12/07 23:24:26  hongyu
// *** empty log message ***
//
// Revision 1.27  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.26  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.25  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.24  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.23  1999/08/17 06:15:17  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.20  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.18  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseGlobals.java

//TEMP package edu.wisc.cs.devise.js.util;
package JavaScreen;

import java.util.*;

public final class DEViseGlobals
{
    // global constants
    //TEMP -- no longer need to distinguish w/ and w/o ID
    // API_JAVA is a JavaScreen command without JS ID; API_JAVA_WID is
    // a JavaScreen command *with* JS ID.
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2,
                              API_CTL = 3, API_JAVA = 5,
			      // API_IMAGE = 6, API_DATA = 7,
			      API_JAVA_WID = 8,
			      API_JAVA_CID = 9;    
    public static final int DEFAULTCMDPORT = 6666, DEFAULTIMGPORT = 6644,
      JSSPORT = 1688, JSPOPPORT = 1689;
    public static final String JSPOPHOST = new String("localhost");
    public static final String VERSION = new String("5.7.0");

    public static final String PROTOCOL_VERSION = new String("16.0");

    public static final int DEFAULTID = 0;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");

    public static final String DEFAULT_COLLAB_PASS = "";

    // If we haven't gotten a heartbeat from a JavaScreen in this interval
    // (in milliseconds) we'll kill the client object on the assumption that
    // the JavaScreen has died.
    public static final int KILLINTERVAL = 24 * 60 * 60 * 1000;

    public static final int DEBUG_THREADS = 0;

    // The default value for the maximum number of clients we allow (once we
    // reach that value, we kill off the oldest client each time a new
    // client connects).
    public static final int DEFAULTMAXCLIENT = 100;

    // global variables

    // global functions

    // Parse the given string; tokens are enclosed by startChar and endChar,
    // or delimited by spaces.  If keep is true, startChar and endChar are
    // included in the output tokens.
    // Note: this method is currently being used only to parse GData,
    // put it should eventually entirely replace the method immediately
    // below.
    public static String[] parseString(String inputStr, char startChar,
                                       char endChar, boolean keep)
    {
        //TEMP -- what about xx{ } or xx{x} ? how should they be parsed?
	String[] result = null;

	// Maximum possible number of tokens.
        int maxToks = inputStr.length();

	// Index into input string of the start and end of each token.
	int[] starts = new int[maxToks];
	int[] ends = new int[maxToks];

	int tokCount = 0;
	int depth = 0; // depth of braces we're currently enclosed by
	boolean inSpaceTok = false;
	int index = 0;
	int length = inputStr.length();
	while (index < length) {
	    char current = inputStr.charAt(index);
	    if (current == startChar) {
	        if (depth == 0) {
		    tokCount++;
		    if (keep) {
		        starts[tokCount-1] = index;
		    } else {
		        starts[tokCount-1] = index + 1;
		    }
		}
		depth++;
	    } else if (current == endChar) {
	        depth--;
		if (depth == 0) {
		   if (keep) {
		       ends[tokCount-1] = index;
		   } else {
		       ends[tokCount-1] = index - 1;
		   }
		}
	    } else if (current == ' ') {
	        if (depth == 0) {
		    if (inSpaceTok) {
		        inSpaceTok = false;
		    }
		}
	    } else {
	        if (depth == 0) {
		    if (!inSpaceTok) {
			inSpaceTok = true;
		        tokCount++;
			starts[tokCount-1] = index;
			ends[tokCount-1] = index;
		    } else {
			ends[tokCount-1] = index;
		    }
		}
	    }

	    index++;
	}

	if (tokCount > 0) {
	    result = new String[tokCount];
	    for (int tokNum = 0; tokNum < tokCount; tokNum++) {
	        if (ends[tokNum] >= starts[tokNum]) {
		    // Note: substring is weird -- it is supposed to be
		    // (firstIndex, lastIndex), but substring(2, 2), for
		    // example, returns a zero-length string -- hence
		    // the '+ 1' below.  RKW 2000-07-20.
		    result[tokNum] = inputStr.substring(starts[tokNum],
		      ends[tokNum] + 1);
		} else {
		    result[tokNum] = "";
		}
	    }
	}

	return result;
    }

    public static String[] parseString(String inputStr, char startChar, char endChar)
    {
        return DEViseGlobals.parseString(inputStr, startChar, endChar, false);
    }

    public static String[] parseString(String inputStr, boolean keep)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', keep);
    }

    public static String[] parseString(String inputStr)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', false);
    }

    public static String[] parseStr(String str, String delim, boolean keep)
    {
        String[] outStr = null;

        if (str == null)
            return null;

        StringTokenizer token = new StringTokenizer(str, delim, false);
        Vector tokenList = new Vector();
        while (token.hasMoreTokens()) {
            try {
                tokenList.addElement(token.nextToken());
            } catch (NoSuchElementException e) {
                // this should not be happening
            }
        }

        outStr = new String[tokenList.size()];
        for (int i = 0; i < outStr.length; i++) {
            if (keep) {
                outStr[i] = (String)tokenList.elementAt(i) + delim;
            } else {
                outStr[i] = (String)tokenList.elementAt(i);
            }
        }

        return outStr;
    }

    public static String[] parseStr(String inputStr, String delim)
    {
        return DEViseGlobals.parseStr(inputStr, delim, false);
    }

    public static String[] parseStr(String inputStr)
    {
        return DEViseGlobals.parseStr(inputStr, "\n", false);
    }

    //TEMP -- why does this return an int instead of a short??
    public static int toUshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset) {
            return 0;
        }

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;

        return ((v1 << 8) + (v2 << 0));
    }

    //TEMP -- why does this return an int instead of a short??
    public static int toUshort(byte[] data)
    {
        return DEViseGlobals.toUshort(data, 0);
    }

    public static int toInt(byte[] data, int offset)
    {
        if (data == null || data.length < 4 + offset) {
            return 0;
	}

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;
        int v3 = (int)data[2 + offset] & 0x000000FF;
        int v4 = (int)data[3 + offset] & 0x000000FF;

        return ((v1 << 24) + (v2 << 16) +  (v3 << 8) + (v4 << 0));
    }

    public static long getCurrentTime()
    {
        return System.currentTimeMillis();
    }

    // Given a string of the form "<major>.<minor>" returns "<major>";
    // if there is no dot, returns the whole string.
    public static String getMajorVersion(String version)
    {
	String result = "";

        int dotIndex = version.indexOf('.');
	if (dotIndex != -1) {
	    result = version.substring(0, dotIndex);
	} else {
	    result = version;
	}

	return result;
    }

    /**
     * Find whether the input string is an instance of the given argument; if
     * so, get the argument value (if any) in the JavaScreen "all one word"
     * format.
     * @param The input string.
     * @param The argument name we're checking for.
     * @param Whether an value is required for this argument.
     * @param A place to stick the argument value.
     * @return True iff the input string is an instance of the given argument.
     */
    public static boolean checkArgument(String input, String argName,
      boolean mustHaveVal, StringBuffer value) throws YException
    {
	boolean result = false;

        if (input.startsWith(argName)) {
	    result = true;
	    String tmpVal = input.substring(argName.length());
	    value.insert(0, tmpVal);
	    if (mustHaveVal && tmpVal.equals("")) {
		throw new YException("Argument " + argName +
		  " must have a value");
	    }
	}

	return result;
    }

}

