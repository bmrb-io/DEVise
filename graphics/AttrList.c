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
  Revision 1.6  1995/12/20 07:04:08  ravim
  High and low values of attrs can be specified.

  Revision 1.5  1995/12/13 18:42:43  jussi
  Now iterator works even if attribute list has holes i.e. list is
  non-contiguous.

  Revision 1.4  1995/11/21 21:10:43  jussi
  Added copyright notice and cleaned up a bit.

  Revision 1.3  1995/09/05 21:12:22  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:21  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "AttrList.h"
#include "Util.h"

AttrList::AttrList(char *name)
{
  for(int i = 0; i < MAX_ATTRLIST_SIZE; i++)
    _attrs[i] = NULL;
  _size = 0;
  _name = name;
}

AttrList::~AttrList()
{
  for(int i = 0; i < _size; i++) {
    if (_attrs[i] != NULL)
      delete _attrs[i];
  }
}

/* Insert attribute into list of attributes */
void AttrList::InsertAttr(int attrNum, char *name, int offset, int length,
			  AttrType type, Boolean hasMatchVal,
			  AttrVal *matchVal, Boolean isComposite,
			  Boolean isSorted, Boolean hasHiVal, AttrVal *hiVal,
			  Boolean hasLoVal, AttrVal *loVal)
{
  if (attrNum < 0 || attrNum >= MAX_ATTRLIST_SIZE) {
    fprintf(stderr,"AttrList::InsertAttr: invalid attrNum %d\n", attrNum);
    Exit::DoExit(1);
  }

  if (_attrs[attrNum] != NULL) {
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
  info->hasHiVal = hasHiVal;
  if (hasHiVal)
    info->hiVal = *hiVal;
  info->hasLoVal = hasLoVal;
  if (hasLoVal)
    info->loVal = *loVal;
  
  if (_size < attrNum+1)
    _size = attrNum+1;
  _attrs[attrNum] = info;
}

void AttrList::InitIterator()
{
  _index = 0;
}

Boolean AttrList::More()
{
  return (_index < _size);
}

AttrInfo *AttrList::Next()
{
  AttrInfo *info;

  do {
    info = _attrs[_index];
    _index++;
  } while (!info && _index < _size);

  return info;
}

void AttrList::DoneIterator()
{
}

AttrInfo *AttrList::Find(char *name)
{
  for(int index = 0; index < _size; index++) {
    AttrInfo *info = _attrs[index];
    if ( info != NULL && !strcmp(info->name,name)){
      return info;
    }
  }
  return NULL;
}

AttrInfo *AttrList::Get(int n)
{
  if (n < 0 || n >= _size) {
    fprintf(stderr, "AttrList::Get(%d): %d existing attrs\n", n, _size);
    Exit::DoExit(2);
  }
  return _attrs[n];
}

void AttrList::Print()
{
  printf("AttrList:\n");
  for (InitIterator(); More(); ){
    AttrInfo *info = Next();
    printf("  name %s, num %d, offset %d, length %d, composite %d, ",
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
    if (info->hasHiVal) {
      printf(", hi ");
      printVal(&(info->hiVal), info->type);
    }
    if (info->hasLoVal) {
      printf(", lo ");
      printVal(&(info->loVal), info->type);
    }
    printf("\n");
  }
}

void AttrList::printVal(AttrVal *aval, AttrType atype)
{
  switch(atype) {
    case IntAttr:
      printf(" %d ", aval->intVal);
      break;
    case FloatAttr:
      printf(" %f ", aval->floatVal);
      break;
    case DoubleAttr:
      printf(" %f ", aval->doubleVal);
      break;
    case StringAttr:
      printf(" %s ", aval->strVal);
      break;
    case DateAttr:
      printf(" %ld ", (long)aval->dateVal);
      break;
    default:
      break;
  }
}
