/* TDataRangeList.h */
#ifndef TDataRangeList_h
#define TDataRangeList_h

class TData;
class RangeList;
struct TDataRangeListEntry {
	TData *tdata;
	RangeList *rangeList;
	TDataRangeListEntry *next;
};

const int TDataRangeListHashSize = 11;
class TDataRangeList {
public:

	/* constructor */
	TDataRangeList();

	/* Get the RangeList associated with tdata. Create one if
	there isn't one already */
	RangeList *Get(TData *tdata);

	/* Clear all RangeLists, but don't free the RangeInfos */
	void Clear();
private:
	/* Find entry. Return NULL if not found */
	TDataRangeListEntry *Find(TData *tdata);

	int Hash(TData *tdata){
		return ((unsigned)tdata) % TDataRangeListHashSize;
	}

	TDataRangeListEntry *AllocEntry();
	void FreeEntry(TDataRangeListEntry *entry);
	
	TDataRangeListEntry *_hashTbl[TDataRangeListHashSize];
	static TDataRangeListEntry *_freeList;
};
#endif
