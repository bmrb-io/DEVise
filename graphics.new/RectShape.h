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
  Revision 1.2  1995/09/05 22:15:34  jussi
  Added CVS header.
*/

#ifndef RectShape_h
#define RectShape_h

#include "DeviseTypes.h"
#include "Transform.h"
#include "Geom.h"
#include "WindowRep.h"
#include "Shape.h"
#include "Exit.h"

class RectShape : public Shape {};

/* RectXShape: rect shape whose height always looks the same in
   proportion to width, despite zooming or scaling.
   Note: orientation and scale not yet implemented. */

class RectXShape : public Shape {};

/* BarShape: a shape drawn from (x,0) to (x,y), 0th attribute = width */

class BarShape : public Shape {};

#endif
