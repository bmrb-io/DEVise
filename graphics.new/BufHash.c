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
  Revision 1.2  1995/09/05 22:14:06  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "BufHash.h"

/*****************************************************************
Constructor
******************************************************************/

BufHash::BufHash()
{
  for(int i = 0;i < HASH_TABLE_SIZE; i++)
    _hashTable[i] = NULL;
}

/******************************************************************
Insert buffer into hash table. Error if already exists 
********************************************************************/

void BufHash::Insert(BufPage *buf)
{
  DiskFile *pfile= buf->GetDiskFile();
  int page = buf->PageNum();
  
  BufPage *temp;
  if (Find(pfile,page, temp)) {
    fprintf(stderr,"BufHash::Insert: buffer already exists\n");
    Exit::DoExit(1);
  }
  
  int bucket = Hash(pfile, page);
  buf->nextHash = _hashTable[bucket];
  buf->prevHash = NULL;
  if (buf->nextHash != NULL)
    buf->nextHash->prevHash = buf;
  _hashTable[bucket] = buf;
}

/******************************************************************
Delete buffer from hash table.
********************************************************************/

void BufHash::Delete(BufPage *buf)
{
  DiskFile *pfile = buf->GetDiskFile();
  int page = buf->PageNum();
  
  if (buf->prevHash == NULL){
    /* deleting 1st element */
    int bucket = Hash(pfile, page);
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

Boolean BufHash::Find(DiskFile *pfile, int page, BufPage *& buf)
{
  int bucket = Hash(pfile, page);
  _numFind++;
  for(BufPage *entry= _hashTable[bucket]; entry != NULL;
      entry = entry->nextHash) {
    _numFindSearch++;
    if (entry->GetDiskFile()== pfile && entry->PageNum() == page) {
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

void BufHash::Print()
{
  printf("Hash Table:\n");
  printf("buf\t\tpfile\t\tpage\n");
  for(int i = 0;i < HASH_TABLE_SIZE; i++) {
    for(BufPage *entry = _hashTable[i]; entry != NULL;
	entry = entry->nextHash) {
      printf("%x\t\t%x\t\t%d\n", entry,
	     entry->GetDiskFile(), entry->PageNum());
    }
  }
}

/********************************************************************
Print Statistics
*********************************************************************/

void BufHash::PrintStat()
{
  printf("BufPage: Find called %d times, %d steps, %f steps/find\n",
	 _numFind, _numFindSearch, (double)_numFindSearch / (double)_numFind);
}
