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
  Revision 1.12  1996/03/05 22:06:04  jussi
  Minor fix in debugging output.

  Revision 1.11  1996/02/01 18:04:41  jussi
  Disabled 'Ignoring invalid record.' because this message shouldn't
  appear for ignored comments.

  Revision 1.10  1996/01/25 20:22:34  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.9  1996/01/12 15:24:45  jussi
  Replaced libc.h with stdlib.h.

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

TDataAscii::TDataAscii(char *name, int recSize) : TData()
{
  Dispatcher::Current()->Register(this);

  _name = name;
  _recSize = recSize;

  _file = NULL;
  _tape = NULL;

  if (!strncmp(name, "/dev/rmt", 8) || !strncmp(name, "/dev/nrmt", 9)
      || !strncmp(name, "/dev/rst", 8) || !strncmp(name, "/dev/nrst", 9)) {
    _tape = new TapeDrive(name, "r");
    if (!_tape || !*_tape) {
      fprintf(stderr, "Cannot open tape '%s': ", name);
      perror("fopen");
      Exit::DoExit(1);
    }
  } else {
    _file = fopen(name, "r");
    if (!_file) {
      fprintf(stderr, "Cannot open file '%s': ", name);
      perror("fopen");
      Exit::DoExit(1);
    }
  }

  _bytesFetched = 0;
  
  _lastPos = 0;
  _currPos = 0;

  _totalRecs = 0;
  _recBuf = new char[recSize];

  _indexSize = INIT_INDEX_SIZE;
  _index = new long[_indexSize];

  _fileGrown = false;
}

TDataAscii::~TDataAscii()
{
#ifdef DEBUG
  printf("TDataAscii destructor\n");
#endif

  delete _recBuf;
  delete _index;

  Dispatcher::Current()->Unregister(this);

  if (_tape)
    delete _tape;
  else
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
  if (_file) {
    // see if file has grown
    if (fseek(_file, 0, SEEK_END) < 0) {
      perror("LastID:fseek");
      Exit::DoExit(1);
    }
    _currPos = ftell(_file);
    if (_currPos < 0) {
      perror("LastID:ftell");
      Exit::DoExit(1);
    }
    if (_currPos > _lastPos)
      BuildIndex();
  } else {
    if (_fileGrown)
      BuildIndex();
  }
  
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
  if (fstat(fileno(_file), &sbuf) < 0) {
    fprintf(stderr, "TDataAscii::GetModTime: can't find stat\n");
    Exit::DoExit(1);
  }
  return (long)sbuf.st_mtime;
}

char *TDataAscii::MakeCacheName(char *file)
{
  char *fname = StripPath(file);
  unsigned int nameLen = strlen(Init::WorkDir()) + strlen(fname) + 8;
  char *name = new char[nameLen];
  sprintf(name, "%s/%s.cache", Init::WorkDir(), fname);
  assert(strlen(name) < nameLen);
  return name;
}

void TDataAscii::Initialize()
{
  int i = 0;

  /* Read file contents into buffer */
  _cacheFileName = MakeCacheName(_name);

  Boolean fileOpened = false;
  int cacheFd;
  if ((cacheFd = open(_cacheFileName, O_RDONLY, 0)) <0)
    goto error;
  fileOpened = true;
  
  unsigned long magicNumber;
  if (read(cacheFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
    perror("BuildIndex:read");
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
      perror("BuildIndex:tapeseek");
      goto error;
    }
    if (_tape->read(fileContent, FILE_CONTENT_COMPARE_BYTES)
	!= FILE_CONTENT_COMPARE_BYTES) {
      perror("BuildIndex:taperead");
      goto error;
    }
  } else {
    if (fseek(_file, 0, SEEK_SET) < 0) {
      perror("BuildIndex:fseek");
      goto error;
    }
    if (fread(fileContent, FILE_CONTENT_COMPARE_BYTES, 1, _file) != 1) {
      perror("BuildIndex:fread");
      goto error;
    }
  }

  if (read(cacheFd, cachedFileContent, FILE_CONTENT_COMPARE_BYTES)
      != FILE_CONTENT_COMPARE_BYTES) {
    perror("BuildIndex:read");
    goto error;
  }
  if (memcmp(cachedFileContent, fileContent, FILE_CONTENT_COMPARE_BYTES)) {
    printf("Cache file invalid; rebuilding\n");
    goto error;
  }
  
  /* File has not changed since cache file was built */

  /* Let subclass read cache */
  if (!ReadCache(cacheFd))
    goto error;
  
  /* Read last file position */
  if (read(cacheFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("BuildIndex:read");
    goto error;
  }
  
  /* Read number of records */
  if (read(cacheFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("BuildIndex:read");
    goto error;
  }

  if (_totalRecs >= _indexSize) {
    delete _index;
    _indexSize = _totalRecs + INDEX_ALLOC_INCREMENT;
#ifdef DEBUG
    printf("BuildIndex:allocating %ld index elements\n", _indexSize);
#endif
    _index = new long[_indexSize];
  }

  /* read the index */
  if (read(cacheFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("BuildIndex:read");
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
  
  unsigned long magicNumber = 0xdeadbeef;
  if (write(cacheFd, &magicNumber, sizeof magicNumber) != sizeof magicNumber) {
    perror("Checkpoint:write");
    goto error;
  }

  if (_tape) {
    if (_tape->seek(0) != 0) {
      perror("Checkpoint:fseek");
      goto error;
    }
    if (_tape->read(fileContent, FILE_CONTENT_COMPARE_BYTES)
	!= FILE_CONTENT_COMPARE_BYTES) {
      perror("Checkpoint:fread");
      goto error;
    }
  } else {
    if (fseek(_file, 0, SEEK_SET) < 0) {
      perror("Checkpoint:fseek");
      goto error;
    }
    if (fread(fileContent, FILE_CONTENT_COMPARE_BYTES, 1, _file) != 1) {
      perror("Checkpoint:fread");
      goto error;
    }
  }
  
  /* write contents of file to be compared later */
  if (write(cacheFd, fileContent, FILE_CONTENT_COMPARE_BYTES) !=
      FILE_CONTENT_COMPARE_BYTES) {
    perror("Checkpoint:write");
    goto error;
  }
  
  /* let subclass write its contents */
  if (!WriteCache(cacheFd))
    goto error;
  
  /* write last position in the file */
  if (write(cacheFd, &_lastPos, sizeof(_lastPos)) != sizeof _lastPos) {
    perror("Checkpoint:write");
    goto error;
  }
  
  /* write # of records */
  if (write(cacheFd, &_totalRecs, sizeof(_totalRecs)) != sizeof _totalRecs) {
    perror("Checkpoint:write");
    goto error;
  }
    
  /* write indices */
  if (write(cacheFd, _index, _totalRecs * sizeof(long))
      != (int)(_totalRecs * sizeof(long))) {
    perror("Checkpoint:write");
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
  printf("Entering BuildIndex\n");
#endif

  char buf[LINESIZE];
  int oldTotal = _totalRecs;
  
  if (_tape) {
    // File has been appended, extend index
    if (_tape->seek(_lastPos) != _lastPos) {
      perror("BuildIndex:tapeseek");
      Exit::DoExit(1);
    }
  } else {
    // File has been appended, extend index
    if (fseek(_file, _lastPos, SEEK_SET) < 0) {
      perror("BuildIndex:fseek");
      Exit::DoExit(1);
    }
  }

  _currPos = _lastPos;

  while(1) {

    int len = 0;

    if (_tape) {
      len = _tape->gets(buf, LINESIZE);
      if (!len)
	break;
      if (len < 0) {
	perror("BuildIndex:taperead");
	Exit::DoExit(1);
      }
    } else {
      if (fgets(buf, LINESIZE, _file) == NULL)
	break;
      len = strlen(buf);
    }

    if (len > 0 && buf[len - 1] == '\n') {
      buf[len - 1] = 0;
      if (Decode(_recBuf, buf)) {
	if (_totalRecs >= _indexSize)     // index buffer too small?
	  ExtendIndex();                  // extend it
	_index[_totalRecs++] = _currPos;
      } else {
#if 0
	printf("Ignoring invalid record: \"%s\"\n", buf);
#endif
      }
    } else {
      printf("Ignoring incomplete record: \"%s\"\n", buf);
    }

    _currPos += len;
  }

  if (_tape)
    _lastPos = _tape->tell();
  else
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
  printf("TDataAscii::ReadRec %ld,%d,0x%p\n", id, numRecs, buf);
#endif

  char line[LINESIZE];
  
  char *ptr = (char *)buf;

  for(int i = 0; i < numRecs; i++) {

    int len;
    if (_tape) {
      if (_tape->seek(_index[id + i]) != _index[id + i]) {
	perror("ReadRec:tapeseek");
	Exit::DoExit(1);
      }
      len = _tape->gets(line, LINESIZE);
      if (len <= 0) {
	perror("ReadRec:taperead");
	Exit::DoExit(1);
      }
    } else {
      if (_currPos != _index[id + i]) {
	if (fseek(_file, _index[id + i], SEEK_SET) < 0) {
	  perror("ReadRec:fseek");
	  Exit::DoExit(1);
	}
	_currPos = _index[id + i];
      }
      if (fgets(line, LINESIZE, _file) == NULL) {
	perror("ReadRec:fgets");
	Exit::DoExit(1);
      }
      len = strlen(line);
    }

    if (len > 0 ) {
      if (line[len - 1] != '\n') {
	fprintf(stderr,"ReadRec:line too long (%d)\n", len);
	Exit::DoExit(1);
      }
      line[len - 1] = '\0';
    }

    Boolean valid = Decode(ptr, line);
    assert(valid);
    ptr += _recSize;

    _currPos += len;
  }
}

void TDataAscii::ExtendIndex()
{
#ifdef DEBUG
  printf("ExtendIndex:allocating %ld index elements\n",
	 _indexSize + INDEX_ALLOC_INCREMENT);
#endif

  long *newIndex = new long[_indexSize + INDEX_ALLOC_INCREMENT];
  memcpy(newIndex, _index, _indexSize * sizeof(long));
  delete _index;
  _index = newIndex;
  _indexSize += INDEX_ALLOC_INCREMENT;
}

void TDataAscii::WriteRecs(RecId startRid, int numRecs, void *buf)
{
  if (_tape) {
    fprintf(stderr, "Writing to tape not supported yet\n");
    Exit::DoExit(1);
  }

  if (_totalRecs >= _indexSize)         // index buffer too small?
    ExtendIndex();                      // extend it

  _index[_totalRecs++] = _lastPos;
  int len = strlen((char *)buf);

  if (_tape) {
    if (_tape->append(buf, len) != len) {
      perror("WriteRecs:tapeappend");
      Exit::DoExit(1);
    }
    _lastPos = _tape->tell();
  } else {
    if (fseek(_file, _lastPos, SEEK_SET) < 0) {
      perror("WriteRecs:fseek");
      Exit::DoExit(1);
    }
    if (fwrite(buf, len, 1, _file) != 1) {
      perror("WriteRecs:fwrite");
      Exit::DoExit(1);
    }
    _lastPos = ftell(_file);
  }

  _currPos = _lastPos;

  _fileGrown = true;
}

void TDataAscii::WriteLine(void *line)
{
  int len = strlen((char *)line);

  if (_tape) {
    if (_tape->append(line, len) != len) {
      perror("WriteLine:tapeappend");
      Exit::DoExit(1);
    }
    _lastPos = _tape->tell();
  } else {
    if (fseek(_file, _lastPos, SEEK_SET) < 0) {
      perror("WriteLine:fseek");
      Exit::DoExit(1);
    }
    if (fwrite(line, len, 1, _file) != 1) {
      perror("WriteLine:fwrite");
      Exit::DoExit(1);
    }
    _lastPos = ftell(_file);
  }

  _currPos = _lastPos;

  _fileGrown = true;
}

void TDataAscii::Cleanup()
{
  Checkpoint();

  if (_tape) {
    _tape->printStats();
    delete _tape;
    _tape = NULL;
  }
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
