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
  Revision 1.3  1995/11/21 23:23:38  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:15:35  jussi
  Added CVS header.
*/

#ifndef Shape_h
#define Shape_h

#include <stdio.h>
#include "DeviseTypes.h"

class WindowRep;
class TDataMap;

class Shape {
 public:
  /* Return # of shape attributes needed for this shape */
  virtual int NumShapeAttrs() {
    return 0;
  }

  /* draw gdata. width == width of rectangle if size of
     gdata is smaller than 1 pixel. Used to enhance the drawing
     if the density of points is small.*/
  virtual void DrawGDataArray(WindowRep *, void **, int, TDataMap *,
			      int width) {}

  /* Find bounding box for gdata. By default, set both to 0.0  */
  virtual void BoundingBoxGData(TDataMap *, void **, int, Coord &width,
				Coord &height) {
    width = 0.0;
    height = 0.0;
  }
};

#endif
