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
  Revision 1.3  1995/11/18 01:53:37  ravim
  SchemaList created and groups stored with corr. schema.

  Revision 1.2  1995/09/27 23:59:49  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.1  1995/09/22 20:09:27  ravim
  Group structure for viewing schema
*/

#ifndef _GROUPDIR_H_
#define _GROUPDIR_H_

#include "ItemList.h"

class GroupDir
{
public:
  struct SchemaList
  {
    char *sname;
    ItemList *topgrps;
    SchemaList *next;
  };

  SchemaList *list;

  GroupDir();
  ~GroupDir();

  void add_entry(char *schema);

  void add_topgrp(char *schema, Group *gp);
  void top_level_groups(Tcl_Interp *interp, char *schema);
  
  void get_items(Tcl_Interp *interp, char *schema, char *topgname, char *gname);
  int find_entry(char *schema);
  int num_topgrp(char *schema);

private:
  Group *find_grp(Group *gp, char *gname);
};

#endif
