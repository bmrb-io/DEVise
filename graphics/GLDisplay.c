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
  Wed Oct  1 14:32:06 CDT 1997 Zhenhai Lin
  Added/updated CVS header.
*/

#include <math.h>
#ifdef SUN
#include <strings.h>
#endif
#include <X11/Xatom.h>

#include "GLDisplay.h"
#include "GLWindowRep.h"
#if !defined(LIBCS)
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif
#include "Version.h"

#include "Color.h"
#include "XColor.h"

#define String XVString
#define black XVblack

extern "C" {
#include "xv.h"
}

//#define DEBUG

#include "Util.h"
#ifndef LIBCS
#include "WinClassInfo.h"
#endif

/* The maximum intensity of red, green, and blue. */
const int MaxColorIntensity = 65535;

#ifdef TK_WINDOW_EV2
static int HandleTkEvent(ClientData data, XEvent *event)
{
  return ((GLDisplay *)data)->HandleXEvent(*event);
}
#endif

/*******************************************************************
Open a new X display
********************************************************************/

GLDisplay::GLDisplay(char *name)
{
#if defined(DEBUG)
  printf("GLDisplay::GLDisplay(%s)\n", name != NULL ? name : "NULL");
#endif
  if (!(_display = XOpenDisplay(name))) {
    fprintf(stderr, "Cannot open GLDisplay\n");
    Exit::DoExit(1);
  }

  /* set normal font to be the current font */
  _fontStruct = NULL;
  SetFont("Courier", "Medium", "r", "Normal", 12.0);
  _normalFontStruct = _fontStruct;
  if (!_normalFontStruct) {
      fprintf(stderr, "Cannot load 12-point Courier font\n");
      Exit::DoExit(1);
  }

  /* init stipples for patterns */
  Window win = XCreateSimpleWindow(_display, DefaultRootWindow(_display),
				   (unsigned)0, (unsigned)0, (unsigned)10,
				   (unsigned)10, 5, 0, 0);
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

  if (!InitColor(_display)) {
    reportErrNosys("Color initialization failed");
  }
}

GLDisplay::~GLDisplay()
{
  TermColor();
}

/* Register the display with the dispatcher */

#ifndef LIBCS
void GLDisplay::Register()
{
  Dispatcher::Current()->Register(this, 10, AllState, true, fd());
}
#endif

void GLDisplay::SetNormalFont()
{
#if defined(DEBUG)
  printf("GLDisplay::SetNormalFont()\n");
#endif

  if (_fontStruct != _normalFontStruct) {
#if defined(DEBUG)
    printf("Freeing font 0x%p\n", _fontStruct);
#endif
    XFreeFont(_display, _fontStruct);
  }

  _fontStruct = _normalFontStruct;
}

/* Set font and point size */

void GLDisplay::SetFont(char *family, char *weight, char *slant,
                       char *width, float pointSize)
{
#if defined(DEBUG)
  printf("GLDisplay::SetFont(%s %s %s %s %f\n", family, weight, slant, width,
    pointSize);
#endif

    XFontStruct *oldFont = _fontStruct;

    /* Special case for Courier or Helvetica italic. */
    if (((!strcasecmp(family, "courier")) ||
      (!strcasecmp(family, "helvetica"))) && !strcasecmp(slant, "i")) {
      slant = "o";
    }

    /*
       Attempt to load font as specified. If font cannot be loaded,
       increase point size by one unit and try again.
    */
    for(float p = pointSize; p <= pointSize + 5.0; p += 1.0) {
        char fname[128];
        sprintf(fname, "*-%s-%s-%s-%s-*-*-%d-*-*-*-*-*-*",
                family, weight, slant, width, (int) (p * 10.0));
#if 0
        printf("  Trying font %s\n", fname);
#endif
        _fontStruct = XLoadQueryFont(_display, fname);
        if (_fontStruct) {
#if defined(DEBUG)
            printf("Loaded font %s\n", fname);
#endif
            if (oldFont && oldFont != _normalFontStruct) {
#if defined(DEBUG)
                printf("Freeing font 0x%p\n", _fontStruct);
#endif
                XFreeFont(_display, oldFont);
            }
            return;
        }
    }
    fprintf(stderr, "Warning: could not find font %s %f\n", family, pointSize);
    _fontStruct = _normalFontStruct;
}

#ifndef LIBCS
/* Export display image to other graphics formats */

void GLDisplay::ExportImage(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("GLDisplay::ExportImage(%d, %s)\n", (int) format, filename);
#endif

  if (format == GIF) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Cannot open file %s for writing\n", filename);
        return;
    }
    ExportGIF(fp);
    if (fp != stderr && fp != stdout)
        fclose(fp);
    return;
  } else {
#ifndef LIBCS
    ExportToPS(format, filename);
#else
    fprintf(stderr, "Cannot export display image in PS/EPS yet\n");
#endif
  }
}

/* Export every view as a window in the display */

void GLDisplay::ExportView(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("GLDisplay::ExportView(%d, %s)\n", (int) format, filename);
#endif

  if (format == GIF) {
	int i = 0;
	int index1 = _winList.InitIterator();

	while(_winList.More(index1)) {
	   GLWindowRep *win = _winList.Next(index1);
	   if (win->_parent)
	      continue;
	   int index2 = win->_children.InitIterator();
	   while(win->_children.More(index2)) {
	      GLWindowRep *winc = win->_children.Next(index2);
	      char fn[strlen(filename) + 10];
	      sprintf(fn,"%s.%d.gif", filename, i);
#if defined(DEBUG)
	      printf("Saving the view to file %s\n", fn);
#endif
	      FILE *fp = fopen(fn, "wb");
	      if (!fp) {
		 fprintf(stderr, "Cannot open file %s for writing.\n", fn);
		 return;
	      }
	      winc->ExportGIF(fp, 1);
	      fclose(fp);
	      i++;
	   }
	   win->_children.DoneIterator(index2);
	}
	_winList.DoneIterator(index1);
	return;
  } else {
    fprintf(stderr, "Cannot export all views in PS/EPS yet\n");
  }
}

/* Export the display and the map file associated with the display */

void GLDisplay::ExportImageAndMap(DisplayExportFormat format, char *gifFilename, 
				char *mapFileName, char *url, char *defaultUrl)
{
#if defined(DEBUG)
  printf("GLDisplay::ExportImageAndMap(%d, %s)\n", (int) format, gifFilename);
#endif

  if (format == GIF) {
     FILE *fp1 = fopen(gifFilename,"wb");
     if (!fp1) {
	fprintf(stderr, "Cannot open file %s for writing\n", gifFilename);
	return;
      }

      char prename[strlen(gifFilename) + 1];
      strcpy(prename, gifFilename);
      char *p = strstr(prename, ".gif");
      char *p_start = strrchr(prename, '/');
      if (p_start) p_start++;
      else p_start = prename;
      if (p) *p = '\0';
      else fprintf(stderr, "Warning: gif file name %s is not ended with .gif\n", 
				gifFilename);
#if defined(DEBUG)
      printf("prename is %s\n", prename);
#endif
     FILE *fp2 = fopen(mapFileName,"wb");
     if (!fp2) {
        fprintf(stderr, "Cannot open file %s for writing\n", mapFileName);
        return;
      }
      int x = 0 ,y = 0;
      unsigned int w = 0, h = 0;

      int index1 = _winList.InitIterator();
      int i = 0;

      while(_winList.More(index1)) {
         GLWindowRep *win = _winList.Next(index1); 
	 if (win->_parent)
            continue;   
         int wx, wy;
	 unsigned int ww, wh;
	 win->GetRootGeometry(wx, wy, ww, wh);

#if defined(DEBUG) || 0
            printf("The geometry of this window: wx = %d, wy = %d, 
                        ww = %u, wh = %u\n", wx, wy, ww, wh);
#endif

	 if (!w && !h) {
      	    /* this is the first window */
      	    x = wx;
            y = wy;
            w = ww;
            h = wh;
#if defined(DEBUG) || 0
    	    printf("In the first window: x = %d, y = %d, 
			w = %u, h = %u\n", x, y, w, h);
#endif
    	 } else {
      	    /* compute combined area */
      	    int x2 = MAX(x + w - 1, wx + ww - 1);
      	    int y2 = MAX(y + h - 1, wy + wh - 1);
      	    x = MIN(x, wx);
       	    y = MIN(y, wy);
      	    w = x2 - x + 1;
      	    h = y2 - y + 1;
    	 }
      }
      _winList.DoneIterator(index1);
#if defined(DEBUG) || 0
      printf("The display has: x = %d, y = %d,
			w = %u, h = %u\n", x, y, w, h);
#endif

      index1 = _winList.InitIterator();
      while(_winList.More(index1)) {
	 GLWindowRep *win = _winList.Next(index1);
	 if (win->_parent)
	    continue;
	 int rx =0, ry = 0;
	 unsigned int rw, rh;
	 win->GetRootGeometry(rx, ry, rw, rh);
#if  defined(DEBUG) || 0
	 printf("The parent has: rx = %d, ry = %d, rw = %u, rh = %u\n",
			rx, ry, rw, rh);
#endif

         int index2 = win->_children.InitIterator();
	 while(win->_children.More(index2)) {
	     GLWindowRep *winc = win->_children.Next(index2);
	     int sub_x = 0, sub_y = 0;
	     unsigned int sub_w, sub_h;
	     winc->Origin(sub_x, sub_y);
	     winc->Dimensions(sub_w, sub_h);
	     char temp[LINE_SIZE];
  	     sprintf(temp, "rect %s?%s+%d %d,%d %u,%u\n", url, p_start, i, sub_x+(rx-x), 
			sub_y+(ry-y)+23, sub_w+sub_x+(rx-x), sub_h+sub_y+(ry-y)+23); 
	     i++;
	     fprintf(fp2, temp);
	 }
         win->_children.DoneIterator(index2);
      }
      _winList.DoneIterator(index1);
      fprintf(fp2, "default %s\n", defaultUrl);
      fclose(fp2);
      MakeAndWriteGif(fp1, x, y, w, h);
      fclose(fp1);
      *p = '.';
      return;
  } else {
    fprintf(stderr, "Cannot export display image in PS/EPS yet\n");
  } 
}

void GLDisplay::ExportGIF(FILE *fp, int isView)
{
#if defined(DEBUG)
  printf("GLDisplay::ExportGIF()\n");
#endif

  /* compute the bounding rectangle of all windows */

  int x = 0, y = 0;
  unsigned int w = 0, h = 0;

  ClassInfo *windowInfo;
  ViewWin *windowP;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    windowP = (ViewWin *) windowInfo->GetInstance();
    if ((windowP != NULL) && !windowP->Iconified() &&
        !windowP->GetPrintExclude()) {
#if defined(DEBUG)
    printf("Getting bounding box for window %s\n", windowP->GetName());
#endif
      GLWindowRep *win = (GLWindowRep *) windowP->GetWindowRep();
      int wx, wy;
      unsigned int ww, wh;
      win->GetRootGeometry(wx, wy, ww, wh);
      if (!w && !h) {
        /* this is the first window */
        x = wx;
        y = wy;
        w = ww;
        h = wh;
      } else {
        /* compute combined area */
        int x2 = MAX(x + w - 1, wx + ww - 1);
        int y2 = MAX(y + h - 1, wy + wh - 1);
        x = MIN(x, wx);
        y = MIN(y, wy);
        w = x2 - x + 1;
        h = y2 - y + 1;
      }
    }
  }
  DevWindow::DoneIterator(index);

#ifdef DEBUG
  printf("Bounding rectangle of windows is %d,%d,%u,%u\n", x, y, w, h);
#endif
  if ((w != 0) && (h != 0)) {
    MakeAndWriteGif(fp, x, y, w, h);
  }
}

void GLDisplay::MakeAndWriteGif(FILE *fp, int x, int y, int w, int h)
{
#if defined(DEBUG)
    printf("GLDisplay::MakeAndWriteGif()\n");
#endif
  /* Allocate a pixmap large enough to hold all windows */

  unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
  Window parent = DefaultRootWindow(_display);
  Pixmap pixmap = XCreatePixmap(_display, parent, w, h, depth);
  if (!pixmap) {
    fprintf(stderr, "Cannot create %ux%d pixmap\n", w, h);
    return;
  }

  /* Create and set up graphics context */

  GC gc = XCreateGC(_display, DefaultRootWindow(_display), 0, NULL);
  XSetState(_display, gc, CM_GetXColorID(GetColorID(gifWhiteColor)),
            CM_GetXColorID(GetColorID(gifBlackColor)), GXcopy, AllPlanes);
  XFillRectangle(_display, pixmap, gc, 0, 0, w, h);

  /* Copy windows to pixmap */

  ClassInfo *windowInfo;
  ViewWin *windowP;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    windowP = (ViewWin *) windowInfo->GetInstance();
    if ((windowP != NULL) && !windowP->Iconified() &&
        !windowP->GetPrintExclude()) {
#if defined(DEBUG)
    printf("Printing window %s\n", windowP->GetName());
#endif
      GLWindowRep *win = (GLWindowRep *) windowP->GetWindowRep();
      int wx, wy;
      unsigned int ww, wh;
      win->GetRootGeometry(wx, wy, ww, wh);
      int dx = wx - x;
      int dy = wy - y;
#ifdef DEBUG
      printf("Copying from window 0x%lx to %d,%d,%u,%u\n",
             win->GetWinId(), dx, dy, ww, wh);
#endif
      DOASSERT(dx >= 0 && dy >= 0, "Invalid window coordinates");
      Pixmap src = win->GetPixmapId();
      if (src)
        GLWindowRep::CoalescePixmaps(win);
      else {
        src = win->FindTopWindow(win->GetWinId());
        XRaiseWindow(_display, src);
      }
      XImage *image = XGetImage(_display, src, 0, 0, ww, wh, AllPlanes,
	ZPixmap);
      if (!image || !image->data) {
        fprintf(stderr, "Cannot get image of window or pixmap\n");
        continue;
      }
      XPutImage(_display, pixmap, gc, image, 0, 0, dx, dy, ww, wh);
      image->data = NULL;
      XDestroyImage(image);
    }
  }
  DevWindow::DoneIterator(index);

  /* Cannot get attributes of pixmap using XGetWindowAttributes
     so let's get the attributes from the root window and then
     patch xwa with the pixmap's information */

  XWindowAttributes xwa;
  if (!XGetWindowAttributes(_display, DefaultRootWindow(_display), &xwa)) {
    fprintf(stderr, "Cannot get window attributes\n");
    return;
  }
  xwa.x = x;
  xwa.y = y;
  xwa.width = w;
  xwa.height = h;
  
  /* Convert pixmap to GIF and write to file */

  ConvertAndWriteGIF(pixmap, xwa, fp);

  /* Free gc and pixmap area */

  XFreeGC(_display, gc);
  XFreePixmap(_display, pixmap);
}
#endif

/* Convert drawable to GIF and write to file */
// Color code is independent of ColorManager for now. Ignore.

void GLDisplay::ConvertAndWriteGIF(Drawable drawable,
                                  XWindowAttributes xwa,
                                  FILE *fp)
{
  XImage *image = XGetImage(_display, drawable, 0, 0, xwa.width, xwa.height,
			    AllPlanes, ZPixmap);
  if (!image || !image->data) {
    fprintf(stderr, "Cannot get image of window or pixmap\n");
    return;
  }

  XColor *colors = 0;
  int ncolors = getxcolors(&xwa, &colors, _display);//TEMPTEMP -- xv function
  int code = convertImage(image, colors, ncolors, &xwa);//TEMPTEMP -- xv function

  XDestroyImage(image);
  if (colors)
    free(colors);

  if (code != 1 || !grabPic) {
    fprintf(stderr, "Cannot convert image\n");
    return;
  }

  int ptype = (gbits == 24 ? PIC24 : PIC8);
  int colorstyle = -1;                  // use 1 for grayscale
  const char *comment = Version::GetWinLogo();

  int status = WriteGIF(fp, grabPic, ptype, gWIDE, gHIGH,
			grabmapR, grabmapG, grabmapB, ncolors,
			colorstyle, (char *) comment);
  if (status)
    fprintf(stderr, "Cannot write GIF image\n");

  free(grabPic);
  grabPic = 0;
}

void GLDisplay::SetTasvirServer(const char *server) {
  int index = _winList.InitIterator();
  while(_winList.More(index)) {
    GLWindowRep *win = _winList.Next(index);
    win->SetDaliServer(server);
  }
  _winList.DoneIterator(index);
}

void GLDisplay::SetETkServer(const char *server) {
  int index = _winList.InitIterator();
  while(_winList.More(index)) {
    GLWindowRep *win = _winList.Next(index);
    win->SetETkServer(server);
  }
  _winList.DoneIterator(index);
}

void GLDisplay::Dimensions(Coord &width, Coord &height)
{
  int screen = DefaultScreen(_display);
  width = (Coord)DisplayWidth(_display, screen);
  height = (Coord)DisplayHeight(_display, screen);
#ifndef LIBCS
  if (_desiredScreenWidth > 0)
    width = _desiredScreenWidth;
  if (_desiredScreenHeight > 0)
    height = _desiredScreenHeight;
#endif
}

void GLDisplay::WinDimensions(Window win, Coord &winWidth, Coord &winHeight)
{
  if (win == DefaultRootWindow(_display)) {
    Dimensions(winWidth, winHeight);
    return;
  }

  Window root;
  int x, y;
  unsigned int width, height;
  unsigned int border_width, depth;
  XGetGeometry(_display, win, &root, &x, &y, &width, &height,
               &border_width, &depth);
  winWidth = (Coord)width;
  winHeight = (Coord)height;
}

/*
 * This structure forms the WMHINTS property of the window,
 * letting the window manager know how to handle this window.
 * See Section 9.1 of the Xlib manual.
 */

static XWMHints xwmh = {
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

WindowRep *GLDisplay::CreateWindowRep(char *name, Coord x, Coord y,
				     Coord width, Coord height, 
				     WindowRep *parentRep,
				     Coord min_width, Coord min_height,
				     Boolean relative, Boolean winBoundary)
{
  DO_DEBUG(printf("GLDisplay::CreateWindowRep(%s)\n", name));
  Window parent;// = DefaultRootWindow(_display);

  if (parentRep)
    parent = ((GLWindowRep *)parentRep)->GetWinId();
  else
    parent = DefaultRootWindow(_display);

  unsigned int border_width;
  if (winBoundary)
    border_width = (!parentRep ? 5 : 1);
  else
    border_width = (!parentRep ? 5 : 0);

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
    printf("GLDisplay: Created X pixmap 0x%lx at %u,%u, size %ux%u\n",
	   pixmap, (unsigned)realX, (unsigned)realY,
	   (unsigned)realWidth, (unsigned)realHeight);
#endif

    GLWindowRep *xwin = new GLWindowRep(_display, pixmap, this,
				      (GLWindowRep *)parentRep,
				      (int)realX, (int)realY);
    DOASSERT(xwin, "Cannot create GLWindowRep");

    _winList.Insert(xwin);
  
    return xwin;
  }
#endif

  /* Define event mask. */

  unsigned long int mask = ExposureMask | ButtonPressMask 
	                   | ButtonReleaseMask | Button1MotionMask
                           | Button2MotionMask | Button3MotionMask
                           | StructureNotifyMask | KeyPressMask
                           | VisibilityChangeMask;

#ifdef RAWMOUSEEVENTS
  mask |= PointerMotionMask;
#endif

  XVisualInfo *vi;

/*  int configuration[] = {GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 12,
                         GLX_RED_SIZE, 1, GLX_BLUE_SIZE, 1,
                         GLX_GREEN_SIZE, 1, None};
*/
  int configuration[] = {GLX_DEPTH_SIZE, 12,
                         GLX_RED_SIZE, 1, GLX_BLUE_SIZE, 1,
                         GLX_GREEN_SIZE, 1, None};
  int configuration2[] = {None};
  GLboolean double_buffer;

  vi = glXChooseVisual(_display, DefaultScreen(_display), configuration);
  if (vi == NULL) {
    vi = glXChooseVisual(_display, DefaultScreen(_display), configuration2);
    DOASSERT(vi != NULL, "no appropriate RGB visual with depth buffer");
    double_buffer=GL_FALSE;
  }
  else {
//    double_buffer=GL_TRUE;
    double_buffer=GL_FALSE;
  }
  
  /* Define window attributes. */
#ifdef SGI
  Colormap cmap = XCreateColormap(_display, RootWindow(_display, vi->screen), vi->visual, AllocNone);
  //  Colormap cmap = XCopyColormapAndFree(_display, DefaultColormap(_display, DefaultScreen(_display)));
#else
  Colormap cmap	= DefaultColormap(_display, DefaultScreen(_display));
#endif

  XSetWindowAttributes attr;
  attr.background_pixmap 	= None;
  attr.background_pixel  	= CM_GetXColorID(GetColorID(xWinBackColor));
  attr.border_pixmap  		= CopyFromParent;
  attr.border_pixel  		= CM_GetXColorID(GetColorID(xWinForeColor));
  attr.bit_gravity  		= ForgetGravity;   /* CenterGravity */
  attr.win_gravity  		= NorthWestGravity;
  attr.backing_store  		= Always;          /* WhenMapped/NotUseful */
  attr.backing_planes  		= AllPlanes;
  attr.backing_pixel  		= 0;
  attr.save_under  		= False;
  attr.event_mask  		= mask;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect  	= False;
  attr.colormap			= cmap;
  attr.cursor  			= None;

  /* Create the window. */




#ifndef SGI
  Window w = XCreateWindow(_display, parent, (unsigned)realX, (unsigned)realY, 
			   (unsigned)realWidth, (unsigned)realHeight,
			   border_width, vi->depth, InputOutput, vi->visual,
			   AllPlanes, &attr);
#else
  Window w = XCreateWindow(_display, parent,
    (unsigned)realX, (unsigned)realY, (unsigned)realWidth, (unsigned)realHeight,
    border_width, vi->depth,
    InputOutput, vi->visual,
/* These bits caused invalid config error on SGI:
   CWBorderPixmap
*/
    CWBackPixmap | CWBackPixel | CWBorderPixel |
    CWBitGravity | CWWinGravity |
    CWBackingStore | CWBackingPlanes | CWBackingPixel |
    CWOverrideRedirect | CWSaveUnder | 
    CWEventMask | CWDontPropagate | CWColormap | CWCursor,
    &attr);
#endif

/*  Window w = XCreateWindow(_display, RootWindow(_display, vi->screen),
    0, 0, 300, 300, 0, vi->depth,
    InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
*/

  DOASSERT(w, "Cannot create window");

  GLXContext gc = glXCreateContext(_display, vi,
                         /* No sharing of display lists */ NULL,
                         /* Direct rendering if possible */ True);


#ifdef DEBUG
  printf("GLDisplay: Created X window 0x%lx to parent 0x%lx at %u,%u,\n",
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

  /* Return the GLWindowRep structure. */

  GLWindowRep *xwin = new GLWindowRep(_display, w, this,
                                    (GLWindowRep *)parentRep, gc, double_buffer);
  DOASSERT(xwin, "Cannot create GLWindowRep");
  _winList.Insert(xwin);
  
  return xwin;
}

/**************************************************************
Destroy a window. Parameter "win" better be of type XwindowRep *.
**************************************************************/

void GLDisplay::DestroyWindowRep(WindowRep *win)
{
  DO_DEBUG(printf("GLDisplay::DestroyWindowRep(%p)\n", win));

  GLWindowRep *xwin = (GLWindowRep *)win;
  if (!_winList.Delete(xwin)) {
    fprintf(stderr, "GLDisplay:Window to be deleted not found\n");
    Exit::DoExit(1);
  }

  // Free the Dali images and sleep before destroying the X window so
  // Dali doesn't get 'bad window' errors.
  if (xwin->DaliImageCount() > 0)
  {
    (void) xwin->DaliFreeImages();
    sleep(1);
  }

//#if !defined(LIBCS) || defined(NEW_LIBCS)
  // Free embedded Tk windows and sleep before destroying the X window
  if (xwin->ETk_WindowCount() > 0)
  {
    (void) xwin->ETk_FreeWindows();
    sleep(1);
  }
//#endif

  if (xwin->GetWinId()) {
#ifdef DEBUG
    printf("GLDisplay::DestroyWindowRep 0x%p, window 0x%lx\n",
	   xwin, xwin->GetWinId());
#endif
    XDestroyWindow(_display, xwin->GetWinId());
    xwin->_win = 0;
  } else {
#ifdef DEBUG
    printf("GLDisplay::DestroyWindowRep 0x%p, pixmap 0x%lx\n",
	   xwin, xwin->GetPixmapId());
#endif
    XFreePixmap(_display, xwin->GetPixmapId());
    xwin->_pixmap = 0;
  }

  /* KLUDGE WARNING.  This somehow fixes bug 182.  I think we ended up
   * getting goofed up by the destroy event on the XWindow we're destroying
   * here, and somehow processing the events at this point fixes it. */
  Run();

  delete xwin;

  XSync(_display, false);
}

/*******************************************************************
Internal non-blocking event processing
********************************************************************/

void GLDisplay::InternalProcessing()
{
#ifdef TK_WINDOW_EV2
  /* X events are handled in HandleTkEvent */
  return;
#endif

#if defined(DEBUG)
  printf("GLDisplay:: Received callback\n");
#endif

  XFlush(_display);

  while (XPending(_display) > 0) {
    /* There are events in the queue */
    XEvent event;
    XNextEvent(_display, &event);

    /* dispatch event to appropriate window */
    Boolean found = false;
    int index;
    for(index = _winList.InitIterator(); _winList.More(index);) {
      GLWindowRep *win = _winList.Next(index);
      if (win->GetWinId() && win->GetWinId() == event.xany.window) {
	/* Note: got to be careful here. We need to
	   call DoneIterator() before informing the WindowRep
	   because it might trigger a call to DestroyWindowRep()
	   to delete the window. Delete() can't be called
	   when the iterator is active. */
	_winList.DoneIterator(index);
	found = true;
#if defined(DEBUG)
	printf("GLDisplay::Dispatching event %d to GLWindowRep 0x%p\n",
	       event.type, win);
#endif
	win->HandleEvent(event);
	break;
      }
    }
    if (!found) {
      _winList.DoneIterator(index);
#ifdef DEBUG
      printf("GLDisplay::InternalProcessing: window for event %d not found\n",
	     event.type);
#endif
    }
  }

#ifdef DEBUG
  printf("X event queue length = %d\n", XQLength(_display));
#endif
}

#ifdef TK_WINDOW_EV2
int GLDisplay::HandleXEvent(XEvent &event)
{
#ifdef DEBUG
  printf("GLDisplay received X event %d from Tk\n", event.type);
#endif

  /* dispatch event to appropriate window.*/
  Boolean found = false;
  int index;
  for(index = _winList.InitIterator(); _winList.More(index);) {
    GLWindowRep *win = _winList.Next(index);
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
    printf("GLDisplay::InternalProcessing: window for event %d not found\n",
	   event.type);
#endif
    return 0;
  }

  return 1;
}
#endif
