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

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include "MapIntToInt.h"

#ifdef __GNUG__
template class HashTable<int, int>;
#endif



int IntHash(int& i, int buckets)
{
    return i % buckets;
}

