/* TDataRec.h: File of records */
#ifndef TDataRec_h
#define TDataRec_h
#include "TData.h"

class RecFile;

extern int TDATAREC_CHUNK_SIZE ;
class TDataRec: public TData {
public:
	/* constructor */
	TDataRec(char *name, int recSize);

	/* destructor */
	virtual ~TDataRec();

	/**** MetaData about TData ****/

	/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	int Dimensions(int *sizeDimension);


	/* Return record size, or -1 if variable record size */
	virtual int RecSize() { return _recSize;}

	/* Return page size of TData, or -1 if no paging structure */
	virtual int PageSize() { return -1;}

	/* Return true if TData deletes records from the beginning
	due to limited disk/memory buffer. */
	Boolean HasDeletion() { return 0;}

	/* Return true if TData appends records */
	Boolean HasAppend(){ return 0;}

	/* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices); 

	virtual char *GetName();

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId);

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId);

	/**** Getting Records ****/
	virtual void InitGetRecs(RecId lowId, RecId highId,RecordOrder order);

	virtual Boolean GetRecs( void *buf, int bufSize, RecId &startRid,
		int &numRecs, int &dataSize, void **recPtrs);

	virtual void DoneGetRecs();

	/* Given buffer space and RecId, set the array "recPtrs" to
	the address of individual records. For varialbe size records. */
	virtual void GetRecPointers(RecId startId, int numRecs,
				void *buf, void **recPtrs);

    /* For writing records. Default: not implemented. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	virtual int GetModTime();


private:
	int _recSize;
	int _totalRecs;
	RecFile *_rFile;
	RecId _nextId, _endId;
	RecordOrder _recOrder; /* order to return the records */

	RecId _lowId, _highId;
	RecId _chunkStart; /* ID of start of chunk 0 */
	int _loopIndex, 
		_chunkSize, /* # of records to fetch in each chunk */
		_nextChunk, /* next chunk to fetch */
		_chunkSkip, /* how many chunks to sip */
		_numChunks; /* # of chunks */

};
#endif
