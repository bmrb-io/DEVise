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
  Revision 1.69  1998/04/13 22:24:59  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.68  1998/03/26 15:21:43  zhenhai
  The cursor drawings now use CursorStore as backup instead of using
  XOR mode for drawing and erasing.

  Revision 1.67  1998/03/18 08:19:46  zhenhai
  Added visual links between 3D graphics.

  Revision 1.66  1998/03/05 08:10:29  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.65  1998/02/26 00:19:07  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.64  1998/02/16 15:41:27  wenger
  Fixed (I believe) bug 287.

  Revision 1.63  1998/02/09 17:29:08  wenger
  Conditionaled out some debug output.

  Revision 1.62  1998/02/04 20:22:05  zhenhai
  Fixed bugs of displaying date and customized text at axis.

  Revision 1.61  1998/01/23 20:37:54  zhenhai
  Fixed a bug on transformation which was caused by inconsistency between origins
  or XWindows (Upper left) and OpenGL (Lower left). Also fixed a bug for
  incorrect labelling of axis.

  Revision 1.60  1998/01/14 16:38:57  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.59  1997/12/18 18:40:18  wenger
  Turned off debug output, cleaned things up a little bit in OpenGL code.

  Revision 1.58  1997/12/16 17:53:54  zhenhai
  Added OpenGL features to graphics.

  Revision 1.57  1997/11/24 23:14:39  weaver
  Changes for the new ColorManager.

  Revision 1.56  1997/11/12 15:45:26  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.55.4.2  1998/01/13 18:27:45  wenger
  Printing display now works in batch mode (pixmaps);  cleaned up
  WindowRep classes slightly; interrupted system calls in server code
  don't cause server to exit.

  Revision 1.55.4.1  1997/11/11 19:13:46  wenger
  Added getWindowImageAndSize and waitForQueries commands; fixed bug in
  WindowRep::ExportGIF() inheritance.

  Revision 1.55  1997/08/12 15:32:13  wenger
  Removed unnecessary include dependencies.

  Revision 1.54  1997/07/18 20:25:06  wenger
  Orientation now works on Rect and RectX symbols; code also includes
  some provisions for locating symbols other than at their centers.

  Revision 1.53  1997/06/13 18:07:28  wenger
  Orientation is now working for text labels and fixed text labels.

  Revision 1.52  1997/06/10 18:38:25  wenger
  Got DEVise to compile on HP again; updated hp, solaris, and sun dependencies.

  Revision 1.51  1997/06/04 15:50:32  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.50  1997/05/29 15:21:23  wenger
  Updated hp, solaris, and sun dependencies; fixes to make new LM stuff
  compile on linux.

  Revision 1.49  1997/05/21 22:10:00  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.48  1997/05/05 16:53:47  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.47  1997/04/21 15:02:02  wenger
  Added DrawRubberband() virtual function to WindowRep class, so that it
  doesn't have to be called directly through the XWindowRep class;
  cleaned out unnecessary includes from the cslib example server code and
  distribution.

  Revision 1.46  1997/04/11 18:48:53  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.45  1997/03/25 17:59:01  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.44  1997/03/19 19:41:09  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.43.4.1  1997/02/27 22:46:06  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.43  1997/01/17 20:31:48  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.42  1997/01/09 18:41:18  wenger
  Added workarounds for some Tasvir image bugs, added debug code.

  Revision 1.41  1996/12/30 23:51:13  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.40  1996/12/15 20:22:30  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.39  1996/12/11 18:05:38  wenger
  Arc() method now works in PSWindowRep class; put SetSmallFont() method
  back into WindowRep classes for backwards compatibility for Opossum;
  server example program tests more of the WindowRep methods than before;
  removed 'not yet implemented' warnings from some PSWindowRep methods.

  Revision 1.38  1996/12/04 22:38:54  wenger
  Fixed bug 087 (view removal crash); noted several other fixed bugs
  in the bug list.

  Revision 1.37  1996/12/03 17:00:26  jussi
  Added SetFont() for generic font support. Removed SetSmallFont().

  Revision 1.36  1996/11/26 16:47:48  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.35  1996/11/26 15:44:12  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.34  1996/11/13 16:56:17  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.33  1996/10/28 15:55:48  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.32  1996/10/18 20:34:12  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.31  1996/09/06 06:59:45  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.30  1996/09/04 21:24:50  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

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

#include "Geom.h"
#include "Transform.h"
#include "DList.h"
#include "Pattern.h"
#include "Exit.h"
#include "DevisePixmap.h"
#include "VisualArg.h"
#include "DevError.h"
#include "ETkIfc.h"

/* Note: this is defined explicitly here, instead of including machdep.h,
 * because that breaks the compile on HPs.  RKW June 10, 1997. */
#ifndef FILENAME_MAX
#define FILENAME_MAX    1024    /* max # of characters in a path name */
#endif

#include "GraphicsDebug.h"

#include "Color.h" // Note: this includes X-specific stuff -- bad!!
#include "Coloring.h"
#include "CursorStore.h"

enum DisplayExportFormat { POSTSCRIPT, EPS, GIF };

class DeviseDisplay;

/* Callback from windowRep for processing window events. 
   Default: no event is handled. */
class WindowRep;

//******************************************************************************
// class WindowRepCallback
//******************************************************************************

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

//
// Struct to maintain info about a single embedded Tk window
//
struct ETkInfo
{
    int handle;
    Coord x;
    Coord y;
    Coord width;
    Coord height;
    char script[FILENAME_MAX + 1];
    bool in_use;
};

//******************************************************************************
// class WindowRep
//******************************************************************************

class WindowRep
{
	protected:

		DeviseDisplay*	_display;	// Display object
		Coloring	coloring;	// Coloring of shape being drawn

	public:

		// Constructors and Destructors
		WindowRep(DeviseDisplay* disp, Pattern p = Pattern0);
		virtual ~WindowRep(void);

		// Getters and Setters
		DeviseDisplay*	 	GetDisplay(void)		{ return _display;	};
		const DeviseDisplay*	GetDisplay(void) const	{ return _display;	};

		Coloring&				GetColoring(void)		{ return coloring;	}
		const Coloring&			GetColoring(void) const { return coloring;	}

		PColorID		GetForeground(void) const;
		PColorID		GetBackground(void) const;
		virtual void	SetForeground(PColorID fgid);
		virtual void	SetBackground(PColorID bgid);

		// Utility Functions
		virtual void	SetWindowBackground(PColorID bgid) = 0;

public:

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

  /* Find out whether window has backing store. */
  virtual Boolean HasBackingStore() { return false; }
	
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
  virtual void ExportGIF(FILE *fp, int isView = 0) {}


  /* import graphics via Dali */
  virtual void SetDaliServer(const char *serverName) { DOASSERT(false,
    "Can't do SetDaliServer() on this object"); }
  virtual DevStatus DaliShowImage(Coord centerX, Coord centerY, Coord width,
    Coord height, char *filename, int imageLen, char *image,
    float timeoutFactor = 1.0, Boolean maintainAspect = true)
    { reportErrNosys("This object does not support Tasvir operations");
      return StatusFailed; }
  virtual DevStatus DaliFreeImages()
    { reportErrNosys("This object does not support Tasvir operations");
      return StatusFailed; }
  virtual int DaliImageCount() { return 0; }

  /* Display embedded Tk (ETk) windows */
  virtual void SetETkServer(const char *serverName)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
  }
  virtual DevStatus ETk_CreateWindow(Coord x, Coord y,
				     Coord width, Coord height,
				     ETkIfc::Anchor anchor,
				     char *filename,
				     int argc, char **argv,
				     int &handle)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual int ETk_FindWindow(Coord centerX, Coord centerY, char *script)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return -1;
  }
  virtual DevStatus ETk_MoveWindow(int handle, Coord centerX, Coord centerY)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_ResizeWindow(int handle, Coord width, Coord height)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_NotifyResize(int handle, Coord x, Coord y,
				     Coord width, Coord height)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_MoveResizeWindow(int handle,
					 Coord centerX, Coord centerY,
					 Coord centerX, Coord centerY)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_FreeWindow(int handle)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_MapWindow(int handle)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_UnmapWindow(int handle)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_EvalCmd(int handle, int argc, char **argv,
				char *&returnValue)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_FreeWindows()
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_Cleanup()
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_Mark(int handle, bool in_use)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual DevStatus ETk_MarkAll(bool in_use)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual int ETk_WindowCount()
  {
      return 0;
  }
  virtual DevStatus ETk_GetInfo(int handle, ETkInfo &info)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return StatusFailed;
  }
  virtual int ETk_InitIterator()
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return -1;
  }
  virtual Boolean ETk_More(int index)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return false;
  }
  virtual ETkInfo *ETk_Next(int index)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
      return NULL;
  }
  virtual void ETk_DoneIterator(int index)
  {
      reportErrNosys("This object does not support EmbeddedTk operations");
  }
  
  /* drawing primitives */
  enum SymbolAlignment { AlignNorthWest, AlignNorth, AlignNorthEast, 
			 AlignWest, AlignCenter, AlignEast, AlignSouthWest,
			 AlignSouth, AlignSouthEast};

  /* Return TRUE if window is scrollable */
  virtual Boolean Scrollable() = 0;

  /* Scroll a region in the window, done in world coordinates */
  virtual void Scroll(Coord x, Coord y, Coord width, Coord height,
		      Coord dstX, Coord dstY) = 0;
	
  /* Scroll absolute */
  virtual void ScrollAbsolute(int x, int y, unsigned width, unsigned height,
			      int dstX, int dstY) = 0;

  virtual void SetPattern(Pattern p) { _pattern = p; }
  Pattern GetPattern(){ return _pattern; }

  virtual void SetLineWidth(int w) { _line_width = w; }
  int GetLineWidth(){ return _line_width; }
#ifdef LIBCS
  virtual void SetDashes(int dashCount, int dashes[], int startOffset) {}
#endif
  virtual void SetNumDim(int numDim) {
    _numDim=numDim;
  }

  virtual int GetNumDim() {
    return _numDim;
  }

  // Use FillRect is inefficient for openGL
  virtual void ClearBackground(Coord xlow, Coord ylow,
			Coord width, Coord height) {
	FillRect(xlow, ylow, width, height);
  }

  virtual void FillRect(Coord xlow, Coord ylow,
			Coord width, Coord height, CursorStore *cstore=0) = 0;
  virtual void FillRectAlign(Coord xlow, Coord ylow, Coord width,
			     Coord height,
			     SymbolAlignment alignment = AlignSouthWest,
			     Coord orientation = 0.0) = 0;

  /* Fill rectangles, variable width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
			     Coord *height, int num,
			     SymbolAlignment alignment = AlignSouthWest,
			     Coord orientation = 0.0) = 0;
  /* Fill rectangles, same width/height */
  virtual void FillRectArray(Coord *xlow, Coord *ylow, Coord width, 
			     Coord height, int num,
			     SymbolAlignment alignment = AlignSouthWest,
                             Coord orientation = 0.0) = 0;

  virtual void DrawPixel(Coord x, Coord y) = 0;
  virtual void DrawPixelArray(Coord *x, Coord *y, int num, int width) = 0;
  /* fill rectangle. All coordinates are in pixels. x and y are
     at the center of the rectangle */
  virtual void FillPixelRect(Coord x, Coord y, Coord width, Coord height,
			     Coord minWidth = 1.0, Coord minHeight = 1.0,
			     SymbolAlignment alignment = AlignSouthWest,
                             Coord orientation = 0.0) = 0;

  virtual void FillPoly(Point *, int n) = 0;
  virtual void FillPoly3D(Point3D *, int n) {};

  virtual void FillPixelPoly(Point *, int n) = 0;

  /* Draw an arc.  Angles are in radians, end is _relative_ to start. */
  virtual void Arc(Coord xCenter, Coord yCenter, Coord horizDiam,
		   Coord vertDiam, Coord startAngle, Coord endAngle) = 0;

  /* draw line. end points are in world coord, but the width is in pixels */
  virtual void Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width,
                    CursorStore *cstore=0) = 0;
  virtual void Line3D(Coord x1, Coord y1, Coord z1,
		      Coord x2, Coord y2, Coord z2, Coord width,
                      CursorStore *cstore=0) {};
  virtual void Text3D(Coord x, Coord y, Coord z, char* text) {};
  virtual void AbsoluteLine(int x1, int y1, int x2, int y2, int width) = 0;

  /* draw and scaled text to fit inside box, according to alignment.
     If skipLeadingSpaces == TRUE, then the leading spaces are used to
     scale the text inside the box, but not used in alignment 
     calculations or for drawing. Padding all texts with leading spaces
     so that they are the same lenght and setting skipLeadingSpaces
     to TRUE ensures that the texts are draw with the same size */
  virtual void ScaledText(char *text, Coord x, Coord y, Coord width,
		    Coord height, SymbolAlignment alignment = AlignCenter, 
		    Boolean skipLeadingSpaces = false,
		    Coord orientation = 0.0) = 0;
  
  virtual void ScaledDataText(char *text, Coord x, Coord y, Coord width,
		    Coord height, SymbolAlignment alignment = AlignCenter, 
		    Boolean skipLeadingSpaces = false,
		    Coord orientation = 0.0) = 0;
  
  /* draw absolute text: one that does not scale the text */
  virtual void AbsoluteText(char *text, Coord x, Coord y, Coord width, 
			    Coord height,
			    SymbolAlignment alignment = AlignCenter, 
			    Boolean skipLeadingSpaces = false, 
			    Coord orientation = 0.0) = 0;

  virtual void AbsoluteDataText(char *text, Coord x, Coord y, Coord width, 
			    Coord height,
			    SymbolAlignment alignment = AlignCenter, 
			    Boolean skipLeadingSpaces = false, 
			    Coord orientation = 0.0) = 0;

  virtual void FillTriangle3D
	(Point3D p1, Point3D p2, Point3D p3) {};

  virtual void FillSphere(Coord x, Coord y, Coord z, Coord r){};
  // (x, y, z) = origin of sphere
  //        r  = radius of sphere

  virtual void FillCone(Coord x0, Coord y0, Coord z0,
                        Coord x1, Coord y1, Coord y2, Coord r,
                        CursorStore* cstore=0) {};
  // (x0, y0, z0) = center of base of cone
  // (x1, y1, z1) = top point of cone


  /* Set XOR or normal drawing mode on */
  virtual void SetXorMode() = 0;
  virtual void SetCopyMode() = 0;
  virtual void SetOrMode() {}

  /* Set font or return to normal */
  virtual void SetFont(char *family, char *weight, char *slant,
                       char *width, float pointSize) = 0;
  virtual void SetNormalFont() = 0;
  virtual void SetSmallFont() { SetFont("Courier", "Medium", "r", "Normal",
					 8.0); }

  /* Draw rubberbanding rectangle */
  virtual void DrawRubberband(int x1, int y1, int x2, int y2) = 0;

  /* Get window rep dimensions */
  virtual void Dimensions(unsigned int &width, unsigned int &height ) = 0;
  virtual void PrintDimensions() {} // for debug
  virtual void ReadCursorStore(CursorStore & c) {}
  virtual void DrawCursorStore(CursorStore & c) {}

  /* Set window rep dimensions */
  virtual void SetDimensions(unsigned int width, unsigned int height) {}
  
  /* get window rep origin from parent */
  virtual void Origin(int &x, int &y) = 0;
  
  /* Get absolute window rep origin from upper left corner
     of the screen */
  virtual void AbsoluteOrigin(int &x,int &y) = 0;

  /* Set window rep absolute origin */
  virtual void SetAbsoluteOrigin(int x, int y) {}
  
  /* Get flag which indicates if window destroy is pending */
  static Boolean IsDestroyPending() { return _destroyPending; }

  // ---------------------------------------------------------- 

  /* Copy the "state" (2D transforms, 3D transforms, clip mask)
     of the given WindowRep. */
  void CopyState(WindowRep *winRepP);

  // ---------------------------------------------------------- 

  /* 2D transformation matrix operations */
  
  /* Push a copy of the top of stack onto the stack */
  virtual void PushTop() {
    if (_current >= WindowRepTransformDepth-1 ){
      reportErrNosys("WindowRep::PushTop: overflow");
      Exit::DoExit(1);
    };
    DEBUGE(_transforms[_current+1].Copy(_transforms[_current]));
    DEBUGE(_current++);
  }
  
  /* pop transformation matrix */
  virtual void PopTransform() {
    if (_current <= 0){
      reportErrNosys("WindowRep::PopTransform: underflow");
      Exit::DoExit(1);
    }
    DEBUGE(_current--);
  }

  virtual void SetViewCamera(const Camera & c) {}
  // set a transform to in order to draw the cursor
  virtual void SetCamCursorTransform(const Camera & c) {}

  /* operations on current transformation matrix */
  virtual void Scale(Coord sx, Coord sy) {
    DEBUGE(_transforms[_current].Scale(sx,sy));
  }

  virtual void Translate(Coord dx, Coord dy) {
    DEBUGE(_transforms[_current].Translate(dx,dy));
  }

  virtual void MakeIdentity() {
    DEBUGE(_transforms[_current].MakeIdentity());
  }

  /* return the transform on top of the stack. */
  virtual Transform2D *TopTransform() {
    DPRINTF("WindowRep::TopTransform\n");
    return &_transforms[_current];
  }

/*  virtual void PostMultiply(Transform2D *t) {
    DEBUGE(_transforms[_current].PostMultiply(t));
  }*/

  virtual void Transform(Coord x, Coord y, Coord &newX, Coord &newY) {
    _transforms[_current].Transform(x, y, newX, newY);
  }

  virtual void Transform(int n, Coord *x, Coord *y, Coord *newX, Coord *newY) {
    for (int i=0; i<n; i++) {
      _transforms[_current].Transform(x[i], y[i], newX[i], newY[i]);
    }
  }

  virtual void Transform3(Coord x, Coord y, Coord z,
                  Coord &newX, Coord &newY) {
    Coord newZ;
    _transforms3[_current3].Transform(x, y, z, newX, newY, newZ);
  }

  virtual void Transform3(int n, Coord *x, Coord *y, Coord *z,
                  Coord *newX, Coord *newY) {
    Coord newZ;
    for (int i=0; i<n; i++) {
      _transforms3[_current3].Transform
	(x[i], y[i], z[i], newX[i], newY[i], newZ);
    }
  }

  virtual void InverseTransform(Coord x, Coord y, Coord &newX, Coord &newY) {
    _transforms[_current].InverseTransform(x, y, newX, newY);
  }

  virtual void InverseTransform(int n, Coord *x, Coord *y,
                                Coord *newX, Coord *newY) {
    for (int i=0; i<n; i++) {
      _transforms[_current].InverseTransform(*x, *y, *newX, *newY);
    }
  }

  virtual void PrintTransform() {
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
  virtual void PushTop3() {
    if (_current3 >= WindowRepTransformDepth-1 ){
      reportErrNosys("WindowRep::PushTop: overflow");
      Exit::DoExit(1);
    };
    DEBUGE(_transforms3[_current3+1].Copy(_transforms3[_current3]));
    _current3++;
  }
  
  /* pop transformation matrix */
  virtual void PopTransform3() {
    if (_current3 <= 0){
      reportErrNosys("WindowRep::PopTransform: underflow");
      Exit::DoExit(1);
    }
    _current3--;
  }

  /* operations on current transformation matrix */
  virtual void Scale3(Coord sx, Coord sy, Coord sz) {
    DEBUGE(_transforms3[_current3].Scale(sx,sy,sz));
  }

  virtual void Translate3(Coord dx, Coord dy, Coord dz) {
    DEBUGE(_transforms3[_current3].Translate(dx,dy,dz));
  }

  virtual void Rotate3(Coord angle, Coord x, Coord y, Coord z) {};
  // Rotate the object by angle around the vector ((0,0,0),(x,y,z))

  void MakeIdentity3() {
    DEBUGE(_transforms3[_current3].MakeIdentity());
  }

  /* return the transform on top of the stack. */
  Transform3D *TopTransform3() {
    return &_transforms3[_current3];
  }

  void PostMultiply3(Transform3D *t) {
    DEBUGE(_transforms3[_current3].PostMultiply(t));
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
     current transformation matrix. */
  virtual void PushClip(Coord x, Coord y, Coord w, Coord h) = 0;
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

  /* called by derived class to cache current clip region */
  void _PushClip(Coord x, Coord y, Coord w, Coord h) {
    if (_clipCurrent >= WindowRepClipDepth - 1){
      reportErrNosys("WindowRep::PushClip: overflow");
      Exit::DoExit(1);
    };
#if 0
    cout << "Clip current " << _clipCurrent << " to "
	 << _clipCurrent+1 << endl;
#endif
    ClipRect *rect = &_clippings[++_clipCurrent];
    rect->x = x; rect->y = y; rect->width = w; rect->height = h;
  }
  
  /* called by derived class to pop clip region */
  void _PopClip(){
    if (_clipCurrent < 0){
      reportErrNosys("WindowRep::PopClip: underflow");
      Exit::DoExit(1);
    }
#if 0
    cout << "Clip current " << _clipCurrent << " to "
	 << _clipCurrent-1 << endl;
#endif
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

  WindowRepCallbackList  *_callbackList;

  Transform2D _transforms[WindowRepTransformDepth];

  int _current;         /* current index in the stack */
  ClipRect _clippings[WindowRepClipDepth];
  int _clipCurrent;	/* index of top of stack for _clippings*/

  Transform3D _transforms3[WindowRepTransformDepth];
  int _current3;        /* current index in the stack */

  ClipRectList  _damageRects;     /* damaged areas */
  Boolean _damaged;

  Pattern _pattern;               /* current pattern */
  int _line_width;		  /* current border line width */

  static Boolean _destroyPending; /* true if window destroy is pending */

  int _numDim;
};

//******************************************************************************
// Getters and Setters (Inline)
//******************************************************************************

inline PColorID		WindowRep::GetForeground(void) const
{
	return coloring.GetForeground();
}

inline PColorID		WindowRep::GetBackground(void) const
{
	return coloring.GetBackground();
}

//******************************************************************************
#endif
