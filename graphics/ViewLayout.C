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
 * Revision 1.1  1995/12/02  20:53:05  jussi
 * Initial revision.
 **/

#include "ViewLayout.h"

//#define DEBUG

/* top level window */

ViewLayout:: ViewLayout(char *name,  Coord x, Coord y, Coord w, Coord h) :
	ViewWin(name)
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
    DoResize(TotalWeight() + child->GetWeight(), x, y, w, h);

    /* map the child */
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
    int x, y;
    unsigned int w, h;
    DoResize(TotalWeight(), x, y, w, h);
  }
}

void ViewLayout::HandleResize(WindowRep *win, int x, int y,
			      unsigned w, unsigned h)
{
#ifdef DEBUG
  printf("ViewLayout::HandleResize 0x%x at %d,%d, size %u,%u\n",
	 this, x, y, w, h);
#endif

  ViewWin::HandleResize(win, x, y, w, h);
  if (Mapped()) {
    int x, y;
    unsigned int w, h;
    DoResize(TotalWeight(), x, y, w, h);
  }
}

void ViewLayout::UnmapChildren()
{
  for(int index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    vw->Unmap();
  }
  DoneIterator(index);
}

void ViewLayout::Iconify(Boolean iconified)
{
  for(int index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    vw->Iconify(iconified);
  }
  DoneIterator(index);
}
