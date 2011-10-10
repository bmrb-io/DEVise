// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides an interface between the DEViseCmdDispatcher
// class and the DEViseCanvas and DEViseView classes.  It also handles
// events that don't fall into a canvas.

// One of the most important functions of this class is to maintain
// consistency of the DEViseCanvas and DEViseView objects, because
// they have a number of cross-references.

// This class also has a number of features dealing with the fact
// that the DEViseCmdDispatcher and the drawing are done in different
// threads.  It could be made safer by making more members private
// and more methods syncronized (according to Hongyu).

// There is one instance of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.90  2011/08/26 15:37:34  wenger
// Merged js_button_fix_br_0 thru js_button_fix_br_1 to trunk.
//
// Revision 1.89.2.13  2011/10/10 19:27:03  wenger
// More cleanup of session-specific menus: we now properly get rid of
// any session-specific menu items when we close a session.
//
// Revision 1.89.2.12  2011/10/05 23:44:51  wenger
// Early version of "session-specific" menu working -- only works for
// showing URLs at this point.
//
// Revision 1.89.2.11  2011/10/05 17:48:07  wenger
// Okay, removed the debug code.
//
// Revision 1.89.2.10  2011/10/05 17:40:28  wenger
// Lots of extra debug output to try to figure out the button/Jmol lockup
// problem on Macs.  No luck, though, I'm going to take a different
// approach...
//
// Revision 1.89.2.9  2011/10/04 17:00:28  wenger
// This amount of debug output makes things work on the Union South kiosks,
// if you have the log window showing...
//
// Revision 1.89.2.8  2011/09/29 17:55:17  wenger
// We now actually create embedded buttons in DEViseScreen instead of
// in DEViseGData, so that they're created by the event queue thread instead
// of a command thread (this didn't actually solve the Mac Jmol/button
// lockup problem, but it seems safer).  Also changed the DEViseCanvas
// class to extend JComponent rather than Container, in case this works
// better for adding JButtons to it.
//
// Revision 1.89.2.7  2011/08/25 21:35:53  wenger
// Hopefully final cleanup of the JavaScreen embedded button fixes.
//
// Revision 1.89.2.6  2011/08/15 18:51:18  wenger
// Fixed a problem with "obsolete" buttons not getting removed correctly;
// cleaned up a few other things.
//
// Revision 1.89.2.5  2011/06/08 21:19:35  wenger
// We no longer change the mouse cursor to the disabled cursor in a
// view with a button in "standard" toolbar mode.
//
// Revision 1.89.2.4  2011/06/07 18:00:58  wenger
// More cleanup of no-longer-used code (including some test code I added
// temporarily).
//
// Revision 1.89.2.3  2011/06/06 21:01:09  wenger
// Okay, I think I pretty much have things working -- done by adding the
// buttons to the appropriate *DEViseCanvas*, rather than directly to
// the DEViseScreen -- keeps the buttons on top.
//
// Revision 1.89.2.2  2011/06/03 23:21:28  wenger
// Cleaned up some of the junk that was just commented out previously.
//
// Revision 1.89.2.1  2011/06/03 23:10:51  wenger
// Working on getting embedded buttons in the JS working again -- big
// change so far is getting rid of the paint() method in DEViseScreen
// -- I think it was an error that that ever existed.  Lots of test/debug
// code in place right now as I play around with getting buttons to work.
//
// Revision 1.89  2011/04/21 15:34:17  wenger
// Merged js_highlight_thread_fix_br_0 thru js_highlight_thread_fix_br_1
// to trunk.
//
// Revision 1.88.2.2  2011/04/20 20:12:41  wenger
// Removed debug code.
//
// Revision 1.88.2.1  2011/04/20 17:15:07  wenger
// Changed the DEViseGenericTree.setSelection() method and the YLogGUI
// p() and pn() methods to only actually update the GUI in the event
// dispatched thread, to hopefully cure problems with incorrect 3D
// highlight updating in the s2pred visualization, and null pointer
// problems when showing the log window.  (I actually meant to do some
// earlier commits to this branch with more of the debug code in place,
// but I forgot to do that.)
//
// Revision 1.88  2011/02/13 23:56:24  wenger
// Merged bug_1005_br_0 thru bug_1005_br_1 to trunk.
//
// Revision 1.87.4.11  2011/02/13 22:42:19  wenger
// Pretty much finished cleaning up the Jmol state preservation code.
//
// Revision 1.87.4.10  2011/02/13 02:43:00  wenger
// Got rid of debug output.
//
// Revision 1.87.4.9  2011/02/13 01:48:41  wenger
// The 'saving Jmol state on resizes' feature is now working at home
// -- the fix was putting the stuff that actually restores the Jmol
// state into the event dispatched thread.  (Lots of debug code still
// in place.)
//
// Revision 1.87.4.8  2011/02/09 18:31:44  wenger
// Okay, more test changes -- sleep in DEViseCanvas3DJmol.restoreJmoState()
// seems to fix things, but is not what I want for a "real" fix.
//
// Revision 1.87.4.7  2011/02/09 16:18:01  wenger
// Committing test changes, including sleep in
// DEViseCmdDispatcher.waitForCmds(), that seems to fix the problem with the Jmol restore state code hanging the JS on my laptop.
//
// Revision 1.87.4.6  2011/02/04 23:25:48  wenger
// Saving Jmol state: okay, trying to re-use the canvas didn't seem to
// work right (it didn't want to have the right size and location);
// got rid of temporary debug code.  Hmm -- maybe I can save the selection
// tree objects with the Jmol state...
//
// Revision 1.87.4.5  2011/02/04 22:39:47  wenger
// Saving Jmol state: this is pretty much working; leaving in a bunch
// of debug code and temporary comments for now (although I'm wondering
// if we could avoid destroying and re-creating the selection trees
// by saving and re-using the entire DEViseCanvas3DJmol object...).
//
// Revision 1.87.4.4  2011/01/21 22:42:50  wenger
// Cleanups (with no change in functionality) to DEViseSCreen: consolidated
// allViews into viewTable, and removed some other variables that weren't
// really used for anything; other minor code cleanups.
//
// Revision 1.87.4.3  2011/01/21 22:10:23  wenger
// Removed javaGDatas from DEViseScreen because we never used it for
// anything; added some more debug output and comments to DEViseScreen;
// some temporary debug output for working on bug 1007.
//
// Revision 1.87.4.2  2011/01/21 19:37:50  wenger
// Cleaned up some of the temporary code.
//
// Revision 1.87.4.1  2011/01/21 19:18:37  wenger
// Initial part of fix of bug 1005 (Jmol loses state on resize) -- it's
// basically working, but needs cleanup because it relies on static
// variables in the DEViseCanvas3DJmol class, etc.; also, there's still
// some test code in place.
//
// Revision 1.87  2010/07/09 23:46:27  wenger
// Changed the JavaScreen client to do the session re-opening for
// a resize in separate thread, so that the GUI isn't locked up while
// the re-opening is happening.
//
// Revision 1.86  2010/07/01 17:32:59  wenger
// Implemented JavaScreen to-do 09.019 -- JS window can now be re-sized
// while a session is open, added new view menu options to enlarge and
// reduce by a fixed amount.
//
// Revision 1.85  2010/06/02 17:41:07  wenger
// The JavaScreen now puts the BMRB visualization type into the JSA
// window title if the hidebmrbsess parameter is turned on.
//
// Revision 1.84  2008/07/17 20:28:00  wenger
// (Mostly) fixed bug 968 (JavaScreen doesn't correctly handle cursors
// that are entirely outside the destination view's visual filter).
//
// Revision 1.83  2007/09/10 22:10:36  wenger
// Mouse cursor now changes to the disabled cursor if the selected
// toolbar mode is not applicable in the current view; mouse cursor
// is default on non-view areas; changed Jmol views to not show the
// non-applicable mouse cursors (this only happened in some JVMs).
//
// Revision 1.82  2007/08/27 20:53:46  wenger
// Made separate menu items for showing and hiding view help, as requested
// by Chris Schulte.
//
// Revision 1.81  2007/08/03 20:17:27  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.80  2007/06/27 21:28:21  wenger
// Fixed the "obscured tooltips" problem by changing the DEViseScreen
// to extend a JPanel rather than a Panel; made other changes to
// DEViseScreen to (at least apparently) fix the resulting deadlock
// problems (still needs more testing).
//
// Revision 1.79  2007/06/27 17:47:59  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.78  2007/04/20 19:42:35  wenger
// Merged andyd_gui_br_2 thru andyd_gui_br_5 to the trunk.
// merged-andyd_gui_br_2-thru-andyd_gui_br_5-to-trunk
//
// Revision 1.77  2007/03/30 15:43:09  wenger
// (Hopefully) cured the lockups we've been seeing with JS 5.8.0 (removed
// a bunch of calls to validate() in the GUI); fixed up the client logging
// functionality somewhat; various improvements to debug output.
//
// Revision 1.76.2.4  2007/08/03 19:21:16  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.76.2.3  2007/07/25 18:25:17  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.76.2.2  2007/06/21 14:41:16  wenger
// Tried changing the DEViseScreen to extend a JPanel instead of a Panel
// (this fixed the problem with toolbar tooltips being obscured, but
// caused the JS to lock up really a lot, so I have temporarily changed
// it back); changed some of the tooltip text; put code into
// DEViseToolBar.java to make sure graphics code is getting called from
// the AWT Event thread.
//
// Revision 1.76.2.1  2007/04/19 21:16:08  wenger
// Fixed the problem with component layout in the jsb; got rid of
// jsdevisec screenPanel, since it caused problem with the fix and only
// was there for color; added the sbgcolor applet parameter to set
// the "screen background" color, since this is now more prominent.
//
// Revision 1.76  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.75.6.8  2006/05/12 14:50:26  wenger
// Now have two trees for a Jmol view: the first one selects which
// atoms are shown in the Jmol view and in the second tree; the
// second tree selects which atoms are highlighted in the Jmol view.
//
// Revision 1.75.6.7  2006/03/30 20:51:26  wenger
// Partially done splitting up the DEViseCanvas class.
//
// Revision 1.75.6.6  2006/02/28 22:31:47  wenger
// Implemented highlighting in Jmol views.
//
// Revision 1.75.6.5  2006/02/23 22:08:40  wenger
// Added flag for whether or not 3D views should use Jmol.
//
// Revision 1.75.6.4  2005/12/29 21:19:20  wenger
// Improved Jmol integration into the JavaScreen -- the Jmol
// viewer isn't constantly destroyed and constructed; also a
// bunch of other improvements in the Jmol-related DEViseCanvas
// code.
//
// Revision 1.75.6.3  2005/12/15 19:28:09  wenger
// JmolPanel is now sized to match 3D DEViseCanvas; removed
// Jmol stuff from DEViseScreen and jsdevisec.
//
// Revision 1.75.6.2  2005/12/15 19:04:06  wenger
// Jmol is now in the DEViseScreen and DEViseCanvas (test
// version only).
//
// Revision 1.75.6.1  2005/12/09 20:58:53  wenger
// Got Jmol to show up in the JavaScreen! (not yet connected to a
// visualization); added a bunch of debug code to help understand
// things for Jmol.
//
// Revision 1.75  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.74  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.73  2002/05/01 21:28:59  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.72.2.4  2002/08/20 14:43:18  sjlong
// Fixed "help" bugs 609 and 621.
//
// Revision 1.72.2.3  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.72.2.2  2002/04/24 20:06:37  xuk
// Fixed bug 777: avoid long waiting time for repainting in DEViseCanvas
// Handle double-buffering in paint();
// Added paintCanvas() method for painting.
//
// Revision 1.72.2.1  2002/04/03 17:39:23  xuk
// Fixed bug 766: Hide view help for collaboration followers.
// Added method hideAllHelp().
//
// Revision 1.72  2002/02/06 23:10:34  xuk
// Draw the axis labels in the JavaScreen.
// Changed function updateViewDataRange().
//
// Revision 1.71  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.70  2001/05/11 20:36:08  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.69  2001/05/03 16:25:56  xuk
// Added multiply factor for displaying mouse postion.
// Changed updateViewDataRange() function;
// Added factor argument when calling updateInfo().
//
// Revision 1.68  2001/04/01 03:51:17  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.67  2001/03/25 20:39:29  xuk
// Added collab3DView() method to process 3D view collaboration commands.
//
// Revision 1.66  2001/03/20 17:49:46  xuk
// Added collaboration for 3D Views.
//
// Revision 1.65  2001/01/08 20:31:52  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.63.4.5  2000/11/21 01:51:33  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.63.4.4  2000/10/18 20:28:11  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.64  2000/10/03 19:23:27  wenger
// Fixed bugs 592 and 594 (JavaScreen sometimes missing keystrokes in views).
//
// Revision 1.63.4.3  2000/10/18 18:29:23  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.63.4.2  2000/10/11 02:59:48  xuk
// Add notifyall() to wake up dispatcher thread.
//
// Revision 1.63.4.1  2000/10/09 16:32:22  xuk
// Change dispathcer.start() to dispatcher.command = cmd, since DEViseCmdDispatcher thread starts only once now.
//
// Revision 1.63  2000/07/20 16:26:07  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.62  2000/06/21 18:37:30  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.61  2000/06/21 18:10:15  wenger
// Changes to 3D requested by BMRB: removed axes; up/down mouse movement
// does zooming; molecule doesn't move when changing atoms; 'r' resets
// location and zoom as well as rotation.
//
// Revision 1.60  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.59  2000/05/25 14:47:39  wenger
// 3D coordinate system remains unchanged when new GData arrives; 'r' or 'R'
// in view resets to default coordinates.
//
// Revision 1.58  2000/05/22 17:52:49  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.57  2000/05/12 20:43:56  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
// Revision 1.56  2000/05/11 20:59:49  wenger
// More comments.
//
// Revision 1.55  2000/05/11 20:19:33  wenger
// Cleaned up jsdevisec.java and added comments; eliminated
// jsdevisec.lastCursor (not really needed).
//
// Revision 1.54  2000/05/04 15:53:31  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.53  2000/04/27 20:15:25  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.52  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.51  2000/04/07 22:43:14  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.50  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.49  2000/03/31 19:29:16  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.48  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.47  2000/02/23 21:12:13  hongyu
// *** empty log message ***
//
// Revision 1.46  2000/02/23 10:44:02  hongyu
// *** empty log message ***
//
// Revision 1.45  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.44  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.43  1999/12/07 23:18:21  hongyu
// *** empty log message ***
//
// Revision 1.42  1999/11/03 08:00:49  hongyu
// *** empty log message ***
//
// Revision 1.41  1999/11/01 17:40:16  hongyu
// *** empty log message ***
//
// Revision 1.40  1999/10/14 15:09:05  hongyu
// *** empty log message ***
//
// Revision 1.39  1999/10/12 21:58:14  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/10/12 00:08:43  hongyu
// *** empty log message ***
//
// Revision 1.37  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.36  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.35  1999/08/04 06:05:54  hongyu
// *** empty log message ***
//
// Revision 1.34  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.33  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.31  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseScreen.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.*;
import javax.swing.*;

public class DEViseScreen extends JPanel
{
    jsdevisec jsc = null;

    Dimension screenDim = new Dimension(600, 400);

    // Hongyu says that having these three vectors for canvases is related
    // to making things thread-safe.  I guess it's so we only actually
    // add and remove objects to/from the screen in the paint() method,
    // which should be called in the GUI thread.
    Vector allCanvas = new Vector();
    Vector newCanvas = new Vector();
    Vector obsoleteCanvas = new Vector();

    // A list of all of the DEViseView objects we have, indexed by the
    // view name.
    Hashtable viewTable = new Hashtable();

    // This is for dealing with GData symbols that are rendered as
    // actual Java GUI objects (e.g., buttons).
    Vector newGData = new Vector();
    Vector obsoleteGData = new Vector();

    // Note: this must *not* get reset when we open or close a session,
    // because we're preserving Jmol state across some session opens/closes.
    Hashtable jmolStates = null;

    private DEViseView currentView = null; // the view the mouse is in
    public DEViseView lastActionView = null;
    public boolean guiAction = false;

    private static final int DEBUG = 0; // 0 - 3

    boolean isDimChanged = false;

    public Image offScrImg = null;

    public Point finalMousePosition = new Point(-1, -1);

    //-------------------------------------------------------------------
    // Constructor.
    public DEViseScreen(jsdevisec what)
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.DEViseScreen()");
        }

        jsc = what;

        screenDim = new Dimension(jsc.jsValues.uiglobals.screenSize.width,
	  jsc.jsValues.uiglobals.screenSize.height);

        setLayout(null);
        setBackground(jsc.jsValues.uiglobals.screenBg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);

        addMouseMotionListener(new MouseMotionAdapter()
            {
                public void mouseMoved(MouseEvent event)
                {
                    Point p = event.getPoint();

                    if (jsc.dispatcher.getStatus() ==
		      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                        setTemporaryCursor(jsc.mouseCursor.waitCursor);
                    } else {
                        setTemporaryCursor(jsc.mouseCursor.defaultCursor);
                    }

                    if (currentView != null) {
                        jsc.viewInfo.updateInfo();

                        setCurrentView(null);
                    }
                }
            });
        addFocusListener(new DSFocusListener());
	addKeyListener(new DSKeyListener());
    }

    //-------------------------------------------------------------------
    protected void setToPermanentCursor()
    {
        jsc.mouseCursor.setToPermanentCursor(this);
    }

    //-------------------------------------------------------------------
    protected void setTemporaryCursor(Cursor cursor)
    {
        jsc.mouseCursor.setTemporaryCursor(cursor, this);
    }

    //-------------------------------------------------------------------
    protected void processMouseEvent(MouseEvent event)
    {
        if (event.getID() == MouseEvent.MOUSE_EXITED) {
            finalMousePosition.x = -1;
            finalMousePosition.y = -1;
        } else {
            finalMousePosition.x = event.getX();
            finalMousePosition.y = event.getY();
        }

        super.processMouseEvent(event);
    }

    //-------------------------------------------------------------------
    // Hongyu says that we have both this method and a mouse motion
    // listener because this method gets both move and drag events,
    // which we want in order to update the mouse location display.
    protected void processMouseMotionEvent(MouseEvent event)
    {
        finalMousePosition.x = event.getX();
        finalMousePosition.y = event.getY();

        super.processMouseMotionEvent(event);
    }

    //-------------------------------------------------------------------
    //TEMP -- what does this actually do?  Disabling it doesn't seem
    // to make any obvious difference...  wenger 2007-06-25
    public void reEvaluateMousePosition()
    {
    	Runnable doReEvaluateMosePosition = new DoReEvaluateMousePosition();
	SwingUtilities.invokeLater(doReEvaluateMosePosition);
    }

    //-------------------------------------------------------------------
    // Hongyu says that this code has something to do with fixing a
    // case where Java does things wrong.
    private class DoReEvaluateMousePosition implements Runnable {
    	public void run() {
            if (finalMousePosition.x < 0) {
                if (currentView != null) {
                    jsc.viewInfo.updateInfo();
                    setCurrentView(null);
                }

                return;
            }

            //if (!DEViseUIGlobals.isApplet) {
            //    MouseEvent finishEvent = new MouseEvent(this, MouseEvent.MOUSE_MOVED, DEViseGlobals.getCurrentTime(), 0, finalMousePosition.x, finalMousePosition.y, 0, false);
            //    Toolkit tk = getToolkit();
            //    //EventQueue queue = new EventQueue();
            //    EventQueue queue = tk.getSystemEventQueue();
            //    queue.postEvent(finishEvent);
            //} else {
                for (int i = 0; i < allCanvas.size(); i++) {
                    DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(i);
                    Rectangle loc = canvas.getLocInScreen();
                    Point p = new Point(finalMousePosition.x - loc.x,
                    finalMousePosition.y - loc.y);
                    if (p.x >= 0 && p.x <= loc.width && p.y >= 0 &&
                        p.y <= loc.height) {
                        canvas.checkMousePos(p, false);
                        repaint();
                        return;
                    }
                }

                if (currentView != null) {
                    jsc.viewInfo.updateInfo();
                    setCurrentView(null);
                }

	        setToPermanentCursor();
            //}
        }
    }

    //-------------------------------------------------------------------
    public Dimension getPreferredSize()
    {
        return screenDim;
    }

    //-------------------------------------------------------------------
    public Dimension getMinimumSize()
    {
	// Give the layout manager some room to work!!
	int width = Math.max(100, screenDim.width - 200);
	int height = Math.max(100, screenDim.height - 200);
	Dimension minSize = new Dimension (width, height);
	return minSize;
    }

    //-------------------------------------------------------------------
    // At this time, you can not change the size of the screen after a session
    // is opened. If you really want to change the screen size, you will need
    // to close the session, re-set the size of the screen and reopen the
    // session
    public void setScreenDim(int width, int height)
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.setScreenDim(" + width +
	      ", " + height + ")");
        }

        if (width < 1 || height < 1) {
            return;
        }

	if (screenDim.width == width && screenDim.height == height) {
            return;
	}

        screenDim.width = width;
        screenDim.height = height;

        jsc.jsValues.uiglobals.screenSize.width = screenDim.width;
        jsc.jsValues.uiglobals.screenSize.height = screenDim.height;

        isDimChanged = true;

	if (jsc.isSessionOpened) {
	    // We do this stuff in a new thread so that the stop button
	    // shows up red immediately, and the GUI is responsive.
	    Thread thread = new Thread(new SessionReopen());
	    thread.start();
	}

        repaint();
    }

    //-------------------------------------------------------------------
    private class SessionReopen implements Runnable
    {
        public void run()
        {
	    if (DEBUG >= 1) {
                System.out.println("DEViseScreen.SessionReopen.run()");
            }

	    saveJmolStates();

	    // Save and close the current session.
	    String command = new String();
	    command = DEViseCommands.SAVE_CUR_SESSION + "\n";
	    jsc.dispatcher.start(command);
	    jsc.dispatcher.waitForCmds();
	    jsc.closeSession();

	    // Send updated size, re-open the session.
	    jsc.dispatcher.waitForCmds();
	    command = DEViseCommands.SET_DISPLAY_SIZE + " " +
	      jsc.jsValues.uiglobals.screenSize.width + " " +
	      jsc.jsValues.uiglobals.screenSize.height + " " +
	      jsc.jsValues.uiglobals.screenRes + " " +
	      jsc.jsValues.uiglobals.screenRes + "\n";
	    command += DEViseCommands.REOPEN_SESSION + "\n";
	    jsc.dispatcher.start(command);
	    jsc.dispatcher.waitForCmds();

	    restoreJmolStates();
        }
    }

    //-------------------------------------------------------------------
    // Show help messages in all views.
    public synchronized void showAllHelp()
    {
	if (DEBUG >= 2) {
            System.out.println("DEViseScreen.showAllHelp()");
        }

        String cmd = "";
        for (int i = 0; i < allCanvas.size(); i++) {
            DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
            cmd = cmd + DEViseCommands.GET_VIEW_HELP + " " +
	      c.view.getCurlyName() + " " + 0 + " " + 0 + "\n";
	    // If this view has children, send a command to show their help as well.
	    for(int j = 0; j < c.view.viewChilds.size(); j++) {
	        DEViseView v = (DEViseView)c.view.viewChilds.elementAt(j);
	        cmd = cmd + DEViseCommands.GET_VIEW_HELP + " " +
		  v.getCurlyName() + " " + 0 + " " + 0 + "\n";
	    }
        }

        jsc.dispatcher.start(cmd);
    }

    //-------------------------------------------------------------------
    // Hide help messages in all views.
    public synchronized void hideAllHelp()
    {
	if (DEBUG >= 2) {
            System.out.println("DEViseScreen.hideAllHelp()");
        }

	for (int i = 0; i < allCanvas.size(); i++) {
	    DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
	    c.helpMsg = null;

	    for(int j = 0; j < c.childViewHelpMsgs.size(); j++) {
	        c.childViewHelpMsgs.setElementAt(null, j);
	    }
	}
	
	repaint();
    }

    //-------------------------------------------------------------------
    public void addView(DEViseView view)
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.addView(" +
	      view.viewName + ")");
        }

        if (view == null) {
            return;
        }

        // view already exists
        if (viewTable.get(view.viewName) != null) {
            return;
        }

        // parent view always created before view symbol
        if (view.parentView != null) {
            view.parentView.addChild(view);
        }

        // top view always created before bottom view
        if (view.pileBaseView != null) {
            view.pileBaseView.addPiledView(view);
        }

        viewTable.put(view.viewName, view);
    }

    //-------------------------------------------------------------------
    // Put an image into a view, creating the appropriate DEViseCanvas
    // object if necessary.
    // Only the base view (not view symbols, not non-base views in piles)
    // will have an image and only the base view will associated with a
    // canvas; a canvas will only be assiciated one view and one view only.
    public void updateViewImage(String name, Image image)
    {
        if (name == null || image == null) {
            return;
        }

	if (DEBUG >= 2) {
            System.out.println("DEViseScreen.updateViewImage(" +
	      name + ")");
        }

        DEViseView view = getView(name);

        if (view == null) {
            return;
        }

        if (view.canvas == null) {
	    DEViseCanvas canvas = null;
	    if (view.viewDimension == 2) {
            	canvas = new DEViseCanvas2D(view, image);
	    } else if (view.viewDimension == 3) {
	        if (view.getUseJmol()) {
            	    canvas = new DEViseCanvas3DJmol(view, image);
		} else {
            	    canvas = new DEViseCanvas3DPlain(view, image);
		}
	    } else {
		System.err.println("Illegal number of dimensions: " +
		  view.viewDimension);
	    }
            view.canvas = canvas;

            int i;
            for (i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                if (c.view.viewZ > canvas.view.viewZ) {
                    canvas.posZ = i;
                    break;
                }
            }
            if (i > 0 && canvas.posZ == 0) {
                canvas.posZ = allCanvas.size();
            }

            allCanvas.insertElementAt(canvas, canvas.posZ);

            newCanvas.addElement(canvas);
        } else {
            view.canvas.updateImage(image);
        }

	updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Remove the given view by name.
    //
    // When we remove a view, everything assiciated with this view is removed,
    // such as child views, cursors, gdata, and even the canvas and piled
    // views (if the view to be removed is a base view).
    public void removeView(String name)
    {
        if (name == null) {
            return;
        }

        removeView(getView(name));
    }

    //-------------------------------------------------------------------
    // Remove the given view by object reference.
    public synchronized void removeView(DEViseView view)
    {
        if (view == null) {
            return;
        }

	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.removeView(" +
	      view.viewName + ")");
        }

        Vector vector = null;

        // remove itself from its parent or piled view, if any
        if (view.pileBaseView != null) {
            view.pileBaseView.removePiledView(view);
        }

        if (view.parentView != null) {
            view.parentView.removeChild(view);
        }

        // remove GDatas associated with it
        vector = view.viewGDatas;
        if (vector.size() > 0) {
            for (int i = 0; i < vector.size(); i++) {
                DEViseGData data = (DEViseGData)vector.elementAt(i);
                if (data.symbol != null) {
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        // remove cursors associated with this view
        view.removeAllCursor();

        // remove any piled view might associated with this view
        vector = view.viewPiledViews;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0)); // recursive call to removeView
        }

        // remove any child view might associated with this view
        vector = view.viewChilds;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0)); // recursive call to removeView
        }

        // if this view is current view, set current view to be null
        if (currentView == view) {
            currentView = null;
        }

        // remove canvas associated with this view, if any
        if (view.canvas != null) {
            allCanvas.removeElement(view.canvas);
            obsoleteCanvas.addElement(view.canvas);
        }

        // remove this view from view table
        viewTable.remove(view.viewName);

	updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // remove the child views only
    public void removeChildViews(String viewName)
    {
        if (viewName == null) {
            return;
        }

        removeChildViews(getView(viewName));
    }

    //-------------------------------------------------------------------
    public void removeChildViews(DEViseView view)
    {
        if (view == null) {
            return;
        }

	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.removeChildViews(" +
	      view.viewName + ")");
        }

        Vector vector = view.viewChilds;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0));
        }

	// In this case, also remove any help messages that might currently be displayed by the
	// children by setting them to null
	if(view.canvas != null) {
	    for(int i = 0; i < view.canvas.childViewHelpMsgs.size(); i++) {
		view.canvas.childViewHelpMsgs.setElementAt(null, i);
	    }
	}
        // Can not use the for iteration, because removeView also remove that view
        // from its parents, the the size of the viewChilds of the parentView is
        // actually changing on the fly
        //for (int i = 0; i < child.size(); i++) {
        //    removeView(view.getChild(i));
        //}

	updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Translate view name into view object.
    public DEViseView getView(String name)
    {
        if (name == null) {
            return null;
        }

        return (DEViseView)viewTable.get(name);
    }

    //-------------------------------------------------------------------
    public DEViseView getCurrentView()
    {
        return currentView;
    }

    //-------------------------------------------------------------------
    public void setCurrentView(DEViseView view)
    {
	if (DEBUG >= 3) {
            System.out.println("DEViseScreen.setCurrentView(" +
	      view != null ? view.viewName : "null" + ")");
        }

        if (currentView != null) {
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().repaint();
                if (currentView != view && currentView.getCanvas().activeView == currentView) {
                    currentView.getCanvas().activeView = null;
                }
            }
        }

        currentView = view;

        if (!jsc.isShowingMsg() && currentView != null) {
            requestFocus();
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().requestFocus();
                currentView.getCanvas().repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    // Remove the given view's current GData and add the new GData;
    // force a repaint.
    public synchronized void updateViewGData(String viewName, Vector gdList)
    {
	if (DEBUG >= 2) {
            System.out.println("DEViseScreen.updateViewGData(" +
	      viewName + ")");
        }

        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        Vector viewGD = view.viewGDatas;
        if (viewGD.size() > 0) {
            for (int i = 0; i < viewGD.size(); i++) {
                DEViseGData data = (DEViseGData)viewGD.elementAt(i);
                if (data.symbol != null) {
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        if (gdList != null && gdList.size() > 0) {
            for (int i = 0; i < gdList.size(); i++) {
                DEViseGData g = (DEViseGData)gdList.elementAt(i);
                if (g.isJavaSymbol) {
                    newGData.addElement(g);
                }

                view.addGData(g);
            }
        }

	//TEMP -- this could really stand to get cleaned up
        if (view.viewDimension == 3 && view.canvas != null) {
	    DEVise3DLCS oldLcs = null;
	    int oldSX = 0, oldSY = 0, oldTSX = 0, oldTSY = 0;
	    if (view.canvas.crystal != null) {
	        oldLcs = view.canvas.crystal.lcs;
		oldSX = view.canvas.crystal.shiftedX;
		oldSY = view.canvas.crystal.shiftedY;
		oldTSX = view.canvas.crystal.totalShiftedX;
		oldTSY = view.canvas.crystal.totalShiftedY;
	    }
	    view.canvas.clear3DViewer();
	    view.canvas.create3DViewer();
	    if (oldLcs != null) {
	        view.canvas.crystal.lcs = oldLcs;
		view.canvas.crystal.shiftedX = oldSX;
		view.canvas.crystal.shiftedY = oldSY;
		view.canvas.crystal.totalShiftedX = oldTSX;
		view.canvas.crystal.totalShiftedY = oldTSY;
	    }
	} else if (view.pileBaseView != null &&
	  view.pileBaseView.viewDimension == 3 &&
	  view.pileBaseView.canvas != null) {
	    DEVise3DLCS oldLcs = null;
	    int oldSX = 0, oldSY = 0, oldTSX = 0, oldTSY = 0;
	    if (view.pileBaseView.canvas.crystal != null) {
	        oldLcs = view.pileBaseView.canvas.crystal.lcs;
		oldSX = view.pileBaseView.canvas.crystal.shiftedX;
		oldSY = view.pileBaseView.canvas.crystal.shiftedY;
		oldTSX = view.pileBaseView.canvas.crystal.totalShiftedX;
		oldTSY = view.pileBaseView.canvas.crystal.totalShiftedY;
	    }
            view.pileBaseView.canvas.create3DViewer();
	    if (oldLcs != null) {
	        view.pileBaseView.canvas.crystal.lcs = oldLcs;
		view.pileBaseView.canvas.crystal.shiftedX = oldSX;
		view.pileBaseView.canvas.crystal.shiftedY = oldSY;
		view.pileBaseView.canvas.crystal.totalShiftedX = oldTSX;
		view.pileBaseView.canvas.crystal.totalShiftedY = oldTSY;
	    }
        }

	updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Add the given cursor to the named view.
    public void updateCursor(String viewName, DEViseCursor cursor)
    {
	if (DEBUG >= 1) {
            System.out.println("DIAG DEViseScreen.updateCursor(" +
	      viewName + ", " + cursor.name + ")");
        }

        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        if (cursor != null) {
            if (view.addCursor(cursor)) {
	        updateObjects();
                repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    // Remove the named cursor from the named view.
    public void hideCursor(String cursorName, String viewname)
    {
        DEViseView view = getView(viewname);

        if (view != null) {
            if (view.hideCursor(cursorName)) {
	        updateObjects();
                repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    public void updateViewDataRange(String viewName, String axis,
	   float min, float max, String format, float factor, 
	   int label, int type, int size, int bold, int italic)
    {
        DEViseView view = getView(viewName);

        if (view != null && axis != null) {
            view.updateDataRange(axis, min, max, format, factor, 
				 label, type, size, bold, italic);
        }
    }

    //-------------------------------------------------------------------
    public synchronized void setLastAction(String cmd)
    {
        if (guiAction) {
            guiAction = false;
            if (currentView != null) {
                if (lastActionView != currentView) {
                    if (lastActionView != null) {
                        lastActionView.isFirstTime = true;
                    }

                    lastActionView = currentView;
                    if (currentView.isFirstTime) {
                        currentView.isFirstTime = false;
                    }
                }
            }
        }
    }

    //-------------------------------------------------------------------
    // Update the JS when a session is opened or closed.
    public synchronized void updateScreen(boolean opening)
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.updateScreen(" + opening + ")");
        }

        if (opening) {
            jsc.isSessionOpened = true;

	    if (jsc.parentFrame != null && jsc._currentVisType != null) {
	        jsc.parentFrame.setTitle(jsc._currentVisType + " (" +
		  DEViseUIGlobals.javaScreenTitle + ")");
	    }
        } else {
	    // Note: this section of code does the work of cleaning
	    // up when we close a session.  RKW 2000-03-31.

	    if (jsc.parentFrame != null) {
	        jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
	    }

	    // Remove the view's references to all of its GData records,
	    // so they hopefully get garbage collected. RKW 2000-03-31.
	    Enumeration views = viewTable.elements();
	    while (views.hasMoreElements()) {
	        DEViseView tmpView = (DEViseView)views.nextElement();
	        tmpView.removeAllGData();
	    }

	    // Call close() on all DEViseCanvases (needed to get rid of tree windows
	    // in Jmol canvases, at least for now).
	    for (int index = 0; index < allCanvas.size(); index++) {
	        DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(index);
		canvas.close();
	    }

	    // Get rid of any session-specific menu items we have.
	    jsc.hideSessionMenu();

	    // Note: I'm wondering if some of this stuff should only happen
	    // in the event thread, and the fact that it's happening in
	    // a command thread here might be causing us some problems.
	    // But when I moved it to the event thread using
	    // SwingUtilities.invokeLater, things didn't work -- it seemed
	    // like some stuff here was not happening in the correct order
	    // relative to other parts of the command.  wenger 2011-10-10
            removeAll();

            allCanvas = new Vector();
            obsoleteGData = new Vector();
            newGData = new Vector();
            newCanvas = new Vector();
            obsoleteCanvas = new Vector();

            viewTable = new Hashtable();

            currentView = null;
            lastActionView = null;

            jsc.isSessionOpened = false;

            jsc.viewInfo.updateInfo();
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_IDLE, false);
            jsc.viewInfo.updateCount(0);

	    setToPermanentCursor();

            offScrImg = null;

	    // Get rid of our current fonts, so the list of fonts doesn't
	    // keep increasing as we perhaps draw different GDatas.
	    DEViseFonts.ClearFonts();

	    // We've just set a whole bunch of references to null, so
	    // this should be a good time to garbage collect.
	        System.gc();

	    updateObjects();
            repaint();
        }
    }

    //-------------------------------------------------------------------
    // Show the given help message in the named view.
    public synchronized void showHelpMsg(String viewName, String msg)
    {
        DEViseView view = getView(viewName);
        if (view == null) {
            return;
        }

        if (msg == null) {
            return;
        }

        if (view.canvas != null && view.parentView == null) {
	    view.canvas.helpMsg = msg;
	    view.canvas.repaint();
	}

	// We are a child view, figure out where to add childViewHelpMsgs vector
	else if (view.parentView != null) {
	    int element = view.parentView.viewChilds.indexOf(view);
	    if(view.parentView.canvas.childViewHelpMsgs.size() <= element) {
		view.parentView.canvas.childViewHelpMsgs.addElement(msg);
	    }
	    else {
		view.parentView.canvas.childViewHelpMsgs.setElementAt(msg, element);
	    }

	    view.parentView.canvas.repaint();
	}
    }

    //-------------------------------------------------------------------
    // Enable double-buffering
    //TEMP -- hmm -- does this goof up Jmol?
    public synchronized void update(Graphics gc)
    {
	if (offScrImg == null) {
            offScrImg = createImage(screenDim.width, screenDim.height);
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

    //-------------------------------------------------------------------
    // Update objects (canvases, buttons, etc.) -- remove obsolete ones
    // and actually add new ones to the GUI.
    private void updateObjects()
    {
        Runnable doUpdateObjects = new DoUpdateObjects();
	SwingUtilities.invokeLater(doUpdateObjects);
    }

    //-------------------------------------------------------------------
    // This class allows us to make sure that we actually do the object
    // updating in the GUI thread.
    private class DoUpdateObjects implements Runnable {
        public void run() {
            if (isDimChanged) {
                isDimChanged = false;
                setSize(screenDim);

                if (jsc.jsValues.uiglobals.inBrowser) {
		    //TEMP  AWT uses validate to cause a container to lay out its subcomponents again after the components it contains have been added to or modified
		    //TEMP -- do we really need to call validate here?
                    jsc.validate();
                } else {
                    jsc.parentFrame.pack();
                }

                jsc.repaint();
            }

	    // Actually remove from the GUI any canvases that are no longer
	    // needed (doing this here so it's done in the GUI thread).
            while (obsoleteCanvas.size() > 0) {
                DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
                remove(c);
                obsoleteCanvas.removeElement(c);
            }

	    // Actually add to the GUI any new canvases (doing this here
	    // so it's done in the GUI thread).
            while (newCanvas.size() > 0) {
                DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
                add(c, c.posZ);
                c.setBounds(c.getLocInScreen());
                newCanvas.removeElement(c);
            }

	    // Actually remove from the GUI any GUI objects (e.g., buttons)
	    // corresponding to GData that's no longer valid (doing this
	    // here so it's done in the GUI thread).  ("Normal" GDatas never
	    // get into the obsoleteGData list.)
            while (obsoleteGData.size() > 0) {
                DEViseGData gdata = (DEViseGData)obsoleteGData.firstElement();
		gdata.parentView.canvas.remove(gdata.symbol);
                obsoleteGData.removeElement(gdata);
		gdata.parentView.canvas._hasJavaSym = false;
            }

	    // Actually add to the GUI any GUI objects (e.g., buttons)
	    // corresponding to new GData records (doing this here so
	    // it's done in the GUI thread).  ("Normal" GDatas never
	    // get into the newGData list.)
            while (newGData.size() > 0) {
                DEViseGData gdata = (DEViseGData)newGData.firstElement();
                newGData.removeElement(gdata);
		if (gdata._menuType.equals("")) {
	            if (gdata.symbol == null) {
	                JButton button = new DEViseButton(gdata._buttonLabel,
		          jsc.jsValues);
                        button.setActionCommand(gdata._buttonCmd);
                        button.setFont(DEViseFonts.getFont(10,
		          DEViseFonts.MONOSPACED, 0, 0));
                        button.addActionListener(new ActionListener()
                            {
                                public void actionPerformed(ActionEvent event)
                                {
				    // Note: this is very specific to BMRB
				    // dynamics movies.
		                    if (event.getActionCommand().contains(
				      "&residues=")) {
		                        if (!jsc.showDynamicsMovieDlg()) {
			                    // User clicked "Cancel".
			                    return;
			                }
		                    }
                                    jsc.showDocument(event.getActionCommand());
                                }
                            });

                        gdata.symbol = button;
	            }
                    Component gs = gdata.symbol;
		    gdata.parentView.canvas.add(gs);
		    gs.setBounds(gdata.GDataLoc);
		    gdata.parentView.canvas._hasJavaSym = true;
		} else {
		    jsc.createSessionMenuItem(gdata._menuType,
		      gdata._menuName, gdata._buttonLabel, gdata._buttonCmd);
		}
            }
	}
    }

    //-------------------------------------------------------------------
    // Find the DEViseCanvas (if any) that the mouse pointer is currently
    // within.
    private DEViseCanvas findMouseCanvas()
    {
        for (int index = 0; index < allCanvas.size(); index++) {
	    DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(index);
	    if (canvas.mouseIsWithin()) {
	        return canvas;
	    }
	}

        return null;
    }

    //-------------------------------------------------------------------
    // Save the Jmol states for all Jmol canvases.  Note: this hasn't
    // really been tested with multiple Jmol canvases in a single
    // session (part of bug 1005 fix).
    private void saveJmolStates()
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.saveJmolStates()");
        }

	jmolStates = new Hashtable();

	Enumeration views = viewTable.elements();
	while (views.hasMoreElements()) {
	    DEViseView tmpView = (DEViseView)views.nextElement();
	    DEViseCanvas canvas = tmpView.canvas;
	    if (canvas != null) {
	        Object jmState = canvas.saveJmolState();
	        if (jmState != null) {
		    if (DEBUG >= 1) {
		        System.out.println("Saving Jmol state for view " +
		          tmpView.viewName);
		    }
	            jmolStates.put(tmpView.viewName, jmState);
	        }
	    }
        }
    }

    //-------------------------------------------------------------------
    // Restore the Jmol states for all Jmol canvases.  Note: this hasn't
    // really been tested with multiple Jmol canvases in a single
    // session (part of bug 1005 fix).
    private void restoreJmolStates()
    {
	if (DEBUG >= 1) {
            System.out.println("DEViseScreen.restoreJmolStates()");
        }

	Enumeration views = viewTable.elements();
	while (views.hasMoreElements()) {
	    DEViseView tmpView = (DEViseView)views.nextElement();
	    DEViseCanvas canvas = tmpView.canvas;
	    Object jmState = jmolStates.get(tmpView.viewName);
	    if (jmState != null) {
		if (DEBUG >= 1) {
		    System.out.println("Restoring Jmol state for view " +
		      tmpView.viewName);
		}
	        canvas.restoreJmolState(jmState);
	        // Make sure we don't restore more than once from one
	        // save.
	        jmolStates.remove(tmpView.viewName);
	    }
        }
    }

    //-------------------------------------------------------------------
    //
    // Note: we don't absolutely need this class; it's here just to help
    // debug the problem with input focus sometimes improperly getting
    // taken away from a DEViseCanvas.
    //
    class DSFocusListener extends FocusAdapter
    {
        //---------------------------------------------------------------
        public void focusGained(FocusEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("DEViseScreen gained focus");
	    }
	}

        //---------------------------------------------------------------
	public void focusLost(FocusEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("DEViseScreen lost focus");
	    }
	}
    }

    //-------------------------------------------------------------------
    //
    // Note: the methods here should not generally get called; however,
    // the JVM sometimes gets confused, and transfers input focus
    // to the DEViseScreen when the mouse is in a view (DEViseCanvas).
    // In that case, the event comes here and we have to pass it
    // along to the appropriate canvas.
    //
    class DSKeyListener extends KeyAdapter
    {
        //---------------------------------------------------------------
        public void keyPressed(KeyEvent event)
	{
	    if (DEBUG >= 2) {
	        System.out.println("DEViseScreen.keyPressed(" +
	          event.getKeyChar() + ")");
	    }

	    DEViseCanvas canvas = findMouseCanvas();
	    if (canvas != null) {
	        canvas.dispatchEvent(event);
	    }
	}

        //---------------------------------------------------------------
        public void keyReleased(KeyEvent event)
	{
	    if (DEBUG >= 2) {
	        System.out.println("DEViseScreen.keyReleased(" +
	          event.getKeyChar() + ")");
	    }

	    DEViseCanvas canvas = findMouseCanvas();
	    if (canvas != null) {
	        canvas.dispatchEvent(event);
	    }
	}
    }

    //-------------------------------------------------------------------
    // Following methods are for collaboration JavaScreens to 
    // process 3D view actions.
    public void collab3DView(String[] args)
    {
	DEViseView view = getView(args[1]);
	if (view == null) {
	    jsc.pn("No view found!");
	    return;
	}

	DEViseCanvas canvas = view.getCanvas();
	if (canvas == null) {
	    jsc.pn("No canvas found!");
	    return;
	}

	DEViseCrystal crystal = canvas.crystal;
	if (crystal == null) {
	    jsc.pn("No crystal found!");
	    return;
	}	    

	jsc.pn("Crystal found!");

	float[][] data = new float[3][3];
	float[] origin = new float[3];
	Float f = null;

	for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
		f = new Float(args[3*i+j+2]);
		data[i][j] = f.floatValue();
	    }
	}

	for (int i=0; i<3; i++) {
	    f = new Float(args[11+i]);
	    origin[i] = f.floatValue();
	}
	
	crystal.lcs.assign(data, origin);

	crystal.shiftedX = Integer.parseInt(args[14]);
	crystal.shiftedY = Integer.parseInt(args[15]);

	canvas.isMouseDragged = true;
	crystal.isTransformed = true;
	canvas.repaint();
	repaint();
    }
}
