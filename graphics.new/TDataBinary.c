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
#include "Init.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DevError.h"
#include "DataSeg.h"

//#define DEBUG

static char fileContent[BIN_CONTENT_COMPARE_BYTES];
static char cachedFileContent[BIN_CONTENT_COMPARE_BYTES];
static char *   srcFile = __FILE__;

TDataBinary::TDataBinary(char *name, char *alias, int recSize, int physRecSize)
{
  _name = name;
  _alias = alias;
  _recSize = recSize;
  _physRecSize = physRecSize;

  _data = NULL;

  // Find out whether the data occupies an entire file or only
  // a segment of a file.
  char *	segLabel;
  char *	segFile;
  long		segOffset;
  long		segLength;

  DataSeg::Get(segLabel, segFile, segOffset, segLength);

  if (strcmp(name, segFile) || strcmp(alias, segLabel))
  {
    DOASSERT(false, "data segment does not match tdata");
  }

  DataSeg::Set(NULL, NULL, 0, 0);

  // Now instantiate the appropriate type of object, according to
  // whether this is a tape or disk file, and whether or not the
  // data occupies the entire file.
  if (!strncmp(name, "/dev/rmt", 8) || !strncmp(name, "/dev/nrmt", 9)
     || !strncmp(name, "/dev/rst", 8) || !strncmp(name, "/dev/nrst", 9)) {

     if ((segOffset == 0) && (segLength == 0))
     {
	_data = new DataSourceTape(name, NULL);
     }
     else
     {
	_data = new DataSourceSegment<DataSourceTape>(name, NULL,
	  segOffset, segLength);
     }
  } else {
     if ((segOffset == 0) && (segLength == 0))
     {
	_data = new DataSourceFileStream(name, NULL);
     }
     else
     {
	_data = new DataSourceSegment<DataSourceFileStream>(name, NULL,
	  segOffset, segLength);
     }
  }

  if (_data == NULL)
  {
    DOASSERT(0, "Cannot open data file");
  }
  else
  {
	if (_data->Open("r") != StatusOk)
	{
      DOASSERT(0, "Cannot open data file");
	}
  }

  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;
  _totalRecs = 0;

  _indexSize = BIN_INIT_INDEX_SIZE;
  _index = new long[_indexSize];

  _fileGrown = false;

  Dispatcher::Current()->Register(this);
}

TDataBinary::~TDataBinary()
{
#ifdef DEBUG
  printf("TDataBinary destructor\n");
#endif

  Dispatcher::Current()->Unregister(this);

  delete _data;
  delete _index;
  delete _alias;
  delete _name;
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
  // see if file has grown
    _currPos = _data->gotoEnd();
  DOASSERT(_currPos >= 0, "Error finding end of data");

  if (_currPos > _lastPos)
    BuildIndex();

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
  int		result;
  struct stat sbuf;

  if (_data->isTape())
  {
	reportError("Can't get modification time for tape data", devNoSyserr);
	result = -1;
  }
  else
  {
  	int status = fstat(_data->Fileno(), &sbuf);
  	DOASSERT(status >= 0, "Cannot get file statistics");
	result = (long) sbuf.st_mtime;
  }

  return result;
}

char *TDataBinary::MakeCacheName(char *alias)
{
  char *fname = StripPath(alias);
  int nameLen = strlen(Init::WorkDir()) + 1 + strlen(fname) + 1;
  char *name = new char[nameLen];
  sprintf(name, "%s/%s", Init::WorkDir(), fname);
  return name;
}

void TDataBinary::Initialize()
{
  int i;

  _cacheFileName = MakeCacheName(_alias);

  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_RDONLY, 0)) <0)
    goto error;
  fileOpened = true;
  
  unsigned long magicNumber;
  if (read(cacheFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
    perror("read");
    goto error;
  }
  if (magicNumber != 0xdeadbeef) {
    printf("Cache file incompatible; rebuilding\n");
    goto error;
  }

  /* cache file exists. See if we are still working on the same
     file, and if we are, reinitialize */
  if (_data->Seek(0, SEEK_SET) < 0) {
    perror("fseek");
    goto error;
  }
  if (_data->Fread(fileContent, BIN_CONTENT_COMPARE_BYTES, 1) != 1) {
    perror("fread");
    goto error;
  }

  if (read(cacheFd, cachedFileContent, BIN_CONTENT_COMPARE_BYTES)
      != BIN_CONTENT_COMPARE_BYTES) {
    perror("read");
    goto error;
  }
  if (memcmp(cachedFileContent, fileContent, BIN_CONTENT_COMPARE_BYTES)) {
    printf("Cache file invalid; rebuilding\n");
    goto error;
  }
  
  /* File has not changed since cache file was built */

  /* Let subclass read cache */
  if (!ReadCache(cacheFd))
    goto error;
  
  /* Read last file position */
  if (read(cacheFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("read");
    goto error;
  }
  
  /* Read number of records */
  if (read(cacheFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("read");
    goto error;
  }

  if (_totalRecs >= _indexSize) {
    delete _index;
    _indexSize = _totalRecs + BIN_INDEX_ALLOC_INCREMENT;
#ifdef DEBUG
    printf("Initialize:allocating %ld index elements\n", _indexSize);
#endif
    _index = new long[_indexSize];
  }

  /* read the index */
  if (read(cacheFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("read");
    goto error;
  }
  
  for(i = 1; i < _totalRecs; i++) {
    if (_index[i - 1] > _index[i]) {
      printf("Cached index inconsistent; rebuilding\n");
      goto error;
    }
  }

  close(cacheFd);
  
  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  /* continue to build index */
  BuildIndex();

  return;

 error:
  /* recover frome error by building index from scratch  */
  if (fileOpened)
    close(cacheFd);

  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;

  BuildIndex();
}

void TDataBinary::Checkpoint()
{
  printf("Checkpointing %s: %ld total records, %ld new\n", _alias,
	 _totalRecs, _totalRecs - _initTotalRecs);
  
  if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
    /* no need to checkpoint */
    return;
  
  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_CREAT| O_RDWR,
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
		      | S_IROTH | S_IWOTH)) < 0)
  fileOpened = true;
  
  unsigned long magicNumber = 0xdeadbeef;
  if (write(cacheFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
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
  if (write(cacheFd, fileContent, BIN_CONTENT_COMPARE_BYTES) !=
      BIN_CONTENT_COMPARE_BYTES) {
    perror("write");
    goto error;
  }
  
  /* let subclass write its contents */
  if (!WriteCache(cacheFd))
    goto error;
  
  /* write last position in the file */
  if (write(cacheFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("write");
    goto error;
  }
  
  /* write # of records */
  if (write(cacheFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("write");
    goto error;
  }
    
  /* write indices */
  if (write(cacheFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("write");
    goto error;
  }

  close(cacheFd);

  _currPos = _data->Tell();

  return;
  
 error:
  if (fileOpened)
    close(cacheFd);
  (void)unlink(_cacheFileName);

  _currPos = _data->Tell();
}

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataBinary::BuildIndex()
{
#ifdef DEBUG
  printf("Entering BuildIndex\n");
#endif

  char physRec[_physRecSize];
  char recBuf[_recSize];
  int oldTotal = _totalRecs;
  
  // File has been appended, extend index
  if (_data->Seek(_lastPos, SEEK_SET) < 0) {
    perror("fseek");
    DOASSERT(0, "Cannot perform file seek");
  }

  _currPos = _lastPos;

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
#if 0
	printf("Ignoring invalid or non-matching record\n");
#endif
      }
    } else {
      printf("Ignoring incomplete record (%d bytes)\n", len);
    }

    _currPos += len;
  }

  // last position is > current position because TapeDrive advances
  // bufferOffset to the next block, past the EOF, when tape file
  // ends
  _lastPos = _data->Tell();
  DOASSERT(_lastPos >= _currPos, "Incorrect file position");

  _fileGrown = false;

  printf("Index for %s: %ld total records, %ld new\n", _alias,
	 _totalRecs, _totalRecs - oldTotal);
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
	 _indexSize + BIN_INDEX_ALLOC_INCREMENT);
#endif

  long *newIndex = new long[_indexSize + BIN_INDEX_ALLOC_INCREMENT];
  memcpy(newIndex, _index, _indexSize * sizeof(long));
  delete _index;
  _index = newIndex;
  _indexSize += BIN_INDEX_ALLOC_INCREMENT;
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");

  if (_totalRecs >= _indexSize)         // index buffer too small?
    ExtendIndex();                      // extend it

  _index[_totalRecs++] = _lastPos;
  int len = numRecs * _physRecSize;

  if (_data->append(buf, len) != len) {
    perror("tapewrite");
    DOASSERT(0, "Cannot append to tape");
  }
  _lastPos = _data->Tell();

  _currPos = _lastPos;
  _fileGrown = true;
  _totalRecs += numRecs;
}

void TDataBinary::WriteLine(void *rec)
{
  WriteRecs(0, 1, rec);
}

void TDataBinary::Cleanup()
{
  if (_data->isTape()) {
    _data->printStats();
    delete _data;
    _data = NULL;
  }
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
