/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:39:21  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "AttrList.h"
#include "Util.h"

AttrList::AttrList(char *name){
	int i;
	for (i=0; i < MAX_ATTRLIST_SIZE; i++)
		_attrs[i] = NULL;
	_size = 0;
	_name = name;
}

AttrList::~AttrList(){
	int i;
	for (i=0; i < _size; i++){
		if (_attrs[i] != NULL)
			delete _attrs[i];
	}
}

/* Insert attribute into list of attributes */
void AttrList::InsertAttr(int attrNum,
	char *name, int offset, int length, AttrType type,
	Boolean hasMatchVal, AttrVal *matchVal, Boolean isComposite,
	Boolean isSorted){

	if (attrNum < 0 || attrNum >= MAX_ATTRLIST_SIZE){
		fprintf(stderr,"AttrList::InsertAttr: invalid attrNum %d\n", attrNum);
		Exit::DoExit(1);
	}
	if (_attrs[attrNum] != NULL){
		fprintf(stderr,"AttrList::InsertAttr: attrNum %d already exists\n",
			attrNum);
		Exit::DoExit(1);
	}

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

	if (_size < attrNum+1)
		_size = attrNum+1;
	_attrs[attrNum] = info;
}

/* iterator for list of attributes */
void AttrList::InitIterator(){
	_index = 0;
}
Boolean AttrList::More(){
	return (_index < _size);
}
AttrInfo *AttrList::Next(){
	AttrInfo *info = _attrs[_index];
	_index++;
	return info;
}
void AttrList::DoneIterator(){
}

AttrInfo *AttrList::Find(char *name){
	int index;
	for (index = 0; index < _size; index++){
		AttrInfo *info = _attrs[index];
		if ( info != NULL && !strcmp(info->name,name)){
			return info;
		}
	}
	return NULL;
}

/* Get ith TData info */
AttrInfo *AttrList::Get(int n){
	if (n < 0 || n >= _size ){
		fprintf(stderr,"AttrList::Get(%d): %d existing attrs\n", n,_size);
		Exit::DoExit(2);
	}
	return _attrs[n];
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
				printf("float");
				break;
			case DoubleAttr:
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
