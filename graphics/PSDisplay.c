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

void PSDisplay::AllocColor(char *name, Color globalColor)
{
#ifdef DEBUG
  printf("PSDisplay::AllocColor(%s)\n", name);
#endif

  /* do something */
}

/*********************************************************************
Allocate color by RGB
*********************************************************************/

void PSDisplay::AllocColor(float r, float g, float b, Color globalColor)
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
				     Color fgnd, Color bgnd, 
				     WindowRep *parentRep,
				     Coord min_width, Coord min_height,
				     Boolean relative, Boolean winBoundary)
{
  DO_DEBUG(printf("PSDisplay::CreateWindowRep(%s)\n", name));

  return new PSWindowRep(this/*TEMPTEMP?*/, fgnd, bgnd, NULL/*TEMPTEMP?*/,
    (int) x, (int) y);

#if 0 //TEMPTEMP
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
    printf("XDisplay: Created X pixmap 0x%lx at %u,%u, size %ux%u\n",
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

  Color realFgnd, realBgnd;
  realFgnd = GetLocalColor(fgnd);
  realBgnd = GetLocalColor(bgnd);

  /* Define event mask. */

  unsigned long int mask = ExposureMask | ButtonPressMask 
	                   | ButtonReleaseMask | Button1MotionMask
                           | Button2MotionMask | Button3MotionMask
                           | StructureNotifyMask | KeyPressMask
                           | VisibilityChangeMask;

#ifdef RAWMOUSEEVENTS
  mask |= PointerMotionMask;
#endif
  
  /* Define window attributes. */

  XSetWindowAttributes attr;
  attr.background_pixmap 	= None;
  attr.background_pixel  	= realBgnd;
  attr.border_pixmap  		= CopyFromParent;
  attr.border_pixel  		= realFgnd;
  attr.bit_gravity  		= ForgetGravity;   /* CenterGravity */
  attr.win_gravity  		= NorthWestGravity;
  attr.backing_store  		= Always;          /* WhenMapped/NotUseful */
  attr.backing_planes  		= AllPlanes;
  attr.backing_pixel  		= 0;
  attr.save_under  		= False;
  attr.event_mask  		= mask;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect  	= False;
  attr.colormap	= DefaultColormap(_display, DefaultScreen(_display));
  attr.cursor  			= None;

  /* Create the window. */

  if (parentRep)
    parent = ((XWindowRep *)parentRep)->GetWinId();

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

  XSetStandardProperties(_display, w, name, name, None, 0, 0, &xsh);

  /* Set window manager hints for iconification. */

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
  
  /* Allow window manager to send WM_DELETE_WINDOW messages. */

  Atom deleteWindowAtom = XInternAtom(_display, "WM_DELETE_WINDOW", False);
  Atom protocolAtom = XInternAtom(_display, "WM_PROTOCOLS", False);
  XChangeProperty(_display, w, protocolAtom, XA_ATOM, 32,
                  PropModeReplace, (unsigned char *)&deleteWindowAtom, 1);

  /* Map the window so that it appears on the display. */

  if (XMapWindow(_display, w) < 0)
    return NULL;

  /* Do a sync to force the window to appear immediately. */

  XSync(_display, false);

#if 0
  /* Wait for MapNotify event to come back from server. */

  XEvent e;
  e.event = 0;
  while (e.event != MapNotify) {
    XNextEvent(_display, &e);
  }
#endif

  /* Return the XWindowRep structure. */

  XWindowRep *xwin = new XWindowRep(_display, w, this,
                                    (XWindowRep *)parentRep,
                                    fgnd, bgnd, false);
  DOASSERT(xwin, "Cannot create XWindowRep");
  _winList.Insert(xwin);
  
  return xwin;
#endif //TEMPTEMP

  /* do something */
  return 0;
}

/**************************************************************
Destroy a window
**************************************************************/

void PSDisplay::DestroyWindowRep(WindowRep *win)
{
  DO_DEBUG(printf("PSDisplay::DestroyWindowRep(%p)\n", win));

#if 0 //TEMPTEMP
  XWindowRep *xwin = (XWindowRep *)win;
  if (!_winList.Delete(xwin)) {
    fprintf(stderr, "XDisplay:Window to be deleted not found\n");
    Exit::DoExit(1);
  }

  // Free the Dali images and sleep before destroying the X window so
  // Dali doesn't get 'bad window' errors.
  if (xwin->DaliImageCount() > 0)
  {
    (void) xwin->DaliFreeImages();
    sleep(1);
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
#endif //TEMPTEMP

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

  //TEMPTEMP
  fprintf(_printFile, "%%!PS-Adobe-1.0\n");
  fprintf(_printFile, "%%%%BoundingBox: 0 0 558 480\n");
  fprintf(_printFile, "%%%%Creator: trigger:wenger (R. Kent Wenger)\n");
  fprintf(_printFile, "%%%%Title: stdin (xwdump)\n");
  fprintf(_printFile, "%%%%CreationDate: Tue Sep 17 14:43:11 1996\n");
  fprintf(_printFile, "%%%%EndComments\n");
  fprintf(_printFile, "%%%%Pages: 1\n");
  fprintf(_printFile, "%%%%EndProlog\n");
  fprintf(_printFile, "%%%%Page: 1 1\n");
}

/**************************************************************
Print PostScript trailer.
**************************************************************/

void PSDisplay::PrintPSTrailer()
{
  DOASSERT(_printFile != NULL, "No print file");

  //TEMPTEMP?
  fprintf(_printFile, "\nshowpage\n");
  fprintf(_printFile, "%%%%Trailer\n");
}
