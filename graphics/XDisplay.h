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
  Revision 1.30  1997/07/22 19:44:30  wenger
  Removed extra dependencies that broke cslib link.

  Revision 1.29  1997/05/21 22:10:02  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.28.6.1  1997/05/21 20:40:09  weaver
  Changes for new ColorManager

  Revision 1.28  1997/05/05 16:53:49  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.27  1997/01/24 21:18:55  wenger
  Fixed memory leak in X font handling code; noted other minor leaks.

  Revision 1.26  1996/12/15 20:22:35  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.25  1996/12/03 17:00:26  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.24  1996/11/18 23:11:21  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.23  1996/11/13 16:56:18  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.22  1996/09/18 20:15:38  guangshu
  Added function ExportView and modified function ExportGIF.

  Revision 1.21  1996/09/13 23:04:52  guangshu
  Added methods to save map files for www when save display.

  Revision 1.20  1996/09/06 06:59:46  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.19  1996/09/05 21:30:15  jussi
  Moved user-specified screen size to Display.

  Revision 1.18  1996/09/05 20:01:50  jussi
  Added support for user-specified screen size (mainly used
  in batch mode).

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

const int LINE_SIZE = 512;

class XDisplay : public DeviseDisplay
{
	public:

		// Constructors and Destructors
		XDisplay(char *name = 0);

		// Getters and Setters
		Display*		GetDisplay(void)		{ return _display;	}
		const Display*	GetDisplay(void) const	{ return _display;	}
		
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
    
    friend class XWindowRep;

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

private:
    Display *_display;		    /* X display */
    XWindowRepList _winList;        /* list of X windows created */
    Pixmap _stipples[XNumBitmaps];  /* bitmaps for patterns */
    XFontStruct *_fontStruct;       /* current font */
    XFontStruct *_normalFontStruct; /* big font used in window */
    XFontStruct *_smallFontStruct;  /* small font used in window */
};

#endif
