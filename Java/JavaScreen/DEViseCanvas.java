//TEMPTEMP Look at the interfaces in org.jmol.api
//TEMP -- why is DEViseCanvas a Container instead of a Canvas???
// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides an interface to canvases in the JavaScreen.
// It puts images into canvases, and also does the drawing of cursors,
// GData, and so on.  It holds some information related to piles (pile
// information is divided between DEViseCanvas and DEViseView).  This
// class also handles events that occur in views.

// There is one instance of this class for each top-level view in
// the session (note that if top-level views are piled, there's only
// one DEViseCanvas for the entire pile).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.102  2008/07/17 20:27:56  wenger
// (Mostly) fixed bug 968 (JavaScreen doesn't correctly handle cursors
// that are entirely outside the destination view's visual filter).
//
// Revision 1.101  2007/09/10 22:10:35  wenger
// Mouse cursor now changes to the disabled cursor if the selected
// toolbar mode is not applicable in the current view; mouse cursor
// is default on non-view areas; changed Jmol views to not show the
// non-applicable mouse cursors (this only happened in some JVMs).
//
// Revision 1.100  2007/08/03 20:17:27  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.99  2007/06/27 17:47:58  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.98.2.4  2007/08/03 19:21:13  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.98.2.3  2007/07/25 18:25:16  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.98.2.2  2007/06/19 20:49:47  wenger
// Toolbar now works for the various zoom modes and for enlarging/reducing
// symbol size; removed buttons for Y-only zoom modes (not supported right
// now).
//
// Revision 1.98.2.1  2007/06/19 19:32:33  wenger
// Toolbar now works for help, home, cursor fill, and toggling visual
// filters; increased the spacing between the "sections" of icons.
//
// Revision 1.98  2006/12/08 16:24:36  wenger
// Merged V1_8b0_br_1 thru V1_8b0_br_2 to the trunk (took some manual
// changes to merge the DEViseCanvas.java stuff correctly).
//
// Revision 1.97  2006/05/26 16:22:13  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.96.4.11  2006/05/12 14:50:25  wenger
// Now have two trees for a Jmol view: the first one selects which
// atoms are shown in the Jmol view and in the second tree; the
// second tree selects which atoms are highlighted in the Jmol view.
//
// Revision 1.96.4.10  2006/03/31 22:41:13  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.96.4.9  2006/03/30 20:51:25  wenger
// Partially done splitting up the DEViseCanvas class.
//
// Revision 1.96.4.8  2006/02/28 22:31:47  wenger
// Implemented highlighting in Jmol views.
//
// Revision 1.96.4.7  2006/02/23 22:08:38  wenger
// Added flag for whether or not 3D views should use Jmol.
//
// Revision 1.96.4.6  2006/02/23 16:57:42  wenger
// Cleaned up JavaScreen code that sends data to Jmol, including
// adding new DEViseJmolData class.
//
// Revision 1.96.4.5  2006/02/02 22:41:48  wenger
// New tests for loading PDB files and selecting and coloring atoms.
//
// Revision 1.96.4.4  2005/12/29 21:19:19  wenger
// Improved Jmol integration into the JavaScreen -- the Jmol
// viewer isn't constantly destroyed and constructed; also a
// bunch of other improvements in the Jmol-related DEViseCanvas
// code.
//
// Revision 1.96.4.3  2005/12/15 19:28:09  wenger
// JmolPanel is now sized to match 3D DEViseCanvas; removed
// Jmol stuff from DEViseScreen and jsdevisec.
//
// Revision 1.96.4.2  2005/12/15 19:04:02  wenger
// Jmol is now in the DEViseScreen and DEViseCanvas (test
// version only).
//
// Revision 1.96.4.1  2005/12/09 20:58:52  wenger
// Got Jmol to show up in the JavaScreen! (not yet connected to a
// visualization); added a bunch of debug code to help understand
// things for Jmol.
//
// Revision 1.96.2.1  2006/12/07 22:09:06  wenger
// Fixed bug 929 (drill-down on view with empty data source causes crash)
// and associated bugs in the JavaScreen client.
//
// Revision 1.96  2005/12/06 20:00:15  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.95  2003/01/13 19:23:41  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.94  2002/07/19 17:06:46  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.93  2002/06/17 19:40:13  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.92  2002/05/01 21:28:58  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.91.2.17  2005/11/01 19:30:23  wenger
// JS now sends 3D coordinate updates only when it really needs to
// (when it's a collaboration leader).
//
// Revision 1.91.2.16  2003/04/25 20:26:59  wenger
// Eliminated or reduced "Abrupt end of input stream reached" errors in
// the JSPoP on normal client exit.
//
// Revision 1.91.2.15  2003/04/02 16:19:35  wenger
// Possible fix for hard-to-reproduce null pointer exception in
// DEViseCanvas.checkMousePos(); a little bit of clean up of ugly
// DEViseCollabDlg class.
//
// Revision 1.91.2.14  2002/12/17 18:08:22  wenger
// Fixed bug 844 (slow rubberband line drawing in child views).
// Note: this fix involves some pretty signficant changes to how the
// canvas is painted and rubberband lines are drawn, so it needs some
// testing.
//
// Revision 1.91.2.13  2002/09/27 18:44:41  wenger
// Fixed bug 810 (child view titles in JS can go outside view bounds).
//
// Revision 1.91.2.12  2002/08/29 17:27:21  sjlong
// Fixed Bug 798 - Mouse location is only displayed now when in data area
//
// Revision 1.91.2.11  2002/08/20 18:56:58  sjlong
// Bug 673 fixed (bug: 3D drill-down often doesn't work on the first click)
//
// Revision 1.91.2.10  2002/08/20 14:46:39  sjlong
// Fixed "help" bugs 609 and 621.
//
// Revision 1.91.2.9  2002/07/19 16:05:19  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.91.2.8  2002/06/26 17:29:31  wenger
// Improved various error messages and client debug log messages; very
// minor bug fixes; js_log script is now part of installation.
//
// Revision 1.91.2.7  2002/05/17 17:13:09  wenger
// Did some cleanup of the JavaScreen axis labeling code (including fixing
// a bug that prevented the BMRB histogram sessions from working).
//
// Revision 1.91.2.6  2002/04/24 20:06:36  xuk
// Fixed bug 777: avoid long waiting time for repainting in DEViseCanvas
// Handle double-buffering in paint();
// Added paintCanvas() method for painting.
//
// Revision 1.91.2.5  2002/04/10 17:30:19  xuk
// Fixed bug 773.
//
// Revision 1.91.2.4  2002/04/09 18:21:39  wenger
// 'C'/'c' (full cursor) and <backspace> (last visual filter) keys now
// work in views that have key actions disabled (other key actions are
// still disabled); cleaned up JavaScreen key event handling code; fixed
// bug 772; hopefully fixed first part of bug 765; JavaScreen now allows
// 'C'/'c' and <backspace> keys even in views with key actions disabled;
// JS tells user if they attempt a key action in a view where key actions
// are disabled (bug 770).
//
// Revision 1.91.2.3  2002/04/04 16:59:11  xuk
// Fixed bug 769: use Shift-drag as an alternative to Alt-drag.
//
// Revision 1.91.2.2  2002/04/02 22:51:25  xuk
// Fixed bug 762 and 763.
//
// Revision 1.91.2.1  2002/03/30 19:16:05  xuk
// Fix bugs for displaying axis labels for very small values.
//
// Revision 1.91  2002/03/01 19:58:48  xuk
// Added new command DEViseCommands.UpdateJS to update JavaScreen after
// a DEViseCommands.Open_Session or DEViseCommands.Close_Session command.
//
// Revision 1.90  2002/02/20 18:08:39  xuk
// Improvement on axis label drawing.
//
// Revision 1.89  2002/02/19 22:13:28  xuk
// Improvement for label drawing.
//
// Revision 1.88  2002/02/18 22:07:59  xuk
// Modification for axis label drawing.
//
// Revision 1.87  2002/02/13 17:33:33  xuk
// Drawing axis labels according to the string length in pixels.
//
// Revision 1.86  2002/02/12 20:55:45  xuk
// Improvement to drawing axis labels.
//
// Revision 1.85  2002/02/06 23:07:07  xuk
// Draw the axis labels in the JavaScreen.
// Added new function paintAxisLabel().
//
// Revision 1.84  2001/12/04 18:59:37  wenger
// Added missing space in JAVAC_Set3DConfig commands; better debug
// info in DEViseCommSocket.java.
//
// Revision 1.83  2001/11/28 21:56:18  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.82.4.1  2001/11/21 23:35:36  wenger
// Fixed bug 729 (JavaScreen problem displaying mouse location for date
// axes).
//
// Revision 1.82  2001/10/16 22:14:27  wenger
// Major cleanup of command playback code; fixed bug 711 (problem with
// command log playback).
//
// Revision 1.81  2001/09/28 18:57:28  xuk
// JavaScreen command log playback.
// Modified in paint() mothod.
//
// Revision 1.80  2001/09/26 15:11:57  wenger
// Fixed bugs 694 (JS rubberband line now reflects X-only zoom) and 695
// (JS now properly deals with disallowed cursor movement).
//
// Revision 1.79  2001/09/25 18:49:11  wenger
// Fixed bug 689 (JS no longer draws a rubberband line on mouse drags in
// non-zoomable views); found bugs 693-695.
//
// Revision 1.78  2001/05/11 20:36:04  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.77  2001/05/03 19:34:39  xuk
// Added two factors for displaying mouse position.
//
// Revision 1.76  2001/05/03 16:27:18  xuk
// Added multiply factor for displaying mouse postion.
// Added factor argument when calling updateInfo().
//
// Revision 1.75  2001/04/23 20:04:05  wenger
// Hopefully fixed null pointer problem.
//
// Revision 1.74  2001/04/20 21:05:10  wenger
// Changed 3D version of JAVAC_ShowRecords to use the previously-defined
// JAVAC_ShowRecords3D command; DEVise accepts that command but doesn't
// but doesn't actually return records yet; updated JavaScreen version;
// updated command documentation and protocol version.
//
// Revision 1.73  2001/04/20 19:22:21  xuk
// Changes for 3D drill-down of segment objects.
//
// Revision 1.72  2001/04/20 01:23:46  xuk
// Added Functionality for 3D view drill-down.
// 1.Changed mouseReleased() function;
// 2.Added dirllDown3D() function.
//
// Revision 1.71  2001/04/01 03:51:12  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.70  2001/03/25 20:38:22  xuk
// Send JAVAC_Collab_3DView command for 3D view collaboraion.
// 1. key 'R' or 'r' pressed;
// 2. mouse pressed;
// 3. mouse dragged;
// 4. mouse released.
//
// Revision 1.69  2001/03/20 17:49:45  xuk
// Added collaboration for 3D Views.
//
// Revision 1.68  2001/01/08 20:31:50  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.66.2.6  2000/11/22 17:43:56  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.66.2.5  2000/11/21 01:51:29  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.66.2.4  2000/10/18 20:28:09  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.67  2000/10/03 19:23:26  wenger
// Fixed bugs 592 and 594 (JavaScreen sometimes missing keystrokes in views).
//
// Revision 1.66.2.3  2000/10/18 18:29:12  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.66.2.2  2000/10/11 03:00:21  xuk
// Add notifyall() to wake up dispatcher thread.
//
// Revision 1.66.2.1  2000/10/09 16:32:47  xuk
// Change dispathcer.start() to dispatcher.command = cmd, since D
// EViseCmdDispatcher thread starts only once now.
//
// Revision 1.66  2000/08/07 20:21:23  wenger
// Fixed problems with DEViseView.getFirstCursor() method; cleanup of
// DEViseCanvas CVS log, and other minor cleanup.
//
// Revision 1.65  2000/08/07 17:15:39  wenger
// Removed the 'control-drag' to move a cursor without talking to the devised
// feature because it could end up causing an incorrect state; added
// workaround for bug 607 (problems with control-drag); documented X-only zoom
// in keyboard help; improved JavaScreen makefile; fixed up makejsa.
//
// Revision 1.64  2000/08/07 04:45:23  venkatan
// YError that occurs when mouse-clicked in misc/FamMed_visits.tk is fixed.
//
// Revision 1.63  2000/08/07 03:28:51  venkatan
// Zoom for X - only direction is now available - done with CTRL + Rubberband
//
// Revision 1.62  2000/07/26 15:53:41  venkatan
// Hand - Mouse Cursor is used in the 3D view
//
// Revision 1.61  2000/07/21 03:23:42  venkatan
// Help Display - Multiple lines are now in a single "panel"
//
// Revision 1.60  2000/07/20 15:51:12  wenger
// Fixed bug 586 (can't move cursors with spaces in the names).
//
// Revision 1.59  2000/07/19 20:11:35  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.58  2000/07/18 18:12:49  venkatan
// The highlighting of the border for the active view is disabled.
//
// Revision 1.57  2000/07/18 18:10:56  venkatan
// *** empty log message ***
//
// Revision 1.56  2000/07/14 21:13:07  wenger
// Speeded up 3D GData processing by a factor of 2-3: improved the parser
// used for GData; eliminated Z sorting for bonds-only 3D views; eliminated
// DEViseAtomTypes for atoms used only to define bond ends; reduced string-
// based processing; eliminated various unused variables, etc.
//
// Revision 1.55  2000/07/12 16:52:42  wenger
// F1 key now shows help without clicking mouse button.
//
// Revision 1.54  2000/07/11 16:39:18  venkatan
// *** empty log message ***
//
// Revision 1.53  2000/07/10 12:26:02  venkatan
// *** empty log message ***
//
// Revision 1.52  2000/06/27 16:44:34  wenger
// Considerably cleaned up and simplified the cursor-related code; moved
// cursor grid implementation from the DEViseCanvas class to the DEViseCursor
// class.
//
// Revision 1.51  2000/06/21 18:37:29  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.50  2000/06/21 18:10:14  wenger
// Changes to 3D requested by BMRB: removed axes; up/down mouse movement
// does zooming; molecule doesn't move when changing atoms; 'r' resets
// location and zoom as well as rotation.
//
// Revision 1.49  2000/05/25 15:37:15  wenger
// Fixed bug 590 (JavaScreen "forgets" that you have control or alt pressed
// after you release the mouse button).
//
// Revision 1.48  2000/05/25 14:47:27  wenger
// 3D coordinate system remains unchanged when new GData arrives; 'r' or 'R'
// in view resets to default coordinates.
//
// Revision 1.47  2000/05/24 18:52:28  wenger
// Fixed problem with grabbing of cursor edges not working well.
//
// Revision 1.46  2000/05/22 17:52:48  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.45  2000/05/12 20:43:45  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
// Revision 1.44  2000/05/11 20:19:32  wenger
// Cleaned up jsdevisec.java and added comments; eliminated
// jsdevisec.lastCursor (not really needed).
//
// Revision 1.43  2000/05/08 16:40:39  wenger
// JavaScreen cursors are now drawn as frames rather than filled
// rectangles (so symbols, especially text, show up better within cursors).
//
// Revision 1.42  2000/05/04 15:53:22  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.41  2000/04/27 20:15:23  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.40  2000/04/24 20:21:58  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.39  2000/04/07 22:43:13  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.38  2000/04/05 06:25:36  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.37  2000/04/03 22:24:52  wenger
// Added named constants for GData symbol types; 3D GData symbols are now
// differentiated by symbol type instead of string; removed some commented-
// out code.
//
// Revision 1.36  2000/03/23 16:26:12  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.35  2000/02/23 21:12:11  hongyu
// *** empty log message ***
//
// Revision 1.34  2000/02/23 10:44:01  hongyu
// *** empty log message ***
//
// Revision 1.33  2000/02/22 15:26:39  wenger
// Commented out debug output.
//
// Revision 1.32  2000/02/22 10:00:38  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/02/16 08:53:48  hongyu
// *** empty log message ***
//
// Revision 1.29  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.28  1999/12/10 15:30:12  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.27  1999/12/07 23:18:20  hongyu
// *** empty log message ***
//
// Revision 1.25  1999/12/06 23:22:22  hongyu
// *** empty log message ***
//
// Revision 1.24  1999/12/02 19:47:23  hongyu
// *** empty log message ***
//
// Revision 1.23  1999/11/11 18:11:51  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/11/03 08:00:48  hongyu
// *** empty log message ***
//
// Revision 1.21  1999/11/01 17:40:15  hongyu
// *** empty log message ***
//
// Revision 1.20  1999/10/14 15:08:53  hongyu
// *** empty log message ***
//
// Revision 1.19  1999/10/12 22:00:03  hongyu
// *** empty log message ***
//
// Revision 1.17  1999/10/12 00:08:42  hongyu
// *** empty log message ***
//
// Revision 1.16  1999/10/10 08:49:51  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.15  1999/09/24 17:11:46  hongyu
// adding support for 3-d molecule view
//
// Revision 1.14  1999/08/24 08:45:52  hongyu
// *** empty log message ***
//
// Revision 1.13  1999/08/19 07:20:51  hongyu
// *** empty log message ***
//
// Revision 1.12  1999/08/17 06:15:16  hongyu
// *** empty log message ***
//
// Revision 1.11  1999/08/03 05:56:35  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.9  1999/07/27 17:11:17  hongyu
// *** empty log message ***
//
// Revision 1.7  1999/06/23 20:59:14  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCanvas.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;

public class DEViseCanvas extends Container
{
    protected jsdevisec jsc = null;
    protected DEViseScreen jscreen = null;
    protected DEViseCmdDispatcher dispatcher = null;
    public int posZ = 0;

    public int oldHighlightAtomIndex = -1, highlightAtomIndex = -1;
    //TEMP -- what is point *for*?
    public Point point = new Point();
    public DEViseCrystal crystal = null;

    public DEViseView view = null; // base view if piled

    // ADD COMMENT -- we need clarification here as to why this is on a
    // per-canvas basis, since there can actually only be one active view
    // at a time in the entire JS; Hongyu says that this does not
    // necessarily get set to null when the active view becomes inactive.
    public DEViseView activeView = null;

    protected Dimension canvasDim = null;

    private Image image = null;
    private Image offScrImg = null;
    private boolean isImageUpdated = false;

    public boolean isMouseDragged = false, isInViewDataArea = false,
      isInDataArea = false;
    protected DEViseCursor selectedCursor = null;

    // See DEViseCursor.side* for values.
    protected int whichCursorSide = DEViseCursor.sideNone;

    // Keep track of whether the mouse is actually in this canvas (part of
    // the fix for the "missing keystrokes" bugs.
    private boolean _mouseIsInCanvas = false;

    // These variables will hold the help messages to diplay when the 'Help' button is pressed
    // for both the main view and any child views.
    public String helpMsg = null;
    public Vector childViewHelpMsgs = new Vector();

    //TEMP -- what are these? op is old point???
    Point sp = new Point(), ep = new Point(), op = new Point();

    //
    // This is a fix to a problem that seems to be in the JVM itself:
    // when a mouse button press is closely followed in time by moving
    // the mouse, Java calls the mouseMoved() method of the
    // MouseMotionListener as opposed to the mouseDragged() method,
    // even though it has already called mousePressed() in the
    // MouseListener.  RKW 2000-05-24.
    //
    private boolean buttonIsDown = false;

    private static final int DEBUG = 0; // 0 - 3

    // Various zoom modes.  Note that _zoomMode is only valid while the
    // mouse button is down or has just been released.
    protected static final int ZOOM_MODE_NONE = 0;
    protected static final int ZOOM_MODE_X = 1;
    protected static final int ZOOM_MODE_Y = 2;
    protected static final int ZOOM_MODE_XY = 3;
    protected static int _zoomMode = ZOOM_MODE_NONE;

    // ASCII codes for keys that we have to send to DEVise (we have to
    // send the actual ASCII integer value as opposed to the character).
    protected static int DEVISE_KEY_TOGGLE_FILTER = 8; // (backspace)
    protected static int DEVISE_KEY_INCREASE_SYM_SIZE = 43; // '+'
    protected static int DEVISE_KEY_DECREASE_SYM_SIZE = 45; // '-'
    protected static int DEVISE_KEY_HOME = 53; // '5'
    protected static int DEVISE_KEY_CURSOR_FILL = 67; // 'C'

    // This is used to paint only rubberband lines (in XOR mode) while
    // we are dragging the rubberband line.  (In other words, we don't
    // actually repaint all of the other stuff on the canvas.)
    protected boolean _paintRubberbandOnly = false;

    // Set this to true to draw rubberband lines in XOR mode (so we can
    // just redraw the rubberband line itself on each mouse drag).
    protected static final boolean DRAW_XOR_RUBBER_BAND = true;

    // v is base view if there is a pile in this canvas.
    public DEViseCanvas(DEViseView v, Image img)
    {
        view = v;
        jsc = view.jsc;
        jscreen = jsc.jscreen;
        dispatcher = jsc.dispatcher;
        image = img;

        if (image != null) {
            canvasDim = new Dimension(image.getWidth(this), image.getHeight(this));
            view.viewLoc.width = canvasDim.width;
            view.viewLoc.height = canvasDim.height;
            view.viewLocInCanvas.width = canvasDim.width;
            view.viewLocInCanvas.height = canvasDim.height;
            isImageUpdated = true;
        }

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);
        this.enableEvents(AWTEvent.KEY_EVENT_MASK);
        addMouseListener(new ViewMouseListener());
        addMouseMotionListener(new ViewMouseMotionListener());
        addKeyListener(new ViewKeyListener());
        addFocusListener(new ViewFocusListener());
    }

    // Called when a session is closed.
    public void close() {}

    public Dimension getPreferredSize()
    {
        return canvasDim;
    }

    public Dimension getMinimumSize()
    {
        return canvasDim;
    }

    public Rectangle getLocInScreen()
    {
        return view.viewLoc;
    }

    public boolean mouseIsWithin()
    {
        return _mouseIsInCanvas;
    }

    // This function is used to draw cursors because direct XOR drawing
    // (with Graphics.setXORMode()) doesn't seem to work in Netscape.
    // loc is relative to this canvas.
    public Image getCroppedXORImage(Color c, Rectangle loc)
    {
        if (image == null)
            return null;

        CropImageFilter croppedFilter = new CropImageFilter(loc.x, loc.y,
	  loc.width, loc.height);
        FilteredImageSource source = new FilteredImageSource(
	  image.getSource(), croppedFilter);
        Image img = createImage(source);
        if (img == null) {
            return null;
        }

        source = new FilteredImageSource(img.getSource(),
	  new XORFilter(c, loc.width, loc.height, XORFilter.typeFrame));
        img = createImage(source);

        return img;
    }

    public synchronized void updateImage(Image img)
    {
        if (img != null) {
            image = img;
            canvasDim.width = image.getWidth(this);
            canvasDim.height = image.getHeight(this);
            view.viewLoc.width = canvasDim.width;
            view.viewLoc.height = canvasDim.height;
            view.viewLocInCanvas.width = canvasDim.width;
            view.viewLocInCanvas.height = canvasDim.height;

            isImageUpdated = true;
        }

        // Necessary to draw correct image because double-buffering is used
        //offScrImg = null;
    }

    // Enable double-buffering
    public void update(Graphics gc)
    {
        if (gc == null) {
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(canvasDim.width, canvasDim.height);
        }

        if (offScrImg == null) {
            paint(gc);
        } else {
            Graphics og = offScrImg.getGraphics();
            paint(og);
            gc.drawImage(offScrImg, 0, 0, this);
            og.dispose();
        }
    }

    // enable double-buffering w/o calling update
    public void paint(Graphics gc)
    {
        if (gc == null) {
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(canvasDim.width, canvasDim.height);
        }

	Graphics og = offScrImg.getGraphics();
	paintCanvas(og);
	gc.drawImage(offScrImg, 0, 0, this);
	og.dispose();

	// Needed for Jmol.
	super.paint(gc);
    }

    public void paintCanvas(Graphics gc)
    {
	// for command log playback
	if (jsc.isPlayback && !jsc.isDisplay) {
	    return;
        }
	
        if (jsc.jsValues.canvas.isInteractive) {
            if (jsc.jsValues.canvas.sourceCanvas != this) {
                return;
            }
        }

//TEMPTEMP -- check whether help works in Jmol views -- no, it doesn't
        if (!_paintRubberbandOnly) {
            // draw 3D molecular view
            if (paintCrystal(gc)) {
                paintBorder(gc);
                paintHelpMsg(gc);
                return;
            }

            // draw the background image
            paintBackground(gc);
	
            // draw titles for all views, including view symbols but not
	    // piled views
            paintTitle(gc);

            // draw all the cursors
            paintCursor(gc);

            // draw all the GDatas
            paintGData(gc, view);
        }

        // draw rubber band
        paintRubberBand(gc);

        if (!_paintRubberbandOnly) {
            // draw highlight border
            paintBorder(gc);

            paintHelpMsg(gc);

	    // draw sxis labels
	    paintAxisLabel(gc);
        }
    }

    protected synchronized void paintBackground(Graphics gc)
    {
        if (image != null) {
            gc.drawImage(image, 0, 0, canvasDim.width, canvasDim.height, this);
        } else {
            gc.setColor(new Color(view.viewBg));
            gc.fillRect(0, 0, canvasDim.width, canvasDim.height);
        }
    }

    private void paintHelpMsg(Graphics gc)
    {
        if (helpMsg == null) {
            return;
        }

	int helpMsgX = 0, helpMsgY = 0;
	String tempHelpMsg = "";
	int minX = 0, minY = 0;

	for(int j = 0; ; j++) {
	    if(j == 0) {
		tempHelpMsg = helpMsg;
	    }
	    else {
		if(childViewHelpMsgs.size() >= j) {
		    tempHelpMsg = (String) childViewHelpMsgs.elementAt(j - 1);
		    if(tempHelpMsg == null) continue;
		    if(view.viewChilds.size() >= j) {
			helpMsgX = ((DEViseView) view.viewChilds.elementAt(j - 1)).viewLoc.x;
			helpMsgY = ((DEViseView) view.viewChilds.elementAt(j - 1)).viewLoc.y;
			// This if will keep help msgs from overlapping
			if(helpMsgY < minY && helpMsgX < minX) {
			    helpMsgY = minY;
			}
		    }
		    else {
			break;
		    }
		}
		else {
		    break;
		}
	    }

	    Toolkit tk = Toolkit.getDefaultToolkit();
	    FontMetrics fm = tk.getFontMetrics(jsc.jsValues.uiglobals.textFont);
	    int ac = fm.getAscent(), dc = fm.getDescent(), height = ac + dc + 12;
	    int width = fm.stringWidth(tempHelpMsg) + 12;
	    int minHeight = height + height/3; int minWidth = fm.stringWidth("THIS IS THE MINIMUM");

	    if(canvasDim.width < minWidth || canvasDim.height < minHeight){
		return;
	    }
     
	    StringTokenizer st = new StringTokenizer(tempHelpMsg, "\n");
	    Vector sv = new Vector();
	    String back = "";

	    while(st.hasMoreTokens()|| !back.equals("")){
		String tmp = back + " ";
		if(st.hasMoreTokens()){
		    tmp  +=  st.nextToken(); 
		}
	   
		int tokenWidth = fm.stringWidth(tmp) + 12;
		if(tokenWidth > canvasDim.width){
		    String[] a = stringSplit(tmp, fm);
		    tmp = a[0];
		    back = a[1];
		    sv.addElement(tmp);

		}
		else{
		    sv.addElement(tmp);
		    back = "";
		}    
	    }
	    
	    if( back.equals("")); 
	    else{
		sv.addElement(back);
	    }
	   
	    int totHeight = height * sv.size() ;
	    if( sv.size () > 3 ){ 
		totHeight += sv.size() * (-5);
	    }

	    int allowedHeight = 4* canvasDim.height/5;
	    if(totHeight> allowedHeight ){
		// may want to put some message;
		return;
	    }


	    //      int dispLength = canvasDim.width;	
	    //      computing display length

	    int dispLength = 0;
	    for(int i = 0; i < sv.size(); i++){
		String s = (String) sv.elementAt(i);
		int w = fm.stringWidth(s);
		if(dispLength < w ){
		    dispLength = w;
		} 
	    }   
	    dispLength += 12;
	    if( dispLength < canvasDim.width){
		int k = canvasDim.width - dispLength;
		if( k  > 6){
		    dispLength +=6;
		}
		else{
		    dispLength += k;
		}
	    } 

	    // setting up the color background and text color.
	    gc.setColor(Color.black);
	    gc.drawRect(helpMsgX, helpMsgY, dispLength -1, totHeight);
	    // If this is the main help msg, make sure no other msg is displayed over this one
	    if(j == 0) {
		minX = dispLength - 1 + 1;
		minY = totHeight + 1;
	    }
	    gc.setColor(new Color(255, 255, 192));
	    gc.fillRect(helpMsgX + 1, helpMsgY + 1,dispLength - 2, totHeight- 1);
	    gc.setColor(Color.black);
	    gc.setFont(jsc.jsValues.uiglobals.textFont);
	    
	    // printing each of the line from the Vector of lines.
	    for( int i = 0 ; i < sv.size(); i++){	
		gc.drawString(((String) sv.elementAt(i)), helpMsgX + 6, helpMsgY + height - dc - 6);
		helpMsgY += (height - dc -6);
	    }
	}
    }	

   private String[]  stringSplit(String a, FontMetrics fm){
       StringTokenizer st = new StringTokenizer(a, " "); 
       int defWidth = 0;
       String tmp ="";
       String[] res = new String[2];
       res[0] = ""; res[1] = "";
       while( st.hasMoreTokens()){
	 tmp = st.nextToken();
	 defWidth = fm.stringWidth(res[0]) + fm.stringWidth(tmp + " ") ;
	 if((defWidth + 5) > canvasDim.width){
           break;
         }
	 else{
	    res[0] = res[0] + " " + tmp;
         }
       }
       res[1] = tmp + " " ;
       while( st.hasMoreTokens()){
	  res[1] += (st.nextToken() + " "); 
       }
       // System.out.println( a + " split as " + res[0] + " and " + res[1]);

       return res;
   }

    protected synchronized boolean paintCrystal(Graphics gc)
    {
        return false;
    }

    protected void resetCrystal()
    {
    }

    protected void doMousePressed()
    {
    }

    protected void doMouseReleased(MouseEvent event)
    {
    }

    protected void doMouseClicked(MouseEvent event)
    {
    }

    protected void doMouseDragged(Point p)
    {
    }

    protected void doMouseMoved(Point p)
    {
    }

    private synchronized void paintBorder(Graphics gc)
    {
       // Border for Active view is disabled. - no highlighting
	/* if (activeView != null && activeView == jscreen.getCurrentView()) {
            Rectangle loc = activeView.viewLocInCanvas;

            gc.setColor(Color.red);
            gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
            gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3, loc.height - 3);
	}
	*/
    }

    private synchronized void paintAxisLabel(Graphics gc)
    {
	Rectangle loc;

        // handling child view
        for (int i = 0; i < view.viewChilds.size(); i++) {
            DEViseView childView = (DEViseView)view.viewChilds.elementAt(i);
	    childView.paintAxisLabels(gc, true);
	}

	view.paintAxisLabels(gc, false);
    }


    // only top views get title drawn
    // TEMP -- child views of piled views should probably have titles drawn
    private synchronized void paintTitle(Graphics gc)
    {
	//TEMP -- should probably reverse the order here

        // handling child view
        for (int i = 0; i < view.viewChilds.size(); i++) {
            DEViseView v = (DEViseView)view.viewChilds.elementAt(i);

            if (v.viewDTFont != null && v.viewTitle != null) {
                gc.setColor(new Color(v.viewFg));
                gc.setFont(v.viewDTFont);

		// Fit title into available width of view.
                String title = fitStringToWidth(v.viewTitle,
	          v.viewLoc.width, gc.getFontMetrics());

		// Re-center (possibly shortened) title.
		int titleX = (v.viewLoc.width -
		  gc.getFontMetrics().stringWidth(title)) / 2 +
		  v.viewLocInCanvas.x;
                gc.drawString(title, titleX, v.viewDTY);
            }
        }
	
        // handling itself
        if (view.viewDTFont != null && view.viewTitle != null) {
            gc.setColor(new Color(view.viewFg));
            gc.setFont(view.viewDTFont);

	    // Fit title into available width of view.
            String title = fitStringToWidth(view.viewTitle,
	      view.viewLoc.width, gc.getFontMetrics());

	    // Re-center (possibly shortened) title.
	    int titleX = (view.viewLoc.width -
	      gc.getFontMetrics().stringWidth(title)) / 2 +
	      view.viewLocInCanvas.x;
            gc.drawString(title, titleX, view.viewDTY);
        }
    }

    // every view will have its cursor properly drawn
    private synchronized void paintCursor(Graphics gc)
    {
        paintCursor(gc, view);

        isImageUpdated = false;
    }

    private synchronized void paintCursor(Graphics gc, DEViseView v)
    {
	//TEMP -- should probably reverse the order here

        // handling child views
        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);

            paintCursor(gc, vv);
        }

        // handling itself
        Rectangle loc = null;
        DEViseCursor cursor = null;
        for (int i = 0; i < v.viewCursors.size(); i++) {
            cursor = (DEViseCursor)v.viewCursors.elementAt(i);
	    if (cursor.cursorIsVisible()) {
                loc = cursor.getLocInCanvas();

                if (selectedCursor == cursor && isMouseDragged) {
		    // draw cursor movement box

                    if (loc.width < 4) loc.width = 4;
                    if (loc.height < 4) loc.height = 4;

                    gc.setColor(Color.yellow);
                    gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
                    gc.setColor(Color.red);
                    gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3,
		      loc.height - 3);

		    // Hmm -- I guess this is here because we can only
		    // drag one cursor at a time.  wenger 2008-07-15
                    continue;
                }

                if (cursor.image == null || isImageUpdated) {
                    cursor.image = getCroppedXORImage(cursor.color, loc);
                }

                if (cursor.image != null) {
                    gc.drawImage(cursor.image, loc.x, loc.y, this);
                }
	    }
        }

        // handling piled views
        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            paintCursor(gc, vv);
        }
    }

    private synchronized void paintGData(Graphics gc, DEViseView v)
    {
	//TEMP -- should probably reverse the order here

        // handling child views
        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);

            paintGData(gc, vv);
        }

        // handling itself
        DEViseGData gdata = null;
	Color tmpColor = null;
	Font tmpFont = null;
        for (int i = 0; i < v.viewGDatas.size(); i++) {
            gdata = (DEViseGData)v.viewGDatas.elementAt(i);

            if (gdata.string != null && (gdata.symbolType == gdata._symText ||
	      gdata.symbolType == gdata._symFixedText)) {

		// Set the color and font only when necessary.
		if (!gdata.color.equals(tmpColor)) {
                    gc.setColor(gdata.color);
		    tmpColor = gdata.color;
		}
		if (!gdata.font.equals(tmpFont)) {
                    gc.setFont(gdata.font);
		    tmpFont = gdata.font;
		}

                gc.drawString(gdata.string, gdata.x, gdata.y);
            }
        }

        // handling piled views
        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            paintGData(gc, vv);
        }
    }

    static boolean _lastRBValid = false;
    static Rectangle _lastRB = new Rectangle();

    private synchronized void paintRubberBand(Graphics gc)
    {
        if (isInViewDataArea && isMouseDragged && selectedCursor == null &&
	  activeView.isRubberBand && jsc.toolBar.doRubberband()) {
            int x0, y0, w, h;

            if (sp.x > ep.x)  {
                x0 = ep.x;
                w = sp.x - x0;
            }  else  {
                x0 = sp.x;
                w = ep.x - x0;
            }

	    // If not zooming in the X direction, the zoom box should
	    // fill the whole data area horizontally.
	    if (_zoomMode != ZOOM_MODE_X && _zoomMode != ZOOM_MODE_XY) {
	        x0 = activeView.viewDataLoc.x + activeView.viewLocInCanvas.x;
		w = activeView.viewDataLoc.width;
	    }

            if (sp.y > ep.y)  {
                y0 = ep.y;
                h = sp.y - y0;
            }  else  {
                y0 = sp.y;
                h = ep.y - y0;
            }

	    // If not zooming in the Y direction, the zoom box should
	    // fill the whole data area vertically.
	    if (_zoomMode != ZOOM_MODE_Y && _zoomMode != ZOOM_MODE_XY) {
	        y0 = activeView.viewDataLoc.y + activeView.viewLocInCanvas.y;
		h = activeView.viewDataLoc.height;
	    }

            if (w < 4) {
                w = 4;
	    }
            if (h < 4) {
                h = 4;
	    }

	    if (DRAW_XOR_RUBBER_BAND) {
	        gc.setXORMode(Color.black);
	        if (_lastRBValid) {
                    gc.drawRect(_lastRB.x, _lastRB.y, _lastRB.width - 1,
		      _lastRB.height - 1);
	        }
	    } else {
                gc.setColor(Color.yellow);
	    }
            gc.drawRect(x0, y0, w - 1, h - 1);

	    if (DRAW_XOR_RUBBER_BAND) {
	        gc.setXORMode(Color.red);
	        if (_lastRBValid) {
                    gc.drawRect(_lastRB.x + 1, _lastRB.y + 1,
		      _lastRB.width - 3, _lastRB.height - 3);
	        }
	    } else {
                gc.setColor(Color.red);
	    }
            gc.drawRect(x0 + 1, y0 + 1, w - 3, h - 3);

	    if (DRAW_XOR_RUBBER_BAND) {
	        gc.setPaintMode();

	        _lastRB.x = x0;
	        _lastRB.y = y0;
	        _lastRB.width = w;
	        _lastRB.height = h;
	        _lastRBValid = true;
	    }
        }
    }

    protected void processMouseEvent(MouseEvent event)
    {
        int id = event.getID();

        if (DEBUG >= 1) {
            System.out.println("DEViseCanvas(" + view.viewName +
	      ").processMouseEvent()");
            if (id == MouseEvent.MOUSE_PRESSED) System.out.println(
	      "  Mouse pressed");
            if (id == MouseEvent.MOUSE_CLICKED) System.out.println(
	      "  Mouse clicked");
            if (id == MouseEvent.MOUSE_RELEASED) System.out.println(
	      "  Mouse released");
        }

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();
	//jsc.pn("DEViseCanvas.processMouseEvent()");
        if (id == MouseEvent.MOUSE_PRESSED || id == MouseEvent.MOUSE_RELEASED
	  || id == MouseEvent.MOUSE_CLICKED) {
            if (dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                if (id == MouseEvent.MOUSE_PRESSED) {
                    jsc.showMsg("JavaScreen still talking to the Server!\nPlease try again later or press STOP button!");
                }

                return;
            }
        }

        super.processMouseEvent(event);
    }

    protected void processMouseMotionEvent(MouseEvent event)
    {
        if (DEBUG >= 2) {
            System.out.println("DEViseCanvas(" + view.viewName +
	      ").processMouseMotionEvent()");
        }

        int id = event.getID();

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();

        if (id == MouseEvent.MOUSE_DRAGGED) {
            if (dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                return;
            }
        }

        super.processMouseMotionEvent(event);
    }

    //TEMP -- move to DEViseCanvas2D?
    protected void processKeyEvent(KeyEvent event)
    {
        if (view.viewDimension != 3) {
            if (dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                //if (event.getID() == KeyEvent.KEY_PRESSED) {
                //    jsc.showMsg("Java Screen still talking to the Server!\nPlease try again later or press STOP button!");
                //}

                return;
            }
        }

        super.processKeyEvent(event);
    }

    // This function will do one of two things.  If the help button has
    // been pushed, and a tan box is showing the help in this view, this
    // function will hide that help box.  If there is not currently a
    // help box displayed, this function will show the view's help in a
    // dialog box.
    protected void showHideHelp()
    {
        // Check to see if a help box is being displayed.
        if(helpMsg != null) {
	    // Hide the help for this view and any of its children
	    helpMsg = null;
	
	    for(int i = 0; i < childViewHelpMsgs.size(); i++) {
	        childViewHelpMsgs.setElementAt(null, i);
	    }
	    repaint();
	    return;
        }
    
        //
        // Show this view's help within a dialog box.
        //
        jsc.jsValues.connection.helpBox = true;
        String cmd = DEViseCommands.GET_VIEW_HELP + " " +
          activeView.getCurlyName() + " " + 0 + " " + 0;
        // jscreen.guiAction = true;
        dispatcher.start(cmd);
    }

    // start of class ViewKeyListener
    class ViewKeyListener extends KeyAdapter
    {
        // event sequence: 1. keypressed 2.keytyped 3.keyreleased
        public void keyPressed(KeyEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseCanvas(" + view.viewName +
		  ").keyPressed(" + event.getKeyChar() + ")");
	    }
            jsc.jsValues.canvas.lastKey = event.getKeyCode();
        }

        //
	// Note: should we move most of this to using a keyTyped handler
	// instead of a keyReleased handler?  That is supposed to be
	// higer-level and more hardware-independent.  RKW 2002-04-09.
	//

	// If this is not a 3D-view, send the appropriate key action
	// to the devised.
        public void keyReleased(KeyEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseCanvas(" + view.viewName +
		  ").keyReleased(" + event.getKeyChar() + ", " +
		  event.getKeyCode() + ")");
	    }
            jsc.jsValues.canvas.lastKey = KeyEvent.VK_UNDEFINED;

            char keyChar = event.getKeyChar();
            int keyCode = event.getKeyCode();

	    // F1 key shows or hides view help.
            if (keyCode == KeyEvent.VK_F1) {
	        showHideHelp();
                return;
            }

            if (view.viewDimension == 3) {
		if (keyChar == 'r' || keyChar == 'R') {
		    resetCrystal();
		}
                return;
            }

            int actualKey = translateKey(keyChar, keyCode);

            if (actualKey != 0 && activeView != null) {
		if (activeView.isKey || isSpecialKey(actualKey)) {
                    String cmd = "";
                    cmd = cmd + DEViseCommands.KEY_ACTION + " " +
		      activeView.getCurlyName() + " " + actualKey;
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
		} else {
		    // For some reason, after we show the dialog we get
		    // an extra key event from the DEViseScreen.
		    // RKW 2002-04-09.
		    if (event.getComponent() instanceof DEViseCanvas) {
	                jsc.showMsg("Key actions are disabled in this view");
		    }
		}
            }
        }

	private int translateKey(char keyChar, int keyCode)
	{
	    if (DEBUG >= 2) {
	        System.out.println(
		  "DEViseCanvas.ViewKeyListener.translateKey(" + keyChar +
		  ", " + keyCode + ")");
	    }

	    int actualKey;

	    // Note: check against 0xff here is to (hopefully) fix the
	    // first part of bug 765.  RKW 2002-04-09.
            if (keyChar != KeyEvent.CHAR_UNDEFINED && keyChar <= 0xff) {
	        if (DEBUG >= 2) {
                    System.out.println("key char is defined");
		}

                actualKey = (int)keyChar;

            } else {
	        if (DEBUG >= 2) {
                    System.out.println("key char is NOT defined");
		}

		// TEMP -- document where these constants come from
                switch (keyCode) {
                case KeyEvent.VK_NUMPAD0:
                    actualKey = 0x40000 + 28;
                    break;
                case KeyEvent.VK_NUMPAD1:
                    actualKey = 0x40000 + 29;
                    break;
                case KeyEvent.VK_NUMPAD2:
                    actualKey = 0x40000 + 30;
                    break;
                case KeyEvent.VK_NUMPAD3:
                    actualKey = 0x40000 + 31;
                    break;
                case KeyEvent.VK_NUMPAD4:
                    actualKey = 0x40000 + 32;
                    break;
                case KeyEvent.VK_NUMPAD5:
                    actualKey = 0x40000 + 33;
                    break;
                case KeyEvent.VK_NUMPAD6:
                    actualKey = 0x40000 + 34;
                    break;
                case KeyEvent.VK_NUMPAD7:
                    actualKey = 0x40000 + 35;
                    break;
                case KeyEvent.VK_NUMPAD8:
                    actualKey = 0x40000 + 36;
                    break;
                case KeyEvent.VK_NUMPAD9:
                    actualKey = 0x40000 + 37;
                    break;
                //case KeyEvent.VK_F1:
                 //   actualKey = 0x50000 + 1;
                 //   break;
                //case KeyEvent.VK_F2:
                 //   actualKey = 0x50000 + 2;
                  //  break;
                case KeyEvent.VK_F3:
                    actualKey = 0x50000 + 3;
                    break;
                case KeyEvent.VK_F4:
                    actualKey = 0x50000 + 4;
                    break;
                case KeyEvent.VK_F5:
                    actualKey = 0x50000 + 5;
                    break;
                case KeyEvent.VK_F6:
                    actualKey = 0x50000 + 6;
                    break;
                case KeyEvent.VK_F7:
                    actualKey = 0x50000 + 7;
                    break;
                case KeyEvent.VK_F8:
                    actualKey = 0x50000 + 8;
                    break;
                case KeyEvent.VK_F9:
                    actualKey = 0x50000 + 9;
                    break;
                case KeyEvent.VK_F10:
                    actualKey = 0x50000 + 10;
                    break;
                case KeyEvent.VK_F11:
                    actualKey = 0x50000 + 11;
                    break;
                case KeyEvent.VK_F12:
                    actualKey = 0x50000 + 12;
                    break;
                case KeyEvent.VK_HOME:
                    actualKey = 0x20000 + 1;
                    break;
                case KeyEvent.VK_LEFT:
                    actualKey = 0x20000 + 2;
                    break;
                case KeyEvent.VK_UP:
                    actualKey = 0x20000 + 3;
                    break;
                case KeyEvent.VK_RIGHT:
                    actualKey = 0x20000 + 4;
                    break;
                case KeyEvent.VK_DOWN:
                    actualKey = 0x20000 + 5;
                    break;
                case KeyEvent.VK_PAGE_UP:
                    actualKey = 0x20000 + 7;
                    break;
                case KeyEvent.VK_PAGE_DOWN:
                    actualKey = 0x20000 + 9;
                    break;
                case KeyEvent.VK_END:
                    actualKey = 0x20000 + 10;
                    break;
		case KeyEvent.VK_BACK_SPACE:
                    actualKey = 8;
                    break;
                default:
                    actualKey = 0;
                    break;
                }
            }

	    if (keyCode == KeyEvent.VK_ESCAPE) {
		jsc.toolBar.setNormal();
		actualKey = 0; // don't pass this to the server
	    }

	    if (DEBUG >= 2) {
                System.out.println("actualKey = " + actualKey);
	    }

	    return actualKey;
	}

	// Determine whether the key is a "special" key that should be
	// allowed even if other key actions are disabled.  (See
	// ViewGraph::HandleKey().)
	private boolean isSpecialKey(int actualKey)
	{
	    boolean result = false;

	    switch (actualKey) {
	    case 'c':
	    case 'C':
	    case '\b':
	        result = true;
		break;
	    }

	    return result;
	}
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
		  "DEViseCanvas(" + view.viewName +
		    ").ViewMouseListener.mousePressed()");
            }

	    // Bug fix -- see notes at variable declaration.
            buttonIsDown = true;

            // The starting point will be in this view, otherwise this event
	    // will not be caught.

            // Each mouse click will be here once, so double click actually
	    // will enter this twice.

            // The position of this event will be relative to this view, and
	    // will not exceed the range of this view, ie, p.x >= 0 &&
	    // p.x < view.width ...

            Point p = event.getPoint();
            ep.x = op.x = sp.x = p.x;
            ep.y = op.y = sp.y = p.y;

            doMousePressed();
        }

        public void mouseReleased(MouseEvent event)
        {
	    if (DEBUG >= 1) {
	        System.out.println(
		  "DEViseCanvas(" + view.viewName +
		    ").ViewMouseListener.mouseReleased()");
	    }

	    // Bug fix -- see notes at variable declaration.
	    buttonIsDown = false;

	    _lastRBValid = false;

            _paintRubberbandOnly = false;

            // Each mouse click will be here once, so double click actually
	    // will enter this twice. Also, this event will always reported
	    // with each mouse click and before the mouseClick event is
	    // reported.

            jsc.jsValues.canvas.isInteractive = false;

	    doMouseReleased(event);
        }

        public void mouseClicked(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
		  "DEViseCanvas(" + view.viewName +
		    ").ViewMouseListener.mouseClicked()");
            }

	    doMouseClicked(event);
        }

        public void mouseEntered(MouseEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("Mouse entered view " + view.curlyName);
	    }
	    _mouseIsInCanvas = true;
	}

        public void mouseExited(MouseEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("Mouse exited view " + view.curlyName);
	    }
	    _mouseIsInCanvas = false;
	}
    }
    // end of class ViewMouseListener

    // start of class ViewMouseMotionListener
    class ViewMouseMotionListener extends MouseMotionAdapter
    {
        // When you drag a mouse, all the following mouse positions are
	// relative to the view where you first start the dragging (so we
	// might have value that is less than zero), and the dragging event
	// is still send back to the view where you first start dragging.

        public void mouseDragged(MouseEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseCanvas(" + view.viewName +
		  ").ViewMouseMotionListener.mouseDragged()");
            }

            Point p = event.getPoint();
            isMouseDragged = true;

	    doMouseDragged(p);
        }

        public void mouseMoved(MouseEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseCanvas(" + view.viewName +
		  ").ViewMouseMotionListener.mouseMoved()");
            }

	    // Bug fix -- see notes at variable declaration.
	    if (buttonIsDown) {
	        mouseDragged(event);
		return;
	    }

	    isMouseDragged = false;

            // the position of this event will be relative to this view
	    // and will not exceed the range of this view, ie, p.x >= 0 &&
	    // p.x < view.width, p.y >= 0 && p.y < view.height

            Point p = event.getPoint();

            doMouseMoved(p);
        }
    }
    // end of class ViewMouseMotionListener

    //
    // Note: we don't absolutely need this class; it's here just to help
    // debug the problem with input focus sometimes improperly getting
    // taken away from a DEViseCanvas.
    //
    class ViewFocusListener extends FocusAdapter
    {
        public void focusGained(FocusEvent event)
	{
	    if (DEBUG >= 2) {
	        System.out.println("View " + view.curlyName + " gained focus");
	    }
	}

	public void focusLost(FocusEvent event)
	{
	    if (DEBUG >= 2) {
	        System.out.println("View " + view.curlyName + " lost focus");
	        System.out.println("_mouseIsInCanvas = " + _mouseIsInCanvas);
	    }

	    if (_mouseIsInCanvas) {
	        if (DEBUG >= 1) {
	            System.err.println("WARNING: view " + view.viewName +
		      " lost focus while mouse is in it!");
                }
	    }
	}
    } // end of class ViewFocusListener

    // Update the shape of the mouse cursor based on whether the mouse
    // is on a DEVise cursor, etc.
    public synchronized void checkMousePos(Point mouseLoc,
      boolean checkDispatcher) throws YError
    {
        if (DEBUG >= 3) {
            System.out.println("DEViseCanvas(" + view.viewName +
	      ").checkMousePos(" + mouseLoc.x + ", " + mouseLoc.y + ")");
        }

        // initialize value
        whichCursorSide = DEViseCursor.sideNone;
        selectedCursor = null;
        isInViewDataArea = false;
        activeView = null;

        if (checkMousePos(mouseLoc, view)) { // activeView will not be null

	    // Set mouse cursor according to the toolbar state, whether
	    // we're on a DEVise cursor, etc.
            if (checkDispatcher && jsc.dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                // dispatcher still busy
		jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.waitCursor, this);

            } else if (!isInViewDataArea) {
                // inside the view but not within the data area of that view
		jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.defaultCursor, this);

	    } else if (selectedCursor != null && jsc.toolBar.doCursorOps()) {
		// on a DEVise cursor -- set mouse cursor according to
		// where we are on the DEVise cursor
	        setMouseCursorByDEViseCursor();

	    } else if (selectedCursor == null && jsc.toolBar.doNormal() &&
	      activeView.isRubberBand) {
		// we can draw a rubberband line to zoom
                jsc.mouseCursor.setTemporaryCursor(
                  jsc.mouseCursor.rbCursor, this);

	    } else if (jsc.toolBar.doZoomMode() && !activeView.isRubberBand) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doCursorFill() && !activeView.isCursorMove) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doDrillDown() && !activeView.isDrillDown) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doHome() && !activeView.isKey) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doToggleFilter() && !activeView.isKey) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doIncreaseSymSize() && !activeView.isKey) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doDecreaseSymSize() && !activeView.isKey) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doNormal() &&
	      (!activeView.isCursorMove || view.getFirstCursor() == null) &&
	      !activeView.isRubberBand) {
	        jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.disabledCursor, this);

            } else {
	        jsc.mouseCursor.setToPermanentCursor(this);
	    }


            if (activeView != null && activeView.pileBaseView != null) {
                activeView = activeView.pileBaseView;
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            // show the data at current mouse position

	    if (activeView != null && jsc.viewInfo != null) {

	        // Added check here for mouseLoc being null, because sometimes
	        // (infrequently) we get null pointers in the line calling
	        // jsc.viewInfo.updateInfo below.  The problem is, I have never
	        // been able to reproduce it reliably, and I can't *really*
	        // understand how we could get here with mouseLoc equal to null.
	        // But I can't see what else could be the proble, either.
	        // wenger, 2003-04-01.  *Not* April Fool's!!

		// Note: the extra checks still didn't fix the problem.
		// Could it be that another thread occasionally sets something
		// to null between the check and the updateInfo call below?
		// wenger, 2003-04-25.
		if (mouseLoc != null && isInViewDataArea) {
		    jsc.viewInfo.updateInfo(activeView.viewName,
		      activeView.getX(mouseLoc.x), activeView.getY(mouseLoc.y));
		} else {
		    jsc.viewInfo.updateInfo();
		}
	    }

        } else { // activeView is null and all other values will be initialized value before
	    // I don't think we ever get here...  wenger 2007-08-03.
            whichCursorSide = DEViseCursor.sideNone;
            selectedCursor = null;
            isInViewDataArea = false;
            activeView = null;

	    Cursor tmpCursor = null;
            if (!checkDispatcher || jsc.dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
		jsc.mouseCursor.setTemporaryCursor(
		  jsc.mouseCursor.waitCursor, this);
            } else {
		jsc.mouseCursor.setToPermanentCursor(this);
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            jsc.viewInfo.updateInfo();
        }
    }

    private void setMouseCursorByDEViseCursor()
    {
        switch (whichCursorSide) {
        case DEViseCursor.sideNone:
            // Not on a cursor
            jsc.mouseCursor.setTemporaryCursor(
              jsc.mouseCursor.rbCursor, this);
            break;

         case DEViseCursor.sideMiddle:
            // Inside a cursor, you can move this cursor.
            if (DEBUG >= 3) System.out.println(
              "whichCursorSide = DEViseCursor.sideMiddle");
            jsc.mouseCursor.setTemporaryCursor(
            jsc.mouseCursor.moveCursor, this);
            break;

        case DEViseCursor.sideLeft:
            // On left side of a cursor, you can resize this cursor.
           if (DEBUG >= 3) System.out.println(
             "whichCursorSide = DEViseCursor.sideLeft");
           jsc.mouseCursor.setTemporaryCursor(
             jsc.mouseCursor.lrsCursor, this);
           break;

        case DEViseCursor.sideRight:
            // On right side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
              "whichCursorSide = DEViseCursor.sideRight");
            jsc.mouseCursor.setTemporaryCursor(
              jsc.mouseCursor.rrsCursor, this);
           break;

        case DEViseCursor.sideTop:
            // On top side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
              "whichCursorSide = DEViseCursor.sideTop");
            jsc.mouseCursor.setTemporaryCursor(
              jsc.mouseCursor.trsCursor, this);
           break;

        case DEViseCursor.sideBottom:
            // On bottom side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
              "whichCursorSide = DEViseCursor.sideBottom");
            jsc.mouseCursor.setTemporaryCursor(
              jsc.mouseCursor.brsCursor, this);
           break;

        case DEViseCursor.sideTopLeft:
            // On left-top corner of a cursor, you can resize
            // this cursor.
            if (DEBUG >= 3) System.out.println(
              "whichCursorSide = DEViseCursor.sideTopLeft");
            jsc.mouseCursor.setTemporaryCursor(
              jsc.mouseCursor.tlrsCursor, this);
           break;

        case DEViseCursor.sideBottomLeft:
            // On left-bottom corner of a cursor, you can resize
            // this cursor.
           if (DEBUG >= 3) System.out.println(
             "whichCursorSide = DEViseCursor.sideBottomLeft");
           jsc.mouseCursor.setTemporaryCursor(
             jsc.mouseCursor.blrsCursor, this);
           break;

        case DEViseCursor.sideTopRight:
           // On right-top corner of a cursor, you can resize
           // this cursor.
           if (DEBUG >= 3) System.out.println(
             "whichCursorSide = DEViseCursor.sideTopRight");
           jsc.mouseCursor.setTemporaryCursor(
             jsc.mouseCursor.trrsCursor, this);
           break;

        case DEViseCursor.sideBottomRight:
           // 0n right-bottom corner of a cursor, you can resize
           // this cursor.
           if (DEBUG >= 3) System.out.println(
             "whichCursorSide = DEViseCursor.sideBottomRight");
           jsc.mouseCursor.setTemporaryCursor(
             jsc.mouseCursor.brrsCursor, this);
           break;

        default:
           throw new YError("Illegal whichCursorSide value: " +
             whichCursorSide);
        }
    }

    // Returns some kind of information about p in terms of what it's
    // on or in.  However, the logic is so complicated I haven't figured out
    // any easy way to summarize it.  
    // It also has side effects of setting activeView, selectedCursor,
    // whichCursorSide, and isInViewDataArea.
    // RKW 2000-05-12.
    //TEMP -- rename this??
    public synchronized boolean checkMousePos(Point p, DEViseView v)
    {
        if (!v.inView(p)) {
            return false;
        }

        if (!v.inViewDataArea(p)) {
            if (v.pileBaseView != null) {
		// v is a non-base view in a pile.
                return false;
            } else {
                activeView = v;
                selectedCursor = null;
                whichCursorSide = DEViseCursor.sideNone;
                isInViewDataArea = false;
                return true;
            }
        }

        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);
            if (checkMousePos(p, vv)) {
                return true;
            }
        }

	if (v.isCursorMove && jsc.toolBar.doCursorOps()) {
            for (int i = 0; i < v.viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)v.viewCursors.elementAt(i);
                int cursorSide = cursor.inCursor(p);
	        if (cursorSide != DEViseCursor.sideNone) {
                    activeView = v;
                    whichCursorSide = cursorSide;
                    if (DEBUG >= 3) System.out.println(
		      "Setting selected cursor to " + cursor.name);
                    selectedCursor = cursor;
                    isInViewDataArea = true;
                    return true;
                }
            }
	}

        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            if (checkMousePos(p, vv)) {
                return true;
            }
        }

        if (v.pileBaseView != null) {
	    // v is a non-base view in a pile.
            return false;
        } else {
            activeView = v;
            isInViewDataArea = true;
            selectedCursor = null;
            whichCursorSide = DEViseCursor.sideNone;
            return true;
        }
    }

    public void create3DViewer()
    {
        return;
    }
    
    //TEMP -- document this
    public void clear3DViewer()
    {
    }

    // 3D drill-down
    public void drillDown3D(Point p)
    {
    }

    private static String fitStringToWidth(String str, int width,
      FontMetrics fm)
    {

	final String suffix = "...";
	final int suffixLen = suffix.length();
	int strWidth;
	while ((strWidth = fm.stringWidth(str)) > width &&
	  str.length() > suffixLen) {
	    float factor = (float)width / (float)strWidth;

	    int newLen = (int)(factor * str.length());
	    newLen = Math.min(newLen, str.length() - suffixLen - 1);
	    newLen = Math.max(newLen, 0);

	    str = str.substring(0, newLen) + suffix;
	}

	return str;
    }
}

// this class is used to create XOR of part of image
class XORFilter extends RGBImageFilter
{
    Color color = Color.white;
    int red, green, blue;
    int _width, _height;
    int _drawType;

    public static final int typeFilledRect = 1;
    public static final int typeFrame = 2;
    public static final int typeFrameAndDots = 3;

    public XORFilter(Color c, int width, int height, int drawType)
    {
        if (c != null) {
            color = c;
        }

        red = color.getRed() << 16;
        green = color.getGreen() << 8;
        blue = color.getBlue();

	_width = width;
	_height = height;

        //System.out.println("r = " + red + " g = " + green + " b = " + blue);

	if (drawType == typeFilledRect) {
            canFilterIndexColorModel = true;
	}
	_drawType = drawType;
    }

    public int filterRGB(int x, int y, int rgb) throws YError
    {
	int rgbOut = rgb;
	if (_drawType == typeFilledRect) {
            rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
	      ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
	} else if (_drawType == typeFrame) {
	    if (x < 3 || y < 3 || x > _width - 4 || y > _height - 4) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
	          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
	    }
	} else if (_drawType == typeFrameAndDots) {
	    if (x < 3 || y < 3 || x > _width - 4 || y > _height - 4) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
	          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
	    } else if ((x % 6 == 0) && (y % 6 == 0)) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
	          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
	    }
	} else {
	    throw new YError("Illegal draw type (" + _drawType + ")");
	}

	return rgbOut;
    }
}
