/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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

  $Log$*/

#include <stdio.h>
#include "ItemList.h"

ItemList::ItemList()
{
  list = NULL;
  curr = NULL;
}

ItemList::~ItemList()
{
  GroupItem *nptr;
  GroupItem *ptr = list;

  while (ptr)
  {
    nptr = ptr->nxt;
    delete(ptr);
    ptr = nptr;
  }
  list = NULL;
}

void ItemList::add_entry(Item *itm)
{
  GroupItem *newitem = new(GroupItem);

  newitem->itm = itm;
  newitem->nxt = list;
  list = newitem;
}

void ItemList::remove_entry(Item *itm)
{
  GroupItem *tmptr;
  GroupItem *ptr = list;

  if (!list) return;
  if (list->itm == itm)
  {
    tmptr = list;
    list = list->nxt;
    delete tmptr;
    return;
  }

  while ((ptr->nxt) && (ptr->nxt->itm != itm))
    ptr = ptr->nxt;

  if (!ptr->nxt) return;

  tmptr = ptr->nxt;
  ptr->nxt = ptr->nxt->nxt;
  delete tmptr;
}

Item *ItemList::first_item()
{
  curr = list;
  
  if (!curr)
    return NULL;
  return curr->itm;
}

Item *ItemList::next_item()
{
  curr = curr->nxt;

  if (!curr)
    return NULL;
  return curr->itm;
}  
