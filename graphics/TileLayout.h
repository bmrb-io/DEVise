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
  Revision 1.3  1996/04/11 18:05:31  jussi
  Added support for stacked windows (one overlays another).

  Revision 1.2  1995/12/28 19:04:32  jussi
  Small fixes to remove compiler warnings.

  Revision 1.1  1995/12/06  21:25:14  jussi
  Initial revision.
*/

#ifndef TileLayout_h
#define TileLayout_h

#include "ViewLayout.h"

class TileLayout: public ViewLayout {
public:
  TileLayout(char *name, Coord x = 0.1, Coord y = 0.0, 
	     Coord w = 0.8, Coord h = 0.9, Boolean printExclude = false,
	     Boolean printPixmap = false);

  virtual void SetPreferredLayout(int v, int h, Boolean stacked);

protected:
  virtual void MapChildren(ViewWin *single = 0, Boolean resize = false,
			   int extraWeight = 0, int *x = 0, int *y = 0,
			   unsigned int *w = 0, unsigned int *h = 0);

  virtual void ComputeLayout(unsigned int w, unsigned int h,
			     unsigned int n);

  unsigned int verViews;                // current height of view layout
  unsigned int horViews;                // current width of view layout
};

#endif
