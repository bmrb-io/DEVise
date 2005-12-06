/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.3.42.1  2005/09/06 21:20:16  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.3  1997/03/28 16:09:40  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:14:07  jussi
  Added CVS header.
*/

/* hash table for buffer manager */

#ifndef BufHash_h
#define BufHash_h
#include "DeviseTypes.h"
#include "BufMgr.h"
#include "BufPage.h"

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
