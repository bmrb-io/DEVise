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
  Revision 1.9  1996/08/04 21:59:48  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.8  1996/06/24 19:42:11  jussi
  Cleaned up and removed unused code.

  Revision 1.7  1996/03/05 23:27:50  jussi
  Minor fix.

  Revision 1.6  1995/12/28 20:07:39  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 21:16:43  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 18:06:08  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/09/14 20:34:02  jussi
  Removed extraneous keyword 'virtual'.

  Revision 1.2  1995/09/05 22:14:46  jussi
  Added CVS header.
*/

#include "Exit.h"
#include "TDataAttr.h"
#include "GData.h"
#include "GDataRangeMap.h"
#include "UnixRecFile.h"
#include "Init.h"

//#define DEBUG

GData::GData(TData *tdata, char *fname, int recSize, int maxBuf)
: TData()
{
  _tdata = tdata;

  _recSize = recSize;
  Boolean trunc = true;       /* TRUE if we need to truncate file */
  _recFile = UnixRecFile::OpenFile(fname, recSize, trunc);

#if 0
  if (_recFile) {
    if (_recFile->GetModTime() < Init::ProgModTime() ||
	_recFile->GetModTime() < tdata->GetModTime()) {
      delete _recFile;
      trunc = true;
      _recFile = UnixRecFile::OpenFile(fname, recSize, trunc);
    }
  }
#endif

  if (!_recFile) {
    trunc = true;
    _recFile = UnixRecFile::CreateFile(fname, recSize);
  }
  DOASSERT(_recFile, "Cannot create GData file");

  _data = _recFile;
  _data->AddRef();

  char buf[256];
  sprintf(buf, "%s.Range", fname);
  _rangeMap = new GDataRangeMap(recSize, buf, trunc);
  _totalRecs = _recFile->NumRecs();
  if (maxBuf < 0)
    _recsLeft = -1;
  else
    _recsLeft = (maxBuf / recSize)- _totalRecs;
}

GData::~GData()
{
#ifdef DEBUG
  printf("GData destructor %s, 0x%p \n", GetName(), this);
#endif

  delete _rangeMap;
  // deleted by TData(): delete _recFile; 
}

int GData::Dimensions(int *sizeDimension)
{
  RecId id;
  if (_rangeMap->highID(id))
    sizeDimension[0] = id;
  else
    sizeDimension[0] = 0;
  return 1;
}

/* Return record size, or -1 if variable record size */
int GData::RecSize()
{
  return _recSize;
}

/* Return page size of TData, or -1 if no paging structure */
int GData::PageSize()
{
  return -1;
}

/* Return true if TData deletes records from the beginning
   due to limited disk/memory buffer. */
Boolean GData::HasDeletion()
{
  return 0;
}

Boolean GData::HasAppend()
{
  return true;
}

void GData::GetIndex(RecId id, int *&indices)
{
  static indexArray[1];
  indices = indexArray;
  indexArray[0] = id;
}

Boolean GData::HeadID(RecId &recId)
{
  return _tdata->HeadID(recId);
}

Boolean GData::LastID(RecId &recId)
{
  return _tdata->LastID(recId);
}

char *GData::GetName()
{
  return _recFile->GetName();
}

/**************************************************************
Init getting records.
***************************************************************/

void GData::InitGetRecs(RecId lowId, RecId highId, RecordOrder order)
{
#ifdef DEBUG
  printf("GData::InitGetRecs(%ld,%ld)\n", lowId, highId);
#endif

  _nextId = lowId;
  _highId = highId;
  _numRecs = highId - lowId + 1;
  _rec = _rangeMap->FindMaxLower(_nextId);
  if (!_rec) {
    fprintf(stderr, "GData::InitGetRecs(): %ld,%ld out of range\n",
	    lowId, highId);
    DOASSERT(0, "Invalid record range");
  }
}

Boolean GData::GetRecs(void *buf, int bufSize, RecId &startRid,
		       int &numFetched, int &dataFetched, void **recPtrs)
{
#ifdef DEBUG
  printf("GData::GetRecs bufSize %d, %ld recs left, nex Id %ld\n", 
	 bufSize, _numRecs, _nextId);
#endif

  if (_numRecs <= 0)
    return false;
	
  /* Get all the ranges we need to get */
  int num = bufSize / _recSize;
  DOASSERT(num > 0, "Invalid buffer size");
  
  if (num > (int)_numRecs)
    num = _numRecs;
  
  char *bufAddr = (char *)buf;
  
  /* Set return param and update internal vars */
  startRid = _nextId;
  numFetched = num;
  _numRecs -= num;
  dataFetched = numFetched *_recSize;
  
#ifdef DEBUG
  printf("%d fetched, %d bytes returned, %ld left\n",
	 numFetched, dataFetched, _numRecs);
#endif

  while (num > 0) {
    if (_nextId > _rec->tHigh) {
      /* finished this range. try next range */
      _rec = _rangeMap->NextRangeRec(_rec);
      DOASSERT(_rec, "Invalid record range");
      continue;
    }
    
    /* Get data in this range */
    if (_nextId < _rec->tLow || _nextId > _rec->tHigh)
      DOASSERT(0, "Invalid record range");
    
    int numToReturn = _rec->tHigh - _nextId + 1;
    if (numToReturn > num)
      numToReturn = num;
    RecId gId = _rec->gLow + (_nextId - _rec->tLow);

#ifdef DEBUG
    printf("getting gId %ld, num %d\n", gId, numToReturn);
#endif

    _recFile->ReadRec(gId, numToReturn, bufAddr);
    
    bufAddr += numToReturn * _recSize;
    num -= numToReturn;
    _nextId += numToReturn;
    
  }
  
  return true;
}

void GData::DoneGetRecs()
{
}

/* For writing records. Default: not implemented. */
void GData::WriteRecs(RecId startId, int numRecs, void *buf)
{
  GDataRangeMapRec *rec = _rangeMap->FindMaxLower(startId);
  RecId nextId = startId;
  char *curBuf = (char *)buf;
  while (numRecs > 0) {
    if (!rec || nextId < rec->tLow || nextId > rec->tHigh) {
      if (rec)
	fprintf(stderr,
		"GData::WriteRecs (%ld,%ld) into gap T(%ld,%ld),G(%ld,%ld)\n",
		startId, startId+numRecs-1,
		rec->tLow, rec->tHigh, rec->gLow, rec->gHigh);
      else
	fprintf(stderr, "GData::WriteRecs (%ld,%ld) into gap\n",
		startId, startId+numRecs-1);
      DOASSERT(0, "Invalid write request");
    }
    
#ifdef DEBUG
    printf("GData::WriteRecs: T:(%ld,%ld), G:(%ld,%ld)\n",
	   rec->tLow,rec->tHigh,rec->gLow,rec->gHigh);
#endif
    
    /* Write this record */
    RecId gId = rec->gLow + (nextId - rec->tLow);
    int numToWrite = rec->tHigh - nextId + 1;
    if (numToWrite > numRecs)
      numToWrite = numRecs;
    _recFile->WriteRec(gId, numToWrite, curBuf);
    
    numRecs -= numToWrite;
    curBuf += (numToWrite * _recSize);
    
    rec = _rangeMap->NextRangeRec(rec);
  }
}

/* Update info about which TData records have been converted into GData */
void GData::UpdateConversionInfo(RecId tLow, RecId tHigh,
				 void *firstRec, void *lastRec)
{
  int numRecs = tHigh - tLow + 1;
  if (_recsLeft > 0 && numRecs > _recsLeft)
    DOASSERT(0, "Invalid number of records");

  _rangeMap->InsertRange(tLow, tHigh, _totalRecs,
			 _totalRecs + numRecs - 1,
			 firstRec, lastRec);
  _totalRecs += numRecs;
  if (_recsLeft > 0) _recsLeft -= numRecs;
  
  /* update info about conversion to all callbacks */
  int index;
  for (index=_callbacks.InitIterator(); _callbacks.More(index); ) {
    GDataCallback *c = _callbacks.Next(index);
    c->Converted(tLow, tHigh);
  }
  _callbacks.DoneIterator(index);

#ifdef DEBUG
  printf("GData: %s update %ld,%ld, %d left\n",GetName(), tLow, tHigh,
	 _recsLeft);
#endif
}

void GData::GetRecPointers(RecId startId, int numRecs, void *buf, 
			   void **recPtrs)
{
  DOASSERT(0, "Invalid call");
}

void GData::InitConvertedIterator()
{
  _rangeMap->InitListIterator();
}

Boolean GData::NextRange(RecId &lowId, RecId &highId)
{
  GDataRangeMapRec *rec = _rangeMap->NextRec();
  if (!rec)
    return false;

  lowId = rec->tLow;
  highId = rec->tHigh;
  return true;
}

void GData::DoneConvertedIterator()
{
}

int GData::UserAttr(int attrNum)
{
  if (attrNum == GDataAttrNum)
    return 1;
  
  return TData::UserAttr(attrNum);
}

/* Get next unconverted range <= id.
   Return true if open ended (no high limit) */
Boolean GData::NextUnConverted(RecId id, RecId &low, RecId &high)
{
  return _rangeMap->NextUnprocessed(id,low,high);
}

void GData::PrintConverted()
{
  printf("GData converted: ");
  _rangeMap->Print();
}

void GData::RegisterCallback(GDataCallback *c)
{
  _callbacks.Append(c);
}

#if 0
void GData::Cleanup()
{
  printf("GData::Cleanup %s: 0x%p \n", GetName(),this);
  delete _recFile;
  _recFile = 0;
  delete _rangeMap;
  _rangeMap = 0;
}
#endif

int GData::GetModTime()
{
  return _recFile->GetModTime();
}

AttrList *GData::GetAttrList()
{
  return 0;
}
