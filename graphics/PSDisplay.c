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
  /* do something */
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

  return new PSWindowRep((DeviseDisplay *) this, fgnd, bgnd,
    (PSWindowRep *) parentRep, (int) x, (int) y);
}

/**************************************************************
Destroy a window
**************************************************************/

void PSDisplay::DestroyWindowRep(WindowRep *win)
{
  DO_DEBUG(printf("PSDisplay::DestroyWindowRep(%p)\n", win));

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

/**************************************************************
Print PostScript header.
**************************************************************/

void PSDisplay::PrintPSHeader()
{
  DOASSERT(_printFile != NULL, "No print file");

  /* Print header info. */
  fprintf(_printFile, "%%!PS-Adobe-1.0\n");

  /* Note: we may want to output a bounding box here. RKW 11/15/96. */
//TEMPTEMP -- make bounding box the size of the page (minus the margin?)
// for now

  char *userName = getenv("USER");
  if (userName == NULL) userName = "user unknown";
  fprintf(_printFile, "%%%%Creator: %s\n", userName);

  fprintf(_printFile, "%%%%Title: DEVise visualization\n");

  struct timeval tv;
  gettimeofday(&tv, NULL);
  DateString(tv.tv_sec);
  fprintf(_printFile, "%%%%CreationDate: %s\n", DateString(tv.tv_sec));

  fprintf(_printFile, "%%%%EndComments\n");
  fprintf(_printFile, "%%%%Pages: 1\n");
  fprintf(_printFile, "%%%%EndProlog\n");
  fprintf(_printFile, "%%%%Page: 1 1\n");


  /* Print a procedure to draw a filled rectangle. */
  fprintf(_printFile, "/DevFillRect  %% stack: x1, y1, x2, y2\n");
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
  fprintf(_printFile, "fill\n");
  fprintf(_printFile, "} def\n");


  /* Print a procedure to draw a line. */
  fprintf(_printFile, "/DevDrawLine  %% stack: x1, y1, x2, y2\n");
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
Get the geometry of the output page.
**************************************************************/

void PSDisplay::GetPageGeom(Coord &width, Coord &height, Coord &xMargin,
  Coord &yMargin)
{
  /* Note: the fixed sizes are just temporary -- we need to be able
   * to set the sizes.  RKW 10/25/96. */
  width = 8.5 * pointsPerInch;
  height = 11.0 * pointsPerInch;
  xMargin = 0.5 * pointsPerInch;
  yMargin = 0.5 * pointsPerInch;
}
