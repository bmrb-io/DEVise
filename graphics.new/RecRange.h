/* RecRange.h: used by buffer manager to keep track of
records already processed */
#ifndef RecRange_h
#define RecRange_h
#include "DeviseTypes.h"
#include "RecId.h"

/* RangeData stores information about a contiguous block of pages
that have already been processed. */
const int REC_RANGE_REC_SIZE = 128;
struct RecRangeData{
	RecRangeData *next, *prev;	/* linked list */
	RecId low, high;
	char firstRec[REC_RANGE_REC_SIZE];	/* first record in this range */
	char lastRec[REC_RANGE_REC_SIZE]; /* last record in this range */
};

/* Callback when inserting page */
class RecRangeCallback {
public:
	/* info about recIds being merged.
	Records id and id+1 are being merged.*/
	virtual void RecsMerged(RecId id, void *rec, void *nextRec)=0;
};

class RecRange {
public:
	/* how to keep track of records for a new page entering
	the page range */
	enum RecRangeMergeInfo {
		MergeLeft,	/* merge with page 1 less than current page number */
		MergeRight, /* merge with page 1 more than current page number */
		MergeBoth, /* merge with adjacent pages on both sides*/
		MergeIgnoreRecs /* same as MergeBoth, but ignore records */
	};

	/* constructor */
	RecRange(int recSize);

	/* destructor */
	~RecRange();

	/* Clear all the ranges */
	void Clear();

	/* Insert an unprocessed  range
	Input:
		(low,high): record id range.
		firstInPage: 1st record in the page.
		lastInPage: last record in the page.
	Set the following return values:
	Note: 0)RecRange does not access the actual buffer page 
		1)it is an error to insert an already processed page.
	*/
	void Insert(RecId low, RecId high, void *lowRec, void *highRec,
		RecRangeMergeInfo info, RecRangeCallback *callback);
	
	/* Find the next unprocessed recId range, starting from the given
	id number. Return TRUE if high is not available. (one end
	of range open )*/
	Boolean NextUnprocessed(RecId current, RecId &low, RecId &high);


	/* Return Number of ranges in the page range */
	int NumRanges(){ return _rangeListSize; }

	/* get the list of ranges. This is intended for fast READ-ONLY
	access to the current list of page ranges.
	The pointer is to a doubly linked list of RangeData, where
	listHead is a dummy header. */
	void GetRangeList(int &numRanges, RecRangeData *&listHead){
		numRanges = _rangeListSize;
		listHead = &_rangeList;
	};

	/* print the contents of the ranges, for debugging */
	void Print();
private:

	/* main initialization for constructor */
	void Init(int maxRecSize);

	/* head of linked list of RangeData */
	struct RecRangeData _rangeList;
	int _rangeListSize ;

	/* return TRUE if _rangeList empty */
	Boolean RangeListEmpty() { return _rangeListSize == 0; };

	/* Search for the 1st RangeData that contains recId
	<= id. Return NULL if no such RagneData is found.
	Update _hint. */
	RecRangeData *Search(int id);

	RecRangeData *_hint; /* hint of where to start searching in the list */

	/* Alloc a new range data entry */
	static RecRangeData *AllocRecRangeData(int &size);
	static void FreeRecRangeData(RecRangeData *data,int &size);

	/* for statistics */
	static int _numSearch;	/* # of searches */
	static int _searchSteps; /* steps taken for search */

	int _recSize;

	char *_fname;
};
#endif
