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
  Revision 1.8  1996/01/09 16:35:00  jussi
  Improved console output messages.

  Revision 1.7  1995/12/28 19:59:41  jussi
  Small fixes to remove compiler warnings.

  Revision 1.6  1995/12/14 21:19:31  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.5  1995/12/14 17:57:37  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.4  1995/11/24 21:34:55  jussi
  Added _currPos scheme to eliminate most of the fseek() calls.
  This appears to speed up execution significantly.

  Revision 1.3  1995/11/22 17:51:35  jussi
  Added copyright notice and cleaned up the code. Optimized some
  routines a la TDataTape.C.

  Revision 1.2  1995/09/05 22:15:49  jussi
  Added CVS header.
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
#include "TDataAscii.h"
#include "Exit.h"
#include "Util.h"
#include "Init.h"

//#define DEBUG

static char fileContent[FILE_CONTENT_COMPARE_BYTES];
static char cachedFileContent[FILE_CONTENT_COMPARE_BYTES];

static char *MakeCacheName(char *file)
{
  char *fname = StripPath(file);
  unsigned int nameLen = strlen(Init::WorkDir()) + strlen(fname) + 8;
  char *name = new char[nameLen];
  sprintf(name,"%s/%s.cache", Init::WorkDir(), fname);
  assert(strlen(name) < nameLen);
  return name;
}

TDataAscii::TDataAscii(char *name, int recSize) : TData()
{
  Dispatcher::Current()->Register(this);

  _name = name;
  _recSize = recSize;

  _file = fopen(name,"r");
  if (_file ==NULL) {
    fprintf(stderr,"TDataAscii: cannot open file '%s' ", name);
    perror("fopen");
    Exit::DoExit(1);
  }

  struct stat sbuf;
  if (fstat(fileno(_file),&sbuf) < 0) {
    perror("fstat");
    Exit::DoExit(1);
  }
  
  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;
  _fileGrown = false;

  _totalRecs = 0;
  _recBuf = new char[recSize];
  _index = new long[INIT_INDEX_SIZE];
  _indexSize = INIT_INDEX_SIZE;
}

TDataAscii::~TDataAscii()
{
#ifdef DEBUG
  printf("TDataAscii destructor\n");
#endif

  Dispatcher::Current()->Unregister(this);
  fclose(_file);
}

int TDataAscii::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

Boolean TDataAscii::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

Boolean TDataAscii::LastID(RecId &recId)
{
  // see if file has grown
  if (_fileGrown)
    BuildIndex();
  
  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

void TDataAscii::InitGetRecs(RecId lowId, RecId highId,RecordOrder order)
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

Boolean TDataAscii::GetRecs(void *buf, int bufSize, 
			    RecId &startRid,int &numRecs, int &dataSize,
			    void **recPtrs)
{
#ifdef DEBUG
  printf("TDataAscii::GetRecs buf = 0x%p\n", buf);
#endif

  numRecs = bufSize/_recSize;
  if (!numRecs) {
    fprintf(stderr,"TDataRec::GetRecs(): not enough buffer space \n");
    fprintf(stderr,"buffer size = %d, rec size= %d\n", bufSize, _recSize);
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

void TDataAscii::GetRecPointers(RecId startId, int numRecs,
				void *buf, void **recPtrs)
{
  fprintf(stderr,"TDataRec::GetRecPointers: not implemented\n");
  Exit::DoExit(1);
}

void TDataAscii::GetIndex(RecId id, int *&indices)
{
  static int index[1];
  index[0] = id;
  indices = index;
}

int TDataAscii::GetModTime()
{
  struct stat sbuf;
  if (fstat(fileno(_file),&sbuf) < 0) {
    fprintf(stderr,"UnixRecFile:: can't find stat\n");
    Exit::DoExit(1);
  }
  return (long)sbuf.st_mtime;
}

void TDataAscii::Initialize()
{
  int indexErrors = 0;
  int i = 0;

  /* Read file contents into buffer */
  _cacheFileName = MakeCacheName(_name);

  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_RDONLY, 0)) <0)
    goto error;
  fileOpened = true;
  
  /* cache file exists. See if we are still working on the same
     file, and if we are, reinitialize */
  if (fseek(_file, 0, SEEK_SET) < 0)
    goto error;
  
  if (read(cacheFd, cachedFileContent, FILE_CONTENT_COMPARE_BYTES)
      != FILE_CONTENT_COMPARE_BYTES  ||
      fread(fileContent, FILE_CONTENT_COMPARE_BYTES, 1,_file) != 1 ||
      memcmp(cachedFileContent, fileContent, FILE_CONTENT_COMPARE_BYTES) != 0)
    goto error;
  
  /* Assume identical file */

  /* Read last file position */
  if (lseek(cacheFd,LAST_POS_OFFSET,SEEK_SET) != LAST_POS_OFFSET)
    goto error;
  if (read(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
    goto error;
  
  /* Read number of records */
  if (lseek(cacheFd,TOTAL_RECS_OFFSET,SEEK_SET) != TOTAL_RECS_OFFSET)
    goto error;
  if (read(cacheFd,&_totalRecs, sizeof(_totalRecs))!= 
      sizeof(_totalRecs)) {
    goto error;
  }

  if (_totalRecs >= _indexSize) {
    delete _index;
    _index = new long[_totalRecs+INDEX_ALLOC_INCREMENT];
    _indexSize = _totalRecs+INDEX_ALLOC_INCREMENT;
  }

  /* read the index */
  if (read(cacheFd,_index, _totalRecs*sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    goto error;
  }
  
  for(i = 1; i < _totalRecs; i++) {
    if (_index[i - 1] > _index[i]) {
      printf("Error: index[%d] = %ld > index[%d] = %ld\n",
	     i - 1, _index[i - 1], i, _index[i]);
      ++indexErrors;
    }
  }
  assert(!indexErrors);

  /* Let subclass read cache */
  if (lseek(cacheFd,SUBCLASS_OFFSET,SEEK_SET) != SUBCLASS_OFFSET)
    goto error;
  
  /* can't read cache? */
  if (!ReadCache(cacheFd))
    goto error;
  
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

/* Do a checkpoint */
void TDataAscii::Checkpoint()
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
  
  if (fseek(_file,0,SEEK_SET) < 0) {
    perror("TDataAscii::CheckPoint");
    goto error;
  }
  if (fread(fileContent,FILE_CONTENT_COMPARE_BYTES, 1,_file) != 1) {
    goto error;
  }
  
  /* write contents of file to be compared later */
  if (write(cacheFd,fileContent,FILE_CONTENT_COMPARE_BYTES)!=
      FILE_CONTENT_COMPARE_BYTES)
    goto error;
  
  /* subclass should write its contents */
  if (lseek(cacheFd, SUBCLASS_OFFSET,SEEK_SET) != SUBCLASS_OFFSET) {
    perror("TDataAScii::ChecPoint");
    goto error;
  }
  /* let subclass write */
  if (!WriteCache(cacheFd))
    goto error;
  
  /* write last position in the file */
  if (_lastPos > _initLastPos) {
    if (lseek(cacheFd, LAST_POS_OFFSET,SEEK_SET) != LAST_POS_OFFSET) {
      perror("TDataAScii::ChecPoint");
      goto error;
    }
    if (write(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
      goto error;
  }
  
  /* write # of records */
  if (_totalRecs > _initTotalRecs) {
    if (lseek(cacheFd, TOTAL_RECS_OFFSET,SEEK_SET) != TOTAL_RECS_OFFSET) {
      perror("TDataAScii::ChecPoint");
      goto error;
    }
    if (write(cacheFd, &_totalRecs, sizeof(_totalRecs))!=sizeof(_totalRecs))
      goto error;
    
    /* write indices */
    int indexOffset = INDEX_OFFSET + _initTotalRecs*sizeof(long);
    if (lseek(cacheFd,indexOffset, SEEK_SET) != indexOffset)
      goto error;
    int bytes = (_totalRecs-_initTotalRecs)*sizeof(long);
    if (write(cacheFd, &_index[_initTotalRecs], bytes) != bytes)
      goto error;
  }
  close(cacheFd);

  _currPos = ftell(_file);

  return;
  
 error:
  if (fileOpened)
    close(cacheFd);
  (void)unlink(_cacheFileName);

  _currPos = ftell(_file);
}

/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataAscii::BuildIndex()
{
#ifdef DEBUG
  cout << "Entering BuildIndex" << endl;
#endif

  char buf[LINESIZE];
  int oldTotal = _totalRecs;
  
  // File has been appended, extend index
  if (fseek(_file, _lastPos, SEEK_SET) < 0) {
    perror("TDataAscii::BuildIndex");
    Exit::DoExit(1);
  }

  _currPos = _lastPos;

  while(1) {
    if (fgets(buf, LINESIZE, _file) == NULL)
      break;
  
    /* get rid of '\n' */
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
      buf[len - 1] = '\0';
    }
    
    if (IsValid(buf)) {
      if (_totalRecs >= _indexSize)     // index buffer too small?
	ExtendIndex();                  // extend it
      _index[_totalRecs++] = _currPos;
    }

    _currPos += len;
  }

  _lastPos = ftell(_file);
  if (_lastPos != _currPos)
    printf("lastPos %ld, currPos %ld\n", _lastPos, _currPos);
  assert(_lastPos == _currPos);

  _fileGrown = false;

  printf("Index for %s: %ld total records, %ld new\n", _name,
	 _totalRecs, _totalRecs - oldTotal);
}

void TDataAscii::ReadRec(RecId id, int numRecs, void *buf)
{
#ifdef DEBUG
  printf("TDataAscii::ReadRec %d,%d,0x%p\n",id,numRecs,buf);
#endif

  char line[LINESIZE];
  
  char *ptr = (char *)buf;
  for(int i = 0; i < numRecs; i++) {
    if (_currPos != _index[id + i]) {
      if (fseek(_file, _index[id + i], SEEK_SET) < 0) {
	fprintf(stderr,"TDataAscii::ReadRec(%ld,%d,0x%p) seek\n",
		id, numRecs, buf);
	perror("fseek");
	PrintIndices();
	Exit::DoExit(1);
      }
      _currPos = _index[id + i];
    }

    if (fgets(line, LINESIZE, _file) == NULL) {
      fprintf(stderr,"TDataAscii::ReadRec(%ld,%d,0x%p) fgets pos %ld\n",
	      id, numRecs, buf, _index[id + i]);
      perror("fgets");
      Exit::DoExit(1);
    }

    int len = strlen(line);
    if (len > 0 ) {
      if (line[len-1] != '\n') {
	fprintf(stderr,"TDataAscii::ReadRec: line too long\n");
	Exit::DoExit(1);
      }
      line[len-1] = '\0';
    }

    Boolean valid = Decode(id + i, ptr, line);
    assert(valid);
    ptr += _recSize;

    _currPos += len;
  }
}

void TDataAscii::ExtendIndex()
{
  long *newIndex = new long[_indexSize + INDEX_ALLOC_INCREMENT];
  memcpy(newIndex, _index, _indexSize * sizeof(long));
  delete _index;
  _index = newIndex;
  _indexSize += INDEX_ALLOC_INCREMENT;
}

void TDataAscii::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  if (_totalRecs >= _indexSize)
    /* index buffer too small, extend it */
    ExtendIndex();

  if (fseek(_file,_lastPos, SEEK_SET) < 0) {
    perror("TDataAscii::WriteRecs");
    Exit::DoExit(1);
  }

  _index[_totalRecs++] = _lastPos;
  if (fwrite(buf,strlen((char *)buf),1,_file) != 1) {
    perror("TDataAScii::WriteRecs");
    Exit::DoExit(1);
  }

  _currPos = _lastPos = ftell(_file);
  _fileGrown = true;
}

void TDataAscii::WriteLine(void *line)
{
  if (fseek(_file,_lastPos, SEEK_SET) < 0) {
    perror("TDataAscii::WriteLine");
    Exit::DoExit(1);
  }

  if (fwrite(line,strlen((char *)line),1,_file) != 1) {
    perror("TDataAScii::WriteRecs");
    Exit::DoExit(1);
  }

  _currPos = _lastPos = ftell(_file);
  _fileGrown = true;
}

void TDataAscii::Cleanup()
{
  Checkpoint();
}

void TDataAscii::PrintIndices()
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
