/* Display.c */
#include "XDisplay.h"

/* DList <DeviseDisplay *> DeviseDisplay::_displays;*/
DeviseDisplayList DeviseDisplay::_displays;
DeviseDisplay *DeviseDisplay::_defaultDisplay= NULL;

/********************************************************************
Get the default display
*********************************************************************/
DeviseDisplay *DeviseDisplay::DefaultDisplay(){

	if (_defaultDisplay == NULL)
		_defaultDisplay = new XDisplay();
	return _defaultDisplay;
}

/************************************************************/
DeviseDisplay::DeviseDisplay(){
	_dispatcher = Dispatcher::Current();
	_dispatcher->Register(this,AllState,true);
/*
printf("after DeviseDisplay register 0x%x\n", this);
*/

	_numColors = 0;
	_colorMapSize = InitColorMapSize;
	_colorMap = new Color[InitColorMapSize];
	_displays.Append(this);
}

/************************************************************/
DeviseDisplay::~DeviseDisplay(){
	_dispatcher->Unregister(this);
	delete _colorMap;
	_displays.Delete(this);
}


/***************************************************************
 map local color to global color.
XXX: Need to check for duplicate globalColor 
********************************************************************/
void DeviseDisplay::MapColor(Color localColor, Color globalColor){
	if (globalColor >= _colorMapSize-1){
		/* overflow, create a new array  */
		_colorMapSize = globalColor+AdditionalColorMapSize+1;
		Color *tempMap = new Color[_colorMapSize];
		for(int i=0; i < _numColors; i++)
			tempMap[i] = _colorMap[i];
		delete _colorMap;
		_colorMap = tempMap;
	}
	_numColors = globalColor+1;
	_colorMap[globalColor] = localColor;
}

/**************************************************************************
get local color given global color. 
***************************************************************************/
Color DeviseDisplay::GetLocalColor(Color globalColor){
	if (globalColor >= _numColors){
		fprintf(stderr,"Display::GetLocalColor: undefined color %d\n",
			globalColor);
		Exit::DoExit(1);
	}
	return _colorMap[globalColor];
}

char *DeviseDisplay::DispatchedName(){
	return "Display";
}
