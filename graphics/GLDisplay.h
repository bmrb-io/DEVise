/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Wed Oct  1 14:32:06 CDT 1997 Zhenhai Lin
  Added/updated CVS header.
*/

#ifndef GLDisplay_h
#define GLDisplay_h

#include "Display.h"
#include "Xdef.h"
#include "GLWindowRep.h"
#include "XBitmap.h"
#include "Pattern.h"

#ifdef TK_WINDOW_EV2
#include <tcl.h>
#include <tk.h>
#endif

#ifndef LINE_SIZECONST
#define LINE_SIZECONST
const int LINE_SIZE = 512;
#endif

class GLDisplay: public DeviseDisplay {
public:
    GLDisplay(char *name = 0);
    ~GLDisplay();

#ifdef TK_WINDOW_EV2
    virtual ~GLDisplay();
    virtual int HandleXEvent(XEvent &event);
#endif

    /* get file descriptor for connection */
    virtual int fd() {
      return ConnectionNumber(_display);
    }

    virtual int NumPlanes() {
      return DisplayPlanes(_display, DefaultScreen(_display));
    }

    virtual void Dimensions(Coord &width, Coord &height);

    /* Create a new window Rep. Dimensions are in normatlized
       coord from (0,0) to (1,1) */
    virtual WindowRep *CreateWindowRep(char *name, Coord x, Coord y,
				       Coord width, Coord height, 
				       WindowRep *parentRep = NULL,
                                       Coord min_width = 0.05,
				       Coord min_height = 0.05,
                                       Boolean relative = true,
				       Boolean winBoundary = true);

    /* Destroy a window rep*/
    virtual void DestroyWindowRep(WindowRep *win);

    /* Do internal event processing, but do not block while doing it */
    virtual void InternalProcessing();

    /* Flush buffered window operations to screen */
    virtual void Flush() { XSync(_display, false); }

#ifndef LIBCS
    /* Export display image to other graphics formats */
    virtual void ExportImage(DisplayExportFormat format, char *filename);
    virtual void ExportImageAndMap(DisplayExportFormat format, 
				   char *gifFilename, char *mapFilename, 
				   char *url, char *defaultUrl);
    void ExportView(DisplayExportFormat format, char *filename);
    virtual void ExportGIF(FILE *fp, int isView = 0);
#endif

    virtual void SetTasvirServer(const char *server);
    virtual void SetETkServer(const char *server);

protected:
#if !defined(LIBCS)
    /* Register display with the dispatcher */
    virtual void Register();
#endif

    void MakeAndWriteGif(FILE *fp, int x, int y, int w, int h);
    /* Convert drawable to GIF and write to file */
    void ConvertAndWriteGIF(Drawable drawable, 
                            XWindowAttributes xwa,
                            FILE *fp);

    friend class GLWindowRep;

    /* Get stipple for pattern */
    Pixmap GetPatternStipple(int p) { // p should be of type Pattern
	if( p < 0 ) p = -p;
	if( p >= XNumBitmaps ) p %= XNumBitmaps;
	return _stipples[p];
    };
    
    /* get structure for fonts */
    XFontStruct *GetFontStruct()       { return _fontStruct; };
    XFontStruct *GetNormalFontStruct() { return _normalFontStruct; };
    void SetFont(char *family, char *weight, char *slant,
                 char *width, float pointSize);
    void SetNormalFont();

    /* Get width and height of X window */
    void WinDimensions(Window win, Coord &winWidth, Coord &winHeight);

    /* translate from normalized dimensions of (0,0) to (1,1) into
       real window coordinates.  */
    void RealWindowDimensions(Window win, Coord x, Coord y, Coord w, Coord h,
			      Coord &realX, Coord &realY, Coord &realWidth,
			      Coord &realHeight) {
      Coord winWidth, winHeight;
      WinDimensions(win, winWidth, winHeight);
      realX = x * winWidth;
      realY = y * winHeight;
      realWidth = w * winWidth;
      realHeight = h * winHeight;
    }

    void RealWindowWidthHeight(Window win,Coord width, Coord height,
			       Coord &realWidth, Coord &realHeight){
      Coord winWidth, winHeight;
      WinDimensions(win, winWidth, winHeight);
      realWidth = width * winWidth;
      realHeight = height * winHeight;
    }

    // Temporary -- don't yet know how to figure this out.  RKW 1998-12-15.
    virtual Coord PointsPerPixel() { return 1.0; }

private:
    Display *_display;		    /* X display */
    GLWindowRepList _winList;        /* list of X windows created */
    Pixmap _stipples[XNumBitmaps];  /* bitmaps for patterns */
    XFontStruct *_fontStruct;       /* current font */
    XFontStruct *_normalFontStruct; /* big font used in window */
    XFontStruct *_smallFontStruct;  /* small font used in window */
    Colormap _cmap;
    XVisualInfo * _vi;
    GLboolean double_buffer;
};
#endif
