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
  Revision 1.31  1996/10/18 20:34:14  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.30  1996/09/18 20:17:31  guangshu
  Modified function ExportGIF.

  Revision 1.29  1996/09/09 14:31:42  jussi
  Added #ifdef LIBCS statements to make code compile in the
  ClientServer library target.

  Revision 1.28  1996/09/06 06:59:48  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.27  1996/09/04 21:24:52  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.26  1996/08/29 22:14:09  guangshu
  Changed arguments for ExportGIF.

  Revision 1.25  1996/08/28 00:19:40  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.24  1996/08/23 16:56:04  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.23  1996/08/09 15:27:08  wenger
  Fixed a typo (name of Compression class misspelled).

  Revision 1.22  1996/07/23 15:34:46  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.21  1996/07/18 01:22:50  jussi
  Added GetRootGeometry() method.

  Revision 1.20  1996/07/10 16:21:14  jussi
  Improvements and simplifications to the code.

  Revision 1.19  1996/06/21 19:30:36  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.

  Revision 1.18  1996/06/15 13:48:47  jussi
  Added SetWindowBgColor() which allows Devise to change
  the view background color at runtime.

  Revision 1.17  1996/06/15 07:01:13  yuc
  Add _AimAxisPt and _AimAxis for drawing the Aim axis.

  Revision 1.16  1996/05/22 21:02:49  jussi
  Added ImportImage() method.

  Revision 1.15  1996/05/20 18:45:05  jussi
  Merged with ClientServer library code.

  Revision 1.14  1996/04/18 18:17:04  jussi
  Added support for drawing into pixmaps instead of windows.

  Revision 1.13  1996/04/17 20:33:26  jussi
  Added ExportGIF() method.

  Revision 1.12  1996/04/11 17:56:25  jussi
  Added Raise() and Lower().

  Revision 1.11  1996/02/28 17:39:57  yuc
  Added _xsegs as a protected variable. This is used in 3D display.

  Revision 1.10  1996/02/26 23:45:16  jussi
  Added GetSmallFontHeight().

  Revision 1.9  1996/02/05 23:55:22  jussi
  Added support for small fonts.

  Revision 1.8  1996/01/17 20:54:06  jussi
  Added support for additional image export formats.

  Revision 1.7  1995/12/28 21:28:53  jussi
  Got rid of strings.h and stuck with string.h.

  Revision 1.6  1995/12/28 20:45:13  jussi
  Changed the approach with which inverted lines and areas are
  drawn.

  Revision 1.5  1995/12/18 03:13:47  ravim
  Versions for drawing lines and rectangles with XOR logic.

  Revision 1.4  1995/12/02 21:34:21  jussi
  Added support for TK_WINDOW i.e. Tcl/Tk controls surrounding each
  view and window, but this implementation was superseded by code
  in ViewWin.c and therefore was renamed TK_WINDOW_old. Also
  added Reparent method.

  Revision 1.3  1995/11/28 00:01:15  jussi
  Added WritePostscript() method.

  Revision 1.2  1995/09/05 21:13:42  jussi
  Added/updated CVS header.
*/

#ifndef XWindowRep_h
#define XWindowRep_h

#include <string.h>
#include <math.h>

#include "WindowRep.h"
#include "Xdef.h"
#include "DList.h"
#include "Util.h"

class XWindowRep;

DefinePtrDList(XWindowRepList, XWindowRep *);

#ifndef LIBCS
DefineDList(DaliImageList, int);
#endif

/* Bitmap info */
struct XBitmapInfo {
	Boolean inUse;
	Pixmap pixmap;
	XImage *image;
	GC gc;	/* GC for the pixmap */
	int width;
	int height;
};

class XDisplay;
class Compression;

class XWindowRep: public WindowRep {
public:

#ifdef TK_WINDOW_old
	/* Decorate window */
	virtual void Decorate(WindowRep *parent, char *name,
			      unsigned int min_width,
			      unsigned int min_height);

	/* Undecorate window */
	virtual void Undecorate();
#endif

	/* Reparent this window to 'other' or vice versa. */
	virtual void Reparent(Boolean child, void *other, int x, int y);

	/* Raise window to top of stacking order */
	virtual void Raise();

	/* Lower window to bottom of stacking order */
	virtual void Lower();

	/* Flush windowRep's content to display */
	virtual void Flush();

	/* Move and resize window, relative to the parent */
	virtual void MoveResize(int x, int y, unsigned w, unsigned h);

	/* Iconify window. Not guaranteed to succeed.  */
	virtual void Iconify();

	virtual void PushClip(Coord x, Coord y, Coord w, Coord h);
	virtual void PopClip();

	/* import other graphics and display in window */
	virtual void ImportImage(Coord x, Coord y,
				 DisplayExportFormat format, char *filename);

	/* export window image to other graphics formats */
	virtual void ExportImage(DisplayExportFormat format, char *filename);

#ifndef LIBCS
	/* import graphics via Dali */
	virtual void SetDaliServer(char *serverName) { _daliServer =
	  CopyString(serverName); }
  	virtual DevStatus DaliShowImage(Coord centerX, Coord centerY,
                                        Coord width, Coord height,
                                        char *filename, int imageLen,
                                        char *image);
	virtual DevStatus DaliFreeImages();
	virtual int DaliImageCount() { return _daliImages.Size(); };
#endif

	/* drawing primitives */
	/* Return TRUE if window is scrollable */
	virtual Boolean Scrollable();

	/* Scroll a region in the window */
	virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
			    Coord dstX, Coord dstY);

	/* Scroll absolute */
	virtual void ScrollAbsolute(int x, int y, unsigned width,
				    unsigned height, int dstX, int dstY);

        /* Color selection interface using Devise colormap */
	virtual void SetFgColor(GlobalColor fg);
	virtual void SetBgColor(GlobalColor bg);
	virtual void SetWindowBgColor(GlobalColor bg);

#ifdef LIBCS
        /* Color selection interface using specific colors */
        virtual void SetFgRGB(float r, float g, float b);
        virtual void SetBgRGB(float r, float g, float b);
        virtual void GetFgRGB(float &r, float &g, float &b);
        virtual void GetBgRGB(float &r, float &g, float &b);
        virtual void SetWindowBgRGB(float r, float g, float b);
#endif

	virtual void SetPattern(Pattern p);

	virtual void SetLineWidth(int w);

	virtual void FillRect(Coord xlow, Coord ylow, Coord width,
			      Coord height);
	/* Fill rectangles, variable width/height */
	virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
				   Coord *height, int num);
	/* Fill rectangles, same width/height */
	virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord width,
				   Coord height, int num);

	virtual void DrawPixel(Coord x, Coord y);
	virtual void DrawPixelArray(Coord *x, Coord *y, int num, int width);

	/* Fill rectangle. All coordinates are in pixels. x and y are
	   at the center of the rectangle */
	virtual void FillPixelRect(Coord x, Coord y, Coord width, Coord height,
				   Coord minWidth = 1.0, Coord minHeigh = 1.0);
	virtual void FillPoly(Point *, int n);
	virtual void FillPixelPoly(Point *, int n);
	virtual void Arc(Coord x, Coord y, Coord w, Coord h,
			Coord startAngle, Coord endAngle);

	virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width);
	virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width);

	virtual void ScaledText(char *text, Coord x, Coord y, Coord width,
			  Coord height, TextAlignment alignment = AlignCenter,
			  Boolean skipLeadingSpaces = false);

	virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
				  Coord height,
				  TextAlignment alignment = AlignCenter,
				  Boolean skipLeadingSpaces = false);

	/* Set XOR or normal drawing mode on */
	virtual void SetXorMode();
	virtual void SetCopyMode();

	/* Set normal or small font */
	virtual void SetNormalFont();
	virtual void SetSmallFont();
	virtual int  GetSmallFontHeight();

	/* Draw rubberbanding rectangle */
	virtual void DrawRubberband(int x1, int y1, int x2, int y2);

	/* Get window rep dimensions */
	virtual void Dimensions(unsigned int &width, unsigned int &height);

	/* Get window rep origin */
	virtual void Origin(int &x, int &y);

	/* Get absolute window rep origin from upper left corner
	   of the screen */
	virtual void AbsoluteOrigin(int &x, int &y);

	  /* get display of this Window Rep */
	XDisplay* GetDisplay() { return (XDisplay*)WindowRep::GetDisplay(); }


	// ---------------------------------------------------------- 

	/* Return contents of window as a pixmap */
	virtual DevisePixmap *GetPixmap();

	/* Display pixmap in window */
	virtual void DisplayPixmap(DevisePixmap *pixmap);

	/* Free pixmap from memory */
	virtual void FreePixmap(DevisePixmap *pixmap);

#ifdef TK_WINDOW_old
	/* Tk window size changed -- update size of this window */
	virtual void TkWindowSizeChanged();
#endif

protected:
	friend class XDisplay;

	/* called by XDisplay to create new X window */
 	XWindowRep(Display *display, Window window, XDisplay *DVDisp, 
		   XWindowRep *parent, GlobalColor fgndColor,
		   GlobalColor bgndColor, Boolean backingStore = false); 

	/* called by XDisplay to create new X pixmap */
 	XWindowRep(Display *display, Pixmap pixmap, XDisplay *DVDisp, 
		   XWindowRep *parent, GlobalColor fgndColor,
		   GlobalColor bgndColor, int x, int y); 

	/* called by constructors to initialize object */
	void Init();

	/* destructor */
	~XWindowRep();

	void HandleEvent(XEvent &event);

#ifdef TK_WINDOW_old
	/* Assign window to a new parent. */
	virtual void Reparent(Window newParent, int x, int y);

	/* Build Tk window around this window */
	virtual void EmbedInTkWindow(XWindowRep *parent,
				     char *name,
				     unsigned int min_width,
				     unsigned int min_height);

	/* Detach window from Tk window */
	virtual void DetachFromTkWindow();

	/* Return true if window is inside a Tk window */
	Boolean isInTkWindow() {
	  return (strlen(_tkPathName) > 0 ? true : false);
	}

	char      _tkPathName[32];
	Tk_Window _tkWindow;
#endif

	/* return window identifier */
	Window GetWinId() { return _win; }

	/* return pixmap identifier */
	Pixmap GetPixmapId() { return _pixmap; }

        /* get geometry of root window enclosing this window */
        void GetRootGeometry(int &x, int &y, unsigned int &w, unsigned int &h);

        /* find top window */
        Window FindTopWindow(Window win);

	/* export window image as GIF */
	void ExportGIF(FILE *fp, int isView = 0);

	/* recursively copy the contents of subpixmaps onto parent pixmap */
	static void CoalescePixmaps(XWindowRep *root);

private:
	/* Update window dimensions; globals: _x, _y, _width, _height */
	void UpdateWinDimensions();

#ifndef RAWMOUSEEVENTS
	/* Handle button press event. Return the region covered by
	   the selection in window coordinates */
	void DoButtonPress(int x, int y, int &xlow, int &ylow, int &xhigh,
			   int &yhigh, int button);
#endif

	/* allocate a bitmap of the given width and height into the
	   given info. Free old bitmap data if necessary */
	void AllocBitmap(XBitmapInfo &info, int width, int height);

	/* free bitmap, if not already free */
	void FreeBitmap(XBitmapInfo &info) {
	  if (info.inUse) {
	    /* free old bitmap */
	    XFreeGC(_display, info.gc);
	    XDestroyImage(info.image);
	    XFreePixmap(_display, info.pixmap);
	    info.inUse = false;
	  }
	}

	/* copy bimtap from source to destination. Used to scale text. */
	void CopyBitmap(int width, int height, int dstWidth, int dstHeight);

	/* current dimensions of window */
	int _x, _y;
	unsigned int _width, _height;

	Display *_display;
	Window _win;
	GC _gc;

#ifdef LIBCS
        /* current foreground/background color pixel values */
        LocalColor _rgbForeground;
        LocalColor _rgbBackground;
#endif

	/* pixmap and child/parent links for pixmaps */
	Pixmap _pixmap;
	XWindowRep    *_parent;
	XWindowRepList _children;

	/* GC for rubber-banding */
	GC _rectGc;

	/* bitmaps for drawing/scaling text*/
	XBitmapInfo _srcBitmap, _dstBitmap;

#ifndef RAWMOUSEEVENTS
	/* Pop-up window Manipulation:
	1) display messages
	2) wait till button is released. */
	void DoPopup(int x, int y, int button);
#endif

	Boolean _backingStore;  /* TRUE if window has backingstore */
	Boolean _unobscured; 	/* TRUE if window is totally unobscured */

	/* for compression */
	Compression *_compress;

	/* True if display graphics */
	Boolean _dispGraphics;

#ifndef LIBCS
	DaliImageList _daliImages;
	char *_daliServer;
#endif
};

#endif
