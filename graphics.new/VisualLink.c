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

// hp's stdio.h has _flag #define'd
#ifdef _flag
#undef _flag
#endif


//#define DEBUG


VisualLink::VisualLink(char *name, VisualFlag linkFlag)
{
  _name = name;
  _flag = linkFlag;
  _viewList = new LinkViewList;//TEMPTEMP -- leaked
  _updating = false; 

  View::InsertViewCallback(this);
}

VisualLink::~VisualLink()
{
  View::DeleteViewCallback(this);

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
    _viewList->DeleteCurrent(index);
  }
  DoneIterator(index);
}

/* return true if view belongs to this link */

Boolean VisualLink::ViewInLink(ViewGraph *view)
{
  return _viewList->Find(view);
}

/* insert view into visual link */

void VisualLink::InsertView(ViewGraph *view)
{
  if (_viewList->Find(view))
    /* view already inserted */
    return;

#ifdef DEBUG
  printf("Adding view %s to link %s\n", view->GetName(), GetName());
#endif

  _viewList->Append(view);

  if (_viewList->Size() > 1) {
    View *firstView = _viewList->GetFirst();
    VisualFilter *filter = firstView->GetVisualFilter();
    SetVisualFilter(view, *filter);
  }
}

/* delete view from visual link */

bool VisualLink::DeleteView(ViewGraph *view)
{
  if (!_viewList->Find(view)) {
      /* view not in list */
      return false;
  }
#ifdef DEBUG
  printf("Removing view %s from link %s\n", view->GetName(), GetName());
#endif
  _viewList->Delete(view);
  return true;
}

/* Called by View when its visual filter has changed.
   flushed == index if 1st element in the history that has been flushed.*/

void VisualLink::FilterChanged(View *view, VisualFilter &filter,
			       int flushed)
{
  /* first, make sure that this view is under our control */
  Boolean found = false;
  int index;
  for(index = _viewList->InitIterator(); _viewList->More(index);) {
    View *viewInList = _viewList->Next(index);
    if (viewInList == view) {
      found = true;
      break;
    }
  }
  _viewList->DoneIterator(index);
  
  if (found)
    ProcessFilterChanged(view, filter);
}


void VisualLink::ViewDestroyed(View *view)
{
    DeleteView((ViewGraph *)view);
}


void VisualLink::ProcessFilterChanged(View *view, VisualFilter &filter)
{
  /* We need to do this because once a visual link starts changing
     the filter of its views, this function can be called subsequently by
     those views, and we need to ignore all those calls because we have
     already changed those filters. */

  if (_updating) {
#ifdef DEBUG
    printf("ignored because updating\n");
#endif
    return;
  }
	
  int index;
  for(index = _viewList->InitIterator(); _viewList->More(index);) {
    View *viewInList = _viewList->Next(index);
    if (viewInList!= view) {
      /* Change this view */
      SetVisualFilter(viewInList, filter);
    }
  }
  _viewList->DoneIterator(index);
}

/* update visual filters for view views. */

void VisualLink::SetVisualFilter(View *view, VisualFilter &filter)
{
#if defined(DEBUG)
  printf("VisualLink ->%s setVF view: %s\n", GetName(), view->GetName());
#endif
  _updating = true;

  VisualFilter tempFilter;
  view->GetVisualFilter(tempFilter);
  VisualFlag testFlag;
  testFlag = _flag;
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
  
  if (change)
    view->SetVisualFilter(tempFilter);
    
  _updating = false;
}

int VisualLink::InitIterator()
{
  return _viewList->InitIterator();
}

Boolean VisualLink::More(int index)
{
  return _viewList->More(index);
}

ViewGraph *VisualLink::Next(int index)
{
  return _viewList->Next(index);
}

void VisualLink::DoneIterator(int index)
{
  _viewList->DoneIterator(index);
}

void VisualLink::Print() {
  printf("Name = %s, Flag = %d\n", GetName(),GetFlag() );
  printf("Views in Link ->\n");
  int index = _viewList->InitIterator();
  while(_viewList->More(index)) {
     printf("   %s ", _viewList->Next(index)->GetName());
  }
  printf("\n");
  _viewList->DoneIterator(index);
}













