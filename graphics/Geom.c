/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.3  1995/12/14 17:02:32  jussi
  Added copyright notice and made small fixes.

  Revision 1.2  1995/09/05 21:12:49  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Geom.h"

/**************************************************************
Return true if point (x,) intersects with line (x0,y0)
***************************************************************/

Boolean Geom::PointLineIntersect(Coord x, Coord y,
				 Coord x0, Coord y0,
				 Coord x1, Coord y1,
				 Coord lineWidth )
{
  Coord minx, maxx, miny, maxy;
  minx = MinMax::min(x0,x1); maxx = MinMax::max(x0,x1);
  miny = MinMax::min(y0,y1); maxy = MinMax::max(y0,y1);
  
  if (x < minx || x > maxx || y < miny || y > maxy)
    /* not within the line bounding rectangle */
    return false;
  
  /* equation of a line : y = mx+b */
  Coord m, b, dy, dx;
  dy = y1-y0;
  dx = x1 - x0;

  if (dx <= 0.01) {
    /* vertical line, and point within the bounding rectangle */
    return true;
  }

  m = dy/dx;
  b = y0-m*x0;
  
  /* now let's see if the given x,y fit the equation */
  Coord tmpy = m*x+b;
#ifdef DEBUG_GEOM1
  printf("line intersect: y=%f, tmpy= %f, lineWdith= %f\n",
	 y, tmpy, lineWidth);
#endif
  if (fabs(tmpy - y) <= (lineWidth/2))
    return true;
  
  return false;
}

/***************************************************************/
/* This is the Cohen-Sutherland line clipping algorithm. 
   See Fundamentals of Interactive Computer Graphics, page 145.
   published 1982*/
/* outcode 0 means point is within the window */

static const int above_window = 1; /* point is above window */
static const int below_window = 2; /* point is below window */
static const int right_window = 4; /* point is to the right */
static const int left_window =  8; /* point is to the left */

inline static int outcode(Coord x, Coord y,Coord left, Coord bot, Coord rt, 
			  Coord top)
{
  int code = 0;

  if (y > top) code |= above_window;
  if (y < bot) code |= below_window;
  if (x > rt) code |= right_window;
  if (x < left) code |= left_window;
  
  return code;
}

/**********************************************************
clipping line with rectangle
**********************************************************/

Boolean Geom::LineRectClip(Coord minX, Coord minY, 
			   Coord maxX, Coord maxY, 
			   Coord x1,Coord y1, Coord x2, Coord y2,
			   Coord &newx1, Coord &newy1,
			   Coord &newx2, Coord &newy2)
{
#ifdef DEBUG_GEOM
  printf("\nGeom::LineRectClip rect=(%f,%f),(%f,%f), line= (%f,%f,%f,%f)\n",
	 minX,minY,maxX,maxY,x1,y1,x2,y2);
#endif

  int code1, code2;

  while (true) {
    code1 = outcode(x1,y1,minX,minY, maxX,maxY);
    code2 = outcode(x2,y2,minX,minY, maxX,maxY);
    if ( (code1 & code2) != 0){
      /* trivial reject */
#ifdef DEBUG_GEOM
      printf("return false\n");
#endif
      return false;
    }
    if ((code1 | code2) == 0){
      /* trivial accept */
      newx1 = x1; newy1 = y1; 
      newx2 = x2; newy2 = y2;
#ifdef DEBUG_GEOM
      printf("new line=(%f,%f),(%f,%f)\n",x1,y1,x2,y2);
#endif
      return true;
    }
    
    /* subdivide the line at the end point */
    if (code1 == 0){
      /* The first point is inside the rectangle.  Swap the 2 points 
	 so that the first point is outside the rectangle */
      Coord tmp;
      tmp = x1; x1 = x2; x2 = tmp;
      tmp = y1; y1 = y2; y2 = tmp;
      
      int tmpcode;
      tmpcode = code1; code1 = code2; code2 = tmpcode;
    }
    
    if (code1 & above_window){
      x1 = x1 + (x2-x1)*(maxY-y1)/(y2-y1);
      y1 = maxY;
    }
    else if (code1 & below_window){
      x1 = x1 + (x2-x1)*(minY-y1)/(y2-y1);
      y1 = minY;
    }
    else if (code1 & right_window){
      y1 = y1 + (y2-y1)*(maxX-x1)/(x2-x1);
      x1 = maxX;
    }
    else if (code1 & left_window){
      y1 = y1 + (y2-y1)*(minX-x1)/(x2-x1);
      x1 = minX;
    }
  }
}

/*************************************************************
 Clip 1strectangle against the 2nd, and strore the result
 in 1st. Return true if the two rectangles intersect.
 If they don't intersect, the rectangles are not modified
**************************************************************/

Boolean Geom::RectRectClip(Coord &minX1, Coord &minY1, Coord &maxX1,
			   Coord &maxY1, Coord minX2, Coord minY2,
			   Coord maxX2, Coord maxY2)
{
#ifdef DEBUG_GEOM
  printf("Geom::RectRectClip (%f,%f),(%f,%f), (%f,%f),(%f,%f)\n",
	 minX1, minY1, maxX1, maxY1, minX2, minY2, maxX2, maxY2);
#endif

  if (RectRectIntersect(minX1,minY1,maxX1,maxY1,minX2,minY2,maxX2,maxY2)){
    minX1 = MinMax::max(minX1,minX2);
    minY1 = MinMax::max(minY1,minY2);
    maxX1 = MinMax::min(maxX1,maxX2);
    maxY1 = MinMax::min(maxY1,maxY2);
    return true;
  }
  return false;
}

/******************************************************************
  Return TRUE if rect1 (minX1,minY1)(maxX1,maxY1) intersects with
  rect2 (minX2,minY2),(maxX2,maxY2).
********************************************************************/

Boolean Geom::RectRectIntersect(Coord minX1,Coord minY1,
				Coord maxX1,Coord maxY1,
				Coord minX2,Coord minY2, 
				Coord maxX2,Coord maxY2)
{
#ifdef DEBUG_GEOM
  printf("Geom::RectRectIntersect: %f,%f,%f,%f,%f,%f,%f,%f,\n",
	 minX1,minY1, maxX1,maxY1,minX2,minY2, maxX2,maxY2);
#endif
  /* quick check before complicated stuff */
  if (maxX1 < minX2 || maxX2 < minX1 ||
      maxY1 < minY2 || maxY2 < minY1)
    return false;
  else return(
	      /* test 1st inside second */
	      (minX1 >= minX2 && maxX1 <= maxX2 && minY1 >= minY2 && maxY1 <= maxY2)||
	      /* test 2nd inside first */
	      (minX2 >= minX1 && maxX2 <= maxX1 && minY2 >= minY1 && maxY2 <= maxY1)||
	      
	      /* test intersection of 1st against second. Beware of this case:
		     +-+
		   +-|-|-+
		   | | | |
		   +-|-|-+
		     +-+
	      */
	      LineRectIntersect(minX2,minY2,maxX2,maxY2,minX1,minY1,maxX1,minY1)||
	      LineRectIntersect(minX2,minY2,maxX2,maxY2,maxX1,minY1,maxX1,maxY1)||
	      LineRectIntersect(minX2,minY2,maxX2,maxY2,maxX1,maxY1,minX1,maxY1)||
	      LineRectIntersect(minX2,minY2,maxX2,maxY2,minX1,maxY1,minX1,minY1)
	      ); 
}
	
