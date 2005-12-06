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
  Revision 1.2.42.1  2005/09/06 22:04:55  wenger
  Added proper const-ness to HashTable.

  Revision 1.2  1997/03/28 16:10:23  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _MAPINTTOINT_H_
#define _MAPINTTOINT_H_

#include "HashTable.h"

extern int IntHash(const int& i, int buckets);


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
