/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.5  1996/07/19 03:49:35  jussi
  Changed View * to ViewGraph *.

  Revision 1.4  1996/07/03 22:54:11  jussi
  Updated to reflect new Shape interface.

  Revision 1.3  1996/04/12 23:35:08  jussi
  Added View * parameter to DrawGDataArray(), corresponding to changes
  made to graphics.new/MapInterpShape.h and MappingInterp.c.

  Revision 1.2  1995/12/28 20:37:01  jussi
  Small fixes to remove compiler warnings.

  Revision 1.1  1995/11/27 15:38:58  jussi
  Initial revision.
*/

#ifdef DYNAMIC_X
#define GDATA_X (gdata->x)
#else
#define GDATA_X (map->GetDefaultX())
#endif
#ifdef DYNAMIC_Y
#define GDATA_Y (gdata->y)
#else
#define GDATA_Y (map->GetDefaultY())
#endif
#ifdef DYNAMIC_COLOR
#define GDATA_COLOR (gdata->color)
#else
#define GDATA_COLOR (map->GetDefaultColor())
#endif
#ifdef DYNAMIC_PATTERN
#define GDATA_PATTERN (gdata->pattern)
#else
#define GDATA_PATTERN (map->GetDefaultPattern())
#endif
#ifdef DYNAMIC_SHAPEATTR_0
#define GDATA_SHAPEATTR_0 (gdata->shapeAttr_0)
#else
#define GDATA_SHAPEATTR_0 ((map->GetDefaultShapeAttrs())[0])
#endif
#ifdef DYNAMIC_SHAPEATTR_1
#define GDATA_SHAPEATTR_1 (gdata->shapeAttr_1)
#else
#define GDATA_SHAPEATTR_1 ((map->GetDefaultShapeAttrs())[1])
#endif

class CLASSNAME : public RectShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {
#ifdef DYNAMIC_SHAPEATTR_0
    width = 0.0;
#else
    width = GDATA_SHAPEATTR_0;
#endif
#ifdef DYNAMIC_SHAPEATTR_1
    height = 0.0;
#else
    height = GDATA_SHAPEATTR_1;
#endif

#ifdef DYNAMIC_SHAPEATTR
    for(int i = 0; i < numSyms; i++) {
      GDATANAME *gdata = &((GDATANAME *)gdataPtr)[i];
      Coord temp;
#ifdef DYNAMIC_SHAPEATTR_0
      temp = GDATA_SHAPEATTR_0;
      if (temp > width) width = temp;
#endif
#ifdef DYNAMIC_SHAPEATTR_1
      temp = GDATA_SHAPEATTR_1;
      if (temp > height) height = temp;
#endif
    }
#endif
  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize) {
		
    if (view->GetNumDimensions() == 3)
      return;

    Coord maxWidth, maxHeight, maxDepth;
    map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

#ifndef DYNAMIC_COLOR
    win->SetFgColor((GlobalColor) GDATA_COLOR);
#endif

    if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
			
      int i = 0;
      while (i < numSyms) {

	GDATANAME *gdata = (GDATANAME *)gdataArray[i];
	int count = 1;
	_x[0] = GDATA_X;
	_y[0] = GDATA_Y;
#ifdef DYNAMIC_COLOR
	GlobalColor lastColor = (GlobalColor) GDATA_COLOR;
#endif

	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  GDATANAME *gdata = (GDATANAME *)gdataArray[colorIndex];
#ifdef DYNAMIC_COLOR
	  if (GDATA_COLOR != lastColor)
	    break;
#endif
	  _x[count] = GDATA_X;
	  _y[count++] = GDATA_Y;
	}

#ifdef DYNAMIC_COLOR
	win->SetFgColor(lastColor);
#endif
	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      int i = 0;
      while (i < numSyms) {
	GDATANAME *gdata = (GDATANAME *)gdataArray[i];
	int count = 1;
	Coord width = GDATA_SHAPEATTR_0;
	Coord height = GDATA_SHAPEATTR_1;
	_x[0] = GDATA_X;
	_y[0] = GDATA_Y;
	if (width > 1)
	  _x[0] -= width / 2.0;
	if (height > 1)
	  _y[0] -= height / 2.0;
	_width[0] = width;
	_height[0] = height;

#ifdef DYNAMIC_COLOR
	GlobalColor lastColor = (GlobalColor) GDATA_COLOR;
#endif

	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  GDATANAME *gdata = (GDATANAME *)gdataArray[colorIndex];
#ifdef DYNAMIC_COLOR
	  if (GDATA_COLOR != lastColor)
	    break;
#endif
	  width = GDATA_SHAPEATTR_0;
	  height = GDATA_SHAPEATTR_1;
	  _x[count] = GDATA_X;
	  _y[count] = GDATA_Y;
	  if (width > 1)
	    _x[count] -= width / 2.0;
	  if (height > 1)
	    _y[count] -= height / 2.0;

	  _width[count] = width;
	  _height[count] = height;
	  
	  count++;
	}

#ifdef DYNAMIC_COLOR
	win->SetFgColor((GlobalColor) GDATA_COLOR);
#endif
	win->FillRectArray(_x, _y, _width, _height, count);
	i = colorIndex;
      }
    }
  }
};
