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

#include "PSWindowRep.h"
#include "PSDisplay.h"
#ifndef LIBCS
#include "Init.h"
#endif

//#define DEBUG
//#define PS_DEBUG	// Define this to generate debug info in the
			// PostScript output.
#define MAXPIXELDUMP 0

#define ROUND(type, value) ((type)(value + 0.5))

typedef struct {
    short x, y;
} XPoint;
    
typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;
    
/**********************************************************************
Constructor.
***********************************************************************/

PSWindowRep::PSWindowRep(DeviseDisplay *display,
                          Color fgndColor, Color bgndColor,
                          PSWindowRep *parent, int x, int y) :
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

  Init();
}

/**********************************************************************
Initializer
***********************************************************************/

void PSWindowRep::Init()
{
#if defined(DEBUG)
  printf("PSWindowRep::Init(%p)\n", this);
#endif
  UpdateWinDimensions();
  _pixToPointTrans.MakeIdentity();
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
    fprintf(stderr, "Child windows should have been destroyed first\n");
}

/******************************************************************
  If child == true, make window pointed to by 'other' a new child of
  this window.
  If child == false, make window pointed to by 'other' a new parent of
  this window.
*/

void PSWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
    /* do something */
}

/******************************************************************/

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
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  /* Note: the PostScript Ref. Manual says it's dangerous to to an initclip,
   * but there doesn't seem to be any other way to get rid of the clip mask
   * for the previous view (if any) when setting things up for this view. */
  fprintf(printFile, "initclip\n");
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", xlow, ylow);
  fprintf(printFile, "%f %f lineto\n", xlow, yhi);
  fprintf(printFile, "%f %f lineto\n", xhi, yhi);
  fprintf(printFile, "%f %f lineto\n", xhi, ylow);
  fprintf(printFile, "%f %f lineto\n", xlow, ylow);
  fprintf(printFile, "closepath\n");
  fprintf(printFile, "clip\n");
  fprintf(printFile, "newpath\n");
#endif

  WindowRep::_PushClip(xlow, ylow, width, height);

  return;
}

/*******************************************************************/

void PSWindowRep::PopClip()
{
#if defined(DEBUG)
  printf("PSWindowRep::PopClip()\n");
#endif
  WindowRep::_PopClip();
  Coord x, y, w, h;
  XRectangle rect; 

  if (WindowRep::ClipTop(x, y, w, h)) {
    rect.x = ROUND(short, x);
    rect.y = ROUND(short, y);
    rect.width = (unsigned short)ceil(w);
    rect.height = (unsigned short)ceil(h);
#ifdef GRAPHICS
    /* do something */
#endif

  } else {

    /* no more clipping */
#ifdef GRAPHICS 
    /* do something */
#endif
  }

  return;
}

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

  /* do something */

  fclose(fp);

  /* do something */
}

/* export window image */

void PSWindowRep::ExportImage(DisplayExportFormat format, char *filename)
{
    /* do something */
}

/* color selection interface using Devise colormap */

void PSWindowRep::SetFgColor(Color fg)
{
  WindowRep::SetFgColor(fg);
#ifdef GRAPHICS
    /* do something */
#endif
}

void PSWindowRep::SetBgColor(Color bg)
{
  WindowRep::SetBgColor(bg);
#ifdef GRAPHICS
    /* do something */
#endif
}

void PSWindowRep::SetWindowBgColor(Color bg)
{
#ifdef GRAPHICS
  /* do something */
#endif
}

/* drawing primitives */

void PSWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("PSWindowRep::DrawPixel: %.2f %.2f --> %.2f %.2f\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

static struct XPoint points[WINDOWREP_BATCH_SIZE];
static XRectangle rectAngles[WINDOWREP_BATCH_SIZE];

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
    for(int i = 0; i < num; i++) {
      Coord tx, ty;
      Transform(x[i], y[i], tx, ty);
      points[i].x = ROUND(short, tx);
      points[i].y = ROUND(short, ty);
    }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
    printf("\nAfter transformation:\n\n");
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
	printf("\n");
      printf("(%d,%d)",points[k].x, points[k].y);
    }
    printf("\n");
#endif
#endif

#ifdef GRAPHICS
    /* do something */
#endif

    return;
  }

  /* each "pixel" is wider than one screen pixel */

  int halfWidth = width/2;
  for(int i = 0; i < num; i++) {
    Coord tx,ty;
    Transform(x[i],y[i],tx,ty);
    rectAngles[i].x = ROUND(short, tx - halfWidth);
    rectAngles[i].y = ROUND(short, ty - halfWidth);
    rectAngles[i].width = width;
    rectAngles[i].height = width;
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation 2:\n\n");
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", rectAngles[k].x, rectAngles[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

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
    
    rectAngles[i].x = ROUND(short, txlow);
    rectAngles[i].y = ROUND(short, tylow);
    rectAngles[i].width = ROUND(unsigned short, pixelWidth);
    rectAngles[i].height = ROUND(unsigned short, pixelHeight);
  }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 6 == 0)
      printf("\n");
    printf("(%d,%d,%d,%d)", rectAngles[k].x, rectAngles[k].y,
	   rectAngles[k].width, rectAngles[k].height);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  for(int pointNum = 0; pointNum < num; pointNum++) {
    Coord x1 = (Coord) rectAngles[pointNum].x;
    Coord y1 = (Coord) rectAngles[pointNum].y;
    Coord x2 = x1 + (Coord) rectAngles[pointNum].width;
    Coord y2 = y1 + (Coord) rectAngles[pointNum].height;
    DrawFilledRect(printFile, x1, y1, x2, y2);
  }
#endif
}

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
    
    rectAngles[i].x = ROUND(short, txlow);
    rectAngles[i].y = ROUND(short, tylow);
    rectAngles[i].width = ROUND(unsigned short, pixelWidth);
    rectAngles[i].height = ROUND(unsigned short, pixelHeight);
  }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation: width %d, height %d\n\n",
	 rectAngles[0].width, rectAngles[0].height);
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", rectAngles[k].x, rectAngles[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  for(int pointNum = 0; pointNum < num; pointNum++) {
    Coord x1 = (Coord) rectAngles[pointNum].x;
    Coord y1 = (Coord) rectAngles[pointNum].y;
    Coord x2 = x1 + (Coord) rectAngles[pointNum].width;
    Coord y2 = y1 + (Coord) rectAngles[pointNum].height;
    DrawFilledRect(printFile, x1, y1, x2, y2);
  }
#endif
}

void PSWindowRep::FillRect(Coord xlow, Coord ylow, Coord width, Coord height)
{
#ifdef DEBUG
  printf("PSWindowRep::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         xlow, ylow, width, height);
#endif

  //TEMPTEMP -- temporary expedient to keep from drawing over the whole data
  // area.
  if (GetFgColor() == GetBgColor()) {
#if 0
    printf("Dropping out of PSWindowRep::FillRect() because foreground\n");
    printf("  is the same as background color\n");
#endif
    return;
  }

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
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  DrawFilledRect(printFile, txlow, tylow, txmax, tymax);
#endif
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void PSWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			       Coord minWidth, Coord minHeight)
{
#ifdef DEBUG
  printf("PSWindowRep::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         x, y, width, height);
#endif

  int pixelX, pixelY;
  unsigned pixelWidth = (unsigned)MAX(minWidth, width);
  unsigned pixelHeight = (unsigned)MAX(minHeight, height);
  pixelX = ROUND(int, x - pixelWidth / 2);
  pixelY = ROUND(int, y - pixelHeight / 2);

#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
	 pixelX, pixelY, pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void PSWindowRep::FillPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("PSWindowRep::FillPoly: %d points\n", n);

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[k].x, pts[k].y);
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

  for(int i = 0; i < n; i++) {
    Coord tx, ty;
    Transform(pts[i].x, pts[i].y, tx, ty);
    points[i].x = ROUND(short, tx);
    points[i].y = ROUND(short, ty);
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", points[k].x, points[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  /* do something */
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

  for(int i = 0;  i < n; i++ ) {
    points[i].x = ROUND(short, pts[i].x);
    points[i].y = ROUND(short, pts[i].y);
  }

#ifdef GRAPHICS
  /* do something */
#endif
}

/********************************************************************/

void PSWindowRep::Arc(Coord x, Coord y, Coord w, Coord h,
		     Coord startAngle, Coord endAngle)
{
#ifdef DEBUG
  printf("PSWindowRep::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
	 x, y, w, h, startAngle, endAngle);
#endif
  
  Coord tx, ty, tempX, tempY;
  Transform(x - w / 2, y - h / 2, tx, ty);
  Transform(x + w / 2, y + h / 2, tempX, tempY);
  int realWidth = ROUND(int, fabs(tempX - tx));
  int realHeight = ROUND(int, fabs(tempY - ty));
  int realStart= ROUND(int, ToDegree(startAngle) * 64);
  int realEnd = ROUND(int, ToDegree(endAngle) * 64);

#ifdef GRAPHICS
  /* do something */
#endif
}

/***********************************************************************/

void PSWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, 
		      Coord width)
{
#if defined(DEBUG) || 0
  printf("PSWindowRep::Line %.2f,%.2f,%.2f,%.2f\n", x1, y1, x2, y2);
#endif
  
  Coord tx1, ty1, tx2, ty2;
  Transform(x1 ,y1, tx1, ty1);
  Transform(x2, y2, tx2, ty2);

#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  DrawLine(printFile, tx1, ty1, tx2, ty2);
#endif
}

void PSWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
#ifdef DEBUG
  printf("PSWindowRep::AbsoluteLine %d,%d,%d,%d\n", x1, y1, x2, y2);
#endif
  
#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  Coord x1new;
  Coord y1new;
  Coord x2new;
  Coord y2new;
  TransPixToPoint(x1, y1, x1new, y1new);
  TransPixToPoint(x2, y2, x2new, y2new);

  DrawLine(printFile, x1new, y1new, x2new, y2new);
#endif
}

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

  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  Transform(x, y, tx1, ty1);
  Transform(x + width, y + height, tx2, ty2);
  int winX, winY, winWidth, winHeight;
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

#if 0 //TEMPTEMP
  if (textWidth > winWidth || textHeight > winHeight) {
    ScaledText(text, x, y, width, height, alignment, skipLeadingSpace);
    return;
  }
#endif

  char *comment;
  char *calculation;

  /* Here, instead of actually calculating the text position, we generate
   * the correct PostScript code to calculate the position. */
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

#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

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

  /* Set up the font.  For right now we're just using a fixed font, but
   * that should be changed eventually.  RKW 11/6/96. */
  fprintf(printFile, "/Times-Roman findfont\n");
  fprintf(printFile, "15 scalefont\n");
  fprintf(printFile, "setfont\n");

  /* Get the width and height of the given string in the given font. */
  fprintf(printFile, "(%s) stringwidth\n", text);
  fprintf(printFile, "/textHeight exch def\n");
  fprintf(printFile, "/textWidth exch def\n");

  /* Note: we're forcing textHeight to be 15 because stringwidth seems
   * to return zero for the height. RKW 11/6/96. */
  fprintf(printFile, "/textHeight 15 def\n");

  fprintf(printFile, "/winX %d def\n", winX);
  fprintf(printFile, "/winY %d def\n", winY);
  fprintf(printFile, "/winWidth %d def\n", winWidth);
  fprintf(printFile, "/winHeight %d def\n", winHeight);

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

  /* Move to the actual location and print the text. */
  fprintf(printFile, "textX textY moveto\n");
  fprintf(printFile, "(%s) show\n", text);
#endif
}

/* Draw scale text */

void PSWindowRep::ScaledText(char *text, Coord x, Coord y, Coord width,
		       Coord height, TextAlignment alignment,
		       Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("PSWindowRep::Text: %s at %.2f,%.2f,%.2f,%.2f\n",
	 text, x, y, width, height);
#endif

  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  Transform(x, y, tx1, ty1);
  Transform(x + width, y + height, tx2, ty2);
  int winX, winY, winWidth, winHeight; /* box in window coord */
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
  if (!textLength)
    return;

#ifdef GRAPHICS
  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  FILE * printFile = psDispP->GetPrintFile();

  /* Note: this is a temporary expedient only -- does not scale or
   * properly position the text. RKW 11/6/96. */
  fprintf(printFile, "/Times-Roman findfont\n");
  fprintf(printFile, "15 scalefont\n");
  fprintf(printFile, "setfont\n");
  fprintf(printFile, "%f %f moveto\n", tx1, ty1);
  fprintf(printFile, "(%s) show\n", text);
#endif
}

void PSWindowRep::SetXorMode()
{
#ifdef DEBUG
  printf("PSWindowRep::SetXorMode\n");
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

void PSWindowRep::SetCopyMode()
{
#ifdef DEBUG
  printf("PSWindowRep::SetCopyMode\n");
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

void PSWindowRep::SetNormalFont()
{
    /* do something */
}

void PSWindowRep::SetSmallFont()
{
    /* do something */
}

int PSWindowRep::GetSmallFontHeight()
{
    /* do something */
    return 1;
}

void PSWindowRep::UpdateWinDimensions()
{
  /* do something */
}

/* Get window rep dimensions */

void PSWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

/* Get window rep origin */

void PSWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

void PSWindowRep::AbsoluteOrigin(int &x, int &y)
{
  /* do something */
}

/* Set up the "pixel" to point transform according to the size and location
 * of the screen window, and the size of the page to output. */

void PSWindowRep::SetPPTrans(const Rectangle &viewGeom,
  const Rectangle &parentGeom, Boolean maintainAspect)
{
#if defined(DEBUG)
  printf("PSWindowRep::SetPPTrans()\n");
  printf("  %f %f %f %f\n", viewGeom.x, viewGeom.y, viewGeom.width,
    viewGeom.height);
  printf("  %f %f %f %f\n", parentGeom.x, parentGeom.y, parentGeom.width,
    parentGeom.height);
#endif

  /* Get geometry of output page here. */
  Coord pageWidth;
  Coord pageHeight;
  Coord xMargin;
  Coord yMargin;

  // Note: get rid of cast -- not safe.  RKW 9/19/96.
  PSDisplay *psDispP = (PSDisplay *) DeviseDisplay::GetPSDisplay();
  psDispP->GetPageGeom(pageWidth, pageHeight, xMargin, yMargin);

  pageWidth -= 2 * xMargin;
  pageHeight -= 2 * yMargin;

  if (maintainAspect) {
    double screenAspect = parentGeom.height / parentGeom.width;
    double pageAspect = pageHeight / pageWidth;

    if (screenAspect > pageAspect) {
      pageWidth = pageHeight / screenAspect;
    } else {
      pageHeight = pageWidth * screenAspect;
    }
  }

  _pixToPointTrans.MakeIdentity();

  Coord xScale = pageWidth / parentGeom.width;
  Coord yScale = -1.0 * pageHeight / parentGeom.height;
  _pixToPointTrans.Scale(xScale, yScale);
  Coord widthAdj = viewGeom.x / parentGeom.width * pageWidth;
  Coord heightAdj = viewGeom.y / parentGeom.height * pageHeight;
  _pixToPointTrans.Translate(widthAdj + xMargin,
    pageHeight - heightAdj + yMargin);
}

/* Move and resize window, relative to the parent */

void PSWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("Moving PSWindowRep 0x%p to %d,%d, size %u,%u\n", this,
	 x, y, w, h);
#endif

  /* do something */

  UpdateWinDimensions();
}

/* Iconify window. Not guaranteed to succeed. */

void PSWindowRep::Iconify()
{
    /* do something */
}

/* Raise window to top of stacking order */

void PSWindowRep::Raise()
{
#ifdef DEBUG
  printf("PSWindowRep::Raise window %p\n", this);
#endif

  /* do something */
}

/* Lower window to bottom of stacking order */

void PSWindowRep::Lower()
{
#ifdef DEBUG
  printf("PSWindowRep::Lower window %p:\n", this);
#endif

  /* do something */
}

/* Draw a line (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::DrawLine(FILE *printFile, Coord x1, Coord y1,
    Coord x2, Coord y2)
{
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1, y1);
  fprintf(printFile, "%f %f lineto\n", x2, y2);
  fprintf(printFile, "stroke\n");

  return;
}

/* Draw a filled rectangle (coordinates must have already been scaled if
 * that is necessary). */
void PSWindowRep::DrawFilledRect(FILE *printFile, Coord x1, Coord y1,
    Coord x2, Coord y2)
{
  fprintf(printFile, "newpath\n");
  fprintf(printFile, "%f %f moveto\n", x1, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y2);
  fprintf(printFile, "%f %f lineto\n", x2, y1);
  fprintf(printFile, "%f %f lineto\n", x1, y1);
  fprintf(printFile, "closepath\n");
  fprintf(printFile, "fill\n");

  return;
}
