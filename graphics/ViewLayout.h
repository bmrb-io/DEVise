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
  Revision 1.2  1995/12/14 15:31:40  jussi
  Moved Replace and SwapChildren to this base class from derived class
  (used to be WinVertical and WinHorizontal, now TileLayout).

  Revision 1.1  1995/12/02  20:53:06  jussi
  Initial revision.
*/

#ifndef ViewLayout_h
#define ViewLayout_h

#include "ViewWin.h"
#include "WindowRep.h"

class ViewLayout: public ViewWin {
public:
  ViewLayout(char *name, Coord x = 0.1, Coord y = 0.0, 
	     Coord w = 0.8, Coord h = 0.9);
  ~ViewLayout();

  virtual void Map(int x, int y, unsigned w, unsigned h);
  virtual void Unmap();

  virtual void Append(ViewWin *child);
  virtual void Delete(ViewWin *child);
  
  virtual void Replace(ViewWin *child1, ViewWin *child2);
  virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

  virtual void HandleResize(WindowRep *w, int xlow, int ylow,
			    unsigned width, unsigned height);
  virtual void Iconify(Boolean iconified);

  virtual void SetPreferredLayout(int v, int h, Boolean stacked = false) {
    verRequested = v; horRequested = h;
  }
  virtual void GetPreferredLayout(int &v, int &h, Boolean &stacked) {
    v = verRequested; h = horRequested; stacked = _stacked;
  }

protected:
  virtual void SubClassMapped() {}
  virtual void SubClassUnmapped() {}
  
  virtual void MapChildren(ViewWin *single = 0, Boolean resize = false,
			   int extraWeight = 0, int *x = 0, int *y = 0,
			   unsigned int *w = 0, unsigned int *h = 0) = 0;
  virtual void UnmapChildren();

  int verRequested;                     // requested height of view layout
  int horRequested;                     // requested width of view layout
  Boolean _stacked;                     // true if stacked view requested
};

#endif
