/* RangeHash.c */
#include <stdio.h>
#include <libc.h>
#include "Exit.h"
#include "RangeHash.h"


/*****************************************************************
Constructor
******************************************************************/
RangeHash::RangeHash(){
	_numFind = _numFindSearch = 0;
	for (int i=0;i < RANGE_HASH_TABLE_SIZE; i++)
		_hashTable[i] = NULL;
}

/******************************************************************
Insert buffer into hash table. Error if already exists 
********************************************************************/
void RangeHash::Insert(RangeInfo *rangeInfo){
	TData *tdata= rangeInfo->GetTData();
	RecId id = rangeInfo->low;

	RangeInfo *temp;
	if (Find(tdata,id, temp)){
		fprintf(stderr,"RangeHash::Insert: buffer already exists\n");
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
void RangeHash::Delete(RangeInfo *buf){
	TData *tdata = buf->GetTData();
	RecId id = buf->low;

	if (buf->prevHash == NULL){
		/* deleting 1st element */
		int bucket = Hash(tdata, id);
		_hashTable[bucket] = buf->nextHash;
		if (buf->nextHash != NULL)
			buf->nextHash->prevHash = NULL;
	}
	else {
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
Boolean RangeHash::Find(TData *tdata, RecId id, RangeInfo *& buf){
	int bucket = Hash(tdata, id);
	RangeInfo *entry; 
	_numFind++;
	for (entry= _hashTable[bucket]; entry != NULL; entry= entry->nextHash){
		_numFindSearch++;
		if (entry->tdata == tdata && entry->low == id){
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
void RangeHash::Print(){
	int i;
	printf("Hash Table:\n");
	printf("buf\t\ttdata\t\tid\n");
	for (i=0;i < RANGE_HASH_TABLE_SIZE; i++){
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
void RangeHash::PrintStat(){
	printf("RangeInfo: Find called %d times, %d steps, %f steps/find\n",
		_numFind, _numFindSearch, (double)_numFindSearch/(double)_numFind);
}
