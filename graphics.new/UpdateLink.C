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
  Revision 1.1  1996/08/04 21:59:56  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

*/

#include <unistd.h>
#include "DeviseTypes.h"
#include "UpdateLink.h"
#include "ViewGraph.h"

//#define DEBUG


UpdateLink::UpdateLink(char* name)
: VisualLink(name, 0),
  _master(NULL)
{
}


UpdateLink::~UpdateLink()
{
}


void UpdateLink::SetMasterView(ViewGraph* view)
{
#ifdef DEBUG
    printf("UpdateLink::SetMasterView[%p - %s]\n", view, view->GetName());
#endif
  DOASSERT(!_viewList->Find(view), "Master is already a slave of UpdateLink");
  _master = view;
}


void UpdateLink::RemoveMasterView(ViewGraph* view)
{
#ifdef DEBUG
    printf("UpdateLink::RemoveMasterView[%p - %s]\n", view, view->GetName());
#endif
  DOASSERT(view == _master, "Attempted to remove the wrong master");
  _master = NULL;
}


void UpdateLink::InsertView(ViewGraph *view)
{
#ifdef DEBUG
    printf("UpdateLink::InsertView[%p - %s]\n", view, view->GetName());
#endif
  DOASSERT(view != _master, "Cannot add master as a slave of UpdateLink");
  VisualLink::InsertView(view);
}


void UpdateLink::FilterChanged(View *view, VisualFilter &filter, int flushed)
{
  // ignore message
}


void UpdateLink::ViewRecomputed(View *view)
{ 
#ifdef DEBUG
    printf("UpdateLink::ViewRecomputed[%p - %s]\n", view, view->GetName());
#endif
  if( view == _master ) {
    int index;
    for(index = _viewList->InitIterator(); _viewList->More(index);) {
      ViewGraph* v = _viewList->Next(index);
      v->MasterRecomputed(_master);
    }
    _viewList->DoneIterator(index);
  }
}


bool UpdateLink::DeleteView(ViewGraph *view)
{ 
#ifdef DEBUG
    printf("UpdateLink::DeleteView[%p - %s]\n", view, view->GetName());
    printf("master = %p - %s\n", _master, _master->GetName());
#endif
    if( view == _master ) {
	_master = NULL;
	return true;
    }
    return VisualLink::DeleteView(view);
}    

void UpdateLink::Print() {
  VisualLink::Print();
  printf("master = %s\n", _master->GetName());
}

