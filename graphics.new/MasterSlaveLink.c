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
  Revision 1.5  1998/11/11 14:31:00  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.4  1998/10/20 19:46:17  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

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
#include "MappingInterp.h"

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
 * function: MasterSlaveLink::ClearHighlightViews
 * Redraw highlight views with background view data color to "erase"
 * highlight view symbols.
 */
void
MasterSlaveLink::ClearHighlightViews()
{
#if defined(DEBUG)
  printf("MasterSlaveLink(%s)::ClearHighlightViews()\n", _name);
#endif

  // Note: the criteria for a view being acceptable as a highlight view
  // are as follows:
  // - The highlight view must be piled on top of a background view.
  //   Note that there can be only one highlight view in a pile.
  // - The records drawn in the highlight view must be a subset of the
  //   records drawn in the background view.
  // - The symbols of the highlight view and the backgrounds view must
  //   be of the same type.
  // - The symbols of the highlight view and the background view must be
  //   drawn at the same place.
  // - The symbols of the highlight view must be no larger than the symbols
  //   of the background view.
  // - The highlight view must be the slave of a record link or set link.
  // - Relatively few symbols should be drawn in the highlight view.
  // At this time (1998-11-10) whether a view meets these criteria must
  // be determined by the user.

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
    if (view->IsHighlight() && view->IsInPileMode()) {
      MappingInterp *map = (MappingInterp *)view->GetFirstMap();

      ViewGraph *bgView = (ViewGraph *)view->GetFirstSibling();
      MappingInterp *bgMap = (MappingInterp *)bgView->GetFirstMap();

      //
      // Get current info from mapping.
      //
      VisualFlag *dimensionInfo;
      int numDimensions = map->DimensionInfo(dimensionInfo);
      unsigned long cmdFlag, attrFlag;
      MappingInterpCmd *cmd = map->GetCmd(cmdFlag, attrFlag);

      //
      // Make a new mapping command with the color set to the background
      // view's data color.
      //
      MappingInterpCmd tmpCmd = *cmd;
      tmpCmd.colorCmd = bgMap->GetMappingCmd()->colorCmd;

      //
      // Change the mapping and re-draw this view.
      //
      map->ChangeCmd(&tmpCmd, cmdFlag, attrFlag, dimensionInfo, numDimensions);
      view->Refresh(false);
      // WaitForQueries here seems a little dangerous, but doing something
      // else would *really* get complicated. RKW 1998-11-10.
      Dispatcher::Current()->WaitForQueries();

      //
      // Put the mapping back to the way it was.
      //
      map->ChangeCmd(cmd, cmdFlag, attrFlag, dimensionInfo, numDimensions);
    }
  }
  DoneIterator(index);
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
#ifdef DEBUG
    printf("Refreshing slave view %s\n", view->GetName());
#endif
    view->Refresh();
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
