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
  Revision 1.5  1996/01/11 21:56:06  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.4  1995/11/18 01:57:49  ravim
  Groups associated with schema.

  Revision 1.3  1995/11/15 07:04:08  ravim
  Minor changes.

  Revision 1.2  1995/09/27 23:59:46  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.1  1995/09/22 20:09:26  ravim
  Group structure for viewing schema
*/

#ifndef _GROUP_H_
#define _GROUP_H_

#include <stdlib.h>

#include "TkControl.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "View.h"
#include "TData.h"
#include "TDataMap.h"
#include "Init.h"
#include "Util.h"
#include "MapInterpClassInfo.h"

#define MAX_STR_LEN 200

#define TOPGRP 1
#define SUBGRP 2
#define ITEM 3

class Group;
class ItemList;

class Group
{
public:
  char *name;
  ItemList *subgrps;
  Group *parent;
  int type;

  Group(char *name, Group *par, int typ);
  ~Group();
  Group *insert_item(char *name);
  Group *insert_group(char *name);
  Group *parent_group();
  void subitems(Tcl_Interp *interp);
  void subitems(char *);
};

#endif
