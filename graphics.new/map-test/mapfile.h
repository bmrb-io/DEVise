/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
 */

struct FullMapping_GData {
	double x;
	double y;
	Color color;
	double size;
	Pattern pattern;
	ShapeID shapeID;
	double orientation;
	double shapeAttr_0;
	double shapeAttr_1;
};

 







class FullMapping_RectShape: public RectShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		width = 0.0;

		height = 0.0;

		int i;
		for (i=0; i < numSyms; i++){
			FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
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
				FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					FullMapping_GData *colorGData = (FullMapping_GData *)gdataArray[colorIndex];
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
			while (i < numSyms){
				FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
				int count = 1;
				Coord width = (gdata->shapeAttr_0);
				Coord height = (gdata->shapeAttr_1);

				_x[0] = (gdata->x)- width/2.0;
				_y[0] = (gdata->y) - height/2.0;
				_width[0] = width;
				_height[0] = height;

				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					FullMapping_GData *colorGData = (FullMapping_GData *)gdataArray[colorIndex];
					if ((colorGData->color)!= (gdata->color))
						break;


					width = (colorGData->shapeAttr_0);
					height = (colorGData->shapeAttr_1);
					_x[count] = (colorGData->x)- width/2.0;
					_y[count] = (colorGData->y)- height/2.0;
					_width[count] = width;
					_height[count] = height;

					count++;
				}

				win->SetFgColor((gdata->color));
				win->FillRectArray(_x,_y,_width, _height,count);
	
				i = colorIndex;
			}

		}
	}
};
 







class FullMapping_RectXShape: public RectXShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		width = 0.0;

		height = 0.0;

		int i;
		for (i=0; i < numSyms; i++){
			FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
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

		if (maxWidth <= xPerPixel   ){
			 
			i= 0;
			while (i < numSyms){
				FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
				int count = 1;
				_x[0] = (gdata->x);
				_y[0] = (gdata->y);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					FullMapping_GData *colorGData = (FullMapping_GData *)gdataArray[colorIndex];
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
				FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform((gdata->x),(gdata->y),tx,ty);
				win->Transform((gdata->shapeAttr_0),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = (gdata->shapeAttr_1)/(gdata->shapeAttr_0)*width;

				win->SetFgColor((gdata->color));
				win->SetPattern((gdata->pattern));
				if (width < pixelWidth)
					width = pixelWidth;
				if (height < pixelWidth)
					height = pixelWidth;
				win->FillPixelRect(tx,ty,width,height);
			}
		}
	}
};
 







class FullMapping_BarShape: public BarShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		width = 0.0;
		height = 0.0;
		int i;
		for (i = 0; i < numSyms; i++){
			FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
			Coord curWidth = (gdata->shapeAttr_0);
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
		for (i=0; i < numSyms; i++){
			FullMapping_GData *gdata = (FullMapping_GData *)gdataArray[i];
			win->SetFgColor((gdata->color));

			win->SetFgPattern((gdata->pattern));
			Coord y = (gdata->y);
			if (y >= 0.0)
				win->FillRect((gdata->x),0.0,
					(gdata->shapeAttr_0), y);
			else
				win->FillRect((gdata->x),y,
					(gdata->shapeAttr_0), -y);
		}
	}
};
const int FullMapping_RectShapeID = 0;
const int FullMapping_RectXShapeID = 1;
const int FullMapping_BarShapeID = 2;
class FullMapping : public TDataMapDispatch{
public:
	FullMapping(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch("FullMapping", tdata, gdataName, sizeof(FullMapping_GData), 0|VISUAL_X|VISUAL_Y|VISUAL_COLOR|VISUAL_SIZE|VISUAL_PATTERN|VISUAL_SHAPE|VISUAL_ORIENTATION,3, maxGDataPages,dimensionInfo,numDimensions){
		userData = uData;

	
	_shapes[0] = new FullMapping_RectShape;
	_shapes[1] = new FullMapping_RectXShape;
	_shapes[2] = new FullMapping_BarShape;
	}
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){
		int i;
		int tRecSize= TDataRecordSize();
		if (tRecSize >0){
			char *tptr = (char *)buf;
			for (i=0; i < numRecs; i++){
				FullMapping_GData *symbol = (FullMapping_GData *)gdataBuf;
				FullMapRec *data = (FullMapRec *)tptr;
				tptr += tRecSize;

	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();;
			}
		}
		else{
			for(i=0; i < numRecs; i++){
				FullMapping_GData *symbol = (FullMapping_GData *)gdataBuf;
				FullMapRec *data = (FullMapRec *)tRecs[i];

	
				recId++;
				gdataBuf = (char *)gdataBuf + GDataRecordSize();
			}
		}
	}

	virtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){
		if (TDataMap::TestAndSetPage(pageNum)) return;
		Coord width, height;
		int i = 0;
		while (i < numSyms){
			FullMapping_GData *sym = (FullMapping_GData *)syms[i];
			int symIndex;
			for (symIndex = i+incr; symIndex < numSyms; symIndex += incr){
				if(syms[symIndex]->shapeID != sym->shapeID)
					break;
			}
			/* syms[i..symIndex-incr] have the same shape */
			_shapes[sym->shapeID]->BoundingBoxGData(this,&syms[i],symIndex-i,width, height);
TDataMap::UpdateBoundingBox(width,height);
			i = symIndex;
		}
	}
	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms, Coord xPerPixel, Coord yPerPixel){
		int i=0;
		while (i < numSyms){
			FullMapping_GData *sym = (FullMapping_GData *)syms[i];
			int symIndex;
			for(symIndex= i+1; symIndex < numSyms; symIndex++){
				if ( ((FullMapping_GData *)syms[symIndex])->shapeID != sym->shapeID)
				break;
			}
			map_shape[sym->shapeID]->DrawGDataArray(win,&syms[i],symIndex-i, this, xPerPixel, yPerPixel, GetPixelWidth());
			i = symIndex;
		}
	}
private:
	void *userData;
	Shape *_shapes[3];
};

