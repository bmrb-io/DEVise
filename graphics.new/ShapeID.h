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
  Revision 1.2  1995/09/05 22:15:36  jussi
  Added CVS header.
*/

/* definition of shape ID and shape attributes */

#ifndef ShapeID_H
#define ShapeID_H

typedef unsigned int ShapeID;

/* ID of predefined shapes */

const ShapeID RectShapeID      = 0;
const ShapeID SquareShapeID    = 1;
const ShapeID RectXShapeID     = 2;
const ShapeID BarShapeID       = 3;
const ShapeID PixelRectShapeID = 4;
const ShapeID TriangleShapeID  = 5;

/* Each attribute of the shape is a double, for now */

typedef double ShapeAttr;

#endif
