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

class PSWindowRep;

DefinePtrDList(PSWindowRepList, PSWindowRep *);

class PSWindowRep: public WindowRep {
public:

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

    /* drawing primitives */
    /* Return TRUE if window is scrollable */
    virtual Boolean Scrollable() { return false; }

    /* Scroll a region in the window */
    virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
                        Coord dstX, Coord dstY) {}

    /* Scroll absolute */
    virtual void ScrollAbsolute(int x, int y, unsigned width,
                                unsigned height, int dstX, int dstY) {}

    virtual void SetFgColor(GlobalColor bg);
    virtual void SetBgColor(GlobalColor bg);
    virtual void SetWindowBgColor(GlobalColor bg);

#ifdef LIBCS
    /* Color selection interface using specific colors */
    virtual void SetFgRGB(float r, float g, float b) {}
    virtual void SetBgRGB(float r, float g, float b) {}
    virtual void GetFgRGB(float &r, float &g, float &b) {}
    virtual void GetBgRGB(float &r, float &g, float &b) {}
    virtual void SetWindowBgRGB(float r, float g, float b) {}
#endif

    virtual void FillRect(Coord xlow, Coord ylow, Coord width, Coord height);
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
    virtual void DrawRubberband(int x1, int y1, int x2, int y2) {}

    /* Get window rep dimensions */
    virtual void Dimensions(unsigned int &width, unsigned int &height);

    /* get window rep origin */
    virtual void Origin(int &x, int &y);

    /* Get absolute window rep origin from upper left corner of the screen */
    virtual void AbsoluteOrigin(int &x, int &y);

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
     * of the screen window, and the size of the page to output.  If
     * maintainAspect is true, the printed output will maintain the same
     * aspect ratio as the screen window.  Otherwise, the printed output
     * will be stretched to fill the page. */
    void SetPPTrans(const Rectangle &viewDim, const Rectangle &parentDim,
      Boolean maintainAspect = true);

    /* Transform from "pixels" to points. */
    virtual void TransPixToPoint(Coord oldX, Coord oldY, Coord &newX,
	Coord &newY) {
      _pixToPointTrans.Transform(oldX, oldY, newX, newY);
    }


protected:
    friend class PSDisplay;

    friend class DualWindowRep;

    /* called by PSDisplay to create new window */
    PSWindowRep(DeviseDisplay *display,
                GlobalColor fgndColor, GlobalColor bgndColor,
                PSWindowRep *parent, int x, int y); 

    /* Called to initialize this object -- must be called ONLY when
     * the output file is open -- therefore CANNOT be called by the
     * constructor. */
    void Init();

    /* destructor */
    ~PSWindowRep();

private:
    /* Update window dimensions; globals: _x, _y, _width, _height */
    void UpdateWinDimensions();

    void DrawLine(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void DrawFilledRect(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void SetClipPath(FILE *printFile, Coord x1, Coord y1, Coord x2,
      Coord y2);
    void DrawDot(FILE *printFile, Coord x1, Coord y1, Coord size = 1.0);

    /* current dimensions of window */
    int _x, _y;
    unsigned int _width, _height;

    /* child/parent links for windows */
    PSWindowRep    *_parent;
    PSWindowRepList _children;

    /* "Pixel" to point transform. */
    Transform2D _pixToPointTrans;
};

#endif
