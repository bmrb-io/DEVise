/*
  $Id$

  $Log$*/

#ifndef ViewRanges_h
#define ViewRanges_h
#include "View.h"
#include "RangeCallback.h"

class ViewRanges: public View, private RangeCallback{
public:
	/*TRUE if draw horizontal line, else draw vertical line */
	ViewRanges(char *name,  ViewWin *parent, VisualFilter &initFilter,
		RangeSource *source, Boolean horizontal);
private:
	/* from View */
    virtual void DerivedStartQuery(VisualFilter &filter, int timestamp);
	virtual void DerivedAbortQuery();

	/* from RangeCallback */
	virtual void RangeInserted(long low, long high);
	virtual void RangeDeleted(long low, long high);

	RangeSource *_source; /* source of ranges */
	Boolean _horizontal;
};

#endif
