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
  Revision 1.17  1996/08/29 22:12:17  guangshu
  Changed arguments for ConvertAndWriteGIF.

  Revision 1.16  1996/07/23 15:34:43  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.15  1996/07/18 01:20:14  jussi
  Added ExportImage() and ConvertAndWriteGIF() methods.

  Revision 1.14  1996/06/24 19:37:19  jussi
  Cleaned up a little.

  Revision 1.13  1996/05/20 18:45:02  jussi
  Merged with ClientServer library code.

  Revision 1.12  1996/04/18 18:14:55  jussi
  Moved definition of XWindowRepList to XWindowRep.h.

  Revision 1.11  1996/04/09 18:06:00  jussi
  Added Flush() method.

  Revision 1.10  1996/04/08 16:57:57  jussi
  Minor fixes.

  Revision 1.9  1996/04/04 05:18:31  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from
  Display) corrected a bug in call to register function. Also now
  dispatcher uses socket number passed from the XDisplay class to
  select on it and call the relevant functions.

  Revision 1.8  1996/02/05 23:55:18  jussi
  Added support for small fonts.

  Revision 1.7  1996/01/30 00:03:28  jussi
  Fixed spelling of void.

  Revision 1.6  1996/01/30 00:01:42  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.5  1996/01/10 20:59:26  jussi
  Added member variable baseColor.

  Revision 1.4  1995/12/06 21:23:15  jussi
  Added ClosestColor() method.

  Revision 1.3  1995/12/02 21:30:38  jussi
  Tried if letting Tcl/Tk handle all X events and pass a copy
  of each event to us would solve the problem with TK_WINDOW_old.
  This event handling mechanism didn't turn out to work right
  so I renamed it TK_WINDOW_EV2.

  Revision 1.2  1995/09/05 21:13:39  jussi
  Added/updated CVS header.
*/

#ifndef XDisplay_h
#define XDisplay_h

#include "Display.h"
#include "Xdef.h"
#include "XWindowRep.h"
#include "XBitmap.h"
#include "Pattern.h"

#ifdef TK_WINDOW_EV2
#include <tcl.h>
#include <tk.h>
#endif

class XDisplay: public DeviseDisplay {
public:
    XDisplay(char *name = 0);

#ifdef TK_WINDOW_EV2
    virtual ~XDisplay();
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
				       Color fgnd = ForegroundColor,
				       Color bgnd = BackgroundColor,
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

    /* Export display image to other graphics formats */
    virtual void ExportImage(DisplayExportFormat format, char *filename);

#ifdef LIBCS
    /* Translate RGB colors to pixel values and back */
    virtual Color FindLocalColor(float r, float g, float b);
    virtual void FindLocalColor(Color c, float &r, float &g, float &b);
#endif

protected:
#ifndef LIBCS
    /* Register display with the dispatcher */
    virtual void Register();
#endif

    /* Convert drawable to GIF and write to file */
    void ConvertAndWriteGIF(Drawable drawable, 
                            XWindowAttributes xwa,
                            FILE *fp);

    Boolean ClosestColor(Colormap &map, XColor &color, Color &c,
			 float &error);
    virtual void AllocColor(char *name, Color globalColor);
    virtual void AllocColor(float r, float g, float b, Color globalColor);
    
    friend class XWindowRep;

    /* Get stipple for pattern */
    Pixmap GetPatternStipple(Pattern p){ return _stipples[p]; };
    
    /* get structure for fonts */
    XFontStruct *GetFontStruct()       { return _fontStruct; };
    XFontStruct *GetNormalFontStruct() { return _normalFontStruct; };
    XFontStruct *GetSmallFontStruct()  { return _smallFontStruct; };
    void SetNormalFont() {
      _fontStruct = _normalFontStruct;
    }
    void SetSmallFont() {
      _fontStruct = _smallFontStruct;
    }

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

private:
    Display *_display;		    /* X display */
    XWindowRepList _winList;        /* list of X windows created */
    Pixmap _stipples[XNumBitmaps];  /* bitmaps for patterns */
    XFontStruct *_fontStruct;       /* current font */
    XFontStruct *_normalFontStruct; /* big font used in window */
    XFontStruct *_smallFontStruct;  /* small font used in window */
    Color baseColor;                /* base color */
};

#endif
