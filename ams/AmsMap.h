
struct SolDnCompiled_GData {
	double x;
	double y;
};

 







class SolDnCompiled_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		SolDnCompiled_GData *gdata = (SolDnCompiled_GData *)gdataArray[0];
		width = ((map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		 
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel   ){
			 
			i= 0;
			while (i < numSyms){
				SolDnCompiled_GData *gdata = (SolDnCompiled_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					SolDnCompiled_GData *colorGData = (SolDnCompiled_GData *)gdataArray[colorIndex];
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((map->GetDefaultColor()));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			 
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				SolDnCompiled_GData *gdata = (SolDnCompiled_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((map->GetDefaultColor()));
				win->SetPattern((map->GetDefaultPattern()));
				if (width < pixelWidth)
					width = pixelWidth;
				if (height < pixelWidth)
					height = pixelWidth;
				win->FillPixelRect(tx,ty,width,height);
			}
		}
	}
};
const int SolDnCompiled_RectXShapeID = 0;
class SolDnCompiled : public TDataMapDispatch{
public:
	SolDnCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("SolDnCompiled", tdata, gdataName, sizeof(SolDnCompiled_GData), 0|VISUAL_X|VISUAL_Y,0, maxGDataPages,dimensionInfo,numDimensions){
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
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				SolDnCompiled_GData *symbol = (SolDnCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->TOTAL_INCIDENT_RADTN;
	
				recId++;
				gBuf += gRecSize;;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				SolDnCompiled_GData *symbol = (SolDnCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->TOTAL_INCIDENT_RADTN;
	
				recId++;
				gBuf += gRecSize;
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){
		if (TDataMap::TestAndSetPage(pageNum)) return;
		Coord width, height;
		_shapes[0]->BoundingBoxGData(this, syms,numSyms,width,height);
		TDataMap::UpdateBoundingBox(width, height);
	}
	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms, Coord xPerPixel, Coord yPerPixel){
		_shapes[0]->DrawGDataArray(win,syms,numSyms,this,xPerPixel, yPerPixel, TDataMap::GetPixelWidth());
	}
private:
	void *userData;
	Shape *_shapes[1];
};


struct RainCompiled_GData {
	double x;
	double y;
};

 







class RainCompiled_BarShape: public BarShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		width = 0.0;
		height = 0.0;
		int i;
		for (i = 0; i < numSyms; i++){
			RainCompiled_GData *gdata = (RainCompiled_GData *)gdataArray[i];
			Coord curWidth = ((map->GetDefaultShapeAttrs())[0]);
			if (curWidth > width)
				width = curWidth;
			 
			Coord curHeight = fabs((gdata->y));
			if (height > curHeight)
				height = curHeight;
		}
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		 
		int i;
		win->SetFgColor((map->GetDefaultColor()));
		win->SetPattern((map->GetDefaultPattern()));
		for (i=0; i < numSyms; i++){
			RainCompiled_GData *gdata = (RainCompiled_GData *)gdataArray[i];

			Coord y = (gdata->y);
			if (y >= 0.0)
				win->FillRect((gdata->x),0.0,
					((map->GetDefaultShapeAttrs())[0]), y);
			else
				win->FillRect((gdata->x),y,
					((map->GetDefaultShapeAttrs())[0]), -y);
		}
	}
};
const int RainCompiled_BarShapeID = 0;
class RainCompiled : public TDataMapDispatch{
public:
	RainCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("RainCompiled", tdata, gdataName, sizeof(RainCompiled_GData), 0|VISUAL_X|VISUAL_Y,0, maxGDataPages,dimensionInfo,numDimensions){
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
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				RainCompiled_GData *symbol = (RainCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->ACCUM_RAINFALL;
	
				recId++;
				gBuf += gRecSize;;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				RainCompiled_GData *symbol = (RainCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->ACCUM_RAINFALL;
	
				recId++;
				gBuf += gRecSize;
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){
		if (TDataMap::TestAndSetPage(pageNum)) return;
		Coord width, height;
		_shapes[0]->BoundingBoxGData(this, syms,numSyms,width,height);
		TDataMap::UpdateBoundingBox(width, height);
	}
	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms, Coord xPerPixel, Coord yPerPixel){
		_shapes[0]->DrawGDataArray(win,syms,numSyms,this,xPerPixel, yPerPixel, TDataMap::GetPixelWidth());
	}
private:
	void *userData;
	Shape *_shapes[1];
};


struct TempCompiled_GData {
	double x;
	double y;
};

 







class TempCompiled_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		TempCompiled_GData *gdata = (TempCompiled_GData *)gdataArray[0];
		width = ((map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		 
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel   ){
			 
			i= 0;
			while (i < numSyms){
				TempCompiled_GData *gdata = (TempCompiled_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					TempCompiled_GData *colorGData = (TempCompiled_GData *)gdataArray[colorIndex];
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((map->GetDefaultColor()));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			 
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				TempCompiled_GData *gdata = (TempCompiled_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((map->GetDefaultColor()));
				win->SetPattern((map->GetDefaultPattern()));
				if (width < pixelWidth)
					width = pixelWidth;
				if (height < pixelWidth)
					height = pixelWidth;
				win->FillPixelRect(tx,ty,width,height);
			}
		}
	}
};
const int TempCompiled_RectXShapeID = 0;
class TempCompiled : public TDataMapDispatch{
public:
	TempCompiled(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("TempCompiled", tdata, gdataName, sizeof(TempCompiled_GData), 0|VISUAL_X|VISUAL_Y,0, maxGDataPages,dimensionInfo,numDimensions){
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
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		int gRecSize= GDataRecordSize();
		char *gBuf= (char *)gdataBuf;
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				TempCompiled_GData *symbol = (TempCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tptr;
				tptr += tRecSize;

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->DOME_TEMP;
	
				recId++;
				gBuf += gRecSize;;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				TempCompiled_GData *symbol = (TempCompiled_GData *)gBuf;
				AmsRec *data = (AmsRec *)tRecs[i];

		symbol->x =  (double)data->DATE;
		symbol->y =  (double)data->DOME_TEMP;
	
				recId++;
				gBuf += gRecSize;
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){
		if (TDataMap::TestAndSetPage(pageNum)) return;
		Coord width, height;
		_shapes[0]->BoundingBoxGData(this, syms,numSyms,width,height);
		TDataMap::UpdateBoundingBox(width, height);
	}
	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms, Coord xPerPixel, Coord yPerPixel){
		_shapes[0]->DrawGDataArray(win,syms,numSyms,this,xPerPixel, yPerPixel, TDataMap::GetPixelWidth());
	}
private:
	void *userData;
	Shape *_shapes[1];
};

