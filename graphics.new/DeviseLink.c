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
  Implementation of DeviseLink class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "DeviseLink.h"
#include "ViewGraph.h"

//#define DEBUG

DeviseLink::DeviseLink(char *name, VisualFlag linkFlag)
{
  _name = name;
  _linkAttrs = linkFlag;
  _viewList = new LinkViewList;//TEMPTEMP -- leaked

  View::InsertViewCallback(this);
}

DeviseLink::~DeviseLink()
{
  View::DeleteViewCallback(this);

  int index = InitIterator();
  while(More(index)) {
    View *view = Next(index);
    _viewList->DeleteCurrent(index);
  }
  DoneIterator(index);
}

/* insert view into link */

void DeviseLink::InsertView(ViewGraph *view)
{
  if (_viewList->Find(view))
    /* view already inserted */
    return;

#if defined(DEBUG)
  printf("Adding view %s to link %s\n", view->GetName(), GetName());
#endif

  _viewList->Append(view);
}

/* delete view from visual link */

bool DeviseLink::DeleteView(ViewGraph *view)
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

/* return true if view belongs to this link */

Boolean DeviseLink::ViewInLink(ViewGraph *view)
{
  return _viewList->Find(view);
}

void DeviseLink::ViewDestroyed(View *view)
{
    DeleteView((ViewGraph *)view);
}

int DeviseLink::InitIterator()
{
  return _viewList->InitIterator();
}

Boolean DeviseLink::More(int index)
{
  return _viewList->More(index);
}

ViewGraph *DeviseLink::Next(int index)
{
  return _viewList->Next(index);
}

void DeviseLink::DoneIterator(int index)
{
  _viewList->DoneIterator(index);
}

void DeviseLink::Print() {
  printf("Name = %s\n", GetName());
  printf("Views in Link ->\n");
  int index = _viewList->InitIterator();
  while(_viewList->More(index)) {
     printf("   %s ", _viewList->Next(index)->GetName());
  }
  printf("\n");
  _viewList->DoneIterator(index);
}

/*============================================================================*/
