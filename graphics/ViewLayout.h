/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.8  2000/03/14 17:05:15  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.7  1999/11/29 21:07:52  wenger
  Fixed bug 535 and partially fixed bug 532 (problems with view order in
  piles); removed (unused) replaceView command and related ViewWin methods

  Revision 1.6  1997/11/24 23:14:35  weaver
  Changes for the new ColorManager.

  Revision 1.5  1997/04/03 21:31:10  wenger
  Eliminated unnecessary tcl dependencies in graphics directory.

  Revision 1.4  1997/02/03 19:40:01  ssl
  1) Added a new Layout interface which handles user defined layouts
  2) Added functions to set geometry and remap views as changes in the
     layout editor
  3) Added a function to notify the front end of some change so that it
     can execute a Tcl command
  4) The old TileLayout.[Ch] files still exist but are commented out
     conditionally using #ifdef NEW_LAYOUT

  Revision 1.3  1996/04/11 18:04:58  jussi
  Added support for stacked windows (one overlays another).

  Revision 1.2  1995/12/14 15:31:40  jussi
  Moved Replace and SwapChildren to this base class from derived class
  (used to be WinVertical and WinHorizontal, now TileLayout).

  Revision 1.1  1995/12/02  20:53:06  jussi
  Initial revision.
*/

//******************************************************************************
//
//******************************************************************************

#ifndef __VIEWLAYOUT_H
#define __VIEWLAYOUT_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "ViewWin.h"
#include "WindowRep.h"
#include "ObjectValid.h"

//******************************************************************************
// class ViewLayout
//******************************************************************************

class ViewLayout : public ViewWin
{
	public:

		// Constructors and Destructors
		ViewLayout(char* name, Coord x = 0.1, Coord y = 0.0, 
				   Coord w = 0.8, Coord h = 0.9);
		virtual ~ViewLayout(void);

  virtual void Map(int x, int y, unsigned w, unsigned h);
  virtual void Unmap();

  virtual void Append(ViewWin *child);
  virtual void Delete(ViewWin *child);
  
  virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

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


	protected:

		// Callback methods (WindowRepCallback)
		virtual void	HandleResize(WindowRep* w, int xlow, int ylow,
									 unsigned width, unsigned height);

private:
  ObjectValid _objectValid;
};

//******************************************************************************
#endif
