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
  Revision 1.3  1995/12/14 16:59:30  jussi
  Small fixes, and added copyright notice.

  Revision 1.2  1995/09/05 21:12:43  jussi
  Added/updated CVS header.
*/

#include "XDisplay.h"

DeviseDisplayList DeviseDisplay::_displays;
DeviseDisplay *DeviseDisplay::_defaultDisplay= NULL;

/********************************************************************
Get the default display
*********************************************************************/

DeviseDisplay *DeviseDisplay::DefaultDisplay()
{
  if (_defaultDisplay == NULL)
    _defaultDisplay = new XDisplay();
  return _defaultDisplay;
}

/************************************************************/

DeviseDisplay::DeviseDisplay()
{
  _dispatcher = Dispatcher::Current();
  _dispatcher->Register(this,AllState,true);

  _numColors = 0;
  _colorMapSize = InitColorMapSize;
  _colorMap = new Color[InitColorMapSize];
  _displays.Append(this);
}

/************************************************************/

DeviseDisplay::~DeviseDisplay()
{
  _dispatcher->Unregister(this);
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
