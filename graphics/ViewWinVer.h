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
  Revision 1.2  1995/09/05 21:13:22  jussi
  Added/updated CVS header.
*/

/* stacks its children vertically */

#ifndef ViewWinVer_h
#define ViewWinVer_h

#include "ViewLayout.h"

class ViewWinVer: public ViewLayout {
public:
  ViewWinVer(char *name, Coord x = 0.1, Coord y = 0.0, 
	     Coord w = 0.8, Coord h = 0.9);

  virtual void Replace(ViewWin *child1, ViewWin *child2);
  virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

protected:
  virtual void MapChildren();
  virtual void DoResize(int totalWeight, int &x, int &y,
			unsigned int &w, unsigned int &h);
};

#endif
