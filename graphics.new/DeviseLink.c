/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2000
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
  Revision 1.5  1999/03/01 23:09:09  wenger
  Fixed a number of memory leaks and removed unused code.

  Revision 1.4  1999/02/11 19:54:58  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.3.4.1  1999/02/11 18:24:21  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.3  1998/05/06 22:04:53  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.2  1998/04/29 17:53:51  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.1  1998/03/08 00:01:08  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

 */

#include <stdio.h>

#include "DeviseLink.h"
#include "ViewGraph.h"

//#define DEBUG

DeviseLink::DeviseLink(const char *name, VisualFlag linkFlag)
{
#if defined(DEBUG)
  printf("DeviseLink(0x%p)::DeviseLink(%s, %d)\n", this, name, linkFlag);
#endif

  _name = name;
  _linkAttrs = linkFlag;
  _viewList = new LinkViewList;//TEMP -- leaked

  View::InsertViewCallback(this);
  _objectValid.Set();
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

  delete _viewList;
}

/* insert view into link */

void DeviseLink::InsertView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  return _viewList->Find(view);
}

void DeviseLink::ViewDestroyed(View *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  DeleteView((ViewGraph *)view);
}

int DeviseLink::InitIterator()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  return _viewList->InitIterator();
}

Boolean DeviseLink::More(int index)
{ 
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  return _viewList->More(index);
}

ViewGraph *DeviseLink::Next(int index)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  return _viewList->Next(index);
}

void DeviseLink::DoneIterator(int index)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  _viewList->DoneIterator(index);
}

void DeviseLink::Print() {
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  printf("Name = %s\n", GetName());
  printf("Views in Link ->\n");
  int index = _viewList->InitIterator();
  while(_viewList->More(index)) {
     printf("   %s ", _viewList->Next(index)->GetName());
  }
  printf("\n");
  _viewList->DoneIterator(index);
}

Boolean
DeviseLink::IsPileLinkName(const char *name)
{
  const char *pileStr = "Pile: ";

  Boolean result = !strncmp(name, pileStr, strlen(pileStr));

  return result;
}

/*============================================================================*/
