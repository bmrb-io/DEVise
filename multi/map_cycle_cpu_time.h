
struct CycleCpuTimeMapping_GData {
	double x;
	double y;
	double shapeAttr_0;
	double shapeAttr_1;
};










































class CycleCpuTimeMapping_RectShape: public RectShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){


		width = 0.0;



		height = 0.0;


		int i;
		for (i=0; i < numSyms; i++){
			CycleCpuTimeMapping_GData *gdata = (CycleCpuTimeMapping_GData *)gdataArray[i];
			Coord temp ;


			temp = (gdata->shapeAttr_0);
			if (temp > width) width = temp;



			temp = (gdata->shapeAttr_1);
			if (temp > height) height = temp;

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
				CycleCpuTimeMapping_GData *gdata = (CycleCpuTimeMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					CycleCpuTimeMapping_GData *colorGData = (CycleCpuTimeMapping_GData *)gdataArray[colorIndex];
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
				CycleCpuTimeMapping_GData *gdata = (CycleCpuTimeMapping_GData *)gdataArray[i];
				int count = 1;
				Coord width = (gdata->shapeAttr_0);
				Coord height = (gdata->shapeAttr_1);

				_x[0] = (gdata->x)- width/2.0;
				_y[0] = (gdata->y) - height/2.0;
				_width[0] = width;
				_height[0] = height;

				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					CycleCpuTimeMapping_GData *colorGData = (CycleCpuTimeMapping_GData *)gdataArray[colorIndex];
					if ((map->GetDefaultColor())!= (map->GetDefaultColor()))
						break;


					width = (colorGData->shapeAttr_0);
					height = (colorGData->shapeAttr_1);
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
const int CycleCpuTimeMapping_RectShapeID = 0;
class CycleCpuTimeMapping : public TDataMapDispatch{
public:
	CycleCpuTimeMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("CycleCpuTimeMapping", tdata, gdataName, sizeof(CycleCpuTimeMapping_GData), 0|VISUAL_X|VISUAL_Y,3, maxGDataPages,dimensionInfo,numDimensions){

		printf("new CycleCputimeMapping(%s, %s)\n",
			tdata->GetName(), gdataName);

		userData = uData;

		printf("2\n");

		double attrs[2];
		printf("2.5\n");
		attrs[0] = 5.0; 
		printf("3\n");
		attrs[1] = 250.0;
		printf("3.5\n");
		TDataMap::SetDefaultShape(CycleCpuTimeMapping_RectShapeID,2,attrs);
		printf("4\n");
		TDataMap::SetDefaultColor(ColorMgr::AllocColor("black"));
		printf("5\n");
	
	_shapes[0] = new CycleCpuTimeMapping_RectShape;
		printf("6\n");
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				CycleCpuTimeMapping_GData *symbol = (CycleCpuTimeMapping_GData *)gdataBuf;
				CycleRec *data = (CycleRec *)tptr;
				tptr += tRecSize;

		symbol->x = (((double)(data->time + data->last_time)) / 2.0);
		symbol->shapeAttr_0 = data->time - data->last_time;
		symbol->y = data->cpu_time;
/*		symbol->shapeAttr_1 = symbol->y / 20; */
		symbol->shapeAttr_1 = 300;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				CycleCpuTimeMapping_GData *symbol = (CycleCpuTimeMapping_GData *)gdataBuf;
				CycleRec *data = (CycleRec *)tRecs[i];

		symbol->x = (((double)(data->time + data->last_time)) / 2.0);
		symbol->shapeAttr_0 = data->time - data->last_time;
		symbol->y = data->cpu_time;
/*		symbol->shapeAttr_1 = symbol->y / 20; */
		symbol->shapeAttr_1 = 300;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){
		printf("UpdateBoundingBox(page %d, numsyms %d)\n",
			pageNum, numSyms);
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

