/*
  $Id$

  $Log$*/

/*
   Selection.h: base class implements linksage in all directons
*/

#ifndef Selection_h
#define Selection_h
#include "View.h"
#include "VisualArg.h"

const int VisualFilterStackSize = 20; /* size of filter stack */

class View;
class SelectionCallback {
public:
	virtual void PushSelection(View *view, VisualFilter &filter,
	VisualFilter &filter, VisualFlag changeFlag)=0;
	virtual void PopSelection(View *view,
			VisualFilter &filter, VisualFlag changeFlag)=0;
	virtual void ChangeSelection(View *view,VisualFilter &filter,
	VisualFilter &filter, VisualFlag changeFlag)=0;
};
DefinePtrDList(SelectionCallbackList,SelectionCallback *)


class Selection;
DefinePtrDList(SelectionList, Selection *);

class Selection : public SelectionCallback {
public:
	/* constructor. flag sets the bits for those visual attributes
	to enforce */
	Selection(char *name, VisualFlag flag);

	virtual ~Selection();

	/* insert a view into a selection */
	virtual void InsertView(View *v);

	/* from SelectionCallback. changeFlag: which attributes of
	view to change.*/
	virtual void PushSelection(View *v, VisualFilter &filter, 
			VisualFilter &hint, VisualFlag changeFlag);
	virtual void ChangeSelection(View *v, VisualFilter &filter, 
			VisualFilter &hint, VisualFlag changeFlag);
	virtual void PopSelection(View *v, VisualFilter &hint,
		VisualFlag changeFlag);

	/* Get Current visual filter. Return true if it's available.
	Return false if no view is managed by the selection */
	Boolean CurrentFilter(VisualFilter &filter);

	/* Find selection with the given id or name */
	static Selection *FindSelectionById(int id);
	static Selection *FindSelectionByName(char *name);
	static int FindSelectionId(Selection *selection);


	int GetId() { return _id; }
	char *GetName() { return _name; };

private:

	void PushFilter(VisualFilter filter);
	Boolean PopFilter(VisualFilter &filter);

	/* getting views for the selection */
	int ViewListSize();
	int InitViewIterator();
	Boolean MoreView(int index);
	View *NextView(int index);
	void DoneViewIterator(int index);

	/* set visual filter for all views */
	void SetVisualFilters(View *view,Boolean hasHint, VisualFilter *hint,
		VisualFlag changeFlag);

	/* stack of visual filters */
	VisualFilter _filterStack[VisualFilterStackSize];
	int _numFilters; /* # of filters on the stack */

	VisualFilter _filter;	/* current filter */
	ViewList *_viewList;
	VisualFlag _flag;	/* flag to specify which visual attributes to link*/

	int _id;		/* ID number of this selection */
	char *_name;	/* name of this selection */
	static int _nextId;	/* next ID number for the selection */
	static SelectionList *_selections; /* list of selections */
};

/* selection on x and y */
class SelectionXY: public Selection{
public:
	SelectionXY(char *name): Selection(name,VISUAL_LOC){};
};

/* selectionon X only */
class SelectionX: public Selection{
public:
	SelectionX(char *name): Selection(name,VISUAL_X){};
};

#endif
