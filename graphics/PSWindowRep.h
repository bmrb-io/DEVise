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
  Revision 1.28  1998/04/13 22:24:53  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.27  1997/11/24 23:14:30  weaver
  Changes for the new ColorManager.

  Revision 1.26  1997/07/18 20:25:05  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.25  1997/06/13 18:07:27  wenger
  Orientation is now working for text labels and fixed text labels.

  Revision 1.24  1997/05/21 22:09:57  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.23.6.1  1997/05/21 20:40:01  weaver
  Changes for new ColorManager

  Revision 1.23  1997/05/05 16:53:46  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.22  1997/04/11 18:48:52  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.21  1997/03/25 17:58:56  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.20  1997/03/19 19:41:05  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.19.4.2  1997/03/15 00:31:05  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.19.4.1  1997/02/27 22:46:05  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.19  1997/01/29 17:02:32  wenger
  Fixed PSWindowRep::ScaledText().

  Revision 1.18  1997/01/17 20:31:44  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.17  1996/12/30 23:51:10  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.16  1996/12/20 18:43:29  wenger
  Unfilled arcs and variable line widths now work in PostScript output.

  Revision 1.15  1996/12/15 20:22:30  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.14  1996/12/11 18:05:38  wenger
  Arc() method now works in PSWindowRep class; put SetSmallFont() method
  back into WindowRep classes for backwards compatibility for Opossum;
  server example program tests more of the WindowRep methods than before;
  removed 'not yet implemented' warnings from some PSWindowRep methods.

  Revision 1.13  1996/12/03 17:00:28  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.12  1996/11/26 15:44:07  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.11  1996/11/23 00:24:12  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.10  1996/11/18 23:11:18  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.9  1996/11/13 16:56:10  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.8  1996/11/07 22:40:11  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.7  1996/10/28 15:55:44  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.6  1996/10/18 20:34:07  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.5  1996/10/18 15:24:33  jussi
  Added RGB color selection methods for LIBCS compiles.

  Revision 1.4  1996/09/24 19:06:28  wenger
  PostScript code for lines, rects, text at least partially working (disabled
  for commit).  Still needs scaling and colors.  Added files for tarring
  sources to make transfer to Linux machines, etc., easier.

  Revision 1.3  1996/09/10 20:07:09  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.2  1996/07/10 16:25:33  jussi
  Removed _dispGraphics variable.

  Revision 1.1  1996/07/10 16:23:03  jussi
  Initial revision.
*/

#ifndef PSWindowRep_h
#define PSWindowRep_h

#include "WindowRep.h"
#include "Display.h"
#include "DList.h"
#include "Geom.h"
#include "Util.h"

#include "Color.h"

class PSWindowRep;

DefinePtrDList(PSWindowRepList, PSWindowRep *);

class PSWindowRep : public WindowRep
{
		friend class PSDisplay;
		friend class DualWindowRep;

	private:

		// Save previous foreground color to simulate XOR
		PColorID	oldForeground;

	protected:

		// Constructors and Destructors
		PSWindowRep(DeviseDisplay* display, PSWindowRep* parent,
					int x, int y, int width, int height); 
		/* called by PSDisplay to create new window */
		virtual ~PSWindowRep(void);

	public:

		// Getters and Setters
		virtual void	SetForeground(PColorID fgid);
		virtual void	SetBackground(PColorID bgid);

		// Utility Functions
		virtual void	SetWindowBackground(PColorID bgid);

		/* Reparent this window to 'other' or vice versa. */
    virtual void Reparent(Boolean child, void *other, int x, int y);

    /* Raise window to top of stacking order */
    virtual void Raise();

    /* Lower window to bottom of stacking order */
    virtual void Lower();

    /* Flush windowRep's content to display */
    virtual void Flush() {}

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

    /* import graphics via Dali */
    virtual void SetDaliServer(const char *serverName) { _daliServer =
      CopyString(serverName); }
    virtual DevStatus DaliShowImage(Coord centerX, Coord centerY,
                                    Coord width, Coord height,
                                    char *filename, int imageLen,
                                    char *image, float timeoutFactor = 1.0,
				    Boolean maintainAspect = true);
    virtual DevStatus DaliFreeImages() { return StatusOk; }
    virtual int DaliImageCount() { return 0; }
    
    /* drawing primitives */
    /* Return TRUE if window is scrollable */
    virtual Boolean Scrollable() { return false; }

    /* Scroll a region in the window */
    virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
                        Coord dstX, Coord dstY) {}

    /* Scroll absolute */
    virtual void ScrollAbsolute(int x, int y, unsigned width,
                                unsigned height, int dstX, int dstY) {}

    virtual void SetLineWidth(int w);
#if defined(LIBCS)
    virtual void SetDashes(int dashCount, int dashes[], int startOffset);
#endif

    virtual void FillRect
      (Coord xlow, Coord ylow, Coord width, Coord height,CursorStore *cstore=0);
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
                               Coord minWidth = 1.0, Coord minHeigh = 1.0,
			       SymbolAlignment alignment = AlignSouthWest,
			       Coord orientation = 0.0);
    virtual void FillPoly(Point *, int n);
    virtual void FillPixelPoly(Point *, int n);

    /* Draw an arc.  Angles are in radians. */
    virtual void Arc(Coord xCenter, Coord yCenter, Coord horizDiam,
                     Coord vertDiam, Coord startAngle, Coord endAngle);

    virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width,
                      CursorStore *cstore=0);
    virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width);

    virtual void ScaledText(char *text, Coord x, Coord y, Coord width,
                      Coord height, SymbolAlignment alignment = AlignCenter,
                      Boolean skipLeadingSpaces = false,
		      Coord orientation = 0.0);
    
    virtual void ScaledDataText(char *text, Coord x, Coord y, Coord width,
                      Coord height, SymbolAlignment alignment = AlignCenter,
                      Boolean skipLeadingSpaces = false,
		      Coord orientation = 0.0);
    
    virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
                              Coord height,
                              SymbolAlignment alignment = AlignCenter,
                              Boolean skipLeadingSpaces = false,
			      Coord orientation = 0.0);

    virtual void AbsoluteDataText(char *text, Coord x, Coord y, Coord width, 
                              Coord height,
                              SymbolAlignment alignment = AlignCenter,
                              Boolean skipLeadingSpaces = false,
			      Coord orientation = 0.0);

    /* Set XOR or normal drawing mode on */
    virtual void SetXorMode();
    virtual void SetCopyMode();

    /* Set font or return to normal font */
    virtual void SetFont(char *family, char *weight, char *slant,
                         char *width, float pointSize);
    virtual void SetNormalFont() { SetFont("Courier", "Medium", "r",
                                           "Normal", 12.0); }

    /* Draw rubberbanding rectangle */
    virtual void DrawRubberband(int x1, int y1, int x2, int y2) {}

    /* --------------------------------------------------------------------
     * Note: the PSWindowRep dimensions and origin are normally _not_
     * in points, but rather are the same as for the "sibling" XWindowRep. */

    /* Get window rep dimensions */
    virtual void Dimensions(unsigned int &width, unsigned int &height);

    /* Set window rep dimensions */
    virtual void SetDimensions(unsigned int width, unsigned int height);

    /* get window rep origin */
    virtual void Origin(int &x, int &y);

    /* Get absolute window rep origin from upper left corner of the screen */
    virtual void AbsoluteOrigin(int &x, int &y);

    /* Set window rep absolute origin */
    virtual void SetAbsoluteOrigin(int x, int y);

    /* Return contents of window as a pixmap */
    virtual DevisePixmap *GetPixmap() { return 0; }

    /* Display pixmap in window */
    virtual void DisplayPixmap(DevisePixmap *pixmap) {}

    /* Free pixmap from memory */
    virtual void FreePixmap(DevisePixmap *pixmap) {}

    virtual void Transform(Coord x, Coord y, Coord &newX, Coord &newY) {
      Coord tmpX, tmpY;
      _transforms[_current].Transform(x, y, tmpX, tmpY);
      TransPixToPoint(tmpX, tmpY, newX, newY);
    }	

    /* Set up the "pixel" to point transform according to the size and location
     * of the screen window, and the size of the output on the page. */
    void SetPPTrans(const Rectangle &viewDim, const Rectangle &parentDim);

    /* Transform from "pixels" to points. */
    virtual void TransPixToPoint(Coord oldX, Coord oldY, Coord &newX,
	Coord &newY) {
      _pixToPointTrans.Transform(oldX, oldY, newX, newY);
    }


protected:
	/* Called to initialize this object -- must be called ONLY when
     * the output file is open -- therefore CANNOT be called by the
     * constructor. */
    void Init();

private:
    /* Update window dimensions; globals: _x, _y, _width, _height */
    void UpdateWinDimensions();

    void DrawLine(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void DrawAlignedRect(FILE *printFile, Coord x1, Coord y1, Coord width,
      Coord height, SymbolAlignment alignment, Coord orientation);
    void DrawAbsRect(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void DrawRelRect(FILE *printFile, Coord width, Coord height);
    void SetClipPath(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void DrawDot(FILE *printFile, Coord x1, Coord y1, Coord size = 1.0);

    virtual void DrawText(Boolean scaled, char *text, Coord x, Coord y,
		      Coord width, Coord height,
		      SymbolAlignment alignment = AlignCenter,
                      Boolean skipLeadingSpaces = false,
		      Coord orientation = 0.0);

    virtual void DrawDataText(Boolean scaled, char *text, Coord x, Coord y,
		      Coord width, Coord height,
		      SymbolAlignment alignment = AlignCenter,
                      Boolean skipLeadingSpaces = false,
		      Coord orientation = 0.0);

    void GetAlignmentStrings(SymbolAlignment alignment, char *&comment,
      char *&moveToWindow, char *&moveToText);

    /* current dimensions of window */
    int _x, _y;
    unsigned int _width, _height;

    /* child/parent links for windows */
    PSWindowRep    *_parent;
    PSWindowRepList _children;

    /* Point size of current font */
    float _fontSize;

    /* "Pixel" to point transform. */
    Transform2D _pixToPointTrans;

    Boolean _xorMode;

#if defined(LIBCS)
    Boolean _dashedLine;
    const int _dashListSize = 1024;
    char _dashList[_dashListSize];
#endif

	char *_daliServer;
};

//******************************************************************************
#endif
