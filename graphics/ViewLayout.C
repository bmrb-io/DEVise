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
  Revision 1.5  1995/12/28 18:53:07  jussi
  Small fixes to remove compiler warnings.

  Revision 1.4  1995/12/14 22:32:39  jussi
  Disabled an assertion which may unnecessarily fail.

  Revision 1.3  1995/12/14 15:31:26  jussi
  Moved Replace and SwapChildren to this base class from derived class
  (used to be WinVertical and WinHorizontal, now TileLayout).

  Revision 1.2  1995/12/02 21:15:35  jussi
  Commented out DEBUG statement.

  Revision 1.1  1995/12/02  20:53:05  jussi
  Initial revision.
*/

#include <assert.h>
#include <math.h>

#include "ViewLayout.h"

//#define DEBUG

ViewLayout:: ViewLayout(char *name,  Coord x, Coord y, Coord w, Coord h) :
	ViewWin(name), verRequested(-1), horRequested(-1),
	_stacked(false)
{
}

ViewLayout::~ViewLayout()
{
  DeleteFromParent();
  Unmap();
}

void ViewLayout::Map(int x, int y, unsigned w, unsigned h)
{
  ViewWin::Map(x, y, w, h);
  MapChildren();
}

void ViewLayout::Unmap()
{
  UnmapChildren();
  ViewWin::Unmap();
}

void ViewLayout::Append(ViewWin *child)
{
  if (Mapped()) {
    /* Resize existing children, compute left-over space for new child */
    int x, y;
    unsigned int w, h;
    MapChildren(0, true, child->GetWeight(), &x, &y, &w, &h);
    child->Map(x, y, w, h);
  }

  /* append child */
  ViewWin::Append(child);
}

void ViewLayout::Delete(ViewWin *child)
{
  ViewWin::Delete(child);

  if (Mapped()) {
    child->Unmap();
    MapChildren(0, true);
  }
}

/* Replace child1 by child2. child1 must be a valid child. */

void ViewLayout::Replace(ViewWin *child1, ViewWin *child2)
{
#ifdef DEBUG
  printf("ViewLayout::Replace: %s %s\n", child1->GetName(),
	 child2->GetName());
#endif

  ViewWin::Replace(child1, child2);

  if (!Mapped())
    return;

  MapChildren(child2);
}

/* Swap position of child1 and child2 */

void ViewLayout::SwapChildren(ViewWin *child1, ViewWin *child2)
{
  ViewWin::SwapChildren(child1,child2);

  if (!Mapped())
    return;

  int x1, y1, x2, y2;
  unsigned w1, w2, h1, h2;
#if defined(MARGINS) || defined(TK_WINDOW)
  child1->RealGeometry(x1, y1, w1, h1);
  child2->RealGeometry(x2, y2, w2, h2);
#else
  child1->Geometry(x1, y1, w1, h1);
  child2->Geometry(x2, y2, w2, h2);
#endif
  child1->GetWindowRep()->Origin(x1, y1);
  child2->GetWindowRep()->Origin(x2, y2);

  // a window containing two views would give an unequal amount of space
  // for the two views if the height (or width) of the window is odd;
  // I've disabled the assertion below for this reason
#if 0
  assert(w1 == w2 && h1 == h2);
#endif

  child1->MoveResize(x2, y2, w2, h2);
  child2->MoveResize(x1, y1, w1, h1);
}

void ViewLayout::HandleResize(WindowRep *win, int x, int y,
			      unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("ViewLayout::HandleResize 0x%x at %d,%d, size %u,%u\n",
	 this, x, y, w, h);
#endif

  ViewWin::HandleResize(win, x, y, w, h);
  if (Mapped())
    MapChildren(0, true);
}

void ViewLayout::Iconify(Boolean iconified)
{
  int index;
  for(index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    vw->Iconify(iconified);
  }
  DoneIterator(index);
}

void ViewLayout::UnmapChildren()
{
  int index;
  for(index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    vw->Unmap();
  }
  DoneIterator(index);
}
