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
  Revision 1.51  1996/07/23 15:34:45  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.50  1996/07/22 19:32:04  jussi
  Requesting a pixmap of a window that is partially off the screen
  no longer causes Devise to crash.

  Revision 1.49  1996/07/18 01:24:10  jussi
  Added GetRootGeometry() and FindTopWindow() methods.

  Revision 1.48  1996/07/15 21:02:19  jussi
  Fixed Arc() computation.

  Revision 1.47  1996/07/15 00:30:21  jussi
  Added handling of ButtonRelease events when not in RAWMOUSEEVENTS
  mode.

  Revision 1.46  1996/07/14 16:50:49  jussi
  Added handling of DestroyNotify events.

  Revision 1.45  1996/07/14 04:08:50  jussi
  Added handling of ReparentNotify events and WM_DELETE_WINDOW
  messages from the window manager.

  Revision 1.44  1996/07/10 16:21:12  jussi
  Improvements and simplifications to the code.

  Revision 1.43  1996/06/21 21:33:43  jussi
  Changed xpr parameters so that a grayscale Postscript image is
  produced instead of black and white.

  Revision 1.42  1996/06/21 19:31:07  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.
  Replaced MinMax calls with calls to MIN() and MAX().

  Revision 1.41  1996/06/16 01:52:54  jussi
  Minor improvements here and there, mostly debugging
  statements.

  Revision 1.40  1996/06/15 13:49:56  jussi
  Added SetWindowBgColor() which allows Devise to change
  the view background color at runtime.

  Revision 1.39  1996/06/15 06:59:43  yuc
  Add "clipping" to MapAllXpoints, move the code related to perspective
  view to CompProjectionOnViewingPlane, add aim axis (or focus axis) to
  DrawRefAxis.

  Revision 1.38  1996/05/31 20:54:24  jussi
  Fixed computation in Arc().

  Revision 1.37  1996/05/31 15:32:12  jussi
  Added 'state' variable to HandleButton(). This tells the callee
  whether the shift/control keys were pressed in conjunction with
  the mouse button.

  Revision 1.36  1996/05/22 21:02:47  jussi
  Added ImportImage() method.

  Revision 1.35  1996/05/20 18:45:04  jussi
  Merged with ClientServer library code.

  Revision 1.34  1996/05/20 17:27:36  jussi
  Removed platform dependency in ExportImage().

  Revision 1.33  1996/04/20 19:52:29  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.32  1996/04/18 18:17:01  jussi
  Added support for drawing into pixmaps instead of windows.

  Revision 1.31  1996/04/17 20:33:41  jussi
  Added ExportGIF() method.

  Revision 1.30  1996/04/16 19:49:41  jussi
  Replaced assert() calls with DOASSERT().

  Revision 1.29  1996/04/11 17:54:28  jussi
  Added Raise() and Lower().

  Revision 1.28  1996/03/29 18:14:05  wenger
  Got testWindowRep to compile and run, added drawing in
  windows; fixed a few more compile warnings, etc.

  Revision 1.27  1996/03/27 17:55:02  wenger
  Changes to get DEVise to compile and run on Linux.

  Revision 1.26  1996/03/26 15:34:41  wenger
  Fixed various compile warnings and errors; added 'clean' and
  'mostlyclean' targets to makefiles; changed makefiles so that
  object lists are derived from source lists.

  Revision 1.25  1996/02/28 17:37:34  yuc
  Added CompLocationOnViewingSpace, CompProjectionOnViewingPlane,
  DrawXSegments, MapAllPoints, MapAllSegments, and DrawRefAxis.  These
  functions are for 3D display.

  Revision 1.24  1996/02/26 23:46:08  jussi
  Added GetSmallFontHeight().

  Revision 1.23  1996/02/26 16:42:54  jussi
  Makes 10 attempts to export an image.

  Revision 1.22  1996/02/06 19:24:25  jussi
  Restore line attributes (line width and join style).

  Revision 1.21  1996/02/05 23:55:20  jussi
  Added support for small fonts.

  Revision 1.20  1996/01/31 21:35:31  jussi
  Refined Xor stuff.

  Revision 1.19  1996/01/30 21:13:51  jussi
  Removed references to specific colors.

  Revision 1.18  1996/01/30 00:04:17  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.17  1996/01/17 22:40:22  jussi
  One more refinement.

  Revision 1.16  1996/01/17 22:18:32  jussi
  Refined image export support.

  Revision 1.15  1996/01/17 21:05:14  jussi
  Minor fix for SGI EPS output.

  Revision 1.14  1996/01/17 20:53:34  jussi
  Added support for additional image export formats.

  Revision 1.13  1995/12/28 18:59:23  jussi
  Small fixes to remove compiler warnings.

  Revision 1.12  1995/12/18 03:13:13  ravim
  Inverted forms of drawing lines and rectangles.

  Revision 1.11  1995/12/14 21:14:02  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.10  1995/12/14 17:22:01  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.9  1995/12/06 21:24:09  jussi
  Minor bug fixes in computing exact pixel values (removed some one-off
  errors).

  Revision 1.8  1995/12/05 22:02:01  jussi
  DoPopup now uses AbsoluteOrigin to compute the absolute pixel
  position of a popup window rather than duplicate the code.
  Minor other improvements in debugging output.

  Revision 1.7  1995/12/02 21:28:01  jussi
  Added support for TK_WINDOW but the first implementation is
  obsoleted and renamed TK_WINDOW_old. Added various debugging
  output statements.

  Revision 1.6  1995/11/29 15:11:15  jussi
  Minor fix to really #ifdef out WritePostscript debugging output.

  Revision 1.5  1995/11/28 00:00:43  jussi
  Added WritePostscript() method.

  Revision 1.4  1995/11/25 01:16:18  jussi
  Replaced calls to bcopy() with more portable memcpy()'s.

  Revision 1.3  1995/11/24 21:29:33  jussi
  Added copyright notice and cleaned up code. Added debugging
  statements.

  Revision 1.2  1995/09/05 21:13:41  jussi
  Added/updated CVS header.
*/

#include <unistd.h>
#include <errno.h>

#include "XWindowRep.h"
#include "XDisplay.h"
#include "Compress.h"
#ifndef LIBCS
#include "Init.h"
#endif

extern "C" {
#include "xv.h"
}

//#define DEBUG
#define MAXPIXELDUMP 0

#define ROUND(type, value) ((type)(value + 0.5))
#define DRAWABLE           (_win ? _win : _pixmap)

/**********************************************************************
Initializer
***********************************************************************/

XWindowRep:: XWindowRep(Display *display, Window window, XDisplay *DVDisp,
			XWindowRep *parent, Color fgndColor, Color bgndColor,
			Boolean backingStore) :
	WindowRep(DVDisp, fgndColor, bgndColor)
{
  _display = display;
  _win = window;
  _pixmap = 0;
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);
  _backingStore = false;

  Init();
}

XWindowRep:: XWindowRep(Display *display, Pixmap pixmap, XDisplay *DVDisp,
			XWindowRep *parent, Color fgndColor, Color bgndColor,
			int x, int y) :
	WindowRep(DVDisp, fgndColor, bgndColor)
{
  _display = display;
  _win = 0;
  _pixmap = pixmap;
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);
  _backingStore = false;

  _x = x;
  _y = y;

  Init();

  // clear all pixels in pixmap
  SetFgColor(bgndColor);
  XFillRectangle(_display, _pixmap, _gc, 0, 0, _width, _height);
}

void XWindowRep::Init()
{
#ifdef LIBCS
  _dispGraphics = true;
#else
  _dispGraphics = Init::DispGraphics();
#endif
  _compress = new SimpleCompress();

  UpdateWinDimensions();

  _unobscured = false;

  _gc = XCreateGC(_display, DRAWABLE, 0, NULL);
  SetCopyMode();

  _rectGc = XCreateGC(_display, DRAWABLE, 0, NULL);
  XSetState(_display, _rectGc, AllPlanes, AllPlanes, GXxor, AllPlanes);
  
  /* set default pattern to pattern 0: no stipple */
  WindowRep::SetPattern(Pattern0);
  XSetFillStyle(_display, _gc, FillSolid);
  
  /* set normal font */
  SetNormalFont(); 
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
 
  /* create pixmaps for character manipulation */
  _srcBitmap.inUse = false;
  _dstBitmap.inUse = false;
  int bitmapWidth = 80 * (fontStruct->max_bounds.rbearing -
			  fontStruct->min_bounds.lbearing);
  int bitmapHeight = fontStruct->max_bounds.ascent +
                     fontStruct->max_bounds.descent;
  AllocBitmap(_srcBitmap, bitmapWidth, bitmapHeight);
  AllocBitmap(_dstBitmap, 3 * bitmapWidth, 3 * bitmapHeight);

#ifdef TK_WINDOW_old
  _tkPathName[0] = 0;
  _tkWindow = 0;
#endif
}

/**************************************************************
  destructor 
**************************************************************/

XWindowRep::~XWindowRep()
{
  XFreeGC(_display, _gc);
  XFreeGC(_display, _rectGc);
  
  FreeBitmap(_srcBitmap);
  FreeBitmap(_dstBitmap);
  
  /* _win or _pixmap is destroyed by XDisplay */

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

void XWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
  DOASSERT(_win, "Cannot reparent a pixmap");

  Window newParent = (child ? _win : (Window)other);
  Window newChild = (child ? (Window)other : _win);

  if (!newParent)
    newParent = DefaultRootWindow(_display);

#ifdef DEBUG
  printf("XWindowRep::Reparent 0x%lx to 0x%lx at %d,%d\n",
	 newChild, newParent, x, y);
#endif

  XReparentWindow(_display, newChild, newParent, x, y);
}

/******************************************************************/

void XWindowRep::PushClip(Coord x, Coord y, Coord w, Coord h)
{
#ifdef DEBUG
  printf("XWindowRep::PushClip(%.2f,%.2f,%.2f,%.2f)\n", x, y, w, h);
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
  printf("XwindowRep::PushClip: transformed into (%.2f,%.2f,%.2f,%.2f)\n",
	 xlow, ylow, width, height);
#endif

  XRectangle rect;
  rect.x = ROUND(short, xlow);
  rect.y = ROUND(short, ylow);
  rect.width = (unsigned)(ROUND(int, xhi) - ROUND(int, xlow) + 1);
  rect.height = (unsigned)(ROUND(int, yhi) - ROUND(int, ylow) + 1);

#ifdef DEBUG
  printf("XwindowRep::PushClip: rounded to (%d,%d,%d,%d)\n",
	 rect.x, rect.y, rect.width, rect.height);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XSetClipRectangles(_display, _gc, 0, 0, &rect, 1, Unsorted);
#endif

  WindowRep::_PushClip(xlow, ylow, width, height);
}

/*******************************************************************/

void XWindowRep::PopClip()
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
    if (_dispGraphics)
      XSetClipRectangles(_display, _gc, 0, 0, &rect, 1, Unsorted);
#endif

  } else {

    /* no more clipping */
#ifdef GRAPHICS 
    if (_dispGraphics)
      XSetClipMask(_display, _gc, None);
#endif
  }
}

/* import window image */

void XWindowRep::ImportImage(Coord x, Coord y,
			     DisplayExportFormat format,
			     char *filename)
{
  if (format != GIF) {
    fprintf(stderr, "Import format not supported yet.\n");
    return;
  }

#ifdef DEBUG
  printf("Reading GIF image from file %s\n", filename);
#endif

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Cannot open file %s", filename);
    return;
  }

  PICINFO picinfo;
  if (LoadGIF(filename, &picinfo) != 1) {
    fprintf(stderr, "Error importing GIF image\n");
    fclose(fp);
    return;
  }

  fclose(fp);

  if (picinfo.comment)
    printf("GIF comment: %s\n", picinfo.comment);

  int screen = DefaultScreen(_display);
  int depth = DefaultDepth(_display,screen);
  Visual *visual = DefaultVisual(_display, screen);
  XImage *image = XCreateImage(_display, visual, depth, ZPixmap,
			       0, (char *)picinfo.pic,
			       picinfo.w, picinfo.h, 8, 0);
  DOASSERT(image, "Cannot create image");
  XPutImage(_display, DRAWABLE, _gc, image, 0, 0, (int)(x - picinfo.w / 2.0),
	    (int)(y - picinfo.h / 2.0), picinfo.w, picinfo.h);
  XDestroyImage(image);
}

/* export window image */

void XWindowRep::ExportImage(DisplayExportFormat format, char *filename)
{
  if (format == GIF) {
    ExportGIF(filename);
    return;
  }

  DOASSERT(_win, "Exporting a pixmap not supported yet");

  Window win = FindTopWindow(_win);
  XRaiseWindow(_display, win);

  char cmd[256];
  if (format == POSTSCRIPT || format == EPS) {
    sprintf(cmd,
       "xwd -frame -id %ld | xpr -device ps -portrait -compact -gray 4 > %s",
	    win, filename);
  } else {
    printf("Requested export format not supported yet.\n");
    return;
  }

#ifdef DEBUG
  printf("ExportImage: for window id 0x%lx:\n", win);
  printf("ExportImage: executing %s\n", cmd);
#endif

#if defined(SUN) || defined(HPUX) || defined(ULTRIX)
  int errorcode = 127;
#else
  int errorcode = -1;
#endif

  for(int attempt = 0; attempt < 10; attempt++) {
    int status = system(cmd);
#if defined(LINUX)
    if ((status != -1) && (status != 127))
#else
    if (status != errorcode)
#endif
      break;
    if (errno == EAGAIN && attempt < 10 - 1) {
      perror("system");
      fprintf(stderr, "Retrying to execute command in 1.0 seconds...\n");
      sleep(1);
    } else {
      fprintf(stderr, "Could not execute command (%d attempts):\n",
	      attempt + 1);
      fprintf(stderr, "  %s\n", cmd);
      perror("The reason is");
      break;
    }
  }
}

/* get geometry of root window enclosing this window */

void XWindowRep::GetRootGeometry(int &x, int &y,
                                 unsigned int &w, unsigned int &h)
{
  if (_pixmap) {
    Origin(x, y);
    Dimensions(w, h);
    return;
  }

  Window root = FindTopWindow(_win);
  Window dummy;
  unsigned int border_width, depth;
  XGetGeometry(_display, root, &dummy, &x, &y, &w, &h, &border_width,
               &depth);
#ifdef DEBUG
  printf("Geometry of root 0x%lx of XWin 0x%p is %d,%d,%u,%u\n",
         root, this, x, y, w, h);
#endif
}

/* find top window */

Window XWindowRep::FindTopWindow(Window win)
{
  Window parent, root;
  Window *children;
  unsigned int nchildren;
  if (!XQueryTree(_display, win, &root, &parent,
                  &children, &nchildren)) {
    DOASSERT(0, "XQueryTree failed");
  }
  if (children)
    XFree(children);

  /* if the root window (display) is the parent of the current 
     window, we've found the top enclosing window enclosing */

  if (parent == root)
    return win;

  return FindTopWindow(parent);
}

/* export image as GIF */

void XWindowRep::ExportGIF(char *filename)
{
  if (_win) {
    Window win = FindTopWindow(_win);
    XWindowAttributes xwa;
    if (!XGetWindowAttributes(_display, win, &xwa)) {
      fprintf(stderr, "Cannot get window attributes\n");
      return;
    }
    XRaiseWindow(_display, win);
    ((XDisplay *)GetDisplay())->ConvertAndWriteGIF(win, xwa, filename);
    return;
  }

  // Cannot get attributes of pixmap using XGetWindowAttributes
  // so let's get the attributes from the root window and then
  // patch xwa with the pixmap's information.

  XWindowAttributes xwa;
  if (!XGetWindowAttributes(_display, DefaultRootWindow(_display), &xwa)) {
    fprintf(stderr, "Cannot get window attributes\n");
    return;
  }

  xwa.x = _x;
  xwa.y = _y;
  xwa.width = _width;
  xwa.height = _height;

  // Wow we must copy all child windows to this window because they
  // are stored in separate (non-overlapping) pixmaps; we want the
  // dump to look the same as on screen where the windows would
  // overlap.

  CoalescePixmaps(this);

  ((XDisplay *)GetDisplay())->ConvertAndWriteGIF(_pixmap, xwa, filename);
}

void XWindowRep::CoalescePixmaps(XWindowRep *root)
{
  if (!root->GetPixmapId())
    return;

  // recursively copy the contents of subpixmaps onto the parent pixmap

  int index = root->_children.InitIterator();

  while(root->_children.More(index)) {

    XWindowRep *win = root->_children.Next(index);
    if (!win->GetPixmapId())
      continue;

    CoalescePixmaps(win);

#ifdef DEBUG
    printf("Copying from 0x%p to 0x%p, width %u, height %u, x %d, y %d\n",
	   win, root, win->_width, win->_height, win->_x, win->_y);
#endif
    XCopyArea(root->_display, win->GetPixmapId(), root->GetPixmapId(),
	      root->_gc, 0, 0, win->_width, win->_height, win->_x, win->_y);
  }

  root->_children.DoneIterator(index);
}

/* color selection interface using Devise colormap */

void XWindowRep::SetFgColor(Color fg)
{
  WindowRep::SetFgColor(fg);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetForeground(_display, _gc, WindowRep::GetLocalColor(fg));
#endif
}

void XWindowRep::SetBgColor(Color bg)
{
  WindowRep::SetBgColor(bg);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetBackground(_display, _gc, WindowRep::GetLocalColor(bg));
#endif
}

void XWindowRep::SetWindowBgColor(Color bg)
{
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetWindowBackground(_display, _win, WindowRep::GetLocalColor(bg));
#endif
}

#ifdef LIBCS
/* color selection interface using local colors */

void XWindowRep::SetFgRGB(float r, float g, float b)
{
  _rgbForeground = GetDisplay()->FindLocalColor(r, g, b);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetForeground(_display, _gc, _rgbForeground);
#endif
}

void XWindowRep::SetBgRGB(float r, float g, float b)
{
  _rgbBackground = GetDisplay()->FindLocalColor(r, g, b);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetBackground(_display, _gc, _rgbBackground);
#endif
}

void XWindowRep::GetFgRGB(float &r, float &g, float &b)
{
  GetDisplay()->FindLocalColor(_rgbForeground, r, g, b);
#ifdef DEBUG
  printf("Current foreground color is %lu: %.2f,%.2f,%.2f\n",
         _rgbForeground, r, g, b);
#endif
}

void XWindowRep::GetBgRGB(float &r, float &g, float &b)
{
  GetDisplay()->FindLocalColor(_rgbBackground, r, g, b);
#ifdef DEBUG
  printf("Current background color is %lu: %.2f,%.2f,%.2f\n",
         _rgbBackground, r, g, b);
#endif
}

void XWindowRep::SetWindowBgRGB(float r, float g, float b)
{
  Color color = GetDisplay()->FindLocalColor(r, g, b);
#ifdef GRAPHICS
  if (_dispGraphics)
    XSetWindowBackground(_display, _win, color);
#endif
}
#endif

/* drawing primitives */

void XWindowRep::DrawPixel(Coord x, Coord y)
{
  Coord tx, ty;
  WindowRep::Transform(x,y,tx,ty);

#ifdef DEBUG
  printf("XWindowRep::DrawPixel: %.2f %.2f --> %.2f %.2f\n", x, y, tx, ty);
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XDrawPoint(_display, DRAWABLE, _gc, ROUND(int, tx), ROUND(int, ty));
#endif
}

static struct XPoint points[WINDOWREP_BATCH_SIZE];
static XRectangle rectAngles[WINDOWREP_BATCH_SIZE];

void XWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("XWindowRep::DrawPixelArray: %d points, width = %d\n", num, width);

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
      XDrawPoints(_display, DRAWABLE, _gc, points, num, CoordModeOrigin);
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
    XFillRectangles(_display, DRAWABLE, _gc, rectAngles, num);
#endif
}

/* Fill rectangles, variable width/height */

void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			       Coord *height, int num)
{
#ifdef DEBUG
  printf("XWindowRep::FillRectArray: %d points\n", num);

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
  for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 6 == 0)
      printf("\n");
    printf("(%d,%d,%d,%d)", rectAngles[k].x, rectAngles[k].y,
	   rectAngles[k].width, rectAngles[k].height);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillRectangles(_display, DRAWABLE, _gc, rectAngles, num);
#endif
}

/* Fill rectangles, same width/height */

void XWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord width,
			       Coord height, int num)
{
#ifdef DEBUG
  printf("XWindowRep::FillRectArray: %d points, width %.2f, height %.2f\n",
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
  if (_dispGraphics)
    XFillRectangles(_display, DRAWABLE, _gc, rectAngles, num);
#endif
}

void XWindowRep::FillRect(Coord xlow, Coord ylow, Coord width, 
			  Coord height)
{
#ifdef DEBUG
  printf("XWindowRep::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
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
  if (_dispGraphics)
    XFillRectangle(_display, DRAWABLE, _gc, ROUND(int, txlow),
		   ROUND(int, tylow), pixelWidth, pixelHeight);
#endif
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */

void XWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			       Coord minWidth, Coord minHeight)
{
#ifdef DEBUG
  printf("XWindowRep::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",
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
  if (_dispGraphics)
    XFillRectangle(_display, DRAWABLE, _gc, pixelX, pixelY, 
		   pixelWidth, pixelHeight);
#endif
}

/*************************************************************************
XXX: need to clip polygon against the window because a large polygon
can overlow the window's integer coordinate system.
*************************************************************************/

void XWindowRep::FillPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("XwindowRep::FillPoly: %d points\n", n);

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
  for(k = 0; k < (n > MAXPIXELDUMP ? MAXPIXELDUMP : n); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%d,%d)", points[k].x, points[k].y);
  }
  printf("\n");
#endif
#endif

#ifdef GRAPHICS
  if (_dispGraphics)
    XFillPolygon(_display, DRAWABLE, _gc, points, n,
                 Nonconvex, CoordModeOrigin);
#endif
}

/*************************************************************************
Draw polygon, given PIXEL coordinates of the corners of the polygon.
No transformation of the coordinates is done.
*************************************************************************/

void XWindowRep::FillPixelPoly(Point *pts, int n)
{
#ifdef DEBUG
  printf("XwindowRep::FillPixelPoly: %d points\n",n);

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
  if (_dispGraphics)
    XFillPolygon(_display, DRAWABLE, _gc, points, n,
                 Nonconvex, CoordModeOrigin);
#endif
}

/********************************************************************/

void XWindowRep::Arc(Coord x, Coord y, Coord w, Coord h,
		     Coord startAngle, Coord endAngle)
{
#ifdef DEBUG
  printf("XWindowRep::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
	 x, y, w, h, startAngle, endAngle);
#endif
  
  Coord tx, ty, tempX, tempY;
  WindowRep::Transform(x, y, tx, ty);
  WindowRep::Transform(x + w, y + h, tempX, tempY);
  Coord width = fabs(tempX - tx);
  Coord height = fabs(tempY - ty);
  int realStart = ROUND(int, ToDegree(startAngle) * 64);
  int realEnd = ROUND(int, ToDegree(endAngle) * 64);
#ifdef GRAPHICS
  if (_dispGraphics) {
    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinRound);
    XDrawArc(_display, DRAWABLE, _gc, ROUND(int, tx - width / 2),
             ROUND(int, ty - height / 2), ROUND(int, width),
             ROUND(int, height), realStart, realEnd);
  }
#endif
}

/***********************************************************************/

void XWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, 
		      Coord width)
{
#ifdef DEBUG
  printf("XWindowRep::Line %.2f,%.2f,%.2f,%.2f\n", x1, y1, x2, y2);
#endif
  
  Coord tx1, ty1, tx2, ty2;
  WindowRep::Transform(x1 ,y1, tx1, ty1);
  WindowRep::Transform(x2, y2, tx2, ty2);
#ifdef GRAPHICS
  if (_dispGraphics) {
    XSetLineAttributes(_display, _gc, ROUND(int, width), LineSolid, CapButt,
		       JoinRound);
    XDrawLine(_display, DRAWABLE, _gc, ROUND(int, tx1), ROUND(int, ty1),
	      ROUND(int, tx2), ROUND(int, ty2));
    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinMiter);
  }
#endif
}

void XWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
#ifdef DEBUG
  printf("XWindowRep::AbsoluteLine %d,%d,%d,%d\n", x1, y1, x2, y2);
#endif
  
#ifdef GRAPHICS
  if (_dispGraphics) {
    XSetLineAttributes(_display, _gc, ROUND(int, width), LineSolid, CapButt,
		       JoinRound);
    XDrawLine(_display, DRAWABLE, _gc, x1, y1, x2, y2);
    XSetLineAttributes(_display, _gc, 0, LineSolid, CapButt, JoinMiter);
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
  return 0;
}

/**************************************************************
Draw rubberband
****************************************************************/

void XWindowRep::DrawRubberband(int x1, int y1, int x2, int y2)
{
  DOASSERT(_win, "Cannot draw rubberband in pixmap");

  int minX = MIN(x1, x2);
  int minY = MIN(y1, y2);
  int maxX = MAX(x1, x2);
  int maxY = MAX(y1, y2);
  unsigned width = maxX - minX;
  unsigned height = maxY - minY;
  
  XDrawRectangle(_display, _win, _rectGc, minX, minY, width, height);
}

#ifndef RAWMOUSEEVENTS
static long buttonMasks[3] = {
  Button1MotionMask, Button2MotionMask, Button3MotionMask
};

/*********************************************************************
Handle button press event. Return the region covered by
the selection in window coordinates. 
***********************************************************************/

void XWindowRep::DoButtonPress(int x, int y, int &xlow, int &ylow, 
			       int &xhigh, int &yhigh, int button)
{
  DOASSERT(_win, "Cannot handle button press in pixmap");

  /* grab server */
  XGrabServer(_display);
  int x1,x2;
  x1 = x2 = x;
  int y1,y2;
  y1 = y2 = y;

  /* draw rubberband rectangle */
  DrawRubberband(x1, y1, x2, y2);
  
  Boolean done = false;
  long buttonMask = buttonMasks[button - 1];
  while(!done) {
    XEvent event;
    XWindowEvent(_display, _win, ButtonReleaseMask | buttonMask, &event);
    switch(event.xany.type) {
    case ButtonRelease:
      if (event.xbutton.button == (unsigned int)button) {
	/* final button position */
	DrawRubberband(x1,y1,x2,y2);
	
	x2 = event.xbutton.x;
	y2 = event.xbutton.y;
	done = true;
      }
      break;
    case MotionNotify:
      DrawRubberband(x1, y1, x2, y2);
      
      /* get rid of all remaining motion events */
      do {
	x2 = event.xmotion.x;
	y2 = event.xmotion.y;
      } while(XCheckWindowEvent(_display,_win, buttonMask, &event));
      
      DrawRubberband(x1, y1, x2, y2);
      break;
    }
  }
  
  xlow = MIN(x1, x2);
  ylow = MIN(y1, y2);
  xhigh = MAX(x1, x2);
  yhigh = MAX(y1, y2);
  
  if (xhigh - xlow <= 5 || yhigh - ylow <= 5) {
    xhigh = xlow;
    yhigh = ylow;
  }
  
  /* ungrab server */
  XUngrabServer(_display);
  XSync(_display,false);
}
#endif

/***********************************************************************
Handle the next X event
************************************************************************/

void XWindowRep::HandleEvent(XEvent &event)
{
  DOASSERT(_win, "Cannot handle events for pixmap");

  char buf[40];
  XEvent ev;
  int count;
  Atom protocol;

  switch(event.xany.type) {
  case KeyPress:
    KeySym keysym;
    XComposeStatus compose;
    count = XLookupString((XKeyEvent *)&event, buf, 40, &keysym, &compose);
    if (count == 1) {
      /* regular key */
      WindowRep::HandleKey(buf[0], event.xkey.x, event.xkey.y);
    }
    break;

#ifdef RAWMOUSEEVENTS
  case ButtonPress:
  case ButtonRelease:
  case MotionNotify:
    WindowRep::HandleButton(event.xbutton.x, event.xbutton.y,
			    event.xbutton.button, event.xbutton.state,
			    event.xbutton.type);
    break;
#else
  case ButtonPress:
    int buttonXlow, buttonYlow, buttonXhigh, buttonYhigh;
    if (event.xbutton.button == 2) {
      /* handle popup */
      DoPopup(event.xbutton.x, event.xbutton.y, event.xbutton.button);
    } else if (event.xbutton.button <= 3) {
      DoButtonPress(event.xbutton.x, event.xbutton.y,
		    buttonXlow, buttonYlow, buttonXhigh, buttonYhigh,
		    event.xbutton.button);
      
      /*
	 if (buttonXhigh > buttonXlow && buttonYhigh > buttonYlow) {
      */
      WindowRep::HandleButtonPress(buttonXlow, buttonYlow,
				   buttonXhigh, buttonYhigh,
				   event.xbutton.button);
    }
    break;

  case ButtonRelease:
  case MotionNotify:
    break;
#endif

  case Expose:
    Coord minX, minY, maxX, maxY;
    minX = (Coord)event.xexpose.x;
    minY = (Coord)event.xexpose.y;
    maxX = minX + (Coord)event.xexpose.width - 1;
    maxY = minY + (Coord)event.xexpose.height - 1;
#ifdef DEBUG
    printf("XWindowRep 0x%p Exposed %d,%d to %d,%d\n", this,
	   (int)minX, (int)minY, (int)maxX, (int)maxY);
#endif
    while (XCheckIfEvent(_display, &ev, check_expose, (char *)&_win)) {
      /* merge next expose event with current one to form
	 a bigger expose region */
#ifdef DEBUG
      printf("Merging expose with other exposure: %d,%d,%d,%d\n",
	     (int)ev.xexpose.x, (int)ev.xexpose.y,
	     (int)(ev.xexpose.x + ev.xexpose.width - 1),
	     (int)(ev.xexpose.y + ev.xexpose.height - 1));
#endif
      Geom::MergeRects(minX, minY, maxX, maxY,
		       (Coord)ev.xexpose.x, (Coord)ev.xexpose.y,
		       (Coord)(ev.xexpose.x + ev.xexpose.width - 1),
		       (Coord)(ev.xexpose.y + ev.xexpose.height - 1));
    }
    WindowRep::HandleExpose((int)minX, (int)minY,
			    (unsigned)(maxX - minX + 1), 
			    (unsigned)(maxY - minY + 1));
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
#ifdef DEBUG
    printf("XWindowRep 0x%p ConfigureNotify: %d,%d,%d,%d\n", this,
	   _x, _y, _width, _height);
#endif
    if (_x != saveX || _y != saveY || _width != saveWidth
	|| _height != saveHeight) {
      /* There is a real change in size */
      WindowRep::HandleResize(_x, _y, _width, _height);
    }
    break;

  case VisibilityNotify:
    if (event.xvisibility.state == VisibilityUnobscured) {
      _unobscured = true;
#ifdef DEBUG
      printf("0x%p unobscured %d\n", this, _unobscured);
#endif
    }
    else if (event.xvisibility.state == VisibilityPartiallyObscured) {
      _unobscured = false;
#ifdef DEBUG
      printf("0x%p partially obscured\n",this);
#endif
    }
    else {
      _unobscured = false;
#ifdef DEBUG
      printf("0x%p obscured\n",this);
#endif
    }
    break;

  case MapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx mapped\n", event.xmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(true);
    break;

  case UnmapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx unmapped\n", event.xunmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(false);
    break;

  case ReparentNotify:
#ifdef DEBUG
    printf("XWin 0x%lx reparented\n", event.xunmap.window);
#endif
    break;

  case DestroyNotify:
#ifdef DEBUG
    printf("XWin 0x%lx destroyed\n", event.xdestroywindow.window);
#endif
    break;

  case ClientMessage:
    protocol = (Atom)event.xclient.data.l[0];
#ifdef DEBUG
    printf("XWin 0x%lx receives client message %ld\n",
           event.xclient.window, protocol);
#endif
    if (protocol == XInternAtom(_display, "WM_DELETE_WINDOW", False)) {
#ifdef DEBUG
      printf("Client message is WM_DELETE_WINDOW\n");
#endif
      WindowRep::HandleWindowDestroy();
    } else {
      printf("Unrecognized client message %ld for window 0x%lx\n",
             protocol, event.xclient.window);
    }
    break;

  default:
    printf("Unexpected event %d for window 0x%lx\n",
           event.xany.type, event.xany.window);
    break;
  }
}

/* Draw absolute text */

void XWindowRep::AbsoluteText(char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      TextAlignment alignment, 
			      Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("XWindowRep::AbsoluteText: %s at %.2f,%.2f,%.2f,%.2f\n",
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
  
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = fontStruct->max_bounds.ascent
                   + fontStruct->max_bounds.descent;
  
  if (textWidth > winWidth || textHeight > winHeight) {
    Text(text, x, y, width, height, alignment, skipLeadingSpace);
    return;
  }
  
  int startX = 0, startY = 0;
  int widthDiff = winWidth - textWidth;
  int halfWidthDiff = widthDiff / 2;
  int heightDiff = winHeight - textHeight;
  int halfHeightDiff = heightDiff / 2;

  switch(alignment) {
  case AlignNorthWest:
    startX = winX; 
    startY = winY + fontStruct->max_bounds.ascent;
    break;

  case AlignNorth:
    startX = winX + halfWidthDiff; 
    startY = winY + fontStruct->max_bounds.ascent;
    break;

  case AlignNorthEast:
    startX = winX + widthDiff; 
    startY = winY + fontStruct->max_bounds.ascent;
    break;

  case AlignWest: 
    startX = winX; 
    startY = winY + halfHeightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignCenter: 
    startX = winX + halfWidthDiff; 
    startY = winY + halfHeightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignEast:
    startX = winX + widthDiff; 
    startY = winY + halfHeightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignSouthWest:
    startX = winX; 
    startY = winY + heightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignSouth:
    startX = winX + halfWidthDiff; 
    startY = winY + heightDiff + fontStruct->max_bounds.ascent;
    break;

  case AlignSouthEast:
    startX = winX + widthDiff; 
    startY = winY + heightDiff + fontStruct->max_bounds.ascent;
    break;
  }
  
#ifdef DEBUG
  printf("Drawing %s starting at %d,%d\n", text, startX, startY);
#endif

#ifdef GRAPHICS
  XDrawString(_display, DRAWABLE, _gc, startX, startY, text, textLength);
#endif
}

/* Draw scale text */

void XWindowRep::Text(char *text, Coord x, Coord y, Coord width, Coord height,
		      TextAlignment alignment, Boolean skipLeadingSpace)
{
#ifdef DEBUG
  printf("XWindowRep::Text: %s at %.2f,%.2f,%.2f,%.2f\n",
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
  if (textLength == 0) return;

  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  int textWidth = XTextWidth(fontStruct, text, textLength);
  int textHeight = fontStruct->max_bounds.ascent
                   + fontStruct->max_bounds.descent;
  
  if (textWidth > _srcBitmap.width || textHeight > _srcBitmap.height)
    /* allocate a bigger source pixmap */
    AllocBitmap(_srcBitmap, textWidth, textHeight);
  
  /* use original text to calculate scale factor 
     to scale text to fit in rectangle of 
     dimensions (winWidth, winHeight)*/
  double xscale = (double)winWidth / (double)textWidth;
  double yscale = (double)winHeight / (double)textHeight;
  double scale = MIN(xscale, yscale);
  
#ifdef DEBUG
  printf("transformed to x=%d,y=%d,w=%d,h=%d\n", winX, winY,
	 winWidth, winHeight);
  printf("textwidth = %d, textHeight = %d\n", textWidth, textHeight);
#endif

  /* draw text in source bitmap */
  XDrawImageString(_display, _srcBitmap.pixmap, _srcBitmap.gc, 
		   0, fontStruct->max_bounds.ascent, text, textLength);
  
  /* scale into dest bitmap */
  int dstWidth = ROUND(int, textWidth * scale);
  int dstHeight = ROUND(int, textHeight * scale);

#ifdef DEBUG
  printf("scale= %.2f, textWidth = %d, textHeight = %d, dstw=%d, dstH = %d\n",
	 scale, textWidth, textHeight, dstWidth, dstHeight);
  printf("dstwidth = %d, dstHeight = %d\n", dstWidth, dstHeight);
#endif

  CopyBitmap(textWidth, textHeight, dstWidth, dstHeight);
  
  /* draw the text according to alignment */
  int startX = 0, startY = 0;
  int widthDiff = winWidth - dstWidth;
  int halfWidthDiff = widthDiff / 2;
  int heightDiff = winHeight - dstHeight;
  int halfHeightDiff = heightDiff / 2;

  switch(alignment) {
  case AlignNorthWest:
    startX = winX; startY = winY;
    break;

  case AlignNorth:
    startX = winX + halfWidthDiff; startY = winY;
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
    startX = winX + widthDiff; startY = winY + halfHeightDiff;
    break;

  case AlignSouthWest:
    startX = winX; startY = winY+ heightDiff;
    break;

  case AlignSouth:
    startX = winX + halfWidthDiff; startY = winY+heightDiff;
    break;
    
  case AlignSouthEast:
    startX = winX + widthDiff; startY = winY+heightDiff;
    break;
  }
  
  XCopyPlane(_display, _dstBitmap.pixmap, DRAWABLE, _gc, 
	     0, 0, dstWidth, dstHeight, startX, startY, 1);
}

void XWindowRep::SetXorMode()
{
#ifdef DEBUG
  printf("XWindowRep::SetXorMode\n");
#endif

#ifdef GRAPHICS
  XSetState(_display, _gc, AllPlanes, AllPlanes, GXxor, AllPlanes);
#endif
}

void XWindowRep::SetCopyMode()
{
#ifdef DEBUG
  printf("XWindowRep::SetCopyMode\n");
#endif

#ifdef GRAPHICS
  XSetState(_display, _gc,
            ((XDisplay *)GetDisplay())->GetLocalColor(GetFgColor()),
	    ((XDisplay *)GetDisplay())->GetLocalColor(GetBgColor()),
            GXcopy, AllPlanes);
#endif
}

void XWindowRep::SetNormalFont()
{
  ((XDisplay *)GetDisplay())->SetNormalFont();
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  XSetFont(_display, _gc, fontStruct->fid);
}

void XWindowRep::SetSmallFont()
{
  ((XDisplay *)GetDisplay())->SetSmallFont();
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetFontStruct();
  XSetFont(_display, _gc, fontStruct->fid);
}

int XWindowRep::GetSmallFontHeight()
{
  XFontStruct *fontStruct = ((XDisplay *)GetDisplay())->GetSmallFontStruct();
  return fontStruct->max_bounds.ascent + fontStruct->max_bounds.descent;
}

/**********************************************************************
Scale and copy the rectangle (0,0,width,height) in _srcBitmap
into _dstBitmap (0,0,dstWidth, dstHeight). X windows coords are used.
***********************************************************************/

void XWindowRep::CopyBitmap(int width,int height,int dstWidth,int dstHeight)
{
#ifdef DEBUG
  printf("XWindowRep::CopyBitmap: %d,%d,%d,%d\n", width, height, dstWidth, dstHeight);
#endif

  if (dstWidth > _dstBitmap.width || dstHeight > _dstBitmap.height)
    /* allocate a bigger pixmap */
    AllocBitmap(_dstBitmap, dstWidth, dstHeight);

  /* copy dst from src */
  int widthMinus1 = width - 1;
  int heightMinus1 = height - 1;

  int dstWidthMinus1 = dstWidth;
  if (dstWidth > 1)
    dstWidthMinus1 = dstWidth - 1;

  int dstHeightMinus1 = dstHeight;
  if (dstHeight > 1)
    dstHeightMinus1 = dstHeight - 1;
  
  XImage *srcImage = XGetImage(_display, _srcBitmap.pixmap, 
			       0, 0, width, height, 1, XYPixmap);
  
  for (int y = 0; y < dstHeight; y++) {
    for (int x = 0; x < dstWidth; x++) {
      XPutPixel(_dstBitmap.image, x, y,
		XGetPixel(srcImage,
			  x * widthMinus1 / dstWidthMinus1,
			  y * heightMinus1 / dstHeightMinus1));
    }
  }
  XPutImage(_display, _dstBitmap.pixmap, _dstBitmap.gc, 
	    _dstBitmap.image, 0, 0, 0, 0,
	    ROUND(unsigned int, dstWidth),
	    ROUND(unsigned int, dstHeight));
  XDestroyImage(srcImage);
}

/***************************************************************
Set current pattern
****************************************************************/

void XWindowRep::SetPattern(Pattern p)
{
  if (WindowRep::GetPattern() != p) {
    WindowRep::SetPattern(p);
    if (p == Pattern0) {
      /* pattern0 is solid */
#ifdef GRAPHICS
      if (_dispGraphics)
	XSetFillStyle(_display, _gc, FillSolid);
#endif
    } else {
#ifdef GRAPHICS
      if (_dispGraphics) {
	XSetFillStyle(_display, _gc, FillOpaqueStippled);
	XSetStipple(_display, _gc,
	     ((XDisplay *)WindowRep::GetDisplay())->GetPatternStipple(p));
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
  info.pixmap = XCreatePixmap(_display, DRAWABLE, width, height, 1);
  DOASSERT(info.pixmap, "Cannot create pixmap");
  info.gc = XCreateGC(_display, info.pixmap, 0, NULL);
  
  /* set foreground to 1 and background to 0 for later XCopyPlane() */
  XSetState(_display, info.gc, 1, 0, GXcopy, AllPlanes);
  
  XSetFont(_display,info.gc,
	   ((XDisplay *)WindowRep::GetDisplay())->GetFontStruct()->fid);
  info.image = XGetImage(_display, info.pixmap, 0, 0, width, height,
			 1, XYPixmap);
}

/* Get Window dimensions */

void XWindowRep::UpdateWinDimensions()
{
  Window root;
  unsigned int border_width, depth;

  if (_win) {
    XGetGeometry(_display, _win, &root, &_x, &_y, &_width, &_height,
		 &border_width, &depth);
  } else {
    // pixmaps don't have position information so XGetGeometry returns
    // X = 0 and Y = 0... just ignore them
    int dummyX, dummyY;
    XGetGeometry(_display, _pixmap, &root, &dummyX, &dummyY, &_width, &_height,
		 &border_width, &depth);
  }
}

/* Get window rep dimensions */

void XWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

/* Get window rep origin */

void XWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

void XWindowRep::AbsoluteOrigin(int &x, int &y)
{
  DOASSERT(_win, "Cannot get screen position of pixmap");

  /* Find the offset from root window */
  x = 0;
  y = 0;
  
  Window current, root;
  Window parent = _win;
  do {
    current = parent;
    
    /* add distace to parent */
    Window winRoot;
    int winX, winY;
    unsigned winW, winH, winBorder, winDepth;
    XGetGeometry(_display, current, &winRoot, &winX, &winY,
		 &winW, &winH, &winBorder, &winDepth);
    x += winX;
    y += winY;
    
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(_display, current, &root, &parent,
		    &children, &nchildren)) {
      DOASSERT(0, "XQueryTree failed");
    }
    if (children != NULL)
      XFree(children);
  } while (parent != root);
}

/* Move and resize window, relative to the parent */

void XWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("Moving XWindowRep 0x%p to %d,%d, size %u,%u\n", this,
	 x, y, w, h);
#endif

  if (_win) {
    /* Tell X to move/resize window. We will be notified by an event
       when it's done */
    XMoveResizeWindow(_display, _win, x, y, w, h);
  } else {
    /* Resizing a pixmap involves deleting it and creating a new one */
    XFreePixmap(_display, _pixmap);
    unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
    _pixmap = XCreatePixmap(_display, DefaultRootWindow(_display),
			    w, h, depth);
    DOASSERT(_pixmap, "Cannot create pixmap");
    _x = x;
    _y = y;

    // clear all pixels in pixmap
    SetFgColor(GetBgColor());
    XFillRectangle(_display, _pixmap, _gc, 0, 0, w, h);
  }

  UpdateWinDimensions();
}

/* Iconify window. Not guaranteed to succeed. */

void XWindowRep::Iconify()
{
  DOASSERT(_win, "Cannot iconify pixmap");
  XIconifyWindow(_display, _win, 0);
}

#ifndef RAWMOUSEEVENTS
void XWindowRep::DoPopup(int x, int y, int button)
{
  DOASSERT(_win, "Cannot display pop-up window in pixmap");

  char **msgs;
  int numMsgs;
  if (!HandlePopUp(x, y, button, msgs, numMsgs) || numMsgs <= 0)
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
    textWidth = MAX(textWidth,
			    XTextWidth(fontStruct, msgs[i], strlen(msgs[i])));
  }
  textHeight = charHeight * numMsgs;
  
  long bgnd = GetLocalColor(BackgroundColor);
  long fgnd = GetLocalColor(ForegroundColor);

  /* Create window */
  XSetWindowAttributes attr;
  attr.background_pixmap	= None;
  attr.background_pixel		= bgnd;
  attr.border_pixmap		= CopyFromParent;
  attr.border_pixel		= fgnd;
  attr.bit_gravity		= ForgetGravity /* CenterGravity */;
  attr.win_gravity		= NorthWestGravity;
  attr.backing_store		= Always /* NotUseful WhenMapped */;
  attr.backing_planes		= AllPlanes;
  attr.backing_pixel		= 0;
  attr.save_under		= True;
  attr.event_mask		= 0;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect	= True;
  attr.colormap			= DefaultColormap(_display,
						  DefaultScreen(_display));
  attr.cursor			= None;
  
  /* Create the window. */
  Window win;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  int absX, absY;
  AbsoluteOrigin(absX, absY);
  x += absX;
  y += absY;

  /* Make bottom right of window appear at cursor position */
  textWidth +=  2; /* allow for border */
  textHeight += 2; /* allow for border */
  x = x - textWidth + 1;
  y = y - textHeight + 1;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  if ((win = XCreateWindow(_display, DefaultRootWindow(_display), x, y,
			   textWidth, textHeight, 1, 0, InputOutput,
			   CopyFromParent, AllPlanes, &attr)) == 0) {
    DOASSERT(0, "Cannot create popup window");
  }
  
  XSelectInput(_display, win, ExposureMask | ButtonPressMask);
  
  /* Map the window so that it appears on the display. */
  if (XMapWindow(_display, win) < 0) {
    DOASSERT(0, "Cannot map popup window");
  }
  
  /* Do a sync to force the window to appear immediately */
  XSync(_display, false);
  
  XGCValues gcVal;
  gcVal.function = GXcopy;
  gcVal.plane_mask = AllPlanes;
  gcVal.foreground = fgnd;
  gcVal.background = bgnd;
  gcVal.font = fontStruct->fid;
  GC popUpGc = XCreateGC(_display, win, GCFunction | GCPlaneMask
			 | GCForeground | GCBackground | GCFont,
			 &gcVal);
  /*
     XSetState(_display, popUpGc, fgnd, bgnd, GXcopy,AllPlanes);
  */
  
#ifdef LIBCS
  Boolean savePopup = false;
#else
  Boolean savePopup = Init::SavePopup();
#endif
  if (!savePopup)
    XGrabServer(_display);

  while(1) {
    XEvent event;
    /* check parent for button release */
    if (savePopup) {
      if (XCheckWindowEvent(_display, win, ButtonPressMask, &event)) {
	/* done */
	XDestroyWindow(_display, win);
	break;
      }
    } else {
      if (XCheckWindowEvent(_display, _win, ButtonReleaseMask, &event)) {
	if (event.xbutton.button == (unsigned int)button) {
	  /* done */
	  XDestroyWindow(_display, win);
	  break;
	}
      }
    }
    
    /* check popUp window for exposure */
    if (XCheckWindowEvent(_display, win, ExposureMask, &event)) {
      /* Draw the text messages into window */
      for(int i = 0; i < numMsgs; i++) {
	int startY = charHeight * i;
	XDrawString(_display, win, popUpGc, 1, startY + charAscent, 
		    msgs[i], strlen(msgs[i]));
      }
    }
  }

  XFreeGC(_display, popUpGc);
  if (!savePopup)
    XUngrabServer(_display);
  XSync(_display,false);
}
#endif

Boolean XWindowRep::Scrollable()
{
  /* window is crollable if it has a backing store or
     if it is not obscured */
  /*
     printf("XWindowRep::Scrollable: 0x%p backingstore: %d, unobscured %d\n",
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
	 x, y, w, h, dstX, dstY);
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
  
  Coord tdx, tdy;
  WindowRep::Transform(dstX, dstY + h, tdx, tdy);
  
#ifdef DEBUG
  printf("XWindowRep::Scroll after xform x:%d,y:%d,w:%d,h:%d to x:%d, y%d\n",
	 ROUND(int, xlow), ROUND(int, ylow), ROUND(int, width),
	 ROUND(int, height), ROUND(int, tdx), ROUND(int, tdy));
#endif
  
  XCopyArea(_display, DRAWABLE, DRAWABLE, _gc, ROUND(int, xlow),
	    ROUND(int, ylow), ROUND(unsigned int, width),
	    ROUND(unsigned int, height), ROUND(int, tdx), ROUND(int, tdy));
}

void XWindowRep::ScrollAbsolute(int x, int y, unsigned width, unsigned height,
				int dstX, int dstY)
{
#ifdef DEBUG
  printf("XWindowRep::ScrollAbsolute(x:%d,y:%d,w:%d,h:%d,dx:%d,dy:%d)\n",
	 x, y, width, height, dstX, dstY);
#endif

  XCopyArea(_display, DRAWABLE, DRAWABLE, _gc, x, y, width, height,
	    dstX, dstY);
}

/* Raise window to top of stacking order */

void XWindowRep::Raise()
{
#ifdef DEBUG
  printf("XWindowRep::Raise window 0x%lx:\n", _win);
#endif

  if (_win)
    XRaiseWindow(_display, _win);

  if (!_parent)
    return;

  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Append(this);
}

/* Lower window to bottom of stacking order */

void XWindowRep::Lower()
{
#ifdef DEBUG
  printf("XWindowRep::Lower window 0x%lx:\n", _win);
#endif

  if (_win)
    XLowerWindow(_display, _win);

  if (!_parent)
    return;

  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Insert(this);
}

/* Flush windowRep's content to display */

void XWindowRep::Flush()
{
  /* Do a sync to force output */
  XSync(_display, false);
}

static const int MAX_PIXMAP_BUF_SIZE = 256 * 1024;
static char _pixmapBuf[MAX_PIXMAP_BUF_SIZE];

/* Get current window pixmap */

DevisePixmap *XWindowRep::GetPixmap()
{
#ifdef DEBUG
  printf("XWindowRep::GetPixmap()\n");
#endif

  /* see if window is entirely visible on screen or not (XGetImage
     fails if window is partially off-screen */

  int dummyX, dummyY;
  unsigned int width, height;
  Coord screenW, screenH;
  GetRootGeometry(dummyX, dummyY, width, height);
  GetDisplay()->Dimensions(screenW, screenH);
#ifdef DEBUG
  printf("XWin 0x%p is %d,%d,%u,%u, screen is %.2f,%.2f\n",
         this, dummyX, dummyY, width, height, screenW, screenH);
#endif

  if (dummyX < 0 || dummyX + width - 1 > screenW ||
      dummyY < 0 || dummyY + height - 1 > screenH) {
#ifdef DEBUG
    printf("Window partially off-screen\n");
#endif
    return NULL;
  }
  
  Window root;
  unsigned int border_width, depth;
  XGetGeometry(_display, DRAWABLE, &root, &dummyX, &dummyY, &width, &height,
	       &border_width, &depth);
  
  DevisePixmap *pixmap = new DevisePixmap;
  DOASSERT(pixmap, "Out of memory");
  pixmap->width = width;
  pixmap->height = height;
  Boolean initPixmap = true;
  int outIndex = 0;

  for(int i = 0; i < (int)height; i++) {
    XImage *image = XGetImage(_display, DRAWABLE, 0, i, width, 1,
                              AllPlanes, ZPixmap);
    if (!image) {
        delete pixmap;
      return NULL;
    }

    if (initPixmap) {
      pixmap->imageBytes = image->bytes_per_line * height;
      pixmap->bytes_per_line = image->bytes_per_line;
      pixmap->padding = image->bitmap_pad;
      pixmap->data = (unsigned char *)image->data;
      initPixmap = false;
    }
    int outCount;
    char *cBuf = _compress->CompressLine(image->data, image->bytes_per_line,
					 outCount);
    XDestroyImage(image);
    if (outCount + outIndex + sizeof(int) > MAX_PIXMAP_BUF_SIZE ) {
      /* no more buffer */
      delete pixmap;
      return NULL;
    }
    memcpy(&_pixmapBuf[outIndex], (char *)&outCount, sizeof(int));
    outIndex += sizeof(int);
    memcpy(&_pixmapBuf[outIndex], cBuf, outCount);
    outIndex += outCount;
  }
  
  pixmap->compressedBytes = outIndex;
  pixmap->data = (unsigned char *)malloc(outIndex);
  if (!pixmap->data) {
    delete pixmap;
    return NULL;
  }
  memcpy(pixmap->data, _pixmapBuf, outIndex);

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
  Visual *visual = DefaultVisual(_display, screen);
  
  int index = 0;
  unsigned char *data = pixmap->data;
  for(int i = 0; i < pixmap->height; i++) {
    int count;
    int outCount;
    memcpy((char *)&count, (char *)&data[index], sizeof(int));
    index += sizeof(int);
    DOASSERT(index + count <= pixmap->compressedBytes,
	     "Invalid pixmap format");
    char *buf = _compress->DecompressLine((char *)&data[index],
					  count, outCount);
    index += count;
    DOASSERT(outCount == pixmap->bytes_per_line, "Invalid pixmap format");
    XImage *image = XCreateImage(_display, visual, depth, ZPixmap,
				 0, buf, pixmap->width, 1, pixmap->padding, 0);
    DOASSERT(image, "Cannot create image");
    XPutImage(_display, DRAWABLE, _gc, image, 0, 0, 0, i, image->width, 1);
    image->data = NULL;
    XDestroyImage(image);
  }
}

void XWindowRep::FreePixmap(DevisePixmap *pixmap)
{
  free(pixmap->data);
  delete pixmap;
}

#ifdef TK_WINDOW_old
void XWindowRep::Decorate(WindowRep *parent, char *name,
			  unsigned int min_width, unsigned int min_height)
{
  DOASSERT(!isInTkWindow(), "Invalid Tk window");
  EmbedInTkWindow((XWindowRep *)parent, name, min_width, min_height);
}

void XWindowRep::Undecorate()
{
  if (isInTkWindow())
    DetachFromTkWindow();
}

static void HandleTkEvents(ClientData win, XEvent *eventPtr)
{
#ifdef DEBUG
  printf("HandleTkEvents: received event %d\n", eventPtr->type);
#endif

  if (eventPtr->type != ConfigureNotify)
    return;

  ((XWindowRep *)win)->TkWindowSizeChanged();
}

void XWindowRep::TkWindowSizeChanged()
{
  extern Tcl_Interp *ControlPanelTclInterp;

  // first find out new size of Tk window

  DOASSERT(_tkWindow, "Invalid Tk window");
  unsigned int w = (unsigned int)Tk_Width(_tkWindow);
  unsigned int h = (unsigned int)Tk_Height(_tkWindow);

  // adjust with margins

  int x = _leftMargin;
  w -= _leftMargin + _rightMargin;
  int y = _topMargin;
  h -= _topMargin + _bottomMargin;

  MoveResize(x, y, w, h);
  WindowRep::HandleResize(x, y, w, h);

  char cmd[256];
  sprintf(cmd, "ResizeTkDataWindow %s %u %u", _tkPathName, w, h);
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot resize Tk window");
}

static const unsigned int TkRootLeftMargin   = 0;
static const unsigned int TkRootRightMargin  = 0;
static const unsigned int TkRootTopMargin    = 0;
static const unsigned int TkRootBottomMargin = 20;

static const unsigned int TkViewLeftMargin   = 40;
static const unsigned int TkViewRightMargin  = 0;
static const unsigned int TkViewTopMargin    = 0;
static const unsigned int TkViewBottomMargin = 20;

void XWindowRep::EmbedInTkWindow(XWindowRep *parent,
				 char *name,
				 unsigned int min_width,
				 unsigned int min_height)
{
  DOASSERT(_win, "Cannot embed pixmap in Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;
  extern Tk_Window ControlPanelMainWindow;

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // figure out the correct margins for this type of window

  // default: root data display window

  _leftMargin   = TkRootLeftMargin;
  _rightMargin  = TkRootRightMargin;
  _topMargin    = TkRootTopMargin;
  _bottomMargin = TkRootBottomMargin;

  // for views: left and bottom margin

  if (parent) {
    _leftMargin   = TkViewLeftMargin;
    _rightMargin  = TkViewRightMargin;
    _topMargin    = TkViewTopMargin;
    _bottomMargin = TkViewBottomMargin;
  }

  static int tkWinCount = 1;
  char cmd[256];
  sprintf(_tkPathName, ".devisewin%d", tkWinCount++);
  sprintf(cmd, "CreateTkDataWindow %s {%s} %d %d %u %u %u %u %u %u %u %u",
	  _tkPathName, name, x, y, w, h, _leftMargin,
	  _rightMargin, _topMargin, _bottomMargin,
	  min_width, min_height);
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot create Tk window");

  _tkWindow = Tk_NameToWindow(ControlPanelTclInterp,
			      _tkPathName,
			      ControlPanelMainWindow);
  DOASSERT(_tkWindow, "Cannot get name of Tk window");

#ifdef DEBUG
  printf("Created %s, id 0x%p, X id 0x%p, at %d,%d size %u,%u\n",
	 _tkPathName, _tkWindow, Tk_WindowId(_tkWindow),
	 x, y, w, h);
#endif

  unsigned long mask = StructureNotifyMask;
  Tk_CreateEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // first make this window a child of the new Tk window

#ifdef DEBUG
  printf("XWindowRep::Reparenting 0x%p to 0x%p at %d,%d\n",
	 _win, Tk_WindowId(_tkWindow), _leftMargin, _topMargin);
#endif
  XReparentWindow(_display, _win, Tk_WindowId(_tkWindow),
		  _leftMargin, _topMargin);

  // make this window smaller so that margins have space inside Tk window

  w -= _leftMargin + _rightMargin;
  h -= _topMargin + _bottomMargin;
  MoveResize(_leftMargin, _topMargin, w, h);

  // then optionally make the Tk window a child of this window's parent
  // i.e. the Tk window gets inserted between this window and its parent
  
  if (parent) {
#ifdef DEBUG
    printf("XWindowRep::Reparenting 0x%p to 0x%p at %d,%d\n",
	   Tk_WindowId(_tkWindow), parent->_win, x, y);
#endif
    XReparentWindow(_display, Tk_WindowId(_tkWindow), parent->_win, x, y);
  }
}

void XWindowRep::DetachFromTkWindow()
{
  DOASSERT(_win, "Cannot detach pixmap from Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;

#ifdef DEBUG
  printf("ViewWin::Detaching 0x%p from 0x%0x\n", this, _tkWindow);
#endif

  DOASSERT(_tkWindow, "Invalid Tk window");
  unsigned long mask = StructureNotifyMask;
  Tk_DeleteEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // adjust location and size since margins are disappearing

  x -= _leftMargin;
  w += _leftMargin + _rightMargin;
  y -= _topMargin;
  H += _topMargin + _bottomMargin;
  MoveResize(x, y, w, h);

  XReparentWindow(_display, _win, DefaultRootWindow(_display), x, y);

  // destroy Tk window

  char cmd[256];
  sprintf(cmd, "destroy %s", _tkPathName);
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot destroy Tk window");

  _tkPathName[0] = 0;
  _tkWindow = 0;
}
#endif
