/*
  $Id$

  $Log$*/

#ifndef VisualLink_h
#define VisualLink_h
#include "DList.h"
#include "Dispatcher.h"
#include "View.h"

DefinePtrDList(LinkViewList,View *)

class VisualLink: public ViewCallback, private DispatcherCallback {
public:
	VisualLink(char *name, VisualFlag linkFlag);
	virtual ~VisualLink();

	/* change visual flag */
	void ChangeFlag(VisualFlag flag) { _flag = flag; }

	/* Return current flag */
	VisualFlag GetFlag() { return _flag;}

	/* insert view into visual link */
	void InsertView(View *view);

	/* delete view from visual link */
	void DeleteView(View *view);

	/* Return TRUE if view is part of this link */
	Boolean ViewInLink(View *view);	

	/* from ViewCallback */
	/* Called by View when its visual filter has changed.
	flushed == TRUE if 1st element in the history has been flushed.*/
	virtual void FilterChanged(View *view, VisualFilter &filter,
				int flushed);

	virtual void ViewCreated(View *view){};
	virtual void ViewDestroyed(View *view){};

	/*iterator for all views */
	int InitIterator();
	Boolean More(int);
	View *Next(int);
	void DoneIterator(int);

private:
	void ProcessFilterChanged(View *view, VisualFilter &filter);

    virtual char *DispatchedName(){ return _name; }
	virtual void Run();

	/* Find the xMin coord */
	Boolean HasXMin(Coord &xMin);

	void SetVisualFilter(View *view,  VisualFilter &filter);
	LinkViewList *_viewList;
	Boolean _updating; /* TRUE if we are currently updating view */
	char *_name;
	VisualFlag _flag;

	/* view queued up to be updated */
	View *_view;
	VisualFilter _filter;
};
#endif
