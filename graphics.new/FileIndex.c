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
  Implementation of FileIndex class.

  Note:  structure of an index file is as follows:
	magic number
	a copy of the first fileCompareBytes bytes of the actual data
	information about the attributes (written by the TDataAsciiInterp
	  or TDataBinaryInterp class)
	the last position in the data
	the number of records in the data
	the actual indices
 */

/*
  $Id$

  $Log$
  Revision 1.7  1997/05/28 15:39:15  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.6.4.2  1997/05/27 18:02:54  wenger
  Minor bug fixes and cleanup to Shilpa's layout manager code and associated
  code.

  Revision 1.6.4.1  1997/05/20 19:49:38  ssl
  Fixed stuff to allow empty TData

  Revision 1.6  1997/03/19 21:33:57  wenger
  Fixed bug 172 (DTE index filename conflict) -- DTE TData names are now
  the data file name instead of the schema file name.

  Revision 1.5  1996/11/25 18:15:14  wenger
  Changes to non-indexed attrproj to match other changes to TData;
  changes to compile non-index attrproj and to get the rest of Devise
  to compile with that.

  Revision 1.4  1996/11/18 22:51:04  jussi
  Improved the way the index array is allocated and reallocated.

  Revision 1.3  1996/10/08 21:49:08  wenger
  ClassDir now checks for duplicate instance names; fixed bug 047
  (problem with FileIndex class); fixed various other bugs.

  Revision 1.2  1996/10/05 16:24:43  wenger
  Fixed up includes (didn't work on HP).

  Revision 1.1  1996/10/04 17:44:13  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

 */

#define _FileIndex_c_

#define DEBUGLVL 0

#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "FileIndex.h"
#include "Util.h"
#include "Exit.h"
#include "DevError.h"

// Note: neither indexMagicNumber nor fileCompareBytes can be changed
// without goofing up existing index files.
static const unsigned long indexMagicNumber = 0xdeadbeef;
static const int fileCompareBytes = 4096;

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: FileIndex::FileIndex
 * Constructor.
 */
FileIndex::FileIndex(int initSize)
{
#if (DEBUGLVL >= 1)
  printf("FileIndex::FileIndex()\n");
#endif

  _indexSize = 0;
  _indexArray = NULL;
  _highestValidIndex = -1;

  _indexFileName = NULL;
  _changedSinceCheckpoint = false;

  ExpandArray(initSize);
}

/*------------------------------------------------------------------------------
 * function: FileIndex::~FileIndex
 * Destructor.
 */
FileIndex::~FileIndex()
{
#if (DEBUGLVL >= 1)
  printf("FileIndex::~FileIndex()\n");
#endif

  delete [] _indexArray;
  _indexArray = NULL;

  if (_indexFileName) {
    free(_indexFileName);
    _indexFileName = NULL;
  }
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Get
 * Get the offset for the given record.
 */
OffsetType
FileIndex::Get(RecId recId)
{
#if (DEBUGLVL >= 5)
  printf("FileIndex::Get(%ld)\n", recId);
#endif
  
  //printf("_highestValidIndex %d\n", _highestValidIndex);
  DOASSERT((int) recId <= _highestValidIndex, "Illegal record ID");

  return _indexArray[recId];
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Set
 * Set the offset for the given record.
 */
void
FileIndex::Set(RecId recId, OffsetType offset)
{
#if (DEBUGLVL >= 5)
  printf("FileIndex::Set(%d, %d)\n", (int) recId, (int) offset);
#endif

  _changedSinceCheckpoint = true;

  if ((int) recId >= _indexSize)
  {
    ExpandArray(recId);
  }

  _indexArray[recId] = offset;
  _highestValidIndex = MAX(((int)recId), _highestValidIndex);

  return;
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Clear
 * Clear the index.
 */
void
FileIndex::Clear()
{
#if (DEBUGLVL >= 3)
  printf("FileIndex::Clear()\n");
#endif

  _changedSinceCheckpoint = false; // false because we unlink the index file

  _highestValidIndex = -1;
  if (_indexFileName) (void) unlink(_indexFileName);
}


/*------------------------------------------------------------------------------
 * function: FileIndex::Initialize
 * Read the index from the given file.
 */
DevStatus
FileIndex::Initialize(char *indexFileName, DataSource *dataP, TData *tdataP,
                      long& lastPos, long& totalRecs)
{
#if (DEBUGLVL >= 1)
  printf("FileIndex::Initialize(%s)\n", indexFileName);
#endif

  DevStatus result(StatusOk);
  int indexFd = -1;

  if (_indexFileName) free(_indexFileName);
  _indexFileName = CopyString(indexFileName);

  indexFd = open(indexFileName, O_RDONLY, 0);
  if (indexFd < 0)
  {
    // Don't report an error, because we can recover from this.  (This is
    // not necessarily an error -- we may just not have an index for this
    // data yet.)
#if defined(DEBUG)
    reportErrSys("Can't open index file");
#endif
    result += StatusFailed;
  }

/*
 * Check the magic number to make sure the file really is an index file.
 */
  if (result.IsComplete())
  {
    unsigned long magicNumber;
    if (read(indexFd, &magicNumber, sizeof(magicNumber)) != sizeof(magicNumber))
    {
      reportErrSys("Can't read index file");
      result += StatusFailed;
    }
    else
    {
      if (magicNumber != indexMagicNumber)
      {
	reportErrNosys("Index file incompatible (bad magic number)");
	result += StatusFailed;
      }
    }
  }

/*
 * Make sure the index file corresponds to the data.
 */
  char fileContent[fileCompareBytes];
  if (result.IsComplete())
  {
    if (dataP->Seek(0, SEEK_SET) < 0)
    {
      reportErrNosys("Error seeking on data file");
      result += StatusFailed;
    }
    else
    {
      if (dataP->Fread(fileContent, fileCompareBytes, 1) != 1)
      {
        reportErrNosys("Error reading data file");
        result += StatusFailed;
      }
    }
  }

  char indexFileContent[fileCompareBytes];
  if (result.IsComplete())
  {
    if (read(indexFd, indexFileContent, fileCompareBytes) != fileCompareBytes)
    {
      reportErrNosys("Error reading data file");
      result += StatusFailed;
    }
    else
    {
      if (memcmp(fileContent, indexFileContent, fileCompareBytes))
      {
	reportErrNosys("Index file invalid");
	result += StatusFailed;
      }
    }
  }

/*
 * If we got here without any errors, the data has not changed since the
 * index file was built.
 */

/*
 * Let TData subclass read index.
 */
  if (result.IsComplete())
  {
    if (!tdataP->ReadIndex(indexFd))
    {
      result += StatusFailed;
    }
  }

/*
 * Read last file position and number of records.
 */
  if (result.IsComplete())
  {
    if (read(indexFd, &lastPos, sizeof(lastPos)) != sizeof(lastPos))
    {
      reportErrSys("Error reading index file");
      result += StatusFailed;
    }
    else if (read(indexFd, &totalRecs, sizeof(totalRecs)) != sizeof(totalRecs))
    {
      reportErrSys("Error reading index file");
      result += StatusFailed;
    }
  }

/*
 * Read the actual index data.
 */
  if (result.IsComplete())
  {
    result += Read(indexFd, totalRecs);
  }

/*
 * Make sure the index is consistent.
 */
  if (result.IsComplete())
  {
    int recId;
    for (recId = 1; recId < totalRecs; recId++)
    {
      if (_indexArray[recId - 1] > _indexArray[recId])
      {
	reportErrNosys("Index is inconsistent");
	result += StatusFailed;
	break;
      }
    }
  }

  _highestValidIndex = totalRecs - 1;

/*
 * Make sure the index file is closed.
 */
  if (indexFd >= 0)
  {
    if (close(indexFd) != 0)
    {
      reportErrSys("Error closing index file");
      result += StatusWarn;
    }
  }

/*
 * Delete the index file if there was an error.
 */
  if (!result.IsComplete())
  {
    (void) unlink(indexFileName);
  }

  if (result.IsComplete()) {
    _changedSinceCheckpoint = false;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Checkpoint
 * Checkpoint the data (write out the index file).
 */
DevStatus
FileIndex::Checkpoint(char *indexFileName, DataSource *dataP, TData *tdataP,
    long lastPos, long totalRecs)
{
#if (DEBUGLVL >= 1)
  printf("FileIndex::Checkpoint()\n");
#endif

  DevStatus result(StatusOk);

  if (_indexFileName && !strcmp(_indexFileName, indexFileName) &&
      !_changedSinceCheckpoint) {
    // We don't need to do anything.
#if (DEBUGLVL >= 1)
    printf("Index unchanged since last checkpoint\n");
#endif
    return result;
  }

  if (_indexFileName && strcmp(_indexFileName, indexFileName)) {
    free(_indexFileName);
    _indexFileName = CopyString(indexFileName);
  }

  int indexFd = -1;
  indexFd = open(indexFileName, O_CREAT| O_RDWR,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (indexFd < 0)
  {
    char errBuf[1024];
    sprintf(errBuf, "Cannot create index file %s\n", indexFileName);
    result += StatusFailed;
  }

/*
 * Write the magic number.
 */
  if (result.IsComplete())
  {
    if (write(indexFd, &indexMagicNumber, sizeof(indexMagicNumber)) !=
      sizeof(indexMagicNumber))
    {
      reportErrSys("Error writing index file");
      result += StatusFailed;
    }
  }

/*
 * Read the first fileCompareBytes from the data and write it to the index
 * file.
 */
  char fileContent[fileCompareBytes];
  if (result.IsComplete())
  {
    if (dataP->Seek(0, SEEK_SET) < 0)
    {
      reportErrNosys("Error seeking on data file");
      result += StatusFailed;
    }
    else
    {
      if (dataP->Fread(fileContent, fileCompareBytes, 1) != 1)
      {
	if (errno != 0)
	{
          reportErrSys("Error reading data file");
          result += StatusFailed;
	}
	else
	{
	  fprintf(stderr, "File not checkpointed due to its small size\n");
	  result += StatusCancel;
	}
      }
    }
  }

  if (result.IsComplete())
  {
    if (write(indexFd, fileContent, fileCompareBytes) != fileCompareBytes)
    {
      reportErrSys("Error writing index file");
      result += StatusFailed;
    }
  }

/*
 * Let the TData subclass write its index.
 */
  if (result.IsComplete())
  {
    if (!tdataP->WriteIndex(indexFd))
    {
      result += StatusFailed;
    }
  }

/*
 * Write last file position and number of records.
 */
  if (result.IsComplete())
  {
    if (write(indexFd, &lastPos, sizeof(lastPos)) != sizeof(lastPos))
    {
      reportErrSys("Error writing index file");
      result += StatusFailed;
    }
    else if (write(indexFd, &totalRecs, sizeof(totalRecs)) != sizeof(totalRecs))
    {
      reportErrSys("Error writing index file");
      result += StatusFailed;
    }
  }

/*
 * Write the actual index data.
 */
  if (result.IsComplete())
  {
    result += Write(indexFd, totalRecs);
  }

/*
 * Make sure the index file is closed.
 */
  if (indexFd >= 0)
  {
    if (close(indexFd) != 0)
    {
      reportErrSys("Error closing index file");
      result += StatusWarn;
    }
  }

/*
 * Delete the index file if there was an error.
 */
  if (!result.IsComplete())
  {
    (void) unlink(indexFileName);
  }

  if (result.IsComplete()) {
    _changedSinceCheckpoint = false;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Read
 * Read the index from the given file.
 */
DevStatus
FileIndex::Read(int fd, long recordCount)
{
#if (DEBUGLVL >= 3)
  printf("FileIndex::Read(%ld)\n", recordCount);
#endif

  DevStatus result(StatusOk);

  if (recordCount > _indexSize)
  {
    delete [] _indexArray;
    _indexArray = new OffsetType[recordCount];
    DOASSERT(_indexArray != NULL, "Out of memory");
    _indexSize = recordCount;
  }

  if (result.IsComplete())
  {
    long bytesToRead = recordCount * sizeof(OffsetType);
    if (read(fd, _indexArray, bytesToRead) != bytesToRead)
    {
      reportErrSys("Error reading index");
      result += StatusFailed;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: FileIndex::Write
 * Write the index to the given file.
 */
DevStatus
FileIndex::Write(int fd, long recordCount)
{
#if (DEBUGLVL >= 3)
  printf("FileIndex::Write(%ld)\n", recordCount);
#endif

  DevStatus result(StatusOk);

  long bytesToWrite = recordCount * sizeof(OffsetType);
  if (write(fd, _indexArray, bytesToWrite) != bytesToWrite)
  {
    reportErrSys("Error writing index");
    result += StatusFailed;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: FileIndex::ExpandArray
 * Expand the index array to hold records through at least recId.
 */
void
FileIndex::ExpandArray(RecId recId)
{
#if (DEBUGLVL >= 7)
  printf("FileIndex::ExpandArray(%d)\n", (int) recId);
#endif

  int newIndexSize = _indexSize + MAX(10000, _indexSize);
  if (recId + 1 > (unsigned) newIndexSize) newIndexSize = recId + 1;

  OffsetType *newArray = new OffsetType[newIndexSize];
  DOASSERT(newArray != NULL, "Out of memory");
  memcpy(newArray, _indexArray, _indexSize * sizeof(OffsetType));
  delete [] _indexArray;

  _indexArray = newArray;
  _indexSize = newIndexSize;

  return;
}

/*============================================================================*/
