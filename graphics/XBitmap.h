/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.3  1996/09/06 06:59:45  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.2  1995/09/05 21:13:37  jussi
  Added/update CVS header.
*/

#ifndef XBitmap_h
#define XBitmap_h

const int XBitmapWidth = 16;
const int XBitmapHeight = 16;

const int XNumBitmaps = 55;
extern char xBitmaps[XNumBitmaps][XBitmapWidth*XBitmapHeight];

#endif
