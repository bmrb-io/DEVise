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
#include "Group.h"
#include "ItemList.h"

Item::Item(char *nm)
{
  name = nm;
}

Item::~Item()
{

}

Group::Group(char *name,Group *par, int top)
:Item(name)
{
  parent = par;
  toplevel = top;
  subgrps = new(ItemList);
}

Group::~Group()
{
  delete(subgrps);
}

Item *Group::insert_item(char *name)
{
  Item *newitem = new Item(name);

  subgrps->add_entry(newitem);
  return newitem;
}

Group *Group::insert_group(char *name)
{
  Group *newgrp = new Group(name, this, NOTTOP);
  
  subgrps->add_entry((Item *)newgrp);
  return newgrp;
}

Group *Group::parent_group()
{
  return parent;
}

void Group::subitems()
{
  cout << "Returning list of subitems in this group" <<endl;
}
