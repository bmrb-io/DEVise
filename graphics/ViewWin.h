/* A ViewWin is consist of two parts:
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
#ifndef ViewWin_h
#define ViewWin_h
#include "DList.h"
#include "DeviseTypes.h"
#include "WindowRep.h"
#include "Color.h"

class ViewWin;
DefinePtrDList(ViewWinList, ViewWin *);
class WindowRep;

class ViewWin: protected WindowRepCallback {
public:
	ViewWin(char *name, 
		Color foreground= BlackColor, Color background= WhiteColor,
			int weight=1, Boolean boundary = true);

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

	/* Get current geometry of child w. r. t. parent */
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
	Color getFgColor() { return _foreground; }

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
	int _x, _y; unsigned int _width, _height; /* current geometry */
	ViewWinList _children;
	ViewWin *_parent;
	WindowRep *_windowRep;
	char *_name;
	int _weight;
	Boolean _mapped; /* TRUE if this window is mapped */
	Color _background, _foreground;

};
#endif
