/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "DictGroup.h"

DictGroup::DictGroup(){
	int i;
	_numKeys = 0;
	for (i=0; i < DictGroup_MaxKeys; i++){
		_entries[i].list = new VoidList;
	}
}

/* insert value with given key. Values are sorted by key */
void DictGroup::Append(void *key, void *val){
	int i;
	for (i=0; i < _numKeys; i++){
		if (_entries[i].key == key){
			if (!_entries[i].list->Find(val))
				_entries[i].list->Append(val);
			return;
		}
	}
	/* add a new key */
	if (_numKeys == DictGroup_MaxKeys){
		fprintf(stderr,"DictGroup: too many keys. max = %d\n", DictGroup_MaxKeys);
		Exit::DoExit(1);
	}
	_entries[_numKeys].key = key;
	_entries[_numKeys].list->Append(val);
	_numKeys++;
}

/* Delete key/val */
void DictGroup::Delete(void *key, void *val){
	int i;
	for (i=0; i < _numKeys; i++){
		if (_entries[i].key == key){
			(void)_entries[i].list->Delete(val);
			return;
		}
	}
}

/* Iterator to find value given key */
void DictGroup::InitIterator(void *key){
	int i;
	for (i=0; i < _numKeys; i++){
		if (_entries[i].key == key){
			_keyIndex = i;
			_iteratorIndex = _entries[i].list->InitIterator();
			return;
		}
	}
	/* no iterator */
	_keyIndex = -1;
	_iteratorIndex = -1;
}

Boolean DictGroup::More(){
	if (_keyIndex < 0)
		return false;
	else return _entries[_keyIndex].list->More(_iteratorIndex);
}

void *DictGroup::Next(){
	if (_keyIndex < 0){
		fprintf(stderr,"DictGroup::Next called but no more data\n");
		Exit::DoExit(1);
	}

	return _entries[_keyIndex].list->Next(_iteratorIndex);
}

void DictGroup::DoneIterator() {
	if (_keyIndex >= 0)
		_entries[_keyIndex].list->DoneIterator(_iteratorIndex);
}

/* Get ith key */
void *DictGroup::Key(int i){
	if (i < 0 || i >= _numKeys){
		fprintf(stderr,"DictGroup::Key(%d): index < 0 or >= max keys(%d)\n",
			i,_numKeys);
		Exit::DoExit(2);
	}

	return _entries[i].key;
}
