/*
  $Id$

  $Log$*/

/* used by query processor to keep track of pages already processed */

#ifndef PageRange_h
#define PageRange_h
#include "DeviseTypes.h"

/* RangeData stores information about a contiguous block of pages
that have already been processed. */
const int PAGE_RANGE_REC_SIZE = 32;
struct RangeData{
	RangeData *next, *prev;	/* linked list */
	int lowPage; /* low page number in this range */
	int highPage; /* high page number in this range */
	char firstRec[PAGE_RANGE_REC_SIZE]; /* 1st record in this range */
	char lastRec[PAGE_RANGE_REC_SIZE]; /* last record in this range */
};


/* Callback when inserting page */
class PageRangeCallback {
public:
	/* info about pages being merged. 
	lastInP1 == last record in p1, while firstInP2 == first record in P2 */
	virtual void PageMerged(int p1, int p2, void *lastInP1, void *firstInP2)=0;
};

class PageRange {
public:
	/* how to keep track of records for a new page entering
	the page range */
	enum PageRangeMergeInfo {
		MergeLeft,	/* merge with page 1 less than current page number */
		MergeRight, /* merge with page 1 more than current page number */
		MergeBoth, /* merge with adjacent pages on both sides*/
		MergeIgnoreRecs /* same as MergeBoth, but ignore records */
	};

	/* constructor, with no file back up, and with
	memAlloc for keeping track of records in the ranges*/
	PageRange(int recSize);


	/* destructor */
	~PageRange();

	/* Clear all the ranges */
	void Clear();

	/* Insert an unprocessed  page into page range. 
	Input:
		pageNum: page number of page to process.
		firstInPage: 1st record in the page.
		lastInPage: last record in the page.
	Set the following return values:
	Note: 0)PageRange does not access the actual buffer page 
		1)it is an error to insert an already processed page.
	*/
	void InsertPage(int pageNum, void *firstInPage, void *lastInPage,
		PageRangeMergeInfo info, PageRangeCallback *callback);
	
	/* Return the page number of the next unprocessed page,
	given current page number. If currentPageNum is not
	yet processed, it is returned. Note: lowest page number == 1.*/
	int NextUnprocessed(int currentPageNum);


	/* Return Number of ranges in the page range */
	int NumRanges(){ return _rangeListSize; }


	/* get the list of ranges. This is intended for fast READ-ONLY
	access to the current list of page ranges.
	The pointer is to a doubly linked list of RangeData, where
	listHead is a dummy header. */
	void GetRangeList(int &numRanges, RangeData *&listHead){
		numRanges = _rangeListSize;
		listHead = &_rangeList;
	};

	/* print the contents of the ranges, for debugging */
	void Print();
private:

	/* main initialization for constructor */
	void Init(int recSize);

	/* head of linked list of RangeData */
	struct RangeData _rangeList;
	int _rangeListSize ;

	/* return TRUE if _rangeList empty */
	Boolean RangeListEmpty() { return _rangeListSize == 0; };

	/* Search for the 1st RangeData that contains page numbers
	<= pageNum. Return NULL if no such RagneData is found.
	Update _hint. */
	RangeData *Search(int pageNum);

	RangeData *_hint; /* hint of where to start searching in the list */

	/* Alloc a new range data entry */
	static RangeData *AllocRangeData(int &size);
	static void FreeRangeData(RangeData *data,int &size);

	/* for statistics */
	static int _numSearch;	/* # of searches */
	static int _searchSteps; /* steps taken for search */

	int _recSize;

	char *_fname;
};
#endif
