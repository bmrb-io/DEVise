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
  Revision 1.2  1995/09/05 21:13:18  jussi
  Added/updated CVS header.
*/

#ifndef ViewCallback_h
#define ViewCallback_h

#include "DeviseTypes.h"
#include "DList.h"

class View;

class ViewCallback {
public:
  /* Called by View when its visual filter is about to be changed. */
  virtual void FilterAboutToChange(View *view) = 0;

  /* Called by View when its visual filter has changed.
     flushed == index of 1st element in the history that has been flushed,
     or -1 if none. index 0 == oldest element .*/
  virtual void FilterChanged(View *view, VisualFilter &filter,
			     int flushed) = 0;
  
  /* Called by View when view is created. */
  virtual void ViewCreated(View *view) = 0;

  /* Called by View when view is destroyed */
  virtual void ViewDestroyed(View *view) = 0;
};

DefinePtrDList(ViewCallbackList, ViewCallback *);

#endif
