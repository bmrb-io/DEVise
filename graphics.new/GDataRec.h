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
  Revision 1.8  1998/01/30 21:53:14  wenger
  Did some cleaning up of the MappingInterp and NativeExpr code
  (NativeExpr still needs a lot more); NativeExpr code can now
  parse expressions containing constant strings (they are treated
  as numerical zero for now) (this fixes bug 275).

  Revision 1.7  1997/11/24 23:14:51  weaver
  Changes for the new ColorManager.

  Revision 1.6.10.1  1997/05/21 20:40:26  weaver
  Changes for new ColorManager

  Revision 1.6  1996/11/13 16:57:03  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.5  1996/07/18 20:31:54  jussi
  Renamed fields in structure to make sure no source file
  really uses these fields.

  Revision 1.4  1996/01/13 23:10:32  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.3  1995/11/28 00:05:28  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:55  jussi
  Added CVS header.
*/

#ifndef GDataRec_h
#define GDataRec_h

#include "ShapeID.h"
#include "Pattern.h"
#include "RecId.h"

#include "Color.h"

const int MAX_SHAPE_ATTRS = 10;

// Note that GData in memory is *not* an array of GDataRecs; MappingInterp.c
// uses a dynamic structure which stores and caches only those fields that
// are variable.  GDataRec is used to hold default GData values and compute
// the maximum GData record size.

struct GDataRec {
  RecId     _recId;
  Coord     _x;
  Coord     _y;
  Coord     _z;
  PColorID  _color;
  Coord     _size;
  Pattern   _pattern;
  ShapeID   _shape;
  Coord     _orientation;
  ShapeAttr _shapeAttrs[MAX_SHAPE_ATTRS];
};

const int MAX_GDATA_REC_SIZE = sizeof(GDataRec);


// Offsets for the attributes in an actual GData record.  Note that an offset
// can be -1, which means that the given attribute is a constant and is
// therefore not included in the individual records.

struct GDataAttrOffset { //TEMP -- change for BB
  int _recIdOffset;
  int _xOffset;
  int _yOffset;
  int _zOffset;
  int _colorOffset;
  int _sizeOffset;
  int _shapeOffset;
  int _patternOffset;
  int _orientationOffset;
  int _shapeAttrOffset[MAX_SHAPE_ATTRS];
};


#endif
