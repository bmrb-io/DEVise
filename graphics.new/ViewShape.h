#ifdef VIEW_SHAPE 
#ifndef VIEW_SHAPE_H
#define VIEW_SHAPE_H

#include "RectShape.h"


class FullMapping_ViewShape
: public ViewShape
{
  public :
  virtual int NumShapeAttrs();
  
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms, 
			  Coord &width, Coord &height);
  
  virtual void DrawGDataArray (WindowRep *win, void **gdataArray, 
			       int numSyms, TDataMap *map,
			       ViewGraph *view, int pixelSize,
			       int &recordsProcessed);
};
#endif

#endif
