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

static char fileContent[BIN_CONTENT_COMPARE_BYTES];
static char indexFileContent[BIN_CONTENT_COMPARE_BYTES];
static char *   srcFile = __FILE__;

TDataBinary::TDataBinary(char *name, char *type, char *param,
                         int recSize, int physRecSize)
{
  _name = name;
  _type = type;
  _param = param;
  _recSize = recSize;
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

  _data = NULL;

  // Find out whether the data occupies an entire file or only
  // a segment of a file.
  char *	segLabel;
  char *	segFile;
  long		segOffset;
  long		segLength;

  DataSeg::Get(segLabel, segFile, segOffset, segLength);

  // Now instantiate the appropriate type of object, according to
  // whether this is a tape, disk file, or Web resource, and whether
  // or not the data occupies the entire file.

#ifndef ATTRPROJ
  if (!strcmp(_type, "WWW"))
  {
    if (strcmp(_name, segLabel))
    {
      DOASSERT(false, "Data segment does not match tdata");
    }
    if ((segOffset == 0) && (segLength == 0))
    {
      _data = new DataSourceWeb(_param, NULL, _file);
    }
    else
    {
      _data = new DataSourceSegment<DataSourceWeb>(_param, NULL, _file,
                                                   segOffset, segLength);
    }
  }
  else
#endif
  {
    if (strcmp(_file, segFile) || strcmp(_name, segLabel))
    {
      DOASSERT(false, "Data segment does not match tdata");
    }
    if (!strncmp(name, "/dev/rmt", 8)
        || !strncmp(name, "/dev/nrmt", 9)
        || !strncmp(name, "/dev/rst", 8)
        || !strncmp(name, "/dev/nrst", 9)) {
      if ((segOffset == 0) && (segLength == 0))
      {
        _data = new DataSourceTape(_file, NULL);
      }
      else
      {
        _data = new DataSourceSegment<DataSourceTape>(_file, NULL, NULL,
                                                      segOffset, segLength);
      }
    } else {
      if ((segOffset == 0) && (segLength == 0))
      {
        _data = new DataSourceFileStream(_file, NULL);
      }
      else
      {
        _data = new DataSourceSegment<DataSourceFileStream>(_file, NULL, NULL,
                                                            segOffset,
                                                            segLength);
      }
    }
  }

  DOASSERT(_data, "Out of memory");

  _fileOpen = true;
  if (_data->Open("r") != StatusOk)
    _fileOpen = false;
  
  DataSeg::Set(NULL, NULL, 0, 0);

  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;
  _lastIncompleteLen = 0;

  _totalRecs = 0;

  _indexSize = 0;
  _index = 0;

  Dispatcher::Current()->Register(this, 10, AllState, false, _data->AsyncFd());
}

TDataBinary::~TDataBinary()
{
#ifdef DEBUG
  printf("TDataBinary destructor\n");
#endif

  if (_fileOpen)
    _data->Close();

  Dispatcher::Current()->Unregister(this);

  delete _data;
  delete _index;
  delete _file;
  delete _param;
  delete _type;
  delete _name;
  delete _indexFileName;
}

Boolean TDataBinary::CheckFileStatus()
{
  // see if file is (still) okay
  if (!_data->IsOk()) {
    // if file used to be okay, close it
    if (_fileOpen) {
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
      delete _index;
      _initTotalRecs = _totalRecs = 0;
      _initLastPos = _lastPos = 0;
      _lastIncompleteLen = 0;
      _indexSize = 0;
      _index = 0;
      return false;
    }
    printf("Data stream %s has become available\n", _name);
    _fileOpen = true;
    Dispatcher::Current()->Register(this, 10, AllState,
                                    false, _data->AsyncFd());
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

  // see if file has grown
  _currPos = _data->gotoEnd();
  DOASSERT(_currPos >= 0, "Error finding end of data");

  if (_currPos < _lastPos) {
#ifdef DEBUG
    printf("Rebuilding index...\n");
#endif
    RebuildIndex();
#ifndef ATTRPROJ
    QueryProc::Instance()->ClearTData(this);
#endif
  } else if (_currPos > _lastPos) {
#ifdef DEBUG
    printf("Extending index...\n");
#endif
    BuildIndex();
#ifndef ATTRPROJ
    QueryProc::Instance()->RefreshTData(this);
#endif
  }

  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

void TDataBinary::InitGetRecs(RecId lowId, RecId highId, RecordOrder order)
{
  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  _lowId = lowId;
  _highId = highId;
  _nextId = lowId;
  _endId = highId;
}

Boolean TDataBinary::GetRecs(void *buf, int bufSize, 
			     RecId &startRid, int &numRecs, int &dataSize,
			     void **recPtrs)
{
#ifdef DEBUG
  printf("TDataBinary::GetRecs buf = 0x%p\n", buf);
#endif

  numRecs = bufSize / _recSize;
  DOASSERT(numRecs, "Not enough record buffer space");

  if (_nextId > _endId)
    return false;
  
  int num = _endId - _nextId + 1;
  if (num < numRecs)
    numRecs = num;
  
  ReadRec(_nextId, numRecs, buf);
  
  startRid = _nextId;
  dataSize = numRecs * _recSize;
  _nextId += numRecs;
  
  _bytesFetched += dataSize;
  
  return true;
}

void TDataBinary::GetRecPointers(RecId startId, int numRecs,
				 void *buf, void **recPtrs)
{
  DOASSERT(0, "Feature not implemented");
}

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

char *TDataBinary::MakeCacheFileName(char *name, char *type)
{
  char *fname = StripPath(name);
  char *cacheDir = getenv("DEVISE_CACHE");
  if (!cacheDir)
    cacheDir = ".";
  int nameLen = strlen(cacheDir) + 1 + strlen(fname) + 1 + strlen(type) + 1;
  char *fn = new char [nameLen];
  sprintf(fn, "%s/%s.%s", cacheDir, fname, type);
  return fn;
}

void TDataBinary::Initialize()
{
  _indexFileName = MakeIndexFileName(_name, _type);

  if (!CheckFileStatus())
    return;

  Boolean fileOpened = false;
  int i;

  int indexFd;
  if ((indexFd = open(_indexFileName, O_RDONLY, 0)) < 0)
    goto error;

  fileOpened = true;
 
  unsigned long magicNumber;
  if (read(indexFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
    perror("read");
    goto error;
  }
  if (magicNumber != 0xdeadbeef) {
    printf("Index file incompatible\n");
    goto error;
  }

  /* index file exists. See if we are still working on the same
     file, and if we are, reinitialize */
  if (_data->Seek(0, SEEK_SET) < 0) {
    perror("fseek");
    goto error;
  }
  if (_data->Fread(fileContent, BIN_CONTENT_COMPARE_BYTES, 1) != 1) {
    perror("fread");
    goto error;
  }

  if (read(indexFd, indexFileContent, BIN_CONTENT_COMPARE_BYTES)
      != BIN_CONTENT_COMPARE_BYTES) {
    perror("read");
    goto error;
  }
  if (memcmp(indexFileContent, fileContent, BIN_CONTENT_COMPARE_BYTES)) {
    printf("Index file invalid\n");
    goto error;
  }
  
  /* File has not changed since index file was built */

  /* Let subclass read index */
  if (!ReadIndex(indexFd))
    goto error;
  
  /* Read last file position */
  if (read(indexFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("read");
    goto error;
  }
  
  /* Read number of records */
  if (read(indexFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("read");
    goto error;
  }

  if (_totalRecs >= _indexSize) {
    delete _index;
    _indexSize = _totalRecs;
#ifdef DEBUG
    printf("Allocating %ld index elements\n", _indexSize);
#endif
    _index = new long [_indexSize];
    DOASSERT(_index, "Out of memory");
  }

  /* read the index */
  if (read(indexFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("read");
    goto error;
  }
  
  for(i = 1; i < _totalRecs; i++) {
    if (_index[i - 1] > _index[i]) {
      printf("Indexed index inconsistent\n");
      goto error;
    }
  }

  close(indexFd);
  
  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  /* continue to build index */
  BuildIndex();
  return;

 error:
  /* recover from error by building index from scratch  */
  if (fileOpened)
    close(indexFd);
  (void)unlink(_indexFileName);

#ifdef DEBUG
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

  printf("Checkpointing %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - _initTotalRecs);
  
  if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
    /* no need to checkpoint */
    return;
  
  Boolean fileOpened = false;
  unsigned long magicNumber = 0xdeadbeef;

  int indexFd;
  if ((indexFd = open(_indexFileName, O_CREAT| O_RDWR,
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
		      | S_IROTH | S_IWOTH)) < 0) {
    fprintf(stderr, "Cannot create index file %s\n", _indexFileName);
    perror("open");
    goto error;
  }

  fileOpened = true;
  
  if (write(indexFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
    perror("write");
    goto error;
  }

  if (_data->Seek(0, SEEK_SET) < 0) {
    perror("fseek");
    goto error;
  }
  if (_data->Fread(fileContent, BIN_CONTENT_COMPARE_BYTES, 1) != 1) {
    if (!errno)
      fprintf(stderr, "File not checkpointed due to its small size\n");
    else
      perror("fread");
    goto error;
  }
  
  /* write contents of file to be compared later */
  if (write(indexFd, fileContent, BIN_CONTENT_COMPARE_BYTES) !=
      BIN_CONTENT_COMPARE_BYTES) {
    perror("write");
    goto error;
  }
  
  /* let subclass write its contents */
  if (!WriteIndex(indexFd))
    goto error;
  
  /* write last position in the file */
  if (write(indexFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("write");
    goto error;
  }
  
  /* write # of records */
  if (write(indexFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("write");
    goto error;
  }
    
  /* write indices */
  if (write(indexFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("write");
    goto error;
  }

  close(indexFd);
  _currPos = _data->Tell();

  return;
  
 error:
  if (fileOpened)
    close(indexFd);
  (void)unlink(_indexFileName);

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

  // First go to last valid position of file
  if (_data->Seek(_currPos, SEEK_SET) < 0) {
    perror("fseek");
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
	if (_totalRecs >= _indexSize)     // index buffer too small?
	  ExtendIndex();                  // extend it
	_index[_totalRecs++] = _currPos;
      } else {
#ifdef DEBUG
	printf("Ignoring invalid or non-matching record\n");
#endif
      }
      _lastIncompleteLen = 0;
    } else {
#ifdef DEBUG
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

  printf("Index for %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - oldTotal);
}

/* Rebuild index */

void TDataBinary::RebuildIndex()
{
  InvalidateIndex();

  delete _index;
  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;
  _lastIncompleteLen = 0;
  _indexSize = 0;

  BuildIndex();
}

void TDataBinary::ReadRec(RecId id, int numRecs, void *buf)
{
#ifdef DEBUG
  printf("TDataBinary::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  char *ptr = (char *)buf;

  for(int i = 0; i < numRecs; i++) {

    long recloc = _index[id + i];

    // Note that if the data source is a tape, we _always_ seek, even if
    // we think we're already at the right place.  This was copied from
    // the previously-existing code.  RKW 5/21/96.
    if (_data->isTape() || (_currPos != recloc)) {
      if (_data->Seek(recloc, SEEK_SET) < 0) {
	perror("fseek");
	DOASSERT(0, "Cannot perform file seek");
      }
      _currPos = recloc;
    }
    if (_data->Fread(ptr, _physRecSize, 1) != 1) {
      perror("fread");
      DOASSERT(0, "Cannot read from file");
    }

    Boolean valid = Decode(ptr, _currPos / _physRecSize, ptr);
    DOASSERT(valid, "Inconsistent validity flag");

    ptr += _recSize;
    _currPos += _physRecSize;
  }
}

void TDataBinary::ExtendIndex()
{
#ifdef DEBUG
  printf("ExtendIndex:allocating %ld index elements\n",
	 _indexSize + BIN_INDEX_ALLOC_INC);
#endif

  long *newIndex = new long [_indexSize + BIN_INDEX_ALLOC_INC];
  DOASSERT(newIndex, "Out of memory");
  memcpy(newIndex, _index, _indexSize * sizeof(long));
  delete _index;
  _index = newIndex;
  _indexSize += BIN_INDEX_ALLOC_INC;
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");

  _totalRecs += numRecs;
  if (_totalRecs >= _indexSize)         // index buffer too small?
    ExtendIndex();                      // extend it

  _index[_totalRecs - 1] = _lastPos;
  int len = numRecs * _physRecSize;

  if (_data->append(buf, len) != len) {
    perror("tapewrite");
    DOASSERT(0, "Cannot append to file");
  }

  _lastPos = _data->Tell();
  _currPos = _lastPos;
}

void TDataBinary::WriteLine(void *rec)
{
  WriteRecs(0, 1, rec);
}

void TDataBinary::Run()
{
    int fd = _data->AsyncFd();
    _data->AsyncIO();
    if (_data->AsyncFd() != fd) {
        Dispatcher::Current()->Unregister(this);
        Dispatcher::Current()->Register(this, 10, AllState,
                                        false, _data->AsyncFd());
    }
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
    printf("%ld ", _index[i]);
    if (cnt++ == 10) {
      printf("\n");
      cnt = 0;
    }
  }
  printf("\n");
}
