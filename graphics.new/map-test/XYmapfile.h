/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:16:26  jussi
  Added CVS header.
*/

struct XYMapping_GData {
	double x;
	double y;
	Color color;
};








































class XYMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		XYMapping_GData *gdata = (XYMapping_GData *)gdataArray[0];
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
				XYMapping_GData *gdata = (XYMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					XYMapping_GData *colorGData = (XYMapping_GData *)gdataArray[colorIndex];
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
				XYMapping_GData *gdata = (XYMapping_GData *)gdataArray[i];
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
const int XYMapping_RectXShapeID = 0;
class XYMapping : public TDataMapDispatch{
public:
	XYMapping(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("XYMapping", tdata, sizeof(XYMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 90.0; attrs[1] = 90.0;
		TDataMap::SetDefaultShape(XYMapping_RectXShapeID,2,attrs);
	
	_shapes[0] = new XYMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				XYMapping_GData *symbol = (XYMapping_GData *)gdataBuf;
				XYData *data = (XYData *)tptr;
				tptr += tRecSize;

		symbol->x = data->x;
		symbol->y = data->y;
		if (symbol->y > 0)
			symbol->color = *((Color *)userData);
		else symbol->color = BlackColor;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				XYMapping_GData *symbol = (XYMapping_GData *)gdataBuf;
				XYData *data = (XYData *)tRecs[i];

		symbol->x = data->x;
		symbol->y = data->y;
		if (symbol->y > 0)
			symbol->color = *((Color *)userData);
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

