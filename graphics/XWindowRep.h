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
  Revision 1.47  1997/11/24 23:14:43  weaver
  Changes for the new ColorManager.

  Revision 1.46  1997/07/18 20:25:09  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.45  1997/06/13 18:07:32  wenger
  Orientation is now working for text labels and fixed text labels.

  Revision 1.44  1997/05/21 22:10:07  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.43.6.1  1997/05/21 20:40:11  weaver
  Changes for new ColorManager

  Revision 1.43  1997/05/05 16:53:53  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.42  1997/04/11 18:48:59  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.41  1997/03/20 19:56:04  wenger
  Minor mods to get latest version to compile on HP and SunOS; cleaned
  up DTE makefiles (moved all targets from architecture-specific makefiles
  to DTE/Makefile.base).

  Revision 1.40  1997/03/19 19:41:11  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.39  1997/01/17 20:31:52  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.38  1997/01/09 18:41:21  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.37  1996/12/30 23:51:15  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.36  1996/12/15 20:22:33  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.35  1996/12/11 18:05:39  wenger
  Arc() method now works in PSWindowRep class; put SetSmallFont() method
  back into WindowRep classes for backwards compatibility for Opossum;
  server example program tests more of the WindowRep methods than before;
  removed 'not yet implemented' warnings from some PSWindowRep methods.

  Revision 1.34  1996/12/03 17:00:24  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.33  1996/11/26 16:47:50  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.32  1996/11/13 16:56:23  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

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
//#include "machdep.h"

#include "Color.h"

class XWindowRep;

DefinePtrDList(XWindowRepList, XWindowRep *);
#ifndef DALIIMAGELIST_INT
#define DALIIMAGELIST_INT
DefineDList(DaliImageList, int);
#endif
#ifndef ETKWINLIST_ETKINFO
#define ETKWINLIST_ETKINFO
DefinePtrDList(ETkWinList, ETkInfo *);
#endif

/* Bitmap info */
#ifndef XBITMAPINFO
#define XBITMAPINFO
struct XBitmapInfo {
	Boolean inUse;
	Pixmap pixmap;
	XImage *image;
	GC gc;	/* GC for the pixmap */
	int width;
	int height;
};
#endif

class XDisplay;
class Compression;

//******************************************************************************
// class XWindowRep
//******************************************************************************

class XWindowRep : public WindowRep
{
		friend class XDisplay;
		
	protected:

		// Constructors and Destructors
		XWindowRep(Display* display, Window window, XDisplay* DVDisp, 
				   XWindowRep* parent, Boolean backingStore = false); 
		/* called by XDisplay to create new X window */

		XWindowRep(Display* display, Pixmap pixmap, XDisplay* DVDisp, 
				   XWindowRep* parent, int x, int y); 
		/* called by XDisplay to create new X pixmap */

		virtual ~XWindowRep(void);

	public:

		// Getters and Setters
		virtual void	SetForeground(PColorID fgid);
		virtual void	SetBackground(PColorID bgid);

		// Utility Functions
		virtual void	SetWindowBackground(PColorID bgid);
		void	ClearPixmap(void);

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

	/* Find out whether window has backing store. */
	virtual Boolean HasBackingStore();

	/* Move and resize window, relative to the parent */
	virtual void MoveResize(int x, int y, unsigned w, unsigned h);

	/* Iconify window. Not guaranteed to succeed.  */
	virtual void Iconify();

	virtual void PushClip(Coord x, Coord y, Coord w, Coord h);
	virtual void PopClip();

	/* export window image to other graphics formats */
	virtual void ExportImage(DisplayExportFormat format, char *filename);

	/* import graphics via Dali */
	virtual void SetDaliServer(const char *serverName)
	{
	    _daliServer = CopyString((char *)serverName);
	}
  	virtual DevStatus DaliShowImage(Coord centerX, Coord centerY,
                                        Coord width, Coord height,
                                        char *filename, int imageLen,
                                        char *image, float timeoutFactor = 1.0,
					Boolean maintainAspect = true);
	virtual DevStatus DaliFreeImages();
	virtual int DaliImageCount()
	{
	    return _daliImages.Size();
	}

	/* Display embedded Tk windows */
	virtual void SetETkServer(const char *serverName)
	{
	    _etkServer = CopyString((char *)serverName);
	}
  	virtual DevStatus ETk_CreateWindow(Coord x, Coord y,
					   Coord width, Coord height,
					   ETkIfc::Anchor anchor,
					   char *filename,
					   int argc, char **argv,
					   int &handle);
  	virtual int ETk_FindWindow(Coord centerX, Coord centerY,
				   char *script);
	virtual DevStatus ETk_MoveWindow(int handle,
					 Coord centerX, Coord centerY);
	virtual DevStatus ETk_ResizeWindow(int handle,
					   Coord width, Coord height);
	virtual DevStatus ETk_NotifyResize(int handle,
					   Coord centerX, Coord centerY,
					   Coord width, Coord height);
	virtual DevStatus ETk_MoveResizeWindow(int handle,
					       Coord centerX, Coord centerY,
					       Coord width, Coord height);
	virtual DevStatus ETk_FreeWindow(int handle);
	virtual DevStatus ETk_MapWindow(int handle);
	virtual DevStatus ETk_UnmapWindow(int handle);
	virtual DevStatus ETk_EvalCmd(int handle,
				      int argc, char **argv,
				      char *&returnValue);
	virtual DevStatus ETk_FreeWindows();
	virtual DevStatus ETk_Mark(int handle, bool in_use);
	virtual DevStatus ETk_MarkAll(bool in_use);
	virtual DevStatus ETk_Cleanup();
	virtual int ETk_WindowCount()
	{
	    return _etkWindows.Size();
	}
	virtual DevStatus ETk_GetInfo(int handle, ETkInfo &info);
	/* Iterate through the ETk windows */
	virtual int ETk_InitIterator()
	{
	    return _etkWindows.InitIterator();
	}
	virtual Boolean ETk_More(int index)
	{
	    return _etkWindows.More(index);
	}
	virtual ETkInfo *ETk_Next(int index)
	{
	    return _etkWindows.Next(index);
	}
	virtual void ETk_DoneIterator(int index)
	{
	    _etkWindows.DoneIterator(index);
	}

	/* drawing primitives */
	/* Return TRUE if window is scrollable */
	virtual Boolean Scrollable();

	/* Scroll a region in the window */
	virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
			    Coord dstX, Coord dstY);

	/* Scroll absolute */
	virtual void ScrollAbsolute(int x, int y, unsigned width,
				    unsigned height, int dstX, int dstY);

	virtual void Transform(Coord x, Coord y, Coord &newX, Coord &newY) {
		WindowRep::Transform(x,y,newX,newY);
		newY=_height-newY;
	}
	
	virtual void SetPattern(Pattern p);

	virtual void SetLineWidth(int w);
#ifdef LIBCS
	virtual void SetDashes(int dashCount, int dashes[], int startOffset);
#endif

	virtual void FillRect(Coord xlow, Coord ylow, Coord width,
			      Coord height);
	virtual void FillRectAlign(Coord xlow, Coord ylow, Coord width,
				   Coord height,
				   SymbolAlignment alignment = AlignSouthWest,
				   Coord orientation = 0.0);

	/* Fill rectangles, variable width/height */
	virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
				   Coord *height, int num,
				   SymbolAlignment alignment = AlignSouthWest,
				   Coord orientation = 0.0);
	/* Fill rectangles, same width/height */
	virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord width,
				   Coord height, int num,
				   SymbolAlignment alignment = AlignSouthWest,
				   Coord orientation = 0.0);

	virtual void DrawPixel(Coord x, Coord y);
	virtual void DrawPixelArray(Coord *x, Coord *y, int num, int width);

	/* Fill rectangle. All coordinates are in pixels. x and y are
	   at the center of the rectangle */
	virtual void FillPixelRect(Coord x, Coord y, Coord width, Coord height,
				   Coord minWidth = 1.0, Coord minHeight = 1.0,
				   SymbolAlignment alignment = AlignSouthWest,
				   Coord orientation = 0.0);
	virtual void FillPoly(Point *, int n);
	virtual void FillPixelPoly(Point *, int n);

        /* Draw an arc.  Angles are in radians. */
        virtual void Arc(Coord xCenter, Coord yCenter, Coord horizDiam,
                         Coord vertDiam, Coord startAngle, Coord endAngle);

	virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width);
	virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width);

	virtual void ScaledText(char *text, Coord x, Coord y, Coord width,
			  Coord height, SymbolAlignment alignment = AlignCenter,
			  Boolean skipLeadingSpaces = false,
			  Coord orientation = 0.0);

	virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
				  Coord height,
				  SymbolAlignment alignment = AlignCenter,
				  Boolean skipLeadingSpaces = false,
				  Coord orientation = 0.0);

	/* Set XOR or normal drawing mode on */
	virtual void SetXorMode();
	virtual void SetCopyMode();
	virtual void SetOrMode();

	/* Set font or return to normal */
        virtual void SetFont(char *family, char *weight, char *slant,
                             char *width, float pointSize);
	virtual void SetNormalFont();
	
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

	Compression *GetCompress() {return _compress;}
	
#ifdef TK_WINDOW_old
	/* Tk window size changed -- update size of this window */
	virtual void TkWindowSizeChanged();
#endif

protected:

	/* called by constructors to initialize object */
	void Init();

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

	virtual void DrawText(Boolean scaled, char *text, Coord x, Coord y,
			      Coord width, Coord height,
			      SymbolAlignment alignment = AlignCenter,
			      Boolean skipLeadingSpaces = false,
			      Coord orientation = 0.0);

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

	/* copy bitmap from source to destination. Used to scale text. */
	void CopyBitmap(int width, int height, int dstWidth, int dstHeight);

	void DrawRectangle(int symbolX, int symbolY, int width, int height,
			   Boolean filled = true,
			   SymbolAlignment alignment = AlignCenter,
			   Coord orientation = 0.0);

	void DrawRectangles(XRectangle rectangles[], int rectCount,
			   Boolean filled = true,
			   SymbolAlignment alignment = AlignCenter,
			   Coord orientation = 0.0);

	/* Calculate the locations of points needed to draw a rectangle,
	 * based on the alignment and orientation.  points array is only
	 * needed if orientation is non-zero.  If used, it must have a
	 * size of 5 (NOT 4!). */
        static void CalculateLocation(int &symbolX, int &symbolY, int width,
				      int height, SymbolAlignment alignment,
				      Coord orientation,
				      XPoint *points = NULL);

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

	int _lineStyle;

//#if !defined(LIBCS) || defined(NEW_LIBCS)
	DaliImageList _daliImages;    // List of Tasvir images
	char *_daliServer;            // Machine where Tasvir is running
	ETkWinList _etkWindows;       // List of embedded Tk windows
	char *_etkServer;             // Machine where ETk server is running
//#endif
};

//******************************************************************************
#endif
