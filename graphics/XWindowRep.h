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

class XWindowRep;

DefinePtrDList(XWindowRepList, XWindowRep *);

/* Bitmap info */
struct XBitmapInfo {
	Boolean inUse;
	Pixmap pixmap;
	XImage *image;
	GC gc;	/* GC for the pixmap */
	int width;
	int height;
};

class Compmression;

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

	/* export window image */
	virtual void ExportImage(DisplayExportFormat format, char *filename);

	/* drawing primitives */
	/* Return TRUE if window is scrollable */
	virtual Boolean Scrollable();

	/* Scroll a region in the window */
	virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
			    Coord dstX, Coord dstY);

	/* Scroll absoluate */
	virtual void ScrollAbsolute(int x, int y, unsigned width,
				    unsigned height, int dstX, int dstY);


	virtual void SetFgColor(Color fg);
	virtual void SetBgColor(Color bg);

	virtual void SetPattern(Pattern p);

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


	virtual void Text(char *text, Coord x, Coord y, Coord width,
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

	/* Get window rep dimensions */
	virtual void Dimensions(unsigned int &width, unsigned int &height);

	/* get window rep origin */
	virtual void Origin(int &x, int &y);

	/* Get absolute window rep origin from upper left corner
	   of the screen */
	virtual void AbsoluteOrigin(int &x,int &y);

	// ---------------------------------------------------------- 
	// 3D

	XPoint CompProjectionOnViewingPlane(POINT);

	virtual POINT CompLocationOnViewingSpace(POINT);

	virtual void MapAllPoints(BLOCK *block_data, int numSyms);
	virtual void MapAllSegments(BLOCK *block_data, int numSyms);
	virtual void DrawXSegments();
	virtual void DrawRefAxis();

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
		   Color fgndColor, Color bgndColor,
		   Boolean backingStore = false); 

	/* called by XDisplay to create new X pixmap */
 	XWindowRep(Display *display, Pixmap pixmap, XDisplay *DVDisp, 
		   XWindowRep *parent, Color fgndColor, Color bgndColor,
		   int x, int y); 

	/* called by constructors to initialize object */
	void Init();

	/* destructor */
	~XWindowRep();

	void HandleEvent(XEvent &event);

	/* data structures for 3D images */
	XSegment _xsegs[WINDOWREP_BATCH_SIZE * BLOCK_EDGES];

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

	/* export window image as GIF */
	void ExportGIF(char *filename);

	/* recursively copy the contents of subpixmaps onto parent pixmap */
	static void CoalescePixmaps(XWindowRep *root);

private:
	/* Update window dimensions; globals: _x, _y, _width, _height */
	void UpdateWinDimensions();

#ifndef RAWMOUSEEVENTS
	/* draw rubberbanding rectangle */
	void DrawRubberband(int x1,int y1, int x2, int y2);

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

	/* pixmap and child/parent links for pixmaps */
	Pixmap _pixmap;
	XWindowRep    *_parent;
	XWindowRepList _children;

	/* GC for rubber-banding */
	GC _rectGc;

	/* temp variables to hold points */
	XPoint *_xpoints;
	int _num_points;

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
};

#endif
