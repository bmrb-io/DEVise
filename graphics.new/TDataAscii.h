/* TDataAscii.h: Textual data virtual base class*/
#ifndef TDataAscii_h
#define TDataAscii_h
#include <stdio.h>
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "TData.h"
#include "RecId.h"
#include "RecOrder.h"

const int INIT_INDEX_SIZE= 50000; /* initial index size */
const int INDEX_ALLOC_INCREMENT = 25000; /* allocation increment for index */
const int LINESIZE = 4096; /* maximum size of each line */

/* We cache the first FILE_CONTENT_COMPARE_BYTES from the
file in the cache. The next time we start up, this cache is
compared with what's in the file to determine if they are
the same file. */
const int FILE_CONTENT_COMPARE_BYTES = 4096;

/* offset for the cache */
const int FILE_CONTENT_OFFSET = 0;
const int SUBCLASS_OFFSET = 4096;
const int LAST_POS_OFFSET = 5120;
const int TOTAL_RECS_OFFSET = 5124;
const int INDEX_OFFSET = 5128;
class TDataAscii: public TData, private DispatcherCallback {
public:
	TDataAscii(char *name, int recSize);

	virtual ~TDataAscii();

	/**** MetaData about TDataAscii ****/
	virtual AttrList *GetAttrList(){ return NULL;}

	/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	virtual int Dimensions(int *sizeDimension);

	/* Return record size, or -1 if variable record size */
	virtual int RecSize();

	/* Return page size of TDataAscii, or -1 if no paging structure */
	virtual int PageSize();

	/* Return true if TDataAscii deletes records from the beginning
	due to limited disk/memory buffer. */
	virtual Boolean HasDeletion();

	/* Return true if TDataAscii appends records */
	virtual Boolean HasAppend();

	/* Use this to get user defined attributes.
	We reserve attributes 0-SysAttrNum for internal use.
	A -1 is returned for none-existing attrNum*/
	virtual int UserAttr(int attrNum){ return -1;};

	/* Get name */
	virtual char *GetName();

	/* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices);

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

	/* Given buffer space and RecId, set the array "recPtrs" to
	the address of individual records. For varialbe size records. */
	virtual void GetRecPointers(RecId startId, int numRecs,
				void *buf, void **recPtrs);

	/* get the time file is modified. We only require that
	files modified later has time > files modified earlier. */
	virtual int GetModTime();

    /* Do a checkpoint */
	virtual void Checkpoint();

	/* writing a record. For TDataAscii, the new record
	is appended to the file (startRid not examined), numRecs ==1, 
	and buf points to a string to be written to disk. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	/* Write a line into the file, but don't make it into a record */
	void WriteLine(void *line);

protected:
	/* For derived class */
	/* should be called by the constructors of derived classes */
	void Initialize();

	/* Decode a record and put data into buffer. Return false if
	this line is not valid. */
	virtual Boolean Decode(RecId id, void *recordBuf, char *line)=0;

	/* Read/Write specific to each subclass cache. The cached information is
	to be read during file start up, and written when file closes,
	so as to get the TData back to the state at the last
	file close. Return false and the index will be rebuilt
	from scratch every time. Return true and the base class
	will reuse the index it has cached. */
	virtual Boolean WriteCache(int fd){ return false; }
	virtual Boolean ReadCache(int fd){ return false;}

private:
	/* From DispatcherCallback */
	char *DispatchedName() {
		return "TDataAscii";
	}
	virtual void Cleanup();

	/* Build index */
	void BuildIndex();

	/* Extend index to hold more */
	void ExtendIndex();

	void ReadRec(RecId id, int numRecs, void *buf);

	/* Print indices */
	void PrintIndices();

	unsigned _totalRecs;
	char *_name;
	char *_cacheFileName;
	int _recSize;	/* size of record */
	FILE *_file;	
	RecId _lowId, _highId, _nextId, _endId; /*current range to read data */

	/* index containing pointers to the file */
	long *_index;
	int _indexSize;

	long _lastPos;

	char *_recBuf;

	int _cacheFd; /* file descriptor of cache */

	/* initial # of records stored in the cache */
	int _initTotalRecs;

	/* initial last postion in the file */
	int _initLastPos;

	/* total # of bytes fetched */
	int _bytesFetched;
};
#endif
