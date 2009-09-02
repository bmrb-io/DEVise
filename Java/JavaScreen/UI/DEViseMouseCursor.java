// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007-2009
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
// Revision 1.2  2007/08/03 20:17:34  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
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

    private DEViseJSValues _jsValues;
    private Toolkit _toolkit;

    /** Constructor.
    */
    public DEViseMouseCursor(DEViseJSValues jsValues)
    {
	_jsValues = jsValues;

	if (decSymSizeCursor == null) {
	    try {
	        _toolkit = Toolkit.getDefaultToolkit();
		boolean doubleCursor = false;
	        Dimension curDim = _toolkit.getBestCursorSize(16, 16);
	        System.out.println("Best cursor size: " + curDim.width +
	          ", " + curDim.height);
		if (curDim.width == 32 && curDim.height == 32) {
		    doubleCursor = true;
		    System.out.println("Warning: expecting doubled " +
		      "cursor size; adjusting hotspot accordingly");
		}

		rbCursor = createCursor("crosshairs.png",
		  7, 7, doubleCursor, "default");

		decSymSizeCursor = createCursor(
		  "decrease_symbol_size.png", 4, 11, doubleCursor,
		  "decrease symbol size");

		disabledCursor = createCursor("disabled.png",
		  7, 7, doubleCursor, "disabled");

		drillDownCursor = createCursor("drill-down.png",
		  4, 11, doubleCursor, "drill down");

		fillViewCursor = createCursor("full_screen_filter.png",
		  7, 7, doubleCursor, "cursor fills view");

		homeCursor = createCursor("home_filter.png",
		  7, 7, doubleCursor, "home");

		incSymSizeCursor = createCursor("increase_symbol_size.png",
		  4, 11, doubleCursor, "increase symbol size");

		jmRotCursor = createCursor("jmol_rotate.png",
		  8, 8, doubleCursor, "Jmol rotate");

		jmTransCursor = createCursor("jmol_translate.png",
		  7, 7, doubleCursor, "Jmol translate");

		jmZoomCursor = createCursor("jmol_zoom.png",
		  8, 6, doubleCursor, "Jmol zoom");

/*
		moveCursor = createCursor("move_cursor.png",
		  7, 8, doubleCursor, "Move DEVise cursor");
*/

		helpCursor = createCursor("show_view_help.png",
		  4, 11, doubleCursor, "view help");

		togFiltCursor = createCursor("toggle_filter.png",
		  4, 11, doubleCursor, "toggle filter");

		zoomInCursor = createCursor("zoom-in.png",
		  5, 5, doubleCursor, "zoom in");

		zoomOutCursor = createCursor("zoom-out.png",
		  5, 5, doubleCursor, "zoom out");

                permanentCursor = defaultCursor;

	    } catch (Exception ex) {
	        System.err.println("Warning: cannot create " +
		  "non-standard cursors" + ex);
	    }
	}
    }

    /** Create a cursor with the given parameters.
        @param imageFile: the file containing the image to use for this
	  cursor (in the JavaScreen resources).
	@param hotSpotX: the X coordinate of the cursor hot spot.
	@param hotSpotY: the Y coordinate of the cursor hot spot.
	@param doubleCursor: whether the cursor size will be doubled
	  (see bug 978).
	@param name: the name of the cursor.
	@return the cursor created according to the given parameters,
	  or the default cursor if there is an error.
    */
    protected Cursor createCursor(String imageFile, int hotSpotX,
      int hotSpotY, boolean doubleCursor, String name)
    {
	Cursor cursor;
    	try {
	    Point hotSpot = new Point(hotSpotX, hotSpotY);
	    if (doubleCursor) {
	        doublePoint(hotSpot);
	    }
	    cursor = _toolkit.createCustomCursor(
	      jsdevisec.loadImage("resources/toolbar_cursors/" + imageFile,
	        _jsValues),
	      hotSpot, name);
	} catch (Exception ex) {
	    System.err.println("Warning: cannot create mouse cursor " + name +
	      ": " + ex.toString() + "; using default cursor");
	    cursor = defaultCursor;
	}

	return cursor;
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

    /** Double the coordinates of the given point
        @param p: the Point to be doubled (input/output).
    */
    public static void doublePoint(Point p)
    {
    	p.x = p.x * 2;
	p.y = p.y * 2;
    }
}
