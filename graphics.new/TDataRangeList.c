/* TDataRangeList.c */
#include "TDataRangeList.h"
#include "RangeList.h"

TDataRangeListEntry *TDataRangeList::_freeList = NULL;

/* Constructor */
TDataRangeList::TDataRangeList (){
	int i;
	for (i=0; i < TDataRangeListHashSize; i++)
		_hashTbl[i] = NULL;
}

TDataRangeListEntry *TDataRangeList::Find(TData *tdata){
	int bucket = Hash(tdata);
	TDataRangeListEntry *cur;
	for (cur=_hashTbl[bucket]; cur != NULL; cur = cur->next){
		if (cur->tdata == tdata)
			break;
	}
	return cur;
}

RangeList *TDataRangeList::Get(TData *tdata){
	TDataRangeListEntry *cur = Find(tdata);
	if (cur == NULL){
		/* create a new one */
		cur = AllocEntry();
		cur->tdata = tdata;
		cur->rangeList = new RangeList;
		int bucket = Hash(tdata);
		cur->next = _hashTbl[bucket];
		_hashTbl[bucket] = cur;
	}
	return cur->rangeList;
}

TDataRangeListEntry *TDataRangeList::AllocEntry(){
	TDataRangeListEntry *entry;
	if (_freeList == NULL)
		entry =new TDataRangeListEntry();
	else {
		entry = _freeList;
		_freeList = _freeList->next;
	}
	return entry;
}

void TDataRangeList::FreeEntry(TDataRangeListEntry *entry) {
	entry->next = _freeList;
	_freeList = entry;
}

void TDataRangeList::Clear(){
	int i;
	for (i=0; i < TDataRangeListHashSize; i++){
		while(_hashTbl[i] != NULL){
			TDataRangeListEntry *entry = _hashTbl[i];
			_hashTbl[i] = _hashTbl[i]->next;
			delete entry->rangeList;
			FreeEntry(entry);
		}
	}
}
