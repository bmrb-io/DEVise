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
  Revision 1.7  1996/05/09 18:12:08  kmurli
  No change to this makefile.

  Revision 1.6  1996/04/08 16:57:02  jussi
  Minor fixes.

  Revision 1.5  1996/04/04 05:18:27  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from
  Display) corrected a bug in call to register function. Also now
  dispatcher uses socket number passed from the XDisplay class to
  select on it and call the relevant functions.

  Revision 1.4  1995/12/28 18:45:35  jussi
  Small fixes to remove compiler warnings.

  Revision 1.3  1995/12/14 16:59:30  jussi
  Small fixes, and added copyright notice.

  Revision 1.2  1995/09/05 21:12:43  jussi
  Added/updated CVS header.
*/

#include "XDisplay.h"

DeviseDisplayList DeviseDisplay::_displays;
DeviseDisplay *DeviseDisplay::_defaultDisplay = 0;

/********************************************************************
Get the default display
*********************************************************************/

DeviseDisplay *DeviseDisplay::DefaultDisplay()
{
  if (!_defaultDisplay)
    _defaultDisplay = new XDisplay();
  return _defaultDisplay;
}

/************************************************************/

DeviseDisplay::DeviseDisplay()
{
#ifndef LIBCS
  _dispatcher = Dispatcher::Current();
#endif

  _numColors = 0;
  _colorMapSize = InitColorMapSize;
  _colorMap = new Color[InitColorMapSize];
  _displays.Append(this);
}

/************************************************************/

DeviseDisplay::~DeviseDisplay()
{
#ifndef LIBCS
  _dispatcher->Unregister(this);
#endif

  delete _colorMap;
  _displays.Delete(this);
}

/***************************************************************
 map local color to global color.
 XXX: Need to check for duplicate globalColor 
********************************************************************/

void DeviseDisplay::MapColor(Color localColor, Color globalColor)
{
  if (globalColor >= _colorMapSize - 1){
    /* overflow, create a new array  */
    _colorMapSize = globalColor + AdditionalColorMapSize + 1;
    Color *tempMap = new Color[_colorMapSize];
    for(unsigned int i = 0; i < _numColors; i++)
      tempMap[i] = _colorMap[i];
    delete _colorMap;
    _colorMap = tempMap;
  }
  _numColors = globalColor + 1;
  _colorMap[globalColor] = localColor;
}

/**************************************************************************
get local color given global color. 
***************************************************************************/

Color DeviseDisplay::GetLocalColor(Color globalColor)
{
  if (globalColor >= _numColors) {
    fprintf(stderr,"Display::GetLocalColor: undefined color %ld\n",
	    globalColor);
    Exit::DoExit(1);
  }
  return _colorMap[globalColor];
}

char *DeviseDisplay::DispatchedName()
{
  return "Display";
}
