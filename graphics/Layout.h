/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.2  1997/03/28 16:09:14  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

/* A new layout class for DEVise combining different kinds of layouts */

#include "ViewLayout.h"


enum LayoutMode  { VERTICAL, HORIZONTAL, STACKED, AUTOMATIC, CUSTOM };

class Layout : public ViewLayout {
  public :
  
  Layout(char *name, Coord x = 0.1, Coord y = 0.0, 
	 Coord w = 0.8, Coord h = 0.9, Boolean printExclude = false,
	 Boolean printPixmap = false);

  virtual void SetPreferredLayout(int v, int h, Boolean stacked);
  virtual void SetLayoutProperties(LayoutMode mode, int rows, int columns);

  virtual void Append(ViewWin *child);
  virtual void Delete(ViewWin *child);
  virtual void HandleResize(WindowRep *win, int x, int y,
			    unsigned w, unsigned h);
  virtual Boolean HandleWindowDestroy(WindowRep *w);
  virtual void HandleWindowMappedInfo(WindowRep *w, Boolean mapped);

  protected :
  virtual void MapChildren(ViewWin *single = 0, Boolean resize = false,
			   int extraWeight = 0, int *x = 0, int *y = 0,
			   unsigned int *w = 0, unsigned int *h = 0);
  
  virtual void ComputeLayout(unsigned int w, unsigned int h,
			     unsigned int numViews,
			     int &horViews, int &verViews);
  
  void ScaleChildren(int oldX, int oldY, unsigned oldW, unsigned oldH,
			     int oldX0, int oldY0);
  private :
  LayoutMode _mode;
};
#endif			   







