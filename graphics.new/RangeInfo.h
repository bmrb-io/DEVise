/* RangeInfo.h */
#ifndef RangeInfo_h
#define RangeInfo_h
#include <stdio.h>
#include "DeviseTypes.h"
#include "RecId.h"

class TData;
/* A range of TData. For now we define fixed size buffers
for each range */
class RangeInfo {
public:
	RangeInfo();

	/* Get TData for this range */
	TData *GetTData(){ return tdata;};

	/* Get record ID associated with this range */
	void RecIds(RecId &lowId, RecId &highId);

	/* Get data size and buffer size */
	void Sizes(int &retDataSize, int &retBufSize){
		retDataSize = dataSize;
		retBufSize = bufSize;
	}
	int DataSize() { return dataSize; }
	int BufSize() { return bufSize; }

	/* Return size of the gap at the beginning where no data can be
	stored. This gap is caused by truncating a range into two
	on page boundaries */
	int GapSize(){return (data-buf);}

	/* Get data area */
	void *GetData() { return data; }

/*
protected:
*/

	int listNum;    /* which list we are in */
	int posNum;		/* position in the list */

	TData *tdata;
	char *data;	/* pointer to beginning of data.
				There might be a gap in the beginning. */
	int dataSize;	/* size of data */
	RecId low, high; /* record IDs */

	Boolean InUse() { return _inUse > 0; };
	void ClearUse(); 
	void SetUse() { _inUse++; }
	Boolean Dirty() { return _dirty; }
	void SetDirty() { _dirty = 1; }
	void ClearDirty() { _dirty = 0;}

	friend class RangeInfoAlloc;
	friend class RangeList;
	RangeInfo *next, *prev;	/* for RangeList, and free list of RangeInfos */

	char *buf;	/* actual data buffer*/
	int bufSize;	/* size of data and buffer */

private:

	Boolean _dirty;
	int _inUse;
};

/* RangeInfo allocator */
class RangeInfoAlloc {
public:
	/* bufSize == # of buffers */
	RangeInfoAlloc(int bufSize);

	/* allocate a new range with the requested size.
	The returned range can be bigger or smaller than the requested size.
	Return NULL if no memory left */
	RangeInfo *AllocRange(int size);

	/* Put a range back onto free list */
	void FreeRange(RangeInfo *info);

	/* Truncate range by the given # of bytes and return
	a new range for it. */
	RangeInfo *Truncate(RangeInfo *rangeInfo, int numBytes);

	/* Clear all ranges.  Go back to the state where we have a single
	unallocated range. */
	void Clear();
private:

	void *_startAddr; /* start address of buffer */
	int _bufSize; /* size of buffer */

	/* Allocate/free RangeInfo */
	RangeInfo *AllocRangeInfo();
	void FreeRangeInfo(RangeInfo *rangeInfo);

	RangeInfo *_rangeInfoFreeList; /* list of rangeInfo with no buffer*/
	RangeInfo *_bufFreeList;	/* Free list of rangeInfo with actual buffer*/
};
#endif
