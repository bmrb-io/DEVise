struct MultiMapping_GData {
	double x;
	double y;
	unsigned long int color;
	double shapeAttr_0;
	double shapeAttr_1;
};

# 1 "/p/devise/parser/proto/RectShape_proto.h"
 













 



















































class MultiMapping_RectShape  : public RectShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {

    width = 0.0;




    height = 0.0;





    for(int i = 0; i < numSyms; i++) {
      MultiMapping_GData  *gdata = &((MultiMapping_GData  *)gdataPtr)[i];
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





    if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
			
      int i = 0;
      while (i < numSyms) {

	MultiMapping_GData  *gdata = (MultiMapping_GData  *)gdataArray[i];
	int count = 1;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;

	Color lastColor = (gdata->color) ;


	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  MultiMapping_GData  *gdata = (MultiMapping_GData  *)gdataArray[colorIndex];

	  if ((gdata->color)  != lastColor)
	    break;

	  _x[count] = (gdata->x) ;
	  _y[count++] = (gdata->y) ;
	}


	win->SetFgColor(lastColor);

	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      int i = 0;
      while (i < numSyms) {
	MultiMapping_GData  *gdata = (MultiMapping_GData  *)gdataArray[i];
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


	Color lastColor = (gdata->color) ;


	int colorIndex;
	for(colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  MultiMapping_GData  *gdata = (MultiMapping_GData  *)gdataArray[colorIndex];

	  if ((gdata->color)  != lastColor)
	    break;

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


	win->SetFgColor((gdata->color) );

	win->FillRectArray(_x, _y, _width, _height, count);
	i = colorIndex;
      }
    }
  }
};

const int MultiMapping_RectShapeID = 0;

class MultiMapping : public TDataMap {
public:
	MultiMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMap("MultiMapping", tdata, gdataName, sizeof(MultiMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR, 3, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    double attrs[2];
    attrs[0] = 10.0; attrs[1] = 10.0;
    TDataMap::SetDefaultShape(MultiMapping_RectShapeID,2,attrs);
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
  
	_shapes[0] = new MultiMapping_RectShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				MultiMapping_GData *symbol = (MultiMapping_GData *)gBuf;
				MultiRec *data = (MultiRec *)tptr;
				tptr += tRecSize;

    MapInfo *mapInfo = (MapInfo *)userData;
    symbol->x = (((float)(data->begin+data->end))/2.0);
    symbol->y = (float)data->node + 0.5;
    if (data->job == 0 )
      /* suspended*/
      symbol->color = suspendColor;
    else if (data->job <0)
      /* aborted? */
      symbol->color = abortColor;
    else {
      int index;
      if (data->cycles == 0)
	index = 2;
      else
	index = (data->cycles -1) %3;
      index = (index + 1) % 3;
      symbol->color = 
	mapInfo->colorArrays[index][mapInfo->job_ordering[data->job]];
    }
#if 0
    printf("color %d job %d \n", symbol->color, data->job);
#endif

    symbol->shapeAttr_0 = data->end-data->begin+1;
    if (data->job == -1)
      symbol->shapeAttr_1 = 0.9;
    else
      symbol->shapeAttr_1 = 0.9;
#if 0
    printf("sym %d (%f,%f,%f,%f) color %d\n", recId,
	   symbol->x, symbol->y, symbol->shapeAttr_0, symbol->shapeAttr_1,
	   symbol->color);
#endif
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				MultiMapping_GData *symbol = (MultiMapping_GData *)gBuf;
				MultiRec *data = (MultiRec *)tRecs[i];

    MapInfo *mapInfo = (MapInfo *)userData;
    symbol->x = (((float)(data->begin+data->end))/2.0);
    symbol->y = (float)data->node + 0.5;
    if (data->job == 0 )
      /* suspended*/
      symbol->color = suspendColor;
    else if (data->job <0)
      /* aborted? */
      symbol->color = abortColor;
    else {
      int index;
      if (data->cycles == 0)
	index = 2;
      else
	index = (data->cycles -1) %3;
      index = (index + 1) % 3;
      symbol->color = 
	mapInfo->colorArrays[index][mapInfo->job_ordering[data->job]];
    }
#if 0
    printf("color %d job %d \n", symbol->color, data->job);
#endif

    symbol->shapeAttr_0 = data->end-data->begin+1;
    if (data->job == -1)
      symbol->shapeAttr_1 = 0.9;
    else
      symbol->shapeAttr_1 = 0.9;
#if 0
    printf("sym %d (%f,%f,%f,%f) color %d\n", recId,
	   symbol->x, symbol->y, symbol->shapeAttr_0, symbol->shapeAttr_1,
	   symbol->color);
#endif
  
				recId++;
				gBuf += gRecSize;
			}
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
