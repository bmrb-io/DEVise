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

#ifndef _GROUPDIR_H_
#define _GROUPDIR_H_

#include "ItemList.h"

class GroupDir
{
public:
  ItemList *dir;

  GroupDir();
  ~GroupDir();

  void add_entry(Group *grp);
  void remove_entry(Group *grp);

  Group *get_entry(char *name);
  void top_level_items();
};

#endif
