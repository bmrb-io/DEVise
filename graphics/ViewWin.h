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
  Revision 1.6  1996/01/30 00:03:38  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.5  1995/12/29 22:41:54  jussi
  Corrected type of GetFgColor() method.

  Revision 1.4  1995/12/02 21:25:50  jussi
  Added support for TK_WINDOW i.e. Tk controls surrounding each
  view and window.

  Revision 1.3  1995/11/29 15:06:19  jussi
  Added copyright notice.

  Revision 1.2  1995/09/05 21:13:19  jussi
  Added/updated CVS header.
*/

/*
   A ViewWin is consist of two parts:
   1) WindowRep part that is its window
   2) children: subwindows

   The base class keeps track of the children, while the derived classes
   determine how to create the window representation of the
   children and how children are to be handled.

   Protocol for creating a new WindowRep in the constructor:
   Call the Map() function with the size of the window.
   1) If parent == NULL, a top level WindowRep is created.
   2) if parent != NULL: a lower level windowRep is created.
*/

#define MARGINS

#ifndef ViewWin_h
#define ViewWin_h

#include "DList.h"
#include "DeviseTypes.h"
#include "WindowRep.h"
#include "Color.h"

#ifdef TK_WINDOW
#include <tcl.h>
#include <tk.h>
#endif

class ViewWin;
DefinePtrDList(ViewWinList, ViewWin *);
class WindowRep;

class ViewWin: protected WindowRepCallback {
public:
	ViewWin(char *name, Color foreground = ForegroundColor,
		Color background = BackgroundColor, int weight = 1,
		Boolean boundary = true);

	/* iconify window, if top level. Not guaranteed to succeed */
	void Iconify();

	/* Insert ViewWin into a parent */
	void AppendToParent(ViewWin *parent);

	/* Delete ViewWin from parent */
	void DeleteFromParent();

	
	/* These are called by parents */
	virtual void Map(int x, int y, unsigned w, unsigned h);
	virtual void Unmap();
	virtual void Iconify(Boolean iconified)=0;

	Boolean Mapped();

	char *GetName(){ return _name; }
	
	virtual ~ViewWin();

	int GetWeight() { return _weight; }

#if defined(MARGINS) || defined(TK_WINDOW)
	/* Get size of margin controls */
	void GetMargins(unsigned int &lm, unsigned int &rm,
			unsigned int &tm, unsigned int &bm);
	/* Get real (non-margin-adjusted) geometry of window */
	void RealGeometry(int &x, int &y, unsigned &w, unsigned &h);
#endif

	/* Get current geometry of window */
	void Geometry(int &x, int &y, unsigned &w, unsigned &h);

	/* Get absolute origin of window, if mapped */
	void AbsoluteOrigin(int &x, int &y);

	/* Move/Resize self. Base class 
	creates a new WindowRep if not already there
	and then resizes it */
	virtual void MoveResize(int x, int y, unsigned w, unsigned h);

	/* Get the Window Rep of this View window */
	WindowRep *GetWindowRep(){ return _windowRep; }

	/* Append child. Base class just inserts the child.
	Derived class should implement functions to MoveResize() child.*/
	virtual void Append(ViewWin *child);

	/* Delete child */
	virtual void Delete(ViewWin *child);

	/* Replace child1 by child2. child1 must be a valid child.
	Base class only updates list of children.*/
	virtual void Replace(ViewWin *child1, ViewWin *child2);

	/* swap child1 and child2. Both must be valid. Base
	class updates only list of children. */
	virtual void SwapChildren(ViewWin *child1, ViewWin *child2);

	/* Clear parent without doing anything else. 
	This is to facilitate implementation of Replace() */
	void SetParent(ViewWin *win) { _parent = win; }

	/* Get the children */
	/* return # of children */
	int NumChildren() { return _children.Size(); };

	/* iterator for children */
	int InitIterator(Boolean backwards=false)
		{ return _children.InitIterator(backwards); }
	Boolean More(int index) { return _children.More(index); }
		virtual ViewWin *Next(int index) { return _children.Next(index);}
	void DoneIterator(int index) { _children.DoneIterator(index);};

	ViewWin *GetParent() { return _parent; }

	/* Return true if window is iconified */
	Boolean Iconified();

	Color GetBgColor() { return _background; }
	Color GetFgColor() { return _foreground; }

protected:
	/* called by base class when it has been mapped/unmapped */
	virtual void SubClassMapped()=0;
	virtual void SubClassUnmapped()=0;


	virtual void HandleResize(WindowRep * w, int xlow,
			int ylow, unsigned width, unsigned height);
	
	virtual void HandleWindowMappedInfo(WindowRep *winRep, Boolean mapped);

	/* Get total weight of children */
	int TotalWeight();

private:
	Boolean _iconified;
	Boolean _winBoundary;
	Boolean _hasGeometry; /* TRUE if we have the geometry */
	int _x, _y;
	unsigned int _width, _height; /* current geometry */
	ViewWinList _children;
	ViewWin *_parent;
	WindowRep *_windowRep;
	char *_name;
	int _weight;
	Boolean _mapped; /* TRUE if this window is mapped */
	Color _background, _foreground;

#ifdef MARGINS
	void DrawMargins();
#endif

#ifdef TK_WINDOW
	void AddMarginControls();
	void DropMarginControls();
	void DestroyMarginControl(char *side);
	void ReparentMarginControl(char *side, int xoff, int yoff);
	void ResizeMargins(unsigned int w, unsigned int h);
	void ToggleMargins();

	Boolean      _marginsOn;
	char         _tkPathName[32];
#endif

#if defined(MARGINS) || defined(TK_WINDOW)
	unsigned int _leftMargin;
	unsigned int _rightMargin;
	unsigned int _topMargin;
	unsigned int _bottomMargin;
#endif
};

#endif
