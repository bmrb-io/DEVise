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
  Revision 1.12  1999/04/20 14:13:32  wenger
  Improved debug output.

  Revision 1.11  1999/01/04 15:33:18  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.10  1998/02/26 00:19:06  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.9  1997/11/24 23:14:35  weaver
  Changes for the new ColorManager.

  Revision 1.8  1997/01/08 19:01:44  wenger
  Fixed bug 064 and various other problems with drawing piled views;
  added related debug code.

  Revision 1.7  1996/04/16 19:49:40  jussi
  Replaced assert() calls with DOASSERT().

  Revision 1.6  1996/04/11 18:04:08  jussi
  Added initialization of _stacked member variable. Fixed bug
  in computation of window geometry in SwapChildren().

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

//******************************************************************************

#include <math.h>


#include "ViewLayout.h"

//#define DEBUG

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewLayout:: ViewLayout(char* name,  Coord x, Coord y, Coord w, Coord h)
	: ViewWin(name), verRequested(-1), horRequested(-1), _stacked(false)
{
#if defined(DEBUG)
  printf("ViewLayout(0x%p)::ViewLayout(%s)\n", this, name);
#endif
}

ViewLayout::~ViewLayout(void)
{
	DeleteFromParent();
	Unmap();
}

//******************************************************************************

void ViewLayout::Map(int x, int y, unsigned w, unsigned h)
{
#if defined(DEBUG)
  printf("ViewLayout(%s)::Map(%d, %d, %d, %d)\n", GetName(), x, y, w, h);
#endif

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
#if defined(DEBUG)
  printf("ViewLayout::SwapChildren()\n");
#endif

  ViewWin::SwapChildren(child1,child2);

  if (!Mapped())
    return;

  int x1, y1, x2, y2;
  unsigned w1, w2, h1, h2;
  child1->RealGeometry(x1, y1, w1, h1);
  child2->RealGeometry(x2, y2, w2, h2);
  child1->GetWindowRep()->Origin(x1, y1);
  child2->GetWindowRep()->Origin(x2, y2);

  // a window containing two views would give an unequal amount of space
  // for the two views if the height (or width) of the window is odd;
  // I've disabled the assertion below for this reason
#if 0
  DOASSERT(w1 == w2 && h1 == h2, "Incompatible window sizes");
#endif

  child1->MoveResize(x2, y2, w2, h2);
  child2->MoveResize(x1, y1, w1, h1);
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

//******************************************************************************
// Callback Methods (WindowRepCallback)
//******************************************************************************

void	ViewLayout::HandleResize(WindowRep* win, int x, int y,
								 unsigned w, unsigned h)
{
#if defined(DEBUG)
	printf("ViewLayout::HandleResize 0x%p at %d,%d, size %u,%u\n",
		   this, x, y, w, h);
    printf("  name = %s\n", GetName());
#endif

	ViewWin::HandleResize(win, x, y, w, h);

	if (Mapped())
		MapChildren(0, true);
}

//******************************************************************************
