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
  Revision 1.2  1995/09/05 22:14:55  jussi
  Added CVS header.
*/

#ifndef GDataRec_h
#define GDataRec_h

#include "Color.h"
#include "ShapeID.h"
#include "Pattern.h"

/* full gdata record */

const int MAX_GDATA_ATTRS = 10;

struct GDataRec {
  double    x;
  double    y;
  Color     color;
  double    size;
  Pattern   pattern; 
  ShapeID   shape; 
  double    orientation;
  ShapeAttr shapeAttrs[MAX_GDATA_ATTRS];
};
#endif
