/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:15:22  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>

#include "RangeInfo.h"
#include "Exit.h"

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
  if (--_inUse < 0){
    fprintf(stderr,"RangeInfo::ClearUse: use count < 0\n");
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
  if ((_startAddr = (char *)malloc(bufSize+32))== NULL){
    fprintf(stderr,"RangeAlloc: no memory\n");
    Exit::DoExit(1);
  }
  unsigned int addr = (unsigned)_startAddr;
  if (addr % 32 != 0){
    /* round off the address to the next higher multiple of 32 */
    addr = addr /32*32 + 32;
    _startAddr = (char *)addr;
  }
  /*
     printf("buffer 0x%x in RangeInfoAlloc\n",_startAddr);
  */
  
  _bufFreeList = AllocRangeInfo();
  _bufFreeList->next = NULL;
  _bufFreeList->buf = (char *)_startAddr;
  _bufFreeList->bufSize = bufSize;
  _bufSize = bufSize;
  printf("Allocate buffer start = 0x%x, end = 0x%x\n",
	 _startAddr, _startAddr + bufSize - 1);
}

/**********************************************************
Allocate a range by given number of bytes 
**********************************************************/

RangeInfo *RangeInfoAlloc::AllocRange(int numBytes)
{
  /*
     printf("RangeInfoAlloc: %d bytes\n",numBytes);
  */
  if (numBytes <= 0){
    fprintf(stderr,"RangeInfoAlloc::AllocRange: can't alloc %d bytes\n",
	    numBytes);
    Exit::DoExit(1);
  }
  /* Go through free list and find first fit */
  RangeInfo *cur;
  RangeInfo *prev = NULL;
  for (cur = _bufFreeList; cur != NULL; prev = cur, cur = cur->next){
    int bufSize = cur->BufSize();
    if (bufSize >= numBytes){
      /* found it */
      RangeInfo *freeInfo = NULL;
      if (bufSize > numBytes){
	/* truncate */
	freeInfo = Truncate(cur, bufSize-numBytes);
      }
      
      if (prev == NULL){
	/* delete head of list */
	_bufFreeList = _bufFreeList->next;
	cur->next = NULL;
	if (freeInfo != NULL){
	  /* insert leftover back into free list */
	  freeInfo->next = _bufFreeList;
	  _bufFreeList = freeInfo;
	}
	/*
	   if (_bufFreeList != NULL)
	   printf("%d bytes left\n", _bufFreeList->bufSize);
	*/
	return cur;
      }
      else {
	prev->next = cur->next;
	cur->next = NULL;
	if (freeInfo != NULL){
	  /* insert leftover back into free list */
	  freeInfo->next = _bufFreeList;
	  _bufFreeList = freeInfo;
	}
	/*
	   if (_bufFreeList != NULL)
	   printf("%d bytes left\n", _bufFreeList->bufSize);
	*/
	return cur;
      }
    }
  }
  /* no match */
  if (_bufFreeList != NULL){
    /* return the 1st one */
    cur = _bufFreeList;
    _bufFreeList = _bufFreeList->next;
    return cur;
  }
  else
    /* nothing found */
    return NULL;
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
  if (numBytes >= rangeInfo->bufSize){
    fprintf(stderr,"RangeAlloc::Truncate: numbytes %d > bufSize %d\n",
	    numBytes,rangeInfo->bufSize);
    Exit::DoExit(2);
  }
  else if (numBytes <= 0){
    fprintf(stderr,"RangeAlloc::Truncate: numbytes %d <= 0\n", numBytes);
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
  if (_rangeInfoFreeList == NULL){
    rangeInfo = new RangeInfo();
  }
  else {
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
  while (_bufFreeList != NULL){
    RangeInfo *rangeInfo = _bufFreeList;
    _bufFreeList = _bufFreeList->next;
    FreeRangeInfo(rangeInfo);
  }
  
  _bufFreeList = AllocRangeInfo();
  _bufFreeList->next = NULL;
  _bufFreeList->buf = (char *)_startAddr;
  _bufFreeList->bufSize = _bufSize;
}
