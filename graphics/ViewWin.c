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
  Revision 1.42  1998/02/16 15:41:26  wenger
  Fixed (I believe) bug 287.

  Revision 1.41  1998/02/04 20:22:04  zhenhai
  Fixed bugs of displaying date and customized text at axis.

  Revision 1.40  1997/12/16 17:53:53  zhenhai
  Added OpenGL features to graphics.

  Revision 1.39  1997/11/24 23:14:36  weaver
  Changes for the new ColorManager.

  Revision 1.38  1997/06/25 17:05:28  wenger
  Fixed bug 192 (fixed problem in the PSWindowRep::FillPixelRect() member
  function, disabled updating of record links during print, print dialog
  grabs input.

  Revision 1.37  1997/06/04 15:50:30  wenger
  Printing windows to PostScript as pixmaps is now implemented, including
  doing so when printing the entire display.

  Revision 1.36  1997/05/30 20:42:27  wenger
  Added GUI to allow user to specify windows to exclude from display
  print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
  implemented but pixmap printing is not.

  Revision 1.35  1997/05/28 15:38:59  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.34.2.1  1997/05/21 20:40:04  weaver
  Changes for new ColorManager

  Revision 1.34  1997/05/08 00:18:06  wenger
  Kludge fix for bug 182 (crash when closing multi1.tk session).

  Revision 1.33.4.1  1997/05/20 16:10:52  ssl
  Added layout manager to DEVise

  Revision 1.33  1997/03/25 17:58:58  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.32  1997/02/03 19:40:02  ssl
  1) Added a new Layout interface which handles user defined layouts
  2) Added functions to set geometry and remap views as changes in the
     layout editor
  3) Added a function to notify the front end of some change so that it
     can execute a Tcl command
  4) The old TileLayout.[Ch] files still exist but are commented out
     conditionally using #ifdef NEW_LAYOUT

  Revision 1.31.4.2  1997/03/15 00:31:08  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.31.4.1  1997/02/27 22:46:06  wenger
  Most of the way to having Tasvir images work in PostScript output;
  various WindowRep-related fixes; version now 1.3.4.

  Revision 1.31  1997/01/08 19:01:44  wenger
  Fixed bug 064 and various other problems with drawing piled views;
  added related debug code.

  Revision 1.30  1996/12/30 23:51:12  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.29  1996/12/15 20:22:34  wenger
  Changed pointSize in SetFont() from tenths of points to points.

  Revision 1.28  1996/12/03 23:29:10  wenger
  Fixed PostScript bounding box to closely surround the image (fixed
  bug 089).

  Revision 1.27  1996/12/03 17:02:11  jussi
  Replaced SetSmallFont() with SetFont().

  Revision 1.26  1996/11/26 16:47:47  ssl
  Added support for Stacked Opaque and Transparent views

  Revision 1.25  1996/11/26 15:44:11  wenger
  Added features and fixed bugs in PostScript-related parts of the
  client/server library and the PSWindowRep class; page size can now be
  set in PSDisplay; did some cleanup of the DeviseDisplay and WindowRep
  methods (allowed elimination of many typecasts).

  Revision 1.24  1996/11/18 23:11:20  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.23  1996/11/13 16:56:15  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.22  1996/11/07 22:40:17  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.21  1996/10/28 15:55:47  wenger
  Scaling and clip masks now work for printing multiple views in a window
  to PostScript; (direct PostScript printing still disabled pending correct
  text positioning and colors); updated all dependencies except Linux.

  Revision 1.20  1996/10/18 20:34:10  wenger
  Transforms and clip masks now work for PostScript output; changed
  WindowRep::Text() member functions to ScaledText() to make things
  more clear; added WindowRep::SetDaliServer() member functions to make
  Dali stuff more compatible with client/server library.

  Revision 1.19  1996/09/19 20:11:54  wenger
  More PostScript output code (still disabled); some code for drawing
  view borders (disabled).

  Revision 1.18  1996/09/10 20:07:12  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.17  1996/09/06 06:59:44  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.16  1996/07/14 16:52:16  jussi
  Added handling of window destroy events from window manager.

  Revision 1.15  1996/07/13 17:25:58  jussi
  Moved Mapped() method to header file.

  Revision 1.14  1996/06/15 13:47:41  jussi
  Added SetFgBgColor() which allows Devise to change the view
  foreground and background colors at runtime.

  Revision 1.13  1996/04/11 18:09:19  jussi
  Added window flush after window has been raised or lowered.

  Revision 1.12  1996/04/11 17:56:11  jussi
  Added Raise() and Lower(). Fixed a bug when MapChildren() updates
  the sizes of the children but the View doesn't get notified of
  the size change because the windowing system hasn't yet reported
  it to the View.

  Revision 1.11  1996/04/09 20:34:32  jussi
  Minor fixes.

  Revision 1.10  1996/02/27 02:12:52  jussi
  TopMargin depends on height of small font.

  Revision 1.9  1996/02/06 19:32:10  jussi
  Added top margin with Devise logo to root windows.

  Revision 1.8  1996/01/30 00:02:48  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.7  1995/12/28 18:52:30  jussi
  Small fix to remove compiler warning.

  Revision 1.6  1995/12/14 21:11:27  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.5  1995/12/02 21:25:20  jussi
  Added support for TK_WINDOW i.e. Tk controls surrounding views
  and windows. Fixed some bugs.

  Revision 1.4  1995/11/29 15:12:51  jussi
  Commented out #define DEBUG.

  Revision 1.3  1995/11/29 15:08:06  jussi
  Added preliminary stuff needed for Tk window support.

  Revision 1.2  1995/09/05 21:13:18  jussi
  Added/updated CVS header.
*/

//******************************************************************************

#include <stdio.h>
#include <string.h>
#include <errno.h>

//#define DEBUG

#include "ViewWin.h"
#include "View.h"
#include "Display.h"
#include "WindowRep.h"
#include "ClassDir.h"
#include "Control.h"
#include "Init.h"
#include "DevError.h"
#include "Version.h"
#include "Util.h"
#include "PSDisplay.h"
#include "ETkIfc.h"
#include "RecordLink.h"

#ifdef TK_WINDOW
#include <tcl.h>
#include <tk.h>
#endif

//******************************************************************************

#define DIRECT_POSTSCRIPT 1
#define LAYOUT 1

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewWin::ViewWin(char* name, PColorID fgid, PColorID bgid,
				 int weight, Boolean boundary)
	: Coloring(fgid, bgid)
{
	DO_DEBUG(printf("ViewWin::ViewWin(%s, this = %p)\n", name, this));

	windowRepCallback = new ViewWin_WindowRepCallback(this);

	_name = name;
	_parent = NULL;
	_mapped = false;
	_weight = weight;
	_winBoundary = boundary;
	_iconified = true;

	_hasPrintIndex = false;

#ifdef MARGINS
	_leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
#endif
//	_alternate = NULL;
#ifdef TK_WINDOW
	_marginsOn = false;
	_leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
	_tkPathName[0] = 0;
#endif

	_excludeFromPrint = false;
	_printAsPixmap = false;
}

ViewWin::~ViewWin(void)
{
#ifdef DEBUG
	printf("ViewWin::~ViewWin(%p)\n", this);
#endif

	DetachChildren();
	DeleteFromParent();
	Unmap();

	delete windowRepCallback;
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

void	ViewWin::SetBackground(PColorID bgid)
{
	if (GetWindowRep())
		GetWindowRep()->SetBackground(bgid);

	Coloring::SetBackground(bgid);
}

//******************************************************************************

void ViewWin::Iconify()
{
  if (GetWindowRep())
    GetWindowRep()->Iconify();
}

DevStatus
ViewWin::ExportImage(DisplayExportFormat format, char *filename)
{
  DO_DEBUG(printf("ViewWin::ExportImage(_parent = %p, filename = %s)\n",
    _parent, filename));
  DevStatus result = StatusOk;

#if DIRECT_POSTSCRIPT
  if ((format == POSTSCRIPT || format == EPS) && !_printAsPixmap)
  {
    ViewWin *printWinP = this;

    // Find top level ViewWin (this should be called on the top level in the
    // first place, but make sure).
    while (printWinP->_parent != NULL)
    {
      printf("Warning: ViewWin::ExportImage() not called on top level object.\n");
      printWinP = printWinP->_parent;
    }

    /* Disable record links so printing doesn't force unnecessary redraws. */
    RecordLink::DisableUpdates();

    DeviseDisplay *psDispP = DeviseDisplay::GetPSDisplay();
    if (!psDispP->OpenPrintFile(filename).IsComplete())
    {
      reportError("Can't open print file", errno);
      result += StatusFailed;
    }
    else
    {
      int x, y;
      unsigned width, height;
      RealGeometry(x, y, width, height);
      AbsoluteOrigin(x, y);

      Rectangle printGeom;
      printGeom.x = x;
      printGeom.y = y;
      printGeom.width = width;
      printGeom.height = height;

      psDispP->PrintPSHeader("DEVise Visualization", printGeom, true);
      result += printWinP->PrintPS();

      /* Allow the generation of the print file to complete before this
       * command returns, so that the Tcl code can lpr it if necessary. */
      while (printWinP->IsPrinting()) {
        Dispatcher::SingleStepCurrent();
      }

      psDispP->PrintPSTrailer();
      result += psDispP->ClosePrintFile();
      printf("Done generating print file\n");

    }
    /* Re-enable record links. */
    RecordLink::EnableUpdates();
  }
  else
#endif
  {
    GetWindowRep()->ExportImage(format, filename);
  }

  return result;
}

void ViewWin::AppendToParent(ViewWin *parent)
{
  if ( _parent) {
    _parent->Delete(this);
  }
  if (_mapped)
    Unmap();
  _parent = parent;
  _parent->Append(this);

//	SetColorParent(_parent);			// Establish coloring inheritance
	// Should probably force a redraw here (happens elsewhere?)
}

void ViewWin::DeleteFromParent()
{
  if (_parent) {
    _parent->Delete(this);
    if (_mapped)
      Unmap();
    _parent = NULL;
  }

//	SetColorParent(NULL);				// Eliminate coloring inheritance
}

void ViewWin::Map(int x, int y, unsigned w, unsigned h)
{
#if defined(DEBUG)
  printf("ViewWin(0x%p)::Map(%d, %d, %d, %d)\n", this, x, y, w, h);
#endif
  if (_mapped) {
    fprintf(stderr,"ViewWin::Map() already mapped\n");
    Exit::DoExit(1);
  }

  _hasGeometry = false;

  Boolean relativeMinSize = false;
  Coord min_width = 170;
  Coord min_height = 100;

  WindowRep *parentWinRep;
	PColorID	fgid, bgid;

  if (_parent) {
#ifdef DEBUG
    printf("ViewWin 0x%p mapping to parent 0x%p\n", this,
	   _parent->GetWindowRep());
#endif
    parentWinRep = _parent->GetWindowRep();

		fgid = GetForeground();
		bgid = GetBackground();
  } else {
#ifdef DEBUG
    printf("ViewWin 0x%p mapping to root\n", this);
#endif
    parentWinRep = NULL;

	    fgid = GetPColorID(defForeColor);
		bgid = GetPColorID(defBackColor);
  }

  WindowRep* screenWinRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
	_name, x, y, w, h, parentWinRep, min_width,
	min_height, relativeMinSize, _winBoundary);

  screenWinRep->SetForeground(fgid);
  screenWinRep->SetBackground(bgid);
  
  //TEMPTEMP -- is this callback ever deleted?  RKW 5/7/97.
  screenWinRep->RegisterCallback(windowRepCallback);
  screenWinRep->SetDaliServer(Init::DaliServer());
  screenWinRep->SetETkServer((char *)ETkIfc::GetServer());
  _winReps.SetScreenWinRep(screenWinRep);

  WindowRep *fileWinRep = DeviseDisplay::GetPSDisplay()->CreateWindowRep(_name,
	x, y, w, h, NULL, min_width,
	min_height, relativeMinSize, _winBoundary);
  
#if 0
  // These functions CANNOT be called here because there is no print file
  // open.  I think we are okay just not calling them, because the colors
  // should be set before we drawn anything.  RKW Feb. 13, 1998.
  fileWinRep->SetForeground(fgid);
  fileWinRep->SetBackground(bgid);
#endif

  fileWinRep->SetDaliServer(Init::DaliServer());
  _winReps.SetFileWinRep(fileWinRep);

  _winReps.SetScreenOutput();

#ifdef MARGINS
  if (!_parent) {
    if (Init::DisplayLogo()) {
      /* Allocate top margin */
      _topMargin = 9 + 2;
      /* Draw margins */
      DrawMargins();
    }
  }
#endif

#ifdef TK_WINDOW_old
    GetWindowRep()->Decorate(parentWinRep, _name, (unsigned int)min_width,
			 (unsigned int)min_height);
#endif

#ifdef TK_WINDOW
  /* Get and set appropriate margins */
  AddMarginControls();
#endif

  _mapped = true;
  _iconified = false;
  

  SubClassMapped();
}

void ViewWin::Unmap()
{
#ifdef DEBUG
  printf("ViewWin 0x%p unmapping\n", this);
#endif
  
  
  if (!_mapped)
    return;

// Unmap children
  int index = _children.InitIterator();
  while (_children.More(index)) {
    ViewWin *vw = _children.Next(index);
    vw->Unmap();
  } 
  _children.DoneIterator(index);

#ifdef TK_WINDOW_old
  GetWindowRep()->Undecorate();
#endif

#ifdef TK_WINDOWxxx
  /* Drop margin controls */
  if (_marginsOn)
    DropMarginControls();
#endif

  SubClassUnmapped();
  //TEMPTEMP?_winReps.GetScreenWinRep()->DeleteCallback(windowRepCallback);

  if (!WindowRep::IsDestroyPending())
  {
    DeviseDisplay::DefaultDisplay()->DestroyWindowRep(
      _winReps.GetScreenWinRep());
    DeviseDisplay::GetPSDisplay()->DestroyWindowRep(_winReps.GetFileWinRep());
  }
  _winReps.SetScreenWinRep(NULL);
  _winReps.SetFileWinRep(NULL);

  _hasGeometry = false;
  _mapped = false;
}

/* Detach all children from this view */

void ViewWin::DetachChildren()
{
  /* Ask each child to delete itself from the parent i.e. this object.
     We need to be careful to close the iterator because
     DeleteFromParent() deletes an entry from the list. */

  while (1) {
    int index = _children.InitIterator();
    if (_children.More(index)) {
      ViewWin *vw = _children.Next(index);
      _children.DoneIterator(index);
      vw->DeleteFromParent();
    } else {
      _children.DoneIterator(index);
      break;
    }
  }
}

/* Append child */

void ViewWin::Append(ViewWin *child)
{
  _children.Append(child);
}

/* Delete child */

void ViewWin::Delete(ViewWin *child)
{
  if (!_children.Delete(child)) {
    fprintf(stderr,"ViewWin::Delete child not found\n");
    Exit::DoExit(2);
  }
}
/* Set geometry of view */
void ViewWin::SetGeometry(int x, int y, unsigned w, unsigned h) 
{
  _x = x;
  _y = y;
  _width = w;
  _height = h;
}

/* Get current geometry of child w. r. t. parent */

#if defined(MARGINS) || defined(TK_WINDOW)
void ViewWin::RealGeometry(int &x, int &y, unsigned &w, unsigned &h)
#else
void ViewWin::Geometry(int &x, int &y, unsigned &w, unsigned &h)
#endif
{
  if (!GetWindowRep()) {
    fprintf(stderr,"ViewWin::Geometry: not mapped\n");
    Exit::DoExit(2);
  }

  if (!_hasGeometry) {
#ifdef DEBUG
    printf("ViewWin::Geometry from WindowRep 0x%p\n", GetWindowRep());
#endif
    GetWindowRep()->Dimensions(_width, _height);
    _x = _y = 0;
    _hasGeometry = true;
  } else {
#ifdef DEBUG
    printf("ViewWin::Geometry from cached values\n");
#endif
  }

  x = _x;
  y = _y;
  w = _width;
  h = _height;

#ifdef DEBUG
  printf("ViewWin::Geometry returns %d,%d,%u,%u\n", x, y, w, h);
#endif
}

#if defined(MARGINS) || defined(TK_WINDOW)
/* Get current geometry of child w. r. t. parent */

void ViewWin::Geometry(int &x, int &y, unsigned &w, unsigned &h)
{
  RealGeometry(x, y, w, h);

  unsigned int lm, rm, tm, bm;
  GetMargins(lm, rm, tm, bm);
  x += lm;
  y += tm;
  w -= lm + rm;
  h -= tm + bm;

#ifdef DEBUG
  printf("ViewWin::Geometry margin-adjusted returns %d,%d,%u,%u\n",
	 x, y, w, h);
#endif
}
#endif

void ViewWin::AbsoluteOrigin(int &x, int &y)
{
  if (!GetWindowRep()) {
    fprintf(stderr,"ViewWin::AbsoluteOrigin: not mapped\n");
    Exit::DoExit(2);
  }
  GetWindowRep()->AbsoluteOrigin(x, y);
}

/* Move/Resize ourselves */

void ViewWin::MoveResize(int x, int y, unsigned w, unsigned h)
{
#if defined(DEBUG)
  printf("ViewWin::MoveResize 0x%p, %d, %d, %u, %u\n", this, x, y, w, h);
#endif

  if (!_mapped) {
    fprintf(stderr,"ViewWin::MoveResize not mapped\n");
    Exit::DoExit(1);
  } else {
    _hasGeometry = false;
    GetWindowRep()->MoveResize(x, y, w, h);
    HandleResize(GetWindowRep(), x, y, w, h);
  }
}

int ViewWin::TotalWeight()
{
  int w = 0;
  int index;
  for(index = InitIterator(); More(index);) {
    ViewWin *vw= Next(index);
    w += vw->GetWeight();
  }
  DoneIterator(index);
  return w;
}

Boolean ViewWin::Iconified()
{
  if (_parent) {
    /* subwindow must query parent */
    return _parent->Iconified();
  }

  /* no parent: this is a top level window. */
  if (Mapped() && !_iconified)
    return false;

  return true;
}

/* Replace child1 by child2. child1 must be a valid child.
   Update the list of children.*/

void ViewWin::Replace(ViewWin *child1, ViewWin *child2)
{
#ifdef DEBUG
  printf("ViewWin::Replace 0x%p with 0x%p\n", child1, child2);
#endif

  for(int index = InitIterator(); More(index);) {
    ViewWin *win = Next(index);
    if (win == child1) {
      _children.InsertBeforeCurrent(index, child2);
      child2->SetParent(this);
      _children.DeleteCurrent(index);
      DoneIterator(index);
      child1->Unmap();
      child1->SetParent(NULL);
      return;
    }
  }

  fprintf(stderr, "ViewWin::Replace: can't find child1\n");
  Exit::DoExit(2);
}

/* Swap child1 and child2. Both must be valid children, and child1 != child2.*/

void ViewWin::SwapChildren(ViewWin *child1, ViewWin *child2)
{
#if defined(DEBUG)
  printf("ViewWin::SwapChildren()\n");
#endif

  DOASSERT(child1 != child2, "Swapping the same child");
  DOASSERT(child1->GetParent() == child2->GetParent(),
    "Children have different parents");

  _children.Swap(child1, child2);

  /* If one of these children becomes the top view in a pile, we need
   * to refresh it. */
  /* Note: I kind of hate having this View-class-specific code in the
   * ViewWin class, but I don't see an easy way of avoiding it.  RKW
   * 1/7/97. */
  ViewWin *parent = child1->GetParent();
  DOASSERT(parent, "View has no parent");

  int index = parent->InitIterator();
  DOASSERT(parent->More(index), "Parent view has no children");

  if (parent->More(index)) {
    ViewWin *topVw = parent->Next(index);
    if (topVw == child1 || topVw == child2) {

      /* FindViewByName is a type-safe way to convert from a ViewWin to
       * a View, which is necessary to call Refresh(). */
      View *view = View::FindViewByName(topVw->GetName());
      DOASSERT(view, "Cannot find view");
      DOASSERT(topVw == view, "ViewWin != View");
      if (view->IsInPileMode()) {
#if defined(DEBUG)
        printf("ViewWin::SwapChildren() refreshes view %s\n", view->GetName());
#endif
        view->Refresh();
      }
    }
  }
  parent->DoneIterator(index);
}

/* Raise window to top of stacking order */

void ViewWin::Raise()
{
#if defined(DEBUG)
  printf("ViewWin(0x%p)::Raise()\n", this);
#endif

  if (GetWindowRep()) {
    GetWindowRep()->Raise();
    GetWindowRep()->Flush();
  }
}

/* Lower window to bottom of stacking order */

void ViewWin::Lower()
{
  if (GetWindowRep()) {
    GetWindowRep()->Lower();
    GetWindowRep()->Flush();
  }
}

#if defined(MARGINS) || defined(TK_WINDOW)
void ViewWin::GetMargins(unsigned int &lm, unsigned int &rm,
			 unsigned int &tm, unsigned int &bm)
{
  lm = _leftMargin;
  rm = _rightMargin;
  tm = _topMargin;
  bm = _bottomMargin;
#ifdef DEBUG
  printf("ViewWin::GetMargins %u, %u, %u, %u\n", lm, rm, tm, bm);
#endif
}
#endif

#ifdef MARGINS
void ViewWin::DrawMargins()
{
#if defined(DEBUG)
  printf("ViewWin::DrawMargins\n");
#endif

  if (!_topMargin)
    return;

  const char *logo = Version::GetWinLogo();

  int x, y;
  unsigned int w, h;
  RealGeometry(x, y, w, h);

  // draw logo

  WindowRep *win = GetWindowRep();
  win->SetForeground(GetForeground());
  win->SetPattern(Pattern0);
  win->SetLineWidth(0);
  win->FillRect(x, h-y-(_topMargin-1), w - 1, _topMargin - 1);
  win->SetForeground(GetBackground());
  win->FillRect(x + 1, h-(y + 1)-(_topMargin-1-2), w - 1 - 2,
		_topMargin - 1 - 2);
  win->SetForeground(GetForeground());
  win->SetFont("Courier", "Medium", "r", "Normal", 8.0);
  //win->AbsoluteText((char *) logo, x + 1, y + 1, w - 2, _topMargin - 2,
		      //WindowRep::AlignNorth, true);
  win->AbsoluteText((char *) logo, x + 1, h-(y + 1)-(_topMargin-2),
		    w - 2, _topMargin - 2,
		    WindowRep::AlignNorth, true);
  win->SetNormalFont();
}
#endif

#ifdef TK_WINDOW
void ViewWin::AddMarginControls()
{
  static int tkWinCnt = 1;
  char cmd[256];

  sprintf(_tkPathName, ".tkMarginControl%d", tkWinCnt++);
  sprintf(cmd, "CreateTkMarginControls %s %s %u %u",
	  _tkPathName, (_parent ? "View" : "Window"),
	  _width, _height);

#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif

  extern Tcl_Interp *ControlPanelTclInterp;

  if (Tcl_Eval(ControlPanelTclInterp, cmd) != TCL_OK) {
    fprintf(stderr, "Cannot execute %s: %s\n",
	    cmd, ControlPanelTclInterp->result);
    Exit::DoExit(2);
  }

  _leftMargin = atoi(ControlPanelTclInterp->result);
  char *space = strchr(ControlPanelTclInterp->result, ' ');
  _rightMargin = atoi(space + 1);
  space = strchr(space + 1, ' ');
  _topMargin = atoi(space + 1);
  space = strchr(space + 1, ' ');
  _bottomMargin = atoi(space + 1);

  if (_leftMargin > 0)
    ReparentMarginControl("left", 0, _topMargin);
  if (_rightMargin > 0)
    ReparentMarginControl("right", _width - _rightMargin, _topMargin);
  if (_topMargin > 0)
    ReparentMarginControl("top", 0, 0);
  if (_bottomMargin > 0)
    ReparentMarginControl("bottom", 0, _height - _bottomMargin);

  _marginsOn = true;
}

void ViewWin::DropMarginControls()
{
  if (_leftMargin > 0)
    DestroyMarginControl("left");
  if (_rightMargin > 0)
    DestroyMarginControl("right");
  if (_topMargin > 0)
    DestroyMarginControl("top");
  if (_bottomMargin > 0)
    DestroyMarginControl("bottom");

  _leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
  _marginsOn = false;
}

void ViewWin::DestroyMarginControl(char *side)
{
  char cmd[256];
  sprintf(cmd, "destroy %s-%s", _tkPathName, side);

#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif

  extern Tcl_Interp *ControlPanelTclInterp;

  if (Tcl_Eval(ControlPanelTclInterp, cmd) != TCL_OK) {
    fprintf(stderr, "Cannot execute %s: %s\n",
	    cmd, ControlPanelTclInterp->result);
    Exit::DoExit(2);
  }
}

void ViewWin::ReparentMarginControl(char *side, int xoff, int yoff)
{
  char tkWinName[256];
  sprintf(tkWinName, "%s-%s", _tkPathName, side);

  extern Tcl_Interp *ControlPanelTclInterp;
  extern Tk_Window ControlPanelMainWindow;

  Tk_Window tkWindow = Tk_NameToWindow(ControlPanelTclInterp,
				       tkWinName,
				       ControlPanelMainWindow);
  if (!tkWindow) {
    fprintf(stderr, "Cannot convert %s to window: %s\n",
	    tkWinName, ControlPanelTclInterp->result);
    Exit::DoExit(2);
  }

  // make Tk window a child of current window
  GetWindowRep()->Reparent(true, (void *)Tk_WindowId(tkWindow), xoff, yoff);
}

void ViewWin::ResizeMargins(unsigned int w, unsigned int h)
{
#if defined(DEBUG)
  printf("ViewWin::ResizeMargins()\n");
#endif

  char cmd[256];
  sprintf(cmd, "UpdateTkControlMargins %s %s %u %u",
	  _tkPathName, (_parent ? "View" : "Window"),
	  _width, _height);
  
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  
  extern Tcl_Interp *ControlPanelTclInterp;
  
  if (Tcl_Eval(ControlPanelTclInterp, cmd) != TCL_OK) {
    fprintf(stderr, "Cannot execute %s: %s\n",
	    cmd, ControlPanelTclInterp->result);
    Exit::DoExit(2);
  }
}

void ViewWin::ToggleMargins()
{
  if (_marginsOn)
    DropMarginControls();
  else
    AddMarginControls();
}
#endif

/*------------------------------------------------------------------------------
 * function: ViewWin::PrintPS
 * Prints all children of this object (and their children, etc.).
 *
 * This works as follows:
 *   It prints the first child of this object.
 *   When that child is done printing (including printing any of its
 *     children) it calls this function again, and the next child of
 *     this object (and its children) are printed.  And so on...
 */
DevStatus
ViewWin::PrintPS()
{
#if defined(DEBUG)
  printf("ViewWin(0x%p)::PrintPS(%s)\n", this, _name);
#endif
  DevStatus result = StatusOk;

  if (!_hasPrintIndex)
  {
#ifdef MARGINS
  if (Init::DisplayLogo()) {
    Rectangle geom;
    int xVal, yVal;
    unsigned int width, height;
    RealGeometry(xVal, yVal, width, height);
    AbsoluteOrigin(xVal, yVal);
    geom.x = xVal;
    geom.y = yVal;
    geom.width = width;
    geom.height = height;

    Rectangle parentGeom;
    DeviseDisplay::GetPSDisplay()->GetScreenPrintRegion(parentGeom);

    SetFileOutput(geom, parentGeom);

    DrawMargins();
    SetScreenOutput();
  }
#endif

#if defined(DEBUG)
    printf("Starting printing of window/view <%s>\n", _name);
#endif
    _printIndex = _children.InitIterator();
    _hasPrintIndex = true;
  }

  if (_children.More(_printIndex))
  {
    ViewWin *child = _children.Next(_printIndex);
    result += child->PrintPS();
  }
  else
  {
#if defined(DEBUG)
    printf("Ending printing of window/view <%s>\n", _name);
#endif
    _children.DoneIterator(_printIndex);
    _hasPrintIndex = false;

    if (_parent != NULL)
    {
      _parent->ViewWin::PrintPS();
    }
  }

  return result;
}

//******************************************************************************
// Callback Methods (WindowRepCallback)
//******************************************************************************
void ViewWin::HandleExpose(WindowRep *w, int x, int y,
			   unsigned width, unsigned height)
{
#ifdef MARGINS
  if (Init::DisplayLogo())
    DrawMargins();
#endif
}

void	ViewWin::HandleResize(WindowRep* w, int xlow, int ylow,
							  unsigned width, unsigned height)
{
#if defined(DEBUG)
	printf("ViewWin::HandleResize 0x%p, %d, %d, %u, %u\n",
		   this, xlow, ylow, width, height);
#endif

	_hasGeometry = true;
	_x = _y = 0;			// Why are these forced to zero?  RKW 10/24/96.
	_width = width;
	_height = height;

#ifdef MARGINS
	if (Init::DisplayLogo())
		DrawMargins();
#endif

#ifdef TK_WINDOW
	if (_marginsOn)			// Resize margin controls
		ResizeMargins(_width, _height);
#endif
}

void ViewWin::HandleWindowMappedInfo(WindowRep*, Boolean mapped)
{
#ifdef DEBUG
	printf("ViewWin::HandleWindowMappedInfo 0x%p %d\n", this, mapped);
#endif

	_iconified = !mapped;
	Iconify(_iconified);
}

Boolean ViewWin::HandleWindowDestroy(WindowRep* w)
{
#ifdef DEBUG
	printf("ViewWin %s being destroyed\n", _name);
#endif

	ClassDir*	classDir = ControlPanel::GetClassDir();

	classDir->DestroyInstance(_name);

	return true;
}

//******************************************************************************
