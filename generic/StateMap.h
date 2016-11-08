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
  Revision 1.1  1997/01/30 19:47:39  jussi
  Initial revision.
*/

#ifndef StateMap_h
#define StateMap_h

#include "HashTable.h"

struct stateMapRec {
  const char *state;
  float latitude;
  float longitude;
};

extern int GenStringHash(const char * const &string, int numBuckets);
extern int GenStringComp(const char * const &string1,
  const char * const &string2);

extern HashTable<const char *, stateMapRec *> genStateMap;

#endif
