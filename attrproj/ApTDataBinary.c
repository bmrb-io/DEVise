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
*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Parse.h"
#include "ApTDataBinary.h"
#include "Exit.h"
#include "Util.h"
#include "ApInit.h"

//#define DEBUG

TDataBinary::TDataBinary(char *name, int recSize) : TData()
{
  //TEMPTEMPDispatcher::Current()->Register(this);

  _name = name;
  _recSize = recSize;

  _file = fopen(name, "r");
  if (!_file) {
    fprintf(stderr, "Cannot open file '%s' ", name);
    perror("fopen");
    DOASSERT(0, "Cannot open file");
  }

  struct stat sbuf;
  if (fstat(fileno(_file), &sbuf) < 0) {
    perror("fstat");
    DOASSERT(0, "Cannot get file statistics");
  }
  
  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;

  _totalRecs = 0;
}

TDataBinary::~TDataBinary()
{
#ifdef DEBUG
  printf("TDataBinary destructor\n");
#endif

  //TEMPTEMPDispatcher::Current()->Unregister(this);
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
  
  if (_currPos > _lastPos) {
    _lastPos = _currPos;
    if (_lastPos % _recSize) {
      fprintf(stderr,
	      "Partial records in binary file: size %ld, record size %d\n",
	      _lastPos, _recSize);
      DOASSERT(0, "Partial records in binary file");
    }
    _totalRecs = _lastPos / _recSize;
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
  struct stat sbuf;
  if (fstat(fileno(_file), &sbuf) < 0) {
    perror("fstat");
    DOASSERT(0, "Cannot get file statistics");
  }
  return (long)sbuf.st_mtime;
}

void TDataBinary::Initialize()
{
  _lastPos = 0;
  _totalRecs = 0;

  RecId recid;
  LastID(recid);
}

void TDataBinary::Checkpoint()
{
}

void TDataBinary::ReadRec(RecId id, int numRecs, void *buf)
{
#ifdef DEBUG
  printf("TDataBinary::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  long recloc = id * _recSize;
  if (_currPos != recloc) {
    if (fseek(_file, recloc, SEEK_SET) < 0) {
      perror("fseek");
      DOASSERT(0, "Cannot perform file seek");
    }
    _currPos = recloc;
  }

  if (fread(buf, numRecs * _recSize, 1, _file) != 1) {
    perror("fread");
    DOASSERT(0, "Cannot read from file");
  }
  _currPos += numRecs * _recSize;
  
  char *ptr = (char *)buf;
  for(int i = 0; i < numRecs; i++) {
    Boolean valid = Decode(id + i, ptr, ptr);
    DOASSERT(valid, "Inconsistent validity flag");
    ptr += _recSize;
  }
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  if (fseek(_file, _lastPos, SEEK_SET) < 0) {
    perror("fseek");
    DOASSERT(0, "Cannot perform file seek");
  }

  if (fwrite(buf, numRecs * _recSize, 1, _file) != 1) {
    perror("fwrite");
    DOASSERT(0, "Cannot write to file");
  }

  _currPos = _lastPos = ftell(_file);
  _totalRecs += numRecs;
}

void TDataBinary::WriteLine(void *rec)
{
  WriteRecs(0, 1, rec);
}

void TDataBinary::Cleanup()
{
}
