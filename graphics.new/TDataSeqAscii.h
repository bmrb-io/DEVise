/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.8  1998/10/13 19:40:47  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.7  1997/12/23 23:35:42  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.6  1997/10/10 21:13:49  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.5  1997/07/03 01:53:49  liping
  changed query interface to TData from RecId to double

  Revision 1.4  1996/12/03 20:31:35  jussi
  Updated to reflect new TData interface.

  Revision 1.3  1996/11/25 18:15:15  wenger
  Changes to non-indexed attrproj to match other changes to TData;
  changes to compile non-index attrproj and to get the rest of Devise
  to compile with that.

  Revision 1.2  1996/11/22 20:41:15  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.

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
#include "DataSource.h"
#include "FileIndex.h"

class TDataSeqAscii: public TData {
public:
	TDataSeqAscii(char *name, char *type, char *param, int recSize);

	virtual ~TDataSeqAscii();

	/**** MetaData about TDataSeqAscii ****/

	// Get list of attributes
	virtual AttrList *GetAttrList() { return NULL; }

	virtual Boolean SetAttrs(const AttrList &attrs) { return false; }

	// Return # of dimensions and the size of each dimension,
	// or -1 if unknown
	virtual int Dimensions(int *sizeDimension);

	// Return true if TDataTape appends records
	virtual Boolean HasAppend() { return true; }

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId);

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId);

	/**** Getting Records ****/

	/**************************************************************
	Init getting records.
	***************************************************************/
	virtual TDHandle InitGetRecs(Interval *interval, int &bytesleft,
                                     Boolean asyncAllowed,
                                     ReleaseMemoryCallback *callback
					);

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
	**************************************************************
        startRid and numRecs are now recorded in range
      *************************************************************/
	virtual Boolean GetRecs(TDHandle handle, void *buf, int bufSize,
                                Interval *interval, int &dataSize);

	virtual void DoneGetRecs(TDHandle handle);

	/* Do a checkpoint */
	virtual void Checkpoint();

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

private:
	/* From DispatcherCallback */
	char *DispatchedName() { return "TDataSeqAscii"; }
	virtual void Cleanup();

	virtual Boolean CheckFileStatus();

	/* Build or rebuild index */
	virtual void BuildIndex();
	virtual void RebuildIndex();

	virtual TD_Status ReadRec(RecId id, int numRecs, void *buf);

	/* Print indices */
	virtual void PrintIndices();

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
