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
  Revision 1.2  1995/09/05 21:13:41  jussi
  Added/updated CVS header.
*/

#include "Init.h"
#include "XWindowRep.h"
#include "XDisplay.h"
#include "Compress.h"

//#define DEBUG
#define MAXPIXELDUMP 200

#define ROUND(type, value) ((type)(value + 0.5))

/**********************************************************************
Initializer
***********************************************************************/

XWindowRep:: XWindowRep(Display *display,Window window, XDisplay * DVDisp,
			Color fgndColor, Color bgndColor,
			Boolean backingStore) :
	WindowRep(DVDisp, fgndColor, bgndColor)
{
  _dispGraphics = Init::DispGraphics();
  _compress = new SimpleCompress();

  _backingStore = backingStore;
  _unobscured = false;

  _display = display,
  _win = window;
  UpdateWinDimensions();
  _gc = XCreateGC(_display, window, 0, NULL);
  _rectGc = XCreateGC(_display, window, 0, NULL); /* for rubberbanding*/
  
  XSetState(_display,_gc, DVDisp->GetLocalColor(fgndColor),
	    DVDisp->GetLocalColor(bgndColor),GXcopy,AllPlanes);
  
  XSetState(_display,_rectGc,AllPlanes, AllPlanes, GXxor,AllPlanes);
  
  /* init temp storage for storing points */
  _xpoints= new XPoint[20];
  _num_points = 20;
  
  /* set default pattern to pattern 0: no stipple */
  WindowRep::SetPattern(Pattern0);
  XSetFillStyle(_display,_gc,FillSolid);
  
  /* load font */
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  XSetFont(_display,_gc, fontStruct->fid);
  
  /* create pixmaps for character manipulation */
  _srcBitmap.inUse = false;
  _dstBitmap.inUse = false;
  int bitmapWidth = 80*(fontStruct->max_bounds.rbearing-
			fontStruct->min_bounds.lbearing);
  int bitmapHeight =  fontStruct->max_bounds.ascent+
    fontStruct->max_bounds.descent;
  AllocBitmap(_srcBitmap, bitmapWidth,bitmapHeight);
  AllocBitmap(_dstBitmap,3*bitmapWidth, 3*bitmapHeight);
}

/**************************************************************
  destructor 
**************************************************************/

XWindowRep::~XWindowRep()
{
  XFreeGC(_display,_gc);
  XFreeGC(_display,_rectGc);
  
  FreeBitmap(_srcBitmap);
  FreeBitmap(_dstBitmap);
  delete _xpoints;
  
  /* This should be called by the display. 
     XDestroyWindow(_display,_win);
  */
}

/******************************************************************/

virtual void XWindowRep::PushClip(Coord x,Coord y,Coord w,Coord h)
{
#ifdef DEBUG
  printf("XWindowRep::PushClip(%.2f,%.2f,%.2f,%.2f)\n",x,y,w,h);
#endif

  Coord xlow,ylow,xhi,yhi,width,height;
  Coord x1, y1, x2,y2;
  WindowRep::Transform(x,y, x1, y1);
  WindowRep::Transform(x+w,y+h, x2,y2);
  xlow = MinMax::min(x1,x2);
  xhi = MinMax::max(x1,x2);
  ylow = MinMax::min(y1,y2);
  yhi = MinMax::max(y1,y2);
  width = xhi-xlow; height = yhi-ylow;
  
#ifdef DEBUG
  printf("XwindowRep::PushClip: transformed into (%.2f,%.2f,%.2f,%.2f)\n",
	 xlow,ylow,width,height);
#endif

  XRectangle rect ;
  rect.x = ROUND(short, xlow);
  rect.y= ROUND(short, ylow);
  rect.width = (unsigned short)ceil(width);
  rect.height = (unsigned short)ceil(height);

#ifdef GRAPHICS
  if (_dispGraphics)
    XSetClipRectangles(_display,_gc,0, 0, &rect,1,Unsorted);
#endif

  WindowRep::_PushClip(xlow,ylow,width,height);
}

/*******************************************************************/

virtual void XWindowRep::PopClip()
{
  WindowRep::_PopClip();
  Coord x,y,w,h;
  XRectangle rect ; 
  if (WindowRep::ClipTop(x,y,w,h)){
    rect.x = ROUND(short, x);
    rect.y= ROUND(short, y);
    rect.width = (unsigned short)ceil(w);
    rect.height = (unsigned short)ceil(h);
#ifdef GRAPHICS
    if (_dispGraphics)
      XSetClipRectangles(_display,_gc,0, 0, &rect,1,Unsorted);
#endif

  } else {

    /* no more clipping */
#ifdef GRAPHICS 
    if (_dispGraphics)
      XSetClipMask(_display,_gc,None);
#endif
  }
}

/* drawing primitives */
virtual void XWindowRep::SetFgColor(Color fg)
{
  WindowRep::SetFgColor(fg);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetForeground(_display,_gc, WindowRep::GetLocalColor(fg));
#endif
}

virtual void XWindowRep::SetBgColor(Color bg)
{
  WindowRep::SetBgColor(bg);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetBackground(_display,_gc, WindowRep::GetLocalColor(bg));
#endif
}

virtual void XWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  WindowRep::Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("DrawPixel: %.2f %.2f --> %d %d\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XDrawPoint(_display,_win, _gc, (int)tx, (int)ty);
#endif
}

static struct XPoint points[WINDOWREP_BATCH_SIZE];
static XRectangle rectAngles[WINDOWREP_BATCH_SIZE];

void XWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("DrawPixelArray: %d points, width = %d\n", num, width);

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
    if (_dispGraphics)
      XDrawPoints(_display,_win, _gc, points, num, CoordModeOrigin);
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
  if (_dispGraphics)
    XFillRectangles(_display,_win, _gc, rectAngles, num);
#endif
}

/* Fill rectangles, variable width/height */

virtual void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
				       Coord *height, int num)
{
#ifdef DEBUG
  printf("FillRectArray: %d points\n", num);

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

  int index = 0;
  for(int i = 0; i < num; i++) {
    Coord x1, y1, x2, y2;
    Coord txlow, tylow, txmax, tymax;
    WindowRep::Transform(xlow[i], ylow[i] + height[i], x1, y1);
    WindowRep::Transform(xlow[i] + width[i], ylow[i], x2, y2);
    txlow = MinMax::min(x1, x2);
    txmax = MinMax::max(x1, x2);
    tylow = MinMax::min(y1, y2);
    tymax = MinMax::max(y1, y2);
    
    /* make it fit inside real window boundaries.
       We assume window resolution can't be 10000x10000 */
    if (txmax < 0.0 || txlow >= 10000.0 ||
	tymax < 0.0 || tylow >= 10000.0)
      continue;
    
    txlow = MinMax::max(txlow, -10000.0);
    txmax = MinMax::min(txmax, 10000.0);
    tylow = MinMax::max(tylow, -10000.0);
    tymax = MinMax::min(tymax, 10000.0);
    
    unsigned pixelWidth = (unsigned)(txmax - txlow + 1);
    if (pixelWidth == 0) pixelWidth = 1;
    unsigned pixelHeight = unsigned(tymax - tylow + 1);
    if (pixelHeight == 0) pixelHeight = 1;
    
    rectAngles[index].x = ROUND(short, txlow);
    rectAngles[index].y = ROUND(short, tylow);
    rectAngles[index].width = ROUND(unsigned short, pixelWidth);
    rectAngles[index].height = ROUND(unsigned short, pixelHeight);
    
    if (xlow[i] == 67.5) {
      printf("x %.2f -> x1 %.2f, x2 %2.f, txlow %2.f, txmax %2.f, r.x %d\n",
	     xlow[i], x1, x2, txlow, txmax, rectAngles[index].x);
    }

    index++;
  }

#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (index > MAXPIXELDUMP ? MAXPIXELDUMP : index); k++) {
    if ((k + 1) % 6 == 0)
      printf("\n");
    printf("(%d,%d,%d,%d)", rectAngles[k].x, rectAngles[k].y,
	   rectAngles[k].width, rectAngles[k].height);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics && index > 0)
    XFillRectangles(_display, _win, _gc, rectAngles, index);
#endif
}

/* Fill rectangles, same width/height */

virtual void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord width,
				       Coord height, int num)
{
#ifdef DEBUG
  printf("FillRectArray: %d points, width %.2f, height %.2f\n", num,
	 width, height);

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
    txlow = MinMax::min(x1, x2);
    txmax= MinMax::max(x1, x2);
    tylow = MinMax::min(y1, y2);
    tymax = MinMax::max(y1, y2);

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
  printf("\nAfter transformation: width %.2f, height %.2f\n\n",
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
  if (_dispGraphics)
    XFillRectangles(_display, _win, _gc, rectAngles, num);
#endif
}

virtual void XWindowRep::FillRect(Coord xlow, Coord ylow, Coord width, 
				  Coord height)
{
#ifdef DEBUG
  printf("FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n", xlow, ylow,
	 width, height);
#endif

  /* XXX: need to clip rect against window dimensions */

  Coord txlow, tylow, txmax, tymax;
  Coord x1,y1,x2,y2;
  WindowRep::Transform(xlow,ylow+height,x1,y1);
  WindowRep::Transform(xlow+width,ylow,x2,y2);
  txlow = MinMax::min(x1,x2);
  txmax= MinMax::max(x1,x2);
  tylow = MinMax::min(y1,y2);
  tymax = MinMax::max(y1,y2);
  
  /* fill rectangle, remember that the window coordinate
     system starts at the upper left corner */

  unsigned pixelWidth = (unsigned)(txmax-txlow+1);
  if (pixelWidth == 0) pixelWidth = 1;
  unsigned pixelHeight = unsigned(tymax-tylow+1);
  if (pixelHeight == 0) pixelHeight=1;
  
#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
	 (int)txlow, (int)tylow, pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillRectangle(_display,_win, _gc, (int)txlow, (int)tylow,
		   pixelWidth, pixelHeight);
#endif
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void XWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			       Coord minWidth, Coord minHeight)
{
#ifdef DEBUG
  printf("FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n", x, y,
	 width, height);
#endif

  int pixelX, pixelY;
  unsigned pixelWidth = (unsigned)MinMax::max(minWidth, width);
  unsigned pixelHeight = (unsigned)MinMax::max(minHeight, height);
  pixelX = ROUND(int, x - pixelWidth / 2);
  pixelY = ROUND(int, y - pixelHeight / 2);

#ifdef DEBUG
  printf("After transformation: x %d, y %d, width %d, height %d\n",
	 pixelX, pixelY, pixelWidth, pixelHeight);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillRectangle(_display, _win, _gc, pixelX, pixelY, 
		   pixelWidth, pixelHeight);
#endif
}

/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void XWindowRep::FillPoly(Point *points, int n)
{
#ifdef DEBUG
  printf("XwindowRep::Fillpoly: %d points\n", n);

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", points[k].x, points[k].y);
  }
  printf("\n");
#endif
#endif

  if (n <= 0)
    return;
  if (_num_points < n){
    delete _xpoints;
    _xpoints = new XPoint[n];
    _num_points = n;
  }

  for(int i = 0; i < n; i++) {
    Coord tx,ty;
    Transform(points[i].x,points[i].y, tx,ty);
    _xpoints[i].x = ROUND(short, tx);
    _xpoints[i].y = ROUND(short, ty);
  }
  
#ifdef DEBUG
#if MAXPIXELDUMP > 0
  printf("\nAfter transformation:\n\n");
  for(k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", _xpoints[k].x, _xpoints[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillPolygon(_display,_win,_gc,_xpoints, n, Nonconvex, CoordModeOrigin);
#endif
}

/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void XWindowRep::FillPixelPoly(Point *points, int n)
{
#ifdef DEBUG
  printf("XwindowRep::FillPixelPoly: %d points\n",n);
  for(int j = 0; j < n; j++){
    printf("(%.2f,%.2f)", points[j].x, points[j].y);
  };
  printf("\n");
#endif

  if (n <= 0)
    return;
  if (_num_points < n){
    delete _xpoints;
    _xpoints = new XPoint[n];
    _num_points = n;
  }

#ifdef DEBUG
  printf("transformed into: ");
#endif
  for (int i=0;  i < n; i++ ){
    _xpoints[i].x = ROUND(short, points[i].x);
    _xpoints[i].y = ROUND(short, points[i].y);
#ifdef DEBUG
    printf("(%d,%d)", _xpoints[i].x, _xpoints[i].y);
#endif
  }
#ifdef DEBUG
  printf("\n");
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillPolygon(_display,_win,_gc,_xpoints, n, Nonconvex, CoordModeOrigin);
#endif
}

/********************************************************************/

virtual void XWindowRep::Arc(Coord x, Coord y, Coord w, Coord h,
			     Coord startAngle, Coord endAngle)
{
  Coord tx, ty, tempX,tempY;
  WindowRep::Transform(x-w/2,y+h/2,tx,ty);
  WindowRep::Transform(x+w/2,y-h/2, tempX,tempY);
  int realWidth = (int)fabs(tempX-tx);
  int realHeight = (int)fabs(tempY-ty);
  int realStart= ((int)ToDegree(startAngle))*64;
  int realEnd = ((int)ToDegree(endAngle))*64;
#ifdef GRAPHICS
  if (_dispGraphics){
    XSetLineAttributes(_display,_gc,0,LineSolid,CapButt,JoinRound);
    XDrawArc(_display,_win,_gc,(int)tx,(int)ty,realWidth,realHeight,
	     realStart, realEnd);
  }
#endif
}

/***********************************************************************/

virtual void XWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, 
			      Coord width)
{
  Coord tx1,ty1,tx2,ty2;
  WindowRep::Transform(x1,y1,tx1,ty1);
  WindowRep::Transform(x2,y2,tx2,ty2);
#ifdef GRAPHICS
  if (_dispGraphics){
    XSetLineAttributes(_display,_gc,(int)width,LineSolid,CapButt,JoinRound);
    XDrawLine(_display,_win,_gc,(int)tx1,(int)ty1,(int)tx2,(int)ty2);
  }
#endif
}

void XWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
  
#ifdef GRAPHICS
  if (_dispGraphics){
    XSetLineAttributes(_display,_gc,(int)width,LineSolid,CapButt,JoinRound);
    /*
       printf("AbsoluteLine %d,%d,%d,%d\n",x1,y1,x2,y2);
    */
    XDrawLine(_display,_win,_gc,x1,y1,x2,y2);
  }
#endif
}

/************************************************************
function supplied to XCheckIfEvent to find the next
Expose event for this window
***************************************************************/
static int check_expose(Display *, XEvent *e, char *arg)
{
  if (e->xany.type == Expose && e->xany.window == *((Window *)arg))
    return 1;
  else return 0;
}

/**************************************************************
Draw rubberband
****************************************************************/

void XWindowRep::DrawRubberband(int x1, int y1, int x2, int y2)
{
  int minX = MinMax::min(x1,x2);
  int minY = MinMax::min(y1,y2);
  int maxX = MinMax::max(x1,x2);
  int maxY = MinMax::max(y1,y2);
  unsigned width = maxX - minX;
  unsigned height = maxY - minY;
  
  XDrawRectangle(_display, _win, _rectGc, minX, minY, width, height);
}

static long buttonMasks[3]= {
  Button1MotionMask, Button2MotionMask,Button3MotionMask
};

/*********************************************************************
Handle button press event. Return the region covered by
the selection in window coordinates. 
***********************************************************************/

void XWindowRep::DoButtonPress(int x,int y, int &xlow, int &ylow, 
			       int &xhigh, int &yhigh, int button)
{
  /* grab server */
  XGrabServer(_display);
  int x1,x2;
  x1 = x2 = x;
  int y1,y2;
  y1 = y2 = y;

  /* draw rubberband rectangle */
  DrawRubberband(x1,y1,x2,y2);
  
  Boolean done = false;
  long buttonMask = buttonMasks[button-1];
  while(!done){
    XEvent event;
    XWindowEvent(_display,_win,ButtonReleaseMask|buttonMask,&event);
    switch(event.xany.type){
    case ButtonRelease:
      if (event.xbutton.button == button){
	/* final button position */
	DrawRubberband(x1,y1,x2,y2);
	
	x2 = event.xbutton.x;
	y2 = event.xbutton.y;
	done = true;
      }
      break;
    case MotionNotify:
      DrawRubberband(x1,y1,x2,y2);
      
      /* get rid of all remaining motion events */
      do {
	x2 = event.xmotion.x;
	y2 = event.xmotion.y;
      } while(XCheckWindowEvent(_display,_win,
				buttonMask,&event));
      
      DrawRubberband(x1,y1,x2,y2);
      break;
    }
  }
  
  xlow = MinMax::min(x1,x2);
  ylow = MinMax::min(y1,y2);
  xhigh = MinMax::max(x1,x2);
  yhigh = MinMax::max(y1,y2);
  
  if (xhigh-xlow <= 5 || yhigh-ylow <= 5 ){
    xhigh = xlow;
    yhigh = ylow;
  }
  
  /* ungrab server */
  XUngrabServer(_display);
  XFlush(_display);
}

/***********************************************************************
Handle the next X event
************************************************************************/

void XWindowRep::HandleEvent(XEvent &event)
{
  char buf[40];
  XEvent ev;
  
  switch(event.xany.type){
  case KeyPress:
    KeySym keysym;
    XComposeStatus compose;
    int count;
    count= XLookupString((XKeyEvent *)&event,buf,40,&keysym,&compose);
    if (count ==1){
      /* regular key */
      WindowRep::HandleKey(buf[0],event.xkey.x,event.xkey.y);
    }
    break;

  case ButtonPress:
    int buttonXlow, buttonYlow,buttonXhigh,buttonYhigh;
    if (event.xbutton.button == 2){
      /* hand popup */
      DoPopup(event.xbutton.x,event.xbutton.y, event.xbutton.button);
    }
    else if (event.xbutton.button <= 3){
      DoButtonPress(event.xbutton.x, event.xbutton.y,
		    buttonXlow, buttonYlow,buttonXhigh, buttonYhigh,
		    event.xbutton.button);
      
      /*
	 if (buttonXhigh > buttonXlow && buttonYhigh > buttonYlow){
	 */
      WindowRep::HandleButtonPress(buttonXlow, buttonYlow,
				   buttonXhigh, buttonYhigh, event.xbutton.button);
    }
    break;

  case Expose:
    Coord minX, minY, maxX, maxY;
    minX= (Coord)event.xexpose.x;
    minY = (Coord)event.xexpose.y;
    maxX= minX+ (Coord)event.xexpose.width-1;
    maxY= minY+ (Coord)event.xexpose.height-1;
    while (XCheckIfEvent(_display,&ev,check_expose,
			 (char *)&this->_win)){
      /* merge next expose event with current one to form
	 a bigger expose region */
      Geom::MergeRects(minX,minY, maxX, maxY,
		       (Coord)ev.xexpose.x, (Coord)ev.xexpose.y,
		       (Coord)(ev.xexpose.x+ev.xexpose.width),
		       (Coord)(ev.xexpose.y+ev.xexpose.height));
    }
    WindowRep::HandleExpose((int)minX,
			    (int)minY, (unsigned)(maxX-minX+1), 
			    (unsigned)(maxY-minY+1));
    break;

  case ConfigureNotify:
    /* resize event */
    int saveX, saveY; 
    unsigned int saveWidth, saveHeight;
    saveX = _x;
    saveY = _y;
    saveWidth = _width;
    saveHeight = _height;
    UpdateWinDimensions();
    if (_x != saveX || _y != saveY || _width != saveWidth
	|| _height != saveHeight)
      /* There is a real change in szie */
      WindowRep::HandleResize(_x,_y,_width,_height);
    break;

  case VisibilityNotify:
    if (event.xvisibility.state == VisibilityUnobscured){
      _unobscured = true;
#ifdef DEBUG
      printf("0x%x unobscured %d\n", this, _unobscured);
#endif
    }
    else if (event.xvisibility.state == VisibilityPartiallyObscured){
      _unobscured = false;
#ifdef DEBUG
      printf("0x%x partially obscured\n",this);
#endif
    }
    else {
      _unobscured = false;
#ifdef DEBUG
      printf("0x%x obscured\n",this);
#endif
    }
    break;

  case MapNotify:
#ifdef DEBUG
    printf("XWin 0x%x mapped\n",event.xmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(true);
    break;

  case UnmapNotify:
#ifdef DEBUG
    printf("XWin 0x%x unmapped\n",event.xunmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(false);
    break;
  }
}

/* Draw absolute text */

void XWindowRep::AbsoluteText(char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      TextAlignment alignment, 
			      Boolean skipLeadingSpace)
{
  /* transforminto window coords */
  Coord tx1,ty1,tx2,ty2;
  WindowRep::Transform(x,y,tx1,ty1);
  WindowRep::Transform(x+width,y+height,tx2,ty2);
  int winX,winY, winWidth, winHeight; /* box in window coord */
  winX = (int)(MinMax::min(tx1,tx2));
  winY = (int)(MinMax::min(ty1,ty2));
  winWidth = (int)(MinMax::max(tx1,tx2))-winX+1;
  winHeight = (int)(MinMax::max(ty1,ty2))-winY+1;
  
  int textLength = strlen(text);
  if (textLength == 0) return;
  
  if (skipLeadingSpace) {
    /* skip leading spaces before drawing text */
    while (*text == ' '){ 
      text++; textLength--; 
    }
    if (*text== '\0') return;
  }
  
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = 
    fontStruct->max_bounds.ascent + fontStruct->max_bounds.descent;
  
  if (textWidth > winWidth || textHeight > winHeight){
    Text(text, x, y, width, height, alignment, skipLeadingSpace);
    return;
  }
  
  int startX, startY;
  int widthDiff = winWidth - textWidth;
  int halfWidthDiff = widthDiff/2;
  int heightDiff = winHeight - textHeight;
  int halfHeightDiff = heightDiff/2;

  switch(alignment){
  case AlignNorthWest:
    startX = winX; 
    startY = winY + fontStruct->max_bounds.ascent;
    break;

  case AlignNorth:
    startX = winX + halfWidthDiff; 
    startY = winY+ fontStruct->max_bounds.ascent;
    break;

  case AlignNorthEast:
    startX = winX + widthDiff; 
    startY = winY+ fontStruct->max_bounds.ascent;
    break;

  case AlignWest: 
    startX = winX; 
    startY = winY + halfHeightDiff+ fontStruct->max_bounds.ascent;
    break;

  case AlignCenter: 
    startX = winX + halfWidthDiff; 
    startY = winY + halfHeightDiff+ fontStruct->max_bounds.ascent;
    break;

  case AlignEast:
    startX = winX+ widthDiff; 
    startY = winY + halfHeightDiff+ fontStruct->max_bounds.ascent;
    break;

  case AlignSouthWest:
    startX = winX; 
    startY = winY+ heightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignSouth:
    startX = winX + halfWidthDiff; 
    startY = winY+heightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignSouthEast:
    startX = winX+ widthDiff; 
    startY = winY+heightDiff + fontStruct->max_bounds.ascent;
    break;
  }
  
  XDrawString(_display, _win, _gc, startX, startY, text, textLength);
}

/* Draw scale text */

void XWindowRep::Text(char *text, Coord x, Coord y, Coord width, Coord height,
		      TextAlignment alignment, Boolean skipLeadingSpace)
{
  /*
     printf("text: %s at %.2f,%.2f,%.2f,%.2f\n",text,x,y,width,height);
  */

  /* transform into window coords */
  Coord tx1,ty1,tx2,ty2;
  WindowRep::Transform(x,y,tx1,ty1);
  WindowRep::Transform(x+width,y+height,tx2,ty2);
  int winX,winY, winWidth, winHeight; /* box in window coord */
  winX = (int)(MinMax::min(tx1,tx2));
  winY = (int)(MinMax::min(ty1,ty2));
  winWidth = (int)(MinMax::max(tx1,tx2))-winX+1;
  winHeight = (int)(MinMax::max(ty1,ty2))-winY+1;
  int textLength = strlen(text);
  if (textLength == 0) return;

  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = 
    fontStruct->max_bounds.ascent + fontStruct->max_bounds.descent;
  
  if (textWidth > _srcBitmap.width || textHeight > _srcBitmap.height)
    /* allocate a bigger source pixmap */
    AllocBitmap(_srcBitmap, textWidth, textHeight);
  
  /* use original text to calculate scale factor 
     to scale text to fit in rectangle of 
     dimensions (winWidth, winHeight)*/
  double xscale = (double)winWidth/(double)textWidth;
  double yscale = (double)winHeight/(double)textHeight;
  double scale = MinMax::min(xscale, yscale);
  
  if (skipLeadingSpace){
    /* skip leading spaces before drawing text */
    while (*text == ' '){ 
      text++; textLength--; 
    }
    if (*text== '\0') return;
    textWidth = XTextWidth(fontStruct, text, textLength);
  }
  
  /*
     printf("transformed to x=%d,y=%d,w=%d,h=%d\n", winX,winY,winWidth,winHeight);
     printf("textwidth = %d, textHeight = %d\n",textWidth,textHeight);
  */

  /* draw text in source bitmap */
  XDrawImageString(_display,_srcBitmap.pixmap,_srcBitmap.gc, 
		   0, fontStruct->max_bounds.ascent, text,textLength);
  
  /* scale into dest bitmap */
  int dstWidth = (int)(textWidth * scale);
  int dstHeight = (int)(textHeight*scale);

  /*
     printf("scale= %.2f, textWidth = %d, textHeight = %d, dstw=%d, dstH = %d\n",
     scale,textWidth, textHeight,dstWidth, dstHeight);
     printf("dstwidth = %d, dstHeight = %d\n",dstWidth,dstHeight);
  */

  CopyBitmap(textWidth, textHeight, dstWidth, dstHeight);
  
  /* draw the text according to alignment */
  int startX, startY;
  int widthDiff = winWidth-dstWidth;
  int halfWidthDiff = widthDiff/2;
  int heightDiff = winHeight-dstHeight;
  int halfHeightDiff = heightDiff/2;

  switch(alignment) {
  case AlignNorthWest:
    startX = winX; startY = winY;
    break;

  case AlignNorth:
    startX = winX + halfWidthDiff; startY = winY ;
    break;
    
  case AlignNorthEast:
    startX = winX + widthDiff; startY = winY;
    break;

  case AlignWest: 
    startX = winX; startY = winY + halfHeightDiff;
    break;

  case AlignCenter: 
    startX = winX + halfWidthDiff; startY = winY + halfHeightDiff;
    break;

  case AlignEast:
    startX = winX+ widthDiff; startY = winY + halfHeightDiff;
    break;

  case AlignSouthWest:
    startX = winX; startY = winY+ heightDiff;
    break;

  case AlignSouth:
    startX = winX + halfWidthDiff; startY = winY+heightDiff;
    break;
    
  case AlignSouthEast:
    startX = winX+ widthDiff; startY = winY+heightDiff;
    break;
  }
  
  XCopyPlane(_display,_dstBitmap.pixmap, _win, _gc, 
	     0,0, dstWidth,dstHeight, startX, startY, 1);
}

/**********************************************************************
Scale and copy the rectangle (0,0,width,height) in _srcBitmap
into _dstBitmap (0,0,dstWidth, dstHeight). X windows coords are used.
***********************************************************************/

void XWindowRep::CopyBitmap(int width,int height,int dstWidth,int dstHeight)
{
  /*
     printf("CopyBitmap: %d,%d,%d,%d\n",width,height,dstWidth,dstHeight);
  */

  if (dstWidth > _dstBitmap.width || dstHeight > _dstBitmap.height)
    /* allocate a bigger pixmap */
    AllocBitmap(_dstBitmap,dstWidth,dstHeight);

  /* copy dst from src */
  int widthMinus1 = width -1;
  int heightMinus1 = height -1;
  int dstWidthMinus1;
  if (dstWidth > 1)
    dstWidthMinus1 = dstWidth-1;
  else dstWidthMinus1 = dstWidth;

  int dstHeightMinus1;
  if (dstHeight > 1)
    dstHeightMinus1 = dstHeight-1;
  else dstHeightMinus1 = dstHeight;
  
  XImage *srcImage = XGetImage(_display, _srcBitmap.pixmap, 
			       0,0,width, height, 1,XYPixmap);
  
  for (int y = 0; y < dstHeight; y++){
    for (int x = 0; x < dstWidth; x++){
      XPutPixel(_dstBitmap.image,x,y,
		XGetPixel(srcImage,
			  x*widthMinus1/dstWidthMinus1,
			  y*heightMinus1/dstHeightMinus1));
    }
  }
  XPutImage(_display,_dstBitmap.pixmap,_dstBitmap.gc, 
	    _dstBitmap.image, 0,0,0,0,(unsigned)dstWidth,(unsigned)dstHeight);
  XDestroyImage(srcImage);
}

/***************************************************************
Set current pattern
****************************************************************/

void XWindowRep::SetPattern(Pattern p)
{
  if (WindowRep::GetPattern() != p){
    WindowRep::SetPattern(p);
    if (p == Pattern0){
      /* pattern0 is solid */
#ifdef GRAPHICS
      if (_dispGraphics)
	XSetFillStyle(_display,_gc, FillSolid);
#endif
    }
    else {
#ifdef GRAPHICS
      if (_dispGraphics){
	XSetFillStyle(_display,_gc,FillOpaqueStippled);
	XSetStipple(_display,_gc,
	     ((XDisplay *)WindowRep::GetDisplay())->GetPatternStipple(p)) ;
      }
#endif
    }
  }
}

/***********************************************************************/

void XWindowRep::AllocBitmap(XBitmapInfo &info, int width, int height)
{
  FreeBitmap(info);
  
  info.inUse = true;
  info.width = width;
  info.height = height;
  info.pixmap = XCreatePixmap(_display,_win, width, height, 1);
  info.gc = XCreateGC(_display,info.pixmap, 0 , NULL);
  
  /* set foreground to 1 and background to 0 for later XCopyPlane()*/
  XSetState(_display,info.gc, 1, 0, GXcopy,AllPlanes);
  
  XSetFont(_display,info.gc,
	   ((XDisplay *)WindowRep::GetDisplay())->GetFontStruct()->fid);
  info.image = XGetImage(_display, info.pixmap,0,0,width, height, 1,XYPixmap);
}

/* Get Window dimensions */

void XWindowRep::UpdateWinDimensions()
{
  Window root;
  unsigned int border_width, depth;
  XGetGeometry(_display,_win,&root,&_x,&_y,&_width,&_height,
	       &border_width, &depth);
}

/* Get window rep dimensions */

void XWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

/* get window rep origin */

void XWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

void XWindowRep::AbsoluteOrigin(int &x, int &y)
{
  /* Find the offset from root window */
  x = 0;
  y = 0;
  
  Window current, root;
  Window parent = _win;
  do {
    current = parent;
    
    /* add distace to parent */
    Window winRoot;
    int winX, winY; unsigned winW, winH, winBorder, winDepth;
    XGetGeometry(_display,current, &winRoot, &winX, &winY,
		 &winW, &winH, &winBorder, &winDepth);
    x += winX;
    y += winY;
    
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(_display,current,
		    &root,&parent,&children,&nchildren)){
      fprintf(stderr,"XwindowRep::HandleEvent QueryTree\n");
      Exit::DoExit(1);
		}
    if (children != NULL)
      XFree(children);
  } while (parent != root);
}

/* Move and resize window, relative to the parent */

void XWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
  /* Tell X to move/resize window. We will be notify by an event
     when it's done */
  XMoveResizeWindow(_display,_win, x, y, w, h);
}

/* Iconify window. Not guaranteed to succeed. */

void XWindowRep::Iconify()
{
  XIconifyWindow(_display, _win, 0);
}

void XWindowRep::DoPopup(int x, int y, int button)
{
  char **msgs;
  int numMsgs;
  if (!HandlePopUp(x,y,button, msgs,numMsgs) || numMsgs <= 0)
    /* no message for pop-up */
    return;
  
  /* find width and height of window */
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  int textWidth = 0;
  int textHeight = 0;
  int charAscent = fontStruct->max_bounds.ascent;
  int charHeight = fontStruct->max_bounds.ascent + 
    fontStruct->max_bounds.descent;

  for(int i = 0; i < numMsgs; i++) {
    textWidth = MinMax::max(textWidth,
			    XTextWidth(fontStruct, msgs[i], strlen(msgs[i])));
  }
  textHeight = charHeight*numMsgs;
  
  long bgnd = GetLocalColor(WhiteColor);
  long fgnd = GetLocalColor(BlackColor);
  /* Create window */
  XSetWindowAttributes    attr;
  attr.background_pixmap = None;
  attr.background_pixel   = bgnd;
  attr.border_pixmap          = CopyFromParent;
  attr.border_pixel       = fgnd;
  attr.bit_gravity        = ForgetGravity /*CenterGravity*/;
  attr.win_gravity        = NorthWestGravity;
  attr.backing_store          = /* Always*/ NotUseful ;
  attr.backing_planes         = AllPlanes;
  attr.backing_pixel          = 0;
  attr.save_under             = True;
  attr.event_mask             = 0;
  attr.do_not_propagate_mask= 0;
  attr.override_redirect      = True;
  attr.colormap   = DefaultColormap(_display, DefaultScreen(_display));
  attr.cursor                 = None;
  
  /* Create the window. */
  Window win;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  Window root, current;
  Window parent = _win;
  
  /* Find the offset from root window */
  do {
    current = parent;
    
    /* add distace to parent */
    Window winRoot;
    int winX, winY; unsigned winW, winH, winBorder, winDepth;
    XGetGeometry(_display,current, &winRoot, &winX, &winY,
		 &winW, &winH, &winBorder, &winDepth);
    x += winX;
    y += winY;
    
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(_display,current,
		    &root,&parent,&children,&nchildren)){
      fprintf(stderr,"XwindowRep::HandleEvent QueryTree\n");
      Exit::DoExit(1);
    }
    if (children != NULL)
      XFree(children);
  } while (parent != root);
  
  /* Make bottom right of window appear at cursor position */
  textWidth += 2; /* allow for border */
  textHeight +=2; /* allow for border */
  x = x-textWidth+1;
  y = y-textHeight+1;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  if ((win = XCreateWindow(_display, root, (unsigned)x,
			   (unsigned)y, (unsigned)textWidth, (unsigned)textHeight, 
			   /* border width */1, 0, InputOutput, CopyFromParent, AllPlanes , 
			   &attr)) == 0){
    fprintf(stderr,"DoPopup::DoPopup: can't create window\n");
    Exit::DoExit(1);
  }
  
  XSelectInput(_display, win, ExposureMask|ButtonPressMask);
  
  /* Map the window so that it appears on the display. */
  if (XMapWindow(_display, win) < 0) {
    fprintf(stderr,"DoPopup: can't map window\n");
    Exit::DoExit(2);
  }
  
  /* Do a sync to force the window to appear immediately */
  XSync(_display,false);
  
  XGCValues gcVal;
  gcVal.function = GXcopy;
  gcVal.plane_mask = AllPlanes;
  gcVal.foreground = fgnd;
  gcVal.background = bgnd;
  gcVal.font = fontStruct->fid;
  GC popUpGc = XCreateGC(_display, win, 
		 GCFunction| GCPlaneMask| GCForeground | GCBackground | GCFont,
			 &gcVal);
  /*
     XSetState(_display,popUpGc, fgnd, bgnd, GXcopy,AllPlanes);
  */
  
  Boolean savePopup = Init::SavePopup();
  if (!savePopup)
    XGrabServer(_display);
  while(1){
    XEvent event;
    /* check parent for button release */
    if (savePopup){
      if (XCheckWindowEvent(_display,win,ButtonPressMask,&event)){
	/* done */
	XDestroyWindow(_display,win);
	break;
      }
    }
    else {
      if (XCheckWindowEvent(_display,_win,ButtonReleaseMask,&event)){
	if (event.xbutton.button == button){
	  /* done */
	  XDestroyWindow(_display,win);
	  break;
	}
      }
    }
    
    /* check popUp window for exposure */
    if (XCheckWindowEvent(_display,win,ExposureMask,&event)){
      /* Draw the text messages into window */
      for(int i = 0; i < numMsgs; i++) {
	int startY = charHeight*i;
	XDrawString(_display, win, popUpGc, 1, startY+charAscent, 
		    msgs[i], strlen(msgs[i]));
      }
    }
  }
  XFreeGC(_display, popUpGc);
  if (!savePopup)
    XUngrabServer(_display);
  XFlush(_display);
}

Boolean XWindowRep::Scrollable()
{
  /* window is crollable if it has a backing store or
     if it is not obscured */
  /*
     printf("XWindowRep::Scrollable: 0x%x backingstore: %d, unobscured %d\n",
     this, _backingStore, _unobscured);
     */
  if (_backingStore || _unobscured)
    return true;
  else return false;
}

void XWindowRep::Scroll(Coord x, Coord y, Coord w, Coord h,
			Coord dstX, Coord dstY)
{
#ifdef DEBUG
  printf("XwindowRep::Scroll(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f)\n",
	 x,y,w,h,dstX,dstY);
#endif

  Coord xlow,ylow,xhi,yhi,width,height;
  Coord x1, y1, x2,y2;
  WindowRep::Transform(x,y, x1, y1);
  WindowRep::Transform(x+w,y+h, x2,y2);
  xlow = MinMax::min(x1,x2);
  xhi = MinMax::max(x1,x2);
  ylow = MinMax::min(y1,y2);
  yhi = MinMax::max(y1,y2);
  width = xhi-xlow+1; height = yhi-ylow+1;
  
  Coord tdx, tdy;
  WindowRep::Transform(dstX,dstY+h, tdx, tdy);
  
#ifdef DEBUG
  printf("XWindowRep::Scroll after xform x:%d,y:%d,w:%d,h:%d to x:%d, y%d\n",
	 (int)xlow, (int)ylow, (int)width, (int)height, (int)tdx, (int)tdy);
#endif
  
  XCopyArea(_display,_win, _win, _gc, (int)xlow, (int)ylow, 
	    (unsigned int)width, (unsigned int)height, 
	    (int)tdx, (int)tdy);
}

void XWindowRep::ScrollAbsolute(int x, int y, unsigned width, unsigned height,
				int dstX, int dstY)
{
#ifdef DEBUG
  printf("XWindowRep::ScrollABsolute(x:%d,y:%d,w:%d,h:%d,dx:%d,dy:%d)\n",
	 x,y,width,height,dstX,dstY);
#endif

  XCopyArea(_display, _win, _win, _gc, x, y, width, height,
	    dstX, dstY);
}

/* Flush windowRep's content to display */

void XWindowRep::Flush()
{
  /*
     XFlush(_display);
  */
  /* Do a sync to force output */
  XSync(_display,false);
}

const int MAX_PIXMAP_BUF_SIZE = 262144;
static char _pixmapBuf[MAX_PIXMAP_BUF_SIZE];

/* Get current window pixmap */

DevisePixmap *XWindowRep::GetPixmap()
{
#ifdef DEBUG
  printf("XWindowRep::GetPixmap()\n");
#endif

  Window root;
  int x,y;
  unsigned int width, height,border_width, depth;
  XGetGeometry(_display,_win,&root,&x,&y,&width,&height,
	       &border_width, &depth);
  
  DevisePixmap *pixmap = new DevisePixmap;
  pixmap->width = width;
  pixmap->height = height;
  Boolean initPixmap = true;
  int outIndex = 0;

  for(int i = 0; i < height; i++) {
    XImage *image = XGetImage(_display, _win, 0, i,width, 1,
			      AllPlanes, ZPixmap);
    if (image == NULL){
      return NULL;
    }
    if (initPixmap){
      pixmap->imageBytes = image->bytes_per_line*height;
      pixmap->bytes_per_line = image->bytes_per_line;
      pixmap->padding = image->bitmap_pad;
      pixmap->data = (unsigned char *)image->data;
      initPixmap = false;
    }
    int outCount;
    char *cBuf = _compress->CompressLine(image->data,image->bytes_per_line,
					 outCount);
    XDestroyImage(image);
    if (outCount + outIndex+sizeof(int) > MAX_PIXMAP_BUF_SIZE ){
      /* no more buffer */
      delete pixmap;
      return NULL;
    }
    bcopy((char *)&outCount,&_pixmapBuf[outIndex],sizeof(int));
    outIndex += sizeof(int);
    bcopy(cBuf, &_pixmapBuf[outIndex], outCount);
    outIndex += outCount;
  }
  
  pixmap->compressedBytes = outIndex;
  pixmap->data = (unsigned char *)malloc(outIndex);
  if (pixmap->data == NULL){
    delete pixmap;
    return NULL;
  }
  bcopy(_pixmapBuf,pixmap->data,outIndex);

#ifdef DEBUG
  printf("XWindowRep::GetPixmap: %d bytes\n",pixmap->compressedBytes);
#endif

  return pixmap;
}

/* Display pixmap in window */

void XWindowRep::DisplayPixmap(DevisePixmap *pixmap)
{
#ifdef DEBUG
  printf("XWindowRep::DisplayPixmap %d bytes\n", pixmap->compressedBytes);
#endif

  int screen = DefaultScreen(_display);
  int depth = DefaultDepth(_display,screen);
  Visual *visual = DefaultVisual(_display,screen);
  
  int i;
  int index = 0;
  unsigned char *data = pixmap->data;
  for (i=0; i < pixmap->height; i++){
    int count;
    int outCount;
    bcopy((char *)&data[index],(char *)&count, sizeof(int));
    index += sizeof(int);
    if (index + count > pixmap->compressedBytes){
      fprintf(stderr,"XWindowRep::DisplayPixmap: pixmap format\n");
      Exit::DoExit(2);
    }
    char *buf = _compress->DecompressLine((char *)&data[index],
					  count, outCount);
    index += count;
    if (outCount != pixmap->bytes_per_line){
      fprintf(stderr,"XWindowRep::DisplayPixmap: pixmap format 2\n");
      Exit::DoExit(2);
    }
    XImage *image = XCreateImage(_display, visual, depth, ZPixmap,
				 0, buf, pixmap->width, 1, pixmap->padding, 0);
    XPutImage(_display,_win, _gc, image, 0, 0, 0, i, image->width, 1);
    image->data = NULL;
    XDestroyImage(image);
  }
}

void XWindowRep::FreePixmap(DevisePixmap *pixmap)
{
  free(pixmap->data);
  delete pixmap;
}
