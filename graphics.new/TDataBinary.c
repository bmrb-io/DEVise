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

TDataBinary::TDataBinary(char *name, int recSize, int physRecSize)
{
  _name = name;
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

  if (_currPos > _lastPos) {
    _lastPos = _currPos;
    if (_lastPos % _physRecSize) {
      fprintf(stderr,
	      "Partial records in binary file: size %ld, record size %d\n",
	      _lastPos, _physRecSize);
      DOASSERT(0, "Partial records in binary file");
    }

    long oldTotal = _totalRecs;
    _totalRecs = _lastPos / _physRecSize;

    // scan records in order to get hi/lo values for each attribute
    for(long rid = oldTotal; rid < _totalRecs; rid++) {
      char buf[_recSize];
      ReadRec((RecId)rid, 1, buf);
    }

    printf("%s: %ld total records, %ld new\n", _name,
	   _totalRecs, _totalRecs - oldTotal);
  }

  _fileGrown = false;

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

char *TDataBinary::MakeCacheName(char *file)
{
  char *fname = StripPath(file);
  unsigned int nameLen = strlen(Init::WorkDir()) + strlen(fname) + 8;
  char *name = new char[nameLen];
  sprintf(name, "%s/%s.cache", Init::WorkDir(), fname);
  DOASSERT(strlen(name) < nameLen, "Name too long");
  return name;
}

void TDataBinary::Initialize()
{
  RecId recid;

  _cacheFileName = MakeCacheName(_name);

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

  close(cacheFd);
  
  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  LastID(recid);

  return;

 error:
  /* recover frome error by building index from scratch  */
  if (fileOpened)
    close(cacheFd);

  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;

  LastID(recid);
}

void TDataBinary::Checkpoint()
{
  printf("Checkpointing %s: %ld total records, %ld new\n", _name,
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

void TDataBinary::ReadRec(RecId id, int numRecs, void *buf)
{
#ifdef DEBUG
  printf("TDataBinary::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  long recloc = id * _physRecSize;

  if (_tape) {
    if (_tape->seek(recloc) != recloc) {
      perror("tapeseek");
      DOASSERT(0, "Cannot perform seek on tape drive");
    }
  } else {
    if (_currPos != recloc) {
      if (fseek(_file, recloc, SEEK_SET) < 0) {
	perror("fseek");
	DOASSERT(0, "Cannot perform file seek");
      }
    }
  }
  _currPos = recloc;

  for(int i = 0; i < numRecs; i++) {

    char *ptr = ((char *)buf) + i * _recSize;

    if (_tape) {
      if (_tape->read(ptr, _physRecSize) != _physRecSize)
	DOASSERT(0, "Cannot read from tape");
    } else {
      if (fread(ptr, _physRecSize, 1, _file) != 1) {
	perror("fread");
	DOASSERT(0, "Cannot read from file");
      }
    }

    Boolean valid = Decode(id + i, ptr, ptr);
    DOASSERT(valid, "Inconsistent validity flag");
  }

  _currPos += numRecs * _physRecSize;
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_tape, "Writing to tape not supported yet");

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
