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
  Declaration of FileIndex class -- for building up an index to a file
  (record ID to file offset mapping).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1996/10/05 16:24:46  wenger
  Fixed up includes (didn't work on HP).

  Revision 1.1  1996/10/04 17:44:14  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

 */

#ifndef _FileIndex_h_
#define _FileIndex_h_


#include <sys/types.h>

#include "RecId.h"
#include "DataSource.h"
#include "TData.h"
#include "DevStatus.h"

typedef unsigned long OffsetType;

// Note: RecId's here start at zero, even though the record id attributes
// start at one.

class FileIndex
{
public:
  FileIndex(int initSize);
  ~FileIndex();

  OffsetType Get(RecId recId);
  void Set(RecId recId, OffsetType offset);

  void Clear();

  DevStatus Initialize(char *indexFileName, DataSource *dataP, TData *tdataP,
      long& lastPos, long& totalRecs);
  DevStatus Checkpoint(char *indexFileName, DataSource *dataP, TData *tdataP,
      long lastPos, long totalRecs);

  int Size() { return _indexSize; }

private:
  DevStatus Read(int fd, long recordCount);
  DevStatus Write(int fd, long recordCount);

  void ExpandArray(RecId recId);

  int _indexSize;
  int _highestValidIndex;
  OffsetType *_indexArray;
};


#endif /* _FileIndex_h_ */

/*============================================================================*/
