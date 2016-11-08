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
  Revision 1.4  1997/10/07 17:06:05  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.3  1996/01/12 15:23:31  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:15:21  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "RangeHash.h"

/*****************************************************************
Constructor
******************************************************************/

RangeHash::RangeHash()
{
  _numFind = _numFindSearch = 0;
  for (int i=0;i < RANGE_HASH_TABLE_SIZE; i++)
    _hashTable[i] = NULL;
}

/******************************************************************
Insert buffer into hash table. Error if already exists 
********************************************************************/

void RangeHash::Insert(RangeInfo *rangeInfo)
{
  TData *tdata= rangeInfo->GetTData();
  Coord id = rangeInfo->low;
  
  RangeInfo *temp;
  if (Find(tdata,id, temp)) {
    fprintf(stderr,"RangeHash::Insert: buffer already exists\n");
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }
  
  int bucket = Hash(tdata, id);
  rangeInfo->nextHash = _hashTable[bucket];
  rangeInfo->prevHash = NULL;
  if (rangeInfo->nextHash != NULL)
    rangeInfo->nextHash->prevHash = rangeInfo;
  _hashTable[bucket] = rangeInfo;
}

/******************************************************************
Delete buffer from hash table.
********************************************************************/

void RangeHash::Delete(RangeInfo *buf)
{
  TData *tdata = buf->GetTData();
  Coord id = buf->low;
  
  if (buf->prevHash == NULL){
    /* deleting 1st element */
    int bucket = Hash(tdata, id);
    _hashTable[bucket] = buf->nextHash;
    if (buf->nextHash != NULL)
      buf->nextHash->prevHash = NULL;
  } else {
    /* somewhere in the middle or end of list in hash table  */
    if (buf->nextHash != NULL)
      buf->nextHash->prevHash = buf->prevHash;
    buf->prevHash->nextHash = buf->nextHash;
  }
  buf->nextHash = buf->prevHash = NULL;
}

/******************************************************************
Find buffer entry for the given page. Return TRUE if found 
********************************************************************/

Boolean RangeHash::Find(TData *tdata, Coord id, RangeInfo *& buf)
{
  int bucket = Hash(tdata, id);
  _numFind++;
  for(RangeInfo *entry= _hashTable[bucket]; entry != NULL;
      entry= entry->nextHash) {
    _numFindSearch++;
    if (entry->tdata == tdata && entry->low == id) {
      /* found */
      buf = entry;
      return true;
    }
  }

  /* not found */
  return false;
}

/****************************************************************
print the hash table
*****************************************************************/

void RangeHash::Print()
{
  printf("Hash Table:\n");
  printf("buf\t\ttdata\t\tid\n");
  for(int i = 0;i < RANGE_HASH_TABLE_SIZE; i++) {
    RangeInfo *entry;
    for (entry= _hashTable[i]; entry != NULL; entry= entry->nextHash){
      printf("%x\t\t%x\t\t%d\n",entry,
	     entry->tdata, entry->low);
    }
  }
}

/********************************************************************
Print Statistics
*********************************************************************/

void RangeHash::PrintStat()
{
  printf("RangeInfo: Find called %d times, %d steps, %f steps/find\n",
	 _numFind, _numFindSearch, (double)_numFindSearch / (double)_numFind);
}
