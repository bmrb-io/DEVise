/*
  $Id$

  $Log$*/

/* hash table for buffer manager */

#ifndef RangeHash_h
#define RangeHash_h
#include "DeviseTypes.h"
#include "RangeInfo.h"

const int RANGE_HASH_TABLE_SIZE = 2001;
class TData;
class RangeHash {
public:
	RangeHash();

	/* Insert buffer into hash table. Error if already exists */
	void Insert(RangeInfo *buf);

	/* Delete buffer from hash table. */
	void Delete(RangeInfo *buf);

	/* Find buffer entry for the given page. Return TRUE if found */
	Boolean Find(TData *tdata, RecId lowId , RangeInfo *& buf);

	/* print the buffers in the hash table */
	void Print();

	/* print statistics */
	void PrintStat();

private:

	/* hash function */
	int Hash(TData *tdata, RecId lowId) { 
		return ((unsigned)(lowId % RANGE_HASH_TABLE_SIZE));
	};

	RangeInfo *_hashTable[RANGE_HASH_TABLE_SIZE];

	int _numFind;	/* # of times Find() is called */
	int _numFindSearch; /* # of steps Find() has to search */
};
#endif
