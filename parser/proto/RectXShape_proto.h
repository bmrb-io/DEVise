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

  $Log$*/

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

class CLASSNAME : public RectXShape {
public:
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
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
      GDATANAME *gdata = (GDATANAME *)gdataArray[i];
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

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {
		
    Coord maxWidth, maxHeight;
    map->MaxBoundingBox(maxWidth, maxHeight);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

#ifndef DYNAMIC_COLOR
    win->SetFgColor(GDATA_COLOR);
#endif
#ifndef DYNAMIC_PATTERN
    win->SetPattern(GDATA_PATTERN);
#endif

    if (maxWidth <= pixelWidth) {
			
      int i = 0;
      while (i < numSyms) {

	GDATANAME *gdata = (GDATANAME *)gdataArray[i];
	int count = 1;
	_x[0] = GDATA_X;
	_y[0] = GDATA_Y;
#ifdef DYNAMIC_COLOR
	Color lastColor = GDATA_COLOR;
#endif

	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
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

      Coord x1, y1, x2, y2;
      win->Transform(0.0, 0.0, x1, y1);

      for(int i = 0; i < numSyms; i++) {
	GDATANAME *gdata = (GDATANAME *)gdataArray[i];
	Coord tx, ty;
	Coord width, height;
	win->Transform(GDATA_X, GDATA_Y, tx, ty);
	win->Transform(GDATA_SHAPEATTR_0, 0.0, x2, y2);
	width = x2 - x1;
	if (width < 0.0)
	  width = -width;
	height = GDATA_SHAPEATTR_1 / GDATA_SHAPEATTR_0 * width;
	
#ifdef DYNAMIC_COLOR
	win->SetFgColor(GDATA_COLOR);
#endif
#ifdef DYNAMIC_PATTERN
	win->SetPattern(GDATA_PATTERN);
#endif
	if (width < pixelSize)
	  width = pixelSize;
	if (height < pixelSize)
	  height = pixelSize;
	win->FillPixelRect(tx, ty, width, height);
      }
    }
  }
};
