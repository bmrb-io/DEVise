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

//#define DEBUG

static char fileContent[BIN_CONTENT_COMPARE_BYTES];
static char cachedFileContent[BIN_CONTENT_COMPARE_BYTES];

TDataBinary::TDataBinary(char *name, char *alias, int recSize, int physRecSize)
{
  _name = name;
  _alias = alias;
  _recSize = recSize;
  _physRecSize = physRecSize;

  _file = NULL;
  _tape = NULL;

  if (!strncmp(name, "/dev/rmt", 8) || !strncmp(name, "/dev/nrmt", 9)
      || !strncmp(name, "/dev/rst", 8) || !strncmp(name, "/dev/nrst", 9)) {
    _tape = new TapeDrive(name, "r");
    if (!_tape || !*_tape) {
      fprintf(stderr, "Cannot open tape '%s': ", name);
      perror("fopen");
      DOASSERT(0, "Cannot open tape drive");
    }
  } else {
    _file = fopen(name, "r");
    if (!_file) {
      fprintf(stderr, "Cannot open file '%s' ", name);
      perror("fopen");
      DOASSERT(0, "Cannot open file");
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

  if (_tape)
    delete _tape;
  else
    fclose(_file);

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
  if (_tape) {
    if (_fileGrown) {
      // seek to EOF
      long end = 1024 * 1024 * 1024;
      if (_tape->seek(end) >= end)
	DOASSERT(0, "Could not seek to end of tape file");
      _currPos = _tape->tell();
    }
  } else {
    // see if file has grown
      if (fseek(_file, 0, SEEK_END) < 0) {
	perror("fseek");
	DOASSERT(0, "Cannot perform file seek");
      }
    _currPos = ftell(_file);
    if (_currPos < 0) {
      perror("ftell");
      DOASSERT(0, "Cannot get current file position");
    }
  }

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
  struct stat sbuf;
  if (fstat(fileno(_file), &sbuf) < 0) {
    perror("fstat");
    DOASSERT(0, "Cannot get file statistics");
  }
  return (long)sbuf.st_mtime;
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
  if (_tape) {
    if (_tape->seek(0) != 0) {
      perror("tapeseek");
      goto error;
    }
    if (_tape->read(fileContent, BIN_CONTENT_COMPARE_BYTES)
	!= BIN_CONTENT_COMPARE_BYTES) {
      perror("taperead");
      goto error;
    }
  } else {
    if (fseek(_file, 0, SEEK_SET) < 0) {
      perror("fseek");
      goto error;
    }
    if (fread(fileContent, BIN_CONTENT_COMPARE_BYTES, 1, _file) != 1) {
      perror("fread");
      goto error;
    }
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

  if (_tape) {
    if (_tape->seek(0) != 0) {
      perror("tapeseek");
      goto error;
    }
    if (_tape->read(fileContent, BIN_CONTENT_COMPARE_BYTES)
	!= BIN_CONTENT_COMPARE_BYTES) {
      perror("taperead");
      goto error;
    }
  } else {
    if (fseek(_file, 0, SEEK_SET) < 0) {
      perror("fseek");
      goto error;
    }
    if (fread(fileContent, BIN_CONTENT_COMPARE_BYTES, 1, _file) != 1) {
      if (!errno)
	fprintf(stderr, "File not checkpointed due to its small size\n");
      else
	perror("fread");
      goto error;
    }
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

  if (_tape)
    _currPos = _tape->tell();
  else
    _currPos = ftell(_file);

  return;
  
 error:
  if (fileOpened)
    close(cacheFd);
  (void)unlink(_cacheFileName);

  if (_tape)
    _currPos = _tape->tell();
  else
    _currPos = ftell(_file);
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
  
  if (_tape) {
    // File has been appended, extend index
    if (_tape->seek(_lastPos) != _lastPos) {
      perror("tapeseek");
      DOASSERT(0, "Cannot perform seek on tape drive");
    }
  } else {
    // File has been appended, extend index
    if (fseek(_file, _lastPos, SEEK_SET) < 0) {
      perror("fseek");
      DOASSERT(0, "Cannot perform file seek");
    }
  }

  _currPos = _lastPos;

  while(1) {

    int len = 0;

    if (_tape)
      len = _tape->read(physRec, _physRecSize);
    else
      len = fread(physRec, 1, _physRecSize, _file);
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

  if (_tape) {
    // last position is > current position because TapeDrive advances
    // bufferOffset to the next block, past the EOF, when tape file
    // ends
    _lastPos = _tape->tell();
    DOASSERT(_lastPos >= _currPos, "Incorrect file position");
  } else {
    _lastPos = ftell(_file);
    DOASSERT(_lastPos == _currPos, "Incorrect file position");
  }

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

    if (_tape) {
      if (_tape->seek(recloc) != recloc) {
	perror("tapeseek");
	DOASSERT(0, "Cannot perform seek on tape drive");
      }
      _currPos = recloc;
      if (_tape->read(ptr, _physRecSize) != _physRecSize)
	DOASSERT(0, "Cannot read from tape");
    } else {
      if (_currPos != recloc) {
	if (fseek(_file, recloc, SEEK_SET) < 0) {
	  perror("fseek");
	  DOASSERT(0, "Cannot perform file seek");
	}
	_currPos = recloc;
      }
      if (fread(ptr, _physRecSize, 1, _file) != 1) {
	perror("fread");
	DOASSERT(0, "Cannot read from file");
      }
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
  DOASSERT(!_tape, "Writing to tape not supported yet");

  if (_totalRecs >= _indexSize)         // index buffer too small?
    ExtendIndex();                      // extend it

  _index[_totalRecs++] = _lastPos;
  int len = numRecs * _physRecSize;

  if (_tape) {
    if (_tape->append(buf, len) != len) {
      perror("tapewrite");
      DOASSERT(0, "Cannot append to tape");
    }
    _lastPos = _tape->tell();
  } else {
    if (fseek(_file, _lastPos, SEEK_SET) < 0) {
      perror("fseek");
      DOASSERT(0, "Cannot perform file seek");
    }
    if (fwrite(buf, len, 1, _file) != 1) {
      perror("fwrite");
      DOASSERT(0, "Cannot write to file");
    }
    _lastPos = ftell(_file);
  }

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
  if (_tape) {
    _tape->printStats();
    delete _tape;
    _tape = NULL;
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
