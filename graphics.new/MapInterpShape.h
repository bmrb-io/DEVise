
/*
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
*/

#define GetAttr(ptr, attrName, attrType, offset) \
	*((attrType *)(ptr+offset->attrName))
/*	*((attrType *)((ptr)+(offset)->(attrName))) */

inline Coord GetX(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->xOffset < 0)
		return map->GetDefaultX();
	else {
		return GetAttr(ptr, xOffset, Coord, offset);
	}
}
inline Coord GetY(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->yOffset < 0)
		return map->GetDefaultY();
	else return GetAttr(ptr,yOffset, Coord, offset);
}

inline Color GetColor(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->colorOffset < 0)
		return map->GetDefaultColor();
	else return GetAttr(ptr,colorOffset, Color , offset);
}
inline Pattern GetPattern(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->patternOffset < 0)
		return map->GetDefaultPattern();
	else return GetAttr(ptr,patternOffset, Pattern, offset);
}

inline Coord GetShapeAttr0(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->shapeAttrOffset[0] < 0){
		ShapeAttr *attrs = map->GetDefaultShapeAttrs();
		return attrs[0];
	}
	else return GetAttr(ptr,shapeAttrOffset[0], Coord, offset);
}
inline Coord GetShapeAttr1(char *ptr, TDataMap *map, GDataAttrOffset *offset){
	if (offset->shapeAttrOffset[1] < 0){
		ShapeAttr *attrs = map->GetDefaultShapeAttrs();
		return attrs[1];
	}
	else return GetAttr(ptr,shapeAttrOffset[1], Coord, offset);
}


class FullMapping_RectShape: public RectShape {
public:
	virtual void BoundingBoxGData(TDataMap *map, void **gdataArray,int numSyms,
		Coord &width,Coord &height){
		width = 0.0;

		height = 0.0;

		GDataAttrOffset *offset = map->GetGDataOffset();
		int i;
		for (i=0; i < numSyms; i++){
			char *gdata = (char *)gdataArray[i];
			Coord temp ;

			temp = GetShapeAttr0(gdata, map, offset);
			if (temp > width) width = temp;

			temp = GetShapeAttr1(gdata, map,  offset);
			if (temp > height) height = temp;
		}
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		 
		GDataAttrOffset *offset = map->GetGDataOffset();
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		if (maxWidth <= xPerPixel  && maxHeight <= yPerPixel ){
			 
			i= 0;
			while (i < numSyms){
				char *gdata = (char *)gdataArray[i];
				int count = 1;
				_x[0] = GetX(gdata, map, offset);
				_y[0] = GetY(gdata, map, offset);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					char *colorGData = (char *)gdataArray[colorIndex];
					if (GetColor(colorGData,map,offset)!= 
						GetColor(gdata, map, offset))
						break;
					_x[count] = GetX(colorGData, map, offset);
					_y[count++] = GetY(colorGData, map, offset);
				}

				win->SetFgColor(GetColor(gdata, map, offset));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			 
			i= 0;
			while (i < numSyms){
				char *gdata = (char *)gdataArray[i];
				int count = 1;
				Coord width = GetShapeAttr0(gdata, map, offset);
				Coord height = GetShapeAttr1(gdata, map, offset);

				_x[0] = GetX(gdata, map, offset)- width/2.0;
				_y[0] = GetY(gdata, map, offset) - height/2.0;
				_width[0] = width;
				_height[0] = height;

				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					char *colorGData = (char *)gdataArray[colorIndex];
					if (GetColor(colorGData, map, offset)!= 
						GetColor(gdata, map, offset))
						break;


					width = GetShapeAttr0(colorGData, map, offset);
					height = GetShapeAttr1(colorGData, map, offset);
					_x[count] = GetX(colorGData, map, offset)- width/2.0;
					_y[count] = GetY(colorGData, map, offset)- height/2.0;
					_width[count] = width;
					_height[count] = height;

					count++;
				}

				win->SetFgColor(GetColor(gdata,map, offset));
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
		GDataAttrOffset *offset = map->GetGDataOffset();

		int i;
		for (i=0; i < numSyms; i++){
			char *gdata = (char *)gdataArray[i];
			Coord temp ;

			temp = GetShapeAttr0(gdata, map, offset);
			if (temp > width) width = temp;

			temp = GetShapeAttr1(gdata, map, offset);
			if (temp > height) height = temp;
		}
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		/*
		printf("RectX draw %d syms\n", numSyms);
		*/
		GDataAttrOffset *offset = map->GetGDataOffset();
		 
		int i;
		Coord maxWidth, maxHeight;
		map->MaxBoundingBox(maxWidth,maxHeight);

		/*
		printf("RectX draw maxWidth %f xPerPixel %f\n",maxWidth, xPerPixel);
		*/
		if (maxWidth <= xPerPixel){
			 
			i= 0;
			while (i < numSyms){
				char *gdata = (char *)gdataArray[i];
				int count = 1;
				_x[0] = GetX(gdata, map, offset);
				_y[0] = GetY(gdata, map, offset);
				int colorIndex;
				for (colorIndex = i+1; colorIndex < numSyms; colorIndex++){
					char *colorGData = (char *)gdataArray[colorIndex];
					if (GetColor(colorGData, map, offset)!= 
						GetColor(gdata, map, offset))
						break;
					_x[count] = GetX(colorGData, map, offset);
					_y[count++] = GetY(colorGData, map, offset);
				}

				win->SetFgColor(GetColor(gdata, map, offset));
				win->DrawPixelArray(_x,_y,count, pixelWidth);
	
				i = colorIndex;
			}
		}
		else {
			 
			Coord x1,y1,x2,y2;
			win->Transform(0.0,0.0,x1,y1);
			for (i=0; i < numSyms; i++){
				char *gdata = (char *)gdataArray[i];
				Coord tx, ty;
				Coord width, height;
				win->Transform(GetX(gdata, map, offset),
					GetY(gdata, map, offset),tx,ty);
				win->Transform(GetShapeAttr0(gdata, map, offset),0.0,x2,y2);
				width = x2-x1;
				if (width < 0.0) width = -width;
				height = GetShapeAttr1(gdata, map, offset)/
					GetShapeAttr0(gdata, map, offset)*width;

				win->SetFgColor(GetColor(gdata, map, offset));
				win->SetPattern(GetPattern(gdata, map, offset));
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
		GDataAttrOffset *offset = map->GetGDataOffset();
		for (i = 0; i < numSyms; i++){
			char *gdata = (char *)gdataArray[i];
			Coord curWidth = GetShapeAttr0(gdata, map, offset);
			if (curWidth > width)
				width = curWidth;
			 
			Coord curHeight = fabs(GetY(gdata, map, offset));
			if (height > curHeight)
				height = curHeight;
		}
	}

	virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int numSyms,
		TDataMap *map, Coord xPerPixel, Coord yPerPixel, int pixelWidth){
		GDataAttrOffset *offset = map->GetGDataOffset();
		int i;
		for (i=0; i < numSyms; i++){
			char *gdata = (char *)gdataArray[i];
			win->SetFgColor(GetColor(gdata, map, offset));

			win->SetPattern(GetPattern(gdata, map, offset));
			Coord y = GetY(gdata, map, offset);
			if (y >= 0.0)
				win->FillRect(GetX(gdata, map, offset),0.0,
					GetShapeAttr0(gdata, map, offset), y);
			else
				win->FillRect(GetX(gdata, map, offset),y,
					GetShapeAttr0(gdata, map, offset), -y);
		}
	}
};
