/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.12  1999/01/06 21:25:10  wenger
  Fixed Condor2.ds redraw problem (a problem with the VisualLink class);
  also added some debug code and code to make sure view filter histories
  are consistent.

  Revision 1.11  1998/03/18 08:20:18  zhenhai
  Added visual links between 3D graphics.

  Revision 1.10  1998/03/08 00:01:16  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

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
#if defined(DEBUG)
  printf("VisualLink(0x%p)::VisualLink(%s, %d)\n", this, name, linkFlag);
#endif

  _filterValid = false;
  _filterLocked = false;
  _originatingView = NULL;

  _dispID = Dispatcher::Current()->Register(this);
}

VisualLink::~VisualLink()
{
#if defined(DEBUG)
  printf("VisualLink(%s)::~VisualLink()\n", GetName());
#endif

  int index = _viewList->InitIterator();
  while (_viewList->More(index)) {
    ViewGraph *view = _viewList->Next(index);
	view->DeleteVisualLink(this);
  }
  _viewList->DoneIterator(index);

  Dispatcher::Current()->Unregister(this);
}

void VisualLink::InsertView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::InsertView(%s)\n", GetName(), view->GetName());
#endif

  if (_viewList->Find(view)) {
    /* view already inserted */
    return;
  }

  DeviseLink::InsertView(view);
  view->AddVisualLink(this);

  if (_viewList->Size() > 1) {
    DOASSERT(_filterValid, "Visual link's filter is invalid");
    _filterLocked = true;
    SetVisualFilter(view);
    _filterLocked = false;
  } else {
    view->GetVisualFilter(_filter);
#if defined(DEBUG)
    printf("  visual filter of link <%s> initialized\n", GetName());
    printf("    filter: %d, (%g, %g), (%g, %g)\n", _filter.flag,
      _filter.xLow, _filter.yLow, _filter.xHigh, _filter.yHigh);
#endif
    _filterValid = true;
  }
}

bool VisualLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::DeleteView(%s)\n", GetName(), view->GetName());
#endif

  bool result = DeviseLink::DeleteView(view);

  if (result) {
    view->DeleteVisualLink(this);
  }

  return result;
}

void VisualLink::SetFlag(VisualFlag flag)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::SetFlag(%d)\n", GetName(), flag);
#endif

  if (_filterLocked) {
    fprintf(stderr, "Illegal attempt to set link type with filter locked (link %s)\n",
	GetName());
  } else {
    _linkAttrs = flag;
  }
}


/* Called by View when its visual filter has changed.
   flushed == index if 1st element in the history that has been flushed.*/
// Note: the list of ViewCallbacks is global to all views, so this method
// gets called any time *any* view's visual filter gets changed, even if
// that view isn't even linked!  RKW 1999-04-05.

void VisualLink::FilterChanged(View *view, VisualFilter &newFilter,
			       int flushed)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::FilterChanged(%s)\n", GetName(), view->GetName());
  printf("  newFilter: %d, (%g, %g), (%g, %g)\n", newFilter.flag,
    newFilter.xLow, newFilter.yLow, newFilter.xHigh, newFilter.yHigh);
#endif

  // If the filter is locked, we don't do anything here.
  if (_filterLocked) {
#if defined(DEBUG)
    printf("  filter for visual link %s is locked\n", GetName());
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
	    (newFilter.xLow != _filter.xLow || newFilter.xHigh != _filter.xHigh)) {
	  filterDifferent = true;
	}
      }
      if (!filterDifferent) {
	if ((_linkAttrs & VISUAL_Y) &&
	    (newFilter.yLow != _filter.yLow || newFilter.yHigh != _filter.yHigh)) {
	  filterDifferent = true;
	}
      }
      if (!filterDifferent) {
        if ((_linkAttrs & VISUAL_ORIENTATION) &&
             !(newFilter.camera == _filter.camera))
         filterDifferent = true;
        }


      if (filterDifferent) {
#if defined(DEBUG)
        printf("  changing filter for visual link %s\n", GetName());
#endif
	_filter = newFilter;
	_filterValid = true;
	_filterLocked = true;
	_originatingView = view;
	Scheduler::Current()->RequestCallback(_dispID);
      } else {
#if defined(DEBUG)
        printf("  new filter for visual link %s is same as old filter\n", GetName());
#endif
      }
    } else {
#if defined(DEBUG)
        printf("  view <%s> is not part of this link\n", view->GetName());
#endif
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
      SetVisualFilter(viewInList);
    }
  }
  DoneIterator(index);

  _filterLocked = false;
}


void
VisualLink::GoHome(ViewGraph *view)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::GoHome(%s)\n", GetName(), view->GetName());
#endif

  DOASSERT(view->GetNumDimensions() == 2,
      "VisualLink::GoHome() only works on 2D views");

  // Get home of each view in the link, save the extremes.
  double xMin = DBL_MAX;
  double yMin = DBL_MAX;
  double xMax = DBL_MIN;
  double yMax = DBL_MIN;

  int index = _viewList->InitIterator();
  while (_viewList->More(index)) {
    ViewGraph *tmpView = (ViewGraph *)_viewList->Next(index);
    if (view->GetNumDimensions() == 2) {
      VisualFilter filter;
      tmpView->GetHome2D(filter);
      if (_linkAttrs & VISUAL_X || tmpView == view) {
        xMin = MIN(xMin, filter.xLow);
        xMax = MAX(xMax, filter.xHigh);
      }
      if (_linkAttrs & VISUAL_Y || tmpView == view) {
        yMin = MIN(yMin, filter.yLow);
        yMax = MAX(yMax, filter.yHigh);
      }
    }
  }
  _viewList->DoneIterator(index);
#if (DEBUG >= 4)
    printf("Link filter: (%g, %g), (%g, %g)\n", xMin, yMin, xMax, yMax);
#endif

  // Set the visual filter of the view that called us (it doesn't really
  // matter which view in the link we set the filter for, because of the
  // link).
  VisualFilter filter;
  view->GetVisualFilter(filter);
  filter.xLow = xMin;
  filter.yLow = yMin;
  filter.xHigh = xMax;
  filter.yHigh = yMax;
  view->SetVisualFilter(filter);
}


/* update visual filters for view views. */

void VisualLink::SetVisualFilter(View *view)
{
#if defined(DEBUG)
  printf("VisualLink(%s)::SetVisualFilter(%s)\n", GetName(), view->GetName());
  printf("  _filter: %d, (%g, %g), (%g, %g)\n", _filter.flag,
    _filter.xLow, _filter.yLow, _filter.xHigh, _filter.yHigh);
#endif

  VisualFilter tempFilter;
  view->GetVisualFilter(tempFilter);
  VisualFlag testFlag;
  testFlag = _linkAttrs;
  Boolean change = false;
  
  if ((testFlag & VISUAL_X) && (tempFilter.xLow != _filter.xLow 
				|| tempFilter.xHigh != _filter.xHigh)) {
    tempFilter.xLow = _filter.xLow;
    tempFilter.xHigh = _filter.xHigh;
    if (tempFilter.xHigh <= tempFilter.xLow)
      tempFilter.xHigh = tempFilter.xLow + 1;
    change = true;
  }

  if ((testFlag & VISUAL_Y) && (tempFilter.yLow != _filter.yLow 
				|| tempFilter.yHigh != _filter.yHigh)) {
    tempFilter.yLow = _filter.yLow;
    tempFilter.yHigh = _filter.yHigh;
    change = true;
  }

#if 0 // Not currently used.  RKW Feb. 25, 1998.
  if ((testFlag & VISUAL_COLOR) && 
      (tempFilter.colorLow != _filter.colorLow 
       || tempFilter.colorHigh != _filter.colorHigh)) {
    tempFilter.colorLow = _filter.colorLow;
    tempFilter.colorHigh = _filter.colorHigh;
    change = true;
  }

  if ((testFlag & VISUAL_SIZE) && 
      (tempFilter.sizeLow != _filter.sizeLow 
       || tempFilter.sizeHigh != _filter.sizeHigh)) {
    tempFilter.sizeLow = _filter.sizeLow;
    tempFilter.sizeHigh = _filter.sizeHigh;
    change = true;
  }

  if ((testFlag & VISUAL_PATTERN) &&
     (tempFilter.patternLow != _filter.patternLow 
      || tempFilter.patternHigh != _filter.patternHigh)) {
    tempFilter.patternLow = _filter.patternLow;
    tempFilter.patternHigh = _filter.patternHigh;
    change = true;
  }

  if ((testFlag & VISUAL_ORIENTATION) && 
      (tempFilter.orientationLow != _filter.orientationLow 
       || tempFilter.orientationHigh != _filter.orientationHigh)) {
    tempFilter.orientationLow = _filter.orientationLow;
    tempFilter.orientationHigh = _filter.orientationHigh;
    change = true;
  }

  if ((testFlag & VISUAL_SHAPE) && 
      (tempFilter.shapeLow != _filter.shapeLow 
       || tempFilter.shapeHigh != _filter.shapeHigh)) {
    tempFilter.shapeLow = _filter.shapeLow;
    tempFilter.shapeHigh = _filter.shapeHigh;
    change = true;
  }
#endif

  if ((testFlag & VISUAL_ORIENTATION) &&
      !(tempFilter.camera == _filter.camera)) {
     tempFilter.camera = _filter.camera;
     change = true;
  }

  if (change) {
    view->SetVisualFilter(tempFilter, false);
  }
}


void VisualLink::Print() {
  printf("Name = %s, Flag = %d\n", GetName(), GetFlag());
  DeviseLink::Print();
}
