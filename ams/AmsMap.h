struct SolDnCompiled_GData {
	double x;
	double y;
};

# 1 "/p/devise/parser/proto/RectXShape_proto.h"
 













 
















































class SolDnCompiled_RectXShape  : public RectXShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {



    width = ((map->GetDefaultShapeAttrs())[0]) ;




    height = ((map->GetDefaultShapeAttrs())[1]) ;


# 92 "/p/devise/parser/proto/RectXShape_proto.h"

  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		
    if (view->GetNumDimensions() == 3)
      return;

    Coord maxWidth, maxHeight, maxDepth;
    map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);


    win->SetFgColor((map->GetDefaultColor()) );


    win->SetPattern((map->GetDefaultPattern()) );


    if (maxWidth <= pixelWidth) {
			
      int i = 0;
      while (i < numSyms) {

	SolDnCompiled_GData  *gdata = (SolDnCompiled_GData  *)gdataArray[i];
	int count = 1;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;




	int colorIndex;
	for(colorIndex = i + 1; colorIndex < numSyms; colorIndex++) {
	  SolDnCompiled_GData  *gdata = (SolDnCompiled_GData  *)gdataArray[colorIndex];




	  _x[count] = (gdata->x) ;
	  _y[count++] = (gdata->y) ;
	}




	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      Coord x1, y1, x2, y2;
      win->Transform(0.0, 0.0, x1, y1);

      for(int i = 0; i < numSyms; i++) {
	SolDnCompiled_GData  *gdata = (SolDnCompiled_GData  *)gdataArray[i];
	Coord tx, ty;
	Coord width, height;
	win->Transform((gdata->x) , (gdata->y) , tx, ty);
	win->Transform(((map->GetDefaultShapeAttrs())[0]) , 0.0, x2, y2);
	width = x2 - x1;
	if (width < 0.0)
	  width = -width;
	height = ((map->GetDefaultShapeAttrs())[1])  / ((map->GetDefaultShapeAttrs())[0])  * width;
	






	if (width < pixelSize)
	  width = pixelSize;
	if (height < pixelSize)
	  height = pixelSize;
	win->FillPixelRect(tx, ty, width, height);
      }
    }
  }
};

const int SolDnCompiled_RectXShapeID = 0;

class SolDnCompiled : public TDataMap {
public:
	SolDnCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMap("SolDnCompiled", tdata, gdataName, sizeof(SolDnCompiled_GData), 0|VISUAL_X|VISUAL_Y, 0, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    AttrList *attrList = new AttrList("SolDnCompiled_Attrs");
    attrList->InsertAttr(0, "x", 0, sizeof(double), DateAttr, false, 
			 NULL, false, true);
    attrList->InsertAttr(1, "y", 8, sizeof(double), DoubleAttr, false, 
			 NULL, false, false);
    SetGDataAttrList(attrList);
    
    double attrs[2];
    attrs[0] = 0.5; attrs[1] = 0.5;
    TDataMap::SetDefaultShape(SolDnCompiled_RectXShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("blue"));
  
	_shapes[0] = new SolDnCompiled_RectXShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				SolDnCompiled_GData *symbol = (SolDnCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->TOTAL_INCIDENT_RADTN;
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				SolDnCompiled_GData *symbol = (SolDnCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->TOTAL_INCIDENT_RADTN;
  
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

	virtual void DrawGDataArray(View *view, WindowRep *win, void **syms, int numSyms) {
		_shapes[0]->DrawGDataArray(win, syms, numSyms, this, view, TDataMap::GetPixelWidth());
	}

private:
	void *userData;
	Shape *_shapes[1];
};
struct RainCompiled_GData {
	double x;
	double y;
};

# 1 "/p/devise/parser/proto/BarShape_proto.h"
 













 













































class RainCompiled_BarShape  : public BarShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {



    width = ((map->GetDefaultShapeAttrs())[0]) ;


    height = 0;




# 89 "/p/devise/parser/proto/BarShape_proto.h"

  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		
    if (view->GetNumDimensions() == 3)
      return;

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);


    win->SetFgColor((map->GetDefaultColor()) );


    win->SetPattern((map->GetDefaultPattern()) );


    for(int i = 0; i < numSyms; i++) {
      RainCompiled_GData  *gdata = (RainCompiled_GData  *)gdataArray[i];






      Coord y = (gdata->y) ;
      Coord width = ((map->GetDefaultShapeAttrs())[0]) ;

      if (y >= 0.0) {
	if (width > pixelWidth)
	  win->FillRect((gdata->x)  - width / 2.0, 0.0, width, y);
	else
	  win->FillRect((gdata->x) , 0.0, width, y);
      } else {
	if (width > pixelWidth)
	  win->FillRect((gdata->x)  - width / 2.0, y, width, -y);
	else
	  win->FillRect((gdata->x) , y, width, -y);
      }
    }
  }
};

const int RainCompiled_BarShapeID = 0;

class RainCompiled : public TDataMap {
public:
	RainCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMap("RainCompiled", tdata, gdataName, sizeof(RainCompiled_GData), 0|VISUAL_X|VISUAL_Y, 0, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    AttrList *attrList = new AttrList("RainCompiled_Attrs");
    attrList->InsertAttr(0, "x", 0, sizeof(double), DateAttr, false, 
			 NULL, false, true);
    attrList->InsertAttr(1, "y", 8, sizeof(double), DoubleAttr, false, 
			 NULL, false, false);
    SetGDataAttrList(attrList);
    double attrs[2];
    attrs[0] = 0.5; attrs[1] = 0.5;
    TDataMap::SetDefaultShape(RainCompiled_BarShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("red"));
  
	_shapes[0] = new RainCompiled_BarShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				RainCompiled_GData *symbol = (RainCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->ACCUM_RAINFALL;
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				RainCompiled_GData *symbol = (RainCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->ACCUM_RAINFALL;
  
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

	virtual void DrawGDataArray(View *view, WindowRep *win, void **syms, int numSyms) {
		_shapes[0]->DrawGDataArray(win, syms, numSyms, this, view, TDataMap::GetPixelWidth());
	}

private:
	void *userData;
	Shape *_shapes[1];
};
struct TempCompiled_GData {
	double x;
	double y;
};

# 1 "/p/devise/parser/proto/RectXShape_proto.h"
 













 
















































class TempCompiled_RectXShape  : public RectXShape {
public:
  virtual void MaxSymSize(TDataMap *map, void *gdataPtr, int numSyms,
                          Coord &width, Coord &height) {



    width = ((map->GetDefaultShapeAttrs())[0]) ;




    height = ((map->GetDefaultShapeAttrs())[1]) ;


# 92 "/p/devise/parser/proto/RectXShape_proto.h"

  }

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
			      TDataMap *map, View *view, int pixelSize) {
		
    if (view->GetNumDimensions() == 3)
      return;

    Coord maxWidth, maxHeight, maxDepth;
    map->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

    Coord x0, y0, x1, y1;
    win->Transform(0, 0, x0, y0);
    win->Transform(1, 1, x1, y1);
    Coord pixelWidth = 1 / fabs(x1 - x0);


    win->SetFgColor((map->GetDefaultColor()) );


    win->SetPattern((map->GetDefaultPattern()) );


    if (maxWidth <= pixelWidth) {
			
      int i = 0;
      while (i < numSyms) {

	TempCompiled_GData  *gdata = (TempCompiled_GData  *)gdataArray[i];
	int count = 1;
	_x[0] = (gdata->x) ;
	_y[0] = (gdata->y) ;




	int colorIndex;
	for(colorIndex = i + 1; colorIndex < numSyms; colorIndex++) {
	  TempCompiled_GData  *gdata = (TempCompiled_GData  *)gdataArray[colorIndex];




	  _x[count] = (gdata->x) ;
	  _y[count++] = (gdata->y) ;
	}




	win->DrawPixelArray(_x, _y, count, pixelSize);
	i = colorIndex;
      }

    } else {

      Coord x1, y1, x2, y2;
      win->Transform(0.0, 0.0, x1, y1);

      for(int i = 0; i < numSyms; i++) {
	TempCompiled_GData  *gdata = (TempCompiled_GData  *)gdataArray[i];
	Coord tx, ty;
	Coord width, height;
	win->Transform((gdata->x) , (gdata->y) , tx, ty);
	win->Transform(((map->GetDefaultShapeAttrs())[0]) , 0.0, x2, y2);
	width = x2 - x1;
	if (width < 0.0)
	  width = -width;
	height = ((map->GetDefaultShapeAttrs())[1])  / ((map->GetDefaultShapeAttrs())[0])  * width;
	






	if (width < pixelSize)
	  width = pixelSize;
	if (height < pixelSize)
	  height = pixelSize;
	win->FillPixelRect(tx, ty, width, height);
      }
    }
  }
};

const int TempCompiled_RectXShapeID = 0;

class TempCompiled : public TDataMap {
public:
	TempCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions, int maxGDataPages= -1, void *uData = NULL) : TDataMap("TempCompiled", tdata, gdataName, sizeof(TempCompiled_GData), 0|VISUAL_X|VISUAL_Y, 0, maxGDataPages, dimensionInfo, numDimensions) {

		userData = uData;

    AttrList *attrList = new AttrList("TempCompiled_Attrs");
    attrList->InsertAttr(0, "x", 0, sizeof(double), DateAttr, false, 
			 NULL, false, true);
    attrList->InsertAttr(1, "y", 8, sizeof(double), DoubleAttr, false, 
			 NULL, false, false);
    SetGDataAttrList(attrList);
    double attrs[2];
    attrs[0] = 0.5; attrs[1] = 0.5;
    TDataMap::SetDefaultShape(TempCompiled_RectXShapeID,2,attrs);
    TDataMap::SetDefaultColor(ColorMgr::AllocColor("red"));
  
	_shapes[0] = new TempCompiled_RectXShape;
	}

	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf) {
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize > 0) {
			char *tptr = (char *)buf;
			for(int i = 0; i < numRecs; i++) {
				TempCompiled_GData *symbol = (TempCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->DOME_TEMP;
  
				recId++;
				gBuf += gRecSize;;
			}
		} else {
			for(int i = 0; i < numRecs; i++) {
				TempCompiled_GData *symbol = (TempCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

    symbol->x = (double)data->DATE;
    symbol->y = (double)data->DOME_TEMP;
  
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

	virtual void DrawGDataArray(View *view, WindowRep *win, void **syms, int numSyms) {
		_shapes[0]->DrawGDataArray(win, syms, numSyms, this, view, TDataMap::GetPixelWidth());
	}

private:
	void *userData;
	Shape *_shapes[1];
};
