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

/* PolygonShape: an elliptical polygon whose center is at (x,y) and whose
   width and height are 0th and 1st shape attribute, respectively. The
   oval shape is drawn using a small number of segments, currently set
   at 16. */

class PolygonShape : public Shape {};

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

#endif
