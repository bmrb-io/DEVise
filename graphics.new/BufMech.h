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
  Revision 1.3  1997/03/28 16:09:41  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:14:09  jussi
  Added CVS header.
*/

#ifndef BufMech_h
#define BufMech_h
#include "DeviseTypes.h"
#include "RecId.h"

class RangeInfoArrays;
class RangeInfo;
class TData;
class BufferPolicy;

/* Buffer manager mechanism */
class RangeInfo;
class MultiArray;
class RangeHash;
class BufMech{
public:
	/* constructor.
	bufSize == total buffer space in bytes.
	policy == policy manager.
	*/
	BufMech(int bufSize, BufferPolicy *policy);

	/******* Retrieving records *******/

	/* Initialization to get RangeInfos within recId ranges.
	*/
	void InitGet(TData *tdata, Coord lowId, Coord highId);

	/* Retrieve next rangeInfo.  Return false if no more.*/
	Boolean Get(RangeInfo *&range);

	/* Free rangeInfo.*/
	void Free(RangeInfo *rangeInfo );

	/* Get info about TData ranges */
	void GetInMemRanges(TData *tdata, Coord low, Coord high,
		RangeInfo **&info, int &numInfo);

	/* Print contents of the lists */
	void Print();
private:
	/* Fetch a range from TData, handling misses.
	note: startId must be aligned to beginning of range, if it's
	already in memory. */
	RangeInfo *FetchRange(TData *tdata, int recsPerPage, Coord startId);

	int _bufSize;
	int _numArrays;
	BufferPolicy *_policy;
	int _policyFlag;
	RangeHash *_rangeHash;
	RangeInfoArrays *_rangeArrays;

	/* for fetching */
	Coord _highId, _nextId;
	TData *_tdata;
	int _recsPerPage;

	/* for returning ranges of TData */
	RangeInfo **_retRanges;
};

#endif
