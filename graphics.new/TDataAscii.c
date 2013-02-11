/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.72  2009/05/13 22:41:30  wenger
  Merged x86_64_centos5_br_0 thru x86_64_centos5_br_1/dist_1_9_1x2 to
  the trunk.

  Revision 1.71.2.2  2009/05/06 20:19:19  wenger
  Got rid of extra debug output, cleaned up a few things.

  Revision 1.71.2.1  2009/05/01 22:26:41  wenger
  Debug code (and a few actual changes) trying to get DEVise to work
  on the x86_64/Centos 5 machines at BMRB (currently, opening
  histogram2.ds causes a core dump).

  Revision 1.71  2008/09/11 20:28:14  wenger
  Committed more of the "easy" compile warning fixes.

  Revision 1.70  2008/01/24 22:08:33  wenger
  Got rid of a bunch of compile warnings.

  Revision 1.69  2005/12/06 20:04:14  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.68.10.2  2003/06/26 19:10:55  wenger
  Improvement to bad file index fix: we now invalidate the TData instead
  of just rebuilding the index, so the query processor knows things
  have changed.

  Revision 1.68.10.1  2003/06/18 17:51:03  wenger
  Fixed (I think) bug 875 -- invalid TData record problem seen by
  Wavelet-IDR.

  Revision 1.68  2001/04/12 20:15:13  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

  Revision 1.67  2000/01/11 22:28:33  wenger
  TData indices are now saved when they are built, rather than only when a
  session is saved; other improvements to indexing; indexing info added
  to debug logs; moved duplicate TDataAscii and TDataBinary code up into
  TData class.

  Revision 1.66  1999/11/30 22:28:29  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.65  1998/12/15 14:55:23  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.64  1998/10/28 19:22:29  wenger
  Added code to check all data sources (runs through the catalog and tries
  to open all of them); this includes better error handling in a number of
  data source-related areas of the code; also fixed errors in the propagation
  of command results.

  Revision 1.63  1998/03/04 19:11:04  wenger
  Fixed some more dynamic memory errors.

  Revision 1.62  1997/12/23 23:35:38  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.61  1997/11/24 23:15:17  weaver
  Changes for the new ColorManager.

  Revision 1.60  1997/10/10 21:13:46  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.59  1997/09/05 22:36:27  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.58  1997/07/03 01:53:46  liping
  changed query interface to TData from RecId to double

  Revision 1.57  1997/05/28 15:39:30  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.56.4.3  1997/05/27 18:02:55  wenger
  Minor bug fixes and cleanup to Shilpa's layout manager code and associated
  code.

  Revision 1.56.4.2  1997/05/20 19:49:38  ssl
  Fixed stuff to allow empty TData

  Revision 1.56.4.1  1997/05/20 16:11:16  ssl
  Added layout manager to DEVise

  Revision 1.56.6.1  1997/05/21 20:40:46  weaver
  Changes for new ColorManager

  Revision 1.56  1997/02/26 16:31:47  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.55.4.1  1997/02/13 18:11:48  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.55  1997/01/11 20:56:04  jussi
  Removed references to _currPos.

  Revision 1.54  1997/01/09 18:48:27  jussi
  Added controlling of live data update frequency.

  Revision 1.53  1996/12/20 16:13:41  jussi
  Removed extraneous message about not being able to use concurrent I/O.

  Revision 1.52  1996/12/18 19:34:05  jussi
  Fixed minor bugs in ReadRecAsync(). Added FlushDataPipe().

  Revision 1.51  1996/12/18 15:30:29  jussi
  Added flushing of pipes when I/Os are prematurely terminated.

  Revision 1.50  1996/12/03 20:32:58  jussi
  Updated to reflect new TData interface. Added support for concurrent I/O.

  Revision 1.49  1996/11/23 21:14:23  jussi
  Removed failing support for variable-sized records.

  Revision 1.48  1996/11/22 20:41:08  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.

  Revision 1.47  1996/11/19 15:23:46  wenger
  Conditionaled out some debug code.

  Revision 1.46  1996/11/18 22:50:15  jussi
  Commented out debugging statement.

  Revision 1.45  1996/11/18 22:29:31  jussi
  Added estimation of total number of records in data set.

  Revision 1.44  1996/11/18 18:10:54  donjerko
  New files and changes to make DTE work with Devise

  Revision 1.43  1996/10/08 21:49:09  wenger
  ClassDir now checks for duplicate instance names; fixed bug 047
  (problem with FileIndex class); fixed various other bugs.

  Revision 1.42  1996/10/07 22:53:59  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.41  1996/10/04 17:24:16  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.40  1996/10/02 15:23:50  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

  Revision 1.39  1996/08/27 19:03:26  flisakow
    Added ifdef's around some informational printf's.

  Revision 1.38  1996/08/04 21:59:53  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.37  1996/07/13 01:32:33  jussi
  Moved initialization of i to make older compilers happy.

  Revision 1.36  1996/07/13 00:22:39  jussi
  Fixed bug in Initialize().

  Revision 1.35  1996/07/12 23:42:51  jussi
  Derived data sources are compiled in only for non-ATTRPROJ executables.

  Revision 1.34  1996/07/12 21:54:00  jussi
  Buffer data sources are not checkpointed.

  Revision 1.33  1996/07/12 19:36:08  jussi
  Modified code to handle derived data sources whose data is
  in a buffer but whose 'type' is one of many possible derived
  data types.

  Revision 1.32  1996/07/12 18:24:52  wenger
  Fixed bugs with handling file headers in schemas; added DataSourceBuf
  to TDataAscii.

  Revision 1.31  1996/07/05 15:20:01  jussi
  Data source object is only deleted in the destructor. The dispatcher
  now properly destroys all TData objects when it shuts down.

  Revision 1.30  1996/07/03 23:13:55  jussi
  Added call to _data->Close() in destructor. Renamed
  _fileOkay to _fileOpen which is more accurate.

  Revision 1.29  1996/07/02 22:48:33  jussi
  Removed unnecessary dispatcher call.

  Revision 1.28  1996/07/01 20:23:15  jussi
  Added #ifdef conditionals to exclude the Web data source from
  being compiled into the Attribute Projection executable.

  Revision 1.27  1996/07/01 19:28:05  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.26  1996/06/27 18:12:37  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.25  1996/06/27 15:49:31  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.24  1996/06/24 19:45:42  jussi
  TDataAscii no longer passes the fd of the open file to the
  Dispatcher. TDataAscii only needs the Dispatcher to call
  it when it's time to shut down; Cleanup() -> Checkpoint()
  functions get called at that time.

  Revision 1.23  1996/06/04 19:58:47  wenger
  Added the data segment option to TDataBinary; various minor cleanups.

  Revision 1.22  1996/06/04 14:21:39  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

  Revision 1.21  1996/05/31 15:02:25  jussi
  Replaced a couple of occurrences of _data->isTape with _data->isTape().

  Revision 1.20  1996/05/22 17:52:13  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.19  1996/05/07 16:43:00  jussi
  Cache file name now based on file alias (TData name). Added parameter
  to the Decode() function call.

  Revision 1.18  1996/04/20 19:56:56  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.17  1996/04/18 17:12:04  jussi
  Added missing #include <errno.h>.

  Revision 1.16  1996/04/18 17:04:59  jussi
  Fixed Checkpoint() which produced an unnecessary error message
  when a very small file (less than FILE_CONTENT_COMPARE_BYTES)
  was checkpointed.

  Revision 1.15  1996/04/16 20:38:50  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.14  1996/03/27 15:31:01  jussi
  Small fixes for tape TData.

  Revision 1.13  1996/03/26 21:18:43  jussi
  Merged with TDataTape. Added magic number to cache file.

  Revision 1.12  1996/03/05 22:06:04  jussi
  Minor fix in debugging output.

  Revision 1.11  1996/02/01 18:04:41  jussi
  Disabled 'Ignoring invalid record.' because this message shouldn't
  appear for ignored comments.

  Revision 1.10  1996/01/25 20:22:34  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.9  1996/01/12 15:24:45  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.8  1996/01/09 16:35:00  jussi
  Improved console output messages.

  Revision 1.7  1995/12/28 19:59:41  jussi
  Small fixes to remove compiler warnings.

  Revision 1.6  1995/12/14 21:19:31  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.5  1995/12/14 17:57:37  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.4  1995/11/24 21:34:55  jussi
  Added _currPos scheme to eliminate most of the fseek() calls.
  This appears to speed up execution significantly.

  Revision 1.3  1995/11/22 17:51:35  jussi
  Added copyright notice and cleaned up the code. Optimized some
  routines a la TDataTape.C.

  Revision 1.2  1995/09/05 22:15:49  jussi
  Added CVS header.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Parse.h"
#include "TDataAscii.h"
#include "Exit.h"
#include "Util.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DataSourceBuf.h"
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

# define  _STREAM_COMPAT

#define DEBUGLVL 0

#define CONCURRENT_IO

static const int LINESIZE = 4096;         /* maximum size of a record */

static const char *   srcFile = __FILE__;

TDataAscii::TDataAscii(char *name, char *type, char *param, int recSize)
: TData(name, type, param, recSize)
{
#if DEBUGLVL >= 1
    printf("TDataAscii::TDataAscii(0x%p)(%s, %s, %s, %d)\n",
           this, name, type, param, recSize);
    printf("_data = 0x%p\n", _data);
#endif

    _fileOpen = true;
    if (!(_data->Open("r") == StatusOk))
      _fileOpen = false;
    
    DataSeg::Set(NULL, NULL, 0, 0);
    
    _bytesFetched = 0;
    
    _lastPos = 0;
    _lastIncompleteLen = 0;

    _totalRecs = 0;
    _lastFileUpdate = 0;

    float estNumRecs = 0;

    if (_fileOpen) {
#ifdef CONCURRENT_IO
      (void)_data->InitializeProc();
#endif

      /* Read first 10 records from data source and estimate record size. */

      float recSizeSum = 0;
      int i;
      for(i = 0; i < 10; i++) {
          char buf[LINESIZE];
          if (_data->Fgets(buf, LINESIZE) == NULL)
              break;
          recSizeSum += strlen(buf);
      }

      _data->Seek(0, SEEK_SET);
    
      if (i > 0) {
        estNumRecs = 1.2 * _data->DataSize() / (recSizeSum / i);
      } else {
	estNumRecs = 0;
      }
    }

    _indexP = new FileIndex((unsigned long)estNumRecs);

#if DEBUGLVL >= 3
    printf("Allocated %lu index entries\n", (unsigned long)estNumRecs);
#endif

    Dispatcher::Current()->Register(this, 10, AllState, false, -1);
}

TDataAscii::~TDataAscii()
{
#if DEBUGLVL >= 1
  printf("TDataAscii destructor\n");
#endif

  if (_fileOpen) {
      if (_data->SupportsAsyncIO() && _data->TerminateProc() < 0)
          fprintf(stderr, "Could not terminate data source process\n");
      _data->Close();
  }

  Dispatcher::Current()->Unregister(this);

  delete _indexP;
  FreeString(_indexFileName);
}

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataAscii::BuildIndex()
{
#if defined(DEBUG)
  printf("TDataAscii(%s)::BuildIndex()\n", GetName());
#endif

  char buf[LINESIZE];
  char recBuf[_recSize];
  int oldTotal = _totalRecs;
  
  long currPos = _lastPos - _lastIncompleteLen;

  /* Don't bother to extend index on tape files */
  if (_data->isTape() && currPos > 0) {
      printf("Not extending index on tape file.\n");
      return;
  }

  /* First go to last valid position of file */
  if (_data->Seek(currPos, SEEK_SET) < 0) {
    reportErrSys("fseek");
    return;
  }

  _lastIncompleteLen = 0;

  while(1) {

    int len = 0;

    if (_data->Fgets(buf, LINESIZE) == NULL)
      break;

    len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n') {
      buf[len - 1] = 0;
      if (Decode(recBuf, currPos, buf)) {
	_indexP->Set(_totalRecs++, currPos);
      } else {
#if DEBUGLVL >= 7
	printf("Ignoring invalid record: \"%s\"\n", buf);
#endif
      }
      _lastIncompleteLen = 0;
    } else {
#if DEBUGLVL >= 7
      printf("Ignoring incomplete record: \"%s\"\n", buf);
#endif
      _lastIncompleteLen = len;
    }
    
    currPos += len;
  }

  /*
     Last position is > current position because TapeDrive advances
     bufferOffset to the next block, past the EOF, when tape file ends.
  */
  _lastPos = _data->Tell();
  DOASSERT(_lastPos >= currPos, "Incorrect file position");

  char logBuf[1024];
  int formatted = snprintf(logBuf, sizeof(logBuf),
          "Index for %s: %ld total records, %ld new", _name,
	  _totalRecs, _totalRecs - oldTotal);
  checkAndTermBuf2(logBuf, formatted);
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#if (DEBUGLVL >= 3)
  printf("%s\n", logBuf);
#endif

  // If things got changed, checkpoint the new index.  (Doing this immediately
  // instead of only when saving sessions.  RKW 2000-01-11.)
  if (_totalRecs - oldTotal > 0) {
    Checkpoint();
  }
  
  /* There can be empty file */
  if (_totalRecs < 0)
      fprintf(stderr, "No valid records for data stream %s\n"
              "    (check schema/data correspondence)\n", _name);

  /*
     This seems unnecessary but it is here to
     improve tape performance.
  */
  _data->Seek(0, SEEK_SET);
}



TD_Status TDataAscii::ReadRec(RecId id, int numRecs, void *buf)
{
#if DEBUGLVL >= 3
  printf("TDataAscii::ReadRec(%ld, %d, %p)\n", id, numRecs, buf);
#endif

  return DoReadRec(id, numRecs, buf, 1);
}

TD_Status TDataAscii::DoReadRec(RecId id, int numRecs, void *buf,
    int iteration)
{
  char line[LINESIZE];
  
  char *ptr = (char *)buf;
  long currPos = -1;

  for(int i = 0; i < numRecs; i++) {

    if (currPos != (long)_indexP->Get(id + i)) {
        if (_data->Seek(_indexP->Get(id + i), SEEK_SET) < 0) {
            perror("fseek");
            DOASSERT(0, "Cannot perform file seek");
        }
        currPos = (long)_indexP->Get(id + i);
    }
    if (_data->Fgets(line, LINESIZE) == NULL) {
      reportErrSys("fgets");
      DOASSERT(0, "Cannot read from file");
    }
    
    int len = strlen(line);
#if DEBUGLVL > 10
    printf("%d : ", id + i);
    for (int j = 0 ; j < len ; j++) {
      printf("%c", line[j]); 
    }
    printf("\n");
#endif
    if (len > 0 ) {
      DOASSERT(line[len - 1] == '\n', "Data record too long");
      line[len - 1] = '\0';
    }

    Boolean valid = Decode(ptr, currPos, line);

    if (!valid) {
      if (iteration == 1) {
	reportErrNosys("Warning: invalid TData record; rebuilding index");
	// Note: this rebuilds the index and also notifies the QP that
	// things have changed, etc.
	InvalidateTData();

        return DoReadRec(id, numRecs, buf, iteration + 1);
      } else {

	// Note: ideally, I'd like to do something other than failing
	// an assertion here -- something like causing the current command
	// to fail and closing the session.  However, there's not a
	// simple way to do that, so I'm just asserting for now.
	// wenger 2003-06-18.
	DOASSERT(false, "Rebuilt index, still have invalid TData record");
      }
    }

    ptr += _recSize;

    currPos += len;
  }

  return TD_OK;
}

TD_Status TDataAscii::ReadRecAsync(TDataRequest *req, RecId id,
                                   int numRecs, void *buf)
{
#if DEBUGLVL >= 3
  printf("TDataAscii::ReadRecAsync %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  int recs = 0;
  char *ptr = (char *)buf;
  char line[LINESIZE];
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
        if (offset != req->nextChunk)
            printf("Got %lu bytes at offset %llu, not at %llu\n",
                   bytes, offset, req->nextChunk);
        DOASSERT(offset == req->nextChunk, "Invalid data chunk consumed");
        req->nextChunk += bytes;
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
      char *eol = (char *)memchr(chunk, '\n', bytes);
      if (!eol) {
        DOASSERT(partialRecSize == 0, "Two consecutive partial records");
        /* Store fraction of record for next loop */
        memcpy(line, chunk, bytes);
        line[bytes] = 0;
        partialRecSize = bytes;
#if DEBUGLVL >= 3
        printf("Caching remainder of chunk (%d bytes): \"%s\"\n",
               partialRecSize, line);
#endif
        break;
      }

      /*
         Append record to existing record from previous iteration of
         outer loop if there is a fragment of it left. Terminating
         newline is first replaced with a null, then appended, and
         then the newline is put back.
      */

      char *record = chunk;
      int recSize = eol - record;
      int fullRecSize = recSize;

      char oldch = *eol;
      *eol = 0;

      if (partialRecSize > 0) {
          fullRecSize += partialRecSize;
          memcpy(&line[partialRecSize], record, recSize + 1);
#if DEBUGLVL >= 5
          printf("Got %d-byte record (%d partial): \"%s\"\n", fullRecSize,
                 partialRecSize, line);
#endif
          partialRecSize = 0;
          record = line;
      } else {
#if DEBUGLVL >= 5
          printf("Got %d-byte full record: \"%s\"\n", fullRecSize, record);
#endif
      }

      Boolean valid = Decode(ptr, req->nextChunk, record);
      if (valid) {
        ptr += _recSize;
        recs++;
      }

      *eol = oldch;
      chunk = eol + 1;
      bytes -= recSize + 1;
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

void TDataAscii::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");

  _totalRecs += numRecs;

  _indexP->Set(_totalRecs - 1, _lastPos);
  int len = strlen((char *)buf);

  if (_data->append(buf, len) != len) {
    reportErrSys("append");
    DOASSERT(0, "Cannot append to file");
  }

  _lastPos = _data->Tell();
}

void TDataAscii::WriteLine(void *line)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");

  int len = strlen((char *)line);

  if (_data->append(line, len) != len) {
    reportErrSys("append");
    DOASSERT(0, "Cannot append to file");
  }

  _lastPos = _data->Tell();
}
