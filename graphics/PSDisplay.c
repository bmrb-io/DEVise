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
  Revision 1.19  1997/06/04 15:50:26  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.18  1997/05/21 22:09:55  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.17  1997/05/05 19:40:37  wenger
  Oops!  Forgot to remove PSWindowReps from _winList when they're destroyed!

  Revision 1.16  1997/05/05 16:53:45  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.15  1997/04/11 18:48:51  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.14  1997/03/25 17:58:53  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.13.4.2  1997/03/15 00:31:03  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.13.4.1  1997/03/07 20:03:57  wenger
  Tasvir images now work in PostScript output; Tasvir images now freed
  on a per-window basis; Tasvir timeout factor can be set on the command
  line; shared memory usage enabled by default.

  Revision 1.13  1997/01/17 20:31:43  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.12  1996/12/03 23:29:09  wenger
  Fixed PostScript bounding box to closely surround the image (fixed
  bug 089).

  Revision 1.11  1996/12/02 18:39:38  wenger
  Removed limits on maximum PostScript page size.

  Revision 1.10  1996/11/26 15:44:05  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.9  1996/11/23 00:24:11  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.8  1996/11/21 19:13:46  wenger
  Fixed more compile warnings; updated devise.dali to match current
  command-line flags.

  Revision 1.7  1996/11/19 15:23:25  wenger
  Minor changes to fix compiles on HP, etc.

  Revision 1.6  1996/11/18 23:11:17  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.5  1996/11/13 16:56:09  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.4  1996/10/28 15:55:41  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.3  1996/09/19 20:11:50  wenger
  More PostScript output code (still disabled); some code for drawing
  view borders (disabled).

  Revision 1.2  1996/09/10 20:07:08  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.1  1996/07/10 16:23:01  jussi
  Initial revision.
*/

//#define DEBUG

#include <errno.h>

#include "machdep.h"
#include "PSDisplay.h"
#include "PSWindowRep.h"
#include "Util.h"
#include "DevError.h"
#ifndef LIBCS
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif

const float pointsPerInch = 72.0;

/*******************************************************************
Open a new display
********************************************************************/

PSDisplay::PSDisplay(char *name)
{
  DO_DEBUG(printf("PSDisplay::PSDisplay(%s)\n", name != NULL ? name : "NULL"));

  _outputWidth = _userWidth = 8.5;
  _outputHeight = _userHeight = 11.0;
  _outputXMargin = _userXMargin = 1.0;
  _outputYMargin = _userYMargin = 1.0;
}

/*******************************************************************
Allocate color by name
********************************************************************/

void PSDisplay::AllocColor(char *name, GlobalColor globalColor, RgbVals &rgb)
{
#ifdef DEBUG
  printf("PSDisplay::AllocColor(%s)\n", name);
#endif

  /* do something */
}

/*********************************************************************
Allocate color by RGB
*********************************************************************/

void PSDisplay::AllocColor(RgbVals &rgb, GlobalColor globalColor)
{
#ifdef DEBUG
  printf("PSDisplay::AllocColor(%.2f,%.2f,%.2f)\n", r, g, b);
#endif

  /* do something */
}

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *PSDisplay::CreateWindowRep(char *name, Coord x, Coord y,
				     Coord width, Coord height, 
				     GlobalColor fgnd, GlobalColor bgnd, 
				     WindowRep *parentRep,
				     Coord min_width, Coord min_height,
				     Boolean relative, Boolean winBoundary)
{
  DO_DEBUG(printf("PSDisplay::CreateWindowRep(%s)\n", name));

  PSWindowRep *psWin = new PSWindowRep((DeviseDisplay *) this, fgnd, bgnd,
    (PSWindowRep *) parentRep, (int) x, (int) y, (int) width, (int) height);

  _winList.Insert(psWin);

  return psWin;
}

/**************************************************************
Destroy a window
**************************************************************/

void PSDisplay::DestroyWindowRep(WindowRep *win)
{
  DO_DEBUG(printf("PSDisplay::DestroyWindowRep(%p)\n", win));

  PSWindowRep *psWin = (PSWindowRep *) win;
  if (!_winList.Delete(psWin)) {
    reportErrNosys("Window to be deleted not found");
    Exit::DoExit(1);
  }

  delete win;
}

/**************************************************************
Open the print file.
**************************************************************/

DevStatus
PSDisplay::OpenPrintFile(char *filename)
{
  DevStatus result(StatusOk);

  _printFile = fopen(filename, "w");
  if (_printFile == NULL)
  {
    reportError("Can't open print file", errno);
    result += StatusFailed;
  }

  return result;
}

/**************************************************************
Close the print file.
**************************************************************/

DevStatus
PSDisplay::ClosePrintFile()
{
  DevStatus result(StatusOk);

  if (_printFile != NULL)
  {
    if (fclose(_printFile) != 0)
    {
       reportError("Error closing print file", errno);
       result += StatusWarn;
    }

    _printFile = NULL;
  }

  return result;
}

#ifndef LIBCS
/**************************************************************
Import a DEVise window, getting pixels from the display, not
generating PostScript directly.
**************************************************************/

DevStatus PSDisplay::ImportWindow(ViewWin *window,
    DisplayExportFormat format)
{
  DevStatus result(StatusOk);

  /* Calculate the size and location in PostScript for this window. */
  Rectangle location;

  unsigned winWidth, winHeight;
  int winXOrig, winYOrig;
  window->GetWindowRep()->Dimensions(winWidth, winHeight);
  window->GetWindowRep()->AbsoluteOrigin(winXOrig, winYOrig);

  location.x = _boundingBox.x + (winXOrig - _screenPrintRegion.x) /
    _screenPrintRegion.width * _boundingBox.width;
  location.y = _boundingBox.y + _boundingBox.height - 1 - (winYOrig -
    _screenPrintRegion.y) / _screenPrintRegion.height * _boundingBox.height;
  location.width = winWidth / _screenPrintRegion.width * _boundingBox.width;
  location.height = winHeight / _screenPrintRegion.height * _boundingBox.height;

  /* Dump the window and import the resulting PostScript. */
  char tmpFile[L_tmpnam];
  tmpnam(tmpFile);
  window->ExportImage(format, tmpFile);
  ImportPSImage(tmpFile, &location);
  unlink(tmpFile);

  return result;
}
#endif

/**************************************************************
Import a PostScript image.
**************************************************************/

DevStatus PSDisplay::ImportPSImage(char *filename, Rectangle *location)
{
  DevStatus result(StatusOk);

  DOASSERT(_printFile != NULL, "No print file");

  /* Call procedure to prepare for including EPS file. */
  fprintf(_printFile, "\nBeginEPSF\n");

  /* Now open and read the file to import. */
  FILE *importFile = fopen(filename, "r");
  if (importFile == NULL) {
    reportError("Can't open import file", errno);
    result += StatusFailed;
  } else {
    const int bufSize = 256;
    char buf[bufSize];

    /* Note: location is desired location and size of the imported image
     * on the page, in points.  x, y are top left corner, y positive up.
     * If location is not specified, just leave the imported image wherever
     * it already is. */
    if (location != NULL) {
      /* Find out the bounding box for what we're importing. */
      char *bbString = "%%BoundingBox:";
      int bbStrLen = strlen(bbString);
      Coord x1, y1, x2, y2;
      while (fgets(buf, bufSize, importFile) != NULL) {
        if (!strncmp(bbString, buf, bbStrLen)) {
	  sscanf(buf, "%*s %lf %lf %lf %lf", &x1, &y1, &x2, &y2);
	  break;
        }
      }
      if (ferror(importFile)) {
        reportErrSys("Error reading import file");
        result += StatusFailed;
      }
      rewind(importFile);

      /* Figure out what we have to do to size and position the imported
       * image correctly. */
      Coord xTrans, yTrans;
      Coord xScale, yScale;

      xTrans = location->x - MIN(x1, x2);
      yTrans = (location->y - location->height + 1) - MIN(x1, x2);
      xScale = location->width / (ABS(x2 - x1) + 1);
      yScale = location->height / (ABS(y2 - y1) + 1);
 
      fprintf(_printFile, "%f %f translate\n", xTrans, yTrans);
      fprintf(_printFile, "%f %f scale\n", xScale, yScale);
    }

    /* Transfer the imported image into the output file. */
    fprintf(_printFile, "\n%%%%BeginDocument: %s\n", filename);

    while (fgets(buf, bufSize, importFile) != NULL) {
      fputs(buf, _printFile);
    }
    if (ferror(importFile)) {
	reportErrSys("Error reading import file");
	result += StatusFailed;
    }

    fprintf(_printFile, "\n%%%%EndDocument\n\n");

    if (fclose(importFile) != 0) {
      reportError("Error closing import file", errno);
      result += StatusWarn;
    }
  }

  /* Clean up after the EPS file. */
  fprintf(_printFile, "\nEndEPSF\n\n");

  return result;
}


/**************************************************************
Print PostScript header.
**************************************************************/

void PSDisplay::PrintPSHeader(char *title, const Rectangle &screenPrintRegion,
    Boolean maintainAspect)
{
  DOASSERT(_printFile != NULL, "No print file");

  _screenPrintRegion = screenPrintRegion;

  /* Print header info. */
  fprintf(_printFile, "%%!PS-Adobe-1.0\n");


  /* Calculate and print the bounding box, based on the dimensions of the
   * region of the screen we're printing, and whether to maintain the same
   * aspect ratio or fill the available area of the paper. */
  Coord width, height, xMargin, yMargin;
  GetPageGeom(width, height, xMargin, yMargin);
  width -= 2 * xMargin;
  height -= 2 * yMargin;

  Coord bbX1, bbY1, bbX2, bbY2;

  if (maintainAspect) {
    double winAspect = screenPrintRegion.height / screenPrintRegion.width;
    double pageAspect = height / width;

    if (winAspect > pageAspect) {
      /* Screen is taller than page. */
      Coord printWidth = height / winAspect;
      bbX1 = xMargin + (width - printWidth) / 2.0;
      bbY1 = yMargin;
      bbX2 = bbX1 + printWidth;
      bbY2 = yMargin + height;
    } else {
      /* Page is taller than screen. */
      Coord printHeight = width * winAspect;
      bbX1 = xMargin;
      bbY1 = yMargin + (height - printHeight) / 2.0;
      bbX2 = xMargin + width;
      bbY2 = bbY1 + printHeight;
    }
  } else {
    /* Go with default dimensions. */
    bbX1 = xMargin;
    bbY1 = yMargin;
    bbX2 = width + xMargin;
    bbY2 = height + yMargin;
  }

  _boundingBox.x = bbX1;
  _boundingBox.y = bbY1;
  _boundingBox.width = bbX2 - bbX1;
  _boundingBox.height = bbY2 - bbY1;

  /* Give ourselves a little margin for error. */
  bbX1 -= 2.0;
  bbY1 -= 2.0;
  bbX2 += 2.0;
  bbY2 += 2.0;

  fprintf(_printFile, "%%%%BoundingBox: %f %f %f %f\n", bbX1, bbY1, bbX2,
    bbY2);


  char *userName = getenv("USER");
  if (userName == NULL) userName = "user unknown";
  fprintf(_printFile, "%%%%Creator: %s\n", userName);

  fprintf(_printFile, "%%%%Title: %s\n", title);

  struct timeval tv;
  gettimeofday(&tv, NULL);
  fprintf(_printFile, "%%%%CreationDate: %s\n",
    DateString((time_t) tv.tv_sec));

  fprintf(_printFile, "%%%%EndComments\n");
  fprintf(_printFile, "%%%%Pages: 1\n");
  fprintf(_printFile, "%%%%EndProlog\n");
  fprintf(_printFile, "%%%%Page: 1 1\n");


  /* Print a procedure to create a rectangular path (to be stroked or
   * filled). */
  fprintf(_printFile, "/DevRect  %% stack: x1, y1, x2, y2\n");
  fprintf(_printFile, "{ newpath\n");
  fprintf(_printFile, "/y2 exch def\n");
  fprintf(_printFile, "/x2 exch def\n");
  fprintf(_printFile, "/y1 exch def\n");
  fprintf(_printFile, "/x1 exch def\n");
  fprintf(_printFile, "x1 y1 moveto\n");
  fprintf(_printFile, "x1 y2 lineto\n");
  fprintf(_printFile, "x2 y2 lineto\n");
  fprintf(_printFile, "x2 y1 lineto\n");
  fprintf(_printFile, "x1 y1 lineto\n");
  fprintf(_printFile, "closepath\n");
  fprintf(_printFile, "} def\n");


  /* Print a procedure to draw a line. */
  fprintf(_printFile, "/DevDrawLine  %% stack: x2, y2, x1, y1\n");
  fprintf(_printFile, "{ newpath\n");
  fprintf(_printFile, "moveto\n");
  fprintf(_printFile, "lineto\n");
  fprintf(_printFile, "stroke\n");
  fprintf(_printFile, "} def\n");


  /* Print a procedure to find text extents. */
  fprintf(_printFile, "/DevTextExtents  %% stack: string\n");
  fprintf(_printFile, "%% returns width height on stack\n");
  fprintf(_printFile, "{ 0 0 moveto\n");
  fprintf(_printFile, "true charpath flattenpath pathbbox\n");
  fprintf(_printFile, "/y2 exch def\n");
  fprintf(_printFile, "/x2 exch def\n");
  fprintf(_printFile, "/y1 exch def\n");
  fprintf(_printFile, "/x1 exch def\n");
  fprintf(_printFile, "x2 x1 sub\n");
  fprintf(_printFile, "y2 y1 sub\n");
  fprintf(_printFile, "} def\n");


  /* Print a procedure to prepare for including an EPS file. */
  fprintf(_printFile, "/BeginEPSF { %%def\n");
  fprintf(_printFile, "  /b4_inc_state save def		"
    "%% Save state for cleanup\n");
  fprintf(_printFile, "  /dict_count countdictstack def	"
    "%% Count objects on dict stack\n");
  fprintf(_printFile, "  /op_count count 1 sub def		"
    "%% Count objects on operand stack\n");
  fprintf(_printFile, "  userdict begin			"
    "%% Push userdict on dict stack\n");
  fprintf(_printFile, "  /showpage {} def			"
    "%% Redefine showpage to null proc\n");
  fprintf(_printFile, "  0 setgray 0 setlinecap		"
    "%% Prepare graphics state\n");
  fprintf(_printFile, "  1 setlinewidth 0 setlinejoin\n");
  fprintf(_printFile, "  10 setmiterlimit [] 0 setdash newpath\n");
  fprintf(_printFile, "  /languagelevel where			"
    "%% If level != 1 then\n");
  fprintf(_printFile, "  {pop languagelevel			"
    "%% set strokeadjust and\n");
  fprintf(_printFile, "  1 ne					"
    "%% overprint to their defaults\n");
  fprintf(_printFile, "    {false setstrokeadjust false setoverprint\n");
  fprintf(_printFile, "    } if\n");
  fprintf(_printFile, "  } if\n");
  fprintf(_printFile, "} bind def\n");


  /* Print a procedure to clean up after including an EPS file. */
  fprintf(_printFile, "/EndEPSF { %%def\n");
  fprintf(_printFile, "  count op_count sub {pop} repeat	"
    "%% Clean up stacks\n");
  fprintf(_printFile, "  countdictstack dict_count sub {end} repeat\n");
  fprintf(_printFile, "  b4_inc_state restore\n");
  fprintf(_printFile, "} bind def\n");

#if 0
  fprintf(_printFile, "%f %f %f %f DevFillRect\n", bbX1, bbY1, bbX2, bbY2);
#endif
}

/**************************************************************
Print PostScript trailer.
**************************************************************/

void PSDisplay::PrintPSTrailer()
{
  DOASSERT(_printFile != NULL, "No print file");

  fprintf(_printFile, "\nshowpage\n");
  fprintf(_printFile, "%%%%Trailer\n");
}

/**************************************************************
Set the geometry of the output page.
**************************************************************/

void PSDisplay::SetUserPageGeom(Coord width, Coord height, Coord xMargin,
  Coord yMargin)
{
  _outputWidth = _userWidth = width;
  _outputHeight = _userHeight = height;
  _outputXMargin = _userXMargin = xMargin;
  _outputYMargin = _userYMargin = yMargin;
}

/**************************************************************
Get the geometry of the output page.
**************************************************************/

void PSDisplay::GetPageGeom(Coord &width, Coord &height, Coord &xMargin,
  Coord &yMargin)
{
  width = _outputWidth * pointsPerInch;
  height = _outputHeight * pointsPerInch;
  xMargin = _outputXMargin * pointsPerInch;
  yMargin = _outputYMargin * pointsPerInch;
}

/**************************************************************
Set the hostname of the Tasvir server.
**************************************************************/
void PSDisplay::SetTasvirServer(const char *server)
{
  int index = _winList.InitIterator();
  while(_winList.More(index)) {
    PSWindowRep *win = _winList.Next(index);
    win->SetDaliServer(server);
  }
  _winList.DoneIterator(index);
}

