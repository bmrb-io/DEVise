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
  Revision 1.10  1999/11/30 22:28:30  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.9  1997/12/23 23:35:42  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.8  1997/11/24 23:15:20  weaver
  Changes for the new ColorManager.

  Revision 1.7  1997/10/10 21:13:48  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.6  1997/07/03 01:53:49  liping
  changed query interface to TData from RecId to double

  Revision 1.5.10.1  1997/05/21 20:40:49  weaver
  Changes for new ColorManager

  Revision 1.5  1996/12/23 22:57:28  wenger
  Minor improvements to attrproj code.

  Revision 1.4  1996/12/03 20:31:34  jussi
  Updated to reflect new TData interface.

  Revision 1.3  1996/11/25 18:15:15  wenger
  Changes to non-indexed attrproj to match other changes to TData;
  changes to compile non-index attrproj and to get the rest of Devise
  to compile with that.

  Revision 1.2  1996/11/22 20:41:14  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.
 
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
#include "TDataSeqAscii.h"
#include "Exit.h"
#include "Util.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DataSourceBuf.h"
#include "DevError.h"
#include "DataSeg.h"
#include "QueryProc.h"
#include "DevError.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#endif

# define  _STREAM_COMPAT

static const int INDEX_ALLOC_INC = 10000; // allocation increment for index
static const int LINESIZE = 4096;         // maximum size of each line

/* We cache the first FILE_CONTENT_COMPARE_BYTES from the file.
   The next time we start up, this cache is compared with what's in
   the file to determine if they are the same file. */

static const int FILE_CONTENT_COMPARE_BYTES = 4096;


static char fileContent[FILE_CONTENT_COMPARE_BYTES];
static char indexFileContent[FILE_CONTENT_COMPARE_BYTES];
static char *   srcFile = __FILE__;

TDataSeqAscii::TDataSeqAscii(char *name, char *type, char *param, int recSize)
: TData(name, type, param, recSize)
{
    DO_DEBUG(printf("TDataSeqAscii::TDataSeqAscii(0x%p)(%s, %s, %s, %d)\n",
		    this, name, type, param, recSize));
    DO_DEBUG(printf("_data = 0x%p\n", _data));

    _fileOpen = true;
    if (!(_data->Open("r") == StatusOk)) {
	_fileOpen = false;
    }
    
    DataSeg::Set(NULL, NULL, 0, 0);
    
    _bytesFetched = 0;
    
    _lastPos = 0;
    _currPos = 0;
    _lastIncompleteLen = 0;

    _prevPos = 0;
    _prevRid = 0;

    _data->Seek(0, SEEK_SET);

    Dispatcher::Current()->Register(this, 10, AllState, false, -1);
}

TDataSeqAscii::~TDataSeqAscii()
{
#ifdef DEBUG
  printf("TDataSeqAscii destructor\n");
#endif

  if (_fileOpen)
    _data->Close();

  Dispatcher::Current()->Unregister(this);
}

Boolean TDataSeqAscii::CheckFileStatus()
{
  CheckDataSource();
  // see if file is (still) okay
  if (!_data->IsOk()) {
    // if file used to be okay, close it
    if (_fileOpen) {
      Dispatcher::Current()->Unregister(this);
      printf("Data stream %s is no longer available\n", _name);
      _data->Close();
      TData::InvalidateTData();
      _fileOpen = false;
    }
    Boolean old = DevError::SetEnabled(false);
    if (!(_data->Open("r") == StatusOk)) {
      (void)DevError::SetEnabled(old);
      return false;
    }
    (void)DevError::SetEnabled(old);
    printf("Data stream %s has become available\n", _name);
    _fileOpen = true;
    Dispatcher::Current()->Register(this, 10, AllState, false, -1);
  }

  return true;
}

int TDataSeqAscii::Dimensions(int *sizeDimension)
{
  return 0;
}

Boolean TDataSeqAscii::HeadID(RecId &recId)
{
  recId = 0;
  return true;
}

Boolean TDataSeqAscii::LastID(RecId &recId)
{
  reportErrNosys("LastID is not valid on non-indexed TDatas");
  return false;
}

TData::TDHandle TDataSeqAscii::InitGetRecs(Interval *interval, int &bytesleft,
                                           Boolean asyncAllowed,
                                           ReleaseMemoryCallback *callback
					   )
{
	if (!strcmp(interval->AttrName, "recId")){ //recId supported
                RecId lowId = (RecId)(interval->Low);
                RecId highId = (RecId)(interval->High);

#if defined(DEBUG)
  cout << " RecID lowID  = " << lowId << " highId " << highId << " order = "
       << order << endl;
#endif

  TDataRequest *req = new TDataRequest;
  DOASSERT(req, "Out of memory");

  req->nextVal = lowId;
  req->endVal = highId;
  req->relcb = callback;
  req->AttrName = interval->AttrName;
  req->granularity = interval->Granularity;

  // Do things similar to LimitRecords
  if (interval->NumRecs < (int)(highId - lowId + 1))
  {
        req->endVal = lowId + (interval->NumRecs) - 1;
  }

  bytesleft = (int)((req->endVal - req->nextVal + 1)) * RecSize();

  return req;}
	else 
	{
		cout << "TDataSeqAscii supports recId only.\n";
		exit (1);
	}
}

Boolean TDataSeqAscii::GetRecs(TDHandle req, void *buf, int bufSize,
                               Interval *interval, int &dataSize)
{
  DOASSERT(req, "Invalid request handle");
	
	if (!strcmp(req->AttrName, "recId")) { // recId stuff

#ifdef DEBUG
  printf("TDataSeqAscii::GetRecs buf = 0x%p\n", buf);
#endif

  interval->NumRecs = bufSize / _recSize;
  DOASSERT(interval->NumRecs, "Not enough record buffer space");

  if (req->nextVal > req->endVal)
    return false;
  
  int num = (int)(req->endVal) - (int)(req->nextVal) + 1;
  if (num < interval->NumRecs)
    interval->NumRecs = num;
  
  TD_Status status = ReadRec((RecId)(req->nextVal), interval->NumRecs, buf);
  if (status != TD_OK)
    return false;
 
  interval->Low = req->nextVal;
  dataSize = interval->NumRecs * _recSize;
  req->nextVal += interval->NumRecs;

  interval->High = interval->Low + interval->NumRecs - 1;
  interval->AttrName = req->AttrName;
  interval->Granularity = req->granularity;

  RecId HIGHId, LOWId;
  DOASSERT(HeadID(LOWId), "can not find HeadID");
  DOASSERT(LastID(HIGHId), "can not find LastID");
  if (LOWId < req->nextVal)
  {
        interval->has_left = true;
        interval->left_adjacent = interval->Low - 1;
  }
  else
        interval->has_left = false;

  if (HIGHId > interval->High)
  {
        interval->has_right = true;
        interval->right_adjacent = interval->High + 1;
  }
  else
        interval->has_right = false;

  
  _bytesFetched += dataSize;
  
  return true;}
	else
        {
                cout << "TDataSeqAscii: GetRecs deals with recId only.\n";
                reportErrNosys("Fatal error");//TEMP -- replace with better message
                exit(1);
        }
}

void TDataSeqAscii::DoneGetRecs(TDHandle req)
{
  DOASSERT(req, "Invalid request handle");

  delete req;
}

void TDataSeqAscii::Initialize()
{
  return;
}

void TDataSeqAscii::Checkpoint()
{
    printf("Cannot checkpoint %s\n", _name);
    return;
}

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataSeqAscii::BuildIndex()
{
}

/* Rebuild index */

void TDataSeqAscii::RebuildIndex()
{
}

TD_Status TDataSeqAscii::ReadRec(RecId id, int numRecs, void *buf)
{
#ifdef DEBUG
  printf("TDataSeqAscii::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  char line[LINESIZE];
  
  char *ptr = (char *) buf;
  Boolean valid;

  for (int i = 0; i < numRecs; i++) {

    int len;

    //fprintf(stdout, "In ReadRec id: %ld  numRecs: %d\n",id,numRecs);
    if (_prevRid + 1 == id + i) {
        // We're reading sequentially, _currPos is valid.
        _prevPos = _currPos;
        _prevRid = id + i;
    } else { 
        // Handle non-sequential case (scan)
        // (Also will happen for the first read of a file.)

        if (id + i == _prevRid) {
          // They want the last record again.
          if (_data->Seek(_prevPos, SEEK_SET) < 0) {
            perror("fseek");
            DOASSERT(0, "Cannot perform file seek");
          }
          _currPos = _prevPos;

        } else if (id + i < _prevRid) {
          if (_data->Seek(0, SEEK_SET) < 0) {
            perror("fseek");
            DOASSERT(0, "Cannot perform file seek");
          }

          _prevRid = 0;
          _prevPos = 0;
          _currPos = 0;
        }

        // Now read forward to find it.
        while (id + i > _prevRid) {

            if (_data->Fgets(line, LINESIZE) == NULL)
              return TD_EOF;

            len = strlen(line);

            if (len > 0 ) {
              DOASSERT(line[len - 1] == '\n', "Data record too long");
              line[len - 1] = '\0';
            }

              // Check for a valid record
            valid = Decode(ptr, _currPos, line);
            _currPos += len;

            if (valid) {
                _prevPos = _currPos;
                _prevRid++;
            }
        }

        // We are now at the record we want.
    }

    do {
        if (_data->Fgets(line, LINESIZE) == NULL)
          return TD_EOF;

        len = strlen(line);
    
        if (len > 0 ) {
          DOASSERT(line[len - 1] == '\n', "Data record too long");
          line[len - 1] = '\0';
        }
    
        valid = Decode(ptr, _currPos, line);
        _currPos += len;

    } while (!valid);

    ptr += _recSize;
  }

  return TD_OK;
}

void TDataSeqAscii::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");
}

void TDataSeqAscii::WriteLine(void *line)
{
  DOASSERT(!_data->isTape(), "Writing to tape not supported yet");
}

void TDataSeqAscii::Cleanup()
{
  Checkpoint();

  if (_data->isTape())
    _data->printStats();
}

void TDataSeqAscii::PrintIndices()
{
}
