struct HostMapping_GData {
	double x;
	double y;
	unsigned long int color;
	double shapeAttr_0;
	double shapeAttr_1;
};

# 1 "/p/devise/parser/proto/RectShape_proto.h"
class HostMapping_RectShape  : public RectShape {
public:
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
				Coord &width, Coord &height) {

    width = 0.0;




    height = 0.0;





    for(int i = 0; i < numSyms; i++) {
      HostMapping_GData  *gdata = (HostMapping_GData  *)gdataArray[i];

      Coord temp = gdata->shapeAttr_0;
      if (temp > width) width = temp;


      temp = gdata->shapeAttr_1;
      if (temp > height) height = temp;

    }

  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, int pixelSize) {
		
    Coord maxWidth, maxHeight;
    map->MaxBoundingBox(maxWidth,maxHeight);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);
    Coord pixelHeight = 1 / fabs(y1 - y0);

    if (maxWidth <= pixelWidth  && maxHeight <= pixelHeight) {
			
      int i = 0;
      while (i < numSyms) {

	HostMapping_GData  *gdata = (HostMapping_GData  *)gdataArray[i];
	int count = 1;

	_x[0] = gdata->x;




	_y[0] = gdata->y;




	Color lastColor = gdata->color;


	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  HostMapping_GData  *colorGData = (HostMapping_GData  *)gdataArray[colorIndex];

	  if (colorGData->color != lastColor)
	    break;


	  _x[count] = colorGData->x;




	  _y[count++] = colorGData->y;



	}


	win->SetFgColor(lastColor);



	win->DrawPixelArray(_x, _y,count, pixelSize);
	i = colorIndex;
      }

    } else {

      int i = 0;
      while (i < numSyms) {
	HostMapping_GData  *gdata = (HostMapping_GData  *)gdataArray[i];
	int count = 1;

	Coord width = gdata->shapeAttr_0;




	Coord height = gdata->shapeAttr_1;




	_x[0] = gdata->x;




	_y[0] = gdata->y;



	if (width > 1)
	  _x[0] -= width / 2.0;
	if (height > 1)
	  _y[0] -= height / 2.0;
	_width[0] = width;
	_height[0] = height;


	Color lastColor = gdata->color;


	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  HostMapping_GData  *colorGData = (HostMapping_GData  *)gdataArray[colorIndex];

	  if (colorGData->color != lastColor)
	    break;



	  width = colorGData->shapeAttr_0;




	  height = colorGData->shapeAttr_1;




	  _x[count] = colorGData->x;




	  _y[count] = colorGData->y;



	  if (width > 1)
	    _x[count] -= width / 2.0;
	  if (height > 1)
	    _y[count] -= height / 2.0;

	  _width[count] = width;
	  _height[count] = height;
	  
	  count++;
	}


	win->SetFgColor(gdata->color);



	win->FillRectArray(_x, _y, _width, _height, count);
	i = colorIndex;
      }
    }
  }
};

const int HostMapping_RectShapeID = 0;

class HostMapping : public TDataMapDispatch {
public:
	HostMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMapDispatch("HostMapping", tdata, gdataName, sizeof(HostMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR, 3, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    double attrs[2];
    attrs[0] = 1; attrs[1] = 0.5;
    TDataMap::SetDefaultShape(HostMapping_RectShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("red"));
  
	_shapes[0] = new HostMapping_RectShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				HostMapping_GData *symbol = (HostMapping_GData *)gBuf;
				HostRec *data = (HostRec *)tptr;
				tptr += tRecSize;

    static int last_time = 10000;
    static int last_working = -1;
    /*
       MapInfo *mapInfo = (MapInfo *)userData;
    */
#if 1
    if (data->time > last_time) {
      symbol->x = (((float)(data->time + last_time)) / 2.0);
      symbol->shapeAttr_0 = data->time - last_time;
    } else {
      symbol->x = (float)(data->time);
      symbol->shapeAttr_0 = 2;
    }
    last_time = data->time;
    if (last_working > 0) {
      symbol->y = (((float)(data->hosts + last_working)) / 2.0 );
      symbol->shapeAttr_1 = data->hosts - last_working;
    } else {
      symbol->y = (((float) (data->hosts + data->working)) / 2.0 );
      symbol->shapeAttr_1 = data->hosts - data->working;
    }
    last_working = data->working;
#else
    
    symbol->shapeAttr_0 = 1;
    symbol->shapeAttr_1 = data->hosts - data->working;
    symbol->x = data->time;
    symbol->y = (((float) (data->hosts + data->working)) / 2.0 );
#endif
    
    symbol->color = some_idleColor;
    if (symbol->shapeAttr_1 < 1) {
      symbol->shapeAttr_1 = 0.5;
      symbol->y -= 0.25;
      symbol->color = all_busyColor;
    }
    
    /*
       printf("sym %d (%f,%f,%f,%f) color %d\n", recId,
       symbol->x, symbol->y, symbol->shapeAttr_0, symbol->shapeAttr_1,
       symbol->color);
    */
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				HostMapping_GData *symbol = (HostMapping_GData *)gBuf;
				HostRec *data = (HostRec *)tRecs[i];

    static int last_time = 10000;
    static int last_working = -1;
    /*
       MapInfo *mapInfo = (MapInfo *)userData;
    */
#if 1
    if (data->time > last_time) {
      symbol->x = (((float)(data->time + last_time)) / 2.0);
      symbol->shapeAttr_0 = data->time - last_time;
    } else {
      symbol->x = (float)(data->time);
      symbol->shapeAttr_0 = 2;
    }
    last_time = data->time;
    if (last_working > 0) {
      symbol->y = (((float)(data->hosts + last_working)) / 2.0 );
      symbol->shapeAttr_1 = data->hosts - last_working;
    } else {
      symbol->y = (((float) (data->hosts + data->working)) / 2.0 );
      symbol->shapeAttr_1 = data->hosts - data->working;
    }
    last_working = data->working;
#else
    
    symbol->shapeAttr_0 = 1;
    symbol->shapeAttr_1 = data->hosts - data->working;
    symbol->x = data->time;
    symbol->y = (((float) (data->hosts + data->working)) / 2.0 );
#endif
    
    symbol->color = some_idleColor;
    if (symbol->shapeAttr_1 < 1) {
      symbol->shapeAttr_1 = 0.5;
      symbol->y -= 0.25;
      symbol->color = all_busyColor;
    }
    
    /*
       printf("sym %d (%f,%f,%f,%f) color %d\n", recId,
       symbol->x, symbol->y, symbol->shapeAttr_0, symbol->shapeAttr_1,
       symbol->color);
    */
  
				recId++;
				gBuf += gRecSize;
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms) {
		if (TDataMap::TestAndSetPage(pageNum)) return;
		Coord width, height;
		_shapes[0]->BoundingBoxGData(this, syms, numSyms, width, height);
		TDataMap::UpdateBoundingBox(width, height);
	}

	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms) {
		_shapes[0]->DrawGDataArray(win, syms, numSyms, this, TDataMap::GetPixelWidth());
	}

private:
	void *userData;
	Shape *_shapes[1];
};
