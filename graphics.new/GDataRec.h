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
  Revision 1.11  1999/06/30 17:38:47  wenger
  Data color of parent view's mapping (if specified) now controls the
  background color of view symbols; defined constant strings for GData
  attribute values to avoid potential problems.

  Revision 1.10  1999/05/26 19:50:49  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.9  1999/05/21 14:52:28  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

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
  Coord     _orientation;
  ShapeID   _shape;
  Coord     _bbULx; // Upper left corner of bounding box relative to x, y
  Coord     _bbULy;
  Coord     _bbLRx; // Lower right corner of bounding box relative to x, y
  Coord     _bbLRy;
  ShapeAttr _shapeAttrs[MAX_SHAPE_ATTRS];
};

const int MAX_GDATA_REC_SIZE = sizeof(GDataRec);


// Offsets for the attributes in an actual GData record.  Note that an offset
// can be -1, which means that the given attribute is a constant and is
// therefore not included in the individual records.

struct GDataAttrOffset {
  int _recIdOffset;
  int _xOffset;
  int _yOffset;
  int _zOffset;
  int _colorOffset;
  int _sizeOffset;
  int _patternOffset;
  int _orientationOffset;
  int _shapeOffset;
  int _bbULxOffset;
  int _bbULyOffset;
  int _bbLRxOffset;
  int _bbLRyOffset;
  int _shapeAttrOffset[MAX_SHAPE_ATTRS];
};

// Note: it would probably make sense to handle the bounding box as a
// single structure rather than as four individual values, but I've got
// it working this way and I don't want to change it right now.  RKW
// 1999-05-26.


// Strings for attribute names in GData attribute list.

static const char *gdataXName = "x";
static const char *gdataYName = "y";
static const char *gdataZName = "z";
static const char *gdataColorName = "color";
static const char *gdataSizeName = "size";
static const char *gdataPatternName = "pattern";
static const char *gdataOrientationName = "orientation";
static const char *gdataShapeName = "shape";
static const char *gdataShapeAttrName = "shapeAttr_"; // append number
static const char *gdataShapeAttr0Name = "shapeAttr_0";
static const char *gdataShapeAttr1Name = "shapeAttr_1";
static const char *gdataShapeAttr3Name = "shapeAttr_3";
static const char *gdataShapeAttr4Name = "shapeAttr_4";
static const char *gdataShapeAttr9Name = "shapeAttr_9";

#endif
