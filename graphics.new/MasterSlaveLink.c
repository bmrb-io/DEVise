/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of MasterSlaveLink class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "MasterSlaveLink.h"
#include "ViewGraph.h"

//#define DEBUG

Boolean MasterSlaveLink::_disableUpdates = false;

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::MasterSlaveLink
 * Constructor.
 */
MasterSlaveLink::MasterSlaveLink(char *name, VisualFlag linkFlag) :
	DeviseLink(name, linkFlag)
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::MasterSlaveLink(%s, %d)\n", this, name,
      linkFlag);
#endif

  _masterView = NULL;
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::~MasterSlaveLink
 * Destructor.
 */
MasterSlaveLink::~MasterSlaveLink()
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::~MasterSlaveLink()\n", this);
#endif

  if (_masterView)
    _masterView->DropAsMasterView(this);

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
    view->DropAsSlaveView(this);
  }
  DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::SetMasterView
 * Makes the given view the master of this link.
 */
void
MasterSlaveLink::SetMasterView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::SetMasterView(%s)\n", this, view->GetName());
#endif

  if (_masterView) {
    _masterView->DropAsMasterView(this);
  }

  _masterView = view;

  if (_masterView) {
    _masterView->AddAsMasterView(this);
  }
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::InsertView
 * Inserts a slave view into the link.
 */
void
MasterSlaveLink::InsertView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::InsertView(%s)\n", this, view->GetName());
#endif

  DeviseLink::InsertView(view);
  view->AddAsSlaveView(this);
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::DeleteView
 * Deletes a view (master or slave) from the link.
 */
bool
MasterSlaveLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::DeleteView(%s)\n", this, view->GetName());
#endif

  if( view == _masterView ) {
      view->DropAsMasterView(this);
      _masterView = NULL;
  } else if( DeviseLink::DeleteView(view) ) {
      view->DropAsSlaveView(this);
  } else {
      // view was not part of this link
      return false;
  }
  return true;
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::Done
 * Propagate updates when insertions to link are done.
 */
void
MasterSlaveLink::Done()
{
#if defined(DEBUG)
  printf("MasterSlaveLink(0x%p)::Done()\n", this);
#endif

  /* Refresh the views of this link. */
  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);

    if (view->IsInPileMode()) {
      /* Refresh the whole pile of a piled view. */

      /* This code sometimes causes "unnecessary" redraws of a view if a view
       * below it in a pile has its record link updated (for example, if no
       * symbols were actually drawn in the lower view, we don't really have
       * to redraw the upper view).  However, it seems like an awful lot of
       * work to keep track of whether we really have to redraw the upper
       * view(s), so for now we refresh the top view in the pile, which
       * refreshes the whole pile. RKW 1/9/97. */
      View *slave = View::FindViewByName(view->GetFirstSibling()->GetName());
#ifdef DEBUG
      printf("Refreshing piled view %s\n", slave->GetName());
#endif
      slave->Refresh();
    } else {
#ifdef DEBUG
      printf("Refreshing slave view %s\n", view->GetName());
#endif
      view->Refresh();
    }
  }
  DoneIterator(index);
}

/*============================================================================*/
