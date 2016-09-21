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

// $Id$

// $Log$
// Revision 1.196  2015/07/21 14:40:38  wenger
// Merged js_suggest_button_br_0 thru js_suggest_button_br_1 to trunk.
//
// Revision 1.195  2015/07/09 20:02:44  wenger
// Merged aditya_merge_br_0 thru aditya_merge_br_3 to trunk.
//
// Revision 1.194.2.6  2015/07/20 21:54:52  wenger
// Removed (for now) in-progress About dialog and visualization help.
//
// Revision 1.194.2.5  2015/07/20 21:18:23  wenger
// Moved Feedback button to the right side of the JavaScreen.
//
// Revision 1.194.2.4  2015/07/16 21:50:17  wenger
// A little more cleanup of the JavaScreen feedback GUI.
//
// Revision 1.194.2.3  2015/07/16 19:54:24  wenger
// A lot of the JavaScreen feedback GUI is working, but quite a bit of
// cleanup still needed to the PHP script, and also need to reposition
// the feedback button.  Started on "About" dialog.  Various debug code
// still in place.
//
// Revision 1.194.2.2  2015/06/19 17:02:58  wenger
// Changed "suggest" to "feedback" as per feedback from Eldon (still working
// on moving the feedback button to the right side).  Added -showallbut
// command-line flag (for debugging) that causes the JS to show the
// Jmol and session-specific buttons.
//
// Revision 1.194.2.1  2015/06/18 21:34:09  wenger
// First cut at the "Suggest" button and related HTML form.  Also, a few
// other changes to the menu buttons.  Fixed version in JavaScreen
// help page.
//
// Revision 1.194  2015/05/08 17:54:50  wenger
// Merged mod_perl_br_0 thru mod_perl_br_2 to trunk.
//
// Revision 1.193.4.2  2015/05/08 16:55:14  wenger
// Hopefully final mod_perl cleanup.
//
// Revision 1.193.4.1  2015/05/05 18:34:15  wenger
// Fixed bug 1045:  CGI path dialog now works more intuitively.
//
// Revision 1.193.2.4  2015/07/09 19:00:13  wenger
// Final cleanup of Aditya's changes.
//
// Revision 1.193.2.3  2015/07/08 21:48:04  wenger
// Merged aditya1_br_8 thru aditya1_br_9 to aditya_merge_br.
//
// Revision 1.193.2.2  2015/07/08 20:57:11  wenger
// Merged aditya1_br_7 thru aditya1_br_8 to aditya_merge_br.
//
// Revision 1.193.2.1  2015/07/07 14:56:37  wenger
// Merged aditya1_br_4 thru aditya1_br_5 to aditya_merge_br.
//
// Revision 1.193  2015/02/18 22:53:47  wenger
// The JavaScreen now reports in the log window how long each command
// takes.  Socket mode can now be turned on in an applet by setting the
// usecgi parameter to 0.  Added the capability to make jar files that
// request all-permissions instead of sandbox.
//
// Revision 1.192  2015/01/14 23:35:58  wenger
// Improved messages related to communication protocol version.
// Revision 1.191.8.12  2015/02/13 20:15:44  kancherla
// Fixed the drill down marker disappearing bug
//
// Revision 1.191.8.11  2015/02/04 20:06:07  kancherla
// Fixed bug 1030 - Errors on exit
//
// Revision 1.191.8.10  2015/01/26 21:05:51  kancherla
// Fixed the issue of restart image not showing up in the release version
//
// Revision 1.191.8.9  2015/01/26 18:20:41  kancherla
// Added functionality for enabling selection of a different data point and showing its record by clicking in drill down mode
//
// Revision 1.191.8.8  2015/01/26 16:21:05  kancherla
// Added a check to see if applet is run via browser and if true, calling a dispose method on parent frame
//
// Revision 1.191.8.7  2015/01/26 15:38:56  kancherla
// chnaged the refreshicon location
//
// Revision 1.191.8.6  2015/01/26 14:41:55  kancherla
// Added mouseover to restart button and removed extra space around it
//
// Revision 1.191.8.5  2015/01/16 21:32:58  kancherla
// *** empty log message ***
//
// Revision 1.191.8.4  2015/01/16 21:29:31  kancherla
// *** empty log message ***
//
// Revision 1.191.8.3  2015/01/14 22:26:23  kancherla
// *** empty log message ***
//
// Revision 1.191.8.2  2015/01/14 21:51:01  kancherla
// Getter for status panel
//
// Revision 1.191.8.1  2014/12/13 02:37:49  kancherla
// Fixed bug 1030
//
// Revision 1.191  2012/04/30 22:20:18  wenger
// Merged js_data_save_br_0 thru js_data_save_br_1 to trunk.
//
// Revision 1.190.4.4  2012/04/30 20:39:38  wenger
// (Hopefully final) cleanup.
//
// Revision 1.190.4.3  2012/04/30 19:07:47  wenger
// Fixed toolbar icons for data download; added mouse cursor icon for
// data download, and set the mouse cursor when we go into data download
// mode; added data download info to the JavaScreen help page; updated
// JS & DEVise version histories; put view names back into the download
// files because I think piles are too confusing otherwise.
//
// Revision 1.190.4.2  2012/04/27 21:55:22  wenger
// Added dialog with the URL that's shown when we try to get the browser
// to show a document.
//
// Revision 1.190.4.1  2012/04/27 16:46:01  wenger
// Cleaned up a bunch of temporary/debug code.
//
// Revision 1.190  2012/02/03 16:42:56  wenger
// Found JS bug 1023, added extra debug code to try to diagnose it when
// it recurs.
//
// Revision 1.189  2011/10/10 20:44:06  wenger
// Merged js_button_fix_br_1 thru js_button_fix_br_2 to trunk.
//
// Revision 1.188  2011/09/23 21:47:29  wenger
// Fixed background colors of some dialogs.
//
// Revision 1.187  2011/08/26 15:37:35  wenger
// Merged js_button_fix_br_0 thru js_button_fix_br_1 to trunk.
//
// Revision 1.186.2.13  2011/10/10 20:29:27  wenger
// Fixed up the 'edited dynamics movie' dialog.
//
// Revision 1.186.2.12  2011/10/10 19:27:04  wenger
// More cleanup of session-specific menus: we now properly get rid of
// any session-specific menu items when we close a session.
//
// Revision 1.186.2.11  2011/10/06 15:52:04  wenger
// Some more cleanup of the "session-specific" menu code.
//
// Revision 1.186.2.10  2011/10/05 23:44:51  wenger
// Early version of "session-specific" menu working -- only works for
// showing URLs at this point.
//
// Revision 1.186.2.9  2011/10/05 19:11:58  wenger
// Moved the "Show Selection Trees" button to the Jmol menu to save space.
//
// Revision 1.186.2.8  2011/10/05 17:48:07  wenger
// Okay, removed the debug code.
//
// Revision 1.186.2.7  2011/09/27 20:27:49  wenger
// The movie generation dialog is mostly in place -- the layout needs to
// be fixed.
//
// Revision 1.186.2.6  2011/08/25 21:35:53  wenger
// Hopefully final cleanup of the JavaScreen embedded button fixes.
//
// Revision 1.186.2.5  2011/08/16 22:37:31  wenger
// JS client now has special code to append residue number to URL for
// BMRB dynamics video generation.
//
// Revision 1.186.2.4  2011/08/15 18:51:18  wenger
// Fixed a problem with "obsolete" buttons not getting removed correctly;
// cleaned up a few other things.
//
// Revision 1.186.2.3  2011/06/06 21:50:12  wenger
// Cleaned up the code that actually displays a URL when an embedded button
// is clicked.
//
// Revision 1.186.2.2  2011/06/03 23:21:28  wenger
// Cleaned up some of the junk that was just commented out previously.
//
// Revision 1.186.2.1  2011/06/03 23:10:51  wenger
// Working on getting embedded buttons in the JS working again -- big
// change so far is getting rid of the paint() method in DEViseScreen
// -- I think it was an error that that ever existed.  Lots of test/debug
// code in place right now as I play around with getting buttons to work.
//
// Revision 1.186  2011/04/21 15:34:17  wenger
// Merged js_highlight_thread_fix_br_0 thru js_highlight_thread_fix_br_1
// to trunk.
//
// Revision 1.185.2.3  2011/04/20 21:04:06  wenger
// Added recognition of s2 predicted vs. experimental session names.
//
// Revision 1.185.2.2  2011/04/20 20:12:41  wenger
// Removed debug code.
//
// Revision 1.185.2.1  2011/04/20 17:15:08  wenger
// Changed the DEViseGenericTree.setSelection() method and the YLogGUI
// p() and pn() methods to only actually update the GUI in the event
// dispatched thread, to hopefully cure problems with incorrect 3D
// highlight updating in the s2pred visualization, and null pointer
// problems when showing the log window.  (I actually meant to do some
// earlier commits to this branch with more of the debug code in place,
// but I forgot to do that.)
//
// Revision 1.185  2011/02/13 23:56:24  wenger
// Merged bug_1005_br_0 thru bug_1005_br_1 to trunk.
//
// Revision 1.184.4.2  2011/01/21 19:37:50  wenger
// Cleaned up some of the temporary code.
//
// Revision 1.184.4.1  2011/01/21 19:18:38  wenger
// Initial part of fix of bug 1005 (Jmol loses state on resize) -- it's
// basically working, but needs cleanup because it relies on static
// variables in the DEViseCanvas3DJmol class, etc.; also, there's still
// some test code in place.
//
// Revision 1.184  2010/07/01 17:32:59  wenger
// Implemented JavaScreen to-do 09.019 -- JS window can now be re-sized
// while a session is open, added new view menu options to enlarge and
// reduce by a fixed amount.
//
// Revision 1.183  2010/06/04 16:29:13  wenger
// Enabled auto resizing in drill-down dialog JTable to improve appearance.
//
// Revision 1.182  2010/06/02 17:41:07  wenger
// The JavaScreen now puts the BMRB visualization type into the JSA
// window title if the hidebmrbsess parameter is turned on.
//
// Revision 1.181  2010/06/01 18:50:16  wenger
// Made the the drill-down dialog resizable and made some other cleanups
// (but I think maybe the whole thing should be changed from a Dialog
// to a JDialog, but I haven't gotten that to work yet).
//
// Revision 1.180  2010/05/28 19:29:11  wenger
// Changed open session dialog to show BMRB visualization types (with
// numbers) as the "main" value if they are available, with the new
// hidebmrbsess applet parameter and command-line argument to hide the
// file names completely if visualization types are available; added
// histogram visualization types.
//
// Revision 1.179  2010/05/26 13:54:37  wenger
// Implemented workaround in the JS client for DEVise/JS bug 933 (acdd
// files show up as sessions).
//
// Revision 1.178  2010/05/25 22:19:16  wenger
// The JavaScreen now shows BMRB visualization type along with the
// filename in the 'open session' dialog.
//
// Revision 1.177  2010/05/20 18:38:41  wenger
// In the JavaScreen, you can now switch sessions without explicitly
// closing the current one before opening the new one (to make it easier
// to switch sessions in the new Peptide-CGI setup).
//
// Revision 1.176  2010/04/21 17:10:18  wenger
// Merged devise_dist_rest_1003_br_0 thru devise_dist_rest_1003_br_1 to trunk.
//
// Revision 1.175.4.1  2010/03/29 18:17:47  wenger
// Got things to work as an applet with the latest Jmol version -- needed
// some more initialization in creating the JmolViewer object.  Added
// the jsdevisec.pnStackTrace() method, since we can't get a Java
// console with the lab's Firefox setup.
//
// Revision 1.175  2009/09/10 22:06:39  wenger
// Fixed JavaScreen bug 985 (change 'Reset Filters' to 'Reset Axis Ranges');
// made other menu improvements; changed communication mode dialog to
// menu for simplification.
//
// Revision 1.174  2009/09/04 19:53:12  wenger
// Partially fixed JS bug 981 (all values are now displayed right-justified,
// whether they are numbers or not).
//
// Revision 1.173  2009/06/11 20:38:55  wenger
// Fixed a bug with the new drill-down marker code that caused a null
// pointer exception when drilling down in a Jmol canvas.
//
// Revision 1.172  2009/05/19 18:13:42  wenger
// Implementation of JavaScreen drill-down marker is mostly done --
// committing now with it disabled to make sure changes don't get lost.
//
// Revision 1.171  2008/02/22 17:26:05  wenger
// Fixed some bugs in showing Java properties in the info window.
//
// Revision 1.170  2008/02/20 20:22:11  wenger
// JavaScreen now defaults to showing help in browser window (to
// get around problems on Mac with the Java window); added links
// and images to the help page because of this change.
//
// Revision 1.169  2008/02/13 22:38:17  wenger
// Added a button to show the Jmol selection trees without having to
// go into a menu.
//
// Revision 1.168  2008/02/13 21:21:20  wenger
// The JavaScreen now prints various useful information, such as the
// Java version, OS version, etc., at the beginning of the Program
// Information log.
//
// Revision 1.167  2008/01/24 20:30:53  wenger
// Merged js_ie_fix_br_0 thru js_ie_fix_br_1 to the trunk.
//
// Revision 1.166  2008/01/22 20:02:38  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron); I tried backporting
// my fix to the pre-toolbar version of the JS, but it doesn't work for
// some reason (I suspect that some of the other cleanups since then
// also affect the fix).  Note that this commit has a bunch of temporary
// code still in place; I want to get a working version into CVS ASAP.
//
// Revision 1.165  2007/12/19 00:08:32  wenger
// Changed a bunch of JButton references to DEViseButtons to make things
// more consistent.
//
// Revision 1.164  2007/08/27 20:53:46  wenger
// Made separate menu items for showing and hiding view help, as requested
// by Chris Schulte.
//
// Revision 1.163  2007/08/27 19:16:40  wenger
// Merged andyd_gui_br_7 thru andyd_gui_br_8 to trunk.
//
// Revision 1.162  2007/08/03 20:17:28  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.161  2007/06/27 17:48:00  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.160.4.1  2008/01/22 22:11:21  wenger
// Fixed bug 954 (JavaScreen locks up IE for Miron) in pre-toolbar version
// of the JavaScreen (the problem before was that I forgot to make
// jsdevisec.destroy() non-synchronized).
//
// Revision 1.160  2007/05/14 16:55:46  wenger
// Updated JavaScreen version history; changed JavaScreen version to 5.8.2
// for release.
//
// Revision 1.159  2007/04/23 19:41:47  wenger
// Added some extra GUI-thread-related diagnostic output.
//
// Revision 1.158  2007/04/20 19:42:36  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.157  2007/03/30 17:29:57  wenger
// Moved some GUI functions to the AWT-EventQueue thread (which is where
// they should be) to more correctly fix JavaScreen 5.8.0 lockup problems.
// (Note: there are probably many more that should be fixed.)
//
// Revision 1.156  2007/03/30 15:43:10  wenger
// (Hopefully) cured the lockups we've been seeing with JS 5.8.0 (removed
// a bunch of calls to validate() in the GUI); fixed up the client logging
// functionality somewhat; various improvements to debug output.
//
// Revision 1.155  2007/02/22 23:20:23  wenger
// Merged the andyd_gui_br thru andyd_gui_br_2 to the trunk.
//
// Revision 1.154.2.17  2007/08/10 15:27:27  wenger
// Toolbar now stays in the same mode after an action as opposed to
// resetting to the default mode.
//
// Revision 1.154.2.16  2007/07/25 18:25:18  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.154.2.15  2007/07/24 21:50:29  wenger
// We now create (but don't use yet) mouse cursors corresponding to the
// various toolbar states.
//
// Revision 1.154.2.14  2007/06/21 14:41:20  wenger
// Tried changing the DEViseScreen to extend a JPanel instead of a Panel
// (this fixed the problem with toolbar tooltips being obscured, but
// caused the JS to lock up really a lot, so I have temporarily changed
// it back); changed some of the tooltip text; put code into
// DEViseToolBar.java to make sure graphics code is getting called from
// the AWT Event thread.
//
// Revision 1.154.2.13  2007/06/18 19:57:22  wenger
// Toolbar works for drill-down (including Jmol); we go back to "normal"
// mode after drilling down; drill-down in Jmol is now disabled by
// default; removed Jmol menu options to enable/disable drill-down;
// removed unneeded utils stuff from 'make clean' target.
//
// Revision 1.154.2.12  2007/06/15 20:46:09  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.154.2.11  2007/06/15 16:34:43  wenger
// Got JavaScreen toolbar icon images to load correctly from jar files (and
// significantly cleaned up the image loading in general, getting rid of a
// bunch of duplicate code).
//
// Revision 1.154.2.10  2007/06/01 09:45:46  adayton
// Added toolbar UI elements, although none of the buttons are working yet.
//
// Revision 1.154.2.9  2007/04/20 17:00:00  wenger
// Fixed the problem with the JavaScreen buttons showing up with the
// wrong font; improved handling of color arguments.
//
// Revision 1.154.2.8  2007/04/19 21:16:09  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.154.2.7  2007/04/13 15:51:33  wenger
// Fixed a problem with Andy's changes that prevented the JavaScreen
// from working when embedded in a browser.
//
// Revision 1.154.2.6  2007/04/10 22:50:27  wenger
// Undid a bunch of formatting changes to make subsequent merges to the
// trunk easier.
//
// Revision 1.154.2.5  2007/04/06 16:18:48  wenger
// Re-did fixing the communication mode color (this seems to have
// gotten lost somehow).
//
// Revision 1.154.2.4  2007/03/16 17:12:47  adayton
// Add UI package
//
// Revision 1.154.2.3  2007/02/22 18:14:31  wenger
// Changed communiation mode label to gray so it's visible against
// the new background color.
//
// Revision 1.154.2.2  2007/02/13 18:35:15  adayton
// Updated basic colors, as well as images for 'traffic light' and throbber. Also started updating some components to use Swing, including the main application frame. Additional changes to my makefile as well.
//
// Revision 1.154.2.1  2007/01/25 22:07:08  adayton
// added Makefile.config.adayton
//
// Revision 1.154  2006/12/13 22:42:18  wenger
// Added option to disable Jmol drill down (to avoid accidental drill-
// down); changed "Cancel" to "Close" in the Option dialog.
//
// Revision 1.153  2006/09/28 16:28:24  wenger
// JavaScreen drill-down dialog now does not display the "_url" suffix
// on URL attribute names.
//
// Revision 1.152  2006/09/26 22:55:52  wenger
// Improvements to the JavaScreen drill-down dialog: added the capability
// to have a description that's different from the associated URL; also
// added the capability to have attributes that don't show up in the
// drill-down dialog (attribute name must end with "_nodd").
//
// Revision 1.151  2006/08/31 19:03:37  wenger
// Added "clickable URL in JavaScreen drill-down dialog" feature -- if
// attribute name ends in "_url" the JS considers it a URL.
//
// Revision 1.150  2006/08/30 18:02:58  wenger
// Merged in the "drill-down dialog uses JTable" changes from the 2.1
// branch (1.144.2.1->1.144.2.2), with some cleanup.
//
// Revision 1.149  2006/08/14 18:04:50  wenger
// Added Jmol version to the JavaScreen Option dialog.
//
// Revision 1.148  2006/06/23 19:52:42  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.147  2006/05/26 16:22:18  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.146.4.7  2006/06/14 16:32:02  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//
// Revision 1.146.4.6  2006/06/12 22:05:03  wenger
// Improved fonts -- larger, buttons/menus are now sans-serif.
//
// Revision 1.146.4.5  2006/05/23 18:17:50  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//
// Revision 1.146.4.4  2005/12/15 19:28:09  wenger
// JmolPanel is now sized to match 3D DEViseCanvas; removed
// Jmol stuff from DEViseScreen and jsdevisec.
//
// Revision 1.146.4.3  2005/12/15 19:04:06  wenger
// Jmol is now in the DEViseScreen and DEViseCanvas (test
// version only).
//
// Revision 1.146.4.2  2005/12/13 22:50:55  wenger
// Jmol is now in the jsdevised; it loads a PDB file so it displays
// an actual protein.
//
// Revision 1.146.4.1  2005/12/09 20:58:54  wenger
// Got Jmol to show up in the JavaScreen! (not yet connected to a
// visualization); added a bunch of debug code to help understand
// things for Jmol.
//
// Revision 1.146  2005/12/06 20:00:24  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.145  2003/01/13 19:23:45  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
// Revision 1.144.2.2  2002/07/26 16:46:21  sjlong
// Added JTable formatting to the data that appears when the user shift clicks.
//
// Revision 1.144  2002/07/19 17:06:49  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.143  2002/06/17 19:40:18  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.142  2002/05/01 21:29:00  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.141.2.25  2003/12/22 22:47:15  wenger
// JavaScreen support for print color modes is now in place.
//
// Revision 1.141.2.24  2003/12/03 19:31:09  wenger
// Changed most buttons in the JavaScreen GUI to menus (to save space
// in preparation for adding new functionality).
//
// Revision 1.141.2.23  2003/12/02 23:12:52  wenger
// Added FUNKY_COLORS flag to jsdevisec so we can see which GUI element
// is which.
//
// Revision 1.141.2.22  2003/09/23 21:55:12  wenger
// "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.
//
// Revision 1.141.2.21  2003/06/26 20:18:04  wenger
// Added closeSession() and openSession() methods to the JS applet for
// Wavelet-IDR.
//
// Revision 1.141.2.20  2003/05/21 16:11:19  wenger
// Fixed bug 872 (double append to path in JS open session dialog).
//
// Revision 1.141.2.19  2003/05/02 17:16:17  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.141.2.18  2002/12/17 23:15:02  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.141.2.17  2002/11/25 21:29:35  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.141.2.16  2002/09/27 22:10:58  wenger
// Fixed bug 796 (JS sometimes thinks it's a collaboration leader when it
// really isn't).
//
// Revision 1.141.2.15  2002/08/16 15:30:49  wenger
// Fixed bug 803 (better text in playback dialog).
//
// Revision 1.141.2.14  2002/07/19 16:05:22  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.141.2.13  2002/06/19 17:59:22  sjlong
// Fixed Bug 699 - (fix: JS will give a warning if a non-existant
// log file is attempted to be played back.)
//
// Revision 1.141.2.12  2002/06/18 14:03:42  wenger
// Shorted collaboration labels because of space clash with mouse location
// display.
//
// Revision 1.141.2.11  2002/06/14 14:37:35  sjlong
// Fixed Bug 793 (Fix: It is no longer possible to have two "Current Active
// Clients" windows open at the same time.)
// Fixed Bug 794 (Bug: Quickly hitting the 'Help' button twice caused
// the JS to lock-up.)
//
// Revision 1.141.2.10  2002/06/12 14:02:15  sjlong
// Fixed Bugs 789, 790, 791 (Bugs: In collaboration mode, the follower can
// lock up if the 'Open' or 'Stop' buttons are pressed as well as the
// 'JSPOP Status' from the 'Options' menu.)
//
// Revision 1.141.2.9  2002/06/11 19:09:14  sjlong
// Fixed Bug 787 (Fix: 'Become Follower' option is no longer available if you are
// leading)
// Fixed Bug 788 (Fix: When leading, the message to the right of the buttons now
// says "Collaboration (L)" and it says "Collaboration (F)" when following)
//
// Revision 1.141.2.8  2002/06/05 17:21:59  sjlong
// Fixed bug 786 (JavaScreen: From the 'Collaborate' menu, choosing 'Become Leader'
// and then hitting 'Cancel' will make the JavaScreen believe it is a leader.
//
// Revision 1.141.2.7  2002/05/24 17:48:05  wenger
// Fixed bug 782 (getting JSPoP status fails for > 1 DEVised).
//
// Revision 1.141.2.6  2002/05/08 21:16:51  wenger
// Changed some collaboration GUI for more clarity, updated instructions
// accordingly; default collaboration password is now "" as workaround
// for problem filling in default in GUI on some platforms.
//
// Revision 1.141.2.5  2002/04/12 21:21:54  xuk
// Improvement on autotest: test more dialog-related commands.
//
// Revision 1.141.2.4  2002/04/12 15:56:52  xuk
// Improvement for autotest.
//
// Revision 1.141.2.3  2002/04/08 20:50:24  xuk
// In auto play or auto test, ServerState Dialog is started by a seperate thread.
// It's closed after 5 seconds, without user's action.
//
// Revision 1.141.2.2  2002/04/04 21:19:34  xuk
// Fixed bug 768: collaboration followers can close dialog automatically.
// Changed in RecordDlg::okButton.addActionListener().
// Changed in showRecord().
//
// Revision 1.141.2.1  2002/04/04 16:58:02  xuk
// Fix bug 767: changing collaboration GUI.
//
// Revision 1.141  2002/03/20 22:08:15  xuk
// Added automatic collaboration functionality.
//
// Revision 1.140  2002/03/05 17:49:02  wenger
// Minor cleanups, documented new JAVAC_UpdateJS command better.
//
// Revision 1.139  2002/03/04 19:55:31  xuk
// Hide JS version string in jsb, due to limited space.
//
// Revision 1.138  2002/03/01 19:58:53  xuk
// Added new command DEViseCommands.UpdateJS to update JavaScreen after
// a DEViseCommands.Open_Session or DEViseCommands.Close_Session command.
//
// Revision 1.137  2002/02/22 21:47:42  xuk
// Added GUI in "Option" menu to allow user to find out the client ID.
//
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
// JButton to show or hide debug log is now also in the JSB version.
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
//  Added collabPass (default = "guest");
//  Added CollabPassDlg() and showCollabPass() for set collaboration passwd;
// Enter password for collaboration:
//  Added enterCollabPass() and EnterCollabPassDlg().
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
        jmolButton.setVisible(false;)
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

    public void setDrillDownMarkerCoordinates(int xCoord, int yCoord, DEViseCanvas2D canvas2D){
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
        if(recorddlg != null){
            recorddlg.getOkButton().doClick(); //automatically closes an open dialog so that a new dialog can be opened
        }
        recorddlg = new RecordDlg(parentFrame, isCenterScreen, msg, this);
        dCanvas2D.drawDrillDownMark(drillDownX, drillDownY);
        recorddlg.setModalityType(ModalityType.MODELESS);
        recorddlg.open();
        if (specialID == -1 && ! jsValues.session.autoPlayback)
        recorddlg = null;
    }

    public String showViewDialogHelp(String msg){
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

    public static RecordDlg getInstance(){
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
