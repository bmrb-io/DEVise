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
  Revision 1.1  1995/09/22 20:09:27  ravim
  Group structure for viewing schema
*/

#include <stdio.h>
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
  dir->add_entry(grp);
}

void GroupDir::remove_entry(Group *grp)
{
  dir->remove_entry(grp);
}
  
Group *GroupDir::get_entry(char *name)
{
  Group *curr = dir->first_item();
  
  while (curr && (strcmp(curr->name, name)))
    curr = dir->next_item();

  return curr;
}

void GroupDir::top_level_groups(Tcl_Interp *interp)
{
  Group *curr;
  cout << "Generating list of top level group names" <<endl;
  
  curr = dir->first_item();
  printf("after first item \n");
  while (curr)
  {
    if (curr->type == ITEM)
    {
      printf("Error: item found in group directory\n");
      exit(0);
    }

    if (curr->type == TOPGRP)
    {
      printf("Top level group : %s\n", curr->name);
      Tcl_AppendElement(interp, curr->name);
    }
    else
      printf("Not top level group : %s\n", curr->name);
    curr = dir->next_item();
  }
}
