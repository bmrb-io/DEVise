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
  Revision 1.2  1995/09/05 21:13:20  jussi
  Added/updated CVS header.
*/

#include "ViewWinHor.h"
#include "Display.h"

//#define DEBUG

ViewWinHor::ViewWinHor(char *name,  Coord x, Coord y, Coord w, Coord h) :
	ViewLayout(name, x, y, w, h)
{
  Coord width, height;
  DeviseDisplay::DefaultDisplay()->Dimensions(width, height);
  Map((int)(x * width), (int)(y * height),
      (unsigned)(w * width), (unsigned)(h * height));
}

/* Replace child1 by child2. child1 must be a valid child. */

void ViewWinHor::Replace(ViewWin *child1, ViewWin *child2)
{
#ifdef DEBUG
  printf("ViewWinHor::Replace: %s %s\n", child1->GetName(),
	 child2->GetName());
#endif

  ViewWin::Replace(child1, child2);

#ifdef DEBUG
  printf("ViewWinHor::Replace 2\n");
#endif

  if (!Mapped()) {
    printf("not mapped\n");
    return;
  }

#ifdef DEBUG
  printf("viewWin mapped\n");
#endif

  if (NumChildren() <= 0)
    return;

  int totalWeight = TotalWeight();
  int x, y;
  unsigned int width, height;
  Geometry(x, y, width, height);

  for(int index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    int widthIncrement = (int)(((Coord)vw->GetWeight()) / totalWeight * width);
    if (vw == child2) {
      printf("map %d %d %d %f\n", x, y, widthIncrement, height);
      vw->Map(x, y, widthIncrement, height);
      break;
    }
    x += widthIncrement;
  }
  DoneIterator(index);
}

/* Swap position of child1 and child2 */

void ViewWinHor::SwapChildren(ViewWin *child1, ViewWin *child2)
{
  ViewWin::SwapChildren(child1,child2);

  if (!Mapped())
    return;

  int x1, y1, x2, y2;
  unsigned w1, w2, h1, h2;
#ifdef TK_WINDOW
  child1->RealGeometry(x1, y1, w1, h1);
  child2->RealGeometry(x2, y2, w2, h2);
#else
  child1->Geometry(x1, y1, w1, h1);
  child2->Geometry(x2, y2, w2, h2);
#endif
  child1->GetWindowRep()->Origin(x1, y1);
  child2->GetWindowRep()->Origin(x2, y2);
  child1->MoveResize(x2, y2, w1, h1);
  child2->MoveResize(x1, y1, w2, h2);
}

/* Do actual mapping of each child. */

void ViewWinHor::MapChildren()
{
#ifdef DEBUG
  printf("ViewWinHor 0x%x mapping children\n", this);
#endif

  int totalWeight = TotalWeight();

  int x, y;
  unsigned int w, h;
  Geometry(x, y, w, h);

  for(int index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    int widthIncrement = (int)(((Coord)vw->GetWeight()) / totalWeight * w);
    vw->Map(x, y, widthIncrement, h);
    x += widthIncrement;
  }
  DoneIterator(index);
}

/* Resize each child. Return remaining space to caller. */

void ViewWinHor::DoResize(int totalWeight, int &x, int &y,
			  unsigned int &w, unsigned int &h)
{
#ifdef DEBUG
  printf("ViewWinHor 0x%x resizing children\n", this);
#endif

  Geometry(x, y, w, h);

  int remainingW = w;

  for(int index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);
    int widthIncrement = (int)(((Coord)vw->GetWeight()) / totalWeight * w);
    vw->MoveResize(x, y, widthIncrement, h);
    x += widthIncrement;
    remainingW -= widthIncrement;
  }
  DoneIterator(index);

  w = remainingW;
}
