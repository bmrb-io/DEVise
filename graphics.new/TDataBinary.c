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
  Revision 1.1  1996/01/23 20:54:49  jussi
  Initial revision.
*/

#include <iostream.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Parse.h"
#include "TDataBinary.h"
#include "Exit.h"
#include "Util.h"
#include "Init.h"

#define DEBUG

TDataBinary::TDataBinary(char *name, int recSize) : TData()
{
  Dispatcher::Current()->Register(this);

  _name = name;
  _recSize = recSize;

  _file = fopen(name, "r");
  if (_file == NULL) {
    fprintf(stderr,"TDataBinary: cannot open file '%s' ", name);
    perror("fopen");
    Exit::DoExit(1);
  }

  struct stat sbuf;
  if (fstat(fileno(_file), &sbuf) < 0) {
    perror("fstat");
    Exit::DoExit(1);
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

  Dispatcher::Current()->Unregister(this);
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
    perror("TDAtaAscii::LastID");
    Exit::DoExit(1);
  }
  _currPos = ftell(_file);
  if (_currPos < 0) {
    perror("TDataAscii::Init:ftell");
    Exit::DoExit(1);
  }
  
  if (_currPos > _lastPos) {
    _lastPos = _currPos;
    if (_lastPos % _recSize) {
      fprintf(stderr,
	      "Partial records in binary file: size %ld, record size %d\n",
	      _lastPos, _recSize);
      Exit::DoExit(2);
    }
    _totalRecs = _lastPos / _recSize;
  }

  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

void TDataBinary::InitGetRecs(RecId lowId, RecId highId, RecordOrder order)
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

Boolean TDataBinary::GetRecs(void *buf, int bufSize, 
			     RecId &startRid, int &numRecs, int &dataSize,
			     void **recPtrs)
{
#ifdef DEBUG
  printf("TDataBinary::GetRecs buf = 0x%p\n", buf);
#endif

  numRecs = bufSize / _recSize;
  if (!numRecs) {
    fprintf(stderr, "TDataRec::GetRecs(): not enough buffer space \n");
    fprintf(stderr, "buffer size = %d, rec size= %d\n", bufSize, _recSize);
    Exit::DoExit(1);
  }

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
  fprintf(stderr,"TDataRec::GetRecPointers: not implemented\n");
  Exit::DoExit(1);
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
    fprintf(stderr,"TDataBinary::can't find stat\n");
    Exit::DoExit(1);
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
      fprintf(stderr, "TDataBinary::ReadRec(%ld,%d,0x%p) seek\n",
	      id, numRecs, buf);
      perror("fseek");
    Exit::DoExit(1);
    }
    _currPos = recloc;
  }

  if (fread(buf, numRecs * _recSize, 1, _file) != 1) {
    perror("fread");
    Exit::DoExit(1);
  }
  _currPos += numRecs * _recSize;
  
  char *ptr = (char *)buf;
  for(int i = 0; i < numRecs; i++) {
    Boolean valid = Decode(id + i, ptr, ptr);
    assert(valid);
    ptr += _recSize;
  }
}

void TDataBinary::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  if (fseek(_file, _lastPos, SEEK_SET) < 0) {
    perror("TDataBinary::WriteRecs");
    Exit::DoExit(1);
  }

  if (fwrite(buf, numRecs * _recSize, 1, _file) != 1) {
    perror("TDataBinary::WriteRecs");
    Exit::DoExit(1);
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
