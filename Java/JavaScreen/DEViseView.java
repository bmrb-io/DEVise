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

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
    public float viewZ = 0.0f;
    public int viewDimension = 0;
    public int viewBg, viewFg;

    // viewDataLoc is the location relative to this view
    public Rectangle viewDataLoc = null;
    public float viewDataXMin = 0.0f, viewDataXMax = 0.0f;
    public float viewDataYMin = 0.0f, viewDataYMax = 0.0f;
    // data type could be "real" or "date" or "none"
    public String viewDataXType = null, viewDataYType = null;

    public String viewTitle = null;
    public int viewDTX, viewDTY;
    public Font viewDTFont = null;

    public DEViseView parentView = null;
    public DEViseView piledView = null;
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
        //viewDataLoc.x += DEViseGlobals.screenEdge.width;
        //viewDataLoc.y += DEViseGlobals.
        viewDataXType = xt;
        viewDataYType = yt;

        parentView = jsc.jscreen.getView(pn);
        piledView = jsc.jscreen.getView(piledname);

        viewLocInCanvas = getLocInCanvas();

        gridx = gx;
        gridy = gy;

        if (rb == 1) {
            isRubberBand = true;
        } else {
            isRubberBand = false;
        }

        if (cm == 1) {
            isCursorMove = true;
        } else {
            isCursorMove = false;
        }

        if (dd == 1) {
            isDrillDown = true;
        } else {
            isDrillDown = false;
        }

        if (ky == 1) {
            isKey = true;
        } else {
            isKey = false;
        }

        curlyName = "{" + viewName + "}";
    }

    protected void finalize() {
    	if (_debug) {
            System.out.println("DEViseView(" + viewName + ").finalize()");
        }
	removeAllGData();
    }

    public String getCurlyName()
    {
        return curlyName;
    }

    public DEViseView getChild(int idx)
    {
        if (idx < 0 || idx >= viewChilds.size())
            return null;

        return (DEViseView)viewChilds.elementAt(idx);
    }

    public DEViseCursor getCursor(int idx)
    {
        if (idx < 0 || idx >= viewCursors.size())
            return null;

        return (DEViseCursor)viewCursors.elementAt(idx);
    }

    public DEViseGData getGData(int idx)
    {
        if (idx < 0 || idx >= viewGDatas.size())
            return null;

        return (DEViseGData)viewGDatas.elementAt(idx);
    }

    public void addChild(DEViseView view)
    {
        if (view != null) {
            viewChilds.addElement(view);
        }
    }

    public void addPile(DEViseView view)
    {
        if (view != null) {
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

    public boolean addCursor(DEViseCursor cursor)
    {
        if (cursor == null)
            return false;

        int i = 0;
        for (i = 0; i < viewCursors.size(); i++) {
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

    public void addGData(DEViseGData gdata)
    {
        if (gdata != null) {
            viewGDatas.addElement(gdata);
        }
    }

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

    public void removePile(DEViseView view)
    {
        if (view != null) {
            viewPiledViews.removeElement(view);
        }
    }

    public void removeCursor(DEViseCursor cursor)
    {
        if (cursor != null) {
            viewCursors.removeElement(cursor);
        }
    }

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

    public void removeGData(DEViseGData gdata)
    {
        if (gdata != null) {
            viewGDatas.removeElement(gdata);
        }
    }

    public void removeAllGData()
    {
        if (_debug) {
            System.out.println("DEViseView(" + viewName +
	      ").removeAllGData()");
        }
        viewGDatas = new Vector();
    }

    public void removeAllCursor()
    {
        viewCursors.removeAllElements();
    }

    public void removeAllChild()
    {
        if (canvas != null) {
            if (canvas.activeView != this) {
                canvas.activeView = null;
            }
        }

        viewChilds.removeAllElements();
    }

    public void removeAllPile()
    {
        viewPiledViews.removeAllElements();
    }

    public DEViseCanvas getCanvas()
    {
        if (canvas == null) {
            if (parentView == null) {
                if (piledView == null) { // this is a top view, but the canvas for it has not yet been set
                    return null;
                } else {
                    return piledView.getCanvas();
                }
            } else {
                return parentView.getCanvas();
            }
        } else {
            return canvas;
        }
    }

    public Rectangle getLocInCanvas()
    {
        if (canvas == null) {
            Rectangle loc = null;

            if (parentView == null) {
                if (piledView == null) {
                    // this is a top view, but the canvas for it has not yet been set
                    return new Rectangle(0, 0, viewLoc.width, viewLoc.height);
                } else {
                    loc = piledView.getLocInCanvas();
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

    public void updateDataRange(String axis, float min, float max)
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
        }
    }

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

    // check whether or not the point (relative to this view's canvas) is within the view area
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

    public int translateX(int x, int mode)
    {
        return translateX(x, mode, 0);
    }

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

    public int translateY(int y, int mode)
    {
        return translateY(y, mode, 0);
    }

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

    public DEViseCursor getFirstCursor()
    {
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
