/* AttrList.c */
#include <stdio.h>
#include "AttrList.h"
#include "Util.h"

AttrList::AttrList(char *name){
	_attrs = new AttrInfoList();
	_name = name;
}

AttrList::~AttrList(){
	int index;
	for (index = _attrs->InitIterator(); _attrs->More(index); ){
		AttrInfo *info = (AttrInfo *)_attrs->Next(index);
		_attrs->DeleteCurrent(index);
		delete info;
	}
	_attrs->DoneIterator(index);
}

/* Insert attribute into list of attributes */
void AttrList::InsertAttr(int attrNum,
	char *name, int offset, int length, AttrType type,
	Boolean hasMatchVal, AttrVal *matchVal, Boolean isComposite,
	Boolean isSorted){
	AttrInfo *info = new AttrInfo;
	info->attrNum = attrNum;
	info->name = CopyString(name);
	info->offset = offset;
	info->length = length;
	info->type = type;
	info->hasMatchVal = hasMatchVal;
	if (hasMatchVal)
		info->matchVal = *matchVal;
	info->isComposite = isComposite;
	info->isSorted = isSorted;
	_attrs->Append(info);
}

/* iterator for list of attributes */
void AttrList::InitIterator(){
	_index = _attrs->InitIterator();
}
Boolean AttrList::More(){
	return _attrs->More(_index);
}
AttrInfo *AttrList::Next(){
	return (AttrInfo *)(_attrs->Next(_index)); 
}
void AttrList::DoneIterator(){
	_attrs->DoneIterator(_index);
}

AttrInfo *AttrList::Find(char *name){
	int index;
	for (index = _attrs->InitIterator(); _attrs->More(index); ){
		AttrInfo *info = (AttrInfo *)_attrs->Next(index);
		if (!strcmp(info->name,name)){
			_attrs->DoneIterator(index);
			return info;
		}
	}
	_attrs->DoneIterator(index);
	return NULL;
}

/* Get ith TData info */
AttrInfo *AttrList::Get(int n){
	if (n < 0 || n >= _attrs->Size()){
		fprintf(stderr,"AttrList::Get(%d): %d existing attrs\n",
			n,_attrs->Size());
		Exit::DoExit(2);
	}
	int i,index;
	for (i=0, index = _attrs->InitIterator(); _attrs->More(index); i++){
		AttrInfo *info = (AttrInfo *)_attrs->Next(index);
		if (i == n){
			_attrs->DoneIterator(index);
			return info;
		}
	}
	_attrs->DoneIterator(index);
	Exit::DoExit(2);
}

void AttrList::Print() {
	printf("AttrList ");
	for (InitIterator(); More(); ){
		AttrInfo *info = Next();
		printf("name %s, num %d, offset %d, length %d, composite %d,",
			info->name, info->attrNum, info->offset, info->length,
			(info->isComposite? 1: 0));
		switch(info->type){
			case IntAttr:
				printf("int");
				break;
			case FloatAttr:
				printf("double");
				break;
			case StringAttr:
				printf("string");
				break;
			case DateAttr:
				printf("date");
				break;
		}
		printf("\n");
	}
}
