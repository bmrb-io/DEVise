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
  Revision 1.5  1996/08/04 21:04:52  beyer
  Needed a forward declaration

  Revision 1.4  1996/07/13 17:28:14  jussi
  Added ViewRecomputed() callback interface.

  Revision 1.3  1995/12/29 18:28:18  jussi
  Added FilterAboutToChange() method to facilitate the new
  cursor mechanism.

  Revision 1.2  1995/09/05 21:13:18  jussi
  Added/updated CVS header.
*/

#ifndef ViewCallback_h
#define ViewCallback_h

class View;
class VisualFilter;


class ViewCallback {
public:
  /* Called by View when view is created. */
  virtual void ViewCreated(View *view) {}

  /* Called by View when its visual filter is about to be changed. */
  virtual void FilterAboutToChange(View *view) {}

  /* Called by View when its visual filter has changed.
     flushed == index of 1st element in the history that has been flushed,
     or -1 if none. index 0 == oldest element .*/
  virtual void FilterChanged(View *view, const VisualFilter &filter,
      int flushed) {}
  
  /* Called by View when view is recomputed (statistics have changed). */
  virtual void ViewRecomputed(View *view) {}

  /* Called by View when view is destroyed. */
  virtual void ViewDestroyed(View *view) {}
};

DefinePtrDList(ViewCallbackList, ViewCallback *);

#endif
