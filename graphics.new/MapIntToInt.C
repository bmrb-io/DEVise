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

