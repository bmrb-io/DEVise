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
  Revision 1.1  1996/04/22 18:01:39  wenger
  First version of "attribute projection" code.  The parser (with
  the exception of instantiating any TData) compiles and runs.

*/

#include <stdio.h>
#include <iostream.h>
#include "Group.h"
#include "ItemList.h"

//#define DEBUG

Group::Group(char *nm,Group *par, int typ)
{
  name = new char[strlen(nm) + 1];
  strcpy(name, nm);
  type = typ;
  parent = par;
  subgrps = new(ItemList);
}

Group::~Group()
{
  delete [] name;
  delete(subgrps);
}

Group *Group::insert_item(char *nm)
{
  Group *newitem = new Group(nm, this, ITEM);

  subgrps->add_entry(newitem);
  return newitem;
}

Group *Group::insert_group(char *nm)
{
  Group *newgrp = new Group(nm, this, SUBGRP);
  
  subgrps->add_entry(newgrp);
  return newgrp;
}

Group *Group::parent_group()
{
  return parent;
}

#if 0
void Group::subitems(Tcl_Interp *interp)
{
  char attrbuf[MAX_STR_LEN];
  Group *curr;

  /* If this is a top level group there is an additional item called
     "recId" which is not stored in our lists */
  if (type == TOPGRP)
  {
    sprintf(attrbuf, "recId leaf");
    Tcl_AppendElement(interp, attrbuf);
  }    
  
  curr = subgrps->first_item();
  while (curr)
  {
#ifdef DEBUG
    printf("Item : %s ", curr->name);
#endif
    if (curr->type == SUBGRP)
    {
#ifdef DEBUG
      printf(" is a GROUP item\n");
#endif
      sprintf(attrbuf, "%s intr", curr->name);
    }
    else if (curr->type == ITEM)
    {
#ifdef DEBUG
      printf(" is a leaf item\n");
#endif
      sprintf(attrbuf, "%s leaf", curr->name);
    }
    else
    {
      printf("Error: top level group within group \n");
      exit(0);
    }

    Tcl_AppendElement(interp, attrbuf);
    curr = subgrps->next_item();
  }
}
#endif
