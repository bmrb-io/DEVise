/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.4  1996/05/07 16:41:49  jussi
  Updated constructor and added HandleKey, HandlePress and HandlePopup
  to reflect new interface between View and its derived classes.

  Revision 1.3  1996/04/05 20:13:34  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.2  1995/09/05 22:16:19  jussi
  Added CVS header.
*/

#ifndef ViewRanges_h
#define ViewRanges_h

#include "View.h"
#include "RangeCallback.h"

class ViewRanges: public View, private RangeCallback {
public:
  /*TRUE if draw horizontal line, else draw vertical line */
  ViewRanges(char *name,  ViewWin *parent, VisualFilter &initFilter,
	     RangeSource *source, Boolean horizontal);

  ~ViewRanges();

private:
  /* from View */
  virtual void DerivedStartQuery(VisualFilter &filter, int timestamp);
  virtual void DerivedAbortQuery();
  virtual void HandlePress(WindowRep * w, int xlow, int ylow,
			   int xhigh, int yhigh, int button) {}
  virtual void HandleKey(WindowRep *w ,int key, int x, int y) {}
  virtual Boolean HandlePopUp(WindowRep *, int x, int y, int button,
			      char **&msgs, int &numMsgs) {
    return false;
  }

  /* from RangeCallback */
  virtual void RangeInserted(long low, long high);
  virtual void RangeDeleted(long low, long high);

  RangeSource *_source; /* source of ranges */
  Boolean _horizontal;
};

#endif
