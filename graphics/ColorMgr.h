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
  Color manager -- maintains relationships between GlobalColor, color
  name, and RGB value.  This class must also be used to allocate new
  colors (it propagates changes down to the Display objects).
*/

/*
  $Id$

  $Log$
  Revision 1.6  1996/11/18 23:11:16  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.5  1996/11/13 16:56:04  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.4  1995/12/28 18:39:32  jussi
  Minor fixes to remove compiler warnings.

  Revision 1.3  1995/12/05 21:58:15  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:29  jussi
  Added/updated CVS header.
*/

/*
   ColorMgr.h: used to allocate global colors consistent for all displays.
   Built-in colors: 0th color == black. 1st color = white...
*/

#ifndef ColorMgr_h
#define ColorMgr_h

#include <stdio.h>
#include <string.h>
#include "Color.h"
#include "DeviseTypes.h"
#include "Exit.h"

const unsigned int InitColorArraySize = 64;
const unsigned int AdditionalColorArraySize = 64;

/* RGB color values (0.0 - 1.0). */
struct RgbVals {
  float red;
  float green;
  float blue;
};

/* color data structure */

struct ColorData {
  char *colorName;
  Boolean hasRgb;
  RgbVals rgb;
};

class ColorMgr {
public:

  /* allocate color by name */
  static GlobalColor AllocColor(char *name);

  /* allocate lots of colors by name */
  static void AllocColor(char **names, int numNames) {
    for(int i = 0; i < numNames; i++)
      (void)AllocColor(names[i]);
  }

  /* allocate color by rgb */
  static GlobalColor AllocColor(double r, double g, double b);

  /* Get the name of a given color by its global color number. */
  static char *GetColorName(GlobalColor color) {
    //DOASSERT(color < Instance()->_numColors, "Color not allocated");
    if (color < 0 || color >= Instance()->_numColors) color = BlackColor;
    ColorData *data = Instance()->_colorArray[color];
    if (data->colorName != NULL) {
      return data->colorName;
    }
    //DOASSERT(false, "No name for color");
    return NULL;
  }

  /* Get the RGB values of a given color by its global color number. */
  static void GetColorRgb(GlobalColor color, float &red, float &green,
    float &blue) {
    //DOASSERT(color < Instance()->_numColors, "Color not allocated");
    if (color < 0 || color >= Instance()->_numColors) color = BlackColor;
    ColorData *data = Instance()->_colorArray[color];
    //DOASSERT(data->hasRgb, "No RGB values for color");
    if (data->hasRgb) {
      red = data->rgb.red;
      green = data->rgb.green;
      blue = data->rgb.blue;
    } else {
      fprintf(stderr, "No RGB values for color %d\n", color);
      red = 0.0;
      green = 0.0;
      blue = 0.0;
    }
  }

protected:
  friend class DeviseDisplay;

  /* initialize colors for the newly created display display */
  static void InitializeColors(DeviseDisplay *disp) {
    Instance()->_InitializeColors(disp);
  }

private:

  /* insert one color into the color manager, and
     update all displays to relfect the new global color.
     Return the color.*/
  GlobalColor Insert(ColorData *data);

  /* initialize colors for a display */
  void _InitializeColors(DeviseDisplay *disp);
  
  /* get the one and only instance of ColorMgr */
  static ColorMgr *Instance() {
    if (!_instance)
      _instance = new ColorMgr;
    return _instance;
  }

  ColorMgr();

  ~ColorMgr() {
    delete _colorArray;
  }
  
  unsigned int _numColors;
  unsigned int _colorArraySize;
  ColorData **_colorArray;
  static ColorMgr *_instance;
};
#endif

