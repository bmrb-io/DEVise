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
  Revision 1.10  1996/06/16 01:54:32  jussi
  Added PolylineShape, PolylineFileShape, and TextLabelShape.

  Revision 1.9  1996/05/22 21:05:11  jussi
  Added HighLowShape. Added tentative version of GifImageShape.

  Revision 1.8  1996/04/23 20:35:42  jussi
  Added Segment shape which just connects two end points.

  Revision 1.7  1996/04/16 20:43:11  jussi
  Added HorLineShape, a 2D horizontal line shape that is used
  by statistics views.

  Revision 1.6  1996/01/09 22:15:44  jussi
  Added 3D block object.

  Revision 1.5  1995/12/22 18:06:51  jussi
  Added Vector shape.

  Revision 1.4  1995/11/28 00:05:42  jussi
  Added polygon and oval shapes.

  Revision 1.3  1995/11/21 23:32:49  jussi
  Added copyright notice and cleaned up code significantly. Removed
  old code which had no relevance anymore.

  Revision 1.2  1995/09/05 22:15:34  jussi
  Added CVS header.
*/

#ifndef RectShape_h
#define RectShape_h

#include "Shape.h"

/* RectShape: rectangular shape whose width and height are specified by
   the 0th and 1st shape attribute, respectively. */

class RectShape : public Shape {};

/* RectXShape: square shape whose height is always the same as the width,
   regardless of X and Y zooming or scaling. */

class RectXShape : public Shape {};

/* BarShape: a shape drawn from (x,0) to (x,y), 0th attribute = width */

class BarShape : public Shape {};

/* RegularPolygonShape: an elliptical polygon whose center is at (x,y)
   and whose width and height are 0th and 1st shape attribute, respectively.
   The oval shape is drawn using a small number of segments, currently set
   at 16. */

class RegularPolygonShape : public Shape {};

/* OvalShape: an ellipse whose center is at (x,y) and whose width and
   height are 0th and 1st shape attribute, respectively. Compared to
   PolyShape, this elliptical shape is perfectly round, with no visible
   straight segments between corners. */

class OvalShape : public Shape {};

/* VectorShape: a vector drawn from (x,y) to (x+w,y+h) where w is
   the 0th and h is the 1st shape attribute. Width and height may
   be negative because a vector has a direction (there is an
   arrow head also). */

class VectorShape : public Shape {};

/* HorLine: a horizontal line drawn at Y coordinate. The X coordinate is
   ignored. */

class HorLineShape : public Shape {};

/* Segment: just a plain line segment from (x,y) to (x+w,y+h)
   where w = shape attribute 0 and h = shape attribute 1.   
   Basically the same as a vector but without the arrow head. */

class SegmentShape : public Shape {};

/* HighLow: a high-low bar (aka error bar) which has three distinct points:
   the data point (X,Y), a high point (X,a1), and a low point (X,a2).
   The width of the vertical bar connecting the high and low points
   is a0 units wide. a0 = shape attribute 0, a1 = shape attribute 1. */

class HighLowShape : public Shape {};

/* Polyline: draws a polyline whose first point is at (X,Y) and
   whose additional data points are given in shape attributes 1
   through 2n (including X and Y) where n is given in shape
   attribute 0. */

class PolylineShape : public Shape {};

/* GifImage: draws a GIF image whose filename is given in shape
   attribute 0 at (X,Y). */

class GifImageShape : public Shape {};

/* PolylineFile: draws a polygon whose origin is at (X,Y) and whose
   data points are read from a file given in shape attribute 0.
   Data points are relative to (X,Y). The scanf format is given
   in shape attribute 1 (a la gnuplot). */

class PolylineFileShape : public Shape {};

/* TextLabel: draws a text label given by shape attribute 0 at (X,Y). */

class TextLabelShape : public Shape {};

#endif
