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
  Revision 1.16  1996/07/12 23:30:12  jussi
  Added missing semicolon.

  Revision 1.15  1996/07/12 23:12:36  jussi
  Fixed bug in DrawGDataArray().

  Revision 1.14  1996/07/02 22:46:00  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.13  1996/06/27 19:06:58  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.12  1996/04/15 16:08:29  jussi
  Added GetShape() method.

  Revision 1.11  1996/04/09 22:53:55  jussi
  Added View parameter to DrawGDataArray().

  Revision 1.10  1996/01/13 23:10:13  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.9  1996/01/09 22:08:33  jussi
  Added GetZ() function.

  Revision 1.8  1995/12/28 19:30:37  jussi
  Small fix to remove compiler warning.

  Revision 1.7  1995/12/22 18:06:25  jussi
  Symbol width and height are taken as an absolute value.

  Revision 1.6  1995/11/28 00:23:36  jussi
  Elevated a couple of #include statements from MapInterpShape.h
  to this file.

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
  return GetAttr(ptr, xOffset, Coord, offset);
}

inline Coord GetY(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->yOffset < 0)
    return map->GetDefaultY();
  return GetAttr(ptr, yOffset, Coord, offset);
}

inline Coord GetZ(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->zOffset < 0)
    return map->GetDefaultZ();
  return GetAttr(ptr, zOffset, Coord, offset);
}

inline Color GetColor(View *view, char *ptr, TDataMap *map,
		      GDataAttrOffset *offset)
{
  Boolean active;
  Color color = view->GetOverrideColor(active);
  if (active)
    return color;
  if (offset->colorOffset < 0)
    return map->GetDefaultColor();
  return GetAttr(ptr, colorOffset, Color , offset);
}

inline Coord GetSize(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->sizeOffset < 0)
    return map->GetDefaultSize();
  return GetAttr(ptr, sizeOffset, Coord, offset);
}

inline Pattern GetPattern(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->patternOffset < 0)
    return map->GetDefaultPattern();
  return GetAttr(ptr, patternOffset, Pattern, offset);
}

inline Coord GetOrientation(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->orientationOffset < 0)
    return map->GetDefaultOrientation();
  return GetAttr(ptr, orientationOffset, Coord, offset);
}

inline ShapeID GetShape(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeOffset < 0)
    return map->GetDefaultShape();
  return GetAttr(ptr, shapeOffset, ShapeID, offset);
}

inline Coord GetShapeAttr0(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[0] < 0) {
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[0];
  }
  return GetAttr(ptr, shapeAttrOffset[0], Coord, offset);
}

inline Coord GetShapeAttr1(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[1] < 0){
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[1];
  }
  return GetAttr(ptr, shapeAttrOffset[1], Coord, offset);
}

inline Coord GetShapeAttr2(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->shapeAttrOffset[2] < 0){
    ShapeAttr *attrs = map->GetDefaultShapeAttrs();
    return attrs[2];
  }
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

  /* Find maximum symbol size. By default, use 0th and 1st shape
     attribute as the width and height, respectively.  */
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
                          Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    int gRecSize = map->GDataRecordSize();
    char *ptr = (char *)gdata;

    for(int i = 0; i < numSyms; i++) {
      Coord temp;
      temp = fabs(GetSize(ptr, map, offset) * GetShapeAttr0(ptr, map, offset));
      if (temp > width) width = temp;
      temp = fabs(GetSize(ptr, map, offset) * GetShapeAttr1(ptr, map,  offset));
      if (temp > height) height = temp;
      ptr += gRecSize;
    }
  }

  /* Draw GData symbols. */
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {}

 protected:

  /* By default, the shapes cannot display themselves in 3D */
  virtual void Draw3DGDataArray(WindowRep *win, void **gdataArray, int numSyms,
                                TDataMap *map, View *view, int pixelSize) {}

  /* Draw each GData symbol as a single pixel. Used by derived classes
     as a common method in cases where symbols are smaller than one
     pixel. */
  virtual void DrawPixelArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
    GDataAttrOffset *offset = map->GetGDataOffset();
    int i = 0;
    while (i < numSyms) {
      char *gdata = (char *)gdataArray[i];
      int count = 1;
      _x[0] = GetX(gdata, map, offset);
      _y[0] = GetY(gdata, map, offset);
      Color firstColor = GetColor(view, gdata, map, offset);
      
      int colorIndex;
      for(colorIndex = i + 1; colorIndex < numSyms; colorIndex++) {
	char *colorGData = (char *)gdataArray[colorIndex];
	if (GetColor(view, colorGData, map, offset) != firstColor)
	  break;
	_x[count] = GetX(colorGData, map, offset);
	_y[count] = GetY(colorGData, map, offset);
        count++;
      }
      
      win->SetFgColor(firstColor);
      win->DrawPixelArray(_x, _y, count, pixelSize);
      
      i = colorIndex;
    }
  }
};

#endif
