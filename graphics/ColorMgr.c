/* ColorMgr.c */
#include "ColorMgr.h"
#include "Display.h"

ColorMgr * ColorMgr::_instance = NULL; /* one and only manager instance */

ColorMgr::ColorMgr(){
	/* init default colors. Do not insert them into the
	DeviseDisplays
	1) if there are no displays, we don't need to inser them.
	2) Otherwise, the ColorMgr is created only when
	InitializeColors() is called by a DeviseDisplay. Let InitializeColors()
	insert the default colors */

	_colorArraySize = InitColorArraySize;
	_colorArray = new (ColorData *[_colorArraySize]);

	/* insert 1st three colors */
	ColorData *data= _colorArray[0]= new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "black";

	data = _colorArray[1] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "white";

	data = _colorArray[2] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "red";

	data = _colorArray[3] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue";

	data = _colorArray[4] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "orange";

	data = _colorArray[5] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "tan";

	data = _colorArray[6] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "green";

	data = _colorArray[7] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "purple";

	data = _colorArray[8] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "aquamarine";

	data = _colorArray[9] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "peru";

	data = _colorArray[10] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "chocolate";

	data = _colorArray[11] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "tomato";

	data = _colorArray[12] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "pink";

	data = _colorArray[13] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "plum";

	data = _colorArray[14] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "azure1";

	data = _colorArray[15] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "cyan1";

	data = _colorArray[16] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "SeaGreen1";

	data = _colorArray[17] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "khaki1";

	data = _colorArray[18] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "goldenrod1";

	data = _colorArray[19] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "yellow";

	data = _colorArray[20] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "sienna1";

	data = _colorArray[21] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "green";

	data = _colorArray[22] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue";

	data = _colorArray[23] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "orange";

	data = _colorArray[24] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "LightGray";

	data = _colorArray[25] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "red1";

	data = _colorArray[26] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "red2";

	data = _colorArray[27] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "green1";

	data = _colorArray[28] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "green2";

	data = _colorArray[29] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue1";

	data = _colorArray[30] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue2";

	data = _colorArray[31] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "gold1";

	data = _colorArray[32] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "gold2";

	data = _colorArray[33] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue1";

	data = _colorArray[34] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue2";

	data = _colorArray[35] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue3";

	data = _colorArray[36] = new ColorData;
	data->type = ColorData::NameVal;
	data->val.nameVal = "blue4";

	_numColors = 37;
};





/******************************************************************
Initialize color for a newly created display
********************************************************************/
void ColorMgr::_InitializeColors(DeviseDisplay *disp){
/*
printf("_ColorMgr::_InitializeColors\n");
*/

	for (int i=0; i < _numColors; i++){
		ColorData *data = _colorArray[i];
		if (data->type == ColorData::RGBVal){
			disp->AllocColor(data->val.rgbVal.r, data->val.rgbVal.g,
			data->val.rgbVal.b,i);
			/*
			printf("insert rgb %f,%f,%f\n", data->val.rgbVal.r, 
				data->val.rgbVal.g, data->val.rgbVal.b);
			*/
		}
		else disp->AllocColor(data->val.nameVal,i);
	}
}


/****************************************************************
 insert one color into the color manager, and
update all displays to relfect the new global color.
Return the color.
******************************************************************/
Color ColorMgr::Insert(ColorData *data){
/*
printf("ColorMgr::Insert ");
if (data->type == ColorData::RGBVal){
	printf("insert rgb %f,%f,%f\n", data->val.rgbVal.r, 
		data->val.rgbVal.g, data->val.rgbVal.b);
}
else {
	printf("nameVal\n");
}
*/

	if (_numColors == _colorArraySize){
		/* array overflow. Create another one. */
		_colorArraySize += AdditionalColorArraySize;
		ColorData **temp = new (ColorData *[_colorArraySize]);
		for (int i=0; i < _numColors; i++)
			temp[i] = _colorArray[i];
		delete _colorArray;
		_colorArray = temp;
	}

	_colorArray[_numColors] = data;

	/* propagate color to all displays */
	int index;
	Color tempColor = _numColors++;
	for (index=DeviseDisplay::InitIterator();DeviseDisplay::More(index); ){
		DeviseDisplay *disp = DeviseDisplay::Next(index);
		if (data->type == ColorData::RGBVal){
			disp->AllocColor(data->val.rgbVal.r, data->val.rgbVal.g, 
					data->val.rgbVal.b,tempColor);
		}
		else disp->AllocColor(data->val.nameVal,tempColor);
	}
	DeviseDisplay::DoneIterator(index);
	return tempColor;
}


Color ColorMgr::AllocColor(double r, double g, double b){
		ColorData *data = new ColorData;
		data->type = ColorData::RGBVal;
		data->val.rgbVal.r = r; data->val.rgbVal.g = g; data->val.rgbVal.b = b;
		return Instance()->Insert(data);
};


