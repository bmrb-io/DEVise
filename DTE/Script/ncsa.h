/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */

#ifndef NCSA_H
#define NCSA_H

#define MAX_ENTRIES 10000

typedef struct {
    char *name;
    char *val;
} entry;

extern entry entries[MAX_ENTRIES];

int post_query();

#endif
