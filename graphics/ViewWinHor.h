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
  Revision 1.2  1995/09/05 21:13:21  jussi
  Added/updated CVS header.
*/

/* stacks its children vertically */

#ifndef ViewWinHor_h
#define ViewWinHor_h

#include "ViewLayout.h"

class ViewWinHor: public ViewLayout {
public:
  ViewWinHor(char *name,  Coord x = 0.0, Coord y = 0.2, 
	     Coord w = 0.2, Coord h = 0.7);

  virtual void Replace(ViewWin *child1, ViewWin *child2);
  virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

protected:
  virtual void MapChildren();
  virtual void DoResize(int totalWeight, int &x, int &y,
			unsigned int &w, unsigned int &h);
};

#endif
