
struct Mapping2D_GData {
	double x;
	double y;
	Color color;
};








































class Mapping2D_RectShape: public RectShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){

		Mapping2D_GData *gdata = (Mapping2D_GData *)gdataArray[0];
		width = (( map->GetDefaultShapeAttrs())[0]);
		height = ((map->GetDefaultShapeAttrs())[1]);

	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  && maxHeight <= yPerPixel ){
			
			i= 0;
			while (i < numSyms){
				Mapping2D_GData *gdata = (Mapping2D_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					Mapping2D_GData *colorGData = (Mapping2D_GData *)gdataArray[colorIndex];
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
			

			
			i= 0;
			Mapping2D_GData *gdata = (Mapping2D_GData *)gdataArray[0];
			Coord width = ((map->GetDefaultShapeAttrs())[0]);
			Coord height = ((map->GetDefaultShapeAttrs())[1]);
			Coord halfWidth = width/2.0;
			Coord halfHeight = height/2.0;

			while (i < numSyms){
				gdata = (Mapping2D_GData *)gdataArray[i];
				int count = 1;

				_x[0] = (gdata->x)- halfWidth;
				_y[0] = (gdata->y) - halfHeight;

				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					Mapping2D_GData *colorGData = (Mapping2D_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;

					_x[count] = (colorGData->x)- halfWidth;
					_y[count] = (colorGData->y)- halfHeight;

					count++;
				}

				win->SetFgColor((gdata->color));
				win->FillRectArray(_x,_y,width, height,count);
	
				i = colorIndex;
			}


		}
	}
};
const int Mapping2D_RectShapeID = 0;
class Mapping2D : public TDataMapDispatch{
public:
	Mapping2D(TData *tdata, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("Mapping2D", tdata, sizeof(Mapping2D_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
		attrs[0] = 1.0; attrs[1] = 1.0;
		TDataMap::SetDefaultShape(Mapping2D_RectShapeID,2,attrs);
	
	_shapes[0] = new Mapping2D_RectShape;
	}
	virtual int GDataRecordSize(){ return sizeof(Mapping2D_GData);}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void **gdataArray){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				Mapping2D_GData *symbol = (Mapping2D_GData *)gdataArray[i];
				char *data = (char *)tptr;
				tptr += tRecSize;

		int *indices;
		GetTData()->GetIndex(recId,indices);

		symbol->x = (Coord)indices[1];
		symbol->y = (Coord)indices[0];
		if (*((unsigned char *)data) > threshold)
			symbol->color = red;
		else
			symbol->color = BlackColor;
	
				recId++;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				Mapping2D_GData *symbol = (Mapping2D_GData *)gdataArray[i];
				char *data = (char *)tRecs[i];

		int *indices;
		GetTData()->GetIndex(recId,indices);

		symbol->x = (Coord)indices[1];
		symbol->y = (Coord)indices[0];
		if (*((unsigned char *)data) > threshold)
			symbol->color = red;
		else
			symbol->color = BlackColor;
	
				recId++;
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

