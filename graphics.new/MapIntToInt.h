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
