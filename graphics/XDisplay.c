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
  Revision 1.22  1996/05/20 18:45:02  jussi
  Merged with ClientServer library code.

  Revision 1.21  1996/05/13 21:57:30  jussi
  XDisplay goes into batch mode (pixmaps used instead of screen)
  if control panel is in batch mode.

  Revision 1.20  1996/05/11 02:29:26  jussi
  Ifdef'd out some startup options (whether windows should appear
  iconic or not when a session is restored).

  Revision 1.19  1996/05/08 15:59:36  jussi
  Replaced _display->fd, which is an illegal access, with the
  more portable ConnectionNumber(_display) defined in Xlib.h.

  Revision 1.18  1996/04/20 19:52:24  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.17  1996/04/18 18:16:22  jussi
  This class now creates both X windows and X pixmaps, depending
  on whether the system is in interactive mode or batch mode.

  Revision 1.16  1996/04/11 18:15:36  jussi
  Made XDisplay use the polling mechanism temporarily until the
  occasional display update problem has been resolved.

  Revision 1.15  1996/04/09 18:06:13  jussi
  Added Flush() method.

  Revision 1.14  1996/04/08 16:58:39  jussi
  #define'd XLIB_ILLEGAL_ACCESS so that we have access to Display->fd
  in the X display data structured. Minor other fixes.

  Revision 1.13  1996/04/04 05:18:30  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from Display)
  corrected a bug in call to register function. Also now dispatcher
  uses socket number passed from the XDisplay class to select on it and
  call the relevant functions.

  Revision 1.12  1996/02/26 23:46:39  jussi
  XDisplay tries to load 5x8 font if loading 5x7 fails.

  Revision 1.11  1996/02/05 23:55:17  jussi
  Added support for small fonts.

  Revision 1.10  1996/01/13 20:58:38  jussi
  Made system display the name of the base color when another
  color is mapped to it.

  Revision 1.9  1996/01/10 20:59:03  jussi
  Failure to allocate all colors will now cause a base color
  (black) to be substituted.

  Revision 1.8  1995/12/28 18:56:51  jussi
  Small fix to remove compiler warning.

  Revision 1.7  1995/12/14 21:13:40  jussi
  Replaced 0x%x with 0x%p.

  RevisionXDisplay 1.6  1995/12/06 21:22:33  jussi
  Tries to allocate a color by using a close color approximation
  if requested color cannot be allocated.

  Revision 1.5  1995/12/02 21:30:11  jussi
  Tried if letting Tcl/Tk handle all X events and pass a copy
  of each event to us would solve the problem with TK_WINDOW_old.
  This event handling mechanism didn't turn out to work right
  so I renamed it TK_WINDOW_EV2.

  Revision 1.4  1995/11/29 15:30:09  jussi
  Disabled a debugging message.

  Revision 1.3  1995/11/29 15:12:07  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:13:38  jussi
  Added/updated CVS header.
*/

#include <math.h>

#include "XDisplay.h"
#include "XWindowRep.h"
#ifndef LIBCS
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif

//#define DEBUG

#ifdef TK_WINDOW_EV2
static int HandleTkEvent(ClientData data, XEvent *event)
{
  return ((XDisplay *)data)->HandleXEvent(*event);
}
#endif

static const char *baseColorName = "black";

/*******************************************************************
Open a new X display
********************************************************************/

XDisplay::XDisplay(char *name)
{
  if (!(_display = XOpenDisplay(name))) {
    fprintf(stderr, "Cannot open XDisplay\n");
    Exit::DoExit(1);
  }
  
  /* init base color */
  XColor baseColorDef;
  Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));
  if (!XParseColor(_display, cmap, baseColorName, &baseColorDef)) {
    fprintf(stderr, "Cannot understand color %s.\n", baseColorName);
    Exit::DoExit(1);
  }
  float error;
  if (!ClosestColor(cmap, baseColorDef, baseColor, error)) {
    fprintf(stderr, "Cannot allocate color %s.\n", baseColorName);
    Exit::DoExit(1);
  }

  /* init colors from the color manager. */
  DeviseDisplay::InitializeColors();
  
  /* get normal font */
  if (!(_normalFontStruct = XLoadQueryFont(_display, "7x13"))) {
    fprintf(stderr, "XDisplay: cannot load font 7x13\n");
    Exit::DoExit(1);
  }

  /* get small font */
  if (!(_smallFontStruct = XLoadQueryFont(_display, "5x7"))) {
    if (!(_smallFontStruct = XLoadQueryFont(_display, "5x8"))) {
      fprintf(stderr, "XDisplay: cannot load font 5x7 nor 5x8\n");
      Exit::DoExit(1);
    }
  }
  
  /* set big font to be current font */
  _fontStruct = _normalFontStruct;

  /* init stipples for patterns */
  Window win = XCreateSimpleWindow(_display, DefaultRootWindow(_display),
				   (unsigned)0, (unsigned)0, (unsigned)10,
				   (unsigned)10, /* border width */ 5,
				   /* border */ 0, /* background */ 0);
  DOASSERT(win, "Cannot create window");

  for(int i = 0; i < XNumBitmaps; i++) {
    _stipples[i] = XCreateBitmapFromData(_display, win, xBitmaps[i],
					 XBitmapWidth, XBitmapHeight);
  }

  XDestroyWindow(_display, win);

#ifdef TK_WINDOW_EV2
  /* tell Tk to pass all X events to us */
  Tk_CreateGenericHandler(HandleTkEvent, (ClientData)this);
#endif

#ifndef LIBCS
  Register();
#endif
}

#ifndef LIBCS
void XDisplay::Register()
{
  int fd = ConnectionNumber(_display);

#ifdef DEBUG
  printf("About to be registered... %d\n", fd);
#endif

  (DeviseDisplay::ReturnDispatcher())->Register((DeviseDisplay *)this,
						10, AllState, true, fd);
}
#endif

void XDisplay::Flush()
{
  /* Do a sync to force all buffered window operations on the screen */
  XSync(_display, false);
}

/*******************************************************************
Allocate closest matching color
********************************************************************/

Boolean XDisplay::ClosestColor(Colormap &map, XColor &color, Color &c,
			       float &error)
{
  if (XAllocColor(_display, map, &color)) {
    c = color.pixel;
    error = 0.0;
    return true;
  }

  // if exact color match is not found, try allocating a color that
  // is close enough; maxDeviation specifies maximum distance in each
  // of the RGB directions (X color values are between 0 and 65535);
  // increment specifies the hop size between each attempt

  const int maxDeviation = (int)(0.05 * 65535);   // +-5% max deviation
  const int incFraction = 4;                      // number of probes per color
    
  const long int lwr = MAX(color.red - maxDeviation, 0);
  const long int hir = MIN(color.red + maxDeviation, 65535);
  const long int lwg = MAX(color.green - maxDeviation, 0);
  const long int hig = MIN(color.green + maxDeviation, 65535);
  const long int lwb = MAX(color.blue - maxDeviation, 0);
  const long int hib = MIN(color.blue + maxDeviation, 65535);

  const int rstep = MAX((hir - lwr) / incFraction, 1);
  const int gstep = MAX((hig - lwg) / incFraction, 1);
  const int bstep = MAX((hib - lwb) / incFraction, 1);

  for(int r = lwr; r <= hir; r += rstep) {
    for(int g = lwg; g <= hig; g += gstep) {
      for(int b = lwb; b <= hib; b += bstep) {
	XColor ctry;
	ctry.red   = (unsigned short)r;
	ctry.green = (unsigned short)g;
	ctry.blue  = (unsigned short)b;
#ifdef DEBUG_xxx
	printf("Trying to allocate color %d,%d,%d\n", ctry.red,
	       ctry.green, ctry.blue);
#endif
	if (XAllocColor(_display, map, &ctry)) {
	  c = ctry.pixel;
	  float rerr = (ctry.red - color.red) / 65535;
	  float gerr = (ctry.green - color.green) / 65535;
	  float berr = (ctry.blue - color.blue) / 65535;
	  error = sqrt(rerr * rerr + gerr * gerr + berr * berr);
	  return true;
	}
      }
    }
  }

  return false;
}

/*******************************************************************
Alloc color by name
********************************************************************/

void XDisplay::AllocColor(char *name, Color globalColor)
{
  Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));

  XColor color_def;
  if (!XParseColor(_display, cmap, name, &color_def)) {
    fprintf(stderr, "Cannot understand color %s.\n", name);
    Exit::DoExit(1);
  }

#ifdef DEBUG
  printf("XDisplay::AllocColor %s\n", name);
#endif

  Color color;
  float error;
  if (ClosestColor(cmap, color_def, color, error)) {
    DeviseDisplay::MapColor(color, globalColor);
    if (error > 0)
      printf("Color %s allocated with %.2f error\n", name, error);
    return;
  }

  // substitute base color instead of requested color
  DeviseDisplay::MapColor(baseColor, globalColor);
  printf("Color %s mapped to color %s\n", name, baseColorName);
}

/*********************************************************************
Alloc color by RGB
*********************************************************************/

void XDisplay::AllocColor(double r, double g, double b, Color globalColor)
{
#ifdef DEBUG
  printf("XDisplay::AllocColor(%.2f,%.2f,%.2f)\n",r,g,b);
#endif

  Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));

  /* convert from (0.0,1.0) to (0,65535)*/
  XColor color_def;
  color_def.red = (unsigned short)(r * 65535); 
  color_def.green = (unsigned short)(g * 65535);
  color_def.blue = (unsigned short)(b * 65535);

#ifdef DEBUG
  printf("XDisplay::AllocColor red %d, green %d, blue %d\n",
	 color_def.red, color_def.green, color_def.blue);
#endif

  Color color;
  float error;
  if (ClosestColor(cmap, color_def, color, error)) {
    DeviseDisplay::MapColor(color, globalColor);
    if (error > 0)
      printf("Color <%.2f,%.2f,%.2f> allocated with %.2f error\n",
	     r, g, b, error);
    return;
  }

  // substitute base color instead of requested color
  DeviseDisplay::MapColor(baseColor, globalColor);
  printf("Color <%.2f,%.2f,%.2f> mapped to color %s\n",
	 r, g, b, baseColorName);
}

/*
 * This structure forms the WMHINTS property of the window,
 * letting the window manager know how to handle this window.
 * See Section 9.1 of the Xlib manual.
 */

XWMHints xwmh = {
  (InputHint | StateHint | IconPositionHint),	/* flags */
  False,					/* input */
  NormalState,					/* initialstate */
  0,						/* icon pixmap */
  0,						/* icon window */
  0, 0,						/* icon location */
  0,						/* icon mask */
  0						/* Window group */
};

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *XDisplay::CreateWindowRep(char *name, Coord x, Coord y,
				     Coord width, Coord height, 
				     Color fgnd, Color bgnd, 
				     WindowRep *parentRep,
				     Coord min_width, Coord min_height,
				     Boolean relative, Boolean winBoundary)
{
  Window parent = DefaultRootWindow(_display);

  Coord realX, realY, realWidth, realHeight;
  Coord real_min_width, real_min_height;
  if (relative) {
    RealWindowDimensions(parent, x, y, width, height, realX, realY,
			 realWidth, realHeight);
    RealWindowWidthHeight(parent, min_width, min_height,
			  real_min_width, real_min_height);
  } else {
    realX = x;
    realY = y;
    realWidth = width;
    realHeight = height;
    real_min_width = min_width;
    real_min_height = min_height;
  }

#ifndef LIBCS
  if (ControlPanel::Instance()->GetBatchMode()) {
    // we're in batch mode -- create a pixmap instead of a window

    unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
    Pixmap pixmap = XCreatePixmap(_display, parent, (unsigned)realWidth,
				  (unsigned)realHeight, depth);
    DOASSERT(pixmap, "Cannot create pixmap");

#ifdef DEBUG
    printf("XDisplay: Created X pixmap 0x%lx at %u,%u, size %u%u\n",
	   pixmap, (unsigned)realX, (unsigned)realY,
	   (unsigned)realWidth, (unsigned)realHeight);
#endif

    XWindowRep *xwin = new XWindowRep(_display, pixmap, this,
				      (XWindowRep *)parentRep,
				      fgnd, bgnd, (int)realX, (int)realY);
    DOASSERT(xwin, "Cannot create XWindowRep");

    _winList.Insert(xwin);
  
    return xwin;
  }
#endif

  if (parentRep) {
    XWindowRep *xParentRep = (XWindowRep *)parentRep;
    parent = xParentRep->GetWinId();
  }

  Color realFgnd, realBgnd;
  realFgnd = GetLocalColor(fgnd);
  realBgnd = GetLocalColor(bgnd);

  XSetWindowAttributes attr;
  attr.background_pixmap 	= None;
  attr.background_pixel  	= realBgnd;
  attr.border_pixmap  		= CopyFromParent;
  attr.border_pixel  		= realFgnd;
  attr.bit_gravity  		= ForgetGravity;   /* CenterGravity */
  attr.win_gravity  		= NorthWestGravity;
  attr.backing_store  		= Always;          /* NotUseful*/
  attr.backing_planes  		= AllPlanes;
  attr.backing_pixel  		= 0;
  attr.save_under  		= False;
  attr.event_mask  		= 0;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect  	= False;
  attr.colormap	= DefaultColormap(_display, DefaultScreen(_display));
  attr.cursor  			= None;

  /*
   * Deal with providing the window with an initial position & size.
   * Fill out the XSizeHints struct to inform the window manager. See
   * Sections 9.1.6 & 10.3.
   */

  XSizeHints xsh;
  xsh.flags 	= PPosition | PSize | PMinSize;
  xsh.height 	= (int)realHeight;
  xsh.width 	= (int)realWidth;
  xsh.x 	= (int)realX;
  xsh.y 	= (int)realY;
  xsh.min_width = (int)real_min_width;
  xsh.min_height = (int)real_min_height;

  /* Create the window. */
  unsigned int border_width;

  if (winBoundary)
    border_width = (!parent ? 5 : 1);
  else
    border_width = (!parent ? 5 : 0);

  Window w = XCreateWindow(_display, parent, (unsigned)realX, (unsigned)realY, 
			   (unsigned)realWidth, (unsigned)realHeight,
			   border_width, 0, InputOutput, CopyFromParent,
			   AllPlanes, &attr);
  DOASSERT(w, "Cannot create window");

#ifdef DEBUG
  printf("XDisplay: Created X window 0x%lx to parent 0x%lx at %u,%u,\n",
	 w, parent, (unsigned)realX, (unsigned)realY);
  printf("          size %u,%u, borderwidth %d\n", (unsigned)realWidth,
	 (unsigned)realHeight, border_width);
#endif

  XSelectInput(_display, w, ExposureMask | ButtonPressMask 
	       | ButtonReleaseMask | Button1MotionMask | Button2MotionMask
	       | Button3MotionMask | StructureNotifyMask | KeyPressMask
#ifdef RAWMOUSEEVENTS
               | PointerMotionMask
#endif
	       | VisibilityChangeMask);
  
  /*
   * Set the standard properties for the window managers. See Section
   * 9.1.
   */

  XSetStandardProperties(_display, w, name, name, None, 0, 0, &xsh);
#if 0
  if (ControlPanel::Instance()->Restoring() && Init::Iconify()) {
    xwmh.flags = InputHint | StateHint | IconPositionHint;
    xwmh.input = true;
    xwmh.initial_state = IconicState;
    xwmh.icon_x = (unsigned)realX;
    xwmh.icon_y = (unsigned)realY;
  }
#endif
  xwmh.flags = InputHint | StateHint;
  xwmh.input = true;
  xwmh.initial_state = NormalState;
  XSetWMHints(_display, w, &xwmh);
  
  /* Map the window so that it appears on the display. */
  if (XMapWindow(_display, w) < 0)
    return NULL;

  /* Do a sync to force the window to appear immediately */
  XSync(_display, false);

#if 0
  XEvent e;
  e.event = 0;
  while (e.event != MapNotify) {
    XNextEvent(_display, &e);
  }
#endif

  /* return the XWindowRep structure */
  XWindowRep *xwin = new XWindowRep(_display, w, this, fgnd, bgnd, false);
  DOASSERT(xwin, "Cannot create XWindowRep");
  _winList.Insert(xwin);
  
  return xwin;
}

/**************************************************************
 Destroy a window. Parameter "win" better be of type XwindowRep *.
**************************************************************/

void XDisplay::DestroyWindowRep(WindowRep *win)
{
  XWindowRep *xwin = (XWindowRep *)win;
  if (!_winList.Delete(xwin)) {
    fprintf(stderr, "XDisplay:Window to be deleted not found\n");
    Exit::DoExit(1);
  }

  if (xwin->GetWinId()) {
#ifdef DEBUG
    printf("XDisplay::DestroyWindowRep 0x%p, window 0x%lx\n",
	   xwin, xwin->GetWinId());
#endif
    XDestroyWindow(_display, xwin->GetWinId());
  } else {
#ifdef DEBUG
    printf("XDisplay::DestroyWindowRep 0x%p, pixmap 0x%lx\n",
	   xwin, xwin->GetPixmapId());
#endif
    XFreePixmap(_display, xwin->GetPixmapId());
  }

  delete xwin;

  XSync(_display, false);
}

/******************************************************************
Internal non-blocking event processing .
********************************************************************/

void XDisplay::InternalProcessing()
{
#ifdef TK_WINDOW_EV2
  // X events handled in HandleTkEvent
  return;
#endif

  while (XPending(_display) > 0) {
    /* There are events on the queue */
    XEvent event;
    XNextEvent(_display, &event);

    /* dispatch event to appropriate window.*/
    Boolean found = false;
    int index;
    for(index = _winList.InitIterator(); _winList.More(index);) {
      XWindowRep *win = _winList.Next(index);
      if (win->GetWinId() && win->GetWinId() == event.xany.window) {
	/* Note: got to be careful here. We need to
	   call DoneIterator() before informing the WindowRep
	   because it might trigger a call to DestroyWindowRep() to
	   delete the window. Delete() can't be called
	   when the iterator is active. */
	_winList.DoneIterator(index);
	found = true;
#ifdef DEBUG
	printf("XDisplay::Dispatching event %d to XWindowRep 0x%p\n",
	       event.type, win);
#endif
	win->HandleEvent(event);
	break;
      }
    }
    if (!found) {
      _winList.DoneIterator(index);
#ifdef DEBUG
      printf("XDisplay::InternalProcessing: window for event %d not found\n",
	     event.type);
#endif
    }
  }
}

#ifdef TK_WINDOW_EV2
int XDisplay::HandleXEvent(XEvent &event)
{
#ifdef DEBUG
  printf("XDisplay received X event %d from Tk\n", event.type);
#endif

  /* dispatch event to appropriate window.*/
  Boolean found = false;
  for(int index = _winList.InitIterator(); _winList.More(index);) {
    XWindowRep *win = _winList.Next(index);
    if (win->_win == event.xany.window) {
      /* Note: got to be careful here. We need to
	 call DoneIterator() before informing the WindowRep
	 because it might trigger a call to DestroyWindowRep() to
	 delete the window. Delete() can't be called
	 when the iterator is active. */
      _winList.DoneIterator(index);
      found = true;
      win->HandleEvent(event);
      break;
    }
  }

  if (!found) {
    _winList.DoneIterator(index);
#ifdef DEBUG
    printf("XDisplay::InternalProcessing: window for event %d not found\n",
	   event.type);
#endif
    return 0;
  }

  return 1;
}
#endif
