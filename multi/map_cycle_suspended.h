
struct CycleSuspendedMapping_GData {
	double x;
	double y;
	double shapeAttr_0;
};









































class CycleSuspendedMapping_RectShape: public RectShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){


		width = 0.0;



		height = (( map->GetDefaultShapeAttrs())[1]);


		int i;
		for (i=0; i < numSyms; i++){
			CycleSuspendedMapping_GData *gdata = (CycleSuspendedMapping_GData *)gdataArray[i];
			Coord temp ;


			temp = (gdata->shapeAttr_0);
			if (temp > width) width = temp;



		}

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  && maxHeight <= yPerPixel ){
			
			i= 0;
			while (i < numSyms){
				CycleSuspendedMapping_GData *gdata = (CycleSuspendedMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					CycleSuspendedMapping_GData *colorGData = (CycleSuspendedMapping_GData *)gdataArray[colorIndex];
					if ((map->GetDefaultColor())!= (map->GetDefaultColor()))
						break;
					_x[count] = (colorGData->x);
					_y[count++] = (colorGData->y);
				}

				win->SetFgColor((map->GetDefaultColor()));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			

			i= 0;
			while (i < numSyms){
				CycleSuspendedMapping_GData *gdata = (CycleSuspendedMapping_GData *)gdataArray[i];
				int count = 1;
				Coord width = (gdata->shapeAttr_0);
				Coord height = ((map->GetDefaultShapeAttrs())[1]);

				_x[0] = (gdata->x)- width/2.0;
				_y[0] = (gdata->y) - height/2.0;
				_width[0] = width;
				_height[0] = height;

				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					CycleSuspendedMapping_GData *colorGData = (CycleSuspendedMapping_GData *)gdataArray[colorIndex];
					if ((map->GetDefaultColor())!= (map->GetDefaultColor()))
						break;


					width = (colorGData->shapeAttr_0);
					height = ((map->GetDefaultShapeAttrs())[1]);
					_x[count] = (colorGData->x)- width/2.0;
					_y[count] = (colorGData->y)- height/2.0;
					_width[count] = width;
					_height[count] = height;

					count++;
				}

				win->SetFgColor((map->GetDefaultColor()));
				win->FillRectArray(_x,_y,_width, _height,count);
	
				i = colorIndex;
			}


		}
	}
};
const int CycleSuspendedMapping_RectShapeID = 0;
class CycleSuspendedMapping : public TDataMapDispatch{
public:
	CycleSuspendedMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("CycleSuspendedMapping", tdata, gdataName, sizeof(CycleSuspendedMapping_GData), 0|VISUAL_X|VISUAL_Y,1, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 5; attrs[1] = 250;
		TDataMap::SetDefaultShape(CycleSuspendedMapping_RectShapeID,2,attrs);
		TDataMap::SetDefaultColor(ColorMgr::AllocColor("black"));
	
	_shapes[0] = new CycleSuspendedMapping_RectShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				CycleSuspendedMapping_GData *symbol = (CycleSuspendedMapping_GData *)gdataBuf;
				CycleRec *data = (CycleRec *)tptr;
				tptr += tRecSize;

		symbol->x = (((double)(data->time + data->last_time)) / 2.0);
		symbol->shapeAttr_0 = data->time - data->last_time;
		symbol->y = data->suspended;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				CycleSuspendedMapping_GData *symbol = (CycleSuspendedMapping_GData *)gdataBuf;
				CycleRec *data = (CycleRec *)tRecs[i];

		symbol->x = (((double)(data->time + data->last_time)) / 2.0);
		symbol->shapeAttr_0 = data->time - data->last_time;
		symbol->y = data->suspended;
	
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

