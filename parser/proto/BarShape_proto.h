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
  Revision 1.5.12.1  1997/08/20 21:20:49  wenger
  Changed multi versions of DrawGData functions to have recordsProcessed
  argument needed for interruptible draws.

  Revision 1.5  1996/07/19 03:49:47  jussi
  Changed View * to ViewGraph *.

  Revision 1.4  1996/07/03 22:54:10  jussi
  Updated to reflect new Shape interface.

  Revision 1.3  1996/04/12 23:35:11  jussi
  Added View * parameter to DrawGDataArray(), corresponding to changes
  made to graphics.new/MapInterpShape.h and MappingInterp.c.

  Revision 1.2  1995/12/14 18:50:19  jussi
  Removed unused variables.

  Revision 1.1  1995/11/27 15:38:57  jussi
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

class CLASSNAME : public BarShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {
#ifdef DYNAMIC_SHAPEATTR_0
    width = 0;
#else
    width = GDATA_SHAPEATTR_0;
#endif
#ifdef DYNAMIC_Y
    height = 0;
#else
    height = fabs(GDATA_Y);
#endif

#ifdef DYNAMIC_SHAPEATTR
    for(int i = 0; i < numSyms; i++) {
      GDATANAME *gdata = &((GDATANAME *)gdataPtr)[i];
      Coord temp;
#ifdef DYNAMIC_SHAPEATTR_0
      temp = GDATA_SHAPEATTR_0;
      if (temp > width) width = temp;
#endif
#ifdef DYNAMIC_Y
      temp = fabs(GDATA_Y);
      if (temp > height) height = temp;
#endif
    }
#endif
  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
                              int numSyms, TDataMap *map,
                              ViewGraph *view, int pixelSize,
			      int &recordsProcessed) {
		
    recordsProcessed = numSyms;

    if (view->GetNumDimensions() == 3)
      return;

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);

#ifndef DYNAMIC_COLOR
    win->SetFgColor(GDATA_COLOR);
#endif
#ifndef DYNAMIC_PATTERN
    win->SetPattern(GDATA_PATTERN);
#endif

    for(int i = 0; i < numSyms; i++) {
      GDATANAME *gdata = (GDATANAME *)gdataArray[i];
#ifdef DYNAMIC_COLOR
      win->SetFgColor(GDATA_COLOR);
#endif
#ifdef DYNAMIC_PATTERN
      win->SetPattern(GDATA_PATTERN);
#endif
      Coord y = GDATA_Y;
      Coord width = GDATA_SHAPEATTR_0;

      if (y >= 0.0) {
	if (width > pixelWidth)
	  win->FillRect(GDATA_X - width / 2.0, 0.0, width, y);
	else
	  win->FillRect(GDATA_X, 0.0, width, y);
      } else {
	if (width > pixelWidth)
	  win->FillRect(GDATA_X - width / 2.0, y, width, -y);
	else
	  win->FillRect(GDATA_X, y, width, -y);
      }
    }
  }
};
