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
    public int x, y, width, height;
    public boolean isXMovable, isYMovable, isXResizable, isYResizable;
    public double gridxx, gridyy;
    public int gridx, gridy;
    public String name = null, viewname = null;
    public Image image = null;
    public DEViseView parentView = null;

    public DEViseCursor(String cn, String vn, Rectangle rect, String move, String resize, double gx, double gy) throws YException
    {
        if (cn == null || vn == null)
            throw new YException("Invalid view name or cursor name");

        viewname = vn;
        name = cn;

        gridxx = gx;
        gridyy = gy;

        if (rect == null)
            throw new YException("Invalid cursor location");

        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;

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

    // position p is relative to the parent of this cursor
    public int checkPos(Point p0)
    {
        Point p = null;
        if (parentView.parentView != null) {
            p = new Point(p0.x - parentView.viewLoc.x, p0.y - parentView.viewLoc.y);
        } else {
            p = p0;
        }

        if (p.x < x || p.y < y || p.x > x + width - 1 || p.y > y + height - 1) {
            return -1;
        } else {
            int edge = 4;

            if (p.x >= x && p.x < x + edge) {
                if (p.y >= y && p.y < y + edge) { // left-top corner
                    if (isXResizable && isYResizable) {
                        return 5;
                    } else {
                        if (isXResizable) {
                            return 1;
                        } else if (isYResizable) {
                            return 3;
                        } else {
                            return -1;
                        }
                    }
                } else if (p.y < y + height && p.y >= y + height - edge) { // left-bottom corner
                    if (isXResizable && isYResizable) {
                        return 6;
                    } else {
                        if (isXResizable) {
                            return 1;
                        } else if (isYResizable) {
                            return 4;
                        } else {
                            return -1;
                        }
                    }
                } else { // left side
                    if (isXResizable) {
                        return 1;
                    } else {
                        return -1;
                    }
                }
            } else if (p.x < x + width && p.x >= x + width - edge) {
                if (p.y >= y && p.y < y + edge) { // right-top corner
                    if (isXResizable && isYResizable) {
                        return 7;
                    } else {
                        if (isXResizable) {
                            return 2;
                        } else if (isYResizable) {
                            return 3;
                        } else {
                            return -1;
                        }
                    }
                } else if (p.y < y + height && p.y >= y + height - edge) { // right-bottom corner
                    if (isXResizable && isYResizable) {
                        return 8;
                    } else {
                        if (isXResizable) {
                            return 2;
                        } else if (isYResizable) {
                            return 4;
                        } else {
                            return -1;
                        }
                    }
                } else { // right side
                    if (isXResizable) {
                        return 2;
                    } else {
                        return -1;
                    }
                }
            } else {
                if (p.y >= y && p.y < y + edge) { // top side
                    if (isYResizable) {
                        return 3;
                    } else {
                        return -1;
                    }
                } else if (p.y < y + height && p.y >= y + height - edge) { // bottom side
                    if (isYResizable) {
                        return 4;
                    } else {
                        return -1;
                    }
                } else { // inside cursor
                    if (isXMovable || isYMovable) {
                        return 0;
                    } else {
                        return -1;
                    }
                }
            }
        }
    }

    // get the location of this cursor in its parentView
    public Rectangle getLoc()
    {
        return new Rectangle(x, y, width, height);
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
