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
  Revision 1.34  1999/06/04 16:32:01  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.33  1998/08/24 14:57:31  wenger
  Added misc. debug output.

  Revision 1.32  1998/05/28 15:04:58  wenger
  OpenGL cursors now drawn in view foreground color;
  fixes to OpenGL crashes with some sessions (bugs 342, 356?).

  Revision 1.31  1998/05/21 18:18:33  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.30  1998/05/05 15:14:54  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.29  1998/02/26 00:19:06  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.28  1997/12/18 18:40:17  wenger
  Turned off debug output, cleaned things up a little bit in OpenGL code.

  Revision 1.27  1997/12/16 17:53:54  zhenhai
  Added OpenGL features to graphics.


  Revision 1.27  1997/10/18 23:28:58  zhenhai
  Rewrote matrix manipulation functions and deleted direct matrix manipulation
  code. Because the code is assuming that WindowRep uses the Transformation
  class, which is not true for GLWindowRep.

  Revision 1.26  1997/11/24 23:14:38  weaver
  Changes for the new ColorManager.

  Revision 1.25  1997/05/21 22:10:00  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.24.10.1  1997/05/21 20:40:07  weaver
  Changes for new ColorManager

  Revision 1.24  1997/01/17 20:31:48  wenger
  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
  simulation of XOR drawing in PSWindowRep; removed diagnostic output
  from Tcl/Tk code; removed (at least for now) the ETk interface from
  the cslib versions of WindowRep classes so that the cslib will link
  okay; cslib server now tests XOR drawing.

  Revision 1.23  1996/12/30 23:51:12  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.22  1996/11/26 15:44:11  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.21  1996/11/13 16:56:16  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.20  1996/10/28 15:55:47  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.19  1996/10/18 20:34:11  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.18  1996/09/06 06:59:44  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.17  1996/08/28 00:19:38  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.16  1996/08/04 21:05:34  beyer
  changed key handling

  Revision 1.15  1996/07/14 16:17:03  jussi
  Added destroyPending flag.

  Revision 1.14  1996/07/14 04:02:57  jussi
  HandleWindowDestroy() now destroys the window, conditionally.
  Moved the destructor from the header file to the .c file.

  Revision 1.13  1996/07/14 02:19:39  jussi
  Added HandleWindowDestroy() method. Removed some unnecessary,
  old code.

  Revision 1.12  1996/07/10 18:59:20  jussi
  Moved 3D transform variables to WindowRep.

  Revision 1.11  1996/06/21 19:31:25  jussi
  Moved all 3D-related code to Map3D.C and Map3D.h.

  Revision 1.10  1996/06/15 14:14:04  jussi
  Added yuc's 3D functions.

  Revision 1.9  1996/05/31 15:32:10  jussi
  Added 'state' variable to HandleButton(). This tells the callee
  whether the shift/control keys were pressed in conjunction with
  the mouse button.

  Revision 1.8  1996/05/20 18:45:00  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/02/28 17:48:12  yuc
  Update the WindowRep's constructor with the new protected variables
  for 3D.  Included the member functions: CompRhoPhiTheta and
  CompViewingTransf.

  Revision 1.6  1995/12/28 18:55:35  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 17:22:13  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.4  1995/12/02 21:31:14  jussi
  Cleaned up the code further.

  Revision 1.3  1995/11/24 21:31:05  jussi
  Added copyright notice and cleaned up the code. Added debugging
  statements.

  Revision 1.2  1995/09/05 21:13:34  jussi
  Added/updated CVS header.
*/

#include "WindowRep.h"
#include "Display.h"

//#define DEBUG

Boolean WindowRep::_destroyPending = false;

//******************************************************************************
// Constructor and Destructors
//******************************************************************************

WindowRep::WindowRep(DeviseDisplay* disp, Pattern p)
	:	_display(disp), _numDim(2)
{
  _callbackList = new WindowRepCallbackList;
  DOASSERT(_callbackList, "Out of memory");

  _current = 0;
  _current3 = 0;
  _clipCurrent = -1;
  _pattern = p;
  _gifDirty = false;
}

WindowRep::~WindowRep(void)
{
  delete _callbackList;
  if (DaliImageCount() > 0)
      DaliFreeImages();
//#ifndef LIBCS
//  if (ETk_WindowCount() > 0)
//      ETk_FreeWindows();
//#endif
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

void	WindowRep::SetForeground(PColorID fgid)
{
	coloring.SetForeground(fgid);
}

void	WindowRep::SetBackground(PColorID bgid)
{
	coloring.SetBackground(bgid);
}

//******************************************************************************

/* called by derived class to when window is resized or moved */

void WindowRep::HandleResize(int x, int y, unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("WindowRep::HandleResize(%d,%d,%d,%d)\n",x,y,width,height);
#endif

  int index;
  for(index = InitIterator(); More(index);) {
    WindowRepCallback *callBack = Next(index);
    callBack->HandleResize(this, x, y, width, height);
  }
  DoneIterator(index);
}

#ifdef RAWMOUSEEVENTS
/* called by derived class with button event */

void WindowRep::HandleButton(int x, int y, int button, int state, int type)
{
#if defined(DEBUG)
  printf("WindowRep::HandleButton(%d,%d,%d,%d,%d)\n",
	 x, y, button, state, type);
#endif

  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleButton(this, x, y, button, state, type);
  }
  DoneIterator(index);
}
#else
/* called by derived class when button pressed */

void WindowRep::HandleButtonPress(int xlow, int ylow, 
				  int xhigh, int yhigh, int button)
{
#if defined(DEBUG)
  printf("WindowRep::HandleButtonPress(%d,%d,%d,%d,%d)\n",
         xlow, ylow, xhigh, yhigh, button);
#endif

  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandlePress(this, xlow, ylow, xhigh, yhigh, button);
  }
  DoneIterator(index);
}
#endif

/* called by derived class when key pressed */

void WindowRep::HandleKey(int key, int x, int y)
{
#if defined(DEBUG)
  printf("WindowRep::HandleKey()\n");
#endif

  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleKey(this, key, x, y);
  }
  DoneIterator(index);
}

/* called by derived class when exposed */

void WindowRep::HandleExpose(int x, int y, unsigned w, unsigned h)
{
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleExpose(this, x,y,w, h);
  }
  DoneIterator(index);
}

#ifndef RAWMOUSEEVENTS
/* Called by derived class on pop-up event. Report to first callback */

Boolean WindowRep::HandlePopUp(int x, int y, int button, char **&msgs,
			       int &numMsgs)
{
#if defined(DEBUG)
  printf("WindowRep::HandlePopUp(%d, %d)\n", x, y);
#endif

  int index = InitIterator(); 
  if (More(index)) {
    /* do only first callback */
    WindowRepCallback *c = Next(index);
    DoneIterator(index);
    return c->HandlePopUp(this, x, y, button, msgs, numMsgs);
  }
  DoneIterator(index);

  return false;
}
#endif

/* called by derived class on window mapped info change event */

void WindowRep::HandleWindowMappedInfo(Boolean mapped)
{
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleWindowMappedInfo(this, mapped);
  }
  DoneIterator(index);
}

/* called by derived class on window destroy event */

void WindowRep::HandleWindowDestroy()
{
  /* everyone must know that a window destroy is already pending
     so while we're inside the iterator loop below, they must not
     try to destroy the window again */
  _destroyPending = true;

  /* first tell everyone that the window is going away */
  Boolean canDestroy = true;
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    canDestroy &= c->HandleWindowDestroy(this);
    
  }
  DoneIterator(index);

  _destroyPending = false;

  /* now destroy the window if everyone
     returned a positive acknowledgement */
  if (canDestroy) {
    DeviseDisplay::DefaultDisplay()->DestroyWindowRep(this);
  } else {
    printf("Window not destroyed\n");
  }
}

/* Copy the "state" (2D transforms, 3D transforms, clip mask)
   of the given WindowRep. */
void
WindowRep::CopyState(WindowRep *winRepP)
{
  int count;

  /* The clip masks much be copied with an identity transform in place
   * because they've already been transformed once by the WindowRep
   * we're copying from. */

  /* Copy clip masks. */
  ClearTransformStack();
  ClipRect clip;
  _clipCurrent = -1; // Get rid of all current clip masks.
  for (count = 0; count <= winRepP->_clipCurrent; count++)
  {
    clip = winRepP->_clippings[count];
    PushClip(clip.x, clip.y, clip.width, clip.height);
  }

  /* Copy 2D transforms. */
  ClearTransformStack();
  for (count = 0; count <= winRepP->_current; count++)
  {
    DEBUGE(_transforms[count].Copy(winRepP->_transforms[count]));
  }
  _current = winRepP->_current;

  /* Copy 3D transforms. */
  ClearTransformStack3();
  for (count = 0; count <= winRepP->_current3; count++)
  {
    DEBUGE(_transforms3[count].Copy(winRepP->_transforms3[count]));
  }
  _current3 = winRepP->_current3;

  /* Copy the geometry. */
  int xVal, yVal;
  winRepP->AbsoluteOrigin(xVal, yVal);
  SetAbsoluteOrigin(xVal, yVal);

  unsigned int width, height;
  winRepP->Dimensions(width, height);
  SetDimensions(width, height);

  return;
}

void
WindowRep::SetGifDirty(Boolean dirty)
{
#if defined(DEBUG)
  printf("WindowRep(0x%p)::SetGifDirty(%d)\n", this, dirty);
#endif

  _gifDirty = dirty;
}

//******************************************************************************

