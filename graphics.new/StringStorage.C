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

  $Log$*/

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include <string.h>

#include "StringStorage.h"

int StringStorage::_stringNum = 0;
HashTable<char *, int> StringStorage::_strings(100, StringHash, StringComp);
HashTable<int, char *> StringStorage::_keys(100, KeyHash, 0);
