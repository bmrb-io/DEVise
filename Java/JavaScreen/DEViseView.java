// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$
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
    public String viewTitle = null;
    // viewLoc is the location within parent, parent could be another view or the screen
    public Rectangle viewLoc = null;
    public double viewZ = 0.0;
    public int viewBg, viewFg;

    // viewDataLoc is the location relative to this view
    public Rectangle viewDataLoc = null;
    public double viewDataXMin = 0.0, viewDataXMax = 0.0;
    public double viewDataYMin = 0.0, viewDataYMax = 0.0;
    // data type could be "real" or "date" or "none"
    public String viewDataXType = null, viewDataYType = null;

    public DEViseView parentView = null;
    public DEViseView piledView = null;
    public Vector viewPiledViews = new Vector();
    public Vector viewChilds = new Vector();
    public Vector viewGDatas = new Vector();
    public Vector viewCursors = new Vector();

    public boolean isRubberBand, isCursorMove, isDrillDown, isKey;
    public double dataXStep, dataYStep;
    public double gridx, gridy;

    // whichSide = 0 indicate inside current cursor
    // whichSide = 1 indicate at left side of current cursor
    // whichSide = 2 indicate at right side of current cursor
    // whichSide = 3 indicate at top side of current cursor
    // whichSide = 4 indicate at bottom side of current cursor
    // whichSide = 5 indicate at left-top corner of current cursor
    // whichSide = 6 indicate at left-bottom corner of current cursor
    // whichSide = 7 indicate at right-top corner of current cursor
    // whichSide = 8 indicate at right-bottom corner of current cursor
    public int whichChild, whichCursor, whichSide;
    public Rectangle oldRect = null;
    public boolean isFirstTime = true;

    public DEViseView(jsdevisec panel, String pn, String name, String piledname, String title, Rectangle loc, double Z, int bg, int fg, Rectangle dl, String xt, String yt, double gx, double gy, int rb, int cm, int dd, int ky)
    {
        jsc = panel;

        viewName = name;
        viewTitle = title;
        viewLoc = loc;
        viewZ = Z;
        viewBg = bg;
        viewFg = fg;

        viewDataLoc = dl;
        viewDataXType = xt;
        viewDataYType = yt;

        parentView = jsc.jscreen.getView(pn);
        piledView = jsc.jscreen.getView(piledname);

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

    public String getCurlyName()
    {
        return curlyName;
    }

    public DEViseCanvas getCanvas()
    {
        if (canvas == null) {
            if (parentView == null) {
                if (piledView == null) {
                    return null;
                } else {
                    return piledView.canvas;
                }
            } else {
                return parentView.canvas;
            }
        } else {
            return canvas;
        }
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
        if (view == null)
            return;

        viewChilds.addElement(view);
    }

    public void addPile(DEViseView view)
    {
        if (view != null) {
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
                    if (piledView != null) {
                        piledView.viewCursors.removeElement(c);
                    }

                    cursor.parentView = this;

                    if (dataXStep != 0.0) {
                        cursor.gridx = (int)(cursor.gridxx / dataXStep);
                    } else {
                        cursor.gridx = 0;
                    }

                    if (dataYStep != 0.0) {
                        cursor.gridy = (int)(cursor.gridyy / dataYStep);
                    } else {
                        cursor.gridy = 0;
                    }

                    cursor.image = null;
                    viewCursors.addElement(cursor);
                    if (piledView != null) {
                        piledView.viewCursors.addElement(cursor);
                    }

                    return true;
                }
            }
        }

        cursor.parentView = this;

        if (dataXStep != 0.0) {
            cursor.gridx = (int)(cursor.gridxx / dataXStep);
        } else {
            cursor.gridx = 0;
        }

        if (dataYStep != 0.0) {
            cursor.gridy = (int)(cursor.gridyy / dataYStep);
        } else {
            cursor.gridy = 0;
        }

        viewCursors.addElement(cursor);
        if (piledView != null) {
            piledView.viewCursors.addElement(cursor);
        }

        return true;
    }

    public void addGData(DEViseGData gdata)
    {
        if (gdata == null)
            return;

        viewGDatas.addElement(gdata);
        gdata.parentView = this;
        if (piledView != null) {
            piledView.viewGDatas.addElement(gdata);
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
            if (piledView != null) {
                piledView.viewCursors.removeElement(cursor);
            }
        }
    }

    public boolean removeCursor(String name)
    {
        if (name != null) {
            for (int i = 0; i < viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(i);
                if (cursor.name.equals(name)) {
                    viewCursors.removeElement(cursor);
                    if (piledView != null) {
                        piledView.viewCursors.removeElement(cursor);
                    }

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
            if (piledView != null) {
                piledView.viewGDatas.removeElement(gdata);
            }
        }
    }

    public void removeAllGData()
    {
        if (piledView != null) {
            for (int i = 0; i < viewGDatas.size(); i++) {
                DEViseGData gdata = (DEViseGData)viewGDatas.elementAt(i);
                piledView.viewGDatas.removeElement(gdata);
            }
        }

        viewGDatas.removeAllElements();
    }

    public void removeAllCursor()
    {
        if (piledView != null) {
            for (int i = 0; i < viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(i);
                piledView.viewCursors.removeElement(cursor);
            }
        }

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

    public Rectangle getBoundsInScreen()
    {
        if (parentView != null) {
            Rectangle lc = parentView.getBoundsInScreen();
            return new Rectangle(viewLoc.x + lc.x, viewLoc.y + lc.y, viewLoc.width, viewLoc.height);
        } else {
            return new Rectangle(viewLoc.x + DEViseGlobals.screenEdge.width, viewLoc.y + DEViseGlobals.screenEdge.height, viewLoc.width, viewLoc.height);
        }
    }

    // loc is relative to this view's parentView(if null, then loc is relative to screen)
    public void updateLoc(Rectangle loc)
    {
        if (loc == null)
            return;

        viewLoc.x = loc.x;
        viewLoc.y = loc.y;
        viewLoc.width = loc.width;
        viewLoc.height = loc.height;

        for (int i = 0; i < viewPiledViews.size(); i++) {
            DEViseView view = (DEViseView)viewPiledViews.elementAt(i);
            if (view != null) {
                view.viewLoc.x = viewLoc.x;
                view.viewLoc.y = viewLoc.y;
                view.viewLoc.width = viewLoc.width;
                view.viewLoc.height = viewLoc.height;
            }
        }
    }

    public void updateDataRange(String axis, double min, double max)
    {
        if (axis.equals("X")) {
            viewDataXMin = min;
            viewDataXMax = max;

            dataXStep = 0.0;
            if (viewDataLoc.width > 0)
                dataXStep = (viewDataXMax - viewDataXMin) / viewDataLoc.width;

        } else if (axis.equals("Y")) {
            viewDataYMin = min;
            viewDataYMax = max;

            dataYStep = 0.0;
            if (viewDataLoc.height > 0)
                dataYStep = (viewDataYMax - viewDataYMin) / viewDataLoc.height;
        }
    }

    // x is relative to this view's parentView(if null, then x is relative to itself)
    public String getX(int x)
    {
        //if ((viewDataXType.toLowerCase()).equals("none"))
        //    return "";

        if (parentView != null) {
            x = x - viewLoc.x;
        }

        if (x < viewDataLoc.x) {
            return "";
        }

        if (x > (viewDataLoc.x + viewDataLoc.width)) {
            return "";
        }

        //double xstep = 0.0;
        double xstep = dataXStep;
        //if (viewDataLoc.width > 0)
        //    xstep = (viewDataXMax - viewDataXMin) / viewDataLoc.width;

        // x0 represent the value at the left side of that pixel x
        double x0 = (x - viewDataLoc.x) * xstep + viewDataXMin;

        if ((viewDataXType.toLowerCase()).equals("date")) {
            x0 = x0 * 1000.0;
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
                x0 = (int)(x0 * 1000.0 + 0.5) / 1000.0;
            } else {
                x0 = (int)(x0 * 1000.0 - 0.5) / 1000.0;
            }

            return ("" + x0);
        }
    }

    // y is relative to this view's parentView(if null, then y is relative to itself)
    public String getY(int y)
    {
        //if ((viewDataYType.toLowerCase()).equals("none"))
        //    return "";

        if (parentView != null) {
            y = y - viewLoc.y;
        }

        if (y < viewDataLoc.y) {
            return "";
        }

        if (y > (viewDataLoc.y + viewDataLoc.height)) {
            return "";
        }

        //double ystep = 0.0;
        double ystep = dataYStep;
        //if (viewDataLoc.height > 0)
        //    ystep = (viewDataYMax - viewDataYMin) / viewDataLoc.height;

        // y0 represent the value at the top side of that pixel
        double y0 = viewDataYMax - (y - viewDataLoc.y) * ystep;

        if ((viewDataYType.toLowerCase()).equals("date")) {
            y0 = y0 * 1000.0;
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
                y0 = (int)(y0 * 1000.0 + 0.5) / 1000.0;
            } else {
                y0 = (int)(y0 * 1000.0 - 0.5) / 1000.0;
            }

            return ("" + y0);
        }
    }

    // find out where the point is located
    // the position p is relative to this view's parentView(if null, then p is relative to itself)
    // we assume the cursor of a view is complete within the data area of that view
    // we assume the cursor of a view and the child view of a view is not overlap
    // we assume only one layer of child view, ie, child view will not have child view
    // 0: not within the data area but within the view
    // 1: within the data area, but not within child view or cursor
    // 2: inside the cursor
    // 3: within the range of the edges of one cursor
    // 4: inside a child view, but not within its data area
    // 5: inside a child view and within its data area, but not within any cursor of it
    // 6: inside a child view and inside one of its cursor
    // 7: inside a child view and within the range of the edges of one of its cursor
    public int checkPos(Point p)
    {
        // initialize data
        whichChild = -1;
        whichCursor = -1;
        whichSide = -1;

        if (!isWithinView(p))
            return -1;

        if (!isWithinDataArea(p)) {
            return 0;
        }

        // the viewChilds of child view will always has 0 size
        if (viewChilds.size() > 0) {
            for (int i = 0; i < viewChilds.size(); i++) {
                DEViseView child = getChild(i);
                int status = child.checkPos(p);
                if (status < 0) {
                    continue;
                } else {
                    whichChild = i;
                    whichCursor = child.whichCursor;
                    whichSide = child.whichSide;
                    return status + 4;
                }
            }
        }

        if (viewCursors.size() > 0) {
            for (int i = 0; i < viewCursors.size(); i++) {
                DEViseCursor cursor = getCursor(i);
                int status = cursor.checkPos(p);
                if (status < 0) {
                    continue;
                } else {
                    whichCursor = i;
                    whichSide = status;
                    if (whichSide == 0) {
                        return 2;
                    } else {
                        return 3;
                    }
                }
            }
        }

        return 1;
    }

    // check whether or not the point is within the view area
    // the position p is relative to this view's parentView(if null, then p is relative to itself)
    private boolean isWithinView(Point p)
    {
        int x0 = 0, y0 = 0;
        if (parentView != null) {
            x0 = viewLoc.x;
            y0 = viewLoc.y;
        }

        if (p.x < x0 || p.y < y0 || p.x > x0 + viewLoc.width - 1 || p.y > y0 + viewLoc.height - 1) {
            return false;
        } else {
            return true;
        }
    }

    // check whether or not the point is within the data area,
    // the position p is relative to this view's parentView(if null, then p is relative to itself)
    private boolean isWithinDataArea(Point p)
    {
        int x0 = 0, y0 = 0;
        if (parentView != null) {
            x0 = viewLoc.x;
            y0 = viewLoc.y;
        }

        if (p.x < x0 + viewDataLoc.x || p.y < y0 + viewDataLoc.y || p.x > x0 + viewDataLoc.width + viewDataLoc.x - 1 || p.y > y0 + viewDataLoc.y + viewDataLoc.height - 1)
            return false;
        else
            return true;
    }

    // adjust position x so it is within the range of data area of this view
    // x is relative to this view's parentView(if null, then x is relative to itself)
    public int adjustPosX(int x)
    {
        return adjustPosX(x, 0);
    }

    // we assume width is less than the width of the data area
    public int adjustPosX(int x, int width)
    {
        int x0 = 0;
        if (parentView != null) {
            x0 = viewLoc.x;
        }

        if (x < x0 + viewDataLoc.x)
            x = x0 + viewDataLoc.x;

        if ((x + width) > (x0 + viewDataLoc.x + viewDataLoc.width))
            x = x0 + viewDataLoc.x + viewDataLoc.width - width;

        return x;
    }

    // adjust position y so it is within the range of data area of this view
    // y is relative to this view's parentView(if null, then y is relative to itself)
    public int adjustPosY(int y)
    {
        return adjustPosY(y, 0);
    }

    // we assume height is less than the height of the data area
    public int adjustPosY(int y, int height)
    {
        int y0 = 0;
        if (parentView != null) {
            y0 = viewLoc.y;
        }

        if (y < y0 + viewDataLoc.y)
            y = y0 + viewDataLoc.y;

        if ((y + height) > (y0 + viewDataLoc.y + viewDataLoc.height))
            y = y0 + viewDataLoc.y + viewDataLoc.height - height;

        return y;
    }

    // convert position relative to parentView to be relative to itself
    // x, y is relative to parentView
    public int xtome(int x)
    {
        if (parentView != null) {
            return x - viewLoc.x;
        } else {
            return x;
        }
    }

    public int ytome(int y)
    {
        if (parentView != null) {
            return y - viewLoc.y;
        } else {
            return y;
        }
    }

    // convert position relative to this view to be relative to its parentView
    // x, y is relative to this view
    public int xtoparent(int x)
    {
        if (parentView != null) {
            return x + viewLoc.x;
        } else {
            return x;
        }
    }

    public int ytoparent(int y)
    {
        if (parentView != null) {
            return y + viewLoc.y;
        } else {
            return y;
        }
    }

    // adjust the postion and size of the cursor that is being resizing or moving
    public void updateCursorLoc(int which, int state, int dx, int dy, boolean isLast)
    {
        if (which < 0 || which >= viewCursors.size())
            return;

        DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(which);

        if (oldRect == null) {
            oldRect = new Rectangle(cursor.x, cursor.y, cursor.width, cursor.height);
        }

        cursor.x = oldRect.x;
        cursor.y = oldRect.y;
        cursor.width = oldRect.width;
        cursor.height = oldRect.height;

        int x0 = 0, y0 = 0;
        if (parentView != null) {
            x0 = viewLoc.x;
            y0 = viewLoc.y;
        }

        if (state == 1) { // move cursor
            if (cursor.isXMovable) {
                cursor.x = adjustPosX(x0 + dx + cursor.x, cursor.width) - x0;
            }

            if (cursor.isYMovable) {
                cursor.y = adjustPosY(y0 + dy + cursor.y, cursor.height) - y0;
            }
        } else if (state == 2) { // resize cursor
            int tmpx, tmpy, tx, ty, cx, cy;
            boolean isXChange = false, isYChange = false;

            switch (whichSide) {
            case 1: // left side
                if (!cursor.isXResizable) {
                    whichSide = -1;
                    break;
                }

                tmpx = cursor.x + cursor.width;
                cx = adjustPosX(x0 + dx + cursor.x) - x0;
                //cx = adjustPosX(x0 + dx + cursor.x, cursor.width);

                if (cx >= tmpx) {
                    cursor.x = tmpx;
                    cursor.width = cx - tmpx;

                    if (cursor.width == 0) {
                        cursor.width = 1;
                    }

                    if (isLast) whichSide = 2;
                } else {
                    cursor.x = cx;
                    cursor.width = tmpx - cx;
                }

                break;
            case 2: // right side
                if (!cursor.isXResizable) {
                    whichSide = -1;
                    break;
                }

                tmpx = cursor.x + cursor.width;
                cx = adjustPosX(x0 + dx + tmpx) - x0;

                tx = cursor.x;
                if (tx >= cx) {
                    cursor.x = cx;
                    cursor.width = tx - cx;
                    if (cursor.width == 0) {
                        cursor.width = 1;
                    }

                    if (isLast) whichSide = 1;
                } else {
                    cursor.width = cx - tx;
                }

                break;
            case 3: // top side
                if (!cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                tmpy = cursor.y + cursor.height;
                //cy = adjustPosY(y0 + dy + cursor.y, cursor.height) - y0;
                cy = adjustPosY(y0 + dy + cursor.y) - y0;

                if (cy >= tmpy) {
                    cursor.y = tmpy;
                    cursor.height = cy - tmpy;
                    if (cursor.height == 0) {
                        cursor.height = 1;
                    }

                    if (isLast) whichSide = 4;
                } else {
                    cursor.y = cy;
                    cursor.height = tmpy - cy;
                }

                break;
            case 4: // bottom side
                if (!cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                tmpy = cursor.y + cursor.height;
                cy = adjustPosY(y0 + dy + tmpy) - y0;

                ty = cursor.y;
                if (ty >= cy) {
                    cursor.y = cy;
                    cursor.height = ty - cy;
                    if (cursor.height == 0) {
                        cursor.height = 1;
                    }

                    if (isLast) whichSide = 3;
                } else {
                    cursor.height = cy - ty;
                }

                break;
            case 5: // left top corner
                isXChange = false;
                isYChange = false;
                if (!cursor.isXResizable || !cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                if (cursor.isXResizable) {
                    tmpx = cursor.x + cursor.width;
                    //cx = adjustPosX(x0 + dx + cursor.x, cursor.width) - x0;
                    cx = adjustPosX(x0 + dx + cursor.x) - x0;

                    if (cx >= tmpx) {
                        cursor.x = tmpx;
                        cursor.width = cx - tmpx;
                        if (cursor.width == 0) {
                            cursor.width = 1;
                        }

                        isXChange = true;
                    } else {
                        cursor.x = cx;
                        cursor.width = tmpx - cx;
                    }
                }

                if (cursor.isYResizable) {
                    tmpy = cursor.y + cursor.height;
                    //cy = adjustPosY(y0 + dy + cursor.y, cursor.height) - y0;
                    cy = adjustPosY(y0 + dy + cursor.y) - y0;

                    if (cy >= tmpy) {
                        cursor.y = tmpy;
                        cursor.height = cy - tmpy;
                        if (cursor.height == 0) {
                            cursor.height = 1;
                        }

                        isYChange = true;
                    } else {
                        cursor.y = cy;
                        cursor.height = tmpy - cy;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) whichSide = 8;
                        } else {
                            if (isLast) whichSide = 7;
                        }
                    } else {
                        if (isLast) whichSide = 6;
                    }
                }

                break;
            case 6: // left bottom corner
                isXChange = false;
                isYChange = false;
                if (!cursor.isXResizable || !cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                if (cursor.isXResizable) {
                    tmpx = cursor.x + cursor.width;
                    //cx = adjustPosX(x0 + dx + cursor.x, cursor.width) - x0;
                    cx = adjustPosX(x0 + dx + cursor.x) - x0;

                    if (cx >= tmpx) {
                        cursor.x = tmpx;
                        cursor.width = cx - tmpx;
                        if (cursor.width == 0) {
                            cursor.width = 1;
                        }

                        isXChange = true;
                    } else {
                        cursor.x = cx;
                        cursor.width = tmpx - cx;
                    }
                }

                if (cursor.isYResizable) {
                    tmpy = cursor.y + cursor.height;
                    cy = adjustPosY(y0 + dy + tmpy) - y0;

                    ty = cursor.y;
                    if (ty >= cy) {
                        cursor.y = cy;
                        cursor.height = ty - cy;
                        if (cursor.height == 0) {
                            cursor.height = 1;
                        }

                        isYChange = true;
                    } else {
                        cursor.height = cy - ty;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) whichSide = 7;
                        } else {
                            if (isLast) whichSide = 8;
                        }
                    } else {
                        if (isLast) whichSide = 5;
                    }
                }

                break;
            case 7: // right top corner
                isXChange = false;
                isYChange = false;
                if (!cursor.isXResizable || !cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                if (cursor.isXResizable) {
                    tmpx = cursor.x + cursor.width;
                    cx = adjustPosX(x0 + dx + tmpx) - x0;

                    tx = cursor.x;
                    if (tx >= cx) {
                        cursor.x = cx;
                        cursor.width = tx - cx;
                        if (cursor.width == 0) {
                            cursor.width = 1;
                        }

                        isXChange = true;
                    } else {
                        cursor.width = cx - tx;
                    }
                }

                if (cursor.isYResizable) {
                    tmpy = cursor.y + cursor.height;
                    //cy = adjustPosY(y0 + dy + cursor.y, cursor.height) - y0;
                    cy = adjustPosY(y0 + dy + cursor.y) - y0;

                    if (cy >= tmpy) {
                        cursor.y = tmpy;
                        cursor.height = cy - tmpy;
                        if (cursor.height == 0) {
                            cursor.height = 1;
                        }

                        isYChange = true;
                    } else {
                        cursor.y = cy;
                        cursor.height = tmpy - cy;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) whichSide = 6;
                        } else {
                            if (isLast) whichSide = 5;
                        }
                    } else {
                        if (isLast) whichSide = 8;
                    }
                }

                break;
            case 8: // right bottom corner
                isXChange = false;
                isYChange = false;
                if (!cursor.isXResizable || !cursor.isYResizable) {
                    whichSide = -1;
                    break;
                }

                if (cursor.isXResizable) {
                    tmpx = cursor.x + cursor.width;
                    cx = adjustPosX(x0 + dx + tmpx) - x0;

                    tx = cursor.x;
                    if (tx >= cx) {
                        cursor.x = cx;
                        cursor.width = tx - cx;
                        if (cursor.width == 0) {
                            cursor.width = 1;
                        }

                        isXChange = true;
                    } else {
                        cursor.width = cx - tx;
                    }
                }

                if (cursor.isYResizable) {
                    tmpy = cursor.y + cursor.height;
                    cy = adjustPosY(y0 + dy + tmpy) - y0;

                    ty = cursor.y;
                    if (ty >= cy) {
                        cursor.y = cy;
                        cursor.height = ty - cy;
                        if (cursor.height == 0) {
                            cursor.height = 1;
                        }

                        isYChange = true;
                    } else {
                        cursor.height = cy - ty;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) whichSide = 5;
                        } else {
                            if (isLast) whichSide = 6;
                        }
                    } else {
                        if (isLast) whichSide = 7;
                    }
                }

                break;
            }
        }

        if (isLast) {
            oldRect = null;
        }
    }

}
