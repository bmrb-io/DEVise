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

  $Log$*/

#include "NullWindowRep.h"
#include "NullDisplay.h"
#ifndef LIBCS
#include "Init.h"
#endif

//#define DEBUG
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
Initializer
***********************************************************************/

NullWindowRep:: NullWindowRep(DeviseDisplay *display,
                              Color fgndColor, Color bgndColor,
                              NullWindowRep *parent, int x, int y) :
	WindowRep(display, fgndColor, bgndColor)
{
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);

  _x = x;
  _y = y;

  Init();
}

void NullWindowRep::Init()
{
  UpdateWinDimensions();
}

/**************************************************************
  destructor 
**************************************************************/

NullWindowRep::~NullWindowRep()
{
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

void NullWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
    /* do something */
}

/******************************************************************/

void NullWindowRep::PushClip(Coord x, Coord y, Coord w, Coord h)
{
#ifdef DEBUG
  printf("NullWindowRep::PushClip(%.2f,%.2f,%.2f,%.2f)\n", x, y, w, h);
#endif

  Coord xlow, ylow, xhi, yhi, width, height;
  Coord x1, y1, x2, y2;
  WindowRep::Transform(x, y, x1, y1);
  WindowRep::Transform(x + w, y + h, x2, y2);
  xlow = MIN(x1, x2);
  xhi = MAX(x1, x2);
  ylow = MIN(y1, y2);
  yhi = MAX(y1, y2);
  width = xhi - xlow + 1;
  height = yhi - ylow + 1;
  
#ifdef DEBUG
  printf("NullWindowRep::PushClip: transformed into (%.2f,%.2f,%.2f,%.2f)\n",
	 xlow, ylow, width, height);
#endif

#ifdef GRAPHICS
  /* do something */
#endif

  WindowRep::_PushClip(xlow, ylow, width, height);
}

/*******************************************************************/

void NullWindowRep::PopClip()
{
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
}

/* import window image */

void NullWindowRep::ImportImage(Coord x, Coord y,
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

void NullWindowRep::ExportImage(DisplayExportFormat format, char *filename)
{
    /* do something */
}

/* drawing primitives */

void NullWindowRep::SetWindowBgColor(Color bg)
{
#ifdef GRAPHICS
  /* do something */
#endif
}

void NullWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  WindowRep::Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("NullWindowRep::DrawPixel: %.2f %.2f --> %.2f %.2f\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

static struct XPoint points[WINDOWREP_BATCH_SIZE];
static XRectangle rectAngles[WINDOWREP_BATCH_SIZE];

void NullWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("NullWindowRep::DrawPixelArray: %d points, width = %d\n", num, width);

  printf("Transformation matrix: ");
  WindowRep::PrintTransform();
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
      WindowRep::Transform(x[i], y[i], tx, ty);
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
    WindowRep::Transform(x[i],y[i],tx,ty);
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

void NullWindowRep::FillRectArray(Coord *xlow, Coord *ylow,
                                  Coord *width, Coord *height,
                                  int num)
{
#ifdef DEBUG
  printf("NullWindowRep::FillRectArray: %d points\n", num);

  printf("Transformation matrix: ");
  WindowRep::PrintTransform();
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
    WindowRep::Transform(xlow[i], ylow[i] + height[i], x1, y1);
    WindowRep::Transform(xlow[i] + width[i], ylow[i], x2, y2);
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
  /* do something */
#endif
}

/* Fill rectangles, same width/height */

void NullWindowRep::FillRectArray(Coord *xlow, Coord *ylow,
                                  Coord width, Coord height,
                                  int num)
{
#ifdef DEBUG
  printf("NullWindowRep::FillRectArray: %d points, width %.2f, height %.2f\n",
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
    WindowRep::Transform(xlow[i], ylow[i] + height, x1, y1);
    WindowRep::Transform(xlow[i] + width, ylow[i], x2, y2);
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
  /* do something */
#endif
}

void NullWindowRep::FillRect(Coord xlow, Coord ylow,
                             Coord width, Coord height)
{
#ifdef DEBUG
  printf("NullWindowRep::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
         xlow, ylow, width, height);
#endif

  /* XXX: need to clip rect against window dimensions */

  Coord txlow, tylow, txmax, tymax;
  Coord x1, y1, x2, y2;
  WindowRep::Transform(xlow, ylow + height, x1, y1);
  WindowRep::Transform(xlow + width, ylow, x2, y2);
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
  /* do something */
#endif
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void NullWindowRep::FillPixelRect(Coord x, Coord y,
                                  Coord width, Coord height,
                                  Coord minWidth, Coord minHeight)
{
#ifdef DEBUG
  printf("NullWindowRep::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
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

void NullWindowRep::FillPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("NullWindowRep::FillPoly: %d points\n", n);

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

void NullWindowRep::FillPixelPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("NullWindowRep::FillPixelPoly: %d points\n",n);

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

void NullWindowRep::Arc(Coord x, Coord y, Coord w, Coord h,
                        Coord startAngle, Coord endAngle)
{
#ifdef DEBUG
  printf("NullWindowRep::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
	 x, y, w, h, startAngle, endAngle);
#endif
  
  Coord tx, ty, tempX, tempY;
  WindowRep::Transform(x - w / 2, y - h / 2, tx, ty);
  WindowRep::Transform(x + w / 2, y + h / 2, tempX, tempY);
  int realWidth = ROUND(int, fabs(tempX - tx));
  int realHeight = ROUND(int, fabs(tempY - ty));
  int realStart= ROUND(int, ToDegree(startAngle) * 64);
  int realEnd = ROUND(int, ToDegree(endAngle) * 64);

#ifdef GRAPHICS
  /* do something */
#endif
}

/***********************************************************************/

void NullWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width)
{
#ifdef DEBUG
  printf("NullWindowRep::Line %.2f,%.2f,%.2f,%.2f\n", x1, y1, x2, y2);
#endif
  
  Coord tx1, ty1, tx2, ty2;
  WindowRep::Transform(x1 ,y1, tx1, ty1);
  WindowRep::Transform(x2, y2, tx2, ty2);

#ifdef GRAPHICS
  /* do something */
#endif
}

void NullWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
#ifdef DEBUG
  printf("NullWindowRep::AbsoluteLine %d,%d,%d,%d\n", x1, y1, x2, y2);
#endif
  
#ifdef GRAPHICS
  /* do something */
#endif
}

/* Draw absolute text */

void NullWindowRep::AbsoluteText(char *text, Coord x, Coord y,
                                 Coord width, Coord height,
                                 TextAlignment alignment, 
                                 Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("NullWindowRep::AbsoluteText: %s at %.2f,%.2f,%.2f,%.2f\n",
	 text, x, y, width, height);
#endif

  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  WindowRep::Transform(x, y, tx1, ty1);
  WindowRep::Transform(x + width, y + height, tx2, ty2);
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
  if (!textLength)
    return;
  
#ifdef GRAPHICS
  /* do something */
#endif
}

/* Draw scale text */

void NullWindowRep::Text(char *text, Coord x, Coord y,
                         Coord width, Coord height,
                         TextAlignment alignment,
                         Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("NullWindowRep::Text: %s at %.2f,%.2f,%.2f,%.2f\n",
	 text, x, y, width, height);
#endif

  /* transform into window coords */
  Coord tx1, ty1, tx2, ty2;
  WindowRep::Transform(x, y, tx1, ty1);
  WindowRep::Transform(x + width, y + height, tx2, ty2);
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
  /* do something */
#endif
}

void NullWindowRep::SetXorMode()
{
#ifdef DEBUG
  printf("NullWindowRep::SetXorMode\n");
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

void NullWindowRep::SetCopyMode()
{
#ifdef DEBUG
  printf("NullWindowRep::SetCopyMode\n");
#endif

#ifdef GRAPHICS
  /* do something */
#endif
}

void NullWindowRep::SetNormalFont()
{
    /* do something */
}

void NullWindowRep::SetSmallFont()
{
    /* do something */
}

int NullWindowRep::GetSmallFontHeight()
{
    /* do something */
    return 1;
}

void NullWindowRep::UpdateWinDimensions()
{
  /* do something */
}

/* Get window rep dimensions */

void NullWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

/* Get window rep origin */

void NullWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

void NullWindowRep::AbsoluteOrigin(int &x, int &y)
{
  /* do something */
}

/* Move and resize window, relative to the parent */

void NullWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("Moving NullWindowRep 0x%p to %d,%d, size %u,%u\n", this,
	 x, y, w, h);
#endif

  /* do something */

  UpdateWinDimensions();
}

/* Iconify window. Not guaranteed to succeed. */

void NullWindowRep::Iconify()
{
    /* do something */
}

/* Raise window to top of stacking order */

void NullWindowRep::Raise()
{
#ifdef DEBUG
  printf("NullWindowRep::Raise window 0x%lx:\n", _win);
#endif

  /* do something */
}

/* Lower window to bottom of stacking order */

void NullWindowRep::Lower()
{
#ifdef DEBUG
  printf("NullWindowRep::Lower window 0x%lx:\n", _win);
#endif

  /* do something */
}
