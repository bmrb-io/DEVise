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
  Revision 1.2  1995/09/05 22:16:07  jussi
  Added CVS header.
*/

#ifndef Temp_h
#define Temp_h

#include "DeviseTypes.h"
#include "WindowRep.h"

extern Coord _x[WINDOWREP_BATCH_SIZE];
extern Coord _y[WINDOWREP_BATCH_SIZE];
extern Coord _z[WINDOWREP_BATCH_SIZE];
extern Coord _width[WINDOWREP_BATCH_SIZE];
extern Coord _height[WINDOWREP_BATCH_SIZE];
extern Coord _depth[WINDOWREP_BATCH_SIZE];

#endif
