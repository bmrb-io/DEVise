/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  University of Wisconsin at Madison
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

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include "StateMap.h"

int GenStringHash(char *&string, int numBuckets)
{
  int sum = 0;
  for(int i = 0; i < (int)strlen(string); i++)
    sum += string[i];
  return sum % numBuckets;
}

int GenStringComp(char *&string1, char *&string2)
{
  return strcmp(string1, string2);
}

#if defined(SGI)
template class HashTable<char *, stateMapRec *>;
#endif

HashTable<char *, stateMapRec *> genStateMap(50, GenStringHash,
                                             GenStringComp);
