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

//#define DEBUG
#include <assert.h>
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
#include "TDataDQL.h"
#include "Exit.h"
#include "Util.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DataSourceBuf.h"
#include "DevError.h"
#include "DataSeg.h"
#include "QueryProc.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#endif

# define  _STREAM_COMPAT

TDataDQL::TDataDQL(char *name, char *type, char *query)
: TData(name, type, query, 8)
{
  
  //Do Nothing ..
  _totalRecs = 2000;
  // Computer the record size.. 
  TData::_recSize = 8;

}

TDataDQL::~TDataDQL()
{
#ifdef DEBUG
  printf("TDataDQL destructor\n");
#endif

}

Boolean TDataDQL::CheckFileStatus()
{
  return true;
}

int TDataDQL::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

Boolean TDataDQL::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

Boolean TDataDQL::LastID(RecId &recId)
{
  // Find the last record 
  recId = 1999;
  return (_totalRecs > 0);
}

void TDataDQL::InitGetRecs(RecId lowId, RecId highId,RecordOrder order)
{
  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  _lowId = lowId;
  _highId = highId;
  _nextId = lowId;
  _endId = highId;
}

Boolean TDataDQL::GetRecs(void *buf, int bufSize, 
			    RecId &startRid,int &numRecs, int &dataSize,
			    void **recPtrs)
{
#ifdef DEBUG
  printf("TDataDQL::GetRecs buf = 0x%p\n", buf);
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

void TDataDQL::GetRecPointers(RecId startId, int numRecs,
				void *buf, void **recPtrs)
{
  DOASSERT(0, "Feature not implemented");
}

void TDataDQL::GetIndex(RecId id, int *&indices)
{
  static int index[1];
  index[0] = id;
  indices = index;
}

int TDataDQL::GetModTime()
{
  if (!CheckFileStatus())
    return -1;

  return _data->GetModTime();
}

char *TDataDQL::MakeIndexFileName(char *name, char *type)
{
  return 0;
}

void TDataDQL::Initialize()
{
 // Probably do all the things here..

}

void TDataDQL::Checkpoint()
{
}

void TDataDQL::InvalidateTData()
{
    TData::InvalidateTData();
}


/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataDQL::BuildIndex()
{

 // Find the last record, total Recs,...
 _totalRecs = 2000;

}

void TDataDQL::ReadRec(RecId id, int numRecs, void *buf)
{
 // Read the record..
 // And output it...
	
 for(int i = id  ; i < numRecs+id; i++){
	
	//printf(" DQL:Writing Record %d = %d,10\n",i,i);
	memcpy(buf,&i,sizeof(int));
 	buf += sizeof(int);
	int t = 10;
 	memcpy(buf,&t,sizeof(int));
	buf += sizeof(int);
 }
}

void TDataDQL::WriteRecs(RecId startRid, int numRecs, void *buf)
{
 printf(" Failing Write Recs not supported \n");
 assert(0); 
}

void TDataDQL::WriteLine(void *line)
{
 printf(" Failing Write Recs not supported \n");
 assert(0); 
}

void TDataDQL::Run()
{
}

void TDataDQL::Cleanup()
{
}

void TDataDQL::PrintIndices()
{
}

