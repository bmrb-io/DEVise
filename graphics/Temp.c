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
  Revision 1.2  1996/07/20 18:47:59  jussi
  Added 3D line segment shape and renamed some 3D type names to
  be more general.

  Revision 1.1  1996/06/21 19:25:37  jussi
  Moved file from graphics.new to graphics.

  Revision 1.4  1996/02/28 17:32:39  yuc
  Add global block_data for 3D Block Shape.

  Revision 1.3  1996/01/09 22:13:34  jussi
  Added support for 3D data.

  Revision 1.2  1995/09/05 22:16:06  jussi
  Added CVS header.
*/

#include "Temp.h"
#include "WindowRep.h"

Coord _x[WINDOWREP_BATCH_SIZE];
Coord _y[WINDOWREP_BATCH_SIZE];
Coord _z[WINDOWREP_BATCH_SIZE];
Coord _width[WINDOWREP_BATCH_SIZE];
Coord _height[WINDOWREP_BATCH_SIZE];
Coord _depth[WINDOWREP_BATCH_SIZE];

Object3D _object3D[WINDOWREP_BATCH_SIZE]; 
Segment  _segment[WINDOWREP_BATCH_SIZE * BLOCK_EDGES];
Plane    _plane[WINDOWREP_BATCH_SIZE * 2 * BLOCK_SIDES];
int      _numSegments = 0;
int      _numPlanes = 0;
