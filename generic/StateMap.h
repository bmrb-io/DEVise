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

  $Log$*/

#ifndef StateMap_h
#define StateMap_h

#include "HashTable.h"

struct stateMapRec {
  char *state;
  float latitude;
  float longitude;
};

extern int GenStringHash(char *&string, int numBuckets);
extern int GenStringComp(char *&string1, char *&string2);

extern HashTable<char *, stateMapRec *> genStateMap;

#endif
