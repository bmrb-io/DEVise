/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  I've changed considerably how links, especially visual links, work.  There are two
  main changes:
  1. I've introduced the new DeviseLink class, which is now the superclass of all of
     the other types of links.  This avoids having RecordLinks have all of the filter-
     related stuff in VisualLinks, which they don't use.
  2. VisualLinks are now DispatcherCallbacks, and they store their own visual filter.
     This is done so that when one view controls several links, the filters for each
     link are set and locked before the links start updating other views.  This
     prevents the subsequent view updates from goofing up the links (see bugs 115,
     128, and 311.
  RKW Mar 7, 1998.
 */

/*
  $Id$

  $Log$
  Revision 1.9  1998/02/26 22:59:58  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.8  1997/09/05 22:36:33  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.7  1997/01/23 17:40:03  jussi
  Removed references to GetXMin().

  Revision 1.6  1996/12/03 15:20:04  wenger
  Conditionaled out some debug code.

  Revision 1.5  1996/11/26 16:51:43  ssl
  Added support for piled viws

  Revision 1.4  1996/09/27 21:09:39  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.3  1996/08/05 02:13:22  beyer
  HP's stdio.h has _flag #define'd so I #undef'd it in here.

  Revision 1.2  1996/08/04 21:59:58  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.1  1996/05/31 15:37:41  jussi
  Moved to the graphics.new directory.

  Revision 1.3  1995/12/29 18:28:58  jussi
  Added FilterAboutToChange() method to facilitate the new
  cursor mechanism. Also added the copyright message.

  Revision 1.2  1995/09/05 21:13:25  jussi
  Added/updated CVS header.
*/

#include "VisualLink.h"
#include "ViewGraph.h"
#include "Scheduler.h"

//#define DEBUG

VisualLink::VisualLink(char *name, VisualFlag linkFlag) :
  DeviseLink(name, linkFlag)
{
  _filterValid = false;
  _filterLocked = false;
  _originatingView = NULL;

  _dispID = Dispatcher::Current()->Register(this);
}

VisualLink::~VisualLink()
{
  Dispatcher::Current()->Unregister(this);
}

void VisualLink::InsertView(ViewGraph *view)
{
  if (_viewList->Find(view)) {
    /* view already inserted */
    return;
  }

  DeviseLink::InsertView(view);

  if (_viewList->Size() > 1) {
    DOASSERT(_filterValid, "Visual link's filter is invalid");
    _filterLocked = true;
    SetVisualFilter(view, _filter);
    _filterLocked = false;
  } else {
    view->GetVisualFilter(_filter);
    _filterValid = true;
  }
}

void VisualLink::SetFlag(VisualFlag flag)
{
  if (_filterLocked) {
    fprintf(stderr, "Illegal attempt to set link type with filter locked (link %s)\n",
	GetName());
  } else {
    _linkAttrs = flag;
  }
}


/* Called by View when its visual filter has changed.
   flushed == index if 1st element in the history that has been flushed.*/

void VisualLink::FilterChanged(View *view, VisualFilter &filter,
			       int flushed)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::FilterChanged(%s, ", GetName(), view->GetName());
  filter.Print();
  printf(")\n");
#endif

  // If the filter is locked, we don't do anything here.
  if (_filterLocked) {
#if defined(DEBUG)
    printf("filter for visual link %s is locked\n", GetName());
#endif
  } else {
  /* first, make sure that this view is under our control */
    Boolean found = ViewInLink((ViewGraph *)view);
  
    if (found) {
      // Now find out if the new filter is different from the current filter.
      Boolean filterDifferent = !_filterValid;

      // Note: we'll have to add stuff here if we implement filter attributes other than
      // X and Y.
      if (!filterDifferent) {
	if ((_linkAttrs & VISUAL_X) &&
	    (filter.xLow != _filter.xLow || filter.xHigh != filter.xHigh)) {
	  filterDifferent = true;
	}
      }
      if (!filterDifferent) {
	if ((_linkAttrs & VISUAL_Y) &&
	    (filter.yLow != _filter.yLow || filter.yHigh != _filter.yHigh)) {
	  filterDifferent = true;
	}
      }

      if (filterDifferent) {
#if defined(DEBUG)
        printf("changing filter for visual link %s\n", GetName());
#endif
	_filter = filter;
	_filterValid = true;
	_filterLocked = true;
	_originatingView = view;
	Scheduler::Current()->RequestCallback(_dispID);
      } else {
#if defined(DEBUG)
        printf("new filter for visual link %s is same as old filter\n", GetName());
#endif
      }
    }
  }
}


void VisualLink::Run()
{
#if defined(DEBUG)
  printf("VisualLink(%s)::Run()\n", GetName());
#endif

  int index;
  for(index = InitIterator(); More(index);) {
    View *viewInList = Next(index);
    if (viewInList!= _originatingView) {
      /* Change this view */
      SetVisualFilter(viewInList, _filter);
    }
  }
  DoneIterator(index);

  _filterLocked = false;
}


/* update visual filters for view views. */

void VisualLink::SetVisualFilter(View *view, VisualFilter &filter)
{
#if defined(DEBUG)
  printf("VisualLink ->%s setVF view: %s\n", GetName(), view->GetName());
#endif

  VisualFilter tempFilter;
  view->GetVisualFilter(tempFilter);
  VisualFlag testFlag;
  testFlag = _linkAttrs;
  Boolean change = false;
  
  if ((testFlag & VISUAL_X) && (tempFilter.xLow != filter.xLow 
				|| tempFilter.xHigh != filter.xHigh)) {
    tempFilter.xLow = filter.xLow;
    tempFilter.xHigh = filter.xHigh;
    if (tempFilter.xHigh <= tempFilter.xLow)
      tempFilter.xHigh = tempFilter.xLow + 1;
    change = true;
  }

  if ((testFlag & VISUAL_Y) && (tempFilter.yLow != filter.yLow 
				|| tempFilter.yHigh != filter.yHigh)) {
    tempFilter.yLow = filter.yLow;
    tempFilter.yHigh = filter.yHigh;
    change = true;
  }

#if 0 // Not currently used.  RKW Feb. 25, 1998.
  if ((testFlag & VISUAL_COLOR) && 
      (tempFilter.colorLow != filter.colorLow 
       || tempFilter.colorHigh != filter.colorHigh)) {
    tempFilter.colorLow = filter.colorLow;
    tempFilter.colorHigh = filter.colorHigh;
    change = true;
  }

  if ((testFlag & VISUAL_SIZE) && 
      (tempFilter.sizeLow != filter.sizeLow 
       || tempFilter.sizeHigh != filter.sizeHigh)) {
    tempFilter.sizeLow = filter.sizeLow;
    tempFilter.sizeHigh = filter.sizeHigh;
    change = true;
  }

  if ((testFlag & VISUAL_PATTERN) &&
     (tempFilter.patternLow != filter.patternLow 
      || tempFilter.patternHigh != filter.patternHigh)) {
    tempFilter.patternLow = filter.patternLow;
    tempFilter.patternHigh = filter.patternHigh;
    change = true;
  }

  if ((testFlag & VISUAL_ORIENTATION) && 
      (tempFilter.orientationLow != filter.orientationLow 
       || tempFilter.orientationHigh != filter.orientationHigh)) {
    tempFilter.orientationLow = filter.orientationLow;
    tempFilter.orientationHigh = filter.orientationHigh;
    change = true;
  }

  if ((testFlag & VISUAL_SHAPE) && 
      (tempFilter.shapeLow != filter.shapeLow 
       || tempFilter.shapeHigh != filter.shapeHigh)) {
    tempFilter.shapeLow = filter.shapeLow;
    tempFilter.shapeHigh = filter.shapeHigh;
    change = true;
  }
#endif

  if (change) {
    view->SetVisualFilter(tempFilter, false);
  }
}


void VisualLink::Print() {
  printf("Name = %s, Flag = %d\n", GetName(), GetFlag());
  DeviseLink::Print();
}
