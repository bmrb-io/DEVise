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
  Revision 1.3  1995/11/29 15:08:06  jussi
  Added preliminary stuff needed for Tk window support.

  Revision 1.2  1995/09/05 21:13:18  jussi
  Added/updated CVS header.
*/

#include <stdio.h>

#include "ViewWin.h"
#include "Display.h"
#include "WindowRep.h"

#ifdef TK_WINDOW
#include <tcl.h>
#include <tk.h>
#include "TkControl.h"
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
}

void ViewWin::Iconify()
{
  if (_windowRep != NULL)
    _windowRep->Iconify();
}

void ViewWin::AppendToParent(ViewWin *parent)
{
  if ( _parent != NULL) {
    _parent->Delete(this);
  }
  if (_mapped)
    Unmap();
  _parent = parent;
  _parent->Append(this);
}

void ViewWin::DeleteFromParent()
{
  if (_parent != NULL){
    _parent->Delete(this);
    if (_mapped)
      Unmap();
    _parent = NULL;
  }
}

void ViewWin::Map(int x, int y, unsigned w, unsigned h)
{
  if (_mapped){
    fprintf(stderr,"ViewWin::Map() already mapped\n");
    Exit::DoExit(1);
  }

  if (_parent != NULL){
#ifdef DEBUG
    printf("ViewWin 0x%x mapping to parent 0x%x\n", this,
	   _parent->GetWindowRep());
#endif
    _windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(_name,
		    x,y,w,h,_foreground,_background,_parent->GetWindowRep(),
		    1.0,1.0,false, _winBoundary);
    _windowRep->RegisterCallback(this);
  } else {
    /* Create a new WindowRep */
#ifdef DEBUG
    printf("ViewWin 0x%x mapping to root\n", this);
#endif
#ifdef TK_WINDOW
    static int tkWinCount = 1;
    char pathname[32];
    sprintf(pathname, ".devisewin%d", tkWinCount++);
    Tcl_Interp *interp = ControlPanel::Instance()->GetInterp();
    Tk_Window mainWindow = ControlPanel::Instance()->GetMainWindow();
    Tk_Window _tkWindow = Tk_CreateWindowFromPath(interp, mainWindow,
						  pathname, 0);
    if (!_tkWindow) {
      fprintf(stderr, "Cannot create Tk window %s: %s\n", pathname,
	      interp->result);
      Exit::DoExit(2);
    }
    Tk_MoveWindow(_tkWindow, x, y);
    Tk_GeometryRequest(_tkWindow, w, h);
#endif
    _windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(_name,
		    x,y,w,h,BlackColor,WhiteColor,NULL,
		    100, 100, false, _winBoundary);
    _windowRep->RegisterCallback(this);
  }

  _hasGeometry = false;
  _mapped = true;
  SubClassMapped();
}

void ViewWin::Unmap()
{
#ifdef DEBUG
  printf("ViewWin 0x%x unmapping\n", this);
#endif

  if (_mapped){
    SubClassUnmapped();
    DeviseDisplay::DefaultDisplay()->DestroyWindowRep(_windowRep);
    _windowRep = NULL;
    _hasGeometry = false;
    _mapped = false;
  }
}

Boolean ViewWin::Mapped()
{
  return _mapped;
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

void ViewWin::Geometry(int &x, int &y, unsigned &w, unsigned &h)
{
  if (_windowRep == NULL) {
    fprintf(stderr,"ViewWin::Geometry: not mapped\n");
    Exit::DoExit(2);
  }
  if (!_hasGeometry){
    _windowRep->Dimensions(_width, _height);
    _windowRep->Origin(_x, _y);
    _hasGeometry = true;
  }
  x = _x;
  y = _y;
  w = _width;
  h = _height;
}

void ViewWin::AbsoluteOrigin(int &x, int &y)
{
  if (_windowRep == NULL) {
    fprintf(stderr,"ViewWin::AbsoluteOrigin: not mapped\n");
    Exit::DoExit(2);
  }
  _windowRep->AbsoluteOrigin(x,y);
}

/* Move/Resize ourselves */

void ViewWin::MoveResize(int x, int y, unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("ViewWin::MoveResize 0x%x,%d,%d,%d,%d\n", this,x,y,x+w-1,y+h-1);
#endif

  if (!_mapped) {
    fprintf(stderr,"ViewWin::MoveResize not mapped\n");
    Exit::DoExit(1);
  } else
    _windowRep->MoveResize(x,y,w,h);
}

int ViewWin::TotalWeight()
{
  int w = 0;
  int index;
  for (index=InitIterator(); More(index);){
    ViewWin *vw= Next(index);
    w += vw->GetWeight();
  }
  DoneIterator(index);
  return w;
}

void ViewWin::HandleResize(WindowRep * w, int xlow,
			   int ylow, unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("ViewWin::HandleResize 0x%x,%d,%d,%d,%d\n",
	 this,xlow,ylow,width,height);
#endif

  _hasGeometry = true;
  _x = xlow;
  _y = ylow;
  _width = width;
  _height = height;
}

void ViewWin::HandleWindowMappedInfo(WindowRep *, Boolean mapped)
{
  _iconified = !mapped;
  Iconify(_iconified);
}

Boolean ViewWin::Iconified()
{
  if (_parent != NULL){
    /* subwindow must query parent */
    return _parent->Iconified();
  } else {
    /* no parent: this is a top level window. */
    if (Mapped() && !_iconified)
      return false;
    else
      return true;
  }
}

/* Replace child1 by child2. child1 must be a valid child.
   Update the list of children.*/

void ViewWin::Replace(ViewWin *child1, ViewWin *child2)
{
#ifdef DEBUG
  printf("ViewWin::Replace 0x%x with 0x%x\n", child1, child2);
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
  fprintf(stderr,"ViewWin::Replace: can't find child1\n");
  Exit::DoExit(2);
}

/* Swap child1 and child2. Both must be valid children, and child1 != child2.*/

void ViewWin::SwapChildren(ViewWin *child1, ViewWin *child2)
{
  _children.Swap(child1, child2);
}
