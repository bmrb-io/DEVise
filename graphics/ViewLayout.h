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

  $Log$*/

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
  virtual void Iconify(Boolean iconified);
  
  virtual void Append(ViewWin *child);
  virtual void Delete(ViewWin *child);
  virtual void Replace(ViewWin *child1, ViewWin *child2) = 0;
  virtual void SwapChildren(ViewWin *child1, ViewWin *child2) = 0;

protected:
  virtual void SubClassMapped() {};
  virtual void SubClassUnmapped() {};
  
  virtual void DoResize(int totalWeight, int &x, int &y,
			unsigned int &w, unsigned int &h) = 0;

  virtual void MapChildren() = 0;
  virtual void UnmapChildren();

  virtual void HandleResize(WindowRep *w, int xlow, int ylow,
			    unsigned width, unsigned height);
};

#endif
