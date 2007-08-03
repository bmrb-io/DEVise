// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides methods for dealing with mouse cursors in the
// JavaScreen.  Specifically, it creates all of the necessary cursors,
// and also has provisions for remembering the "permanent" cursor (e.g.,
// set according to toolbar mode) when temporarily changing the cursor
// to the busy cursor or a special cursor for moving or resizing DEVise
// cursors, etc.).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.2  2007/08/03 19:21:24  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.1.2.1  2007/07/25 18:25:26  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen.UI;

import java.util.*;
import java.awt.*;
import java.applet.*;
import JavaScreen.jsdevisec;
import JavaScreen.DEViseJSValues;

public class DEViseMouseCursor
{
    public static final Cursor defaultCursor = new Cursor(Cursor.DEFAULT_CURSOR);
    public static final Cursor waitCursor = new Cursor(Cursor.WAIT_CURSOR);
    public static final Cursor moveCursor = new Cursor(Cursor.MOVE_CURSOR);
    public static final Cursor lrsCursor = new Cursor(Cursor.W_RESIZE_CURSOR);
    public static final Cursor rrsCursor = new Cursor(Cursor.E_RESIZE_CURSOR);
    public static final Cursor trsCursor = new Cursor(Cursor.N_RESIZE_CURSOR);
    public static final Cursor brsCursor = new Cursor(Cursor.S_RESIZE_CURSOR);
    public static final Cursor tlrsCursor = new Cursor(Cursor.NW_RESIZE_CURSOR);
    public static final Cursor blrsCursor = new Cursor(Cursor.SW_RESIZE_CURSOR);
    public static final Cursor trrsCursor = new Cursor(Cursor.NE_RESIZE_CURSOR);
    public static final Cursor brrsCursor = new Cursor(Cursor.SE_RESIZE_CURSOR);
    public static final Cursor hdCursor = new Cursor(Cursor.HAND_CURSOR);
    public static final Cursor txtCursor = new Cursor(Cursor.TEXT_CURSOR);

    public static Cursor rbCursor;
    public Cursor decSymSizeCursor;
    public Cursor disabledCursor;
    public Cursor drillDownCursor;
    public Cursor fillViewCursor;
    public Cursor homeCursor;
    public Cursor incSymSizeCursor;
    public Cursor jmRotCursor;
    public Cursor jmTransCursor;
    public Cursor jmZoomCursor;
    // public Cursor moveCursor;
    public Cursor helpCursor;
    public Cursor togFiltCursor;
    public Cursor zoomInCursor;
    public Cursor zoomOutCursor;

    private Cursor permanentCursor;
    private Cursor temporaryCursor;

    /** Constructor.
    */
    public DEViseMouseCursor(DEViseJSValues jsValues)
    {
	if (decSymSizeCursor == null) {
	    try {
	        Toolkit toolkit = Toolkit.getDefaultToolkit();

		rbCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/crosshairs.png", jsValues),
		  new Point(7, 7), "default");

		decSymSizeCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/decrease_symbol_size.png",
		    jsValues),
		  new Point(4, 11), "decrease symbol size");

		disabledCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/disabled.png",
		    jsValues),
		  new Point(7, 7), "disabled");

		drillDownCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/drill-down.png",
		    jsValues),
		  new Point(4, 11), "drill down");

		fillViewCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/full_screen_filter.png",
		    jsValues),
		  new Point(7, 7), "cursor fills view");

		homeCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/home_filter.png",
		    jsValues),
		  new Point(7, 7), "home");

		incSymSizeCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/increase_symbol_size.png",
		    jsValues),
		  new Point(4, 11), "increase symbol size");

		jmRotCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/jmol_rotate.png",
		    jsValues),
		  new Point(8, 8), "Jmol rotate");

		jmTransCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/jmol_translate.png",
		    jsValues),
		  new Point(7, 7), "Jmol translate");

		jmZoomCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/jmol_zoom.png",
		    jsValues),
		  new Point(8, 6), "Jmol zoom");

/*
		moveCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/move_cursor.png",
		    jsValues),
		  new Point(7, 8), "Move DEVise cursor");
*/

		helpCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/show_view_help.png",
		    jsValues),
		  new Point(4, 11), "view help");

		togFiltCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/toggle_filter.png",
		    jsValues),
		  new Point(4, 11), "toggle filter");

		zoomInCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/zoom-in.png",
		    jsValues),
		  new Point(5, 5), "zoom in");

		zoomOutCursor = toolkit.createCustomCursor(
		  jsdevisec.loadImage(
		    "resources/toolbar_cursors/zoom-out.png",
		    jsValues),
		  new Point(5, 5), "zoom out");

                permanentCursor = defaultCursor;

	    } catch (Exception ex) {
	        System.err.println("Warning: cannot create " +
		  "non-standard cursors" + ex);
	    }
	}
    }

    /** Set the permanent cursor to the given cursor, and also set
        the cursor of the given component (if non-null) to the given
	cursor
	@param cursor: the cursor to switch to.
	@param comp: the Compoent for which to set the cursor.
    */
    public void setPermanentCursor(Cursor cursor, Component comp)
    {
        if (cursor != null) {
	    permanentCursor = cursor;
	} else {
	    System.err.println("Warning: setPermanentCursor called with " +
	      "null cursor; using default cursor");
	    permanentCursor = defaultCursor;
	}

	if (comp != null) {
	    comp.setCursor(permanentCursor);
	}
    }

    /** Set the temporary cursor to the given cursor, and also set
        the cursor of the given component (if non-null) to the given
	cursor
	@param cursor: the cursor to switch to.
	@param comp: the Compoent for which to set the cursor.
    */
    public void setTemporaryCursor(Cursor cursor, Component comp)
    {
        if (cursor != null) {
	    temporaryCursor = cursor;
	} else {
	    System.err.println("Warning: setTemporaryCursor called with " +
	      "null cursor; using default cursor");
	    temporaryCursor = defaultCursor;
	}

	if (comp != null) {
	    comp.setCursor(temporaryCursor);
	}
    }

    /** Set the cursor of the given component to the permanent cursor.
	@param comp: the Compoent for which to set the cursor.
    */
    public void setToPermanentCursor(Component comp)
    {
	if (comp != null) {
	    comp.setCursor(permanentCursor);
	}
    }
}
