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

// This class maintains the state of a given DEVise cursor in the
// JavaScreen.

// There is one instance of this class for each cursor in the
// session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.19  2000/05/12 20:43:56  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
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

    // x, y, width, height and cursorLoc seem to have pretty much the
    // same meaning.  RKW 2000-06-27.
    public int x, y, width, height; // relative to view
    private Rectangle cursorLoc = null; // relative to view?
    public Rectangle cursorLocInCanvas = null;

    public boolean isXMovable, isYMovable, isXResizable, isYResizable;

    private float gridXData, gridYData; // grid in data units

    public String name = null, viewname = null;
    public DEViseView parentView = null;

    public Image image = null;
    public Color color = null;
    public int cursorType = 0; // this is not really used yet -- RKW 2000-06-27

    public static final int GRID_TYPE_CENTER = 0;
    public static final int GRID_TYPE_EDGE = 1;
    private int gridType = 0;

    public static final int sideNone = -1; // not on a cursor, or cursor
                                           // cannot be moved or resized
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
      Rectangle rect, String move, String fixedSize, float gx, float gy,
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

        gridXData = gx;
        gridYData = gy;

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

        if (move == null) {
            isXMovable = false;
            isYMovable = false;
        } else {
            isXMovable = (move.indexOf("X") != -1);
            isYMovable = (move.indexOf("Y") != -1);
	}

        if (fixedSize == null) {
            isXResizable = false;
            isYResizable = false;
        } else {
            isXResizable = (fixedSize.indexOf("X") == -1);
            isYResizable = (fixedSize.indexOf("Y") == -1);
	}
    }

    public synchronized Rectangle getLocInCanvas()
    {
        return new Rectangle (parentView.viewLocInCanvas.x + x,
	  parentView.viewLocInCanvas.y + y, width, height);
    }


    // --------------------------------------------------------------------
    // position p is relative to the canvas of this cursor
    // see side* for return values
    public int inCursor(Point p)
    {
	int result = sideNone;

	boolean onCursor = false;
	boolean topEdge = false;
	boolean bottomEdge = false;
	boolean leftEdge = false;
	boolean rightEdge = false;

        Rectangle loc = cursorLocInCanvas;

        if (p.x < loc.x || p.y < loc.y || p.x > loc.x + loc.width - 1 ||
	  p.y > loc.y + loc.height - 1) {

	    // p is outside of the cursor.
	    onCursor = false;

        } else {

	    onCursor = true;

	    // Figure out how close to the edge to have to be to be on
	    // the edge rather than the middle (depends on the size of
	    // the cursor).
            int xEdgeTol = (int)(loc.width / 4 + 0.4f);
	    xEdgeTol = Math.min(Math.max(xEdgeTol, 1), 10);

	    int yEdgeTol = (int)(loc.height / 4 + 0.4f);
	    yEdgeTol = Math.min(Math.max(yEdgeTol, 1), 10);

	    //
	    // Figure out which edges of the cursor, if any, we're on
	    // (taking into account whether is moveable and resizeable
	    // in the appropriate directions).
	    //
	    if (isXMovable && isXResizable) {
	        if (p.x < loc.x + xEdgeTol) {
	            leftEdge = true;
	        } else if (p.x > loc.x + loc.width - xEdgeTol - 1) {
	            rightEdge = true;
	        }
	    }

	    if (isYMovable && isYResizable) {
	        if (p.y < loc.y + yEdgeTol) {
	            topEdge = true;
	        } else if (p.y > loc.y + loc.height - yEdgeTol - 1) {
	            bottomEdge = true;
	        }
	    }
        }

	//
	// Okay, now put everything together.
	//
	if (onCursor) {
	    if (topEdge) {
	        if (leftEdge) {
		    result = sideTopLeft;
		} else if (rightEdge) {
		    result = sideTopRight;
		} else {
		    result = sideTop;
		}
	    } else if (bottomEdge) {
	        if (leftEdge) {
		    result = sideBottomLeft;
		} else if (rightEdge) {
		    result = sideBottomRight;
		} else {
		    result = sideBottom;
		}
	    } else if (leftEdge) {
	        result = sideLeft;
	    } else if (rightEdge) {
	        result = sideRight;
	    } else {
	        result = sideMiddle;
	    }
	} else {
	    result = sideNone;
	}

        return result;
    }


    // --------------------------------------------------------------------
    // Adjust the postion and size of the cursor that is being resized
    // or moved.
    public synchronized void updateCursorLoc(int dx, int dy,
      int whichSide, boolean isLast)
    {
	Rectangle oldLoc = cursorLoc;

        x = cursorLoc.x;
        y = cursorLoc.y;
        width = cursorLoc.width;
        height = cursorLoc.height;

	switch (whichSide) {
	case sideNone:
	case sideMiddle:
	    x += dx;
	    y += dy;
	    break;

        case sideTopLeft:
	    x += dx;
	    width -= dx;
	    y += dy;
	    height -= dy;
	    break;

        case sideTop:
	    y += dy;
	    height -= dy;
	    break;

        case sideTopRight:
	    width += dx;
	    y += dy;
	    height -= dy;
	    break;

        case sideLeft:
	    x += dx;
	    width -= dx;
	    break;

        case sideRight:
	    width += dx;
	    break;

        case sideBottomLeft:
	    x += dx;
	    width -= dx;
	    height += dy;
	    break;

        case sideBottom:
	    height += dy;
	    break;

        case sideBottomRight:
	    width += dx;
	    height += dy;
	    break;

	default:
	    throw new YError("Illegal whichSide value: " + whichSide);
	}

	if (!isXMovable) {
	    x = oldLoc.x;
	    width = oldLoc.width;
	}

	if (!isYMovable) {
	    y = oldLoc.y;
	    height = oldLoc.height;
	}

	matchGrid();

        if (isLast) {
            cursorLoc = new Rectangle(x, y, width, height);
            cursorLocInCanvas = getLocInCanvas();
        }

    }


    // --------------------------------------------------------------------
    // Move the cursor to a location that matches the cursor grid, if any.
    // Note: this code is based on the DeviseCursor::MatchGrid() method
    // of DEVise itself.  RKW 2000-06-27.
    public void matchGrid()
    {

        double dataX0 = parentView.pixel2DataX(x);
	double dataY0 = parentView.pixel2DataY(y);
	double dataX1 = parentView.pixel2DataX(x + width - 1);
	double dataY1 = parentView.pixel2DataY(y + height - 1);

	if (isXMovable) {
	    if (gridXData > 0.0f) {
	        if (gridType == GRID_TYPE_CENTER) {
		    double dataXC = (dataX0 + dataX1) / 2.0;
		    double dataHt = dataX0 - dataX1;

		    int tmp = (int)Math.round(dataXC / gridXData);
		    dataXC = tmp * gridXData;

		    dataX0 = dataXC + (dataHt / 2.0);
		    dataX1 = dataXC - (dataHt / 2.0);
		} else if (gridType == GRID_TYPE_EDGE) {
		    int tmp = (int)Math.round(dataX0 / gridXData - 0.5);
		    dataX0 = (tmp + 0.5) * gridXData;
		    tmp = (int)Math.round(dataX1 / gridXData + 0.5);
		    dataX1 = (tmp - 0.5) * gridXData;
		} else {
		    throw new YError("Illegal grid type: " + gridType);
		}
	    }
	}

	if (isYMovable) {
	    if (gridYData > 0.0f) {
	        if (gridType == GRID_TYPE_CENTER) {
		    double dataYC = (dataY0 + dataY1) / 2.0;
		    double dataHt = dataY0 - dataY1;

		    int tmp = (int)Math.round(dataYC / gridYData);
		    dataYC = tmp * gridYData;

		    dataY0 = dataYC + (dataHt / 2.0);
		    dataY1 = dataYC - (dataHt / 2.0);
		} else if (gridType == GRID_TYPE_EDGE) {
		    int tmp = (int)Math.round(dataY0 / gridYData + 0.5);
		    dataY0 = (tmp - 0.5) * gridYData;
		    tmp = (int)Math.round(dataY1 / gridYData - 0.5);
		    dataY1 = (tmp + 0.5) * gridYData;
		} else {
		    throw new YError("Illegal grid type: " + gridType);
		}
	    }
	}

	x = parentView.data2PixelX(dataX0);
	y = parentView.data2PixelY(dataY0);
	width = Math.abs(parentView.data2PixelX(dataX1) -
	  parentView.data2PixelX(dataX0)) + 1;
	height = Math.abs(parentView.data2PixelY(dataY1) -
	  parentView.data2PixelY(dataY0)) + 1;
    }

    private int adjustToParentDataAreaX(int x)
    {
        return adjustToParentDataAreaX(x, 0);
    }

    private int adjustToParentDataAreaX(int x, int w)
    {
	int result = x;

        Rectangle loc = parentView.viewDataLoc;

        if (x < loc.x) {
            result = loc.x;
        } else if (x + w > loc.x + loc.width) {
            result = loc.x + loc.width - w;
        }

        return result;
    }

    private int adjustToParentDataAreaY(int y)
    {
        return adjustToParentDataAreaY(y, 0);
    }

    private int adjustToParentDataAreaY(int y, int h)
    {
	int result = y;

        Rectangle loc = parentView.viewDataLoc;

        if (y < loc.y) {
            result = loc.y;
        } else if (y + h > loc.y + loc.height) {
            result = loc.y + loc.height - h;
        }

        return result;
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
