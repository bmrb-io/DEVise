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
  Revision 1.3  1998/07/30 15:31:21  wenger
  Fixed bug 381 (problem with setting master and slave of a link to the same
  view); generally cleaned up some of the master-slave link related code.

  Revision 1.2  1998/04/14 21:03:14  wenger
  TData attribute links (aka set links) are working except for actually
  creating the join table, and some cleanup when unlinking, etc.

  Revision 1.1  1998/04/10 18:29:25  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

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
  printf("MasterSlaveLink(%s)::~MasterSlaveLink()\n", _name);
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
  printf("MasterSlaveLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  if (_masterView != view) {

    // Unlink the existing master view, if there is one.
    if (_masterView) {
      _masterView->DropAsMasterView(this);
      _masterView = NULL;
      Initialize();
    }

    // Make sure view isn't a slave, too.
    if (view != NULL) {
      (void) DeleteView(view);
    }

    // Link the new master view.
    _masterView = view;
    if (_masterView) {
      _masterView->AddAsMasterView(this);
    }
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
  printf("MasterSlaveLink(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  if (view == _masterView) {
    SetMasterView(NULL);
  }
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
  printf("MasterSlaveLink(%s)::DeleteView(%s)\n", _name, view != NULL ?
      view->GetName() : "NULL");
#endif

  if (view == NULL) {
      return false;
  } else {
      if (view == _masterView) {
	  SetMasterView(NULL);
          return true;
      } else if (DeviseLink::DeleteView(view)) {
          view->DropAsSlaveView(this);
          return true;
      } else {
          // view was not part of this link
          return false;
      }
  }
}

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::Done
 * Propagate updates when insertions to link are done.
 */
void
MasterSlaveLink::Done()
{
#if defined(DEBUG)
  printf("MasterSlaveLink(%s)::Done()\n", _name);
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
       * view(s), so for now we refresh the bottom view in the pile, which
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

/*------------------------------------------------------------------------------
 * function: MasterSlaveLink::Print
 * Print the views of this link.
 */
void
MasterSlaveLink::Print()
{
  DeviseLink::Print();
  char *masterName = NULL;
  if (_masterView != NULL) _masterView->GetName();
  if (masterName == NULL) masterName = "NULL";
  printf("Master view = %s\n", masterName);
}

/*============================================================================*/
