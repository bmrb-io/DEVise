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
  Revision 1.29  1996/08/29 22:03:31  guangshu
  Changed ExportGIF argument to FILE *

  Revision 1.28  1996/08/28 00:19:38  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.27  1996/08/23 16:55:56  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.26  1996/08/04 21:05:35  beyer
  changed key handling

  Revision 1.25  1996/07/23 15:34:43  jussi
  Added mechanism for bypassing the Devise internal color map.

  Revision 1.24  1996/07/14 16:17:27  jussi
  Added destroyPending flag and IsDestroyPending() method.

  Revision 1.23  1996/07/14 04:03:24  jussi
  HandleWindowDestroy() now destroys the window, conditionally.
  Moved the destructor from the header file to the .c file.

  Revision 1.22  1996/07/10 18:59:22  jussi
  Moved 3D transform variables to WindowRep.

  Revision 1.21  1996/07/10 16:21:15  jussi
  Improvements and simplifications to the code.

  Revision 1.20  1996/06/21 19:28:02  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.

  Revision 1.19  1996/06/15 14:13:02  jussi
  Added yuc's 3D functions.

  Revision 1.18  1996/06/15 13:48:39  jussi
  Added SetWindowBgColor() which allows Devise to change
  the view background color at runtime.

  Revision 1.17  1996/05/31 15:32:10  jussi
  Added 'state' variable to HandleButton(). This tells the callee
  whether the shift/control keys were pressed in conjunction with
  the mouse button.

  Revision 1.16  1996/05/22 21:02:45  jussi
  Added ImportImage() method.

  Revision 1.15  1996/05/20 18:45:01  jussi
  Merged with ClientServer library code.

  Revision 1.14  1996/04/11 17:56:35  jussi
  Added Raise() and Lower().

  Revision 1.13  1996/02/28 17:46:21  yuc
  Added a bunch of public functions for 3D: MakeIdentity3,
  TopTransform3, PostMultiply, Transform, PrintTransform3,
  ClearTransformStack3, CompRhoPhiTheta, CompViewingTransf,
  CompLocationOnViewingSpace, MapAllPoints, and MapAllSegments, DrawRefAxis.
  Also added some protected variables: _rho, _phi, _theta, _twist_angle,
  _dvs, _camera, _TransformViewMatrix, _perspective, _NumXSegs, _AxisPt,
  and _Axis.

  Revision 1.12  1996/02/26 23:45:08  jussi
  Added GetSmallFontHeight().

  Revision 1.11  1996/02/05 23:55:15  jussi
  Added support for small fonts.

  Revision 1.10  1996/01/30 00:04:58  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.9  1996/01/17 20:54:11  jussi
  Added support for additional image export formats.

  Revision 1.8  1996/01/11 21:57:00  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.7  1995/12/28 20:45:38  jussi
  Changed the approach with which inverted lines and areas are
  drawn.

  Revision 1.6  1995/12/18 03:14:13  ravim
  Lines and Rectangles can be drawn with XOR display function.

  Revision 1.5  1995/12/02 21:33:14  jussi
  Added support for TK_WINDOW i.e. Tcl/Tk controls surrounding
  each view and window. This implementation was superseded by
  code in ViewWin.c and therefore was renamed TK_WINDOW_old.
  Added Reparent method to allow views and windows to become
  part of another window.

  Revision 1.4  1995/11/28  00:00:08  jussi
  Added WritePostscript() method.

  Revision 1.3  1995/11/24 21:26:21  jussi
  Added copyright notice and cleaned up code. Added PrintTransform
  method to help debugging.

  Revision 1.2  1995/09/05 21:13:35  jussi
  Added/updated CVS header.
*/

/* A WindowRep is an instantiation of a window.

   It has the following coordinates:
   (x,y)
   +----+
   |    | height
   +----+
   width
*/

#ifndef WindowRep_h
#define WindowRep_h

#include <stdio.h>
#include <stdlib.h>

#include "Geom.h"
#include "Transform.h"
#include "DList.h"
#include "Color.h"
#include "Pattern.h"
#include "Exit.h"
#include "DevisePixmap.h"
#include "VisualArg.h"

enum DisplayExportFormat { POSTSCRIPT, EPS, GIF };

class DeviseDisplay;

/* Callback from windowRep for processing window events. 
   Default: no event is handled. */
class WindowRep;

class WindowRepCallback {
public:
  /* draw in the exposed area */
  virtual void HandleExpose(WindowRep *w, int x, int y, 
			    unsigned width, unsigned height) {}

#ifdef RAWMOUSEEVENTS
  /* Handle button event */
  virtual void HandleButton(WindowRep *w, int x, int y,
			    int button, int state, int type) {}
#else
  /* Handle button press event */
  virtual void HandlePress(WindowRep *w, int xlow, int ylow,
                           int xhigh, int yhigh, int button) {}
#endif

  /* Handle resize */
  virtual void HandleResize(WindowRep *w, int xlow, int ylow,
                            unsigned width, unsigned height) {}
  
  /* Handle key press */
  virtual void HandleKey(WindowRep *w, int key, int x, int y) {}
  
#ifndef RAWMOUSEEVENTS
  /* handle pop-up */
  virtual Boolean HandlePopUp(WindowRep *w, int x, int y, int button,
			      char **&msgs, int &numMsgs) {
      return 0;
  }
#endif
	
  /* Handle map/unmap info.
     mapped : means window has been mapped.
     unmapped: means window has been unmapped. (This can also mean
     that window has been iconified) */
  virtual void HandleWindowMappedInfo(WindowRep *w, Boolean mapped) {}

  /* Handle window destroy events */
  virtual Boolean HandleWindowDestroy(WindowRep *w) { return true; }
};

const int WindowRepTransformDepth = 10;	/* max # of transforms in the stack */
const int WindowRepClipDepth = 10;	/* max # of clippings */

/* clipping rectangle */
struct ClipRect {
  Coord x, y, width, height;
};

DefinePtrDList(WindowRepCallbackList, WindowRepCallback *);
DefinePtrDList(ClipRectList, ClipRect *);

/* # of symbols that can be sent at once in a batched call */

const int WINDOWREP_BATCH_SIZE = 1024;

class WindowRep {
public:
  /* destructor */
  virtual ~WindowRep();
  
#ifdef TK_WINDOW_old
  /* Decorate window */
  virtual void Decorate(WindowRep *parent, char *name,
			unsigned int min_width,
			unsigned int min_height) = 0;

  /* Undecorate window */
  virtual void Undecorate() = 0;
#endif

  /* Reparent this window to 'other' or vice versa. */
  virtual void Reparent(Boolean child, void *other, int x, int y) = 0;

  /* Raise window to top of stacking order */
  virtual void Raise() = 0;

  /* Lower window to bottom of stacking order */
  virtual void Lower() = 0;

  /* Flush windowRep's content to display */
  virtual void Flush() = 0;

  /* Move and resize window, relative to the parent */
  virtual void MoveResize(int x, int y, unsigned int w, unsigned int h) = 0;

  /* Iconify window. Not guaranteed to succeed.  */
  virtual void Iconify() = 0;

  /* Register a callback to be called when an event occurs */
  void RegisterCallback(WindowRepCallback *c){
    _callbackList->Append(c);
  }

  /* delete callback. Return TRUE if found */
  Boolean DeleteCallback(WindowRepCallback *c) {
    return _callbackList->Delete(c);
  }

  /* import other graphics and display in window */
  virtual void ImportImage(Coord x, Coord y,
			   DisplayExportFormat format, char *filename) {}

  /* export window image to other graphics formats */
  virtual void ExportImage(DisplayExportFormat format, char *filename) {}
  virtual void ExportGIF(FILE *fp) {}


  /* import graphics via Dali */
  virtual DevStatus DaliShowImage(Coord centerX, Coord centerY, Coord width,
    Coord height, char *filename, int imageLen, char *image)
    { return StatusOk; }
  virtual DevStatus DaliFreeImages() { return StatusOk; }
  virtual int DaliImageCount() { return 0; }

  /* drawing primitives */

  /* Return TRUE if window is scrollable */
  virtual Boolean Scrollable() = 0;

  /* Scroll a region in the window, done in world coordinates */
  virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
		      Coord dstX, Coord dstY) = 0;
	
  /* Scroll absolute */
  virtual void ScrollAbsolute(int x, int y, unsigned width, unsigned height,
			      int dstX, int dstY) = 0;

  /* Color selection interface using Devise colormap */
  virtual void SetFgColor(Color fg) { _fgndColor = fg; }
  virtual void SetBgColor(Color bg) { _bgndColor = bg; }
  virtual Color GetFgColor() { return _fgndColor; }
  virtual Color GetBgColor() { return _bgndColor; }
  virtual void SetWindowBgColor(Color bg) = 0;

#ifdef LIBCS
  /* Color selection interface using specific colors */
  virtual void SetFgRGB(float r, float g, float b) = 0;
  virtual void SetBgRGB(float r, float g, float b) = 0;
  virtual void GetFgRGB(float &r, float &g, float &b) = 0;
  virtual void GetBgRGB(float &r, float &g, float &b) = 0;
  virtual void SetWindowBgRGB(float r, float g, float b) = 0;
#endif

  virtual void SetPattern(Pattern p) { _pattern = p; }
  Pattern GetPattern(){ return _pattern; }

  virtual void FillRect(Coord xlow, Coord ylow,
			Coord width, Coord height) = 0;

  /* Fill rectangles, variable width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			     Coord *height, int num) = 0;
  /* Fill rectangles, same width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord width, 
			     Coord height, int num) = 0;

  virtual void DrawPixel(Coord x, Coord y) = 0;
  virtual void DrawPixelArray(Coord *x, Coord *y, int num, int width) = 0;
  /* fill rectangle. All coordinates are in pixels. x and y are
     at the center of the rectangle */
  virtual void FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			     Coord minWidth = 1.0, Coord minHeight = 1.0) = 0;
  virtual void FillPoly(Point *, int n) = 0;
  virtual void FillPixelPoly(Point *, int n) = 0;
  virtual void Arc(Coord x, Coord y, Coord w, Coord h,
		   Coord startAngle, Coord endAngle) = 0;
  /* draw line. end points are in world coord, but the width is in pixels */
  virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width) = 0;
  virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width) = 0;

  /* draw and scaled text to fit inside box, according to alignment.
     If skipLeadingSpaces == TRUE, then the leading spaces are used to
     scale the text inside the box, but not used in alignment 
     calculations or for drawing. Padding all texts with leading spaces
     so that they are the same lenght and setting skipLeadingSpaces
     to TRUE ensures that the texts are draw with the same size */
  enum TextAlignment { AlignNorthWest, AlignNorth, AlignNorthEast, 
			 AlignWest, AlignCenter, AlignEast, AlignSouthWest,
			 AlignSouth, AlignSouthEast};
  virtual void Text(char *text, Coord x, Coord y, Coord width, Coord height,
		    TextAlignment alignment = AlignCenter, 
		    Boolean skipLeadingSpaces = false) = 0;
  
  /* draw absolute text: one that does not scale the text */
  virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
			    Coord height,
			    TextAlignment alignment = AlignCenter, 
			    Boolean skipLeadingSpaces = false) = 0;

  /* Set XOR or normal drawing mode on */
  virtual void SetXorMode() = 0;
  virtual void SetCopyMode() = 0;

  /* Set normal or small font */
  virtual void SetNormalFont() = 0;
  virtual void SetSmallFont() = 0;
  virtual int  GetSmallFontHeight() = 0;

  /* Get window rep dimensions */
  virtual void Dimensions(unsigned int &width, unsigned int &height ) = 0;
  
  /* get window rep origin from parent */
  virtual void Origin(int &x, int &y) = 0;
  
  /* Get absolute window rep origin from upper left corner
     of the screen */
  virtual void AbsoluteOrigin(int &x,int &y) = 0;
  
  /* get display of this Window Rep */
  DeviseDisplay *GetDisplay() { return _display; };
  
  /* Get flag which indicates if window destroy is pending */
  static Boolean IsDestroyPending() { return _destroyPending; }

  // ---------------------------------------------------------- 

  /* 2D transformation matrix operations */
  
  /* Push a copy of the top of stack onto the stack */
  void PushTop() {
    if (_current >= WindowRepTransformDepth-1 ){
      fprintf(stderr,"WindowRep::PushTop: overflow\n");
      Exit::DoExit(1);
    };
    _transforms[_current+1].Copy(_transforms[_current]);
    _current++;
  }
  
  /* pop transformation matrix */
  void PopTransform(){
    if (_current <= 0){
      fprintf(stderr,"WindowRep::PopTransform: underflow\n");
      Exit::DoExit(1);
    }
    _current--;
  }

  /* operations on current transformation matrix */
  void Scale(Coord sx, Coord sy){
    _transforms[_current].Scale(sx,sy);
  }

  void Translate(Coord dx, Coord dy){
    _transforms[_current].Translate(dx,dy);
  }

  void MakeIdentity() {
    _transforms[_current].MakeIdentity();
  }

  /* return the transform on top of the stack. */
  Transform2D *TopTransform() {
    return &_transforms[_current];
  }

  void PostMultiply(Transform2D *t) {
    _transforms[_current].PostMultiply(t);
  }

  void Transform(Coord x, Coord y, Coord &newX, Coord &newY) {
    _transforms[_current].Transform(x,y,newX,newY);
  }

  void PrintTransform() {
    _transforms[_current].Print();
  }

  /* Clear the transformation stack and put an identity 
     matrix as top of the stack */
  void ClearTransformStack() {
    _current = 0;
    MakeIdentity();
  }

  // ---------------------------------------------------------- 

  /* 3D transformation matrix operations */
  
  /* Push a copy of the top of stack onto the stack */
  void PushTop3() {
    if (_current3 >= WindowRepTransformDepth-1 ){
      fprintf(stderr,"WindowRep::PushTop: overflow\n");
      Exit::DoExit(1);
    };
    _transforms3[_current3+1].Copy(_transforms3[_current3]);
    _current3++;
  }
  
  /* pop transformation matrix */
  void PopTransform3(){
    if (_current3 <= 0){
      fprintf(stderr,"WindowRep::PopTransform: underflow\n");
      Exit::DoExit(1);
    }
    _current3--;
  }

  /* operations on current transformation matrix */
  void Scale3(Coord sx, Coord sy, Coord sz) {
    _transforms3[_current3].Scale(sx,sy,sz);
  }

  void Translate3(Coord dx, Coord dy, Coord dz) {
    _transforms3[_current3].Translate(dx,dy,dz);
  }

  void MakeIdentity3() {
    _transforms3[_current3].MakeIdentity();
  }

  /* return the transform on top of the stack. */
  Transform3D *TopTransform3() {
    return &_transforms3[_current3];
  }

  void PostMultiply3(Transform3D *t) {
    _transforms3[_current3].PostMultiply(t);
  }

  void Transform3(Coord x, Coord y, Coord z,
                  Coord &newX, Coord &newY, Coord &newZ) {
    _transforms3[_current3].Transform(x,y,z,newX,newY,newZ);
  }

  void PrintTransform3() {
    _transforms3[_current3].Print();
  }

  /* Clear the transformation stack and put an identity 
     matrix as top of the stack */
  void ClearTransformStack3() {
    _current3 = 0;
    MakeIdentity3();
  }

  // ---------------------------------------------------------- 

  /* push clipping onto stack. The coords are defined with respect to the
     current trnasformation matrix. */
  virtual void PushClip(Coord x,Coord y,Coord w,Coord h) = 0;
  /* pop the clip region. */
  virtual void PopClip() = 0;
  
  /* called by derived class to get current clip region */

  Boolean ClipTop(Coord &x, Coord &y, Coord &w, Coord &h){
    if (_clipCurrent < 0)
      return false;
    else {
      ClipRect *rect = &_clippings[_clipCurrent];
      x = rect->x; y = rect->y; w = rect->width; h = rect->height;
      return true;
    }
  }

  /******* Pixmap ***************/
  /* Generate pixmap for current window. Return NULL
     if insufficient memory. */
  virtual DevisePixmap *GetPixmap() = 0;
  
  /* Display pixmap in window */
  virtual void DisplayPixmap(DevisePixmap *pixmap) = 0;
  
  /* Free pixmap from memory */
  virtual void FreePixmap(DevisePixmap *pixmap) = 0;

protected:

  /* called by derived class to cached current clip region */
  void _PushClip(Coord x, Coord y, Coord w, Coord h) {
    if (_clipCurrent >= WindowRepClipDepth - 1){
      fprintf(stderr,"WindowRep::PushClip: overflow\n");
      Exit::DoExit(1);
    };
    ClipRect *rect = &_clippings[++_clipCurrent];
    rect->x = x; rect->y = y; rect->width = w; rect->height = h;
  }
  
  /* called by derived class to pop clip region */
  void _PopClip(){
    if (_clipCurrent < 0){
      fprintf(stderr,"WindowRep::PopClip: underflow\n");
      Exit::DoExit(1);
    }
    _clipCurrent--;
  }

  /* called by derived classes to iterate through the callbacks */
  int InitIterator(){ return _callbackList->InitIterator();};
  Boolean More(int index) { return _callbackList->More(index);}
  WindowRepCallback *Next(int index) { return _callbackList->Next(index);}
  void DoneIterator(int index) { _callbackList->DoneIterator(index);};

  /* Called by derived class when part of window is exposed */
  virtual void HandleExpose(int x, int y, unsigned w, unsigned h);
  
  /* called by derived class to when window is resized or moved: 
     Update the current size and inform all callbacks. */
  virtual void HandleResize(int x, int y, unsigned width, unsigned height);
  
#ifdef RAWMOUSEEVENTS
  /* called by derived class with button even */
  virtual void HandleButton(int x, int y, int button, int state, int type);
#else
  /* called by derived class when button presssed: Report event to all
     callbacks */
  virtual void HandleButtonPress(int xlow, int ylow, 
				 int xhigh, int yhigh, int button);
#endif
  
  /* called by derived class when key pressed. report event to all
     callbacks */
  virtual void HandleKey(int key, int x, int y);
  
#ifndef RAWMOUSEEVENTS
  /* Called by derived class on pop-up event. Report to all callbacks */
  virtual Boolean HandlePopUp(int x, int y, int button, char **&msgs,
			      int &numMsgs);
#endif

  /* called by dervied class on window mapped info change event.
     Report to all callbacks */
  virtual void HandleWindowMappedInfo(Boolean mapped);
  
  /* called by derived class on window destroy event */
  virtual void HandleWindowDestroy();

  /* called by derived class to get currennt local color from
     global color */
  Color GetLocalColor(Color globalColor);
  
  /* constructor */
  WindowRep(DeviseDisplay *disp, Color fgndColor = ForegroundColor,
	    Color bgndColor = BackgroundColor, Pattern p = Pattern0);
  
  WindowRepCallbackList  *_callbackList;

  Transform2D _transforms[WindowRepTransformDepth];
  int _current;         /* current index in the stack */
  ClipRect _clippings[WindowRepClipDepth];
  int _clipCurrent;	/* index of top of stack for _clippings*/

  Transform3D _transforms3[WindowRepTransformDepth];
  int _current3;        /* current index in the stack */

  ClipRectList  _damageRects;     /* damaged areas */
  Boolean _damaged;

  Coord _x, _y, _width, _height;  /* location and dimensions of window */
  Color _fgndColor, _bgndColor;   /* current fg and bg colors */
  Pattern _pattern;               /* current pattern */
  DeviseDisplay *_display;        /* display object */

  static Boolean _destroyPending; /* true if window destroy is pending */
};

#endif
