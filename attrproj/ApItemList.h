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

  $Log$
  Revision 1.1  1996/04/22 18:01:46  wenger
  First version of "attribute projection" code.  The parser (with
  the exception of instantiating any TData) compiles and runs.

*/

#ifndef _APITEMLIST_H_
#define _APITEMLIST_H_

#include "ApGroup.h"

struct GroupItem
{
  Group *itm;
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
  
  void add_entry(Group *itm);
  void remove_entry(Group *itm);
  Group *first_item();
  Group *next_item();
};

#endif
