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

#include <iostream.h>
#include "GroupDir.h"

GroupDir::GroupDir()
{
  dir = new(ItemList);
}

GroupDir::~GroupDir()
{
  delete dir;
}

void GroupDir::add_entry(Group *grp)
{
  dir->add_entry((Item *)grp);
}

void GroupDir::remove_entry(Group *grp)
{
  dir->remove_entry((Item *)grp);
}

Group *GroupDir::get_entry(char *name)
{
  Item *curr = dir->first_item();
  
  while (curr && (strcmp(curr->name, name)))
    curr = dir->next_item();

  return (Group *)curr;
}

void GroupDir::top_level_items()
{
  cout << "Generating list of top level item names" <<endl;
}
