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
#include <libc.h>
#include "Geom.h"
#include "Transform.h"
#include "DList.h"
#include "Color.h"
#include "Pattern.h"
#include "Exit.h"
#include "DevisePixmap.h"

class DeviseDisplay;

/* Callback from windowRep for processing window events. 
   Default: no event is handled. */
class WindowRep;

class WindowRepCallback {
public:
  /* draw in the exposed area */
  virtual void HandleExpose(WindowRep * /*w*/, int /*x*/, int /*y*/, 
			    unsigned /*width*/, unsigned /*height*/) {}

  /* Handle button press event */
  virtual void HandlePress(WindowRep * /*w*/, int /*xlow*/, 
			   int /*ylow*/, int /*xhigh*/, int  /*yhigh*/,
			   int /*button*/) {}

  /* Handle resize */
  virtual void HandleResize(WindowRep * /*w*/, int /*xlow*/, 
			    int /*ylow*/, unsigned /*width*/,
			    unsigned /* height*/) {}
  
  virtual void HandleKey(WindowRep * ,char /*key*/, int /*x*/, int /*y*/) {}
  
  /* handle pop-up */
  virtual Boolean HandlePopUp(WindowRep *, int x, int y, int button,
			      char **&msgs, int &numMsgs) { return 0; }
	
  /* Handle map/unmap info.
     mapped : means window has been mapped.
     unmapped: means window has been unmapped. (This can also mean
     that window has been iconified) */
  virtual void HandleWindowMappedInfo(WindowRep *, Boolean /* mapped*/){};
};

const int WindowRepTransformDepth = 10;	/* max # of transforms in the stack */
const int WindowRepClipDepth = 10;	/* max # of clippings */

/* clipping rectangle */
struct ClipRect {
  Coord x,y,width,height;
};

DefinePtrDList(WindowRepCallbackList, WindowRepCallback *);
DefinePtrDList(ClipRectList, ClipRect *);

/* # of symbols that can be sent at once in a batched call */

const int WINDOWREP_BATCH_SIZE = 4096;

class WindowRep {
public:
  /* destructor */
  virtual ~WindowRep() {
    delete _callbackList;
  }
  
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

  /* drawing primitives */

  /* Return TRUE if window is scrollable */
  virtual Boolean Scrollable() = 0;

  /* Scroll a region in the window, done in world coordinates */
  virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
		      Coord dstX, Coord dstY) = 0;
	
  /* Scroll absoluate */
  virtual void ScrollAbsolute(int x, int y, unsigned width, unsigned height,
			      int dstX, int dstY) = 0;

  virtual void SetFgColor(Color fg){ _fgndColor = fg;}
  virtual void SetBgColor(Color bg){ _bgndColor = bg; }
  Color GetFgColor() { return _fgndColor; }
  Color GetBgColor() { return _bgndColor; }
  virtual void SetPattern(Pattern p){_pattern = p; }
  Pattern GetPattern(){ return _pattern;}
  virtual void FillRect(Coord xlow, Coord ylow, Coord width, Coord height) = 0;

  /* Fill rectangles, variable width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			     Coord *height, int num) = 0;
  /* Fill rectangles, same width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord width, 
			     Coord height, int num) = 0;

  virtual void DrawPixel(Coord x, Coord y)= 0;
  virtual void DrawPixelArray(Coord *x, Coord *y, int num, int width)= 0;
  /* fill rectangle. All coordinates are in pixels. x and y are
     at the center of the rectangle */
  virtual void FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			     Coord minWidth = 1.0, Coord minHeight = 1.0)= 0;
  virtual void FillPoly(Point *, int n) = 0;
  virtual void FillPixelPoly(Point *, int n)=0;
  virtual void Arc(Coord x, Coord y, Coord w, Coord h,
		   Coord startAngle, Coord endAngle)=0;
  /* draw line. end points are in world coord, but the width is in pixels */
  virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width)=0;
  virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width)=0;

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
		    Boolean skipLeadingSpaces = false)=0;
  
  /* draw absolute text: one that does not scale the text */
  virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
			    Coord height,
			    TextAlignment alignment = AlignCenter, 
			    Boolean skipLeadingSpaces = false)=0;

  /* Get window rep dimensions */
  virtual void Dimensions(unsigned int &width, unsigned int &height ) =0;
  
  /* get window rep origin from parent */
  virtual void Origin(int &x, int &y)=0;
  
  /* Get absolute window rep origin from upper left corner
     of the screen */
  virtual void AbsoluteOrigin(int &x,int &y)=0;
  
  /* get display of this Window Rep */
  DeviseDisplay *GetDisplay(){ return _display; };
  
  /* Transformation matrix operations */
  
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

  /* push clipping onto stack. The coords are defined with respect to the
     current trnasformation matrix. */
  virtual void PushClip(Coord x,Coord y,Coord w,Coord h)=0;
  /* pop the clip region. */
  virtual void PopClip()=0;
  
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
  virtual DevisePixmap *GetPixmap()=0;
  
  /* Display pixmap in window */
  virtual void DisplayPixmap(DevisePixmap *pixmap)=0;
  
  /* Free pixmap from memory */
  virtual void FreePixmap(DevisePixmap *pixmap)=0;

protected:

  /* called by derived class to cached current clip region */
  void _PushClip(Coord x,Coord y,Coord w,Coord h){
    if (_clipCurrent >= WindowRepClipDepth-1){
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
  
  /* called by derived class when button presssed: Report event to all
     callbacks */
  virtual void HandleButtonPress(int xlow, int ylow, 
				 int xhigh, int yhigh, int button);
  
  /* called by derived class when key pressed. report event to all
     callbacks */
  virtual void HandleKey(char key, int x, int y);
  
  /* Called by derived class on pop-up event. Report to all callbacks */
  virtual Boolean HandlePopUp(int x, int y, int button, char **&msgs,
			      int &numMsgs);

  /* called by dervied class on window mapped info change event.
     Report to all callbacks */
  virtual void HandleWindowMappedInfo(Boolean mapped);
  
  /* called by derived class to get currennt local color from
     global color */
  Color GetLocalColor(Color globalColor);
  
  /* constructor */
  WindowRep(DeviseDisplay *disp, 
	    Color fgndColor=WhiteColor, Color bgndColor=BlackColor,
		Pattern p = Pattern0);
  
private:
  /* DList<WindowRepCallback *> *_callbackList;*/
  WindowRepCallbackList  *_callbackList;
  Transform2D _transforms[WindowRepTransformDepth];
  ClipRect _clippings[WindowRepClipDepth];
  /* DList<ClipRect *> _damageRects; */
  ClipRectList  _damageRects; /* damaged areas*/
  int _clipCurrent;	/* index of top of stack for _clippings*/
  int _current; /* current index in the stack */
  Boolean _damaged ;
  Coord _x, _y, _width, _height; /* location and dimensions of window */
  Color _fgndColor, _bgndColor;
  Pattern _pattern;
  DeviseDisplay *_display;
};

#endif
