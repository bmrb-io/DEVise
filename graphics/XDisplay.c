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
  Revision 1.2  1995/09/05 21:13:38  jussi
  Added/updated CVS header.
*/

#include <X11/Intrinsic.h>
#include "XDisplay.h"
#include "XWindowRep.h"
#include "Control.h"
#include "Journal.h"
#include "Init.h"

//#define DEBUG

/*******************************************************************
Open a new X display
********************************************************************/

XDisplay::XDisplay(char *name)
{
  if (!(this->_display = XOpenDisplay(name))) {
    (void)fprintf(stderr,"can't open display\n");
    Exit::DoExit(1);
  }
  
  /* init colors from the color manager. */
  DeviseDisplay::InitializeColors();
  
  /* get font */
  if (!(_fontStruct = XLoadQueryFont(_display,"8x16"))) {
    fprintf(stderr,"XDisplay: can not load font\n");
    Exit::DoExit(1);
  }
  
  /* init stipples for patterns*/
  Window win = XCreateSimpleWindow(_display, DefaultRootWindow(_display), 
				   (unsigned)0, (unsigned)0, (unsigned)10,
				   (unsigned)10, /* border width */ 5,
				   /* border */ 0, /* background */ 0);
  if (!win) {
    fprintf(stderr, "XDisplay::XDisplay() can't create window\n");
    Exit::DoExit(1);
  }

  for(int i = 0; i < XNumBitmaps; i++) {
    _stipples[i] = XCreateBitmapFromData(_display, win, xBitmaps[i],
					 XBitmapWidth, XBitmapHeight);
  }

  XDestroyWindow(_display, win);
}

/*******************************************************************
Alloc color by name
********************************************************************/

void XDisplay::AllocColor(char *name, Color globalColor)
{
  Colormap cmap = DefaultColormap(this->_display,
				  DefaultScreen(this->_display));
  XColor color_def;
  Color color;

  if (!XParseColor(this->_display, cmap, name, &color_def))
    goto error;
  if (!XAllocColor(this->_display, cmap, &color_def))
    goto error;
  color = color_def.pixel;
  DeviseDisplay::MapColor(color, globalColor);
  return;

 error:
  fprintf(stderr, "XDisplay::AllocColor: can't allocate %s\n", name);
  Exit::DoExit(1);
}

/*********************************************************************
Alloc color by RGB
*********************************************************************/

void XDisplay::AllocColor(double r, double g, double b, Color globalColor)
{
#ifdef DEBUG
  printf("XDisplay::AllocColor(%f,%f,%f,)\n",r,g,b);
#endif

  Colormap cmap = DefaultColormap(this->_display,
				  DefaultScreen(this->_display));
  XColor color_def;
  Color color;
  
  /* convert from (0.0,1.0) to (0,65535)*/
  color_def.red = (unsigned short)(r * 65535); 
  color_def.green = (unsigned short)(g * 65535);
  color_def.blue = (unsigned short)(b * 65535);

#ifdef DEBUG
  printf("Alloc red %d, green %d, blue %d\n",
	 color_def.red, color_def.green, color_def.blue);
#endif

  if (!XAllocColor(this->_display, cmap, &color_def))
    goto error;
  color = color_def.pixel;
  DeviseDisplay::MapColor(color, globalColor);
  return;

error:
  fprintf(stderr, "XDisplay::AllocColor: can't allocate %f,%f,%f\n",
	  r, g, b);
  Exit::DoExit(1);
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
  Window w, parent;
  XSetWindowAttributes attr;
  XSizeHints xsh;
  XWindowRep *xParentRep = (XWindowRep *)parentRep;

  if (!parentRep)
    parent = DefaultRootWindow(_display);
  else
    parent = xParentRep->GetWin();

  Color realFgnd, realBgnd;
  realFgnd = GetLocalColor(fgnd);
  realBgnd = GetLocalColor(bgnd);

  attr.background_pixmap 	= None;
  attr.background_pixel  	= realBgnd;
  attr.border_pixmap  		= CopyFromParent;
  attr.border_pixel  		= realFgnd;
  attr.bit_gravity  		= ForgetGravity /*CenterGravity*/;
  attr.win_gravity  		= NorthWestGravity;
  attr.backing_store  		= Always /* NotUseful*/ ;
  attr.backing_planes  		= AllPlanes;
  attr.backing_pixel  		= 0;
  attr.save_under  		= False;
  attr.event_mask  		= 0;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect  	= False;
  attr.colormap	= DefaultColormap(_display, DefaultScreen(_display));
  attr.cursor  			= None;

  /* Deal with providing the window with an initial position & size.
   * Fill out the XSizeHints struct to inform the window manager. See
   * Sections 9.1.6 & 10.3.
   */

  Coord realX, realY, realWidth, realHeight;
  Coord real_min_width, real_min_height;
  if (relative) {
    RealWindowDimensions(parent,x,y,width,height,realX,realY,realWidth,
			 realHeight);
    RealWindowWidthHeight(parent,min_width,min_height,real_min_width,
			  real_min_height);
  } else {
    realX = x;
    realY = y;
    realWidth = width;
    realHeight = height;
    real_min_width = min_width;
    real_min_height = min_height;
  }

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

  if ((w = XCreateWindow(_display, parent, (unsigned)realX, (unsigned)realY, 
			 (unsigned)realWidth, (unsigned)realHeight,
			 border_width, 0, InputOutput, CopyFromParent,
			 AllPlanes, &attr)) == 0) {
    fprintf(stderr,"XDisplay::CreateWindowRep: can't create window\n");
    Exit::DoExit(1);
  }

#ifdef DEBUG
  printf("XDisplay: Creating X window 0x%x to parent 0x%x at %u,%u,\n",
	 w, parent, (unsigned)realX, (unsigned)realY);
  printf("          size %u,%u, borderwidth %d\n", (unsigned)realWidth,
	 (unsigned)realHeight, border_width);
#endif

  XSelectInput(_display, w, ExposureMask | ButtonPressMask 
	       | ButtonReleaseMask | Button1MotionMask | Button2MotionMask
	       | Button3MotionMask | StructureNotifyMask | KeyPressMask |
	       VisibilityChangeMask);
  
  /*
   * Set the standard properties for the window managers. See Section
   * 9.1.
   */

  XSetStandardProperties(_display, w, name, name, None, 0, 0, &xsh);
  if (ControlPanel::Instance()->Restoring() && Init::Iconify()) {
    xwmh.flags = InputHint | StateHint | IconPositionHint;
    xwmh.input = true;
    xwmh.initial_state = IconicState;
    xwmh.icon_x = (unsigned)realX;
    xwmh.icon_y = (unsigned)realY;
  } else {
    xwmh.flags = InputHint | StateHint;
    xwmh.input = true;
    xwmh.initial_state = NormalState;
  }
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

  XDestroyWindow(_display, xwin->GetWin());

  delete xwin;

  XSync(_display, false);
}

/******************************************************************
Internal non-blocking event processing .
********************************************************************/

void XDisplay::InternalProcessing()
{
#if 1
  static first = true;
  if (first) {
    printf("first XDisplay::InternalProcessing\n");
    first = false;
  }
#endif

  while (XPending(_display)> 0) {
    /* There are events on the queue */
    XEvent event;
    XNextEvent(_display,&event);

    /* dispatch event to appropriate window.*/
    Boolean found = false;
    for(int index = _winList.InitIterator(); _winList.More(index);) {
      XWindowRep *win= _winList.Next(index);
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
    if (!found){
      _winList.DoneIterator(index);
    }
  }
  
#if 0
  /* draw in all windows */

  for(index = _winList.InitIterator(); _winList.More(index);) {
    XWindowRep *win= _winList.Next(index);
    win->Draw();
  }
  _winList.DoneIterator(index);
#endif
}
