/*
  $Id$

  $Log$*/

#ifndef BufMgfFull_h
#define BufMgrFull_h
#include "BufMgr.h"
#include "RangeInfo.h"

class BufferPolicy;
class RecRange;
class TData;
class TDataRangeList;
class RangeList;
class RangeInfo;
class RangeInfoArrays;
class RangeInfoAlloc;
extern int BUFMGR_FULL_PAGESIZE;

const int MaxReturnRanges = 2;
const int RANGE_ALLOC_HIST_SIZE = 6;

class BufMgrFull: public BufMgr {
public:
	/* constructor */
	BufMgrFull(int bufSize);

    /* Free all buffers, and reinitialize */
	virtual void Clear();

	/* flush gdata from buffer */
	void ClearGData(GData *gdata);

    /***** Retrieving/Freeing records *****/

    virtual void InitGetRecs(TData *tdata,GData *gdata,
        RecId lowId, RecId highId, RecordOrder recOrder,
		 Boolean tdataOnly = false);

    /* Retrieve next batch of records.  Return false if no more batches. */
    virtual Boolean GetRecs(Boolean &isTData,
		RecId &startRecId, int &numRecs, void *&buf, void **&recs) ;
	
	virtual void DoneGetRecs();

    /* Free batch of records returned by GetRecs(), with buf
	as the address returned by GetRecs().
    Give buffer manager hint about how to dispose the records.  */
    virtual void FreeRecs(void *buf, BufHint hint, Boolean dirty = false);

    /*****  Prefetch Interface ****/
    /* Init prefetch */
    virtual void InitPrefetch(TData *tdata);

    /* Prefetch record whose ID is "current".
    Return TRUE if there is still buffer for prefetch,
    and set "next" to the next Id after "current" to be prefetched. */
    virtual Boolean DoPrefetch(RecId current, RecId &next);

    /**** Policy ****/
    /* Init buffer manager policy */
    virtual void InitPolicy(BufPolicy::policy policy);

	/* Print what's in memory for tdata */
	void Print(TData *tdata);

	/*** Alocate buffer for appending to tdata */
	virtual Boolean HasBuffer() { return 1; }
	virtual void InitAllocBuf(int recSize, int numRecs);
	virtual Boolean GetBuf(void *&buf, int &numRecs);
	virtual void FreeBuf(void *buf, TData *tdata, RecId startId);
	virtual void DoneAllocBuf();

	void InitTDataInMem(TData *tdata);
	Boolean GetInMemRecs(void *&buf, RecId &low, RecId &high);
	void FreeInMemRecs(void *buf, Boolean dirty = 0);
	void DoneTDataInMem();

	virtual void PhaseHint(BufferPolicy::Phase phase);
	virtual void FocusHint(RecId id, TData *, GData *);

	virtual void PrintStat();
private:
	/* Allocate a range suitable for up to "size" bytes. 
	Return a range with size <= size bytes.
	firm == true if definitely need a range.
		 == false if allocation can fail.
	return NULL if firm == false, and can't find a range.
	Global modified:
		_rangeArrays: remove victim from _rangeArrays
		_policy: used to determine victim and placement.
	*/
	RangeInfo *AllocRange(int size, Boolean firm= true);

	/* Unlink range from all the lists. DO NOT free any memory
	for range, or its buffer space.
	input: range: range information
		array, pos: position on the policy list.
	*/
	void UnlinkRange(RangeInfo *range, int array, int pos);


	/* ******************************************************
	Return new "high" such high-low+1 is the maximum number
	of records that can fit within page boundaries.
	The new ID is not to exceed max.
	*****************************************************/
	RecId RoundToPage(TData *tdata, RecId low, RecId high, RecId max);

	/**** Iterator to get range not in memory */
	/* Init getting a range not in memory */
	void InitGetRangeNotInMem(TData *tdata, RangeList *rangeList, 
		RecId lowId, RecId highId);
	/* Get next range in memory. Return false if no more */
	Boolean GetRangeNotInMem(RangeInfo *& rangeInfo);
	void DoneGetRangeNotInMem();

	void CalcRetArgs(Boolean isTData, Boolean isInMem,
		TData *tdata, RangeInfo *rangeInfo,RecId lowId, RecId highId, 
		void *&buf,RecId &starRid,int &numRecs);

	/* state of retrieval */
	enum State { DoGDataInMem,		/* getting GData in memory */
				DoTDataInMem,	/* getting TData in memory */
				DoGDataScan,		/* Getting GData from disk */
				DoTDataScan,		/* Getting TData from disk */
				DoneBuffer			/* done retrieving data */
	};
	State _state;
	TDataRangeList *_tdataRangeList;

	/***** For GetRecs iterator ****/
	/* for getting in mem ranges */

	/* Get next range in memory that fits _lowId and _highId.
		set rangeInfo to next valid range.
	globals used:
		_lowId, _highId, _nextRangeInMem, _listHeadInMem.
		Note: Set _nextRangeInMem should be initialized to 1st element 
		in the list before this function is called for the first time.

	*/
	RangeInfo *_listHeadInMem;	/* in memory ranges */
	RangeInfo *_nextRangeInMem;
	void InitNextRangeInMem(RangeInfo *listHead);
	Boolean GetNextRangeInMem(RangeInfo *&rangeInfo);

	/* Init getting GData in mem */
	void InitGetGDataInMem();
	Boolean GetGDataInMem(RecId &startRecId, int &numRecs, void *&buf);
	void DoneGetGDataInMem();

	RangeInfo *_tInMemRange; /* current TData range being scanned */
	RecId _tInMemCurLow;	/* next id in the range to look at */
	Boolean _getTInMem;	/* TRUE if we need to get next range to scan */
	RecId _gScanLow, _gScanHigh; /* current range to scan */
	RecId _gScanCur;		/* next Id in [_gScanLow,_gScanHigh] to look at*/
	void InitGetTDataInMem();
	Boolean GetTDataInMem(RecId &startRecId, int &numRecs, void *&buf);
	void DoneGetTDataInMem();

	enum GDataScanState{ GetGRangeState, CheckOverlapState, GScanState,
		GScanDoneState};
	GDataScanState _gScanState; /* State of GData scan */
	void InitGDataScan();
	Boolean GDataScan(RecId &startRecId, int &numRecs, void *&buf);
	void DoneGDataScan();

	void InitTDataScan();
	Boolean TDataScan(RecId &startRecId, int &numRecs, void *&buf);
	void DoneTDataScan();
	Boolean _getTRange; /* TRUE if we need to get anoterh T range to scan */
	RecId _tScanId; /* next TData id to start scanning */

	RangeList *_tRangeList;	/* current in-mem TData list */
	RangeList *_gRangeList;	/* current in-mem GData list */

	TData *_tdata;	/* TData for the query */
	GData *_gdata;	/* GData for the query */
	RecId _lowId, _highId;	/* Range of IDs to retrieve */
	Boolean _tdataOnly; /* TRUE if only fetch TData */
	void **_recPtrs;

	BufferPolicy *_policy;
	RecRange *_recRange;

	/* Find buffer size given data size. Buffer size is always on page
	boundary */
	int BufSize(int dSize){
		if (dSize % BUFMGR_FULL_PAGESIZE == 0)
			return dSize;
		else return (dSize/BUFMGR_FULL_PAGESIZE+1)*BUFMGR_FULL_PAGESIZE;
	}

	/* Truncate rangeInfo by the buffer size from the beginning, 
	and return a new one for the truncated portion. 
	Arrange the pointers so that the remaining data are
	pointing to the right place. */
	RangeInfo *Truncate(RangeInfo *rangeInfo, int bufSize);

	int _numArrays, _policyFlag;
	int _bufSize;
	int _maxPages;
	RangeInfoArrays *_rangeArrays;
	RangeInfoAlloc *_rangeAlloc;

	/* for GetRangeNotInMem */
	TData *_tdataN;
	RangeList *_rangeListN;
	RecId _lowN,_highN;
	int _dSizeN;	/* size of data left to fetch*/
	RangeInfo *_rangeN; /* Current range */

	/* check integrity of RangeInfo */
	void CheckRange(RangeInfo *rangeInfo);


	/** For allocating buffer */
	RangeInfo _allocHead; /* list of allocated ranges */
	int _allocRecs, _allocRecSize; /* # of recs left, and size of each record */
	Boolean _allocDone;


	/* Find a slot int _returnRanges to keep track of range
	being returned. Mark range as in use */
	void SetUse(RangeInfo *rangeInfo);

	/* Mark a range as cleared for usage, and return it */
	void ClearUse(void *buf, Boolean dirty=false);

	/* For checking ranges alrady returned */
	RangeInfo *_returnRanges[MaxReturnRanges];


	/* For getting in memory ranges */
	RangeInfo *_inMemHead;
	RangeInfo *_inMemCur; /* current element */

	BufferPolicy *_defaultPolicy;

	RecordOrder _tdataFetchOrder; /* order to fetch TData records */

	int _tReturned; /* # of TData bytes returned */
	int _tHits;		/* # of TData bytes hit in buffer */
	int _gReturned; /* # GData bytes returned */
	int _gHits;	/* # of GData bytes hit int buffer*/
	int _totalRanges; /* total # of ranges returned */
	int _totalBuf; /* total # of buffers occuipied by ranges returned */
	int _totalData; /* total # of data bytes occupied by ranges returned */

	/* # of times AllocRange called */
	int _totalAllocRange[RANGE_ALLOC_HIST_SIZE]; 
	int _totalAllocRangeBytes[RANGE_ALLOC_HIST_SIZE]; 

	/* for keeping track of tdata->GetRecs() */
	int _totalGetRecBufSize;
	int _totalGetRecBytes;
	int _numGetRecs;
};

#endif
