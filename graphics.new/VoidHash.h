/*
  $Id$

  $Log$*/

#ifndef VoidHash_h
#define VoidHash_h
#include "DeviseTypes.h"

const int VOID_HASH_TABLE_SIZE = 2001;
class VoidHashEntry {
public:
	VoidHashEntry *next, *prev;
	void *val;
	void *data;

	void *operator new(size_t);
	void operator delete(void *);
private:
	static VoidHashEntry *_freeList;
};

class VoidHash {
public:
	VoidHash();

	/* Insert buffer into hash table. Error if already exists */
	void Insert(void *val, void *data);

	/* Delete buffer from hash table. */
	void Delete(void *val);

	/* Find data for the given value. Return TRUE if found */
	Boolean Find(void *val, void *&data);

	/* print the buffers in the hash table */
	void Print();

	/* print statistics */
	void PrintStat();

private:
	/* Find buffer entry for the given page. Return TRUE if found */
	Boolean Find(void *val, VoidHashEntry *&entry);

	/* hash function */
	int Hash(void *val){
		/*
		return (((unsigned)((((int) pfile)<<8 + page))) % HASH_TABLE_SIZE); 
		*/
		return (((unsigned) val) % VOID_HASH_TABLE_SIZE);
	};

	VoidHashEntry *_hashTable[VOID_HASH_TABLE_SIZE];

	int _numFind;	/* # of times Find() is called */
	int _numFindSearch; /* # of steps Find() has to search */
};
#endif
