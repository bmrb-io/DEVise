/*
  $Id$

  $Log$*/

#ifndef ViewCallback_h
#define ViewCallback_h
#include "DeviseTypes.h"
#include "DList.h"

/* Report filter added for a view */
class View;
class ViewCallback {
public:
	/* Called by View when its visual filter has changed.
	flushed == index of 1st element in the history that has been flushed,
		or -1 if none. index 0 == oldest element .*/
	virtual void FilterChanged(View *view, VisualFilter &filter,
		int flushed) = 0;

	virtual void ViewCreated(View *view)=0;
	virtual void ViewDestroyed(View *view)=0;
};

DefinePtrDList(ViewCallbackList, ViewCallback *);


#endif
