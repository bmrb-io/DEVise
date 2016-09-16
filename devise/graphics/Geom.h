/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.12.10.1  1997/05/21 20:39:38  weaver
  Changes for new ColorManager

  Revision 1.12  1996/11/13 16:56:06  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.11  1996/11/07 22:40:09  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.10  1996/10/28 15:55:37  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.9  1996/08/03 15:36:29  jussi
  Added line segment width.

  Revision 1.8  1996/07/20 18:47:57  jussi
  Added 3D line segment shape and renamed some 3D type names to
  be more general.

  Revision 1.7  1996/07/10 16:22:44  jussi
  Added copyright notice.

  Revision 1.6  1996/06/21 19:27:37  jussi
  Cleaned up and moved BLOCK data structures to Map3D.h.

  Revision 1.5  1996/06/15 07:07:19  yuc
  Add the following structures to the system: POINT_ (for 3D point),
  EDGE_ (for 3D object), and BLOCK (for a block object).

  Revision 1.4  1996/05/20 17:46:01  jussi
  Removed unnecessary code.

  Revision 1.3  1996/02/28 17:49:41  yuc
  Added 4 new types (for 3D shapes): POINT, EDGE, VERTEX, and BLOCK.

  Revision 1.2  1995/09/05 21:12:51  jussi
  Added/update CVS header.
*/

#ifndef Geom_h
#define Geom_h

#include <stdio.h>
#include <math.h>

#include "DeviseTypes.h"

#include "Color.h"

// ---------------------------------------------------------- 
// 2D data structures
// ---------------------------------------------------------- 

class Point {               // point in 2D space
public:
  Coord x, y;               // coordinates of point
};

class Segment {             // line segment in 2D space
public:
  Point pt[2];              // two ends of the segment
  Coord width;              // width of line segment
  PColorID color;        	// color of segment
};

class Plane {               // triangular plane in 2D space
public:
  Point pt[3];              // three corners of the triangle
  PColorID color;        	// color of triangle
  Coord dist;               // distance from camera (for sorting)
};

class Rectangle {           // rectangle in 2D space
public:
  Coord x, y;               // upper left corner unless otherwise noted
  Coord width, height;
  PColorID color;	    	// color of rectangle
};

// ---------------------------------------------------------- 
// 3D data structures
// ---------------------------------------------------------- 

class Point3D {             // point in 3D space
public:
  Coord x_, y_, z_;         // coordinates of point
};

//******************************************************************************
// class Geom
//******************************************************************************

class Geom {
public:
  /* Return true if pont (x,y) intersects with 
     rectangle (minX,minY) (maxX,maxY) */
  static Boolean PointRectIntersect(Coord x, Coord y,
				    Coord minX, Coord minY,
				    Coord maxX, Coord maxY) {
    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
  }

  /* Merge two rectangles into the minimum rectangle that contains
     both and place result into first */
  static void MergeRects(Coord &minX1, Coord &minY1,
			 Coord &maxX1, Coord &maxY1,
			 Coord minX2, Coord minY2,
			 Coord maxX2, Coord maxY2) {
    minX1 = MIN(minX1,minX2);
    minY1 = MIN(minY1,minY2);
    maxX1 = MAX(maxX1,maxX2);
    maxY1 = MAX(maxY1,maxY2);
  }

  /* Return true if point (x,y) intersects with line (x0,y0)(x1,y1).
     lineWidth = width of the line.  */
  static Boolean PointLineIntersect(Coord x, Coord y,
				    Coord x0, Coord y0,
				    Coord x1, Coord y1,
				    Coord lineWidth = .1);

  /* Return TRUE if rect1 (minX1,minY1)(maxX1,maxY1) intersects with
     rect2 (minX2,minY2),(maxX2,maxY2). */
  static Boolean RectRectIntersect(Coord minX1,Coord minY1,
				   Coord maxX1,Coord maxY1,
				   Coord minX2,Coord minY2, 
				   Coord maxX2,Coord maxY2);
	
  /* clip a line (x1,y1) (x2, y2) against a rectangle 
     (minX, minY) (maxX, maxY) and return the new coords of the line
     in (newx1, newy1) (newx2, newy2). Return true if line
     intersects with the rectangle. */
  static Boolean LineRectClip(Coord minX, Coord minY,
			      Coord maxX, Coord maxY,
			      Coord x1, Coord y1,
			      Coord x2, Coord y2,
			      Coord &newx1, Coord& newy1,
			      Coord &newx2, Coord &newy2);
	
  /* Return true if line (x1, y1) (x2,y2) intersects with
     rectangle (minX, minY), (maxX, maxY) */
  static Boolean LineRectIntersect(Coord minX, Coord minY,
				   Coord maxX, Coord maxY,
				   Coord x1, Coord y1,
				   Coord x2, Coord y2) {
    Coord tempx1, tempy1, tempx2, tempy2;
    Boolean result;
    result = LineRectClip(minX,minY,maxX,maxY,x1,y1,x2,y2,
			  tempx1,tempy1,tempx2,tempy2);
#ifdef DEBUG
    printf("LineRectIntersect rect(%f,%f),(%f,%f), line(%f,%f),(%f,%f) = %d\n",
	   minX, minY, maxX, maxY, x1, y1, x2, y2,result);
#endif
    return result;
  }

  /* Clip first rectangle against second, and strore the result
     in second. Return true if the two rectangles intersect. 
     If they don't intersect, the rectangles are not modified */
  static Boolean RectRectClip(Coord &minX1, Coord &minY1,
			      Coord &maxX1, Coord &maxY1,
			      Coord minX2, Coord minY2,
			      Coord maxX2, Coord maxY2);
};

//******************************************************************************
#endif
