// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2002
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
// Revision 1.94  2002/07/19 17:06:46  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.93  2002/06/17 19:40:13  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.92  2002/05/01 21:28:58  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
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
import  java.applet.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;
import  java.net.*;
import  java.io.*;
import  java.lang.*;
import  java.text.*;

public class DEViseCanvas extends Container
{
    private jsdevisec jsc = null;
    private DEViseScreen jscreen = null;
    private DEViseCmdDispatcher dispatcher = null;
    public int posZ = 0;

    public int oldHighlightAtomIndex = -1, highlightAtomIndex = -1;
    public Point point = new Point();
    public DEViseCrystal crystal = null;

    public DEViseView view = null; // base view if piled

    // ADD COMMENT -- we need clarification here as to why this is on a
    // per-canvas basis, since there can actually only be one active view
    // at a time in the entire JS; Hongyu says that this does not
    // necessarily get set to null when the active view becomes inactive.
    public DEViseView activeView = null;

    private Dimension canvasDim = null;

    private Image image = null;
    private Image offScrImg = null;
    private boolean isImageUpdated = false;

    public boolean isMouseDragged = false, isInViewDataArea = false,
      isInDataArea = false;
    private DEViseCursor selectedCursor = null;

    // See DEViseCursor.side* for values.
    private int whichCursorSide = DEViseCursor.sideNone;

    // Keep track of whether the mouse is actually in this canvas (part of
    // the fix for the "missing keystrokes" bugs.
    private boolean _mouseIsInCanvas = false;

    // These variables will hold the help messages to diplay when the 'Help' button is pressed
    // for both the main view and any child views.
    public String helpMsg = null;
    public Vector childViewHelpMsgs = new Vector();

    //TEMP -- what are these? op is old point???
    Point sp = new Point(), ep = new Point(), op = new Point();

    private int mouseDragCount = 0;

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
    private static final int ZOOM_MODE_NONE = 0;
    private static final int ZOOM_MODE_X = 1;
    private static final int ZOOM_MODE_Y = 2;
    private static final int ZOOM_MODE_XY = 3;
    private static int _zoomMode = ZOOM_MODE_NONE;

    // This is used to paint only rubberband lines (in XOR mode) while
    // we are dragging the rubberband line.  (In other words, we don't
    // actually repaint all of the other stuff on the canvas.)
    private boolean _paintRubberbandOnly = false;

    // Set this to true to draw rubberband lines in XOR mode (so we can
    // just redraw the rubberband line itself on each mouse drag).
    private static final boolean DRAW_XOR_RUBBER_BAND = true;

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

	    
	    // draw sxis lables
	    paintAxisLabel(gc);
        }
    }

    private synchronized void paintBackground(Graphics gc)
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

	   
          

    private synchronized boolean paintCrystal(Graphics gc)
    {
        if (view.viewDimension != 3) {
            return false;
        }

	if (jsc.jsValues.debug._debugLevel >= 2) {
            jsc.pn("Crystals have been painted " +
	      jsc.jsValues.canvas.crystalPaintCount++ + " times");
        }

        if (crystal == null) {
            createCrystal();
            if (crystal == null) {
                paintBackground(gc);
                return true;
            }
        }

        paintBackground(gc);
        crystal.paint(this, gc, isMouseDragged);

        return true;
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
            loc = cursor.getLocInCanvas();

            if (selectedCursor == cursor && isMouseDragged) { // draw cursor movement box
                if (loc.width < 4)
                    loc.width = 4;
                if (loc.height < 4)
                    loc.height = 4;

                gc.setColor(Color.yellow);
                gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
                gc.setColor(Color.red);
                gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3, loc.height - 3);

                continue;
            }

            if (cursor.image == null || isImageUpdated) {
                cursor.image = getCroppedXORImage(cursor.color, loc);
            }

            if (cursor.image != null) {
                gc.drawImage(cursor.image, loc.x, loc.y, this);
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
	  activeView.isRubberBand) {
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
		if ((keyChar == 'r' || keyChar == 'R') && crystal != null) {
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

	// This function will do one of two things.  If the help button has been pushed, and a tan
	// box is showing the help in this view, this function will hide that help box.  If there is not
	// currently a help box displayed, this function will show the view's help in a dialog box.
	private void showHideHelp()
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

	private void resetCrystal()
	{
	    crystal.totalShiftedX = 0;
	    crystal.totalShiftedY = 0;
	    crystal.totalX = 0.0f;
	    crystal.totalY = 0.0f;
	    crystal.totalZ = 0.0f;
	    crystal.totalXRotation = 0.0f;
	    crystal.totalYRotation = 0.0f;
	    crystal.totalScaleFactor = 1.0f;
	    crystal.resetAll(true);
	    repaint();

            // send command to collaborations if necessary
	    if (jsc.specialID == -1) {
	        String cmd = DEViseCommands.SET_3D_CONFIG + " "
		  + activeView.getCurlyName();
			
		float[][] data = crystal.lcs.getData();
		float[] origin = crystal.lcs.getOrigin();
		Float f = null;
			
		for (int i=0; i<3; i++) 
		    for (int j=0; j<3; j++) {
			f = new Float(data[i][j]);
			cmd = cmd + " {" + f.toString() + "}";
		    }
			
		for (int i=0; i<3; i++) {
		    f = new Float(origin[i]);
		    cmd = cmd + " {" + f.toString() + "}";
		}

		cmd = cmd + " {" + crystal.shiftedX + "}" +
		  " {" + crystal.shiftedY + "}";
			

		if (jsc.dispatcher.getStatus() ==
		  DEViseCmdDispatcher.STATUS_IDLE)
		    dispatcher.start(cmd);	
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

            if (view.viewDimension == 3) {
                // setCursor(DEViseUIGlobals.rbCursor);  - Ven modified
		   setCursor(DEViseUIGlobals.hdCursor);

                activeView = view;
                jsc.viewInfo.updateInfo(activeView.viewName,
		  activeView.getX(sp.x), activeView.getY(sp.y));
                if (jscreen.getCurrentView() != activeView) {
                    jscreen.setCurrentView(activeView);
                }

                return;
            }

            checkMousePos(sp, true);

	    // Control-drag is X-only zoom.
            if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_CONTROL) {
	        _zoomMode = ZOOM_MODE_X;
	    } else {
	        _zoomMode = ZOOM_MODE_XY;
	    }

            if (isInViewDataArea && selectedCursor == null &&
	      activeView.isRubberBand) {
                // We can only draw just the rubberband line if we're
		// drawing it in XOR mode.
                _paintRubberbandOnly = DRAW_XOR_RUBBER_BAND;
            }
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
            if (view.viewDimension == 3) {
		// send command to collaborations if necessary
		if (jsc.specialID == -1) {
		    // for 3D drill-down
		    if ((jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) 
			&& (activeView.isDrillDown) && (!isMouseDragged) 
			&& (isInViewDataArea)) {
			Point p = event.getPoint();			
			drillDown3D(p);
			return;
		    }

		    String cmd = DEViseCommands.SET_3D_CONFIG + " "
			+ activeView.getCurlyName();
		    
		    float[][] data = crystal.lcs.getData();
		    float[] origin = crystal.lcs.getOrigin();
		    Float f = null;
		    
		    for (int i=0; i<3; i++) 
			for (int j=0; j<3; j++) {
			    f = new Float(data[i][j]);
			    cmd = cmd + " {" + f.toString() + "}";
			}

		    for (int i=0; i<3; i++) {
			f = new Float(origin[i]);
			cmd = cmd + " {" + f.toString() + "}";
		    }

		    cmd = cmd + " {" + crystal.shiftedX + "}" +
			" {" + crystal.shiftedY + "}";

		    dispatcher.start(cmd);	
		}

                isMouseDragged = false;
                repaint();		

                return;
            }

            if (isMouseDragged && isInViewDataArea) {
                Point p = event.getPoint();
                String cmd = "";

                if (selectedCursor != null) { // move or resize cursor
                    ep.x = activeView.translateX(p.x, 1);
                    ep.y = activeView.translateY(p.y, 1);

                    jsc.viewInfo.updateInfo(activeView.getX(ep.x),
		      activeView.getY(ep.y));

                    int dx = ep.x - sp.x, dy = ep.y - sp.y;
                    DEViseCursor cursor = selectedCursor;

                    cursor.updateCursorLoc(dx, dy, whichCursorSide, true);

                    cursor.image = null;

                    cmd = cmd + DEViseCommands.CURSOR_CHANGED + " {" +
		      cursor.name + "} " + cursor.x + " " + cursor.y +
		      " " + cursor.width + " " + cursor.height;

                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                } else if (activeView.isRubberBand) { // rubber band (zoom)
                    ep.x = activeView.translateX(p.x, 1);
                    ep.y = activeView.translateY(p.y, 1);

                    jsc.viewInfo.updateInfo(activeView.getX(ep.x),
		      activeView.getY(ep.y));

                    int w = ep.x - sp.x, h = ep.y - sp.y;
                    if (w < 0)
                        w = -w;
                    if (h < 0)
                        h = -h;

                    if (w > jsc.jsValues.uiglobals.rubberBandLimit.width ||
		      h > jsc.jsValues.uiglobals.rubberBandLimit.height) {
			try {
                            cmd = cmd + DEViseCommands.MOUSE_RUBBERBAND +
			      " " + activeView.getCurlyName() + " " +
			      activeView.translateX(sp.x, 2) + " " +
			      activeView.translateY(sp.y, 2) + " " +
			      activeView.translateX(ep.x, 2) + " " +
			      activeView.translateY(ep.y, 2);

			    // Alt-drag zooms out.
                            if (jsc.jsValues.canvas.lastKey == 
				KeyEvent.VK_ALT || 
				jsc.jsValues.canvas.lastKey == 
				KeyEvent.VK_SHIFT) {
                                cmd = cmd + " 1";
                            } else {
                                cmd = cmd + " 0";
                            }

			    if (_zoomMode == ZOOM_MODE_X) {
                                cmd = cmd + " 1";
                            } else if (_zoomMode == ZOOM_MODE_XY) {
                                cmd = cmd + " 0";
                            } else {
			        throw new YError("Illegal zoom mode (" +
			          _zoomMode + ")");
			    }

                            jscreen.guiAction = true;
                            dispatcher.start(cmd);
			} catch (YError err) {
			    System.err.println(err.getMessage());
			}
			_zoomMode = ZOOM_MODE_NONE;
                    }
                }

                isInViewDataArea = false;
                selectedCursor = null;
                whichCursorSide = DEViseCursor.sideNone;
                isMouseDragged = false;

                repaint();

		// Workaround for bug 607.  Note that this means that things
		// will *not* work correctly if you do two consecutive drags
		// while holding down the control key the whole time.
		// RKW 2000-08-07.
		jsc.jsValues.canvas.lastKey = KeyEvent.VK_UNDEFINED;
            }
        }

        public void mouseClicked(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
		  "DEViseCanvas(" + view.viewName +
		    ").ViewMouseListener.mouseClicked()");
            }

            if (view.viewDimension == 3) {
		return;
            }

            if (!isMouseDragged && isInViewDataArea) {
                String cmd = null;
                Point p = event.getPoint();

                if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) {
		    if (activeView.isDrillDown) {
                        cmd = DEViseCommands.SHOW_RECORDS + " " +
			  activeView.getCurlyName() + " " +
			  activeView.translateX(p.x, 2) + " " +
			  activeView.translateY(p.y, 2);
		    }
                } else if (activeView.isCursorMove) {
                    DEViseCursor cursor = activeView.getFirstCursor();

                    if (cursor != null && (cursor.isXMovable ||
		      cursor.isYMovable)) {
                        Rectangle loc = cursor.cursorLocInCanvas;

                        int dx = p.x - loc.x - loc.width / 2;
                        int dy = p.y - loc.y - loc.height / 2;

                        cursor.updateCursorLoc(dx, dy, whichCursorSide,
			  true);

                        cmd = DEViseCommands.CURSOR_CHANGED + " {" +
			  cursor.name + "} " + cursor.x + " " + cursor.y + " "
			  + cursor.width + " " + cursor.height;

                        cursor.image = null;
                    }
                }

                if (cmd != null) {
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                }

                isInViewDataArea = false;
                selectedCursor = null;
                whichCursorSide = DEViseCursor.sideNone;
                isMouseDragged = false;

                repaint();
            }
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

            if (view.viewDimension == 3 && crystal != null) {
                int dx = p.x - op.x, dy = p.y - op.y;
                op.x = p.x;
                op.y = p.y;

                jsc.viewInfo.updateInfo(activeView.getX(p.x),
		  activeView.getY(p.y));

                if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_ALT || 
		    jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) {
                    crystal.translate(dx, dy);
                } else if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_CONTROL) {
                    crystal.scale(dx, dy);
                } else {
                    crystal.rotate(dx, dy);
                }

                jsc.jsValues.canvas.isInteractive = true;
                jsc.jsValues.canvas.sourceCanvas = DEViseCanvas.this;
                repaint();

                return;
            }

            if (isInViewDataArea) {
                ep.x = activeView.translateX(p.x, 1);
                ep.y = activeView.translateY(p.y, 1);

                jsc.viewInfo.updateInfo(activeView.getX(ep.x),
		  activeView.getY(ep.y));

                if (selectedCursor != null) {

                    DEViseCursor cursor = selectedCursor;

                    int dx = ep.x - sp.x, dy = ep.y - sp.y;

                    cursor.updateCursorLoc(dx, dy, whichCursorSide, false);
                }

                jsc.jsValues.canvas.isInteractive = true;
                jsc.jsValues.canvas.sourceCanvas = DEViseCanvas.this;
                repaint();
            }
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

            if (view.viewDimension == 3 && crystal != null) {
                point.x = p.x;
                point.y = p.y;

               //  setCursor(DEViseUIGlobals.rbCursor); - Ven modified
		   setCursor(DEViseUIGlobals.hdCursor);

                activeView = view;
                jsc.viewInfo.updateInfo(activeView.viewName,
		  activeView.getX(p.x), activeView.getY(p.y));
                if (jscreen.getCurrentView() != activeView) {
                    jscreen.setCurrentView(activeView);
                }

		return;
            }

            checkMousePos(p, true);
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
    public synchronized void checkMousePos(Point p, boolean checkDispatcher)
      throws YError
    {
        if (DEBUG >= 3) {
            System.out.println("DEViseCanvas(" + view.viewName +
	      ").checkMousePos(" + p.x + ", " + p.y + ")");
        }

        // initialize value
        whichCursorSide = DEViseCursor.sideNone;
        selectedCursor = null;
        isInViewDataArea = false;
        activeView = null;

        if (checkMousePos(p, view)) { // activeView will not be null
	    Cursor tmpCursor = null;
            if (checkDispatcher && jsc.dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                // dispatcher still busy
                tmpCursor = DEViseUIGlobals.waitCursor;
            } else if (isInViewDataArea && selectedCursor == null) {
                // inside the data area but not within any cursor, you
		// can draw rubber band or get the records at that data point
                 tmpCursor = DEViseUIGlobals.rbCursor; 
            } else if (isInViewDataArea && view.viewDimension == 3) {
		 tmpCursor = DEViseUIGlobals.defaultCursor;
	    } else if (isInViewDataArea && selectedCursor != null) {
                switch (whichCursorSide) {
                case DEViseCursor.sideNone:
		    // Not on a cursor, do nothing.
		    break;

                case DEViseCursor.sideMiddle:
		    // Inside a cursor, you can move this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideMiddle");
                    tmpCursor = DEViseUIGlobals.moveCursor;
                    break;

                case DEViseCursor.sideLeft:
		    // On left side of a cursor, you can resize this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideLeft");
                    tmpCursor = DEViseUIGlobals.lrsCursor;
                    break;

                case DEViseCursor.sideRight:
		    // On right side of a cursor, you can resize this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideRight");
                    tmpCursor = DEViseUIGlobals.rrsCursor;
                    break;

                case DEViseCursor.sideTop:
		    // On top side of a cursor, you can resize this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideTop");
                    tmpCursor = DEViseUIGlobals.trsCursor;
                    break;

                case DEViseCursor.sideBottom:
		    // On bottom side of a cursor, you can resize this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideBottom");
                    tmpCursor = DEViseUIGlobals.brsCursor;
                    break;

                case DEViseCursor.sideTopLeft:
		    // On left-top corner of a cursor, you can resize
		    // this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideTopLeft");
                    tmpCursor = DEViseUIGlobals.tlrsCursor;
                    break;

                case DEViseCursor.sideBottomLeft:
		    // On left-bottom corner of a cursor, you can resize
		    // this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideBottomLeft");
                    tmpCursor = DEViseUIGlobals.blrsCursor;
                    break;

                case DEViseCursor.sideTopRight:
		    // On right-top corner of a cursor, you can resize
		    // this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideTopRight");
                    tmpCursor = DEViseUIGlobals.trrsCursor;
                    break;

                case DEViseCursor.sideBottomRight:
		    // 0n right-bottom corner of a cursor, you can resize
		    // this cursor.
                    if (DEBUG >= 3) System.out.println(
		      "whichCursorSide = DEViseCursor.sideBottomRight");
                    tmpCursor = DEViseUIGlobals.brrsCursor;
                    break;

		default:
		    throw new YError("Illegal whichCursorSide value: " +
		      whichCursorSide);
                }
            } else { // inside the view but not within the data area of that view
                tmpCursor = DEViseUIGlobals.defaultCursor;
            }

	    //TEMP -- should we only do this if the current cursor is
	    // different than the one required???
            setCursor(tmpCursor);

            if (activeView != null && activeView.pileBaseView != null) {
                activeView = activeView.pileBaseView;
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            // show the data at current mouse position
	    if (activeView != null && jsc.viewInfo != null) {
		if(isInViewDataArea) {
		    jsc.viewInfo.updateInfo(activeView.viewName,
		      activeView.getX(p.x), activeView.getY(p.y));
		} else {
		    jsc.viewInfo.updateInfo();
		}
	    }

        } else { // activeView is null and all other values will be initialized value before
            whichCursorSide = DEViseCursor.sideNone;
            selectedCursor = null;
            isInViewDataArea = false;
            activeView = null;

	    Cursor tmpCursor = null;
            if (!checkDispatcher || jsc.dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                tmpCursor = DEViseUIGlobals.waitCursor;
            } else {
                tmpCursor = DEViseUIGlobals.defaultCursor;
            }

	    //TEMP -- should we only do this if the current cursor is
	    // different than the one required???
            setCursor(tmpCursor);

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            jsc.viewInfo.updateInfo();
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

	if (v.isCursorMove) {
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

    // Create "crystal" (used for drawing in 3D views/piles).  Note that
    // in a 3D pile there is only one crystal for the entire pile.
    public void createCrystal()
    {
        if (view.viewDimension != 3) {
            return;
        }

        if (view.viewGDatas.size() == 0) {
            return;
        }

        if (crystal == null) {
            int size = view.viewGDatas.size();
            //float[][] atomPos = new float[size][3];
            //float[][][] bondPos = new float[size][
            //String[] atomName = new String[size];
            Vector aPos = new Vector(size), aName = new Vector(size),
			  aColor = new Vector(size), bPos = new Vector(size),
			  bColor = new Vector(size);
            for (int i = 0; i < size; i++) {
                DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(i);
		if (gdata.symbolType == gdata._symSegment) {
                    float[][] pos = new float[2][3];
                    pos[0][0] = gdata.x0;
                    pos[0][1] = gdata.y0;
                    pos[0][2] = gdata.z0;
                    pos[1][0] = gdata.x1;
                    pos[1][1] = gdata.y1;
                    pos[1][2] = gdata.z1;
                    bPos.addElement(pos);
                    bColor.addElement(gdata.color);
                } else {
                    aName.addElement(gdata.string);
                    float[] pos = new float[3];
                    pos[0] = gdata.x0;
                    pos[1] = gdata.y0;
                    pos[2] = gdata.z0;
                    aPos.addElement(pos);
                    aColor.addElement(gdata.color);
                }
            }

            String[] atomName = null;
            float[][] atomPos = null;
            float[][][] bondPos = null;
            Color[] bondColor = null;
            Color[] atomColor = null;
            if (aName.size() > 0) {
                atomName = new String[aName.size()];
                for (int i = 0; i < atomName.length; i++) {
                    atomName[i] = (String)aName.elementAt(i);
                }
            }

            if (aPos.size() > 0) {
                atomPos = new float[aPos.size()][3];
                for (int i = 0; i < atomPos.length; i++) {
                    atomPos[i] = (float[])aPos.elementAt(i);
                }
            }

            if (aColor.size() > 0) {
                atomColor = new Color[aColor.size()];
                for (int i = 0; i < atomColor.length; i++) {
                    atomColor[i] = (Color)aColor.elementAt(i);
                }
            }

            if (bPos.size() > 0) {
                bondPos = new float[bPos.size()][2][3];
                for (int i = 0; i < bondPos.length; i++) {
                    bondPos[i] = (float[][])bPos.elementAt(i);
                }
            }

            if (bColor.size() > 0) {
                bondColor = new Color[bColor.size()];
                for (int i = 0; i < bondColor.length; i++) {
                    bondColor[i] = (Color)bColor.elementAt(i);
                }
            }

            try {
                DEVise3DLCS lcs = new DEVise3DLCS();
                crystal = new DEViseCrystal(canvasDim.width - 10, canvasDim.height - 10, 5, lcs, -1, -1, atomName, atomPos, atomColor, bondPos, bondColor);
            } catch (YException e) {
                jsc.pn(e.getMessage());
                crystal = null;
                return;
            }
        }

        if (view.viewPiledViews.size() > 0) {
            crystal.setSelect();

            for (int i = 0; i < view.viewPiledViews.size(); i++) {
	        DEViseView v = (DEViseView)view.viewPiledViews.elementAt(i);
		for (int j = 0; j < v.viewGDatas.size(); j++) {
		    DEViseGData gdata = (DEViseGData)v.viewGDatas.elementAt(j);
		    if (gdata.symbolType == gdata._symSegment) {
		        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0,
			  gdata.color, true);
                        crystal.setSelect(gdata.x1, gdata.y1, gdata.z1,
			  gdata.color, true);
                    } else {
                        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0,
			  gdata.color, false);
                    }
                }
            }
        }
    }

    // 3D drill-down
    public void drillDown3D(Point p)
    {
	jsc.pn("Mouse point: " + p.x + " " + p.y);
	Vector atomList = new Vector();
	Float f;
	String s = null;

	s = crystal.drillDown3D(p, atomList);
	if (atomList.size() > 0)
	    jsc.pn(s);
	else 
	    jsc.pn("No atom.");

	if (atomList.size() > 0) {
	    String cmd = DEViseCommands.SHOW_RECORDS3D + " " + 
	    activeView.getCurlyName() + " " + atomList.size();
	    
	    for (int i=0; i<atomList.size(); i++) {
		DEViseAtomInCrystal atom = (DEViseAtomInCrystal) atomList.elementAt(i);
		for (int j=0; j<3; j++) {
		    f = new Float(atom.pos[j]);
		    cmd = cmd + " " + f.toString();
		}
	    }

	    jscreen.guiAction = true;
	    dispatcher.start(cmd);
	}
	
	isInViewDataArea = true;
	selectedCursor = null;
	whichCursorSide = DEViseCursor.sideNone;
	isMouseDragged = false;
	repaint();
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
