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
  Revision 1.3  1995/12/28 19:04:26  jussi
  Small fixes to remove compiler warnings.

  Revision 1.2  1995/12/14 15:30:24  jussi
  Small fixes.

  Revision 1.1  1995/12/06  21:25:13  jussi
  Initial revision.
*/

#include <assert.h>
#include <math.h>

#include "TileLayout.h"
#include "Display.h"

//#define DEBUG

TileLayout:: TileLayout(char *name,  Coord x, Coord y, Coord w, Coord h) :
	ViewLayout(name)
{
  Coord width, height;
  DeviseDisplay::DefaultDisplay()->Dimensions(width, height);
  Map((int)(x * width), (int)(y * height),
      (unsigned)(w * width), (unsigned)(h * height));
}

void TileLayout::SetPreferredLayout(int v, int h, Boolean stacked)
{
  _stacked = stacked;

  if (!stacked) {
    // note that the layout can be fixed in only one direction
    horRequested = (h < 1 ? -1 : h);
    if (horRequested >= 1)
      verRequested = -1;
    else
      verRequested = (v < 1 ? -1 : v);
  }

  if (Mapped())
    MapChildren(0, true);
}

// Do actual mapping of each child

void TileLayout::MapChildren(ViewWin *single, Boolean resize,
			     int extraWeight, int *x, int *y,
			     unsigned int *w, unsigned int *h)
{
#ifdef DEBUG
  printf("TileLayout::MapChildren 0x%x mapping children\n", this);
#endif

  int totalWeight = TotalWeight() + extraWeight;

  int _x, _y;
  unsigned int _w, _h;
  Geometry(_x, _y, _w, _h);

  if (_stacked) {
    // in a stacked view, all children get the total screen space;
    // only the top view is visible, and it's up to someone else
    // to decide which view is on top
#ifdef DEBUG
    printf("TileLayout::MapChildren: stacking views\n");
#endif
    int index;
    for(index = InitIterator(); More(index);) {
      ViewWin *vw = Next(index);
      if (resize)
	vw->MoveResize(_x, _y, _w, _h);
      else
	vw->Map(_x, _y, _w, _h);
    }
    DoneIterator(index);
    if (x) {
      *x = _x;
      *y = _y;
      *w = _w;
      *h = _h;
    }
    return;
  }

  const unsigned int numViews = NumChildren() + (x ? 1 : 0);
  ComputeLayout(_w, _h, numViews);
#ifdef DEBUG
  printf("TileLayout::MapChildren: using %dx%d layout for %d views\n",
	 verViews, horViews, numViews);
#endif
  assert(verViews * horViews >= numViews);

  // compute default, unweighted width and height of views
  unsigned int height = (int)(1.0 * _h / verViews);
  unsigned int width = (int)(1.0 * _w / horViews);

  unsigned int xoff = 0, yoff = 0;

  int index;
  for(index = InitIterator(); More(index);) {
    ViewWin *vw = Next(index);

    // if vertical stack of views, compute height based on weight
    if (horViews == 1)
      height = (int)(1.0 * vw->GetWeight() / totalWeight * _h);

    // if horizontal arrangement, compute width based on weight
    if (verViews == 1)
      width = (int)(1.0 * vw->GetWeight() / totalWeight * _w);

    // see if we're instructed to ignore all but one child
    if (!single || single == vw) {
      // see which method to call, resize or map
      if (resize)
	vw->MoveResize(_x + xoff, _y + yoff, width, height);
      else
	vw->Map(_x + xoff, _y + yoff, width, height);
    }

    // compute position of next view
    if (horViews == 1)
      yoff += height;
    else if (verViews == 1)
      xoff += width;
    else {
      xoff += width;
      // no more views fit in horizontally?
      if (_x + xoff + width > _x + _w) {
	xoff = 0;
	yoff += height;
      }
    }
  }

  DoneIterator(index);

  // see if we need to report back any unused space (for one more child)
  if (x) {
    assert(x && y && w && h);
    *x = _x + xoff;
    *y = _y + yoff;
    if (horViews == 1)
      *h = _h - yoff;
    else
      *h = height;
    if (verViews == 1)
      *w = _w - xoff;
    else
      *w = width;
  }
}

void TileLayout::ComputeLayout(unsigned int w, unsigned int h,
			       unsigned int numViews)
{
  if (numViews <= 1) {
    horViews = verViews = 1;
    return;
  }

  // if width of vertical layout is fixed, arrange accordingly
  if (horRequested >= 1) {
    unsigned int h = (unsigned int)horRequested;
    horViews = (h < numViews ? h : numViews);
    verViews = numViews / horViews;
    if (numViews % horViews)
      verViews++;
    return;
  }

  // if height of horizontal layout is fixed, arrange accordingly
  if (verRequested >= 1) {
    unsigned int v = (unsigned int)verRequested;
    verViews = (v < numViews ? v : numViews);
    horViews = numViews / verViews;
    if (numViews % verViews)
      horViews++;
    return;
  }

  // otherwise, we have total control over the layout, so...

  // if window is much taller than wide, stack views vertically
  // in a single column

  if (h >= 1.5 * w) {
    horViews = 1;
    verViews = numViews;
    return;
  }

  // if window is much wider than tall, arrange views horizontally
  // in a single row -- because we typically display timeseries
  // data requiring more horizontal than vertical space, we have
  // a little more tendency to stack views vertically (see above)
  // than horizontally

  if (w >= 3 * h) {
    horViews = numViews;
    verViews = 1;
    return;
  }

  // otherwise, arrange views so that there's roughly an equal number of
  // rows and columns; also try to make sure whole window is used
  // efficiently (no unused blocks)

  for(horViews = (int)sqrt(numViews); horViews >= 1; horViews--) {
    if (numViews % horViews == 0)
      break;
  }

  verViews = numViews / horViews;
}
