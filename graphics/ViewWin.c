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

#include <stdio.h>
#include <string.h>

#include "ViewWin.h"
#include "Display.h"
#include "WindowRep.h"
#include "Init.h"

#ifdef TK_WINDOW
#include <tcl.h>
#include <tk.h>
#endif

//#define DEBUG

ViewWin::ViewWin(char *name, Color fg, Color bg, int weight, Boolean boundary)
{
  _name = name;
  _windowRep = NULL;
  _parent = NULL;
  _mapped = false;
  _weight = weight;
  _winBoundary = boundary;
  _iconified = true;
  _background = bg;
  _foreground = fg;

#ifdef MARGINS
  _leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
#endif

#ifdef TK_WINDOW
  _marginsOn = false;
  _leftMargin = _rightMargin = _topMargin = _bottomMargin = 0;
  _tkPathName[0] = 0;
#endif
}

void ViewWin::Iconify()
{
  if (_windowRep)
    _windowRep->Iconify();
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
}

void ViewWin::DeleteFromParent()
{
  if (_parent) {
    _parent->Delete(this);
    if (_mapped)
      Unmap();
    _parent = NULL;
  }
}

void ViewWin::Map(int x, int y, unsigned w, unsigned h)
{
  if (_mapped) {
    fprintf(stderr,"ViewWin::Map() already mapped\n");
    Exit::DoExit(1);
  }

  _hasGeometry = false;

  Boolean relativeMinSize = false;
  Coord min_width = 170;
  Coord min_height = 100;

  if (_parent) {
#ifdef DEBUG
    printf("ViewWin 0x%p mapping to parent 0x%p\n", this,
	   _parent->GetWindowRep());
#endif
    _windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(_name,
		    x, y, w, h, _foreground, _background,
		    _parent->GetWindowRep(), min_width, min_height,
                    relativeMinSize, _winBoundary);
    _windowRep->RegisterCallback(this);
#ifdef TK_WINDOW_old
    _windowRep->Decorate(_parent->GetWindowRep(), _name,
			 (unsigned int)min_width,
			 (unsigned int)min_height);
#endif
  } else {
    /* Create a new WindowRep */
#ifdef DEBUG
    printf("ViewWin 0x%p mapping to root\n", this);
#endif
    _windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(_name,
		    x, y, w, h, ForegroundColor, BackgroundColor, NULL,
		    min_width, min_height, relativeMinSize, _winBoundary);
    _windowRep->RegisterCallback(this);
#ifdef MARGINS
    if (Init::DisplayLogo()) {
      /* Allocate top margin */
      _topMargin = _windowRep->GetSmallFontHeight() + 2;
      /* Draw margins */
      DrawMargins();
    }
#endif
#ifdef TK_WINDOW_old
    _windowRep->Decorate(NULL, _name, (unsigned int)min_width,
			 (unsigned int)min_height);
#endif
  }

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

#ifdef TK_WINDOW_old
  _windowRep->Undecorate();
#endif
#ifdef TK_WINDOWxxx
  /* Drop margin controls */
  if (_marginsOn)
    DropMarginControls();
#endif

  SubClassUnmapped();
  DeviseDisplay::DefaultDisplay()->DestroyWindowRep(_windowRep);
  _windowRep = NULL;
  _hasGeometry = false;
  _mapped = false;
}

ViewWin::~ViewWin()
{
#ifdef DEBUG
  printf("ViewWin destructor\n");
#endif

  DeleteFromParent();
  Unmap();
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

/* Get current geometry of child w. r. t. parent */

#if defined(MARGINS) || defined(TK_WINDOW)
void ViewWin::RealGeometry(int &x, int &y, unsigned &w, unsigned &h)
#else
void ViewWin::Geometry(int &x, int &y, unsigned &w, unsigned &h)
#endif
{
  if (!_windowRep) {
    fprintf(stderr,"ViewWin::Geometry: not mapped\n");
    Exit::DoExit(2);
  }

  if (!_hasGeometry) {
#ifdef DEBUG
    printf("ViewWin::Geometry from WindowRep 0x%p\n", _windowRep);
#endif
    _windowRep->Dimensions(_width, _height);
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
  if (!_windowRep) {
    fprintf(stderr,"ViewWin::AbsoluteOrigin: not mapped\n");
    Exit::DoExit(2);
  }
  _windowRep->AbsoluteOrigin(x, y);
}

/* Move/Resize ourselves */

void ViewWin::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("ViewWin::MoveResize 0x%p, %d, %d, %u, %u\n", this, x, y, w, h);
#endif

  if (!_mapped) {
    fprintf(stderr,"ViewWin::MoveResize not mapped\n");
    Exit::DoExit(1);
  } else {
    _hasGeometry = false;
    _windowRep->MoveResize(x, y, w, h);
    HandleResize(_windowRep, x, y, w, h);
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

void ViewWin::HandleResize(WindowRep *w, int xlow, int ylow,
			   unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("ViewWin::HandleResize 0x%p, %d, %d, %u, %u\n",
	 this, xlow, ylow, width, height);
#endif

  _hasGeometry = true;
  _x = _y = 0;
  _width = width;
  _height = height;

#ifdef MARGINS
  if (Init::DisplayLogo())
    DrawMargins();
#endif

#ifdef TK_WINDOW
  /* Resize margin controls */
  if (_marginsOn)
    ResizeMargins(_width, _height);
#endif
}

void ViewWin::HandleWindowMappedInfo(WindowRep *, Boolean mapped)
{
#ifdef DEBUG
  printf("ViewWin::HandleWindowMappedInfo 0x%p %d\n", this, mapped);
#endif

  _iconified = !mapped;
  Iconify(_iconified);
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
  _children.Swap(child1, child2);
}

/* Raise window to top of stacking order */

void ViewWin::Raise()
{
  if (_windowRep) {
    _windowRep->Raise();
    _windowRep->Flush();
  }
}

/* Lower window to bottom of stacking order */

void ViewWin::Lower()
{
  if (_windowRep) {
    _windowRep->Lower();
    _windowRep->Flush();
  }
}

void ViewWin::SetFgBgColor(Color fg, Color bg)
{
  _foreground = fg;
  _background = bg;

  if (_windowRep)
    _windowRep->SetWindowBgColor(bg);
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
#ifdef DEBUG
  printf("ViewWin::DrawMargins\n");
#endif

  if (!_topMargin)
    return;

  char *logo = "Visualization by DEVise (c) 1996";

  int x, y;
  unsigned int w, h;
  RealGeometry(x, y, w, h);

  // draw logo
  WindowRep *win = GetWindowRep();
  win->SetSmallFont();
  win->SetFgColor(GetFgColor());
  win->FillRect(x, y, w - 1, _topMargin - 1);
  win->SetFgColor(GetBgColor());
  win->FillRect(x + 1, y + 1, w - 1 - 2, _topMargin - 1 - 2);
  win->SetFgColor(GetFgColor());
  win->AbsoluteText(logo, x + 1, y + 1, w - 2, _topMargin - 2,
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
  _windowRep->Reparent(true, (void *)Tk_WindowId(tkWindow), xoff, yoff);
}

void ViewWin::ResizeMargins(unsigned int w, unsigned int h)
{
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
