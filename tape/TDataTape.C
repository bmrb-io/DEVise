/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/10/15 18:45:01  jussi
  Added HPUX-specific code.

  Revision 1.3  1995/09/22 15:46:15  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:47  jussi
  Added CVS header.
*/

#include <iostream.h>
#include <assert.h>
#include <stdio.h>
#include <libc.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Parse.h"
#include "TDataTape.h"
#include "Exit.h"
#include "Util.h"
#include "Init.h"

#ifndef HPUX
extern "C" int bcmp(char *b1, char *b2, int length);
#endif

static char fileContent[TAPE_CONTENT_COMPARE_BYTES];
static char cachedFileContent[TAPE_CONTENT_COMPARE_BYTES];

static char *MakeCacheName(char *file)
{
  char *fname = StripPath(file);
  size_t nameLen = strlen(Init::WorkDir()) + strlen(fname) + 8;
  char *name = new char[nameLen];
  sprintf(name, "%s/%s.cache", Init::WorkDir(), fname);
  assert(strlen(name) < nameLen);
  return name;
}

TDataTape::TDataTape(char *name, int recSize)
{
  Dispatcher::Current()->Register(this);
   
  _name = name;
  _recSize = recSize;

  _tape = new TapeDrive(name, "r");
  if (!_tape || !*_tape)
    Exit::DoExit(1);

  _totalRecs = 0;
  _recBuf = new char [recSize];
  _index = new long [TAPE_INIT_INDEX_SIZE];
  _indexSize = TAPE_INIT_INDEX_SIZE;
  _fileGrown = false;
}

TDataTape::~TDataTape()
{
  Dispatcher::Current()->Unregister(this);
  delete _tape;
}

int TDataTape::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

Boolean TDataTape::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

Boolean TDataTape::LastID(RecId &recId)
{
  // see if tape file has grown
  if (_fileGrown)
    BuildIndex();

  recId = _totalRecs-1;
  return (_totalRecs > 0);
}

void TDataTape::InitGetRecs(RecId lowId, RecId highId, RecordOrder order)
{
  if ((long)lowId >= _totalRecs ||	
      (long)highId >= _totalRecs || highId < lowId) {
    fprintf(stderr,"TDataTape::GetRecs: invalid recId %ld %ld, total %ld\n",
	    lowId, highId, _totalRecs);
    Exit::DoExit(1);
  }

  _lowId = lowId;
  _highId = highId;
  _nextId = lowId;
  _endId = highId;
}

Boolean TDataTape::GetRecs(void *buf, int bufSize, 
			   RecId &startRid, int &numRecs,
			   int &dataSize, void **recPtrs)
{
  numRecs = bufSize/_recSize;
  if (!numRecs) {
    fprintf(stderr,"TDataRec::GetRecs(): not enough buffer space \n");
    fprintf(stderr,"buffer size = %d, rec size= %d\n", bufSize, _recSize);
    Exit::DoExit(1);
  }

  if (_nextId > _endId)
    return false;

  int num = _endId - _nextId+1;
  if (num < numRecs)
    numRecs = num;

  ReadRec(_nextId, numRecs, buf);

  startRid = _nextId;
  dataSize = numRecs * _recSize;
  _nextId += numRecs;

  return true;
}

void TDataTape::GetRecPointers(RecId startId, int numRecs,
			       void *buf, void **recPtrs)
{
  fprintf(stderr,"TDataRec::GetRecPointers: not implemented\n");
  Exit::DoExit(1);
}

void TDataTape::GetIndex(RecId id, int *&indices)
{
  static int index[1];
  index[0] = id;
  indices = index;
}


int TDataTape::GetModTime()
{
  return 0;
}

void TDataTape::Initialize()
{
  int indexErrors = 0;
  int i = 0;

  // Read file contents into buffer
  _cacheFileName = MakeCacheName(_name);

  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_RDONLY, 0)) < 0)
    goto error;
  fileOpened = true;

  // cache file exists. See if we are still working on the same
  // file, and if we are, reinitialize

  if (_tape->seek(0) != 0)
    goto error;

  if (read(cacheFd,cachedFileContent, TAPE_CONTENT_COMPARE_BYTES)
      != TAPE_CONTENT_COMPARE_BYTES  ||
      _tape->read(fileContent, TAPE_CONTENT_COMPARE_BYTES)
      != TAPE_CONTENT_COMPARE_BYTES ||
      bcmp(cachedFileContent, fileContent, TAPE_CONTENT_COMPARE_BYTES) != 0)
    goto error;

  // Assume identical file

  // Read last file position
  if (lseek(cacheFd,TAPE_LAST_POS_OFFSET,SEEK_SET) != TAPE_LAST_POS_OFFSET)
    goto error;
  if (read(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
    goto error;

  // Read number of records
  if (lseek(cacheFd,TAPE_TOTAL_RECS_OFFSET,SEEK_SET) != TAPE_TOTAL_RECS_OFFSET)
    goto error;
  if (read(cacheFd,&_totalRecs, sizeof(_totalRecs)) != sizeof(_totalRecs))
    goto error;

  if (_totalRecs >= _indexSize) {
    delete _index;
    _index = new long[_totalRecs + TAPE_INDEX_ALLOC_INC];
    _indexSize = _totalRecs + TAPE_INDEX_ALLOC_INC;
  }
  // read the index
  if (read(cacheFd,_index, _totalRecs * sizeof(long))
      != (long)(_totalRecs * sizeof(long)))
    goto error;

  for(i = 1; i < _totalRecs; i++) {
    if (_index[i - 1] > _index[i]) {
      printf("index[%d] = %ld > index[%d] = %ld\n",
	     i - 1, _index[i - 1], i, _index[i]);
      ++indexErrors;
    }
  }
  assert(!indexErrors);

  // Let subclass read cache
  if (lseek(cacheFd,TAPE_SUBCLASS_OFFSET,SEEK_SET) != TAPE_SUBCLASS_OFFSET)
    goto error;

  // Can't read cache?
  if (!ReadCache(cacheFd))
    goto error;

  close(cacheFd);

  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  printf("Index cache seems to be okay.\n");

  // continue to build index
#ifdef HACKHACKHACK
  BuildIndex();
#endif
  return;

 error:
  // recover frome error by building index from scratch 
  if (fileOpened)
    close(cacheFd);
  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;
  BuildIndex();
}

void TDataTape::Checkpoint()
{
  printf("Checkpointing %s to file %s, %ld existing, %ld additional records\n",
	 _name, _cacheFileName, _initTotalRecs, _totalRecs - _initTotalRecs);

  if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
    return;

  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_CREAT| O_RDWR,
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
		      | S_IROTH | S_IWOTH)) < 0)
    goto error;
  fileOpened = true;

  if (_tape->seek(0) != 0)
    goto error;
  if (_tape->read(fileContent, TAPE_CONTENT_COMPARE_BYTES)
      != TAPE_CONTENT_COMPARE_BYTES)
    goto error;

  // write contents of file to be compared later
  if (write(cacheFd, fileContent, TAPE_CONTENT_COMPARE_BYTES) !=
      TAPE_CONTENT_COMPARE_BYTES)
    goto error;
	
  // subclass should write its contents
  if (lseek(cacheFd, TAPE_SUBCLASS_OFFSET, SEEK_SET) != TAPE_SUBCLASS_OFFSET)
    goto error;
  // let subclass write
  if (!WriteCache(cacheFd))
    goto error;

  // write last position in the file
  if (_lastPos > _initLastPos) {
    if (lseek(cacheFd, TAPE_LAST_POS_OFFSET,SEEK_SET)
	!= TAPE_LAST_POS_OFFSET)
      goto error;
    if (write(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
      goto error;
  }
	
  // write # of records
  if (_totalRecs > _initTotalRecs) {
    if (lseek(cacheFd, TAPE_TOTAL_RECS_OFFSET,SEEK_SET)
	!= TAPE_TOTAL_RECS_OFFSET)
      goto error;
    if (write(cacheFd, &_totalRecs, sizeof(_totalRecs))!=sizeof(_totalRecs))
      goto error;

    // write indices
    int indexOffset = TAPE_INDEX_OFFSET + _initTotalRecs * sizeof(long);
    if (lseek(cacheFd,indexOffset, SEEK_SET) != indexOffset)
      goto error;
    int bytes = (_totalRecs - _initTotalRecs) * sizeof(long);
    if (write(cacheFd, &_index[_initTotalRecs], bytes) != bytes)
      goto error;
  }
  close(cacheFd);
  return;

error:
  perror("checkpoint");
  if (fileOpened)
    close(cacheFd);
  (void)unlink(_cacheFileName);
}

void TDataTape::BuildIndex()
{
  char buf[TAPE_LINESIZE];
  long oldTotal = _totalRecs;

  // File has been appended, extend index
  if (_tape->seek(_lastPos) != _lastPos)
    Exit::DoExit(1);

  while(1) {
    long pos = _tape->tell();
    int bytes = _tape->gets(buf,TAPE_LINESIZE);
  
    if (!bytes)
      break;
    if (bytes < 0)
      Exit::DoExit(1);
    if (buf[bytes - 1] != '\n') {
      fprintf(stderr, "TDataTape::BuildIndex: line too long\n%s\n", buf);
      Exit::DoExit(1);
    }

    if (IsValid(buf)) {
      if (_totalRecs >= _indexSize)     // index buffer too small?
	ExtendIndex();                  // extend it
      _index[_totalRecs++] = pos;
    }
  }

  _lastPos = _tape->tell();
  _fileGrown = false;

  printf("\nExtend Index for %s from %ld to %ld records\n", 
	 _name, oldTotal, _totalRecs);
}

void TDataTape::ReadRec(RecId id, int numRecs, void *buf)
{
  char line[TAPE_LINESIZE];

  if ((id < 3) ||
      (id > 9997 && id < 10001) ||
      (id > 149997 && id < 150001))
    printf("Accessing record %ld\n", id);

  char *ptr = (char *)buf;
  for(int i = 0; i < numRecs; i++) {
    if (_tape->seek(_index[id + i]) != _index[id + i])
      Exit::DoExit(1);
    int bytes = _tape->gets(line, TAPE_LINESIZE);
    if (bytes <= 0)
      Exit::DoExit(1);
    if (line[bytes - 1] != '\n') {
      fprintf(stderr, "TDataTape::ReadRec: line too long\n%s\n", line);
      Exit::DoExit(1);
    }
    Boolean valid = Decode(id + i, ptr, line);
    assert(valid);
    ptr += _recSize;
  }
}

void TDataTape::ExtendIndex()
{
  long *newIndex= new long[_indexSize + TAPE_INDEX_ALLOC_INC];
  memcpy(newIndex, _index, _indexSize * sizeof(long));
  delete _index;
  _index = newIndex;
  _indexSize += TAPE_INDEX_ALLOC_INC;
}

void TDataTape::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  fprintf(stderr, "Writing to tape not supported yet\n");
  Exit::DoExit(1);

  if (_totalRecs >= _indexSize)         // index buffer too small?
    ExtendIndex();                      // extend it

  _index[_totalRecs++] = _lastPos;
  int len = strlen((char *)buf);
  if (_tape->append(buf, len) != len)
    Exit::DoExit(1);

  _lastPos = _tape->tell();
  _fileGrown = true;
}

void TDataTape::WriteLine(void *line)
{
  int len = strlen((char *)line);
  if (_tape->append(line, len) != len)
    Exit::DoExit(1);

  _lastPos = _tape->tell();
  _fileGrown = true;
}

void TDataTape::Cleanup()
{
  Checkpoint();
  _tape->printStats();
  delete _tape;
}

void TDataTape::PrintIndices()
{
  int cnt = 0;
  for(int i = 0; i < _totalRecs; i++) {
    printf("%ld ", _index[i]);
    if (cnt++ == 10) {
      printf("\n");
      cnt = 0;
    }
  }
  printf("\n");
}
