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
  Revision 1.2  1995/09/05 22:16:24  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "VoidHash.h"

VoidHashEntry *VoidHashEntry::_freeList = NULL;

/******************************************************************
Hash entry allocator
********************************************************************/

void *VoidHashEntry :: operator new(size_t sz)
{
  if (sz != sizeof(VoidHashEntry)){
    fprintf(stderr,"VoidHashEntry::new size %d not right\n",sz);
    Exit::DoExit(1);
  }

  VoidHashEntry *entry;
  if (_freeList == NULL)
    entry = new VoidHashEntry;
  else {
    entry = _freeList;
    _freeList = _freeList->next;
  }

  return entry;
}

/******************************************************************
Hash entry destructor 
********************************************************************/

void VoidHashEntry:: operator delete(void *ptr)
{
  VoidHashEntry *entry = (VoidHashEntry *)ptr;
  entry->next = _freeList;
  _freeList = entry;
}

/*****************************************************************
Constructor
******************************************************************/

VoidHash::VoidHash()
{
  _numFind = 0;
  _numFindSearch = 0;
  
  for(int i = 0;i < VOID_HASH_TABLE_SIZE; i++)
    _hashTable[i] = NULL;
}

/******************************************************************
Insert data into hash table. Error if already exists 
********************************************************************/

void VoidHash::Insert(void *val, void *data)
{
  void *temp;
  if (Find(val,temp)){
    fprintf(stderr,"VoidHash::Insert: buffer already exists\n");
    Exit::DoExit(1);
  }
  
  int bucket = Hash(val);
  VoidHashEntry *entry = new VoidHashEntry;
  entry->data = data;
  entry->val = val;
  entry->next = _hashTable[bucket];
  entry->prev = NULL;
  if (entry->next!= NULL)
    entry->next->prev = entry;
  _hashTable[bucket] = entry;
}

/******************************************************************
Delete buffer from hash table.
********************************************************************/

void VoidHash::Delete(void *val)
{
  VoidHashEntry *entry;
  if (!Find(val, entry)){
    fprintf(stderr,"VoidHash::Delete: can't find val %x\n", val);
    Exit::DoExit(2);
  }
  if (entry->prev== NULL){
    /* deleting 1st element */
    int bucket = Hash(val);
    _hashTable[bucket] = entry->next;
    if (entry->next!= NULL)
      entry->next->prev= NULL;
  } else {
    /* somewhere in the middle or end of list in hash table  */
    if (entry->next!= NULL)
      entry->next->prev= entry->prev;
    entry->prev->next= entry->next;
  }
  entry->next= entry->prev= NULL;
}

/******************************************************************
Find entry for the val. Return TRUE if found 
********************************************************************/

Boolean VoidHash::Find(void *val, VoidHashEntry *&retEntry)
{
  int bucket = Hash(val);
  VoidHashEntry *entry; 
  _numFind++;
  for(entry= _hashTable[bucket]; entry != NULL; entry= entry->next){
    _numFindSearch++;
    if (entry->val == val){
      /* found */
      retEntry = entry;
      return true;
    }
  }

  /* not found */
  return false;
}

/******************************************************************
Find entry for the val. Return TRUE if found 
********************************************************************/

Boolean VoidHash::Find(void *val, void *&data)
{
  Boolean stat;
  VoidHashEntry *entry;
  if (stat=Find(val, entry)){
    data = entry->data;
  }
  return stat;
}

/****************************************************************
print the hash table
*****************************************************************/

void VoidHash::Print()
{
  int i;
  printf("Hash Table:\n");
  printf("buf\t\tpfile\t\tpage\n");
  for (i=0;i < VOID_HASH_TABLE_SIZE; i++){
    VoidHashEntry *entry;
    for (entry= _hashTable[i]; entry != NULL; entry= entry->next){
      printf("%x\t\t%x\t\t%x\n",entry, entry->val, entry->data);
    }
  }
}

/********************************************************************
Print Statistics
*********************************************************************/

void VoidHash::PrintStat()
{
  printf("VoidHash:: Find called %d times, %d steps, %f steps/find\n",
	 _numFind, _numFindSearch, (double)_numFindSearch/(double)_numFind);
}
