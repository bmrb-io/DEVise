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
  Implementation of ColorMgr class.
*/

/*
  $Id$

  $Log$
  Revision 1.9  1996/11/21 19:13:45  wenger
  Fixed more compile warnings; updated devise.dali to match current
  command-line flags.

  Revision 1.8  1996/11/13 16:56:03  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.7  1996/06/20 16:49:04  jussi
  Replaced green1 with DarkSeaGreen.

  Revision 1.6  1996/04/16 19:49:38  jussi
  Replaced assert() calls with DOASSERT().

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

#include "machdep.h"
#include "ColorMgr.h"
#include "Display.h"
#include "Color.h"
#include "Util.h"

//#define DEBUG

ColorMgr *ColorMgr::_instance = NULL;   // one and only manager instance

static struct {
  GlobalColor value;                    // GlobalColor value of color
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
  { DarkSeaGreen, "DarkSeaGreen" },
  { GreenColor2, "green2" },
  { BlueColor1, "blue1" },
  { BlueColor2, "blue2" },
  { BlueColor3, "blue3" },
  { BlueColor4, "blue4" },
  { GoldColor1, "gold1" },
  { GoldColor2, "gold2" }
};

/*------------------------------------------------------------------------------
 * Constructor.
 */
ColorMgr::ColorMgr()
{
#if defined(DEBUG)
  printf("ColorMgr::ColorMgr()\n");
#endif

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
    data->colorName = CopyString(defaultColors[i].name);
    data->hasRgb = false;
  }
}

/*------------------------------------------------------------------------------
 * Initialize colors for a newly created display
 */
void ColorMgr::_InitializeColors(DeviseDisplay *disp)
{
#ifdef DEBUG
  printf("ColorMgr::_InitializeColors\n");
#endif

  for(unsigned int i = 0; i < _numColors; i++) {
    ColorData *data = _colorArray[i];
    if (data->hasRgb) {
      disp->AllocColor(data->rgb, (GlobalColor) i);
#ifdef DEBUG
      printf("insert rgb %f,%f,%f\n", data->rgb.red,
	     data->rgb.green, data->rgb.blue);
#endif
    } else {
      DOASSERT(data->colorName != NULL, "No color name or RGB specified");
      disp->AllocColor(data->colorName, (GlobalColor) i, data->rgb);
      data->hasRgb = true;
    }
  }
}

/*------------------------------------------------------------------------------
 * insert one color into the color manager, and
 * update all displays to relfect the new global color.
 * Return the color.
 */
GlobalColor ColorMgr::Insert(ColorData *data)
{
#ifdef DEBUG
  printf("ColorMgr::Insert ");
  if (data->hasRgb) {
    printf("insert rgb %f,%f,%f\n", data->rgb.red, 
	   data->rgb.green, data->rgb.blue);
  } else {
    printf("colorName %s\n", data->colorName);
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
  GlobalColor tempColor = (GlobalColor) _numColors++;
  int index;
  for(index = DeviseDisplay::InitIterator();DeviseDisplay::More(index); ){
    DeviseDisplay *disp = DeviseDisplay::Next(index);
    if (data->hasRgb) {
      disp->AllocColor(data->rgb, tempColor);
    } else {
      disp->AllocColor(data->colorName, tempColor, data->rgb);
      data->hasRgb = true;
    }
  }
  DeviseDisplay::DoneIterator(index);

  return tempColor;
}

/*------------------------------------------------------------------------------
 * Allocate a new color by name.
 */
GlobalColor ColorMgr::AllocColor(char *name)
{
#if defined(DEBUG)
  printf("ColorMgr::AllocColor(%s)\n", name);
#endif

  /* If we've already allocated this color, just return the corresponding
   * GlobalColor value. */
  int numColors = Instance()->_numColors;
  ColorData *data;
  for (int count = 0; count < numColors; count++) {
    data = Instance()->_colorArray[count];
    if (data->colorName != NULL && !strcasecmp(name, data->colorName)) {
#if defined(DEBUG)
      printf("Color %s already allocated\n", name);
#endif
      return (GlobalColor) count;
    }
  }

  /* Otherwise, go ahead and allocate this color. */
  data = new ColorData;
  data->colorName = CopyString(name);
  data->hasRgb = false;

  return Instance()->Insert(data);
}

/*------------------------------------------------------------------------------
 * Allocate a new color by RGB.
 */
GlobalColor ColorMgr::AllocColor(double r, double g, double b)
{
#if defined(DEBUG)
  printf("ColorMgr::AllocColor(%f, %f, %f)\n", r, g, b);
#endif

  /* If we've already allocated this color, just return the corresponding
   * GlobalColor value. */
  int numColors = Instance()->_numColors;
  ColorData *data;
  for (int count = 0; count < numColors; count++) {
    data = Instance()->_colorArray[count];
    if (data->hasRgb && (data->rgb.red == r) && (data->rgb.green == g) &&
      (data->rgb.blue == b)) {
#if defined(DEBUG)
      printf("Color 0x%.2f%.2f%.2f already allocated\n", r, g, b);
#endif
      return (GlobalColor) count;
    }
  }

  /* Otherwise, go ahead and allocate this color. */
  data = new ColorData;
  data->colorName = NULL;
  data->hasRgb = true;
  data->rgb.red = r;
  data->rgb.green = g;
  data->rgb.blue = b;

  return Instance()->Insert(data);
}
