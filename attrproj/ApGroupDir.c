/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.2  1996/05/22 18:50:37  wenger
  Greatly simplified Init::DoInit() to only do what's necessary for
  attribute projection; other minor changes.

  Revision 1.1  1996/04/22 18:01:40  wenger
  First version of "attribute projection" code.  The parser (with
  the exception of instantiating any TData) compiles and runs.

*/

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "GroupDir.h"
#include "Util.h"

GroupDir::GroupDir()
{
  list = NULL;
}

GroupDir::~GroupDir()
{
  SchemaList *next;
  while (list) {
    next = list->next;
    delete list->sname;
    delete list->topgrps;
    delete list;
    list = next;
  }
}

void GroupDir::add_entry(char *schema)
{
  DO_DEBUG(printf("GroupDir::add_entry(%s)\n", schema));
  SchemaList *elem = new SchemaList;
  elem->sname = new char[strlen(schema) + 1];
  strcpy(elem->sname, schema);
  elem->topgrps = new ItemList;
  elem->next = list;
  list = elem;
}

// Searches for the given schema in the directory.
// If found, returns 1, else returns 0.

int GroupDir::find_entry(char *schema)
{
  DO_DEBUG(printf("GroupDir::find_entry(%s)\n", schema));
  SchemaList *curr = list;

  while (curr && (strcmp(curr->sname, schema)))
    curr = curr->next;

  if (!curr)
    return 0;

  return 1;
}

void GroupDir::add_topgrp(char *schema, Group *gp)
{
  SchemaList *curr = list;
  
  while (curr && (strcmp(curr->sname, schema)))
    curr = curr->next;

  if (!curr) {
    cout << "Could not find schema " << schema 
         << " in the directory...group not added" <<endl;
    return;
  }

  curr->topgrps->add_entry(gp);
}

#if 0
// Find all top level groups in the given schema.

void GroupDir::top_level_groups(Tcl_Interp *interp, char *schema)
{
  Group *currgrp = NULL;
  SchemaList *curr = list;
  
  while ((curr) && (strcmp(curr->sname, schema)))
    curr = curr->next;

  if (!curr) {
    cout << "Could not find schema "<< schema 
      << " in the schema directory "<<endl;
    return;
  }

  currgrp = curr->topgrps->first_item();
  while (currgrp) {
    Tcl_AppendElement(interp, currgrp->name);
    currgrp = curr->topgrps->next_item();
  }
}
#endif


#if 0
// Searches through the schema directory to find the appropriate entry.
// Then search through the topgrps list for that schema.
// Finally, perform a recursive search till we find the group with the
// given name.
// Then, call subitems method on that group.

void GroupDir::get_items(Tcl_Interp *interp, char *schema,
			 char *topgname, char *gname)
{
  SchemaList *curr = list;
  Group *currgrp, *retgrp;

  while ((curr) && (strcmp(curr->sname, schema)))
    curr = curr->next;

  if (!curr) {
    cout << "Could not find schema "<< schema << endl;
    return;
  }

  currgrp = curr->topgrps->first_item();
  while ((currgrp) && (strcmp(currgrp->name, topgname)))
    currgrp = curr->topgrps->next_item();

  if (!currgrp) {
    cout << "Could not find top group " << topgname
         << " in schema " << schema << endl;
    return;
  }

  retgrp = find_grp(currgrp, gname);
  if (!retgrp) {
    cout << "Could not find group with name " << gname << endl;
    return;
  }

  retgrp->subitems(interp);
}
#endif
    

Group *GroupDir::find_grp(Group *gp, char *gname)
{
  Group *ret;
  Group *curr;
  if (!(strcmp(gp->name, gname)))
    return gp;

  ret = NULL;
  curr = gp->subgrps->first_item();
  while (curr && (!ret)) {
    if (curr->type != ITEM)
      ret = find_grp(curr, gname);
    curr = gp->subgrps->next_item();
  }

  return ret;
}

int GroupDir::num_topgrp(char *schema)
{
  SchemaList *curr = list;
  Group *currgrp;
  int num = 0;

  while (curr && (strcmp(curr->sname, schema)))
    curr = curr->next;

  if (!curr) {
    cout << "Could not find schema " << schema << endl;
    return 0;
  }
  
  currgrp = curr->topgrps->first_item();
  while (currgrp) {
    num++;
    currgrp = curr->topgrps->next_item();
  }

  return num;
}
