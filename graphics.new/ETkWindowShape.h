
#ifndef _ETKWINDOWSHAPE_H_
#define _ETKWINDOWSHAPE_H_

#include <iostream.h>
#include "RectShape.h"
#include "StringStorage.h"
//#define DEBUG

class FullMapping_ETkWindowShape
: public ETkWindowShape
{
  public:
    
    virtual int NumShapeAttrs();
    
    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize);
};

#endif
