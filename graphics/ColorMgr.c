/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5  1996/01/30 00:01:57  jussi
  Added a few colors and removed duplicate ones.

  Revision 1.4  1995/12/28 18:39:26  jussi
  Minor fixes to remove compiler warnings.

  Revision 1.3  1995/12/05 21:56:39  jussi
  Moved list of default colors to be a structure array rather than
  repeated code, and also made the code use the color constants
  defined in Color.h. Also added copyright notice.

  Revision 1.2  1995/09/05 21:12:28  jussi
  Added/updated CVS header.
*/

#include "ColorMgr.h"
#include "Display.h"
#include "Color.h"

ColorMgr *ColorMgr::_instance = NULL;   // one and only manager instance

static struct {
  Color value;                          // Color value of color
  char *name;                           // name of color
} defaultColors[] = {                   // list of default colors
  { BlackColor, "black" },
  { WhiteColor, "white" },
  { RedColor, "red" },
  { BlueColor, "blue" },
  { OrangeColor, "orange" },
  { TanColor, "tan" },
  { GreenColor, "green" },
  { PurpleColor, "purple" },
  { AquamarineColor, "aquamarine" },
  { PeruColor, "peru" },
  { ChocolateColor, "chocolate" },
  { TomatoColor, "tomato" },
  { PinkColor, "pink" },
  { PlumColor, "plum" },
  { AzureColor, "azure1" },
  { CyanColor, "cyan1" },
  { SeaGreenColor, "SeaGreen1" },
  { KhakiColor, "khaki1" },
  { GoldenRodColor, "goldenrod1" },
  { YellowColor, "yellow" },
  { SiennaColor, "sienna1" },
  { LightCoralColor, "LightCoral" },
  { AntiqueWhiteColor, "AntiqueWhite" },
  { LemonChiffonColor, "LemonChiffon" },
  { LightGrayColor, "LightGray" },
  { LavenderColor, "Lavender" },
  { LavenderBlushColor, "LavenderBlush" },
  { MistyRoseColor, "MistyRose" },
  { NavyBlueColor, "NavyBlue" },
  { SlateBlueColor, "SlateBlue" },
  { MediumBlueColor, "MediumBlue" },
  { DeepSkyBlueColor, "DeepSkyBlue" },
  { SkyBlueColor, "SkyBlue" },
  { RedColor1, "red1" },
  { RedColor2, "red2" },
  { GreenColor1, "green1" },
  { GreenColor2, "green2" },
  { BlueColor1, "blue1" },
  { BlueColor2, "blue2" },
  { BlueColor3, "blue3" },
  { BlueColor4, "blue4" },
  { GoldColor1, "gold1" },
  { GoldColor2, "gold2" }
};

ColorMgr::ColorMgr()
{
  /* init default colors. Do not insert them into the DeviseDisplays yet:
     1) if there are no displays, we don't need to insert them.
     2) otherwise, the ColorMgr is created only when InitializeColors()
        is called by a DeviseDisplay. Let InitializeColors() insert
	the default colors */
  
  _colorArraySize = InitColorArraySize;
  _numColors = sizeof defaultColors / sizeof defaultColors[0];
  DOASSERT(_numColors <= _colorArraySize, "Too many colors");

  _colorArray = new ColorData * [_colorArraySize];

  for(unsigned int i = 0; i < _numColors; i++) {
    DOASSERT(defaultColors[i].value == i, "Invalid color index");
    ColorData *data = _colorArray[defaultColors[i].value] = new ColorData;
    data->type = ColorData::NameVal;
    data->val.nameVal = defaultColors[i].name;
  }
}

/******************************************************************
Initialize color for a newly created display
********************************************************************/

void ColorMgr::_InitializeColors(DeviseDisplay *disp)
{
#ifdef DEBUG
  printf("_ColorMgr::_InitializeColors\n");
#endif

  for(unsigned int i = 0; i < _numColors; i++) {
    ColorData *data = _colorArray[i];
    if (data->type == ColorData::RGBVal) {
      disp->AllocColor(data->val.rgbVal.r, data->val.rgbVal.g,
		       data->val.rgbVal.b, i);
#ifdef DEBUG
      printf("insert rgb %f,%f,%f\n", data->val.rgbVal.r,
	     data->val.rgbVal.g, data->val.rgbVal.b);
#endif
    } else
      disp->AllocColor(data->val.nameVal, i);
  }
}

/****************************************************************
 insert one color into the color manager, and
 update all displays to relfect the new global color.
 Return the color.
******************************************************************/

Color ColorMgr::Insert(ColorData *data)
{
#ifdef DEBUG
  printf("ColorMgr::Insert ");
  if (data->type == ColorData::RGBVal){
    printf("insert rgb %f,%f,%f\n", data->val.rgbVal.r, 
	   data->val.rgbVal.g, data->val.rgbVal.b);
  } else {
    printf("nameVal %s\n", data->val.nameVal);
  }
#endif

  if (_numColors == _colorArraySize){
    /* array overflow. Create another one. */
    _colorArraySize += AdditionalColorArraySize;
    ColorData **temp = new (ColorData *[_colorArraySize]);
    for(unsigned int i = 0; i < _numColors; i++)
      temp[i] = _colorArray[i];
    delete _colorArray;
    _colorArray = temp;
  }
  
  _colorArray[_numColors] = data;
  
  /* propagate color to all displays */
  Color tempColor = _numColors++;
  int index;
  for(index = DeviseDisplay::InitIterator();DeviseDisplay::More(index); ){
    DeviseDisplay *disp = DeviseDisplay::Next(index);
    if (data->type == ColorData::RGBVal){
      disp->AllocColor(data->val.rgbVal.r, data->val.rgbVal.g, 
		       data->val.rgbVal.b,tempColor);
    } else
      disp->AllocColor(data->val.nameVal,tempColor);
  }
  DeviseDisplay::DoneIterator(index);

  return tempColor;
}

Color ColorMgr::AllocColor(double r, double g, double b)
{
  ColorData *data = new ColorData;
  data->type = ColorData::RGBVal;
  data->val.rgbVal.r = r;
  data->val.rgbVal.g = g;
  data->val.rgbVal.b = b;
  return Instance()->Insert(data);
}
