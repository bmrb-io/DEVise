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

#ifndef _ITEMLIST_H_
#define _ITEMLIST_H_

#include "Group.h"

struct GroupItem
{
  Item *itm;
  GroupItem *nxt;
};
typedef GroupItem GroupItem;
  
class ItemList
{
public:
  GroupItem *list;
  GroupItem *curr;

  ItemList();
  ~ItemList();
  
  void add_entry(Item *itm);
  void remove_entry(Item *itm);
  Item *first_item();
  Item *next_item();
};

#endif
