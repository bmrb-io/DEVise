/*
  $Id$

  $Log$*/

/*
   ColorMgr.h: used to allocate global colors consistent for all displays.
   Built-in colors: 0th color == black. 1st color = white...
*/

#ifndef ColorMgr_h
#define ColorMgr_h
#include <stdio.h>
#include "Color.h"

const unsigned int InitColorArraySize = 512; /* init size of color array */
const unsigned int AdditionalColorArraySize = 512; /* size increment for additional allocation */

/* color data structure */
struct ColorData {
	enum ColorDataType { NameVal, RGBVal};
	enum ColorDataType type;
	union {
		char *nameVal;
		struct { double r, g, b; } rgbVal;
	} val;
};

class ColorMgr {
public:
	/* allocate color by name */
	static Color AllocColor(char *name){
		ColorData *data = new ColorData;
		data->type = ColorData::NameVal;
		data->val.nameVal = name;
		return Instance()->Insert(data);
	};

	/* allocate lots of colors by name */
	static void AllocColor(char **names, int numNames){
		for (int i=0; i < numNames; i++)
			(void)AllocColor(names[i]);
	}

	/* allocate color by rgb */
	static Color AllocColor(double r, double g, double b);

protected:
	friend class DeviseDisplay;

	/* initialize colors for the newly created display display */
	static void InitializeColors(DeviseDisplay *disp){
		Instance()->_InitializeColors(disp);
	};


private:

	/* insert one color into the color manager, and
	update all displays to relfect the new global color.
	Return the color.*/
	Color Insert(ColorData *data);

	/* initialize colors for a display */
	void _InitializeColors(DeviseDisplay *disp);

	/* get the one and only instance of ColorMgr */
	static ColorMgr *Instance(){
		if (_instance == NULL)
			_instance = new ColorMgr;
		return _instance;
	}


	ColorMgr();

	~ColorMgr(){
		delete _colorArray;
	}

	int _numColors;
	int _colorArraySize;
	ColorData **_colorArray;
	static ColorMgr *_instance;
};
#endif

