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
  Revision 1.26  1996/12/03 20:31:35  jussi
  Updated to reflect new TData interface.

  Revision 1.25  1996/11/23 21:14:24  jussi
  Removed failing support for variable-sized records.

  Revision 1.24  1996/11/22 20:41:10  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.

  Revision 1.23  1996/11/18 22:50:32  jussi
  Added estimation of total number of records in data set.

  Revision 1.22  1996/10/08 21:49:09  wenger
  ClassDir now checks for duplicate instance names; fixed bug 047
  (problem with FileIndex class); fixed various other bugs.

  Revision 1.21  1996/10/07 22:54:01  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.20  1996/10/04 17:24:17  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.19  1996/10/02 15:23:52  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

  Revision 1.18  1996/08/27 19:03:27  flisakow
    Added ifdef's around some informational printf's.

  Revision 1.17  1996/08/04 21:59:54  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.16  1996/07/13 01:59:24  jussi
  Moved initialization of i to make older compilers happy.

  Revision 1.15  1996/07/05 15:19:15  jussi
  Data source object is only deleted in the destructor. The dispatcher
  now properly destroys all TData objects when it shuts down.

  Revision 1.14  1996/07/03 23:13:42  jussi
  Added call to _data->Close() in destructor. Renamed
  _fileOkay to _fileOpen which is more accurate.

  Revision 1.13  1996/07/02 22:48:33  jussi
  Removed unnecessary dispatcher call.

  Revision 1.12  1996/07/01 20:23:16  jussi
  Added #ifdef conditionals to exclude the Web data source from
  being compiled into the Attribute Projection executable.

  Revision 1.11  1996/07/01 19:28:09  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.10  1996/06/27 18:12:41  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.9  1996/06/27 15:49:34  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.8  1996/06/04 19:58:48  wenger
  Added the data segment option to TDataBinary; various minor cleanups.

  Revision 1.7  1996/05/22 17:52:16  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.6  1996/05/07 16:44:19  jussi
  Cache file name now based on file alias (TData name). Added recPos
  parameter to Decode() function call. Added support for a simple
  index which is needed when streams are split into multiple
  sub-streams (via matching values defined in the schema).

  Revision 1.5  1996/05/05 03:08:15  jussi
  Added support for composite attributes. Also added tape drive
  support.

  Revision 1.4  1996/04/20 19:56:58  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.3  1996/04/16 20:38:52  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.2  1996/01/25 20:22:48  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.1  1996/01/23 20:54:49  jussi
  Initial revision.
*/

//#define DEBUG

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Parse.h"
#include "TDataBinary.h"
#include "Exit.h"
#include "Util.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DevError.h"
#include "DataSeg.h"
#include "QueryProc.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#endif

#define CONCURRENT_IO

/* We cache the first BIN_CONTENT_COMPARE_BYTES from the file.
   The next time we start up, this cache is compared with what's in
   the file to determine if they are the same file. */

static const int BIN_CONTENT_COMPARE_BYTES = 4096;

static char fileContent[BIN_CONTENT_COMPARE_BYTES];
static char indexFileContent[BIN_CONTENT_COMPARE_BYTES];
static char *   srcFile = __FILE__;

TDataBinary::TDataBinary(char *name, char *type, char *param,
                         int recSize, int physRecSize)
: TData(name, type, param, recSize)
{
  _physRecSize = physRecSize;

  if (!strcmp(_type, "UNIXFILE")) {
    _file = CopyString(_param);
#ifndef ATTRPROJ
  } else if (!strcmp(_type, "WWW")) {
    _file = MakeCacheFileName(_name, _type);
#endif
  } else {
    fprintf(stderr, "Invalid TData type: %s\n", _type);
    DOASSERT(0, "Invalid TData type");
  }

  _fileOpen = true;
  if (_data->Open("r") != StatusOk)
    _fileOpen = false;
  
  DataSeg::Set(NULL, NULL, 0, 0);

  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;
  _lastIncompleteLen = 0;

  _totalRecs = 0;

#ifdef CONCURRENT_IO
  if (_fileOpen) {
      if (_data->InitializeProc() < 0)
        fprintf(stderr, "Cannot use concurrent I/O to access data stream\n");
  }
#endif

  float estNumRecs = _data->DataSize() / _physRecSize;
  _indexP = new FileIndex((unsigned long)estNumRecs);

#if defined(DEBUG)
  printf("Allocated %lu index entries\n", (unsigned long)estNumRecs);
#endif

  Dispatcher::Current()->Register(this, 10, AllState, false, -1);
}

TDataBinary::~TDataBinary()
{
#if defined(DEBUG)
  printf("TDataBinary destructor\n");
#endif

  if (_fileOpen) {
      if (_data->SupportsAsyncIO() && _data->TerminateProc() < 0)
          fprintf(stderr, "Could not terminate data source process\n");
      _data->Close();
  }

  Dispatcher::Current()->Unregister(this);

  delete _indexP;
  delete _indexFileName;
}

Boolean TDataBinary::CheckFileStatus()
{
  // see if file is (still) okay
  if (!_data->IsOk()) {
    // if file used to be okay, close it
    if (_fileOpen) {
      if (_data->SupportsAsyncIO() &&_data->TerminateProc() < 0)
          fprintf(stderr, "Could not terminate data source process\n");
      Dispatcher::Current()->Unregister(this);
      printf("Data stream %s is no longer available\n", _name);
      _data->Close();
#ifndef ATTRPROJ
      QueryProc::Instance()->ClearTData(this);
#endif
      _fileOpen = false;
    }
    if (_data->Open("r") != StatusOk) {
      // file access failure, get rid of index
      _indexP->Clear();
      _initTotalRecs = _totalRecs = 0;
      _initLastPos = _lastPos = 0;
      _lastIncompleteLen = 0;
      return false;
    }
    printf("Data stream %s has become available\n", _name);
    _fileOpen = true;
#ifdef CONCURRENT_IO
    if (_data->InitializeProc() < 0)
      fprintf(stderr, "Cannot use concurrent I/O to access data stream\n");
#endif
    Dispatcher::Current()->Register(this, 10, AllState, false, -1);
  }

  return true;
}

int TDataBinary::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

Boolean TDataBinary::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

Boolean TDataBinary::LastID(RecId &recId)
{
  if (!CheckFileStatus()) {
    recId = _totalRecs - 1;
    return false;
  }

  if (!_data->isTape()) {
    /* See if file has shrunk or grown */
    _currPos = _data->gotoEnd();
#if DEBUGLVL >= 5
    printf("TDataBinary::LastID: currpos: %ld, lastpos: %ld\n", 
	   _currPos, _lastPos);
#endif
    if (_currPos < _lastPos) {
      /* File has shrunk, rebuild index from scratch */
      InvalidateTData();
    } else if (_currPos > _lastPos) {
      /* File has grown, build index for new records */
#if DEBUGLVL >= 3
      printf("Extending index...\n");
#endif
      BuildIndex();
#ifndef ATTRPROJ
      QueryProc::Instance()->RefreshTData(this);
#endif
    }
  }
  
  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

TData::TDHandle TDataBinary::InitGetRecs(RecId lowId, RecId highId,
                                         Boolean asyncAllowed,
                                         ReleaseMemoryCallback *callback)
{
#if DEBUGLVL >= 3
  cout << " RecID lowID  = " << lowId << " highId " << highId << endl;
#endif

  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  TDataRequest *req = new TDataRequest;
  DOASSERT(req, "Out of memory");

  req->nextId = lowId;
  req->endId = highId;
  req->relcb = callback;

  /* Compute location and number of bytes to retrieve */
  streampos_t offset = _indexP->Get(req->nextId);
  iosize_t bytes = _indexP->Get(req->endId) + 1024 - offset;
  if ((long)req->endId < _totalRecs - 1) {
      /* Read up to the beginning of next record */
      bytes = _indexP->Get(req->endId + 1) - offset;
  }

  /*
     Don't use asynchronous I/O is caller prohibits it, or if
     data source doesn't support it, or if some other caller is
     already using it.
  */
  if (!asyncAllowed || !_data->SupportsAsyncIO()
      || _data->NumPipeData() > 0 || _data->IsBusy()) {
#if DEBUGLVL >= 3
      printf("Retrieving %llu:%lu bytes from TData 0x%p with direct I/O\n",
             offset, bytes, this);
#endif
      /* Zero handle indicates direct I/O */
      req->iohandle = 0;
  } else {
      /* Submit I/O request to the data source process */
      req->iohandle = _data->ReadProc(offset, bytes);
      DOASSERT(req->iohandle >= 0, "Cannot submit I/O request");
#if DEBUGLVL >= 3
      printf("Retrieving %llu:%lu bytes from TData 0x%p with I/O handle %d\n",
             offset, bytes, this, req->iohandle);
#endif
      req->pipeFlushed = false;
      _currPos = offset;
  }

  req->lastChunk = req->lastOrigChunk = NULL;
  req->lastChunkBytes = 0;

  return req;
}

Boolean TDataBinary::GetRecs(TDHandle req, void *buf, int bufSize,
                             RecId &startRid, int &numRecs, int &dataSize)
{
  DOASSERT(req, "Invalid request handle");

#if DEBUGLVL >= 3
  printf("TDataBinary::GetRecs: handle %d, buf = 0x%p\n", req->iohandle, buf);
#endif

  DOASSERT(req->iohandle >= 0, "I/O request not initialized properly");

  numRecs = bufSize / _recSize;
  DOASSERT(numRecs > 0, "Not enough record buffer space");

  if (req->nextId > req->endId)
    return false;
  
  int num = req->endId - req->nextId + 1;
  if (num < numRecs)
    numRecs = num;
  
  if (req->iohandle == 0)
    ReadRec(req->nextId, numRecs, buf);
  else
    ReadRecAsync(req, req->nextId, numRecs, buf);
  
  startRid = req->nextId;
  dataSize = numRecs * _recSize;
  req->nextId += numRecs;
  
  _bytesFetched += dataSize;
  
  if (req->nextId > req->endId)
    req->iohandle = -1;

  return true;
}

void TDataBinary::DoneGetRecs(TDHandle req)
{
  DOASSERT(req, "Invalid request handle");

  /*
     Release chunk of memory cached from pipe.
  */
  if (req->relcb && req->lastOrigChunk)
      req->relcb->ReleaseMemory(MemMgr::Buffer, req->lastOrigChunk, 1);

  if (!req->pipeFlushed) {
    /*
       Flush data from pipe. We would also like to tell the DataSource
       (which is at the other end of the pipe) to stop, but we can't
       do that yet.
    */
    while (1) {
      char *chunk;
      streampos_t offset;
      iosize_t bytes;
      int status = _data->Consume(chunk, offset, bytes);
      DOASSERT(status >= 0, "Cannot consume data");
      if (bytes <= 0)
        break;
      /*
         Release chunk so buffer manager (or whoever gets the following
         call) can make use of it.
      */
      if (req->relcb)
        req->relcb->ReleaseMemory(MemMgr::Buffer, chunk, 1);
    }
  }

  delete req;
}

#if 0
TData::TDHandle TDataBinary::InitGetRecs(RecId lowId, RecId highId,
                                         Boolean asyncAllowed,
                                         ReleaseMemoryCallback *callback)
{
  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  TDataRequest *req = new TDataRequest;
  DOASSERT(req, "Out of memory");

  req->nextId = lowId;
  req->endId = highId;
  req->relcb = callback;

  return req;
}

Boolean TDataBinary::GetRecs(TDHandle req, void *buf, int bufSize,
                             RecId &startRid, int &numRecs, int &dataSize)
{
  DOASSERT(req, "Invalid request handle");

#if defined(DEBUG)
  printf("TDataBinary::GetRecs buf = 0x%p\n", buf);
#endif

  numRecs = bufSize / _recSize;
  DOASSERT(numRecs > 0, "Not enough record buffer space");

  if (req->nextId > req->endId)
    return false;
  
  int num = req->endId - req->nextId + 1;
  if (num < numRecs)
    numRecs = num;
  
  ReadRec(req->nextId, numRecs, buf);
  
  startRid = req->nextId;
  dataSize = numRecs * _recSize;
  req->nextId += numRecs;
  
  _bytesFetched += dataSize;
  
  return true;
}

void TDataBinary::DoneGetRecs(TDHandle req)
{
  DOASSERT(req, "Invalid request handle");

  delete req;
}
#endif

void TDataBinary::GetIndex(RecId id, int *&indices)
{
  static int index[1];
  index[0] = id;
  indices = index;
}

int TDataBinary::GetModTime()
{
  if (!CheckFileStatus())
    return -1;

  return _data->GetModTime();
}

char *TDataBinary::MakeIndexFileName(char *name, char *type)
{
  char *fname = StripPath(name);
  int nameLen = strlen(Init::WorkDir()) + 1 + strlen(fname) + 1;
  char *fn = new char[nameLen];
  sprintf(fn, "%s/%s", Init::WorkDir(), fname);
  return fn;
}

void TDataBinary::Initialize()
{
  _indexFileName = MakeIndexFileName(_name, _type);

  if (!CheckFileStatus())
    return;

  if (_data->isBuf()) {
    BuildIndex();
    return;
  }

  if (!_indexP->Initialize(_indexFileName, _data, this, _lastPos,
    _totalRecs).IsComplete()) goto error;

  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  /* continue to build index */
  BuildIndex();
  return;

 error:
  /* recover from error by building index from scratch  */
#if defined(DEBUG)
  printf("Rebuilding index...\n");
#endif
  RebuildIndex();
}

void TDataBinary::Checkpoint()
{
  if (!CheckFileStatus()) {
    printf("Cannot checkpoint %s\n", _name);
    return;
  }

  if (_data->isBuf()) {
    BuildIndex();
    return;
  }

  printf("Checkpointing %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - _initTotalRecs);
  
  if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
    /* no need to checkpoint */
    return;

  if (!_indexP->Checkpoint(_indexFileName, _data, this, _lastPos,
    _totalRecs).IsComplete()) goto error;

  /* This seems unnecessary but it is here to improve tape performance */
  _data->Seek(0, SEEK_SET);
  _currPos = _data->Tell();

  return;
  
 error:
  _currPos = _data->Tell();
}

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataBinary::BuildIndex()
{
  char physRec[_physRecSize];
  char recBuf[_recSize];
  int oldTotal = _totalRecs;
  
  _currPos = _lastPos - _lastIncompleteLen;

  // Don't bother to extend index on tape files
  if (_data->isTape() && _currPos > 0) {
      printf("Not extending index on tape file.\n");
      return;
  }

  // First go to last valid position of file
  if (_data->Seek(_currPos, SEEK_SET) < 0) {
    reportErrSys("fseek");
    return;
  }

  _lastIncompleteLen = 0;

  while(1) {

    int len = 0;

    len = _data->Fread(physRec, 1, _physRecSize);
    if (!len)
      break;

    DOASSERT(len >= 0, "Cannot read data stream");

    if (len == _physRecSize) {
      if (Decode(recBuf, _currPos / _physRecSize, physRec)) {
	_indexP->Set(_totalRecs++, _currPos);
      } else {
#if defined(DEBUG)
	printf("Ignoring invalid or non-matching record\n");
#endif
      }
      _lastIncompleteLen = 0;
    } else {
#if defined(DEBUG)
      printf("Ignoring incomplete record (%d bytes)\n", len);
#endif
      _lastIncompleteLen = len;
    }

    _currPos += len;
  }

  // last position is > current position because TapeDrive advances
  // bufferOffset to the next block, past the EOF, when tape file
  // ends
  _lastPos = _data->Tell();
  DOASSERT(_lastPos >= _currPos, "Incorrect file position");

#ifdef    DEBUG
  printf("Index for %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - oldTotal);
#endif

  if (_totalRecs <= 0)
      fprintf(stderr, "No valid records for data stream %s\n"
              "    (check schema/data correspondence)\n", _name);

  /* This seems unnecessary but it is here to improve tape performance */
  _data->Seek(0, SEEK_SET);
  _currPos = _data->Tell();
}

/* Rebuild index */

void TDataBinary::RebuildIndex()
{
  InvalidateIndex();

  _indexP->Clear();
  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;
  _lastIncompleteLen = 0;

  BuildIndex();
}

TD_Status TDataBinary::ReadRec(RecId id, int numRecs, void *buf)
{
#if defined(DEBUG)
  printf("TDataBinary::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  char *ptr = (char *)buf;

  for(int i = 0; i < numRecs; i++) {

    long recloc = _indexP->Get(id + i);

    if (_currPos != recloc) {
      if (_data->Seek(recloc, SEEK_SET) < 0) {
	reportErrSys("fseek");
	DOASSERT(0, "Cannot perform file seek");
      }
      _currPos = recloc;
    }
    if (_data->Fread(ptr, _physRecSize, 1) != 1) {
      reportErrSys("fread");
      DOASSERT(0, "Cannot read from file");
    }

    Boolean valid = Decode(ptr, _currPos / _physRecSize, ptr);
    DOASSERT(valid, "Inconsistent validity flag");

    ptr += _recSize;
    _currPos += _physRecSize;
  }

  return TD_OK;
}

TD_Status TDataBinary::ReadRecAsync(TDataRequest *req, RecId id,
                                    int numRecs, void *buf)
{
#if DEBUGLVL >= 3
  printf("TDataBinary::ReadRecAsync %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  int recs = 0;
  char *ptr = (char *)buf;
  int partialRecSize = 0;

  while (recs < numRecs) {
    /* Take chunk from cached values if present */
    char *chunk = req->lastChunk;
    char *origChunk = req->lastOrigChunk;
    iosize_t bytes = req->lastChunkBytes;

    /* No chunk in cache, get next chunk from data source */
    if (!chunk) {
        streampos_t offset;
        int status = _data->Consume(chunk, offset, bytes);
        DOASSERT(status >= 0, "Cannot consume data");
        DOASSERT((off_t)offset == _currPos, "Invalid data chunk consumed");
        _currPos += bytes;
        origChunk = chunk;
    } else {
        req->lastChunk = req->lastOrigChunk = NULL;
        req->lastChunkBytes = 0;
    }

    DOASSERT(chunk && origChunk, "Inconsistent state");

    if (bytes <= 0) {
        req->pipeFlushed = true;
        break;
    }

    while (recs < numRecs && bytes > 0) {
      if ((int)bytes < _physRecSize) {
        DOASSERT(partialRecSize == 0, "Two consecutive partial records");
        /* Store fraction of record for next loop */
        memcpy(ptr, chunk, bytes);
        partialRecSize = bytes;
#if DEBUGLVL >= 3
        printf("Caching remainder of chunk (%d bytes)\n", partialRecSize);
#endif
        break;
      }

      /*
         Append record to existing record from previous iteration of
         outer loop if there is a fragment of it left.
      */

      if (partialRecSize > 0) {
          memcpy(&ptr[partialRecSize], chunk, _physRecSize - partialRecSize);
#if DEBUGLVL >= 5
          printf("Got %d-byte record (%d partial)\n", _physRecSize,
                 partialRecSize);
#endif
          partialRecSize = 0;
      } else {
          memcpy(ptr, chunk, _physRecSize);
#if DEBUGLVL >= 5
          printf("Got %d-byte full record\n", _physRecSize);
#endif
      }

      Boolean valid = Decode(ptr, _currPos, ptr);
      if (valid) {
        ptr += _recSize;
        recs++;
      }

      chunk += _physRecSize;
      bytes -= _physRecSize;
    }

    if (recs == numRecs && bytes > 0) {
      /* Save unused piece of chunk for next call to this function */
      req->lastChunk = chunk;
      req->lastOrigChunk = origChunk;
      req->lastChunkBytes = bytes;
#if DEBUGLVL >= 3
      printf("Saving %ld bytes of chunk 0x%p for next function call\n",
             bytes, origChunk);
#endif
    } else {
      /*
         Release chunk so buffer manager (or whoever gets the following
         call) can make use of it.
      */
      if (req->relcb)
          req->relcb->ReleaseMemory(MemMgr::Buffer, origChunk, 1);
    }
  }

  if (recs != numRecs)
    fprintf(stderr, "Data source produced %d records, not %d\n",
            recs, numRecs);
  DOASSERT(recs == numRecs, "Incomplete data transfer");

  
  return TD_OK;
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");

  _totalRecs += numRecs;
  _indexP->Set(_totalRecs - 1, _lastPos);
  int len = numRecs * _physRecSize;

  if (_data->append(buf, len) != len) {
    reportErrSys("tapewrite");
    DOASSERT(0, "Cannot append to file");
  }

  _lastPos = _data->Tell();
  _currPos = _lastPos;
}

void TDataBinary::WriteLine(void *rec)
{
  WriteRecs(0, 1, rec);
}

void TDataBinary::Cleanup()
{
  Checkpoint();

  if (_data->isTape())
    _data->printStats();
}

void TDataBinary::PrintIndices()
{
  int cnt = 0;
  for(long i = 0; i < _totalRecs; i++) {
    printf("%ld ", _indexP->Get(i));
    if (cnt++ == 10) {
      printf("\n");
      cnt = 0;
    }
  }
  printf("\n");
}
