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
  Revision 1.3  1996/05/05 03:08:29  jussi
  Added support for composite attributes. Also added tape drive
  support.

  Revision 1.2  1996/01/25 20:22:52  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.1  1996/01/23 20:54:50  jussi
  Initial revision.
*/

/* Textual data virtual base class */

#ifndef TDataBinary_h
#define TDataBinary_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "TData.h"
#include "RecId.h"
#include "RecOrder.h"
#include "tapedrive.h"

const int BIN_INIT_INDEX_SIZE= 50000;        // initial index size
const int BIN_INDEX_ALLOC_INCREMENT = 25000; // allocation increment for index

/* We cache the first BIN_CONTENT_COMPARE_BYTES from the
   file in the cache. The next time we start up, this cache is
   compared with what's in the file to determine if they are
   the same file. */

const int BIN_CONTENT_COMPARE_BYTES = 4096;

class TDataBinary: public TData, private DispatcherCallback {
public:
  TDataBinary(char *name, char *alias, int recSize, int physRecSize);

  virtual ~TDataBinary();

  /**** MetaData about TDataBinary ****/
  virtual AttrList *GetAttrList(){ return NULL;}

  /* Return # of dimensions and the size of each dimension,
     or -1 if unknown */
  virtual int Dimensions(int *sizeDimension);

  // Return record size, or -1 if variable record size
  virtual int RecSize() { return _recSize; }

  // Return page size of TDataBinary, or -1 if no paging structure
    virtual int PageSize() { return -1; }

  // Return true if TDataBinary deletes records from the beginning
  // due to limited disk/memory buffer.
  virtual Boolean HasDeletion() { return false; }

  // Return true if TDataBinary appends records
  virtual Boolean HasAppend() { return true; }

  // Use this to get user defined attributes.
  // We reserve attributes 0-SysAttrNum for internal use.
  // A -1 is returned for none-existing attrNum
  virtual int UserAttr(int attrNum) { return -1; }

  // Get name
  virtual char *GetName() { return _name; }

  /* convert RecId into index */
  virtual void GetIndex(RecId id, int *&indices);

  /* Get RecId of 1st available record, return true if available */
  virtual Boolean HeadID(RecId &recId);

  /* Get RecId of last record, return true if available */
  virtual Boolean LastID(RecId &recId);

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

  /* writing a record. For TDataBinary, the new record
     is appended to the file (startRid not examined), numRecs ==1, 
     and buf points to a string to be written to disk. */
  virtual void WriteRecs(RecId startId, int numRecs, void *buf);

  /* Write a line into the file, but don't make it into a record */
  void WriteLine(void *line);

protected:
  /* For derived class */
  /* should be called by the constructors of derived classes */
  void Initialize();

  /* Copy record into buffer. Return false if invalid record. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line) = 0;

  /* Read/Write specific to each subclass cache. The cached information
     is to be read during file start up, and written when file closes,
     so as to get the TData back to the state at the last
     file close. Return false and the index will be rebuilt
     from scratch every time. Return true and the base class
     will reuse the index it has cached. */
  virtual Boolean WriteCache(int fd){ return false; }
  virtual Boolean ReadCache(int fd){ return false; }

  static char *MakeCacheName(char *alias);

private:
  /* From DispatcherCallback */
  char *DispatchedName() { return "TDataBinary"; }

  virtual void Cleanup();

  /* Build index */
  void BuildIndex();

  /* Extend index to hold more */
  void ExtendIndex();

  void ReadRec(RecId id, int numRecs, void *buf);

  /* Print indices */
  void PrintIndices();

  long _totalRecs;                // total number of records
  char *_name;                    // name of file/dataset
  char *_alias;                   // alias of file/dataset
  char *_cacheFileName;           // name of cache file
  int _recSize;                   // size of record
  int _physRecSize;               // physical record size
  FILE *_file;                    // file pointer
  TapeDrive *_tape;               // pointer to tape drive
  Boolean _fileGrown;             // true if file has grown

  RecId _lowId, _highId;          // current range to read data
  RecId _nextId, _endId;          // range of next retrieval

  long *_index;                   // index to records
  long _indexSize;                // size of index

  long _lastPos;                  // position of last record in file
  long _currPos;                  // current file position

  char *_recBuf;                  // record buffer

  long _initTotalRecs;            // initial # of records in cache
  int _initLastPos;               // initial last position in file

  /* total # of bytes fetched */
  int _bytesFetched;
};

#endif
