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
  Revision 1.4.12.1  1997/08/14 16:16:05  wenger
  Statistics, etc., now work correctly for timed-out draw in ViewScatter-
  type views; bumped up version because of improved stop capability.

  Revision 1.4  1995/11/28 00:08:49  jussi
  Removed constant ShapeID's for RectShape etc.

  Revision 1.3  1995/11/27 15:48:19  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:15:36  jussi
  Added CVS header.
*/

#ifndef ShapeID_H
#define ShapeID_H

/* Note: see the MappingInterp::_shapes to figure out which number corresponds
 * to which shape. */
typedef unsigned int ShapeID;

typedef double ShapeAttr;

#endif
