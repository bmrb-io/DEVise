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
  Revision 1.1  1996/06/21 19:25:38  jussi
  Moved file from graphics.new to graphics.

  Revision 1.5  1996/02/28 17:33:22  yuc
  Make block_data an extern variable.

  Revision 1.4  1996/02/12 22:02:58  jussi
  Removed redundant size declarations.

  Revision 1.3  1996/01/09 22:14:12  jussi
  Added support for 3D data.

  Revision 1.2  1995/09/05 22:16:07  jussi
  Added CVS header.
*/

#ifndef Temp_h
#define Temp_h

#include "DeviseTypes.h"
#include "WindowRep.h"
#include "Geom.h"
#include "Map3D.h"

extern Coord _x[];
extern Coord _y[];
extern Coord _z[];
extern Coord _width[];
extern Coord _height[];
extern Coord _depth[];

/* data structures for 3D */
extern Object3D  _object3D[];
extern Segment   _segment[];
extern Plane     _plane[];
extern int       _numSegments;
extern int       _numPlanes;

#endif
