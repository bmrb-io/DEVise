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
  Revision 1.1  1998/03/08 00:01:08  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

 */

#include <stdio.h>

#include "DeviseLink.h"
#include "ViewGraph.h"

//#define DEBUG

DeviseLink::DeviseLink(char *name, VisualFlag linkFlag)
{
#if defined(DEBUG)
  printf("DeviseLink(0x%p)::DeviseLink(%s, %d)\n", this, name, linkFlag);
#endif

  _objectValid = false;
  _name = name;
  _linkAttrs = linkFlag;
  _viewList = new LinkViewList;//TEMP -- leaked

  View::InsertViewCallback(this);
  _objectValid = true;
}

DeviseLink::~DeviseLink()
{
#if defined(DEBUG)
  printf("DeviseLink(0x%p)::~DeviseLink(%s)\n", this, _name);
#endif

  View::DeleteViewCallback(this);

  int index = InitIterator();
  while(More(index)) {
    View *view = Next(index);
    _viewList->DeleteCurrent(index);
  }
  DoneIterator(index);
  _objectValid = false;
}

/* insert view into link */

void DeviseLink::InsertView(ViewGraph *view)
{
  DOASSERT(_objectValid, "operation on invalid object");
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
  DOASSERT(_objectValid, "operation on invalid object");
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
  DOASSERT(_objectValid, "operation on invalid object");
  return _viewList->Find(view);
}

void DeviseLink::ViewDestroyed(View *view)
{
  DOASSERT(_objectValid, "operation on invalid object");
  DeleteView((ViewGraph *)view);
}

int DeviseLink::InitIterator()
{
  DOASSERT(_objectValid, "operation on invalid object");
  return _viewList->InitIterator();
}

Boolean DeviseLink::More(int index)
{ 
  DOASSERT(_objectValid, "operation on invalid object");
  return _viewList->More(index);
}

ViewGraph *DeviseLink::Next(int index)
{
  DOASSERT(_objectValid, "operation on invalid object");
  return _viewList->Next(index);
}

void DeviseLink::DoneIterator(int index)
{
  DOASSERT(_objectValid, "operation on invalid object");
  _viewList->DoneIterator(index);
}

void DeviseLink::Print() {
  DOASSERT(_objectValid, "operation on invalid object");
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
