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
    public double gridxx, gridyy;
    public int gridx, gridy;

    public String name = null, viewname = null;
    public DEViseView parentView = null;

    public Image image = null;
    public Color color = null;
    public int cursorType = 0;
    public int gridType = 0;

    public DEViseCursor(jsdevisec panel, String cn, String vn, Rectangle rect, String move, String resize, double gx, double gy, int gtype, int ctype, Color c) throws YException
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

        if (parentView.dataXStep != 0.0) {
            gridx = (int)(gridxx / parentView.dataXStep);
        } else {
            gridx = 0;
        }

        if (parentView.dataYStep != 0.0) {
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
    // -1: not within cursor or can not be moved or resized
    //  0: inside cursor
    //  1: left side
    //  2: right side
    //  3: top side
    //  4: bottom side
    //  5: left-top corner
    //  6: left-bottom corner
    //  7: right-top corner
    //  8: right-bottom corner
    public int inCursor(Point p)
    {
        Rectangle loc = cursorLocInCanvas;

        if (p.x < loc.x || p.y < loc.y || p.x > loc.x + loc.width - 1 || p.y > loc.y + loc.height - 1) {
            return -1;
        } else {
            int xedge = (int)(loc.width / 4 + 0.4), yedge = (int)(loc.height / 4 + 0.4);
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
                        return 5;
                    } else {
                        if (isXResizable) { // left side
                            return 1;
                        } else if (isYResizable) { // top side
                            return 3;
                        }
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // left-bottom corner
                    if (isXResizable && isYResizable) {
                        return 6;
                    } else {
                        if (isXResizable) { // left side
                            return 1;
                        } else if (isYResizable) { // bottom side
                            return 4;
                        }
                    }
                } else { // left side
                    if (isXResizable) {
                        return 1;
                    }
                }
            } else if (p.x <= loc.x + loc.width && p.x > loc.x + loc.width - xedge) {
                if (p.y >= loc.y && p.y < loc.y + yedge) { // right-top corner
                    if (isXResizable && isYResizable) {
                        return 7;
                    } else {
                        if (isXResizable) { // right side
                            return 2;
                        } else if (isYResizable) { // top side
                            return 3;
                        }
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // right-bottom corner
                    if (isXResizable && isYResizable) {
                        return 8;
                    } else {
                        if (isXResizable) { // right side
                            return 2;
                        } else if (isYResizable) { // bottom side
                            return 4;
                        }
                    }
                } else { // right side
                    if (isXResizable) {
                        return 2;
                    }
                }
            } else {
                if (p.y >= loc.y && p.y < loc.y + yedge) { // top side
                    if (isYResizable) {
                        return 3;
                    }
                } else if (p.y <= loc.y + loc.height && p.y > loc.y + loc.height - yedge) { // bottom side
                    if (isYResizable) {
                        return 4;
                    }
                }
            }

            if (isXMovable || isYMovable) {
                return 0;
            } else {
                return -1;
            }
        }
    }


    // adjust the postion and size of the cursor that is being resizing or moving
    public synchronized int updateCursorLoc(int dx, int dy, int state, int whichSide, boolean isLast)
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
            case 1: // left side
                tmpx = x + width;
                cx = adjustToParentDataAreaX(x + dx);
                if (cx >= tmpx) {
                    x = tmpx;
                    width = cx - tmpx;

                    if (width == 0) {
                        width = 1;
                    }

                    if (isLast) {
                        whichSide = 2;
                    }
                } else {
                    x = cx;
                    width = tmpx - cx;
                }

                break;
            case 2: // right side
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
                        whichSide = 1;
                    }
                } else {
                    width = cx - tx;
                }

                break;
            case 3: // top side
                tmpy = y + height;
                cy = adjustToParentDataAreaY(dy + y);

                if (cy >= tmpy) {
                    y = tmpy;
                    height = cy - tmpy;
                    if (height == 0) {
                        height = 1;
                    }

                    if (isLast) {
                        whichSide = 4;
                    }
                } else {
                    y = cy;
                    height = tmpy - cy;
                }

                break;
            case 4: // bottom side
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
                        whichSide = 3;
                    }
                } else {
                    height = cy - ty;
                }

                break;
            case 5: // left top corner
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
                                whichSide = 8;
                            }
                        } else {
                            if (isLast) {
                                whichSide = 7;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = 6;
                        }
                    }
                }

                break;
            case 6: // left bottom corner
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
                                whichSide = 7;
                            }
                        } else {
                            if (isLast) {
                                whichSide = 8;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = 5;
                        }
                    }
                }

                break;
            case 7: // right top corner
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
                                whichSide = 6;
                            }
                        } else {
                            if (isLast) {
                                whichSide = 5;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = 8;
                        }
                    }
                }

                break;
            case 8: // right bottom corner
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
                                whichSide = 5;
                            }
                        } else {
                            if (isLast) {
                                whichSide = 6;
                            }
                        }
                    } else {
                        if (isLast) {
                            whichSide = 7;
                        }
                    }
                }

                break;
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
