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
  Revision 1.13.4.1  1997/03/15 00:31:02  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.13  1996/11/13 16:56:05  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.12  1996/09/10 20:07:08  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.11  1996/09/05 21:30:15  jussi
  Moved user-specified screen size to Display.

  Revision 1.10  1996/07/19 14:25:30  jussi
  Return black color for undefined colors instead of crashing.

  Revision 1.9  1996/06/24 19:36:50  jussi
  Removed unnecessary code that stored a pointer to the
  dispatcher in a member variable.

  Revision 1.8  1996/05/20 18:44:58  jussi
  Merged with ClientServer library code.

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

//#define DEBUG

#include "XDisplay.h"
#include "PSDisplay.h"
#include "Util.h"
#ifndef LIBCS
#include "Init.h"
#include "WinClassInfo.h"
#include "ViewWin.h"
#endif

DeviseDisplayList DeviseDisplay::_displays;
DeviseDisplay *DeviseDisplay::_defaultDisplay = 0;
DeviseDisplay *DeviseDisplay::_psDisplay = 0;

/********************************************************************
Get the default display
*********************************************************************/

DeviseDisplay *DeviseDisplay::DefaultDisplay()
{
  if (!_defaultDisplay)
    _defaultDisplay = new XDisplay();
  return _defaultDisplay;
}

/********************************************************************
Get the PostScript display
*********************************************************************/

DeviseDisplay *DeviseDisplay::GetPSDisplay()
{
  if (!_psDisplay)
    _psDisplay = new PSDisplay();
  return _psDisplay;
}

/************************************************************/

DeviseDisplay::DeviseDisplay()
{
  DO_DEBUG(printf("DeviseDisplay::DeviseDisplay()\n"));

  _numColors = 0;
  _colorMapSize = InitColorMapSize;
  _colorMap = new LocalColor[InitColorMapSize];
  _displays.Append(this);

  DO_DEBUG(printf("Number of displays = %d\n", _displays.Size()));

#ifndef LIBCS
  _desiredScreenWidth = Init::ScreenWidth();
  _desiredScreenHeight = Init::ScreenHeight();
#endif
}

/************************************************************/

DeviseDisplay::~DeviseDisplay()
{
#ifndef LIBCS
  Dispatcher::Current()->Unregister(this);
#endif

  delete _colorMap;
  _displays.Delete(this);
}

/***************************************************************
 map local color to global color.
 TEMPTEMPXXX: Need to check for duplicate globalColor 
********************************************************************/

void DeviseDisplay::MapColor(LocalColor localColor, GlobalColor globalColor)
{
  if (globalColor >= _colorMapSize - 1){
    /* overflow, create a new array  */
    _colorMapSize = globalColor + AdditionalColorMapSize + 1;
    LocalColor *tempMap = new LocalColor[_colorMapSize];
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

LocalColor DeviseDisplay::GetLocalColor(GlobalColor globalColor)
{
  if (globalColor >= _numColors)
      return _colorMap[BlackColor];
  return _colorMap[globalColor];
}

char *DeviseDisplay::DispatchedName()
{
  return "Display";
}

#ifndef LIBCS
/**************************************************************************
Export the entire DEVise display to PostScript or EPS.
This function is implemented in the DeviseDisplay class, rather than the
XDisplay class, so that it can be inherited by future implementations
of NTWindowRep, etc.
***************************************************************************/

void
DeviseDisplay::ExportToPS(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("DeviseDisplay::ExportToPS(%d, %s)\n", (int) format, filename);
#endif

  DevStatus result = StatusOk;

  DeviseDisplay *psDisplay = GetPSDisplay();

  /* Find the smallest bounding rectangle covering all of the windows.
   * Note: this is in _screen_ coordinates. */
  int winLeft=MAXINT, winRight=0, winTop=MAXINT, winBot=0;

  ClassInfo *windowInfo;
  ViewWin *windowP;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    windowP = (ViewWin *) windowInfo->GetInstance();
    if ((windowP != NULL) && !windowP->Iconified()) {
      int winX, winY; 
      unsigned winWidth, winHeight;
      windowP->RealGeometry(winX, winY, winWidth, winHeight);
      windowP->AbsoluteOrigin(winX, winY);

      winLeft = MIN(winLeft, winX);
      winRight = MAX(winRight, winX + (int) winWidth - 1);
      winTop = MIN(winTop, winY);
      winBot = MAX(winBot, winY + (int) winHeight - 1);
    }
  }
  DevWindow::DoneIterator(index);

  Rectangle printGeom;
  printGeom.x = winLeft;
  printGeom.y = winTop;
  printGeom.width = winRight - winLeft + 1;
  printGeom.height = winBot - winTop + 1;

#if defined(DEBUG)
  printf("Screen bounding rect: %d %d %d %d\n", winLeft, winRight, winTop,
    winBot);
#endif

  /* Open the output file and print the PostScript header. */
  if (!psDisplay->OpenPrintFile(filename).IsComplete()) {
    reportError("Can't open print file", errno);
    result += StatusFailed;
  }

  if (result.IsComplete()) {
    psDisplay->PrintPSHeader("DEVise Visualization", printGeom, true);
  }

  /* Print each window.  Note dispatcher looping to wait for the window
   * to finish printing. */
  if (result.IsComplete()) {
    index = DevWindow::InitIterator();
    while (DevWindow::More(index)) {
      windowInfo = DevWindow::Next(index);
      windowP = (ViewWin *) windowInfo->GetInstance();
      if ((windowP != NULL) && !windowP->Iconified()) {
        windowP->PrintPS();
        while (windowP->IsPrinting()) {
          Dispatcher::SingleStepCurrent();
        }
      }
    }
    DevWindow::DoneIterator(index);
  }

  /* Print the PostScript trailer and close the output file. */
  if (result.IsComplete()) {
    psDisplay->PrintPSTrailer();
  }

  result += psDisplay->ClosePrintFile();
  printf("Done generating print file\n");

#if defined(DEBUG)
  printf("  DeviseDisplay::ExportToPS(): ");
  result.Print();
#endif
}
#endif
