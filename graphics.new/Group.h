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

#ifndef _GROUP_H_
#define _GROUP_H_

#define TOPLEVEL 1
#define NOTTOP   0

class Item
{
public:
  char *name;

  Item(char *name);
  ~Item();
  virtual int composite() { return 0; }
};

class Group;
class ItemList;

class Group:public Item
{
public:
  ItemList *subgrps;
  Group *parent;
  int toplevel;

  Group(char *name, Group *par, int top);
  ~Group();
  virtual int composite() {return 1;}
  Item *insert_item(char *name);
  Group *insert_group(char *name);
  Group *parent_group();
  void subitems();
};

#endif
