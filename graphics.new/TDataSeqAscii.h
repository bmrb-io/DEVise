/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id:

  $Log:
*/

/* Textual data virtual base class */

#ifndef TDataSeqAscii_h
#define TDataSeqAscii_h

#include <stdio.h>
#include <sys/types.h>

#include "DeviseTypes.h"
#ifdef ATTRPROJ
#   include "ApDispatcher.h"
#else
#   include "Dispatcher.h"
#endif
#include "TData.h"
#include "RecId.h"
#include "RecOrder.h"
#include "DataSource.h"
#include "FileIndex.h"

class TDataSeqAscii: public TData, private DispatcherCallback {
public:
	TDataSeqAscii(char *name, char *type, char *param, int recSize);

	virtual ~TDataSeqAscii();

	/**** MetaData about TDataSeqAscii ****/

	// Get list of attributes
	virtual AttrList *GetAttrList() { return NULL; }

	// Return # of dimensions and the size of each dimension,
	// or -1 if unknown
	virtual int Dimensions(int *sizeDimension);

	// Return true if TDataTape appends records
	virtual Boolean HasAppend() { return true; }

	/* Convert RecId into index */
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
	virtual void InitGetRecs(RecId lowId, RecId highId, RecordOrder order);

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

	// rebuild index, et. al
	virtual void InvalidateTData();

	/* writing a record. For TDataSeqAscii, the new record
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
	virtual Boolean Decode(void *recordBuf, int recPos, char *line) = 0;

	/* Read/Write specific to each subclass index. The cached information
           is to be read during file start up, and written when file closes,
           so as to get the TData back to the state at the last
           file close. Return false and the index will be rebuilt
           from scratch every time. Return true and the base class
           will reuse the index it has cached. */
	virtual Boolean WriteIndex(int fd) { return false; }
	virtual Boolean ReadIndex(int fd) { return false; }

        /* This function is called by this class to ask the derived
           class to invalidate all indexed information */
        virtual void InvalidateIndex() {}

	static char *MakeIndexFileName(char *name, char *type);

private:
	/* From DispatcherCallback */
	char *DispatchedName() { return "TDataSeqAscii"; }
        virtual void Run();
	virtual void Cleanup();

	Boolean CheckFileStatus();

	/* Build or rebuild index */
	void BuildIndex();
	void RebuildIndex();

	TD_Status ReadRec(RecId id, int numRecs, void *buf);

	/* Print indices */
	void PrintIndices();

	long _totalRecs;                // total number of records
	long _lastPos;                  // position of last record in file
        long _lastIncompleteLen;        // length of last incomplete record
	long  _currPos;                 // current file position

	long  _prevPos;                 // previous file position
	RecId _prevRid;                 // previous RecId 

        Boolean _fileOpen;              // true if file is okay

	long _initTotalRecs;            // initial # of records in cache
	int _initLastPos;               // initial last position in file

	int _bytesFetched;              // total # of bytes fetched
};

#endif
