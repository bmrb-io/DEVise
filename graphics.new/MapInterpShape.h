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
  Revision 1.5  1995/11/24 20:13:40  jussi
  Added missing closing brace.

  Revision 1.4  1995/11/24 20:07:32  jussi
  Fixed calculation of X and Y starting position for cases where
  pixel width (or height) is at most 1. In such cases one should not
  adjust X/Y coordinates by one half of the width/height.

  Revision 1.3  1995/11/21 23:32:07  jussi
  Added copyright notice and cleaned up the code significantly.

  Revision 1.2  1995/09/05 22:15:01  jussi
  Added CVS header.
*/

/*
struct FullMapping_GData {
  double x;
  double y;
  Color color;
  double size;
  Pattern pattern;
  ShapeID shapeID;
  double orientation;
  double shapeAttr_0;
  double shapeAttr_1;
};
*/

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
    return GetAttr(ptr,yOffset, Coord, offset);
}

inline Color GetColor(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->colorOffset < 0)
    return map->GetDefaultColor();
  else
    return GetAttr(ptr,colorOffset, Color , offset);
}

inline Pattern GetPattern(char *ptr, TDataMap *map, GDataAttrOffset *offset)
{
  if (offset->patternOffset < 0)
    return map->GetDefaultPattern();
  else
    return GetAttr(ptr,patternOffset, Pattern, offset);
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

class FullMapping_RectShape: public RectShape {
public:
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

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {
		 
    GDataAttrOffset *offset = map->GetGDataOffset();
    Coord maxWidth, maxHeight;
    map->MaxBoundingBox(maxWidth, maxHeight);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    // pixelWidth is how many X units one pixel corresponds to
    // pixelHeight is how many Y units one pixel corresponds to
    // maxWidth is the maximum width of rectangles, measured in X units
    // maxHeight is the maximum width of rectangles, measured in Y units
    //
    // see if one pixel is enough to cover the area whose width is
    // pixelWidth and height is pixelHeight

    if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
			 
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

    } else {
      
      int i = 0;
      while (i < numSyms) {
	char *gdata = (char *)gdataArray[i];
	int count = 1;
	Coord width = GetShapeAttr0(gdata, map, offset);
	Coord height = GetShapeAttr1(gdata, map, offset);

	_x[0] = GetX(gdata, map, offset);
	if (width > 1)
	  _x[0] -= width / 2.0;
	_y[0] = GetY(gdata, map, offset);
	if (height > 1)
	  _y[0] -= height / 2.0;
	_width[0] = width;
	_height[0] = height;
	
	Color lastColor = GetColor(gdata, map, offset);

	for(int colorIndex = i + 1; colorIndex < numSyms; colorIndex++) {
	  char *colorGData = (char *)gdataArray[colorIndex];
	  if (GetColor(colorGData, map, offset) != lastColor)
	    break;
	  
	  width = GetShapeAttr0(colorGData, map, offset);
	  height = GetShapeAttr1(colorGData, map, offset);
	  _x[count] = GetX(colorGData, map, offset);
	  if (width > 1)
	    _x[count] -= width / 2.0;
	  _y[count] = GetY(colorGData, map, offset);
	  if (height > 1)
	    _y[count] -= height / 2.0;
	  _width[count] = width;
	  _height[count] = height;
	  
	  count++;
	}
	
	win->SetFgColor(lastColor);
	win->FillRectArray(_x, _y, _width, _height, count);
	
	lastColor = GetColor((char *)gdataArray[colorIndex], map, offset);
	i = colorIndex;
      }
    }
  }
};
 
class FullMapping_RectXShape: public RectXShape {
public:
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

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {

    GDataAttrOffset *offset = map->GetGDataOffset();
    Coord maxWidth, maxHeight;
    map->MaxBoundingBox(maxWidth, maxHeight);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    // pixelWidth is how many X units one pixel corresponds to
    // maxWidth is the maximum width of rectangles, measured in X units
    //
    // see if one pixel is enough to cover the area whose width is pixelWidth

    if (maxWidth <= pixelWidth) {

      int i = 0;
      while (i < numSyms) {

	char *gdata = (char *)gdataArray[i];
	int count = 1;
	_x[0] = GetX(gdata, map, offset);
	_y[0] = GetY(gdata, map, offset);
	Color lastColor = GetColor(gdata, map, offset);

	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++){
	  char *colorGData = (char *)gdataArray[colorIndex];
	  if (GetColor(colorGData,map,offset) != GetColor(gdata, map, offset))
	    break;
	  _x[count] = GetX(colorGData, map, offset);
	  _y[count++] = GetY(colorGData, map, offset);
	}
	
	win->SetFgColor(lastColor);
	win->DrawPixelArray(_x, _y, count, pixelSize);
	
	lastColor = GetColor((char *)gdataArray[colorIndex], map, offset);
	i = colorIndex;
      }

    } else {
      
      Coord x1, y1, x2, y2;
      win->Transform(0.0, 0.0, x1, y1);

      for(int i = 0; i < numSyms; i++) {
	char *gdata = (char *)gdataArray[i];
	Coord tx, ty;
	Coord width, height;
	win->Transform(GetX(gdata, map, offset), GetY(gdata, map, offset),
		       tx, ty);
	win->Transform(GetShapeAttr0(gdata, map, offset), 0.0,
		       x2, y2);
	width = x2 - x1;
	if (width < 0.0)
	  width = -width;
	height = GetShapeAttr1(gdata, map, offset) /
                 GetShapeAttr0(gdata, map, offset) * width;
	
	win->SetFgColor(GetColor(gdata, map, offset));
	win->SetPattern(GetPattern(gdata, map, offset));
	if (width < pixelSize)
	  width = pixelSize;
	if (height < pixelSize)
	  height = pixelSize;
	win->FillPixelRect(tx, ty, width, height);
      }
    }
  }
};
 
class FullMapping_BarShape: public BarShape {
public:
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
				Coord &width, Coord &height) {
    width = 0.0;
    height = 0.0;

    GDataAttrOffset *offset = map->GetGDataOffset();
    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      Coord curWidth = GetShapeAttr0(gdata, map, offset);
      if (curWidth > width)
	width = curWidth;
      Coord curHeight = fabs(GetY(gdata, map, offset));
      if (height > curHeight)
	height = curHeight;
    }
  }
  
  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map,  int pixelSize) {

    GDataAttrOffset *offset = map->GetGDataOffset();

    for(int i = 0; i < numSyms; i++) {
      char *gdata = (char *)gdataArray[i];
      win->SetFgColor(GetColor(gdata, map, offset));
      win->SetPattern(GetPattern(gdata, map, offset));
      Coord y = GetY(gdata, map, offset);
      Coord width = GetShapeAttr0(gdata, map, offset);
      if (y >= 0.0) {
	if (width > 1)
	  win->FillRect(GetX(gdata, map, offset) - width / 2.0, 0.0, width, y);
	else
	  win->FillRect(GetX(gdata, map, offset), 0.0, width, y);
      } else {
	if (width > 1)
	  win->FillRect(GetX(gdata, map, offset) - width / 2.0, y, width, -y);
	else
	  win->FillRect(GetX(gdata, map, offset), y, width, -y);
      }
    }
  }
};
