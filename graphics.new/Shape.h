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
  Revision 1.5  1995/11/28 00:07:25  jussi
  Moved some common code from MapInterpShape.h to this file.
  BoundingBoxGData is by default computed based on the width
  and height of a symbol. Small symbols are drawn as single
  pixels, and for this purpose derived classes can use the
  DrawPixelArray method.

  Revision 1.4  1995/11/25  01:20:15  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.3  1995/11/21 23:23:38  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:15:35  jussi
  Added CVS header.
*/

#ifndef Shape_h
#define Shape_h

#include <stdio.h>
#include "TDataMap.h"
#include "WindowRep.h"
#include "Temp.h"

#define GetAttr(ptr, attrName, attrType, offset) \
	*((attrType *)(ptr+offset->attrName))

inline Coord GetX(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->xOffset < 0)
    return map->GetDefaultX();
  else {
    return GetAttr(ptr, xOffset, Coord, offset);
  }
}

inline Coord GetY(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->yOffset < 0)
    return map->GetDefaultY();
  else
    return GetAttr(ptr, yOffset, Coord, offset);
}

inline Color GetColor(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->colorOffset < 0)
    return map->GetDefaultColor();
  else
    return GetAttr(ptr, colorOffset, Color , offset);
}

inline Pattern GetPattern(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->patternOffset < 0)
    return map->GetDefaultPattern();
  else
    return GetAttr(ptr, patternOffset, Pattern, offset);
}

inline Coord GetShapeAttr0(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[0] < 0) {
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[0];
  } else
    return GetAttr(ptr, shapeAttrOffset[0], Coord, offset);
}

inline Coord GetShapeAttr1(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[1] < 0){
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[1];
  } else
    return GetAttr(ptr, shapeAttrOffset[1], Coord, offset);
}

inline Coord GetShapeAttr2(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[2] < 0){
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[2];
  } else
    return GetAttr(ptr, shapeAttrOffset[2], Coord, offset);
}

class WindowRep;
class TDataMap;

class Shape {
 public:
  /* Return # of shape attributes needed for this shape */
  virtual int NumShapeAttrs() {
    return 0;
  }

  /* Find bounding box for GData. By default, use 0th and 1st shape
     attribute as the width and height, respectively.  */
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
				Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord temp = GetShapeAttr0(gdata, map, offset);
      if (temp > width) width = temp;
      temp = GetShapeAttr1(gdata, map,  offset);
      if (temp > height) height = temp;
    }
  }

  /* Draw GData symbols. */
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {
  }

 protected:
  /* Draw each GData symbol as a single pixel. Used by derived classes
     as a common method in cases where symbols are smaller than one
     pixel. */
  virtual void DrawPixelArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {
    GDataAttrOffset *offset = map->GetGDataOffset();
    int i = 0;
    while (i < numSyms) {
      char *gdata = (char *)gdataArray[i];
      int count = 1;
      _x[0] = GetX(gdata, map, offset);
      _y[0] = GetY(gdata, map, offset);
      Color lastColor = GetColor(gdata, map, offset);
      
      for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	char *colorGData = (char *)gdataArray[colorIndex];
	if (GetColor(colorGData,map,offset) != lastColor)
	  break;
	_x[count] = GetX(colorGData, map, offset);
	_y[count++] = GetY(colorGData, map, offset);
      }
      
      win->SetFgColor(lastColor);
      win->DrawPixelArray(_x, _y, count, pixelSize);
      
      lastColor = GetColor((char *)gdataArray[colorIndex], map, offset);
      i = colorIndex;
    }
  }
};

#endif
