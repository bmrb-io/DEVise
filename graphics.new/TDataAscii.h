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
  $Id$

  $Log$
  Revision 1.14  1996/07/03 23:13:51  jussi
  Added call to _data->Close() in destructor. Renamed
  _fileOkay to _fileOpen which is more accurate.

  Revision 1.13  1996/07/01 19:28:06  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.12  1996/06/27 18:12:39  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.11  1996/06/27 15:49:32  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.10  1996/06/12 14:56:33  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.9  1996/05/22 17:52:15  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.8  1996/05/07 16:43:01  jussi
  Cache file name now based on file alias (TData name). Added parameter
  to the Decode() function call.

  Revision 1.7  1996/03/26 21:18:44  jussi
  Merged with TDataTape. Added magic number to cache file.

  Revision 1.6  1996/01/25 20:22:58  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.5  1995/12/28 19:59:35  jussi
  Small fixes to remove compiler warnings.

  Revision 1.4  1995/11/24 21:35:24  jussi
  Added _currPos member.

  Revision 1.3  1995/11/22 17:52:02  jussi
  Added copyright notice and cleaned up the code. Added some
  optimizations a la TDataTape.h.

  Revision 1.2  1995/09/05 22:15:50  jussi
  Added CVS header.
*/

/* Textual data virtual base class */

#ifndef TDataAscii_h
#define TDataAscii_h

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

const int INDEX_ALLOC_INC = 10000;      // allocation increment for index
const int LINESIZE = 4096;              // maximum size of each line

/* We cache the first FILE_CONTENT_COMPARE_BYTES from the file.
   The next time we start up, this cache is compared with what's in
   the file to determine if they are the same file. */

const int FILE_CONTENT_COMPARE_BYTES = 4096;

class TDataAscii: public TData, private DispatcherCallback {
public:
	TDataAscii(char *name, char *type, char *param, int recSize);

	virtual ~TDataAscii();

	/**** MetaData about TDataAscii ****/

	// Get list of attributes
	virtual AttrList *GetAttrList() { return NULL; }

	// Return # of dimensions and the size of each dimension,
	// or -1 if unknown
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
	static char *MakeCacheFileName(char *name, char *type);

private:
	/* From DispatcherCallback */
	char *DispatchedName() { return "TDataAscii"; }
        virtual void Run();
	virtual void Cleanup();

	Boolean CheckFileStatus();

	/* Build or rebuild index */
	void BuildIndex();
	void RebuildIndex();

	/* Extend index to hold more */
	void ExtendIndex();

	void ReadRec(RecId id, int numRecs, void *buf);

	/* Print indices */
	void PrintIndices();

	long _totalRecs;                // total number of records
	char *_name;                    // name of data stream
        char *_type;                    // type of data stream
        char *_param;                   // parameters of data stream
        char *_indexFileName;           // name of index file
	int _recSize;                   // size of record
	DataSource *_data;              // Source of data (disk file or tape)

	RecId _lowId, _highId;          // current range to read data
	RecId _nextId, _endId;          // range of next retrieval

	long *_index;                   // index to records
	long _indexSize;                // size of index

	long _lastPos;                  // position of last record in file
	long _currPos;                  // current file position
        long _lastIncompleteLen;        // length of last incomplete record

        Boolean _fileOpen;              // true if file is okay

	long _initTotalRecs;            // initial # of records in cache
	int _initLastPos;               // initial last position in file

	int _bytesFetched;              // total # of bytes fetched
};

#endif
