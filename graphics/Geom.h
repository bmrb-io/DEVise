/*
  $Id$

  $Log$
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

// ---------------------------------------------------------- 
// 3D data structures
// ---------------------------------------------------------- 

class POINT3D {             // point in 3D space
public:
  Coord x_, y_, z_;         // coordinates of point
};

class SEGMENT {             // line segment in 2D space
public:
  Point pt[2];              // two ends of the segment
};

class PLANE {               // triangular plane in 2D space
public:
  Point pt[3];              // three corners of the triangle
  Color color;              // color of triangle
  Coord dist;               // distance from camera (for sorting)
};

// ---------------------------------------------------------- 
// useful geometric computations
// ---------------------------------------------------------- 

class Geom {
public:
  /* Return true if pont (x,y) intersects with 
     rectangle (minX,minY)(maxX,maxY)*/
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

#endif
