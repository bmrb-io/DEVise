/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:15:50  jussi
  Added CVS header.
*/

/* Textual data virtual base class */

#ifndef TDataAscii_h
#define TDataAscii_h

#include <stdio.h>
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "TData.h"
#include "RecId.h"
#include "RecOrder.h"

const int INIT_INDEX_SIZE= 50000;            // initial index size
const int INDEX_ALLOC_INCREMENT = 25000;     // allocation increment for index
const int LINESIZE = 4096;                   // maximum size of each line

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

	// Return record size, or -1 if variable record size
	virtual int RecSize() { return _recSize; }

	// Return page size of TDataAscii, or -1 if no paging structure
	virtual int PageSize() { return -1; }

	// Return true if TDataTape deletes records from the beginning
	// due to limited disk/memory buffer.
	virtual Boolean HasDeletion() { return false; }

	// Return true if TDataTape appends records
	virtual Boolean HasAppend() { return true; }

	// Use this to get user defined attributes.
	// We reserve attributes 0-SysAttrNum for internal use.
	// A -1 is returned for none-existing attrNum
	virtual int UserAttr(int attrNum) { return -1; }

	// Get name
	virtual char *GetName() { return _name; }

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

	virtual void DoneGetRecs() {}

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

	// Decode a record; return false if this line is not valid
	virtual Boolean IsValid(char *line) = 0;

	/* Decode a record and put data into buffer. Return false if
	this line is not valid. */
	virtual Boolean Decode(RecId id, void *recordBuf, char *line)=0;

	/* Read/Write specific to each subclass cache. The cached information
	is to be read during file start up, and written when file closes,
	so as to get the TData back to the state at the last
	file close. Return false and the index will be rebuilt
	from scratch every time. Return true and the base class
	will reuse the index it has cached. */
	virtual Boolean WriteCache(int fd){ return false; }
	virtual Boolean ReadCache(int fd){ return false;}

private:
	/* From DispatcherCallback */
	char *DispatchedName() { return "TDataAscii"; }

	virtual void Cleanup();

	/* Build index */
	void BuildIndex();

	/* Extend index to hold more */
	void ExtendIndex();

	void ReadRec(RecId id, int numRecs, void *buf);

	/* Print indices */
	void PrintIndices();

	unsigned _totalRecs;            // total number of records
	char *_name;                    // name of file/dataset
	char *_cacheFileName;           // name of cache file
	int _recSize;                   // size of record
	Boolean _fileGrown;             // true if file has grown
	FILE *_file;                    // file pointer

	RecId _lowId, _highId;          // current range to read data
	RecId _nextId, _endId;          // range of next retrieval

	long *_index;                   // index to records
	int _indexSize;                 // size of index

	long _lastPos;                  // position of last record in file

	char *_recBuf;                  // record buffer

	int _cacheFd;                   // file descriptor of cache

	int _initTotalRecs;             // initial # of records in cache

	int _initLastPos;               // initial last position in file

	/* total # of bytes fetched */
	int _bytesFetched;
};
#endif
