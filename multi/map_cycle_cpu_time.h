/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */

struct CycleCpuTimeMapping_GData {
	double x;
	double y;
	double shapeAttr_0;
	double shapeAttr_1;
};

# 1 "../parser/proto/RectShape_proto.h"
 













 






















































class CycleCpuTimeMapping_RectShape  : public RectShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {

    width = 0.0;




    height = 0.0;





    for(int i = 0; i < numSyms; i++) {
      CycleCpuTimeMapping_GData  *gdata = &((CycleCpuTimeMapping_GData  *)gdataPtr)[i];
      Coord temp;

      temp = (gdata->shapeAttr_0) ;
      if (temp > width) width = temp;


      temp = (gdata->shapeAttr_1) ;
      if (temp > height) height = temp;

    }

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


    win->SetFgColor((GlobalColor) (map->GetDefaultColor()) );


    if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
			
      int i = 0;
      while (i < numSyms) {

	CycleCpuTimeMapping_GData  *gdata = (CycleCpuTimeMapping_GData  *)gdataArray[i];
	int count = 1;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;




	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  CycleCpuTimeMapping_GData  *gdata = (CycleCpuTimeMapping_GData  *)gdataArray[colorIndex];




	  _x[count] = (gdata->x) ;
	  _y[count++] = (gdata->y) ;
	}




	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      int i = 0;
      while (i < numSyms) {
	CycleCpuTimeMapping_GData  *gdata = (CycleCpuTimeMapping_GData  *)gdataArray[i];
	int count = 1;
	Coord width = (gdata->shapeAttr_0) ;
	Coord height = (gdata->shapeAttr_1) ;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;
	if (width > 1)
	  _x[0] -= width / 2.0;
	if (height > 1)
	  _y[0] -= height / 2.0;
	_width[0] = width;
	_height[0] = height;





	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  CycleCpuTimeMapping_GData  *gdata = (CycleCpuTimeMapping_GData  *)gdataArray[colorIndex];




	  width = (gdata->shapeAttr_0) ;
	  height = (gdata->shapeAttr_1) ;
	  _x[count] = (gdata->x) ;
	  _y[count] = (gdata->y) ;
	  if (width > 1)
	    _x[count] -= width / 2.0;
	  if (height > 1)
	    _y[count] -= height / 2.0;

	  _width[count] = width;
	  _height[count] = height;
	  
	  count++;
	}




	win->FillRectArray(_x, _y, _width, _height, count);
	i = colorIndex;
      }
    }
  }
};

const int CycleCpuTimeMapping_RectShapeID = 0;

class CycleCpuTimeMapping : public TDataMap {
public:
	CycleCpuTimeMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMap("CycleCpuTimeMapping", tdata, gdataName, sizeof(CycleCpuTimeMapping_GData), 0|VISUAL_X|VISUAL_Y, 3, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    double attrs[2];
    attrs[0] = 5; attrs[1] = 250;
    TDataMap::SetDefaultShape(CycleCpuTimeMapping_RectShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("black"));

    /*
       offsets of GData attributes must correspond to the dynamic
       GData attributes listed in the next section
    */
    GDataAttrOffset *_offsets = new GDataAttrOffset;
    _offsets->xOffset = 0;
    _offsets->yOffset = sizeof(double);
    _offsets->zOffset = -1;
    _offsets->colorOffset = _offsets->sizeOffset = _offsets->shapeOffset = -1;
    _offsets->patternOffset = _offsets->orientationOffset = -1;
    _offsets->shapeAttrOffset[0] = 2 * sizeof(double);
    _offsets->shapeAttrOffset[1] = 3 * sizeof(double);
    for(int i = 2; i < MAX_GDATA_ATTRS; i++)
      _offsets->shapeAttrOffset[i] = -1;
    TDataMap::SetGDataOffset(_offsets);
  
	_shapes[0] = new CycleCpuTimeMapping_RectShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		char *tptr = (char *)buf;
		for(int i = 0; i < numRecs; i++) {
			CycleCpuTimeMapping_GData *symbol = (CycleCpuTimeMapping_GData *)gBuf;
			CycleRec *data = (CycleRec *)tptr;
			tptr += tRecSize;

    symbol->x = (((float)(data->time + data->last_time)) / 2.0);
    symbol->shapeAttr_0 = data->time - data->last_time;
    symbol->y = data->cpu_time;
    /*		symbol->shapeAttr_1 = symbol->y / 20; */
    symbol->shapeAttr_1 = 300;
  
			recId++;
			gBuf += gRecSize;;
		}
	}

	virtual void UpdateMaxSymSize(void *gdata, int numSyms) {
		_maxSymWidth = 0;
		_maxSymHeight = 0;
		_maxSymDepth = 0;
		_shapes[0]->MaxSymSize(this, gdata, numSyms,
		                       _maxSymWidth, _maxSymHeight);
	}

	virtual void DrawGDataArray(ViewGraph *view, WindowRep *win, void **syms, int numSyms) {
		_shapes[0]->DrawGDataArray(win, syms, numSyms, this, view, TDataMap::GetPixelWidth());
	}

private:
	void *userData;
	Shape *_shapes[1];
};
