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
  Revision 1.25  1999/05/21 14:52:44  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.24  1998/05/06 22:04:57  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.23  1998/03/04 19:11:03  wenger
  Fixed some more dynamic memory errors.

  Revision 1.22  1998/02/20 06:17:13  beyer
  resurected histograms

  Revision 1.21  1997/05/28 15:39:29  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.20.4.1  1997/05/20 16:11:15  ssl
  Added layout manager to DEVise

  Revision 1.20  1997/04/21 22:56:49  guangshu
  Small changes.

  Revision 1.19  1997/03/20 22:18:04  guangshu
  changed GDATASTAT to GDATASTAT_X AND GDATASTAT_Y

  Revision 1.18  1997/03/07 17:44:20  donjerko
  *** empty log message ***

  Revision 1.17  1997/02/03 19:45:35  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.16  1997/01/24 01:56:59  wenger
  Fixed bug 068 (major overhaul of DataSourceSegment class).

  Revision 1.15  1996/12/18 21:13:28  jussi
  Added code that instantiates the query processor/buffer manager
  so that forked DataSource processes can inherit shared memory
  and semaphore from the parent process.

  Revision 1.14  1996/12/03 20:32:09  jussi
  Improved Init/Get/Done interface.

  Revision 1.13  1996/11/23 21:16:01  jussi
  Fixed detection of tape drive device.

  Revision 1.12  1996/11/18 18:10:53  donjerko
  New files and changes to make DTE work with Devise

  Revision 1.11  1996/11/01 19:28:23  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.10  1996/10/07 22:53:59  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.9  1996/08/23 16:56:20  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.8  1996/08/15 19:54:31  wenger
  Added 'pure' targets for attrproj and devread; fixed some dynamic
  memory problems.  Found some bugs while demo'ing for soils science
  people.

  Revision 1.7  1996/08/12 20:49:58  jussi
  Added missing statement that creates a new DataSourceWeb for
  WWW data sources.

  Revision 1.6  1996/08/05 19:48:57  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.5  1996/08/04 21:59:51  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.4  1996/07/23 20:13:05  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.3  1996/01/27 00:17:27  jussi
  Added copyright notice and cleaned up a bit.

  Revision 1.2  1995/09/05 22:15:44  jussi
  Added CVS header.
*/

//#define DEBUG

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "TData.h"
#include "Exit.h"
#include "Util.h"
#include "DevFileHeader.h"
#include "DevError.h"
#include "AttrList.h"

#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceTape.h"
#include "DataSourceBuf.h"
#include "DataSourceDQL.h"
#include "DataSeg.h"
#include "FileIndex.h"
#include "DepMgr.h"

#ifndef ATTRPROJ
#include "ViewGraph.h"
#include "QueryProc.h"
#endif

#ifndef ATTRPROJ
#include "DataSourceWeb.h"
#endif

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#endif

static DevStatus WriteString(int fd, char *string);

/*---------------------------------------------------------------------------*/
TData::TData(char* name, char* type, char* param, int recSize)
{
    DO_DEBUG(printf("TData::TData(%s, %s, %s, %d)\n",
		    name, type, param, recSize));

#ifndef ATTRPROJ
    /*
       Make sure query processor/buffer manager/memory manager exist
       at this time, because creating a data source below may invoke
       fork(), and we want the forked process to have access to the
       shared memory segments and semaphores of the parent process.
    */
    (void)QueryProc::Instance();
#endif

    _name = name;
    _type = type;
    _param = param;
    _recSize = recSize;
    _data = NULL;
    _version = 0;
    _goHomeOnInvalidate = true;

    // Find out whether the data occupies an entire data source or only
    // a segment of it
    char *	segLabel;
    char *	segFile;
    long	segOffset;
    long	segLength;

    DataSeg::Get(segLabel, segFile, segOffset, segLength);

    // Check that data segment label matches TData name

    if (strcmp(_name, segLabel)) {
	cout << (void*) segLabel << " " << segLabel[0] << " " << segLabel[1] << endl; 
	cout << _name << " != " << segLabel << endl;
	cout << flush;
	DOASSERT(false, "Data segment does not match tdata");
    }

    // Now instantiate the appropriate type of source, according to
    // whether this is a tape, disk file, or Web resource, and whether
    // or not the data occupies the entire file.

    if (!strcmp(_type, "UNIXFILE")) {
	char *file = _param;
	if (strcmp(file, segFile)) {
	    DOASSERT(false, "Data segment does not match tdata");
	}
	if (   !strncmp(file, "/dev/rmt", 8)
	    || !strncmp(file, "/dev/nrmt", 9)
	    || !strncmp(file, "/dev/rst", 8)
	    || !strncmp(file, "/dev/nrst", 9)) {

	    _data = new DataSourceTape(file, NULL);
	} else {
	    _data = new DataSourceFileStream(file, NULL);
	}
    }
	else if (!strcmp(_type,"DQL")){
	  // nothin here..
	  // Instantiate to a data stream..
	    _data = new DataSourceDQL(param, _name);
	}
#if 0
    // buffer stuff not working or used
    else if (!strcmp(_type, "BUFFER")) {
	// For BUFFER data sources, _param is a pointer to the buffer
	DOASSERT(_param, "Invalid buffer data source");
	// need buffer & data sizes
	_data = new DataSourceBuf(_param, buffer_size, data_size, NULL);
    }
#endif
 
#ifndef ATTRPROJ
	//--------------------------------------------------------------------
	// statistics datasources are stored in their corresponding viewgraph.
	// the viewgraph will delete the source.
    else if (!strcmp(_type, "BASICSTAT")) {
	DOASSERT( strncmp(name, "Stat: ", 6) == 0, "invalid basicstat prefix");
	ViewGraph* v = (ViewGraph *)ControlPanel::FindInstance(name+6);
	DOASSERT(v, "BASICSTAT view not found");
	_data = v->GetViewStatistics();
    } else if (!strncmp(_type, "HISTOGRAM", 9)) {
	DOASSERT( strncmp(name, "Hist", 4) == 0, "invalid histogram prefix");
        char *viewName = strchr(name, ':');
        viewName ++;
        while (*viewName == ' ') viewName++;
	ViewGraph* v = (ViewGraph *)ControlPanel::FindInstance(viewName);
	DOASSERT(v, "HISTOGRAM view not found");
	_data = v->GetViewHistogram();
#if defined(DEBUG)
	printf("found histogram data source 0x%p from view 0x%p\n", _data, v);
#endif
    } else if (!strncmp(_type, "GDATASTAT_X", 11)) {
	DOASSERT( strncmp(name, "GstatX", 6) == 0, "invalid gdatastat prefix");
	char *viewName = strchr(name, ':');
	viewName ++;
	while (*viewName == ' ') viewName++;
	if (!strncmp(name, "GstatXDTE", 9)) {
		char *pos = strrchr(viewName, ':');
		if (pos) *pos = '\0';
        }
	ViewGraph* v = (ViewGraph *)ControlPanel::FindInstance(viewName);
	DOASSERT(v, "GDATASTAT view not found");
	_data = v->GetGdataStatisticsX();
    } else if (!strncmp(_type, "GDATASTAT_Y", 11)) {
	DOASSERT( strncmp(name, "GstatY", 6) == 0, "invalid gdatastat prefix");
	char *viewName = strchr(name, ':');
	viewName ++;
	while (*viewName == ' ') viewName++;
	ViewGraph* v = (ViewGraph *)ControlPanel::FindInstance(viewName);
	DOASSERT(v, "GDATASTAT view not found");
	_data = v->GetGdataStatisticsY();
    }
    else if (!strcmp(_type, "WWW")) {
	char *file = MakeCacheFileName(_name, _type);
        _data = new DataSourceWeb(param, _name, file);
	delete file;
    }
#endif
    else {
	fprintf(stderr, "Invalid TData type: %s\n", _type);
	DOASSERT(0, "Invalid TData type");
    }

    if ((segOffset != 0) || (segLength != 0)) {
	  _data = new DataSourceSegment(_data, segOffset, segLength);
    }
	
    DOASSERT(_data, "Couldn't find/create data source");
    _data->AddRef();
}

/*---------------------------------------------------------------------------*/
TData::TData(DataSource* data_source)
{
#if defined(DEBUG)
    printf("TData::TData()\n");
#endif

    _name = NULL;
    _type = NULL;
    _param = NULL;
    _recSize = 0;
    _data = data_source;
    _version = 0;
    _goHomeOnInvalidate = true;
}

/*------------------------------------------------------------------------------
 * function: TData::~TData
 * TData destructor.
 */
TData::~TData()
{
    if( _data && _data->DeleteRef() )
        delete _data;
    delete [] _param;
    delete [] _type;
    delete [] _name;
}

//------------------------------------------------------------------------------
Boolean
TData::CheckFileStatus()
{
  CheckDataSource();

  /* See if file is no longer okay */
  if (!_data->IsOk()) {
    /* If file used to be okay, close it */
    if (_fileOpen) {
      if (_data->SupportsAsyncIO() &&_data->TerminateProc() < 0)
          fprintf(stderr, "Could not terminate data source process\n");
      Dispatcher::Current()->Unregister(this);
      printf("Data stream %s is no longer available\n", _name);
      _data->Close();
      TData::InvalidateTData();
      _fileOpen = false;
    }
    Boolean old = DevError::SetEnabled(false);
    if (!(_data->Open("r") == StatusOk)) {
      /* File access failure, get rid of index */
      _indexP->Clear();
      _initTotalRecs = _totalRecs = 0;
      _initLastPos = _lastPos = 0;
      _lastIncompleteLen = 0;
      (void)DevError::SetEnabled(old);
      return false;
    }
    (void)DevError::SetEnabled(old);
    printf("Data stream %s has become available\n", _name);
    _fileOpen = true;
#ifdef CONCURRENT_IO
    (void)_data->InitializeProc();
#endif
    Dispatcher::Current()->Register(this, 10, AllState, false, -1);
  }

  return true;
}

//------------------------------------------------------------------------------
void
TData::InvalidateIndex()
{
  AttrList *attrList = GetAttrList();

  for(int i = 0; i < attrList->NumAttrs(); i++) {
      AttrInfo *info = attrList->Get(i);
      info->hasHiVal = false;
      info->hasLoVal = false;
  }
}

//------------------------------------------------------------------------------
Boolean
TData::WriteIndex(int fd)
{
  AttrList *attrList = GetAttrList();

  int numAttrs = attrList->NumAttrs();
  if (write(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    reportErrSys("write");
    return false;
  }

  for(int i = 0; i < attrList->NumAttrs(); i++) {
    AttrInfo *info = attrList->Get(i);
    if (info->type == StringAttr)
      continue;
    if (write(fd, &info->hasHiVal, sizeof info->hasHiVal)
	!= sizeof info->hasHiVal) {
      reportErrSys("write");
      return false;
    }
    if (write(fd, &info->hiVal, sizeof info->hiVal) != sizeof info->hiVal) {
      reportErrSys("write");
      return false;
    }
    if (write(fd, &info->hasLoVal, sizeof info->hasLoVal)
	!= sizeof info->hasLoVal) {
      reportErrSys("write");
      return false;
    }
    if (write(fd, &info->loVal, sizeof info->loVal) != sizeof info->loVal) {
      reportErrSys("write");
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
Boolean
TData::ReadIndex(int fd)
{
  AttrList *attrList = GetAttrList();

  int numAttrs;
  if (read(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    reportErrSys("read");
    return false;
  }
  if (numAttrs != attrList->NumAttrs()) {
    printf("Index has inconsistent schema; rebuilding\n");
    return false;
  }

  for(int i = 0; i < attrList->NumAttrs(); i++) {
    AttrInfo *info = attrList->Get(i);
    if (info->type == StringAttr)
      continue;
    if (read(fd, &info->hasHiVal, sizeof info->hasHiVal)
	!= sizeof info->hasHiVal) {
      reportErrSys("read");
      return false;
    }
    if (read(fd, &info->hiVal, sizeof info->hiVal) != sizeof info->hiVal) {
      reportErrSys("read");
      return false;
    }
    if (read(fd, &info->hasLoVal, sizeof info->hasLoVal)
	!= sizeof info->hasLoVal) {
      reportErrSys("read");
      return false;
    }
    if (read(fd, &info->loVal, sizeof info->loVal) != sizeof info->loVal) {
      reportErrSys("read");
      return false;
    }
  }

  return true;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteRecs
 * For writing records. Default: not implemented.
 */

void TData::WriteRecs(RecId startId, int numRecs, void *buf)
{
  DOASSERT(0, "TData::WriteRecs not implemented");
}

/*------------------------------------------------------------------------------
 * function: TData::Save
 * Save this TData (schema(s) and actual data) to the given file.
 */
DevStatus
TData::Save(char *filename)
{
  DO_DEBUG(printf("TData::Save(%s)\n", filename));

  DevStatus result = StatusOk;

  int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (fd == -1)
  {
    reportError("Can't open file", errno);
    result = StatusFailed;
  }
  else
  {
    if (WriteHeader(fd) == StatusFailed) result = StatusFailed;
    if (WriteLogSchema(fd) == StatusFailed) result = StatusFailed;
    if (WritePhysSchema(fd) == StatusFailed) result = StatusFailed;
    if (WriteData(fd) == StatusFailed) result = StatusFailed;

    if (close(fd) == -1)
    {
      reportError("Can't close file", errno);
      result = StatusFailed;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteHeader
 * Write the appropriate file header to the given file descriptor.
 */
DevStatus
TData::WriteHeader(int fd)
{
  DevStatus result = StatusOk;

  result += WriteString(fd, DevFileHeader::Get(FILE_TYPE_TDATA));

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteLogSchema
 * Write the logical schema, if any, to the given file descriptor.
 */
DevStatus
TData::WriteLogSchema(int fd)
{
  DevStatus result = StatusOk;

  (void)/*TEMP*/WriteString(fd, "\nstartSchema logical\n");





  (void)/*TEMP*/WriteString(fd, "endSchema\n");

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WritePhysSchema
 * Write the physical schema to the given file descriptor.
 */
DevStatus
TData::WritePhysSchema(int fd)
{
  DevStatus result = StatusOk;

  (void)/*TEMP*/WriteString(fd, "\nstartSchema physical\n");

  (void)/*TEMP*/ WriteString(fd, "type <name> ascii|binary\n"/*TEMP*/);

  (void)/*TEMP*/ WriteString(fd, "comment //\nseparator ','\n"/*TEMP*/);

  AttrList *attrListP = GetAttrList();
  if (attrListP == NULL)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }
  else
  {
    attrListP->Write(fd);
  }

  (void)/*TEMP*/WriteString(fd, "endSchema\n");

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteData
 * Write the actual data to the given file descriptor.
 */
DevStatus
TData::WriteData(int fd)
{
  DevStatus result = StatusOk;

  (void)/*TEMP*/ WriteString(fd, "startData\n");

  return result;
}

/*------------------------------------------------------------------------------
 * function: WriteString
 * TEMP
 */
static DevStatus
WriteString(int fd, char *string)
{
  DevStatus result = StatusOk;

  int stringLen = strlen(string);
  if (write(fd, string, stringLen) != stringLen)
  {
    reportError("Error writing to file", errno);
    result = StatusFailed;
  }

  return result;
}

//---------------------------------------------------------------------------
void
TData::InvalidateTData()
{
    DO_DEBUG(printf("invaliding tdata version %d for %d\n",
		    _version, _data->Version()));

    if (_data->IsOk()) {
      RebuildIndex();
#ifndef ATTRPROJ
      QueryProc::Instance()->ClearTData(this);
#endif
      _version = _data->Version();
    }
}

//---------------------------------------------------------------------------
char *
TData::MakeCacheFileName(char *name, char *type)
{
  char *fname = StripPath(name);
  char *cacheDir = Init::CacheDir();
  int nameLen = strlen(cacheDir) + 1 + strlen(fname) + 1 + strlen(type) + 1;
  char *fn = new char [nameLen];
  sprintf(fn, "%s/%s.%s", cacheDir, fname, type);
  return fn;
}

//---------------------------------------------------------------------------
void
TData::Cleanup()
{
  Checkpoint();

  if (_data->isTape())
    _data->printStats();
}

//---------------------------------------------------------------------------
void
TData::PrintIndices()
{
  int cnt = 0;
  for(long i = 0; i < _totalRecs; i++) {
    printf("%ld ", _indexP->Get(i));
    if (cnt++ == 10) {
      printf("\n");
      cnt = 0;
    }
  }
  printf("\n");
}

//---------------------------------------------------------------------------
void
TData::FlushDataPipe(TDataRequest *req)
{
#if DEBUGLVL >= 3
  printf("TData::FlushDataPipe: handle %d\n", req->iohandle);
#endif

  if (req->pipeFlushed)
    return;

  /*
     Flush data from pipe. We would also like to tell the DataSource
     (which is at the other end of the pipe) to stop, but we can't
     do that yet.
  */
  while (1) {
    char *chunk;
    streampos_t offset;
    iosize_t bytes;
    int status = _data->Consume(chunk, offset, bytes);
    DOASSERT(status >= 0, "Cannot consume data");
#if DEBUGLVL >= 3
    printf("TData::FlushDataPipe: flushed %lu bytes at offset %llu\n",
           bytes, offset);
#endif

    if (bytes <= 0)
      break;
    /*
       Release chunk so buffer manager (or whoever gets the following
       call) can make use of it.
    */
    if (req->relcb)
      req->relcb->ReleaseMemory(MemMgr::Buffer, chunk, 1);
  }

  req->pipeFlushed = true;
}

//---------------------------------------------------------------------------
int
TData::GetModTime()
{
  if (!CheckFileStatus())
    return -1;

  return _data->GetModTime();
}

//---------------------------------------------------------------------------
char *
TData::MakeIndexFileName(char *name, char *type)
{
  char *fname = StripPath(name);
  int nameLen = strlen(Init::WorkDir()) + 1 + strlen(fname) + 1;
  char *fn = new char[nameLen];
  sprintf(fn, "%s/%s", Init::WorkDir(), fname);
  return fn;
}

//---------------------------------------------------------------------------
void
TData::Initialize()
{
  _indexFileName = MakeIndexFileName(_name, _type);

  if (!CheckFileStatus())
    return;

  if (_data->isBuf()) {
      BuildIndex();
      return;
  }

  if (!_indexP->Initialize(_indexFileName, _data, this, _lastPos,
                           _totalRecs).IsComplete()) goto error;

  _initTotalRecs = _totalRecs;
  _initLastPos  = _lastPos;

  /* continue to build index */
  BuildIndex();
  return;

 error:
  /* recover from error by building index from scratch  */
  RebuildIndex();
}

//---------------------------------------------------------------------------
void
TData::Checkpoint()
{
  if (!CheckFileStatus()) {
    printf("Cannot checkpoint %s\n", _name);
    return;
  }

  if (_data->isBuf()) {
      BuildIndex();
      return;
  }

  printf("Checkpointing %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - _initTotalRecs);
  
  if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
      /* no need to checkpoint */
      return;
  
  if (!_indexP->Checkpoint(_indexFileName, _data, this, _lastPos,
                           _totalRecs).IsComplete())
      return;

  /*
     This may appear unnecessary but it is here to
     improve tape performance.
  */
  _data->Seek(0, SEEK_SET);
}

//---------------------------------------------------------------------------
void
TData::RebuildIndex()
{
#if DEBUGLVL >= 3
  printf("TData(%s)::RebuildIndex()\n", GetName());
#endif

  InvalidateIndex();

  _indexP->Clear();
  _initTotalRecs = _totalRecs = 0;
  _initLastPos = _lastPos = 0;
  _lastIncompleteLen = 0;

  BuildIndex();
}

//---------------------------------------------------------------------------
int
TData::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

//---------------------------------------------------------------------------
Boolean TData::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

//---------------------------------------------------------------------------
Boolean
TData::LastID(RecId &recId)
{
#if DEBUGLVL >= 5
  printf("totalRecs = %ld\n", _totalRecs);
#endif
  if (!CheckFileStatus()) {
    recId = _totalRecs - 1;
    return false;
  }

  if (!_data->isTape()) {
    /* See if file has shrunk or grown */
    long currPos = _data->gotoEnd();
#if DEBUGLVL >= 5
    printf("TData::LastID: currpos: %ld, lastpos: %ld\n", 
	   currPos, _lastPos);
#endif
    if (currPos < _lastPos) {
      /* File has shrunk, rebuild index from scratch */
      InvalidateTData();
    } else if (currPos > _lastPos) {
      /* Don't update view more frequently than at 1-second intervals */
      time_t now = time(NULL);
      if (now != _lastFileUpdate) {
        _lastFileUpdate = now;
        /* File has grown, build index for new records */
#if DEBUGLVL >= 3
        printf("Extending index...\n");
#endif
        BuildIndex();
#ifndef ATTRPROJ
	DepMgr::Current()->RegisterEvent(this, DepMgr::EventTdataCh);
        QueryProc::Instance()->RefreshTData(this);
#endif
      }
    }
  }
  
  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

//---------------------------------------------------------------------------
TData::TDHandle
TData::InitGetRecs(Interval *interval, int &bytesleft,
                   Boolean asyncAllowed,
                   ReleaseMemoryCallback *callback)
{
  if (!strcmp(interval->AttrName, "recId")) { //recId supported
    RecId lowId = (RecId)(interval->Low);
    RecId highId = (RecId)(interval->High);

#if DEBUGLVL >= 3
    printf("TData::InitGetRecs [%ld,%ld]\n", lowId, highId);
#endif

    if (_totalRecs == 0 )  {
      return NULL;
    }
    DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
             && highId >= lowId, "Invalid record parameters");

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

    /* Compute location and number of bytes to retrieve */
    streampos_t offset = _indexP->Get((RecId)(req->nextVal));
    iosize_t bytes = _indexP->Get((RecId)(req->endVal)) + 1024 - offset;
    if ((long)req->endVal < _totalRecs - 1) {
      /* Read up to the beginning of next record */
      bytes = _indexP->Get((RecId)(req->endVal) + 1) - offset;
    }

    /*
       Don't use asynchronous I/O is caller prohibits it, or if
       data source doesn't support it, or if some other caller is
       already using it.
    */
    if (!asyncAllowed || !_data->SupportsAsyncIO()
        || _data->NumPipeData() > 0 || _data->IsBusy()) {
#if DEBUGLVL >= 3
      printf("Retrieving %llu:%lu bytes from TData 0x%p with direct I/O\n",
             offset, bytes, this);
#endif
      /* Zero handle indicates direct I/O */
      req->iohandle = 0;
    } else {
      /* Submit I/O request to the data source process */
      req->iohandle = _data->ReadProc(offset, bytes);
      DOASSERT(req->iohandle >= 0, "Cannot submit I/O request");
#if DEBUGLVL >= 3
      printf("Retrieving %llu:%lu bytes from TData 0x%p with I/O handle %d\n",
             offset, bytes, this, req->iohandle);
#endif
      req->pipeFlushed = false;
    }

    req->lastChunk = req->lastOrigChunk = NULL;
    req->lastChunkBytes = 0;
    req->nextChunk = offset;

    bytesleft = (int) (req->endVal - req->nextVal + 1) * RecSize();

    return req;
  } else {
    cout << "Only recId is supported by TData.\n";
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    exit(1);
  }
}

//---------------------------------------------------------------------------
Boolean
TData::GetRecs(TDHandle req, void *buf, int bufSize,
               Interval *interval, int &dataSize)
{
  if (!req) {
    return false;
  }
	
  if (!strcmp(req->AttrName, "recId")) { // recId stuff

#if DEBUGLVL >= 3
    printf("TData::GetRecs: handle %d, buf = 0x%p\n", req->iohandle, buf);
#endif

    interval->NumRecs = bufSize / _recSize;
    DOASSERT(interval->NumRecs > 0, "Not enough record buffer space");

    if (req->nextVal > req->endVal) {
      return false;
    }
  
    int num = (int)(req->endVal) - (int)(req->nextVal) + 1;
    if (num < interval->NumRecs) {
      interval->NumRecs = num;
    }
  
    if (req->iohandle == 0) {
      ReadRec((RecId)(req->nextVal), interval->NumRecs, buf);
    } else {
      ReadRecAsync(req, (RecId)(req->nextVal), interval->NumRecs, buf);
    }
  
    interval->Low = req->nextVal;
    dataSize = interval->NumRecs * _recSize;
    req->nextVal += interval->NumRecs;
  
    _bytesFetched += dataSize;

    interval->High = interval->Low + interval->NumRecs - 1;
    interval->AttrName = req->AttrName;
    interval->Granularity = req->granularity;

    RecId HIGHId, LOWId;
    DOASSERT(HeadID(LOWId), "can not find HeadID");
    DOASSERT(LastID(HIGHId), "can not find LastID");
    if (LOWId < req->nextVal) {
      interval->has_left = true;
      interval->left_adjacent = interval->Low - 1;
    } else {
      interval->has_left = false;
    }

    if (HIGHId > interval->High) {
      interval->has_right = true;
      interval->right_adjacent = interval->High + 1;
    } else {
      interval->has_right = false;
    }

  
    if (req->iohandle > 0 && req->nextVal > req->endVal) {
      FlushDataPipe(req);
    }

    return true;
  } else {
    cout << "TData: GetRecs deals with RecId only right now.\n";
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    exit(1);
  }
}

//---------------------------------------------------------------------------
void
TData::DoneGetRecs(TDHandle req)
{
#if DEBUGLVL >= 3
  printf("TData::DoneGetRecs: handle %d\n", req->iohandle);
#endif

  if (!req) {
     return;
  }

  /*
     Release chunk of memory cached from pipe.
  */
  if (req->relcb && req->lastOrigChunk)
      req->relcb->ReleaseMemory(MemMgr::Buffer, req->lastOrigChunk, 1);

  FlushDataPipe(req);

  delete req;
}

//===========================================================================
