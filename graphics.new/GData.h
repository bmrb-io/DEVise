/* GData.h: graphical data virtual base class*/
#ifndef GData_h
#define GData_h
#include "TData.h"
#include "DList.h"

class RecFile;
class GDataRangeMap;
class GDataRangeMapRec;
class TData;

/* Callback about GData converstion*/
class GDataCallback {
public:
	virtual void Converted(RecId low, RecId high)= 0;
};

DefinePtrDList(GDataCallbackList, GDataCallback *)

class GData: public TData {
public:
	virtual AttrList *GetAttrList();

	/* callback registration */
	void RegisterCallback(GDataCallback *c);

	/* maxBuf: max amount of space to be used to store GData */
	GData(TData *tdata, char *fname, int recSize, int maxBuf);
	virtual ~GData();

    /* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	virtual int Dimensions(int *sizeDimension);

	/* Return record size, or -1 if variable record size */
	virtual int RecSize();

	/* Return page size of TData, or -1 if no paging structure */
	virtual int PageSize();

	int UserAttr(int attrNum);

	/* Return true if TData deletes records from the beginning
	due to limited disk/memory buffer. */
	virtual Boolean HasDeletion(); 

	virtual Boolean HasAppend();

    /* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices);

	virtual char *GetName();

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId);

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId);

	/**** Getting Records ****/

	/**************************************************************
	Init getting records.
	***************************************************************/
	virtual void InitGetRecs(RecId lowId, RecId highId,RecordOrder order);

	/**************************************************************
	Get next batch of records, as much as fits into buffer. 
	Return false if no more.
	input:
		buf: where to start putting records.
		bufSize: size of buffer being supplied.
	output:
		startRid : starting record ID of this batch 
		numRecs: number of records.
		dataSize: # of bytes taken up by data.
		recPtrs: pointer to records for variable size records.
	**************************************************************/
	virtual Boolean GetRecs(void *buf, int bufSize, RecId &startRid,
		int &numRecs, int &dataSize, void **recPtrs);

	virtual void DoneGetRecs();

	virtual int GetModTime();

	/* Do a checkpoint */
	virtual void Checkpoint(){};


	/* Get pointers to variable size records. Not implemented
	for this one */
	virtual void GetRecPointers(RecId startId, int numRecs,
		void *buf, void **recPtrs);

	/* For writing records. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	/* return number of records left to convert, or -1 if no limit */
	int RecsLeftToConvert() { return _recsLeft;}

	/* Update info about which TData records have been converted into GData .
	(tLow, tHigh): new range that has been converted.
	firstRec: GData record for tLow
	lastRec: GDataRec for tHigh. */
	void UpdateConversionInfo(RecId tLow, RecId tHigh,
		void *firstRec, void *lastRec);
	

	/* Get next unconverted range <= id.
	Return true if open ended (no high limit)*/
	Boolean NextUnConverted(RecId id, RecId &low, RecId &high);

	/* Return # of records left in buffer to store GData*/
	int BufRecsLeft() { return _recsLeft; }

	/* Iterator for what's converted, returned in ascending order */
	void InitConvertedIterator();
	Boolean NextRange(RecId &lowId, RecId &highId);
	void DoneConvertedIterator();


	/* Print what has been converted */
	void PrintConverted();

private:
	RecFile *_recFile;
	int _recSize;
	GDataRangeMap *_rangeMap;
	int _recsLeft; /* # of records left to convert */
	int _totalRecs; /* # of recs converted */

	/* For getting records */
	RecId _nextId, _highId, _numRecs;
	GDataRangeMapRec *_rec;

	GDataCallbackList _callbacks;


	/* from DispatcherCallback */
	/*
	virtual void Cleanup();
	*/

	TData *_tdata;
};
#endif
