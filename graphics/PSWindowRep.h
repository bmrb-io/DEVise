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

    virtual void SetFgColor(Color bg);
    virtual void SetBgColor(Color bg);
    virtual void SetWindowBgColor(Color bg);

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
      _transforms[_current].Transform(x, y, newX, newY);
      TransPixToPoint(newX, newY);
    }	

    /* Transform from "pixels" to points. */
    virtual void TransPixToPoint(Coord &newX, Coord &newY) {
      newY *= -1.0;
      //TEMPTEMP -- need more scaling stuff here
    }


protected:
    friend class PSDisplay;

    /* called by PSDisplay to create new window */
    PSWindowRep(DeviseDisplay *display,
                Color fgndColor, Color bgndColor,
                PSWindowRep *parent, int x, int y); 

    /* called by constructors to initialize object */
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

    /* current dimensions of window */
    int _x, _y;
    unsigned int _width, _height;

    /* child/parent links for windows */
    PSWindowRep    *_parent;
    PSWindowRepList _children;
};

#endif
