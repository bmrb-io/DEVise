/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:15:39  jussi
  Added CVS header.
*/

struct CpuMapping_GData {
	double x;
	double y;
	Color color;
};








































class CpuMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		CpuMapping_GData *gdata = (CpuMapping_GData *)gdataArray[0];
		width = (( map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  ){
			
			i= 0;
			while (i < numSyms){
				CpuMapping_GData *gdata = (CpuMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					CpuMapping_GData *colorGData = (CpuMapping_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((gdata->color));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				CpuMapping_GData *gdata = (CpuMapping_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((gdata->color));
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
const int CpuMapping_RectXShapeID = 0;
class CpuMapping : public TDataMapDispatch{
public:
	CpuMapping(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("CpuMapping", tdata, sizeof(CpuMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 1.0; attrs[1] = 1.0;
		TDataMap::SetDefaultShape(CpuMapping_RectXShapeID,2,attrs);
	
	_shapes[0] = new CpuMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				CpuMapping_GData *symbol = (CpuMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tptr;
				tptr += tRecSize;

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->cpu;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				CpuMapping_GData *symbol = (CpuMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tRecs[i];

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->cpu;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
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


struct UserMapping_GData {
	double x;
	double y;
	Color color;
};








































class UserMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		UserMapping_GData *gdata = (UserMapping_GData *)gdataArray[0];
		width = (( map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  ){
			
			i= 0;
			while (i < numSyms){
				UserMapping_GData *gdata = (UserMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					UserMapping_GData *colorGData = (UserMapping_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((gdata->color));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				UserMapping_GData *gdata = (UserMapping_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((gdata->color));
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
const int UserMapping_RectXShapeID = 0;
class UserMapping : public TDataMapDispatch{
public:
	UserMapping(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("UserMapping", tdata, sizeof(UserMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 1.0; attrs[1] = 1.0;
		TDataMap::SetDefaultShape(UserMapping_RectXShapeID,2,attrs);
	
	_shapes[0] = new UserMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				UserMapping_GData *symbol = (UserMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tptr;
				tptr += tRecSize;

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->user;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				UserMapping_GData *symbol = (UserMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tRecs[i];

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->user;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
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


struct IoMapping_GData {
	double x;
	double y;
	Color color;
};








































class IoMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		IoMapping_GData *gdata = (IoMapping_GData *)gdataArray[0];
		width = (( map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  ){
			
			i= 0;
			while (i < numSyms){
				IoMapping_GData *gdata = (IoMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					IoMapping_GData *colorGData = (IoMapping_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((gdata->color));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				IoMapping_GData *gdata = (IoMapping_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((gdata->color));
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
const int IoMapping_RectXShapeID = 0;
class IoMapping : public TDataMapDispatch{
public:
	IoMapping(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("IoMapping", tdata, sizeof(IoMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 1.0; attrs[1] = 1.0;
		TDataMap::SetDefaultShape(IoMapping_RectXShapeID,2,attrs);
	
	_shapes[0] = new IoMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				IoMapping_GData *symbol = (IoMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tptr;
				tptr += tRecSize;

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->io;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				IoMapping_GData *symbol = (IoMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tRecs[i];

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->io;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
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


struct TotalMapping_GData {
	double x;
	double y;
	Color color;
};








































class TotalMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		TotalMapping_GData *gdata = (TotalMapping_GData *)gdataArray[0];
		width = (( map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  ){
			
			i= 0;
			while (i < numSyms){
				TotalMapping_GData *gdata = (TotalMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					TotalMapping_GData *colorGData = (TotalMapping_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((gdata->color));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				TotalMapping_GData *gdata = (TotalMapping_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform(((map->GetDefaultShapeAttrs())[0]),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = ((map->GetDefaultShapeAttrs())[1])/((map->GetDefaultShapeAttrs())[0])*width;

				win->SetFgColor((gdata->color));
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
const int TotalMapping_RectXShapeID = 0;
class TotalMapping : public TDataMapDispatch{
public:
	TotalMapping(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("TotalMapping", tdata, sizeof(TotalMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 1.0; attrs[1] = 1.0;
		TDataMap::SetDefaultShape(TotalMapping_RectXShapeID,2,attrs);
	
	_shapes[0] = new TotalMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				TotalMapping_GData *symbol = (TotalMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tptr;
				tptr += tRecSize;

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->user+ data->cpu + data->io;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				TotalMapping_GData *symbol = (TotalMapping_GData *)gdataBuf;
				StrData *data = (StrData *)tRecs[i];

		StringHash *hashTbl = (StringHash *)userData;
		symbol->x = hashTbl->InsertHashTable(data->name);
		symbol->y = data->user+ data->cpu + data->io;
		if (data->cpu > data->io)
			symbol->color = red;
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
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

