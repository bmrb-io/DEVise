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
  Revision 1.3  1997/03/28 16:10:26  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:15:22  jussi
  Added CVS header.
*/

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
	Boolean Find(TData *tdata, Coord lowId , RangeInfo *& buf);

	/* print the buffers in the hash table */
	void Print();

	/* print statistics */
	void PrintStat();

private:

	/* hash function */
	int Hash(TData *tdata, Coord lowId) { 
		return ((unsigned)(lowId % RANGE_HASH_TABLE_SIZE));
	};

	RangeInfo *_hashTable[RANGE_HASH_TABLE_SIZE];

	int _numFind;	/* # of times Find() is called */
	int _numFindSearch; /* # of steps Find() has to search */
};
#endif
