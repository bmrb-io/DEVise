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
  Revision 1.1  1995/09/22 20:09:25  ravim
  Group structure for viewing schema
*/

#include <stdio.h>
#include <iostream.h>
#include "Group.h"
#include "ItemList.h"

Group::Group(char *nm,Group *par, int typ)
{
  strcpy(name, nm);
  type = typ;
  parent = par;
  subgrps = new(ItemList);
}

Group::~Group()
{
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

void Group::subitems(Tcl_Interp *interp)
{
  char attrbuf[MAX_STR_LEN];
  Group *curr;
  cout << "Returning list of subitems in this group" <<endl;

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
    printf("Item : %s ", curr->name);
    if (curr->type == SUBGRP)
    {
      printf(" is a GROUP item\n");
      sprintf(attrbuf, "%s intr", curr->name);
    }
    else if (curr->type == ITEM)
    {
      printf(" is a leaf item\n");
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
