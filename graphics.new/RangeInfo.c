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
  Revision 1.5  1996/01/12 15:23:55  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.4  1995/12/14 22:04:13  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.3  1995/12/14 18:14:52  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.2  1995/09/05 22:15:22  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "RangeInfo.h"
#include "Exit.h"

//#define DEBUG

/**************************************************************
Constructor
***************************************************************/

RangeInfo::RangeInfo()
{
  _inUse = 0;
  _dirty = 0;
  buf = NULL;
  bufSize = 0;
  tdata = NULL;
  dataSize = 0;
  low = high = 0;
  next = prev = NULL;
}

/**************************************************************
Increment inuse count of page
*****************************************************************/

void RangeInfo::ClearUse()
{
  if (--_inUse < 0) {
    fprintf(stderr, "RangeInfo::ClearUse: use count < 0\n");
    Exit::DoExit(1);
  }
}

void RangeInfo::RecIds(RecId &lowId, RecId &highId)
{
  lowId = low;
  highId = high;
}

/*************************************************************/

RangeInfoAlloc::RangeInfoAlloc(int bufSize)
{
  _rangeInfoFreeList = NULL;
  if ((_startAddr = (char *)malloc(bufSize + 32))== NULL) {
    fprintf(stderr, "Cannot allocate %.2f MB of main memory.\n",
	    1.0 * bufSize / (1024 * 1024));
    Exit::DoExit(1);
  }

  unsigned int addr = (unsigned)_startAddr;
  if (addr % 32 != 0) {
    /* round off the address to the next higher multiple of 32 */
    addr = addr / 32 * 32 + 32;
    _startAddr = (char *)addr;
  }
  
  _bufFreeList = AllocRangeInfo();
  _bufFreeList->next = NULL;
  _bufFreeList->buf = (char *)_startAddr;
  _bufFreeList->bufSize = bufSize;
  _bufSize = bufSize;

#ifdef DEBUG
  printf("Allocated %.2f MB, buffer start = 0x%p, end = 0x%p\n",
	 1.0 * bufSize / (1024 * 1024), _startAddr, _startAddr + bufSize - 1);
#endif
}

/**********************************************************
Allocate a range by given number of bytes 
**********************************************************/

RangeInfo *RangeInfoAlloc::AllocRange(int numBytes)
{
#ifdef DEBUG
  printf("RangeInfoAlloc::AllocRange %d bytes\n", numBytes);
#endif

  if (numBytes <= 0) {
    fprintf(stderr, "RangeInfoAlloc::AllocRange: can't alloc %d bytes\n",
	    numBytes);
    Exit::DoExit(1);
  }

  /* Go through free list and find first fit */
  RangeInfo *cur = NULL;
  RangeInfo *prev = NULL;

  for(cur = _bufFreeList; cur != NULL; prev = cur, cur = cur->next) {
    if (cur->BufSize() >= numBytes) {
#ifdef DEBUG
      printf("Found free range 0x%p, size %d bytes\n", cur, cur->BufSize());
#endif
      break;
    }
  }

  /* no match? use first one on the list */
  if (!cur) {
    cur = _bufFreeList;
    prev = NULL;
#ifdef DEBUG
    if (cur)
      printf("Using first free range, size %d bytes\n", cur->BufSize());
#endif
  }

  /* still no match? */
  if (!cur)
    return NULL;

  RangeInfo *freeInfo = NULL;
  if (cur->BufSize() > numBytes)
    freeInfo = Truncate(cur, cur->BufSize() - numBytes);
      
  if (prev)
    prev->next = cur->next;
  else
    _bufFreeList = _bufFreeList->next;
  
  cur->next = NULL;

  if (freeInfo) {
    /* insert leftover back into free list */
    freeInfo->next = _bufFreeList;
    _bufFreeList = freeInfo;
  }

#ifdef DEBUG
  Print();
#endif

  return cur;
}

/**********************************************************
Free a range of memory
**********************************************************/

void RangeInfoAlloc::FreeRange(RangeInfo *rangeInfo)
{
  rangeInfo->next =_bufFreeList;
  _bufFreeList = rangeInfo;
}

/**********************************************************
Truncate a range by given number of bytes 
**********************************************************/

RangeInfo *RangeInfoAlloc::Truncate(RangeInfo *rangeInfo, int numBytes)
{
#ifdef DEBUG
  printf("Truncating range 0x%p to %d bytes\n", rangeInfo, numBytes);
#endif

  if (numBytes >= rangeInfo->bufSize) {
    fprintf(stderr,"RangeAlloc::Truncate: numbytes %d > bufSize %d\n",
	    numBytes, rangeInfo->bufSize);
    Exit::DoExit(2);
  }

  if (numBytes <= 0) {
    fprintf(stderr, "RangeAlloc::Truncate: numbytes %d <= 0\n", numBytes);
    Exit::DoExit(2);
  }
  
  RangeInfo *newInfo = AllocRangeInfo();
  newInfo->buf = rangeInfo->buf;
  newInfo->bufSize = numBytes;
  
  rangeInfo->bufSize -= numBytes;
  rangeInfo->buf += numBytes;
  
  return newInfo;
}

RangeInfo *RangeInfoAlloc::AllocRangeInfo()
{
  RangeInfo *rangeInfo;
  if (_rangeInfoFreeList == NULL) {
    rangeInfo = new RangeInfo();
  } else {
    rangeInfo = _rangeInfoFreeList;
    _rangeInfoFreeList = _rangeInfoFreeList->next;
  }
  return rangeInfo;
}

void RangeInfoAlloc::FreeRangeInfo(RangeInfo *rangeInfo)
{
  rangeInfo->next = _rangeInfoFreeList;
  _rangeInfoFreeList = rangeInfo;
}

/* Clear to the state of 1 unallocated buffer range */

void RangeInfoAlloc::Clear()
{
  while(_bufFreeList != NULL) {
    RangeInfo *rangeInfo = _bufFreeList;
    _bufFreeList = _bufFreeList->next;
    FreeRangeInfo(rangeInfo);
  }
  
  _bufFreeList = AllocRangeInfo();
  _bufFreeList->next = NULL;
  _bufFreeList->buf = (char *)_startAddr;
  _bufFreeList->bufSize = _bufSize;
}

void RangeInfoAlloc::Print()
{
  printf("RangeInfo::FreeList:\n");
  RangeInfo *rangeInfo = _bufFreeList;
  while(rangeInfo) {
    printf("  at 0x%p, size %d\n", rangeInfo, rangeInfo->BufSize());
    rangeInfo = rangeInfo->next;
  }
}
