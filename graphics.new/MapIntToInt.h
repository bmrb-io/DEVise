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

#ifndef _MAPINTTOINT_H_
#define _MAPINTTOINT_H_

#include "HashTable.h"

extern int IntHash(int& i, int buckets);


class MapIntToInt
: public HashTable<int,int>
{
  public:

    MapIntToInt(int table_size)
    : HashTable<int,int>(table_size, IntHash) {}

    ~MapIntToInt() {}

  protected:

  private:

    // N/A
    MapIntToInt(const MapIntToInt& other);
    MapIntToInt& operator=(const MapIntToInt& other);
};


#endif // _MAPINTTOINT_H_
