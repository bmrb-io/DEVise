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
  Revision 1.3  1996/01/09 22:13:34  jussi
  Added support for 3D data.

  Revision 1.2  1995/09/05 22:16:06  jussi
  Added CVS header.
*/

#include "Temp.h"


BLOCK block_data[WINDOWREP_BATCH_SIZE]; 

Coord _x[WINDOWREP_BATCH_SIZE];
Coord _y[WINDOWREP_BATCH_SIZE];
Coord _z[WINDOWREP_BATCH_SIZE];
Coord _width[WINDOWREP_BATCH_SIZE];
Coord _height[WINDOWREP_BATCH_SIZE];
Coord _depth[WINDOWREP_BATCH_SIZE];

