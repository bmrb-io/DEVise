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

// There is one instance of this class for each cursor in the
// session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.18  2000/04/07 22:43:13  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.17  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.16  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.15  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.14  1999/10/12 00:08:43  hongyu
// *** empty log message ***
//
// Revision 1.13  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.12  1999/08/19 07:21:06  hongyu
// *** empty log message ***
//
// Revision 1.11  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.10  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.9  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCursor.java

import java.awt.*;

public class DEViseCursor
{
    private jsdevisec jsc = null;

    public int x, y, width, height;
    public Rectangle cursorLoc = null;
    public Rectangle cursorLocInCanvas = null;

    public boolean isXMovable, isYMovable, isXResizable, isYResizable;
    public float gridxx, gridyy;
    public int gridx, gridy;

    public String name = null, viewname = null;
    public DEViseView parentView = null;

    public Image image = null;
    public Color color = null;
    public int cursorType = 0;
    public int gridType = 0;

    public static final int sideNone = -1; // not on a cursor, or cannot
    					   // be moved or resized
    public static final int sideMiddle = 0;
    public static final int sideLeft = 1;
    public static final int sideRight = 2;
    public static final int sideTop = 3;
    public static final int sideBottom = 4;
    public static final int sideTopLeft = 5;
    public static final int sideBottomLeft = 6;
    public static final int sideTopRight = 7;
    public static final int sideBottomRight = 8;

    public DEViseCursor(jsdevisec panel, String cn, String vn,
      Rectangle rect, String move, String resize, float gx, float gy,
      int gtype, int ctype, Color c) throws YException
    {
        if (cn == null) {
            throw new YException("Invalid cursor name");
        }

        name = cn;
        viewname = vn;
        jsc = panel;

        parentView = jsc.jscreen.getView(viewname);

        if (parentView == null) {
            throw new YException("Invalid parent view for cursor");
        }

        gridxx = gx;
        gridyy = gy;

        cursorType = ctype;
        gridType = gtype;
        color = c;

        if (rect == null) {
            throw new YException("Invalid cursor location");
        }

        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;
        cursorLoc = new Rectangle(x, y, width, height);

        cursorLocInCanvas = getLocInCanvas();

        if (parentView.dataXStep != 0.0f) {
            gridx = (int)(gridxx / parentView.dataXStep);
        } else {
            gridx = 0;
        }

        if (parentView.dataYStep != 0.0f) {
            gridy = (int)(gridyy / parentView.dataYStep);
        } else {
            gridy = 0;
        }

        if (move == null) {
            isXMovable = false;
            isYMovable = false;
        } else {
            if (move.equals("X")) {
                isXMovable = true;
                isYMovable = false;
            } else if (move.equals("Y")) {
                isXMovable = false;
                isYMovable = true;
            } else if (move.equals("XY")) {
                isXMovable = true;
                isYMovable = true;
            } else {
                isXMovable = false;
                isYMovable = false;
            }
        }

        if (resize == null) {
            isXResizable = false;
            isYResizable = false;
        } else {
            if (resize.equals("X")) {
                isXResizable = false;
                isYResizable = true;
            } else if (resize.equals("Y")) {
                isXResizable = true;
                isYResizable = false;
            } else if (resize.equals("XY")) {
                isXResizable = false;
                isYResizable = false;
            } else if (resize.equals("none")) {
                isXResizable = true;
                isYResizable = true;
            } else {
                isXResizable = false;
                isYResizable = false;
            }
        }
    }

    public synchronized Rectangle getLocInCanvas()
    {
        return new Rectangle (parentView.viewLocInCanvas.x + x, parentView.viewLocInCanvas.y + y, width, height);
    }

    // position p is relative to the canvas of this cursor
    // see side* for return values
    public int inCursor(Point p)
    {
        Rectangle loc = cursorLocInCanvas;

        if (p.x < loc.x || p.y < loc.y || p.x > loc.x + loc.width - 1 || p.y > loc.y + loc.height - 1) {
            return sideNone;
        } else {
            int xedge = (int)(loc.width / 4 + 0.4f), yedge = (int)(loc.height / 4 + 0.4f);
            if (xedge < 1) {
                xedge = 1;
            } else if (xedge > 10) {
                xedge = 10;
            }

            if (yedge < 1) {
                yedge = 1;
            } else if (yedge > 10) {
                yedge = 10;
            }

            if (p.x >= loc.x && p.x < loc.x + xedge) {
                if (p.y >= loc.y && p.y < loc.y + yedge) { // left-top corner
                    if (isXResizable && isYResizable) {
                        return sideTopLeft;
                    } else {
                        if (isXResizable) { // left side
                            return sideLeft;
                        } else if (isYResizable) { // top side
                            return sideTop;
                        }
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // left-bottom corner
                    if (isXResizable && isYResizable) {
                        return sideBottomLeft;
                    } else {
                        if (isXResizable) { // left side
                            return sideLeft;
                        } else if (isYResizable) { // bottom side
                            return sideBottom;
                        }
                    }
                } else { // left side
                    if (isXResizable) {
                        return sideLeft;
                    }
                }
            } else if (p.x <= loc.x + loc.width && p.x > loc.x + loc.width - xedge) {
                if (p.y >= loc.y && p.y < loc.y + yedge) { // right-top corner
                    if (isXResizable && isYResizable) {
                        return sideTopRight;
                    } else {
                        if (isXResizable) { // right side
                            return sideRight;
                        } else if (isYResizable) { // top side
                            return sideTop;
                        }
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // right-bottom corner
                    if (isXResizable && isYResizable) {
                        return sideBottomRight;
                    } else {
                        if (isXResizable) { // right side
                            return sideRight;
                        } else if (isYResizable) { // bottom side
                            return sideBottom;
                        }
                    }
                } else { // right side
                    if (isXResizable) {
                        return sideRight;
                    }
                }
            } else {
                if (p.y >= loc.y && p.y < loc.y + yedge) { // top side
                    if (isYResizable) {
                        return sideTop;
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // bottom side
                    if (isYResizable) {
                        return sideBottom;
                    }
                }
            }

            if (isXMovable || isYMovable) {
                return sideMiddle;
            } else {
                return sideNone;
            }
        }
    }


    // Adjust the postion and size of the cursor that is being resizing
    // or moving.
    // state = 1 means move cursor; state = 2 means resize cursor.
    // return value is side*
    public synchronized int updateCursorLoc(int dx, int dy, int state,
      int whichSide, boolean isLast)
    {
        x = cursorLoc.x;
        y = cursorLoc.y;
        width = cursorLoc.width;
        height = cursorLoc.height;

        if (state == 1) { // move cursor
            if (isXMovable) {
                x = adjustToParentDataAreaX(x + dx, width);
            }

            if (isYMovable) {
                y = adjustToParentDataAreaY(y + dy, height);
            }
        } else if (state == 2) { // resize cursor
            int tmpx, tmpy, tx, ty, cx, cy;
            boolean isXChange = false, isYChange = false;

            switch (whichSide) {
	    case sideNone:
		throw new YError("Shouldn't get here with sideNone");

	    case sideMiddle:
		throw new YError("Shouldn't get here with sideMiddle");

            case sideLeft: // left side
                tmpx = x + width;
                cx = adjustToParentDataAreaX(x + dx);
                if (cx >= tmpx) {
                    x = tmpx;
                    width = cx - tmpx;

                    if (width == 0) {
                        width = 1;
                    }

                    if (isLast) {
                        whichSide = sideRight;
                    }
                } else {
                    x = cx;
                    width = tmpx - cx;
                }

                break;

            case sideRight: // right side
                tmpx = x + width;
                cx = adjustToParentDataAreaX(dx + tmpx);

                tx = x;
                if (tx >= cx) {
                    x = cx;
                    width = tx - cx;
                    if (width == 0) {
                        width = 1;
                    }

                    if (isLast) {
                        whichSide = sideLeft;
                    }
                } else {
                    width = cx - tx;
                }

                break;

            case sideTop: // top side
                tmpy = y + height;
                cy = adjustToParentDataAreaY(dy + y);

                if (cy >= tmpy) {
                    y = tmpy;
                    height = cy - tmpy;
                    if (height == 0) {
                        height = 1;
                    }

                    if (isLast) {
                        whichSide = sideBottom;
                    }
                } else {
                    y = cy;
                    height = tmpy - cy;
                }

                break;

            case sideBottom: // bottom side
                tmpy = y + height;
                cy = adjustToParentDataAreaY(dy + tmpy);

                ty = y;
                if (ty >= cy) {
                    y = cy;
                    height = ty - cy;
                    if (height == 0) {
                        height = 1;
                    }

                    if (isLast) {
                        whichSide = sideTop;
                    }
                } else {
                    height = cy - ty;
                }

                break;

            case sideTopLeft: // left top corner
                isXChange = false;
                isYChange = false;

                if (isXResizable) {
                    tmpx = x + width;
                    cx = adjustToParentDataAreaX(dx + x);

                    if (cx >= tmpx) {
                        x = tmpx;
                        width = cx - tmpx;
                        if (width == 0) {
                            width = 1;
                        }

                        isXChange = true;
                    } else {
                        x = cx;
                        width = tmpx - cx;
                    }
                }

                if (isYResizable) {
                    tmpy = y + height;
                    cy = adjustToParentDataAreaY(dy + y);

                    if (cy >= tmpy) {
                        y = tmpy;
                        height = cy - tmpy;
                        if (height == 0) {
                            height = 1;
                        }

                        isYChange = true;
                    } else {
                        y = cy;
                        height = tmpy - cy;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) {
                                whichSide = sideBottomRight;
                            }
                        } else {
                            if (isLast) {
                                whichSide = sideTopRight;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = sideBottomLeft;
                        }
                    }
                }

                break;

            case sideBottomLeft: // left bottom corner
                isXChange = false;
                isYChange = false;

                if (isXResizable) {
                    tmpx = x + width;
                    cx = adjustToParentDataAreaX(dx + x);

                    if (cx >= tmpx) {
                        x = tmpx;
                        width = cx - tmpx;
                        if (width == 0) {
                            width = 1;
                        }

                        isXChange = true;
                    } else {
                        x = cx;
                        width = tmpx - cx;
                    }
                }

                if (isYResizable) {
                    tmpy = y + height;
                    cy = adjustToParentDataAreaY(dy + tmpy);

                    ty = y;
                    if (ty >= cy) {
                        y = cy;
                        height = ty - cy;
                        if (height == 0) {
                            height = 1;
                        }

                        isYChange = true;
                    } else {
                        height = cy - ty;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) {
                                whichSide = sideTopRight;
                            }
                        } else {
                            if (isLast) {
                                whichSide = sideBottomRight;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = sideTopLeft;
                        }
                    }
                }

                break;

            case sideTopRight: // right top corner
                isXChange = false;
                isYChange = false;

                if (isXResizable) {
                    tmpx = x + width;
                    cx = adjustToParentDataAreaX(dx + tmpx);

                    tx = x;
                    if (tx >= cx) {
                        x = cx;
                        width = tx - cx;
                        if (width == 0) {
                            width = 1;
                        }

                        isXChange = true;
                    } else {
                        width = cx - tx;
                    }
                }

                if (isYResizable) {
                    tmpy = y + height;
                    cy = adjustToParentDataAreaY(dy + y);

                    if (cy >= tmpy) {
                        y = tmpy;
                        height = cy - tmpy;
                        if (height == 0) {
                            height = 1;
                        }

                        isYChange = true;
                    } else {
                        y = cy;
                        height = tmpy - cy;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) {
                                whichSide = sideBottomLeft;
                            }
                        } else {
                            if (isLast) {
                                whichSide = sideTopLeft;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = sideBottomRight;
                        }
                    }
                }

                break;

            case sideBottomRight: // right bottom corner
                isXChange = false;
                isYChange = false;

                if (isXResizable) {
                    tmpx = x + width;
                    cx = adjustToParentDataAreaX(dx + tmpx);

                    tx = x;
                    if (tx >= cx) {
                        x = cx;
                        width = tx - cx;
                        if (width == 0) {
                            width = 1;
                        }

                        isXChange = true;
                    } else {
                        width = cx - tx;
                    }
                }

                if (isYResizable) {
                    tmpy = y + height;
                    cy = adjustToParentDataAreaY(dy + tmpy);

                    ty = y;
                    if (ty >= cy) {
                        y = cy;
                        height = ty - cy;
                        if (height == 0) {
                            height = 1;
                        }

                        isYChange = true;
                    } else {
                        height = cy - ty;
                    }
                }

                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            if (isLast) {
                                whichSide = sideTopLeft;
                            }
                        } else {
                            if (isLast) {
                                whichSide = sideBottomLeft;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = sideTopRight;
                        }
                    }
                }

                break;

	    default:
		throw new YError("Illegal whichSide value: " + whichSide);
            }
        }

        if (isLast) {
            cursorLoc = new Rectangle(x, y, width, height);
            cursorLocInCanvas = getLocInCanvas();
        }

        return whichSide;
    }

    public int adjustToParentDataAreaX(int x)
    {
        return adjustToParentDataAreaX(x, 0);
    }

    public int adjustToParentDataAreaX(int x, int w)
    {
        Rectangle loc = parentView.viewDataLoc;

        if (x < loc.x) {
            x = loc.x;
        } else if (x + w > loc.x + loc.width) {
            x = loc.x + loc.width - w;
        }

        return x;
    }

    public int adjustToParentDataAreaY(int y)
    {
        return adjustToParentDataAreaY(y, 0);
    }

    public int adjustToParentDataAreaY(int y, int h)
    {
        Rectangle loc = parentView.viewDataLoc;

        if (y < loc.y) {
            y = loc.y;
        } else if (y + h > loc.y + loc.height) {
            y = loc.y + loc.height - h;
        }

        return y;
    }

    public boolean isSame(DEViseCursor cursor)
    {
        if (cursor != null) {
            if (cursor.name.equals(name) && cursor.x == x && cursor.y == y && cursor.width == width && cursor.height == height) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}
