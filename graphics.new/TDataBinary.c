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
  Revision 1.38  1999/11/30 22:28:29  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.37  1998/12/15 14:55:24  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.36  1997/12/23 23:35:40  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.35  1997/11/24 23:15:18  weaver
  Changes for the new ColorManager.

  Revision 1.34  1997/10/10 21:13:47  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.33  1997/09/05 22:36:29  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.32  1997/07/03 01:53:47  liping
  changed query interface to TData from RecId to double

  Revision 1.31.10.1  1997/05/21 20:40:47  weaver
  Changes for new ColorManager

  Revision 1.31  1997/01/11 20:56:05  jussi
  Removed references to _currPos.

  Revision 1.30  1997/01/09 18:51:59  jussi
  Added controlling of live data update frequency.

  Revision 1.29  1996/12/20 16:13:53  jussi
  Removed extraneous message about not being able to use concurrent I/O.

  Revision 1.28  1996/12/18 19:34:06  jussi
  Fixed minor bugs in ReadRecAsync(). Added FlushDataPipe().

  Revision 1.27  1996/12/18 15:30:43  jussi
  Added support for concurrent I/O.

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
#include "FileIndex.h"
#include "DebugLog.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#   include "DepMgr.h"
#endif

#define CONCURRENT_IO

#define DEBUGLVL 0

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
  if (!(_data->Open("r") == StatusOk))
    _fileOpen = false;
  
  DataSeg::Set(NULL, NULL, 0, 0);

  _bytesFetched = 0;
  
  _lastPos = 0;
  _lastIncompleteLen = 0;

  _totalRecs = 0;
  _lastFileUpdate = 0;

#ifdef CONCURRENT_IO
  if (_fileOpen)
    (void)_data->InitializeProc();
#endif

  float estNumRecs = _data->DataSize() / _physRecSize;
  _indexP = new FileIndex((unsigned long)estNumRecs);

#if DEBUGLVL >= 3
  printf("Allocated %lu index entries\n", (unsigned long)estNumRecs);
#endif

  Dispatcher::Current()->Register(this, 10, AllState, false, -1);
}

TDataBinary::~TDataBinary()
{
#if DEBUGLVL >= 3
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

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataBinary::BuildIndex()
{
  char physRec[_physRecSize];
  char recBuf[_recSize];
  int oldTotal = _totalRecs;
  
  long currPos = _lastPos - _lastIncompleteLen;

  // Don't bother to extend index on tape files
  if (_data->isTape() && currPos > 0) {
      printf("Not extending index on tape file.\n");
      return;
  }

  // First go to last valid position of file
  if (_data->Seek(currPos, SEEK_SET) < 0) {
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
      if (Decode(recBuf, currPos / _physRecSize, physRec)) {
	_indexP->Set(_totalRecs++, currPos);
      } else {
#if DEBUGLVL >= 3
	printf("Ignoring invalid or non-matching record\n");
#endif
      }
      _lastIncompleteLen = 0;
    } else {
#if DEBUGLVL >= 3
      printf("Ignoring incomplete record (%d bytes)\n", len);
#endif
      _lastIncompleteLen = len;
    }

    currPos += len;
  }

  // last position is > current position because TapeDrive advances
  // bufferOffset to the next block, past the EOF, when tape file
  // ends
  _lastPos = _data->Tell();
  DOASSERT(_lastPos >= currPos, "Incorrect file position");

  char logBuf[1024];
  sprintf(logBuf, "Index for %s: %ld total records, %ld new", _name,
          _totalRecs, _totalRecs - oldTotal);
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#if (DEBUGLVL >= 3)
  printf("%s\n", logBuf);
#endif

  // If things got changed, checkpoint the new index.  (Doing this immediately
  // instead of only when saving sessions.  RKW 2000-01-11.)
  if (_totalRecs - oldTotal > 0) {
    Checkpoint();
  }

  if (_totalRecs <= 0)
      fprintf(stderr, "No valid records for data stream %s\n"
              "    (check schema/data correspondence)\n", _name);

  /*
     This seems unnecessary but it is here to
     improve tape performance.
  */
  _data->Seek(0, SEEK_SET);
}

TD_Status TDataBinary::ReadRec(RecId id, int numRecs, void *buf)
{
#if DEBUGLVL >= 3
  printf("TDataBinary::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  char *ptr = (char *)buf;
  long currPos = -1;

  for(int i = 0; i < numRecs; i++) {

    long recloc = _indexP->Get(id + i);

    if (currPos != recloc) {
      if (_data->Seek(recloc, SEEK_SET) < 0) {
	reportErrSys("fseek");
	DOASSERT(0, "Cannot perform file seek");
      }
      currPos = recloc;
    }
    if (_data->Fread(ptr, _physRecSize, 1) != 1) {
      reportErrSys("fread");
      DOASSERT(0, "Cannot read from file");
    }

    Boolean valid = Decode(ptr, currPos / _physRecSize, ptr);
    DOASSERT(valid, "Inconsistent validity flag");

    ptr += _recSize;
    currPos += _physRecSize;
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
        DOASSERT(offset == req->nextChunk, "Invalid data chunk consumed");
        req->nextChunk += bytes;
        origChunk = chunk;
#if DEBUGLVL >= 3
        printf("Consumed %ld bytes from data source\n", bytes);
#endif
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

      long realCurrPos = req->nextChunk - bytes;
      if (partialRecSize > 0) {
          memcpy(&ptr[partialRecSize], chunk, _physRecSize - partialRecSize);
          chunk += _physRecSize - partialRecSize;
          bytes -= _physRecSize - partialRecSize;
          realCurrPos -= partialRecSize;
#if DEBUGLVL >= 5
          printf("Got %d-byte record (%d partial)\n", _physRecSize,
                 partialRecSize);
#endif
          partialRecSize = 0;
      } else {
          memcpy(ptr, chunk, _physRecSize);
          chunk += _physRecSize;
          bytes -= _physRecSize;
#if DEBUGLVL >= 5
          printf("Got %d-byte full record\n", _physRecSize);
#endif
      }

      Boolean valid = Decode(ptr, realCurrPos / _physRecSize, ptr);
      if (valid) {
        ptr += _recSize;
        recs++;
      }
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
}

void TDataBinary::WriteLine(void *rec)
{
  WriteRecs(0, 1, rec);
}
