/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.27  1997/04/11 18:48:52  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.26  1997/03/25 17:58:55  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.25.4.3  1997/03/15 00:31:05  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.25.4.2  1997/03/07 20:03:58  wenger
  Tasvir images now work in PostScript output; Tasvir images now freed
  on a per-window basis; Tasvir timeout factor can be set on the command
  line; shared memory usage enabled by default.

  Revision 1.25.4.1  1997/02/27 22:46:04  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.25  1997/01/30 15:57:44  wenger
  Fixed compile warnings on Sun.

  Revision 1.24  1997/01/29 17:02:31  wenger
  Fixed PSWindowRep::ScaledText().

  Revision 1.23  1997/01/28 19:46:31  wenger
  Fixed bug 139; better testing of ScaledText() in client/server example;
  fixes to Exit class for client/server library.

  Revision 1.22  1997/01/28 16:50:38  wenger
  Fixed bugs 122 and 124 (reduced data and X axis area so selection rectangle
  doesn't draw over them); Devise now returns a status of 0 when exiting
  normally; cleaned up some of the exit code.

  Revision 1.21  1997/01/24 16:38:24  wenger
  Fixed bugs 078, 103, 125; also improved X font handling (does a better
  job of finding fonts).

  Revision 1.20  1997/01/17 20:31:44  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.19  1997/01/13 18:07:49  wenger
  Fixed bugs 043, 083, 084, 091, 114.

  Revision 1.18  1997/01/10 16:05:38  wenger
  Fixed bugs 090 and 108 (two instances of the same problem).

  Revision 1.17  1996/12/20 18:43:28  wenger
  Unfilled arcs and variable line widths now work in PostScript output.

  Revision 1.16  1996/12/15 20:22:29  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.15  1996/12/11 18:05:37  wenger
  Arc() method now works in PSWindowRep class; put SetSmallFont() method
  back into WindowRep classes for backwards compatibility for Opossum;
  server example program tests more of the WindowRep methods than before;
  removed 'not yet implemented' warnings from some PSWindowRep methods.

  Revision 1.14  1996/12/04 18:12:40  wenger
  Unimplemented methods in PSWindowRep now report an error but do not
  abort when called; fixed code in cslib server example that caused problems
  for Anand; corrected the values returned by some of the NumShapeAttrs()
  methods in MapInterpShape.

  Revision 1.13  1996/12/03 17:00:27  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.12  1996/11/26 15:44:06  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.11  1996/11/23 00:24:11  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.10  1996/11/20 20:34:52  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.9  1996/11/18 23:11:17  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.8  1996/11/13 16:56:10  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.7  1996/11/07 22:40:10  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.6  1996/11/06 17:00:38  wenger
  Text alignment now working for AbsoluteText() method for PostScript
  output. (Direct PostScript output is still disabled pending colors.)

  Revision 1.5  1996/10/28 15:55:43  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.4  1996/10/18 20:34:06  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.3  1996/09/24 19:06:27  wenger
  PostScript code for lines, rects, text at least partially working (disabled
  for commit).  Still needs scaling and colors.  Added files for tarring
  sources to make transfer to Linux machines, etc., easier.

  Revision 1.2  1996/09/19 20:11:52  wenger
  More PostScript output code (still disabled); some code for drawing
  view borders (disabled).

  Revision 1.1  1996/07/10 16:23:02  jussi
  Initial revision.
*/

#include <stdio.h>
#include <math.h>
#ifdef SUN
#include <strings.h>
#endif

#include "PSWindowRep.h"
#include "PSDisplay.h"
#include "DevError.h"

#ifndef LIBCS
#include "DaliIfc.h"
#include "Init.h"
#endif

//#define DEBUG

// Define this to generate debug info in the PostScript output.
//#define PS_DEBUG

#define MAXPIXELDUMP 0

#define ROUND(type, value) ((type)(value + 0.5))

#define USE_PS_PROCEDURES 1


/**********************************************************************
Constructor.
***********************************************************************/

PSWindowRep::PSWindowRep(DeviseDisplay *display,
                          GlobalColor fgndColor, GlobalColor bgndColor,
                          PSWindowRep *parent, int x, int y, int width,
			  int height) :
	WindowRep(display, fgndColor, bgndColor)
{
#if defined(DEBUG)
  printf("PSWindowRep::PSWindowRep(%p)\n", this);
#endif
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);

  _x = x;
  _y = y;
  _width = width;
  _height = height;

  _xorMode = false;

#ifdef LIBCS
  _dashedLine = false;
#endif

#ifndef LIBCS
  _daliServer = NULL;
#endif

#ifdef LIBCS
  ColorMgr::GetColorRgb(fgndColor, _foreground.red, _foreground.green,
    _foreground.blue);
  ColorMgr::GetColorRgb(bgndColor, _background.red, _background.green,
    _background.blue);
#endif
}



/**********************************************************************
Initializer
Must be called only when the output file is open.
***********************************************************************/

void PSWindowRep::Init()
{
#if defined(DEBUG)
  printf("PSWindowRep::Init(%p)\n", this);
#endif
  UpdateWinDimensions();
  _pixToPointTrans.MakeIdentity();
  SetCopyMode();
  WindowRep::SetPattern(Pattern0);
  SetNormalFont();
//TEMPTEMP -- maybe this should clear the background like XWindowRep does
}



/**************************************************************
  destructor 
**************************************************************/

PSWindowRep::~PSWindowRep()
{
#if defined(DEBUG)
  printf("PSWindowRep::~PSWindowRep(%p)\n", this);
#endif
  if (_parent) {
    if (!_parent->_children.Delete(this))
      fprintf(stderr, "Cannot remove child from parent window\n");
  }

  if (_children.Size() > 0)
    reportErrNosys("Child windows should have been destroyed first");

#ifndef LIBCS
  delete [] _daliServer;
#endif
}



/******************************************************************
  If child == true, make window pointed to by 'other' a new child of
  this window.
  If child == false, make window pointed to by 'other' a new parent of
  this window.
*/

void PSWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
    reportErrNosys("PSWindowRep::Reparent() not yet implemented");
    /* do something */
}



/*---------------------------------------------------------------------------*/

void PSWindowRep::PushClip(Coord x, Coord y, Coord w, Coord h)
{
#if defined(DEBUG)
  printf("PSWindowRep::PushClip(%.2f, %.2f, %.2f, %.2f)\n", x, y, w, h);
#endif

  Coord xlow, ylow, xhi, yhi, width, height;
  Coord x1, y1, x2, y2;
  Transform(x, y, x1, y1);
  Transform(x + w, y + h, x2, y2);
  xlow = MIN(x1, x2);
  xhi = MAX(x1, x2);
  ylow = MIN(y1, y2);
  yhi = MAX(y1, y2);
  width = xhi - xlow + 1;
  height = yhi - ylow + 1;
  
#if defined(DEBUG)
  printf("PSWindowRep::PushClip: transformed into (%.2f, %.2f, %.2f, %.2f)\n",
	 xlow, ylow, width, height);
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  SetClipPath(printFile, xlow, ylow, xhi, yhi);
#endif

  WindowRep::_PushClip(xlow, ylow, width, height);

  return;
}



/*---------------------------------------------------------------------------*/

void PSWindowRep::PopClip()
{
#if defined(DEBUG)
  printf("PSWindowRep::PopClip()\n");
#endif

  WindowRep::_PopClip();

  Coord x1, y1, width, height;
  Coord x2, y2;

  if (WindowRep::ClipTop(x1, y1, width, height)) {
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;

#ifdef GRAPHICS
    FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
    fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

    SetClipPath(printFile, x1, y1, x2, y2);
#endif

  } else {

    /* no more clipping */
#ifdef GRAPHICS 
    FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
    fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

    /* Note: the PostScript Ref. Manual says it's dangerous to to an initclip,
     * but there doesn't seem to be any other way to get rid of the clip mask
     * for the previous view (if any) when setting things up for this view. */
    fprintf(printFile, "initclip\n");
#endif
  }

  return;
}



/*---------------------------------------------------------------------------*/
/* import window image */

void PSWindowRep::ImportImage(Coord x, Coord y,
                              DisplayExportFormat format,
                              char *filename)
{
#ifdef DEBUG
  printf("Reading image from file %s\n", filename);
#endif

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Cannot open file %s", filename);
    return;
  }

  reportErrNosys("PSWindowRep::ImportImage() not yet implemented");
  /* do something */

  fclose(fp);

  /* do something */
}



/*---------------------------------------------------------------------------*/
/* export window image */

void PSWindowRep::ExportImage(DisplayExportFormat format, char *filename)
{
  reportErrNosys("PSWindowRep::ExportImage() not yet implemented");
    /* do something */
}



#ifndef LIBCS
/*------------------------------------------------------------------------------
 * function: PSWindowRep::DaliShowImage
 * Show a Dali image in this window.
 */
DevStatus
PSWindowRep::DaliShowImage(Coord centerX, Coord centerY, Coord width,
        Coord height, char *filename, int imageLen, char *image,
        float timeoutFactor, Boolean maintainAspect)
{
#if defined(DEBUG)
  printf("PSWindowRep::DaliShowImage(%f, %f, %f, %f, %s)\n", centerX, centerY,
    width, height, filename != NULL ? filename : "(image)");
#endif

  DevStatus result = StatusOk;

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

#if defined(PS_DEBUG)
  /* Transform from center, size into corner coords. */
  Coord tx1, ty1, tx2, ty2;
  tx1 = centerX - width / 2.0;
  ty1 = centerY - height / 2.0;
  tx2 = centerX + width / 2.0;
  ty2 = centerY + height / 2.0;

  /* Outline the region the image is supposed to occupy -- for
   * debugging purposes. */
  GlobalColor tmpColor = GetFgColor();
  SetFgColor(tmpColor != BlackColor ? BlackColor : WhiteColor);
  DrawLine(printFile, tx1, ty1, tx1, ty2);
  DrawLine(printFile, tx1, ty2, tx2, ty2);
  DrawLine(printFile, tx2, ty2, tx2, ty1);
  DrawLine(printFile, tx2, ty1, tx1, ty1);
  SetFgColor(tmpColor);
#endif

  /* Call procedure to prepare for including EPS file. */
  fprintf(printFile, "\nBeginEPSF\n");

  /* Translate to the correct location -- relative to the page center,
   * since that's how Tasvir is positioning the image. */
  Coord pageWidth;
  Coord pageHeight;
  Coord xMargin;
  Coord yMargin;

  DeviseDisplay::GetPSDisplay()->GetPageGeom(pageWidth, pageHeight, xMargin,
    yMargin);
  fprintf(printFile, "%f %f translate\n", centerX - pageWidth / 2.0,
    centerY - pageHeight / 2.0);

  fprintf(printFile, "\n%%%%BeginDocument: %s\n",
    filename != NULL ? filename : "(image)");

  /* Get the image from Tasvir and put it into the output file. */
  result += DaliIfc::PSShowImage(_daliServer, (int) width, (int) height,
    filename, imageLen, image, printFile, timeoutFactor, maintainAspect);

  fprintf(printFile, "\n%%%%EndDocument\n\n");

  /* Clean up after the EPS file. */
  fprintf(printFile, "\nEndEPSF\n\n");

#endif

  return result;
}
#endif // #ifndef LIBCS



/*---------------------------------------------------------------------------*/
/* color selection interface using Devise colormap */

void PSWindowRep::SetFgColor(GlobalColor fg)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetFgColor(%d)\n", fg);
#endif

  WindowRep::SetFgColor(fg);
  _oldFgndColor = fg;

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();
  
  //TEMPTEMP -- maybe this should go through the PSDisplay to be
  // consistent w/ X stuff
  float red, green, blue;
  ColorMgr::GetColorRgb(fg, red, green, blue);
  fprintf(printFile, "%f %f %f setrgbcolor\n", red, green, blue);
#ifdef LIBCS
  _foreground.red = red;
  _foreground.green = green;
  _foreground.blue = blue;
#endif
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::SetBgColor(GlobalColor bg)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetBgColor(%d)\n", bg);
#endif

  WindowRep::SetBgColor(bg);
#ifdef GRAPHICS
  //reportErrNosys("PSWindowRep::SetBgColor() not yet implemented");
    /* do something */
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::SetWindowBgColor(GlobalColor bg)
{
#ifdef GRAPHICS
  reportErrNosys("PSWindowRep::SetWindowBgColor() not yet implemented");
  /* do something */
#endif
}


#ifdef LIBCS

/*---------------------------------------------------------------------------*/
void PSWindowRep::SetFgRGB(float r, float g, float b)
{
  _foreground.red = r;
  _foreground.green = g;
  _foreground.blue = b;

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();
  
  fprintf(printFile, "%f %f %f setrgbcolor\n", r, g, b);
#endif
}


/*---------------------------------------------------------------------------*/
void PSWindowRep::SetBgRGB(float r, float g, float b)
{
  _background.red = r;
  _background.green = g;
  _background.blue = b;
}


/*---------------------------------------------------------------------------*/
void PSWindowRep::GetFgRGB(float &r, float &g, float &b)
{
  r = _foreground.red;
  g = _foreground.green;
  b = _foreground.blue;
}


/*---------------------------------------------------------------------------*/
void PSWindowRep::GetBgRGB(float &r, float &g, float &b)
{
  r = _background.red;
  g = _background.green;
  b = _background.blue;
}

#endif


/*---------------------------------------------------------------------------*/
void
PSWindowRep::SetLineWidth(int w)
{
  WindowRep::SetLineWidth(w);

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

  // Scale width to points.  Note: in most cases X and Y scaling should be
  // the same, so we can just use X for the scaling.
  Coord tx3, ty3, tx4, ty4;
  TransPixToPoint(0.0, 0.0, tx3, ty3);
  TransPixToPoint((Coord) w, 0.0, tx4, ty4);

  fprintf(printFile, "%f setlinewidth\n", fabs(tx4 - tx3));
#endif
}

#ifdef LIBCS
/*---------------------------------------------------------------------------*/
void PSWindowRep::SetDashes(int dashCount, int dashes[], int startOffset)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetDashes(%d, ...)\n", dashCount);
#endif

  char errBuf[256];

  if (dashCount > 0) {
    _dashedLine = true;

    // Figure out the scaling factor to convert the dash lengths from pixels
    // to points.  Note: in most cases X and Y scaling should be the same,
    // so we can just use X for the scaling.
    Coord tx1, ty1, tx2, ty2;
    TransPixToPoint(0.0, 0.0, tx1, ty1);
    TransPixToPoint(1.0, 0.0, tx2, ty2);
    Coord scale = tx2 - tx1;

    strcpy(_dashList, "[");
    int dashNum;
    char buffer[32];
    for (dashNum = 0; dashNum < dashCount; dashNum++) {
      Coord dash = dashes[dashNum] * scale;
      if (dash < 0.0) {
	sprintf(errBuf, "Illegal dash value (%f)", dash);
	reportErrNosys(errBuf);
	dash = 0.0;
      }
      sprintf(buffer, "%f ", dash);
      strncat(_dashList, buffer, _dashListSize);
    }

    // Make sure there's room for start offset.
    _dashList[_dashListSize - 32] = '\0';
    strcat(_dashList, "] ");
    sprintf(buffer, "%d", startOffset);
    strncat(_dashList, buffer, _dashListSize);

    // Make sure the string is terminated even if we ran out of space.
    _dashList[_dashListSize - 1] = '\0';
  } else {
    _dashedLine = false;
  }
}
#endif



/* drawing primitives */

/*---------------------------------------------------------------------------*/
void PSWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("PSWindowRep::DrawPixel: %.2f %.2f --> %.2f %.2f\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  DrawDot(printFile, tx, ty);
#endif
}



static Point pointArray[WINDOWREP_BATCH_SIZE];
static Rectangle rectArray[WINDOWREP_BATCH_SIZE];



/*---------------------------------------------------------------------------*/
void PSWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("PSWindowRep::DrawPixelArray: %d points, width = %d\n", num, width);

  printf("Transformation matrix: ");
  PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", x[k], y[k]);
  }
  printf("\n");
#endif
#endif

  if (width == 1) {

    /* each "pixel" is no wider than one screen pixel */

    for(int i = 0; i < num; i++) {
      Transform(x[i], y[i], pointArray[i].x, pointArray[i].y);
    }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
    printf("\nAfter transformation:\n\n");
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
	printf("\n");
      printf("(%f,%f)",pointArray[k].x, pointArray[k].y);
    }
    printf("\n");
#endif
#endif

#ifdef GRAPHICS
    FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
    fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

    for (int pointNum = 0; pointNum < num; pointNum++)
    {
      DrawDot(printFile, pointArray[pointNum].x, pointArray[pointNum].y);
    }
#endif

    return;
  }


  /* each "pixel" is wider than one screen pixel */

  Coord halfWidth = ((Coord) width) / 2;
  for(int i = 0; i < num; i++) {
    Coord tx,ty;
    Transform(x[i], y[i], tx, ty);
    rectArray[i].x = tx - halfWidth;
    rectArray[i].y = ty - halfWidth;
    rectArray[i].width = (Coord) width;
    rectArray[i].height = (Coord) width;
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation 2:\n\n");
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%f,%f)", rectArray[k].x, rectArray[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  for(int pointNum = 0; pointNum < num; pointNum++) {
    Coord x1 = rectArray[pointNum].x;
    Coord y1 = rectArray[pointNum].y;
    Coord x2 = x1 + rectArray[pointNum].width;
    Coord y2 = y1 + rectArray[pointNum].height;
    DrawFilledRect(printFile, x1, y1, x2, y2);
  }
#endif
}



/*---------------------------------------------------------------------------*/
/* Fill rectangles, variable width/height */

void PSWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			       Coord *height, int num)
{
#ifdef DEBUG
  printf("PSWindowRep::FillRectArray: %d points\n", num);

  printf("Transformation matrix: ");
  PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 6 == 0)
      printf("\n");
    printf("(%.2f,%.2f,%.2f,%.2f)", xlow[k], ylow[k], width[k], height[k]);
  }
  printf("\n");
#endif
#endif

  for(int i = 0; i < num; i++) {
    Coord x1, y1, x2, y2;
    Coord txlow, tylow, txmax, tymax;
    Transform(xlow[i], ylow[i] + height[i], x1, y1);
    Transform(xlow[i] + width[i], ylow[i], x2, y2);
    txlow = MIN(x1, x2);
    txmax = MAX(x1, x2);
    tylow = MIN(y1, y2);
    tymax = MAX(y1, y2);
    
    unsigned pixelWidth = (unsigned)(txmax - txlow + 1);
    if (pixelWidth == 0) pixelWidth = 1;
    unsigned pixelHeight = unsigned(tymax - tylow + 1);
    if (pixelHeight == 0) pixelHeight = 1;
    
    rectArray[i].x = txlow;
    rectArray[i].y = tylow;
    rectArray[i].width = pixelWidth;
    rectArray[i].height = pixelHeight;
  }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 6 == 0)
      printf("\n");
    printf("(%f,%f,%f,%f)", rectArray[k].x, rectArray[k].y,
	   rectArray[k].width, rectArray[k].height);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  for(int pointNum = 0; pointNum < num; pointNum++) {
    Coord x1 = rectArray[pointNum].x;
    Coord y1 = rectArray[pointNum].y;
    Coord x2 = x1 + rectArray[pointNum].width;
    Coord y2 = y1 + rectArray[pointNum].height;
    DrawFilledRect(printFile, x1, y1, x2, y2);
  }
#endif
}



/*---------------------------------------------------------------------------*/
/* Fill rectangles, same width/height */

void PSWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord width,
			       Coord height, int num)
{
#ifdef DEBUG
  printf("PSWindowRep::FillRectArray: %d points, width %.2f, height %.2f\n",
         num, width, height);

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", xlow[k], ylow[k]);
  }
  printf("\n");
#endif
#endif

  for(int i = 0; i < num; i++) {
    Coord txlow, tylow, txmax, tymax;
    Coord x1,y1,x2,y2;
    Transform(xlow[i], ylow[i] + height, x1, y1);
    Transform(xlow[i] + width, ylow[i], x2, y2);
    txlow = MIN(x1, x2);
    txmax= MAX(x1, x2);
    tylow = MIN(y1, y2);
    tymax = MAX(y1, y2);

    unsigned pixelWidth = (unsigned)(txmax - txlow+1);
    if (pixelWidth == 0) pixelWidth = 1;
    unsigned pixelHeight = unsigned(tymax - tylow+1);
    if (pixelHeight == 0) pixelHeight=1;
    
    rectArray[i].x = txlow;
    rectArray[i].y = tylow;
    rectArray[i].width = pixelWidth;
    rectArray[i].height = pixelHeight;
  }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation: width %f, height %f\n\n",
	 rectArray[0].width, rectArray[0].height);
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%f,%f)", rectArray[k].x, rectArray[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  for(int pointNum = 0; pointNum < num; pointNum++) {
    Coord x1 = rectArray[pointNum].x;
    Coord y1 = rectArray[pointNum].y;
    Coord x2 = x1 + rectArray[pointNum].width;
    Coord y2 = y1 + rectArray[pointNum].height;
    DrawFilledRect(printFile, x1, y1, x2, y2);
  }
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::FillRect(Coord xlow, Coord ylow, Coord width, Coord height)
{
#ifdef DEBUG
  printf("PSWindowRep::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         xlow, ylow, width, height);
#endif

  /* XXX: need to clip rect against window dimensions */

  Coord txlow, tylow, txmax, tymax;
  Coord x1, y1, x2, y2;
  Transform(xlow, ylow + height, x1, y1);
  Transform(xlow + width, ylow, x2, y2);
  txlow = MIN(x1, x2);
  txmax = MAX(x1, x2);
  tylow = MIN(y1, y2);
  tymax = MAX(y1, y2);
  
  /* fill rectangle, remember that the window coordinate
     system starts at the upper left corner */

  unsigned pixelWidth = (unsigned)(ROUND(int, txmax) - ROUND(int, txlow) + 1);
  if (pixelWidth == 0)
    pixelWidth = 1;
  unsigned pixelHeight = (unsigned)(ROUND(int, tymax) - ROUND(int, tylow) + 1);
  if (pixelHeight == 0)
    pixelHeight = 1;
  
#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
	 ROUND(int, txlow), ROUND(int, tylow), pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  DrawFilledRect(printFile, txlow, tylow, txmax, tymax);
#endif
}



/*---------------------------------------------------------------------------*/
/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void PSWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			       Coord minWidth, Coord minHeight)
{
#if defined(DEBUG)
  printf("PSWindowRep::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         x, y, width, height);
#endif

  width = MAX(width, minWidth);
  height = MAX(height, minHeight);

  Coord x1 = x - width / 2;
  Coord y1 = y - width / 2;
  Coord x2 = x + width / 2;
  Coord y2 = y + width / 2;

  Coord tx1, ty1, tx2, ty2;

#if 1
  /* Pixel to point transformation is already taken care of before this
   * function is called. */
  tx1 = x1;
  ty1 = y1;
  tx2 = x2;
  ty2 = y2;
#else
  TransPixToPoint(x1, y1, tx1, ty1);
  TransPixToPoint(x2, y2, tx2, ty2);
#endif

#if defined(DEBUG)
  printf("After transformation: (%f, %f), (%f, %f)\n", tx1, ty1, tx2, ty2);
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  DrawFilledRect(printFile, tx1, ty1, tx2, ty2);
#endif
}



/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void PSWindowRep::FillPoly(Point *pts, int pointCount)
{
#ifdef DEBUG
  printf("PSWindowRep::FillPoly: %d points\n", pointCount);

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (pointCount > MAXPIXELDUMP ? MAXPIXELDUMP : pointCount);
    k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[k].x, pts[k].y);
  }
  printf("\n");
#endif
#endif

  if (pointCount <= 0)
    return;

  if (pointCount > WINDOWREP_BATCH_SIZE) {
    printf("Point array too large: %d > %d\n", pointCount,
      WINDOWREP_BATCH_SIZE);
    pointCount = WINDOWREP_BATCH_SIZE;
  }

  for(int i = 0; i < pointCount; i++) {
    Transform(pts[i].x, pts[i].y, pointArray[i].x, pointArray[i].y);
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (pointCount > MAXPIXELDUMP ? MAXPIXELDUMP : pointCount);
    k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%f, %f)", pointArray[k].x, pointArray[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", pointArray[0].x, pointArray[0].y);
  for (int pointNum = 1; pointNum < pointCount; pointNum++) {
    fprintf(printFile, "%f %f lineto\n", pointArray[pointNum].x,
      pointArray[pointNum].y);
  }
  fprintf(printFile, "%f %f lineto\n", pointArray[0].x, pointArray[0].y);
  fprintf(printFile, "closepath\n");
  fprintf(printFile, "%s\n", _xorMode ? "stroke" : "fill");
#endif
}



/*************************************************************************
Draw polygon, given PIXEL coordinates of the corners of the polygon.
No transformation of the coordinates is done.
*************************************************************************/

void PSWindowRep::FillPixelPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("PSWindowRep::FillPixelPoly: %d points\n",n);

#if MAXPIXELDUMP > 0
  for(int j = 0; j < (n < MAXPIXELDUMP ? MAXPIXELDUMP : n); j++) {
    if ((j + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[j].x, pts[j].y);
  }
  printf("\n");
#endif
#endif

  if (n <= 0)
    return;

  if (n > WINDOWREP_BATCH_SIZE) {
    printf("Point array too large: %d > %d\n", n, WINDOWREP_BATCH_SIZE);
    n = WINDOWREP_BATCH_SIZE;
  }

#if 0
  for(int i = 0;  i < n; i++ ) {
    points[i].x = ROUND(short, pts[i].x);
    points[i].y = ROUND(short, pts[i].y);
  }
#endif

#ifdef GRAPHICS
  reportErrNosys("PSWindowRep::FillPixelPoly() not yet implemented");
  /* do something */
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::Arc(Coord xCenter, Coord yCenter, Coord horizDiam,
		      Coord vertDiam, Coord startAngle, Coord endAngle)
{
#ifdef DEBUG
  printf("PSWindowRep::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
	 xCenter, yCenter, horizDiam, vertDiam, startAngle, endAngle);
#endif
  
  Coord tx, ty, tempX, tempY;
  Transform(xCenter, yCenter, tx, ty);
  Transform(xCenter + horizDiam, yCenter + vertDiam, tempX, tempY);
  Coord realHorD = fabs(tempX - tx);
  Coord realVertD = fabs(tempY - ty);
  Coord startDeg = ToDegree(startAngle);
  Coord endDeg = ToDegree(endAngle) + startDeg;

  Coord radius = horizDiam / 2.0;
  Coord diamRatio = 1.0;

  Boolean circular = (horizDiam == vertDiam);
  if (!circular) {
    diamRatio = vertDiam / horizDiam;
  }

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  Boolean fill = (GetPattern() != -Pattern1);

  /* Change coordinate system for non-circular arc. */
  if (!circular) {
    fprintf(printFile, "gsave\n");
    fprintf(printFile, "1.0 %f scale\n", diamRatio);
  }

  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", tx, ty / diamRatio);

  fprintf(printFile, "%f %f %s\n", radius * cos(startAngle),
    radius * sin(startAngle), fill ? "rlineto" : "rmoveto");
  fprintf(printFile, "%f %f %f %f %f arc\n", tx, ty / diamRatio, radius,
    startDeg, endDeg);
  fprintf(printFile, "closepath\n");

  if (fill) {
    fprintf(printFile, "%s\n", _xorMode ? "stroke" : "fill");
  } else {
    fprintf(printFile, "stroke\n");
  }

  /* Set the coordinate system back the way it was. */
  if (!circular) {
    fprintf(printFile, "grestore\n");
  }
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, 
		      Coord width)
{
#if defined(DEBUG)
  printf("PSWindowRep::Line %.2f,%.2f,%.2f,%.2f\n", x1, y1, x2, y2);
#endif
  
  Coord tx1, ty1, tx2, ty2;
  Transform(x1 ,y1, tx1, ty1);
  Transform(x2, y2, tx2, ty2);

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  // Scale width to points.
  Coord tx3, ty3, tx4, ty4;
  TransPixToPoint(0.0, 0.0, tx3, ty3);
  TransPixToPoint(width, 0.0, tx4, ty4);

  fprintf(printFile, "%f setlinewidth\n", fabs(tx4 - tx3));
#ifdef LIBCS
  if (_dashedLine) fprintf(printFile, "%s setdash\n", _dashList);
#endif
  DrawLine(printFile, tx1, ty1, tx2, ty2);
#ifdef LIBCS
  if (_dashedLine) fprintf(printFile, "[] 0 setdash\n");
#endif
  fprintf(printFile, "0 setlinewidth\n");
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
#ifdef DEBUG
  printf("PSWindowRep::AbsoluteLine %d,%d,%d,%d\n", x1, y1, x2, y2);
#endif
  
#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", __FUNCTION__);
#endif

  Coord x1new;
  Coord y1new;
  Coord x2new;
  Coord y2new;
  TransPixToPoint(x1, y1, x1new, y1new);
  TransPixToPoint(x2, y2, x2new, y2new);

  // Scale width to points.
  Coord tx3, ty3, tx4, ty4;
  TransPixToPoint(0.0, 0.0, tx3, ty3);
  TransPixToPoint(width, 0.0, tx4, ty4);

  fprintf(printFile, "%f setlinewidth\n", fabs(tx4 - tx3));
#ifdef LIBCS
  if (_dashedLine) fprintf(printFile, "%s setdash\n", _dashList);
#endif
  DrawLine(printFile, x1new, y1new, x2new, y2new);
#ifdef LIBCS
  if (_dashedLine) fprintf(printFile, "[] 0 setdash\n");
#endif
  fprintf(printFile, "0 setlinewidth\n");
#endif
}



/*---------------------------------------------------------------------------*/
/* Draw absolute text */

void PSWindowRep::AbsoluteText(char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      TextAlignment alignment, 
			      Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("PSWindowRep::AbsoluteText: %s at %.2f,%.2f,%.2f,%.2f\n",
	 text, x, y, width, height);
#endif

  DrawText(false, text, x, y, width, height, alignment, skipLeadingSpace);
}



/*---------------------------------------------------------------------------*/
/* Draw scaled text */

void PSWindowRep::ScaledText(char *text, Coord x, Coord y, Coord width,
		       Coord height, TextAlignment alignment,
		       Boolean skipLeadingSpace)
{
#if defined(DEBUG)
  printf("PSWindowRep::ScaledText: %s at %.2f,%.2f,%.2f,%.2f\n",
	 text, x, y, width, height);
#endif

  DrawText(true, text, x, y, width, height, alignment, skipLeadingSpace);
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::SetXorMode()
{
#if defined(DEBUG)
  printf("PSWindowRep::SetXorMode\n");
#endif

  if (_xorMode) return;

  _xorMode = true;

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

  _oldFgndColor = GetFgColor();

  float red, green, blue;
  ColorMgr::GetColorRgb(GetBgColor(), red, green, blue);
  red = 1.0 - red;
  green = 1.0 - green;
  blue = 1.0 - blue;
  fprintf(printFile, "%f %f %f setrgbcolor\n", red, green, blue);
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::SetCopyMode()
{
#ifdef DEBUG
  printf("PSWindowRep::SetCopyMode\n");
#endif

  if (!_xorMode) return;

  _xorMode = false;

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

  if (GetFgColor() != _oldFgndColor) {
    SetFgColor(_oldFgndColor);
  }
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::SetFont(char *family, char *weight, char *slant,
                          char *width, float pointSize)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetFont(%s %s %s %s %f\n", family, weight, slant,
    width, pointSize);
#endif

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

  if (!strcasecmp(family, "new century schoolbook")) {
    family = "NewCenturySchoolbook";
  }

  char *boldString = "";
  if (!strcasecmp(weight, "bold")) {
    boldString = "-Bold";
  }

  char *angleString = "";
  if (!strcasecmp(slant, "i")) {
    angleString = "-Oblique";
  }

  _fontSize = pointSize;
  /* Adjust the font size according to the relationship between X and
   * PostScript, so the font size stays the same in relation to the
   * window size. */
  Coord x1, x2, y1, y2;
  TransPixToPoint(0.0, 0.0, x1, y1);
  TransPixToPoint(0.0, 1.0, x2, y2);
  Coord yScale = ABS(y2 - y1);
  _fontSize *= yScale;

  fprintf(printFile, "/%s%s%s findfont\n", family, boldString, angleString);
  fprintf(printFile, "%f scalefont\n", _fontSize);
  fprintf(printFile, "setfont\n");
#endif
}



/*---------------------------------------------------------------------------*/
void PSWindowRep::UpdateWinDimensions()
{
#if 0
  reportErrNosys("PSWindowRep::UpdateWinDimensions() not yet implemented");
#endif
  /* do something */
}



/*---------------------------------------------------------------------------*/
/* Get window rep dimensions */

void PSWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}



/*---------------------------------------------------------------------------*/
/* Set window rep dimensions */

void PSWindowRep::SetDimensions(unsigned int width, unsigned int height)
{
  _width = width;
  _height = height;
}


/*---------------------------------------------------------------------------*/
/* Get window rep origin */

void PSWindowRep::Origin(int &x, int &y)
{
  x = 0;
  y = 0;
}



/*---------------------------------------------------------------------------*/
/* Get window rep absolute origin */

void PSWindowRep::AbsoluteOrigin(int &x, int &y)
{
  x = _x;
  y = _y;
}



/*---------------------------------------------------------------------------*/
/* Set window rep absolute origin */

void PSWindowRep::SetAbsoluteOrigin(int x, int y)
{
  _x = x;
  _y = y;
}



/*---------------------------------------------------------------------------*/
/* Set up the "pixel" to point transform according to the size and location
 * of the screen window, and the size of the page to output. */

void PSWindowRep::SetPPTrans(const Rectangle &viewGeom,
  const Rectangle &parentGeom)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetPPTrans()\n");
  printf("  %f %f %f %f\n", viewGeom.x, viewGeom.y, viewGeom.width,
    viewGeom.height);
  printf("  %f %f %f %f\n", parentGeom.x, parentGeom.y, parentGeom.width,
    parentGeom.height);
#endif

  /* Get geometry of output region on the page here. */
  Rectangle boundingBox;

  DeviseDisplay::GetPSDisplay()->GetBoundingBox(boundingBox);

  _pixToPointTrans.MakeIdentity();

  Coord xScale = boundingBox.width / parentGeom.width;
  Coord yScale = -1.0 * boundingBox.height / parentGeom.height;
  _pixToPointTrans.Scale(xScale, yScale);
  Coord widthAdj = (viewGeom.x - parentGeom.x) /
    parentGeom.width * boundingBox.width;
  Coord heightAdj = (viewGeom.y - parentGeom.y) /
    parentGeom.height * boundingBox.height;
  _pixToPointTrans.Translate(widthAdj + boundingBox.x,
    boundingBox.height - heightAdj + boundingBox.y);
}



/*---------------------------------------------------------------------------*/
/* Move and resize window, relative to the parent */

void PSWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("Moving PSWindowRep 0x%p to %d,%d, size %u,%u\n", this,
	 x, y, w, h);
#endif

#if 0
  reportErrNosys("PSWindowRep::MoveResize() not yet implemented");
#endif
  /* do something */

  UpdateWinDimensions();
}



/*---------------------------------------------------------------------------*/
/* Iconify window. Not guaranteed to succeed. */

void PSWindowRep::Iconify()
{
#if 0
  reportErrNosys("PSWindowRep::Iconify() not yet implemented");
#endif
    /* do something */
}



/*---------------------------------------------------------------------------*/
/* Raise window to top of stacking order */

void PSWindowRep::Raise()
{
#ifdef DEBUG
  printf("PSWindowRep::Raise window %p\n", this);
#endif

#if 0
  reportErrNosys("PSWindowRep::Raise() not yet implemented");
#endif
  /* do something */
}



/*---------------------------------------------------------------------------*/
/* Lower window to bottom of stacking order */

void PSWindowRep::Lower()
{
#ifdef DEBUG
  printf("PSWindowRep::Lower window %p:\n", this);
#endif

#if 0
  reportErrNosys("PSWindowRep::Lower() not yet implemented");
#endif
  /* do something */
}



/*---------------------------------------------------------------------------*/
/* Draw a line (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::DrawLine(FILE *printFile, Coord x1, Coord y1,
    Coord x2, Coord y2)
{
#if USE_PS_PROCEDURES
  fprintf(printFile, "%f %f %f %f DevDrawLine\n", x2, y2, x1, y1);
#else
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1, y1);
  fprintf(printFile, "%f %f lineto\n", x2, y2);
  fprintf(printFile, "stroke\n");
#endif

  return;
}



/*---------------------------------------------------------------------------*/
/* Draw a filled rectangle (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::DrawFilledRect(FILE *printFile, Coord x1, Coord y1,
    Coord x2, Coord y2)
{
#if USE_PS_PROCEDURES
  fprintf(printFile, "%f %f %f %f DevRect\n", x1, y1, x2, y2);
#else
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y1);
  fprintf(printFile, "closepath\n");
#endif

  /* This is kind of a kludge so that the outlining of the text shapes works
   * correctly. */
  if ((_pattern == Pattern1) || (_pattern == (Pattern) -1)) {
    fprintf(printFile, "%s\n", "stroke");
  } else {
    fprintf(printFile, "%s\n", _xorMode ? "stroke" : "fill");
  }

  return;
}



/*---------------------------------------------------------------------------*/
/* Set the clip path to the given rectangle (coordinates must have already
 * been scaled if that is necessary). */
void PSWindowRep::SetClipPath(FILE *printFile, Coord x1, Coord y1,
    Coord x2, Coord y2)
{
  /* Note: the PostScript Ref. Manual says it's dangerous to to an initclip,
   * but there doesn't seem to be any other way to get rid of the clip mask
   * for the previous view (if any) when setting things up for this view. */
  fprintf(printFile, "initclip\n");
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y1);
  fprintf(printFile, "closepath\n");
  fprintf(printFile, "clip\n");
  fprintf(printFile, "newpath\n");

  return;
}


/*---------------------------------------------------------------------------*/
/* Draw a dot (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::DrawDot(FILE *printFile, Coord x1, Coord y1,
    Coord size)
{
  Coord halfsize = size / 2.0;
#if USE_PS_PROCEDURES
  fprintf(printFile, "%f %f %f %f DevRect\n", x1 - halfsize,
    y1 - halfsize, x1 + halfsize, y1 + halfsize);
  fprintf(printFile, "%s\n", _xorMode ? "stroke" : "fill");
#else
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1 - halfsize, y1 - halfsize);
  fprintf(printFile, "%f %f lineto\n", x1 - halfsize, y1 + halfsize);
  fprintf(printFile, "%f %f lineto\n", x1 + halfsize, y1 + halfsize);
  fprintf(printFile, "%f %f lineto\n", x1 + halfsize, y1 - halfsize);
  fprintf(printFile, "%f %f lineto\n", x1 - halfsize, y1 - halfsize);
  fprintf(printFile, "closepath\n");
  fprintf(printFile, "%s\n", _xorMode ? "stroke" : "fill");
#endif

  return;
}


/*---------------------------------------------------------------------------*/
/* Draw scaled or absolute text */

void PSWindowRep::DrawText(Boolean scaled, char *text, Coord x, Coord y,
		       Coord width, Coord height, TextAlignment alignment,
		       Boolean skipLeadingSpace)
{
  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  Transform(x, y, tx1, ty1);
  Transform(x + width, y + height, tx2, ty2);
  Coord winX, winY, winWidth, winHeight; /* box in window coord */
  winX = ROUND(int, MIN(tx1, tx2));
  winY = ROUND(int, MIN(ty1, ty2));
  winWidth = ROUND(int, MAX(tx1, tx2)) - winX + 1;
  winHeight = ROUND(int, MAX(ty1, ty2)) - winY + 1;

  if (skipLeadingSpace) {
    /* skip leading spaces before drawing text */
    while (*text == ' ')
      text++;
  }
  
  int textLength = strlen(text);
  if (textLength <= 0) return;

  char *comment;
  char *calculation;

  /* Here, instead of actually calculating the text position, we generate
   * the correct PostScript code to calculate the position. */
  GetAlignmentStrings(alignment, comment, calculation);

#ifdef GRAPHICS
  FILE * printFile = DeviseDisplay::GetPSDisplay()->GetPrintFile();

#if defined(PS_DEBUG)
  fprintf(printFile, "%% PSWindowRep::%s()\n", scaled ? "ScaledText" :
    "AbsoluteText");
#endif

#if defined(PS_DEBUG)
  /* Draw the "text window" within which the text is positioned -- for
   * debugging purposes. */
  fprintf(printFile, "[3] 0 setdash\n");
  DrawLine(printFile, tx1, ty1, tx1, ty2);
  DrawLine(printFile, tx1, ty2, tx2, ty2);
  DrawLine(printFile, tx2, ty2, tx2, ty1);
  DrawLine(printFile, tx2, ty1, tx1, ty1);
  fprintf(printFile, "[] 0 setdash\n");
#endif

  fprintf(printFile, "newpath\n");

  /* Get the width and height of the given string in the given font. */
  fprintf(printFile, "(%s) DevTextExtents\n", text);
  fprintf(printFile, "/textHeight exch def\n");
  fprintf(printFile, "/textWidth exch def\n");

  fprintf(printFile, "/winX %f def\n", winX);
  fprintf(printFile, "/winY %f def\n", winY);
  fprintf(printFile, "/winWidth %f def\n", winWidth);
  fprintf(printFile, "/winHeight %f def\n", winHeight);

  if (scaled) {
    /* Now scale the text to fit the window. */
    fprintf(printFile, "%f textWidth div\n", winWidth);
    fprintf(printFile, "/xScale exch def\n");
    fprintf(printFile, "%f textHeight div\n", winHeight);
    fprintf(printFile, "/yScale exch def\n");
    fprintf(printFile, "xScale yScale gt\n");
    fprintf(printFile, "{/xyScale yScale def} {/xyScale xScale def} ifelse\n");

    /* Adjust the text width and height values by the scale factor. */
    fprintf(printFile, "textWidth xyScale mul\n");
    fprintf(printFile, "/textWidth exch def\n");
    fprintf(printFile, "textHeight xyScale mul\n");
    fprintf(printFile, "/textHeight exch def\n");
  }

  /* Calculate some values based on the "text window" size and the size
   * of the text. */
  fprintf(printFile, "/widthDiff winWidth textWidth sub def\n");
  fprintf(printFile, "/halfWidthDiff widthDiff 0.5 mul def\n");
  fprintf(printFile, "/heightDiff winHeight textHeight sub def\n");
  fprintf(printFile, "/halfHeightDiff heightDiff 0.5 mul def\n");

#if defined(PS_DEBUG)
  /* Print the comment if necessary to help debug the PostScript output. */
  fprintf(printFile, "%s\n", comment);
#endif

  /* Now calculate the location of the text according to the
   * specified alignment and the size of the text. */
  fprintf(printFile, "%s", calculation);

  /* Move to the calculation location. */
  fprintf(printFile, "textX textY moveto\n");

  if (scaled) {
    /* Save the graphics state and then set the scaling so that the text
     * will fit the window. */
    fprintf(printFile, "gsave\n");
    fprintf(printFile, "xyScale xyScale scale\n");
  }

  fprintf(printFile, "(%s) show\n", text);

  if (scaled) {
    /* Set the coordinate system back the way it was. */
    fprintf(printFile, "grestore\n");
  }
#endif
}


/*---------------------------------------------------------------------------*/
/* Draw a dot (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::GetAlignmentStrings(TextAlignment alignment, char *&comment,
    char *&calculation)
{
  comment = "";
  calculation = "";

  switch(alignment) {
  case AlignNorthWest:
    comment = "% AlignNorthWest";
    calculation = "/textX winX def\n"
      "/textY winY heightDiff add def\n";
    break;

  case AlignNorth:
    comment = "% AlignNorth";
    calculation = "/textX winX halfWidthDiff add def\n"
      "/textY winY heightDiff add def\n";
    break;

  case AlignNorthEast:
    comment = "% AlignNorthEast";
    calculation = "/textX winX widthDiff add def\n"
      "/textY winY heightDiff add def\n";
    break;

  case AlignWest: 
    comment = "% AlignWest";
    calculation = "/textX winX def\n"
      "/textY winY halfHeightDiff add def\n";
    break;

  case AlignCenter: 
    comment = "% AlignCenter";
    calculation = "/textX winX halfWidthDiff add def\n"
      "/textY winY halfHeightDiff add def\n";
    break;

  case AlignEast:
    comment = "% AlignEast";
    calculation = "/textX winX widthDiff add def\n"
      "/textY winY halfHeightDiff add def\n";
    break;

  case AlignSouthWest:
    comment = "% AlignSouthWest";
    calculation = "/textX winX def\n"
      "/textY winY def\n";
    break;

  case AlignSouth:
    comment = "% AlignSouth";
    calculation = "/textX winX halfWidthDiff add def\n"
      "/textY winY def\n";
    break;

  case AlignSouthEast:
    comment = "% AlignSouthEast";
    calculation = "/textX winX widthDiff add def\n"
      "/textY winY def\n";
    break;

  default:
    comment = "error";
    DOASSERT(false, "Illegal alignment option");
    break;
  }
}
