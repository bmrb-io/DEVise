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
  Revision 1.6  1996/05/09 18:14:29  kmurli
  Modified Group.C and GroupDir.C to include an oiverloaded functions for
  get_items, subitems to take in a char * instead of Tcp_interp *. This
  is for use in the ServerAPI.c

  Revision 1.5  1995/11/18 01:57:28  ravim
  Groups associated with schema.

  Revision 1.4  1995/11/14 00:55:17  jussi
  Restricted strcpy to name to be up to sizeof name, no more.
  Some fields in compustat.schema were longer than 50 characters
  and caused memory corruption.

  Revision 1.3  1995/11/07 20:23:52  jussi
  Made output statements be part of DEBUG setup.

  Revision 1.2  1995/09/27 23:59:43  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.1  1995/09/22 20:09:25  ravim
  Group structure for viewing schema
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

void Group::subitems(char *result)
{
  Group *curr;

  /* If this is a top level group there is an additional item called
     "recId" which is not stored in our lists */
  if (type == TOPGRP)
    strcpy(result, "{recid leaf} ");

  curr = subgrps->first_item();
  while (curr) {
#ifdef DEBUG
    printf("Item : %s ", curr->name);
#endif
 if (curr->type == SUBGRP) {
#ifdef DEBUG
      printf("is a GROUP item\n");
#endif
      strcat(result, "{");
      strcat(result, curr->name);
      strcat(result, " intr} ");
    } else if (curr->type == ITEM) {
#ifdef DEBUG
      printf("is a leaf item\n");
#endif
      strcat(result, "{");
      strcat(result, curr->name);
      strcat(result, " leaf} ");
    } else {
      printf("Error: top level group within group\n");
      exit(0);
    }
    curr = subgrps->next_item();
  }
}


