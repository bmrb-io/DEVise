// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class holds view properties (such as location, cursors,
// pile information, and so on).  Methods of this class are mostly
// called by the DEViseCanvas class.

// There is one instance of this class for each view in the session.
// (This includes view symbols, piled views, etc.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.44  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.43  2000/04/07 22:44:10  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.42  2000/04/05 15:42:24  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.41  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.40  2000/03/31 19:29:17  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.39  2000/03/23 16:26:15  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.38  2000/02/14 10:45:23  hongyu
// *** empty log message ***
//
// Revision 1.37  1999/12/07 23:18:21  hongyu
// *** empty log message ***
//
// Revision 1.36  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.35  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.34  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.33  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.32  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseView.java

import  java.awt.*;
import  java.util.*;
import  java.text.*;

// This class represent a image view in DEVise
public class DEViseView
{
    public jsdevisec jsc = null;

    public DEViseCanvas canvas = null;

    public String viewName = null;
    public String curlyName = null;

    public Rectangle viewLoc = null;
    public Rectangle viewLocInCanvas = null;
    public float viewZ = 0.0f; // for piled views
    public int viewDimension = 0; // number of dimensions
    public int viewBg, viewFg;

    // viewDataLoc is the location relative to this view
    public Rectangle viewDataLoc = null;
    public float viewDataXMin = 0.0f, viewDataXMax = 0.0f;
    public float viewDataYMin = 0.0f, viewDataYMax = 0.0f;
    // data type could be "real" or "date" or "none"
    public String viewDataXType = null, viewDataYType = null;

    public String viewTitle = null;
    public int viewDTX, viewDTY; // title X and Y location
    public Font viewDTFont = null;

    public DEViseView parentView = null;

    // Base view of a piled view.  Note that this is null in the base view
    // itself.
    public DEViseView pileBaseView = null;

    public Vector viewPiledViews = new Vector();
    public Vector viewChilds = new Vector();
    public Vector viewGDatas = new Vector();
    public Vector viewCursors = new Vector();

    public boolean isRubberBand, isCursorMove, isDrillDown, isKey;
    public float dataXStep, dataYStep;
    public float gridx, gridy;

    public boolean isFirstTime = true;

    private static final boolean _debug = false;

    public DEViseView(jsdevisec panel, String pn, String name, String piledname, String title, Rectangle loc, float Z, int dim, int bg, int fg, Rectangle dl, String xt, String yt, float gx, float gy, int rb, int cm, int dd, int ky)
    {
        jsc = panel;

        viewName = name;
        viewTitle = title;
        viewLoc = loc;
        viewZ = Z;
        viewDimension = dim;
        viewBg = bg;
        viewFg = fg;

        viewDataLoc = dl;
        //viewDataLoc.x += DEViseUIGlobals.screenEdge.width;
        //viewDataLoc.y += DEViseGlobals.
        viewDataXType = xt;
        viewDataYType = yt;

        parentView = jsc.jscreen.getView(pn);

	// Note that this will be null for the base view, because in this
	// constructor the DEViseScreen object doesn't know about this
	// view yet.
        pileBaseView = jsc.jscreen.getView(piledname);

        viewLocInCanvas = getLocInCanvas();

        gridx = gx;
        gridy = gy;

        isRubberBand = (rb == 1);

        isCursorMove = (cm == 1);

        isDrillDown = (dd == 1);

        isKey = (ky == 1);

        curlyName = "{" + viewName + "}";
    }

    protected void finalize() {
    	if (_debug) {
            System.out.println("DEViseView(" + viewName + ").finalize()");
        }
	removeAllGData();
    }

    // Returns view name enclosed by curly brackets.
    public String getCurlyName()
    {
        return curlyName;
    }

/* Not used.  RKW 2000-05-03.
    public DEViseView getChild(int idx)
    {
        if (idx < 0 || idx >= viewChilds.size())
            return null;

        return (DEViseView)viewChilds.elementAt(idx);
    }
*/

/* Not used.  RKW 2000-05-03.
    public DEViseCursor getCursor(int idx)
    {
        if (idx < 0 || idx >= viewCursors.size())
            return null;

        return (DEViseCursor)viewCursors.elementAt(idx);
    }
*/

/* Not used.  RKW 2000-05-03.
    public DEViseGData getGData(int idx)
    {
        if (idx < 0 || idx >= viewGDatas.size())
            return null;

        return (DEViseGData)viewGDatas.elementAt(idx);
    }
*/

    // Add a child view to this view.
    public void addChild(DEViseView view) throws YError
    {
        if (view != null) {
	    if (view.parentView != this) {
	        throw new YError("Adding child view (" + view.viewName +
		  ") to wrong parent(" + viewName + ")");
	    }

	    if (viewChilds.contains(view)) {
	        throw new YError("Child view " + view.viewName +
		  " is already in parent (" + viewName + ")");
	    } else {
                viewChilds.addElement(view);
	    }
        }
    }

    // Add a view to the base view of a pile.
    public void addPiledView(DEViseView view) throws YError
    {
	if (pileBaseView != null) {
	    throw new YError("Adding piled view (" + view.viewName +
	      ") to non-base view (" + viewName + ")");
	}

        if (view != null) {
	    if (view.pileBaseView != this) {
	        throw new YError("Adding piled view (" + view.viewName +
		  ") to wrong base view (" + viewName + ")");
	    }
	    if (viewPiledViews.contains(view)) {
	        throw new YError("Pile " + viewName +
		  "already contains view " + view.viewName);
	    }

            for (int i = 0; i < viewPiledViews.size(); i++) {
                DEViseView v = (DEViseView)viewPiledViews.elementAt(i);
                if (view.viewZ < v.viewZ) {
                    viewPiledViews.insertElementAt(view, i);
                    return;
                }
            }

            viewPiledViews.addElement(view);
        }
    }

    // Add a cursor to this view.  Returns true iff cursor exists and is
    // not already in this view.
    public boolean addCursor(DEViseCursor cursor) throws YError
    {
        if (cursor == null) {
            return false;
        }

	if (cursor.parentView != this) {
	    throw new YError("Adding cursor (" + cursor.name +
	      ") to wrong view (" + viewName + ")");
	}

        for (int i = 0; i < viewCursors.size(); i++) {
            DEViseCursor c = (DEViseCursor)viewCursors.elementAt(i);
            if (c.name.equals(cursor.name)) {
                if (c.isSame(cursor)) {
                    return false;
                } else {
                    viewCursors.removeElementAt(i);
                }
            }
        }

        cursor.image = null;

        viewCursors.addElement(cursor);

        return true;
    }

    // Add a GData record to this view.
    public void addGData(DEViseGData gdata)
    {
	// Note: I'm not checking GData.parentView here for speed.
	// RKW 2000-05-3.
        if (gdata != null) {
            viewGDatas.addElement(gdata);
        }
    }

    // Remove a child view from this view.
    public void removeChild(DEViseView view)
    {
        if (view != null) {
            if (canvas != null) {
                if (canvas.activeView == view) {
                    canvas.activeView = null;
                }
            }

            viewChilds.removeElement(view);
        }
    }

    // Removed a piled view from its base view.
    public void removePiledView(DEViseView view) throws YError
    {
	if (pileBaseView != null) {
	    throw new YError("Removing piled view from non-base view (" +
	      viewName + ")");
	}
        if (view != null) {
            viewPiledViews.removeElement(view);
        }
    }

    // Remove the given cursor from this view.
    public void removeCursor(DEViseCursor cursor)
    {
        if (cursor != null) {
            viewCursors.removeElement(cursor);
        }
    }

    // Remove the named cursor from this view; returns true iff the cursor
    // is in this view.
    public boolean removeCursor(String name)
    {
        if (name != null) {
            for (int i = 0; i < viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(i);
                if (cursor.name.equals(name)) {
                    viewCursors.removeElement(cursor);
                    return true;
                }
            }

            return false;
        } else {
            return false;
        }
    }

/* Not used.  RKW 2000-05-01.
    public void removeGData(DEViseGData gdata)
    {
        if (gdata != null) {
            viewGDatas.removeElement(gdata);
        }
    }
*/

    // Remove all GData records from this view.
    public void removeAllGData()
    {
        if (_debug) {
            System.out.println("DEViseView(" + viewName +
	      ").removeAllGData()");
        }
        viewGDatas = new Vector();
    }

    // Remove all cursors from this view.
    public void removeAllCursor()
    {
        viewCursors.removeAllElements();
    }

    // Remove all child views from this view.
    public void removeAllChild()
    {
        if (canvas != null) {
            if (canvas.activeView != this) {
                canvas.activeView = null;
            }
        }

        viewChilds.removeAllElements();
    }

/* Not used.  RKW 2000-05-03.
    public void removeAllPile()
    {
        viewPiledViews.removeAllElements();
    }
*/

    // Get the canvas corresponding to this view (note that if this view
    // is a child view or a non-base piled view, the canvas returned is
    // not "owned" by this view).
    public DEViseCanvas getCanvas()
    {
        if (canvas == null) {
            if (parentView == null) {
                if (pileBaseView == null) { // this is a top view, but the canvas for it has not yet been set
                    return null;
                } else {
                    return pileBaseView.getCanvas();
                }
            } else {
                return parentView.getCanvas();
            }
        } else {
            return canvas;
        }
    }

    // Returns the location of this view relative to its canvas.
    // Relative to upper left corner of canvas?
    public Rectangle getLocInCanvas()
    {
        if (canvas == null) {
            Rectangle loc = null;

            if (parentView == null) {
                if (pileBaseView == null) {
                    // this is a top view, but the canvas for it has not yet been set
                    return new Rectangle(0, 0, viewLoc.width, viewLoc.height);
                } else {
                    loc = pileBaseView.getLocInCanvas();
                }
            } else {
                loc = parentView.getLocInCanvas();
                loc.x = loc.x + viewLoc.x;
                loc.y = loc.y + viewLoc.y;
                loc.width = viewLoc.width;
                loc.height = viewLoc.height;
            }

            return loc;
        } else {
            return new Rectangle(0, 0, viewLoc.width, viewLoc.height);
        }
    }

    // Returns the location of this view relative to the entire JavaScreen.
    // Relative to upper left corner of screen?
    public Rectangle getLocInScreen()
    {
        DEViseCanvas canvas = getCanvas();
        Rectangle loc = viewLocInCanvas;

        if (canvas == null) {
            DEViseView v = this;
            while (v.parentView != null) {
                v = v.parentView;
            }
            return new Rectangle(loc.x + v.viewLoc.x, loc.y + v.viewLoc.y, viewLoc.width, viewLoc.height);
        } else {
            return new Rectangle(loc.x + canvas.view.viewLoc.x, loc.y + canvas.view.viewLoc.y, viewLoc.width, viewLoc.height);
        }
    }

    // Update the data range of the given axis.
    public void updateDataRange(String axis, float min, float max)
      throws YError
    {
        if (axis.equals("X")) {
            viewDataXMin = min;
            viewDataXMax = max;

            dataXStep = 0.0f;
            if (viewDataLoc.width > 0)
                dataXStep = (viewDataXMax - viewDataXMin) / viewDataLoc.width;

        } else if (axis.equals("Y")) {
            viewDataYMin = min;
            viewDataYMax = max;

            dataYStep = 0.0f;
            if (viewDataLoc.height > 0)
                dataYStep = (viewDataYMax - viewDataYMin) / viewDataLoc.height;
        } else {
	    throw new YError("Illegal axis type (" + axis + ")");
	}
    }

    // Note: getX and getY should probably be combined into a single
    // method. RKW 2000-05-03.

    // Convert mouse X coordinate (in pixels) to a String.
    // x is relative to this view's canvas
    public String getX(int x)
    {
        Rectangle loc = viewLocInCanvas;

        if (x < loc.x || x > (loc.x + loc.width)) {
            return "";
        }

        x = x - loc.x;

        float xstep = dataXStep;
        // x0 represent the value at the left side of that pixel x
        float x0 = (x - viewDataLoc.x) * xstep + viewDataXMin;

        if ((viewDataXType.toLowerCase()).equals("date")) {
            x0 = x0 * 1000.0f;
            Date date = new Date((long)x0);
            DateFormat format = DateFormat.getDateInstance(DateFormat.SHORT);
            Calendar cal = Calendar.getInstance();
            cal.setTime(date);
            String time = " " + cal.get(Calendar.HOUR_OF_DAY) + ":"
                             + cal.get(Calendar.MINUTE) + ":"
                             + cal.get(Calendar.SECOND);
            return (format.format(date) + time);
        } else {
            if (x0 > 0) {
                x0 = (int)(x0 * 1000.0f + 0.5f) / 1000.0f;
            } else {
                x0 = (int)(x0 * 1000.0f - 0.5f) / 1000.0f;
            }

            return ("" + x0);
        }
    }

    // Convert mouse Y coordinate (in pixels) to a String.
    // y is relative to this view's canvas
    public String getY(int y)
    {
        Rectangle loc = viewLocInCanvas;

        if (y < loc.y || y > (loc.y + loc.height)) {
            return "";
        }

        y = y - loc.y;

        float ystep = dataYStep;
        // y0 represent the value at the top side of that pixel
        float y0 = viewDataYMax - (y - viewDataLoc.y) * ystep;

        if ((viewDataYType.toLowerCase()).equals("date")) {
            y0 = y0 * 1000.0f;
            Date date = new Date((long)y0);
            DateFormat format = DateFormat.getDateInstance(DateFormat.SHORT);
            Calendar cal = Calendar.getInstance();
            cal.setTime(date);
            String time = " " + cal.get(Calendar.HOUR_OF_DAY) + ":"
                             + cal.get(Calendar.MINUTE) + ":"
                             + cal.get(Calendar.SECOND);
            return (format.format(date) + time);
        } else {
            if (y0 > 0) {
                y0 = (int)(y0 * 1000.0f + 0.5f) / 1000.0f;
            } else {
                y0 = (int)(y0 * 1000.0f - 0.5f) / 1000.0f;
            }

            return ("" + y0);
        }
    }

    // check whether or not the point (relative to this view's canvas) is within the view area
    public boolean inView(Point p)
    {
        Rectangle loc = viewLocInCanvas;

        //jsc.pn("view name" + viewName + " = p " + p.x + " " + p.y + "  loc " + loc.x + " " + loc.y + " " + loc.width + " " + loc.height + "  viewloc" + viewLoc.x + " " + viewLoc.y);
        if (p.x >= loc.x && p.y >= loc.y && p.x <= loc.x + loc.width && p.y <= loc.y + loc.height) {
            return true;
        } else {
            return false;
        }
    }

    // check whether or not the point (relative to this view's canvas) is
    // within the view's data area
    public boolean inViewDataArea(Point p)
    {
        Rectangle loc = viewLocInCanvas;

        int x0 = p.x - loc.x - viewDataLoc.x, y0 = p.y - loc.y - viewDataLoc.y;

        if (x0 >= 0 && y0 >= 0 && x0 <= viewDataLoc.width && y0 <= viewDataLoc.height) {
            return true;
        } else {
            return false;
        }
    }

    // ADD COMMENT -- what is this doing?
    public int translateX(int x, int mode)
    {
        return translateX(x, mode, 0);
    }

    // ADD COMMENT -- what is this doing?
    public int translateX(int x, int mode, int width)
    {
        Rectangle loc = viewLocInCanvas;

        if (mode == 1) { // adjust position x so it is within the range of data area of this view
            if (x < loc.x + viewDataLoc.x) {
                x = loc.x + viewDataLoc.x;
            } else if (x + width > loc.x + viewDataLoc.x + viewDataLoc.width) {
                x = loc.x + viewDataLoc.x + viewDataLoc.width - width;
            }
        } else if (mode == 2) { // adjust position x so it is relative to this view
            x = x - loc.x;
        }

        return x;
    }

    // ADD COMMENT -- what is this doing?
    public int translateY(int y, int mode)
    {
        return translateY(y, mode, 0);
    }

    // ADD COMMENT -- what is this doing?
    public int translateY(int y, int mode, int height)
    {
        Rectangle loc = viewLocInCanvas;

        if (mode == 1) { // adjust position y so it is within the range of data area of this view
            if (y < loc.y + viewDataLoc.y) {
                y = loc.y + viewDataLoc.y;
            } else if (y + height > loc.y + viewDataLoc.y + viewDataLoc.height) {
                y = loc.y + viewDataLoc.y + viewDataLoc.height - height;
            }
        } else if (mode == 2) { // adjust position y so it is relative to this view
            y = y - loc.y;
        }

        return y;
    }

    // Get this first cursor in this view or this view's pile.  Note that
    // this method should only be called on a base view.
    public DEViseCursor getFirstCursor() throws YError
    {
        if (pileBaseView != null) {
	    throw new YError("Calling getFirstCursor() on non-base view (" +
	      viewName + ")");
	}

        for (int i = 0; i < viewCursors.size(); i++) {
            DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(i);
            return cursor;
        }

        for (int i = 0; i < viewPiledViews.size(); i++) {
            DEViseView v = (DEViseView)viewPiledViews.elementAt(i);
            DEViseCursor cursor = v.getFirstCursor();
            if (cursor != null) {
                return cursor;
            }
        }

        return null;
    }
}
