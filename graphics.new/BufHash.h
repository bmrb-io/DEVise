/*
  $Id$

  $Log$*/

/* hash table for buffer manager */

#ifndef BufHash_h
#define BufHash_h
#include "DeviseTypes.h"
#include "BufMgr.h"

const int HASH_TABLE_SIZE = 2001;
class BufHash {
public:
	BufHash();

	/* Insert buffer into hash table. Error if already exists */
	void Insert(BufPage *buf);

	/* Delete buffer from hash table. */
	void Delete(BufPage *buf);

	/* Find buffer entry for the given page. Return TRUE if found */
	Boolean Find(DiskFile *pfile, int page, BufPage *& buf);

	/* print the buffers in the hash table */
	void Print();

	/* print statistics */
	void PrintStat();

private:

	/* hash function */
	int Hash(DiskFile *pfile, int page) { 
		/*
		return (((unsigned)((((int) pfile)<<8 + page))) % HASH_TABLE_SIZE); 
		*/
		return ((unsigned)(page % HASH_TABLE_SIZE));
	};

	BufPage *_hashTable[HASH_TABLE_SIZE];

	int _numFind;	/* # of times Find() is called */
	int _numFindSearch; /* # of steps Find() has to search */
};
#endif
