
struct FullMapping_GData {
	double x;
	double y;
	Color color;
	double orientation;
};

const int FullMapping_RectXShapeID = 0;
class FullMapping : public TDataMapDispatch{
public:
	FullMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("FullMapping", tdata, gdataName, sizeof(FullMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR|VISUAL_ORIENTATION,0, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

		double attrs[2];
	
	_shapes[0] = new FullMapping_RectXShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				FullMapping_GData *symbol = (FullMapping_GData *)gdataBuf;
				GDataRec *data = (GDataRec *)tptr;
				tptr += tRecSize;

		int numDisks = 0;
	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				FullMapping_GData *symbol = (FullMapping_GData *)gdataBuf;
				GDataRec *data = (GDataRec *)tRecs[i];

		int numDisks = 0;
	
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

