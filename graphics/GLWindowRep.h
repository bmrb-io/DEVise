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

*/

#ifndef GLWindowRep_h
#define GLWindowRep_h

#include <string.h>
#include <math.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "WindowRep.h"
#include "Xdef.h"
#include "DList.h"
#include "Util.h"

#include "Color.h"

class GLWindowRep;

DefinePtrDList(GLWindowRepList, GLWindowRep *);
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

class GLDisplay;
class Compression;

class GLWindowRep: public WindowRep {
  friend class GLDisplay;
public:

  /* called by GLDisplay to create new X window */
  GLWindowRep(Display *display, Window window, GLDisplay *DVDisp, 
	      GLWindowRep *parent, Boolean backingStore = false); 

  /* called by GLDisplay to create new X pixmap */
  GLWindowRep(Display *display, Pixmap pixmap, GLDisplay *DVDisp, 
	      GLWindowRep *parent, int x, int y); 

  ~GLWindowRep();

public:
// Getters and Setters
  virtual void    SetForeground(PColorID fgid);
  virtual void    SetBackground(PColorID bgid);

  // Utility Functions
  virtual void    SetWindowBackground(PColorID bgid);
  void    ClearPixmap(void);


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

  virtual void SetPattern(Pattern p);

  virtual void SetLineWidth(int w);
#ifdef LIBCS
  virtual void SetDashes(int dashCount, int dashes[], int startOffset);
#endif
  /* operations on current transformation matrix */
  virtual void PushTop();
  virtual void PopTransform();
  virtual void Scale(Coord sx, Coord sy);
  virtual void Translate(Coord dx, Coord dy);
  virtual void MakeIdentity();
  virtual void InverseTransform(Coord x, Coord y, Coord &newX, Coord &newY);
  // Convert win coordinate to object coordinate

  /* return the transform on top of the stack. */
  virtual Transform2D *TopTransform();
  virtual void PrintTransform();

  /* Clear the transformation stack and put an identity 
     matrix as top of the stack */
  void ClearTransformStack();
  /* Drawing primitives */
  /*virtual void ClearBackground(Coord xlow, Coord ylow, Coord width,
                        Coord height)
  {
	// will be inefficient to call FillRect
	// not implemented currently
  }*/
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
  virtual void PrintDimensions();

  /* Get window rep origin */
  virtual void Origin(int &x, int &y);

  /* Get absolute window rep origin from upper left corner
     of the screen */
  virtual void AbsoluteOrigin(int &x, int &y);

  /* get display of this Window Rep */
  GLDisplay* GetDisplay() { return (GLDisplay*)WindowRep::GetDisplay(); }


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
  virtual void EmbedInTkWindow(GLWindowRep *parent,
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
  static void CoalescePixmaps(GLWindowRep *root);
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

/*
  In XWindowRep, this is to draw rectangle(s) at absolute values of screen.
  Not useful for openGL .

  void DrawRectangle(int symbolX, int symbolY, int width, int height,
		     Boolean filled = true,
		     SymbolAlignment alignment = AlignCenter,
		     Coord orientation = 0.0);

  void DrawRectangles(XRectangle rectangles[], int rectCount,
		      Boolean filled = true,
		      SymbolAlignment alignment = AlignCenter,
		      Coord orientation = 0.0);
*/

  /* Calculate the locations of points needed to draw a rectangle,
   * based on the alignment and orientation.  points array is only
   * needed if orientation is non-zero.  If used, it must have a
   * size of 5 (NOT 4!). */
  static void CalculateLocation(Coord centerx, Coord centery,
				Coord width, Coord height,
				SymbolAlignment alignment,
				Coord orientation,
				GLfloat point_x[4], GLfloat point_y[4]);

  /* current dimensions of window */

  int _x, _y;
  unsigned int _width, _height;

  Display *_display;
  Window _win;
  GLXContext _gc;  

  /* pixmap and child/parent links for pixmaps */
  Pixmap _pixmap;
  GLWindowRep    *_parent;
  GLWindowRepList _children;
  
  /* GC for rubber-banding */
  //GC _rectGc;
  
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

  // for openGL
  static GLWindowRep * currWinRep; // current gl graphical context
  GLboolean makeCurrent();
  GLboolean double_buffer;
  XFontStruct *_specialfontstruct;
  GLuint _specialfont;
  XFontStruct *_normalfontstruct;
  GLuint _normalfont;
  XFontStruct *_currentfontstruct;
  GLuint _currentfont;
  friend class RubberbandBuffer {
  public:
    RubberbandBuffer(int xx1, int yy1, int xx2, int yy2);
    ~RubberbandBuffer();
  private:
    GLubyte *Buffer1, *Buffer2, *Buffer3, *Buffer4;
    GLubyte *altRow, *altCol;
    int x1, y1, x2, y2;
  } ;
};

#endif



