struct CycleSuspendedMapping_GData {
	double x;
	double y;
	double shapeAttr_0;
};

# 1 "/p/devise/parser/proto/RectShape_proto.h"
 













 






































class CycleSuspendedMapping_RectShape  : public RectShape {
public:
  virtual void BoundingBoxGData(TDataMap *map, void **gdataArray, int numSyms,
				Coord &width, Coord &height) {

    width = 0.0;






    height = ((map->GetDefaultShapeAttrs())[1]) ;



    for(int i = 0; i < numSyms; i++) {
      CycleSuspendedMapping_GData  *gdata = (CycleSuspendedMapping_GData  *)gdataArray[i];
      Coord temp;

      temp = (gdata->shapeAttr_0) ;
      if (temp > width) width = temp;





    }

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


    win->SetFgColor((map->GetDefaultColor()) );


    if (maxWidth <= pixelWidth && maxHeight <= pixelHeight) {
			
      int i = 0;
      while (i < numSyms) {

	CycleSuspendedMapping_GData  *gdata = (CycleSuspendedMapping_GData  *)gdataArray[i];
	int count = 1;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;




	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  CycleSuspendedMapping_GData  *gdata = (CycleSuspendedMapping_GData  *)gdataArray[colorIndex];




	  _x[count] = (gdata->x) ;
	  _y[count++] = (gdata->y) ;
	}




	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      int i = 0;
      while (i < numSyms) {
	CycleSuspendedMapping_GData  *gdata = (CycleSuspendedMapping_GData  *)gdataArray[i];
	int count = 1;
	Coord width = (gdata->shapeAttr_0) ;
	Coord height = ((map->GetDefaultShapeAttrs())[1]) ;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;
	if (width > 1)
	  _x[0] -= width / 2.0;
	if (height > 1)
	  _y[0] -= height / 2.0;
	_width[0] = width;
	_height[0] = height;





	for(int colorIndex = i+1; colorIndex < numSyms; colorIndex++) {
	  CycleSuspendedMapping_GData  *gdata = (CycleSuspendedMapping_GData  *)gdataArray[colorIndex];




	  width = (gdata->shapeAttr_0) ;
	  height = ((map->GetDefaultShapeAttrs())[1]) ;
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

const int CycleSuspendedMapping_RectShapeID = 0;

class CycleSuspendedMapping : public TDataMapDispatch {
public:
	CycleSuspendedMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMapDispatch("CycleSuspendedMapping", tdata, gdataName, sizeof(CycleSuspendedMapping_GData), 0|VISUAL_X|VISUAL_Y, 1, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    double attrs[2];
    attrs[0] = 5; attrs[1] = 250;
    TDataMap::SetDefaultShape(CycleSuspendedMapping_RectShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("black"));
  
	_shapes[0] = new CycleSuspendedMapping_RectShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				CycleSuspendedMapping_GData *symbol = (CycleSuspendedMapping_GData *)gBuf;
				CycleRec *data = (CycleRec *)tptr;
				tptr += tRecSize;

    symbol->x = (((float)(data->time + data->last_time)) / 2.0);
    symbol->shapeAttr_0 = data->time - data->last_time;
    symbol->y = data->suspended;
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				CycleSuspendedMapping_GData *symbol = (CycleSuspendedMapping_GData *)gBuf;
				CycleRec *data = (CycleRec *)tRecs[i];

    symbol->x = (((float)(data->time + data->last_time)) / 2.0);
    symbol->shapeAttr_0 = data->time - data->last_time;
    symbol->y = data->suspended;
  
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
