/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.61  1999/09/01 19:27:02  wenger
  Debug logging improved -- directory of log file can now be specified
  with the DEVISE_LOG_DIR environment variable (changed most startup scripts
  to put it in the DEVise tmp directory); added logging of a bunch of elapsed
  times to help figure out JavaScreen performance bottlenecks.

  Revision 1.60  1999/06/15 18:09:44  wenger
  Added dumping of ViewWin objects to help with pile debugging.

  Revision 1.59  1999/06/11 20:46:49  wenger
  Fixed bug that caused DEVise to crash when closing the
  SoilSci/TwoStations.ds demo session.

  Revision 1.58  1999/05/12 21:01:02  wenger
  Views containing view symbols can now be piled.

  Revision 1.57  1999/05/07 16:09:38  wenger
  Fixed bug in the ordering of viewsym piles.

  Revision 1.56  1999/05/07 14:13:45  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.55  1999/04/21 20:35:20  wenger
  Improved interface for changing fonts, titles, etc.:
  * Fonts can now be set on a window-wide basis.
  * Setting fonts, title, or axis date format in a piled view automatically
  changes all views in the pile accordingly.

  Revision 1.54  1999/03/01 17:47:34  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.53  1999/02/22 19:07:37  wenger
  Piling of views with view symbols is not allowed; fixed bug 461 (redrawing
  of piles); fixed bug 464 (toggling axes in a pile); fixed dynamic memory
  problems in PileStack and ViewClassInfo classes.

  Revision 1.52  1999/02/11 19:54:39  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.51  1999/01/04 15:33:19  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.50.2.2  1999/02/11 18:24:08  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.50.2.1  1998/12/29 17:24:48  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.50  1998/12/02 23:46:30  wenger
  Changes as per request from Miron: minimum window size is now 1x1; default
  is to not show trademark notice in windows.

  Revision 1.49  1998/09/30 17:44:34  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.48  1998/09/22 17:23:42  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.47  1998/09/10 23:24:29  wenger
  Fixed JavaScreen client switch GIF geometry problem.

  Revision 1.46  1998/05/21 18:18:32  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.45  1998/05/14 18:21:11  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.44  1998/05/05 15:14:52  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.43  1998/03/13 18:10:39  wenger
  Fixed bug 327 (gaps in view background colors).

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
#include "PileStack.h"
#include "ElapsedTime.h"

//******************************************************************************

#define DIRECT_POSTSCRIPT 1
#define LAYOUT 1
#define VIEWWIN_TIMER

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewWin::ViewWin(char* name, PColorID fgid, PColorID bgid,
				 int weight, Boolean boundary)
	: Coloring(fgid, bgid)
{
#if defined(DEBUG)
	printf("ViewWin::ViewWin(%s, this = %p)\n", name, this);
#endif

	windowRepCallback = new ViewWin_WindowRepCallback(this);

	_name = name;
	_parent = NULL;
	_mapped = false;
	_weight = weight;
	_winBoundary = boundary;
	_iconified = true;

	_hasPrintIndex = false;

	_leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
//	_alternate = NULL;

	_excludeFromPrint = false;
	_printAsPixmap = false;

	_myPileStack = NULL;
	_parentPileStack = NULL;

	_pileZValid = false;
}

ViewWin::~ViewWin(void)
{
#if defined(DEBUG)
	printf("ViewWin::~ViewWin(%p)\n", this);
#endif

	DetachChildren();
	DeleteFromParent();
	Unmap();

	delete windowRepCallback;

	if (_myPileStack) {
	  _myPileStack->DetachFromWindow();
	  delete _myPileStack;
	}
	if (_parentPileStack) {
	  _parentPileStack->DeleteView(this);
	}
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

Boolean
ViewWin::GetGifDirty()
{
  WindowRep *winRep = GetWindowRep();
  if (winRep != NULL) {
	return winRep->GetGifDirty();
  } else {
	return false;
  }
}

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
ViewWin::ExportImage(DisplayExportFormat format, const char *filename)
{
  DO_DEBUG(printf("ViewWin(%s)::ExportImage(_parent = %p, filename = %s)\n",
    GetName(), _parent, filename));
#if defined(VIEWWIN_TIMER)
  ElapsedTime et;
  et.Start();
#endif

  DevStatus result = StatusOk;

#if 0 //TEMP
  static int count = 0;
  if (++count % 5 == 0) {
      reportErrNosys("ExportImage failed for test");
      return StatusFailed;
  }
#endif //TEMP

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

#if defined(VIEWWIN_TIMER)
  char timeBuf[256];
  sprintf(timeBuf, "View(%s)::ExportImage()", GetName());
  et.ReportTime(timeBuf);
#endif

  return result;
}

void ViewWin::AppendToParent(ViewWin *parent)
{
#if defined(DEBUG)
  printf("ViewWin(%s)::AppendToParent(%s)\n", GetName(), parent->GetName());
#endif

  if ( _parent) {
    _parent->Delete(this);
  }
  if (_mapped) {
    Unmap();
  }
  _parent = parent;
  _parent->Append(this);

//	SetColorParent(_parent);			// Establish coloring inheritance

  // Make sure this gets redrawn if it's a view.
  Refresh();
}

void ViewWin::DeleteFromParent()
{
#if defined(DEBUG)
  printf("ViewWin(%s)::DeleteFromParent()\n", GetName());
#endif

  if (_parent) {
    _parent->Delete(this);
    if (_mapped) {
      Unmap();
    }

    // Make sure pile gets redrawn if we just removed this view from
    // a pile.
    if (GetParentPileStack()) {
      PileStack *ps = GetParentPileStack();
      int index = ps->InitIterator();
      while (ps->More(index)) {
        ViewWin *tmpView = ps->Next(index);
	if (tmpView != this) {
	  tmpView->Refresh(false);
	  break;
	}
      }
      ps->DoneIterator(index);
    } else if (GetFirstSibling()) {
      GetFirstSibling()->Refresh();
    }

    _parent = NULL;
  }

//	SetColorParent(NULL);				// Eliminate coloring inheritance
}

void ViewWin::Map(int x, int y, unsigned w, unsigned h)
{
#if defined(DEBUG)
  printf("ViewWin(0x%p, %s)::Map(%d, %d, %d, %d)\n", this, GetName(),
      x, y, w, h);
#endif
  if (_mapped) {
    fprintf(stderr,"ViewWin::Map() already mapped\n");
    Exit::DoExit(1);
  }

  _hasGeometry = false;

  Boolean relativeMinSize = false;
  Coord min_width = 1;
  Coord min_height = 1;

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
  
  //TEMP -- is this callback ever deleted?  RKW 5/7/97.
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

  if (!_parent) {
    if (Init::DisplayLogo()) {
      /* Allocate top margin */
      _topMargin = 9 + 2;
      /* Draw margins */
      DrawMargins();
    }
  }

  _mapped = true;
  _iconified = false;
  

  SubClassMapped();
}

void ViewWin::Unmap()
{
#if defined(DEBUG)
  printf("ViewWin(%s)::Unmap()\n", GetName());
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

  SubClassUnmapped();
  //TEMP?_winReps.GetScreenWinRep()->DeleteCallback(windowRepCallback);

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
#if defined(DEBUG)
  printf("ViewWin(%s)::Append(%s)\n", GetName(), child->GetName());
#endif

  _children.Append(child);
  if (_myPileStack) _myPileStack->InsertView(child);
}

/* Delete child */

void ViewWin::Delete(ViewWin *child)
{
#if defined(DEBUG)
  printf("ViewWin(%s)::Delete(%s)\n", GetName(), child->GetName());
#endif

  if (!_children.Delete(child)) {
    fprintf(stderr,"ViewWin::Delete child not found\n");
    Exit::DoExit(2);
  }
  if (_myPileStack) _myPileStack->DeleteView(child);
}

/* Set geometry of view */
// Note: I'm not sure what this really does that MoveResize() doesn't do.
void ViewWin::SetGeometry(int x, int y, unsigned w, unsigned h) 
{
  _x = x;
  _y = y;
  _width = w;
  _height = h;
}

/* Get current geometry of child w. r. t. parent */

void ViewWin::RealGeometry(int &x, int &y, unsigned &w, unsigned &h)
{
#if defined(DEBUG)
  printf("ViewWin(%s)::Geometry()\n", GetName());
#endif
  if (!GetWindowRep()) {
    fprintf(stderr,"ViewWin(%s)::Geometry: not mapped\n", GetName());
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
  printf("ViewWin(%s, 0x%p)::MoveResize(%d, %d, %u, %u)\n", GetName(),
    this, x, y, w, h);
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
/* Note that if views are piled, this should only be called from
 * PileStack::Flip().  RKW 1999-05-06. */

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
  printf("ViewWin(0x%p, %s)::Raise()\n", this, GetName());
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
  PColorID	fgid, bgid;

  fgid = GetForeground();
  bgid = GetBackground();

  win->SetForeground(fgid);
  win->SetPattern(Pattern0);
  win->SetLineWidth(0);
  win->FillRect(x, h-y-(_topMargin-1), w - 1, _topMargin - 1);
  win->SetForeground(bgid);
  win->FillRect(x + 1, h-(y + 1)-(_topMargin-1-2), w - 1 - 2,
		_topMargin - 1 - 2);
  win->SetForeground(fgid);
  win->SetFont("Courier", "Medium", "r", "Normal", 8.0);
  win->AbsoluteText((char *) logo, x + 1, h-(y + 1)-(_topMargin-2),
		    w - 2, _topMargin - 2,
		    WindowRep::AlignNorth, true);
  win->SetNormalFont();
}

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
  if (Init::DisplayLogo())
    DrawMargins();
}

void	ViewWin::HandleResize(WindowRep* w, int xlow, int ylow,
							  unsigned width, unsigned height)
{
#if defined(DEBUG)
	printf("ViewWin(%s, 0x%p)::HandleResize %d, %d, %u, %u\n",
		   GetName(), this, xlow, ylow, width, height);
#endif

	_hasGeometry = true;
	_x = _y = 0;			// Why are these forced to zero?  RKW 10/24/96.
	_width = width;
	_height = height;

	if (Init::DisplayLogo())
		DrawMargins();
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

void
ViewWin::SetMyPileStack(PileStack *ps)
{
#if defined(DEBUG)
  const char *psName = ps ? ps->GetName() : "(null)";
  printf("ViewWin(%s)::SetMyPileStack(%s)\n", GetName(), psName);
#endif

  _myPileStack = ps;
}

void
ViewWin::SetParentPileStack(PileStack *ps)
{
#if defined(DEBUG)
  char const *psName = ps ? ps->GetName() : "(null)";
  printf("ViewWin(%s)::SetParentPileStack(%s)\n", GetName(), psName);
#endif

  _parentPileStack = ps;
}

void
ViewWin::SetFont(const char *which, int family, float pointSize,
    Boolean bold, Boolean italic, Boolean notifyPile)
{
#if defined(DEBUG)
  printf("ViewWin(%s)::SetFont()\n", GetName());
#endif

  int index = InitIterator();
  while (More(index)) {
    ViewWin *vw = Next(index);
    vw->SetFont(which, family, pointSize, bold, italic, notifyPile);
  }
  DoneIterator(index);
}

void
ViewWin::Dump(FILE *fp)
{
  fprintf(fp, "  <%s>\n", GetName());
  if (NumChildren() > 0) {
    fprintf(fp, "    children:\n");
    int index = InitIterator();
    while (More(index)) {
      ViewWin *view = Next(index);
      fprintf(fp, "      <%s>\n", view->GetName());
    }
    DoneIterator(index);

    index = InitIterator();
    while (More(index)) {
      ViewWin *view = Next(index);
      view->Dump(fp);
    }
    DoneIterator(index);
  }
}

//******************************************************************************
