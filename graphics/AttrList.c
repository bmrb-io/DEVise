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
  Revision 1.16  1997/05/28 20:01:41  andyt
  Shape attributes for 'Tcl/Tk Window' shape can now be variables, numeric
  constants, or string constants. Used to assume that all attributes except
  argc were strings.

  Revision 1.15  1997/05/03 19:53:42  wenger
  Fixed bug in AttrList class that caused Devise to crash on mappings
  that use recId; commented out debug code in UnixRecFile.c..

  Revision 1.14  1997/04/25 23:15:48  ssl
  turned off debug code.

  Revision 1.12  1997/03/11 22:49:46  donjerko
  *** empty log message ***

  Revision 1.11  1997/03/07 17:41:40  donjerko
  Added method Clear() that resets attrList to empty.

  Revision 1.10  1996/09/27 15:52:33  wenger
  Fixed a number of memory leaks.

  Revision 1.9  1996/07/23 20:12:39  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.8  1996/05/07 16:14:19  jussi
  Added copy constructor and GetVal() method.

  Revision 1.7  1996/01/10 00:38:16  jussi
  Minor changes.

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
#include <unistd.h>

#include "AttrList.h"
#include "Util.h"
#include "DevError.h"

static char *GetTypeString(AttrType type);
static void WriteVal(int fd, AttrVal *aval, AttrType atype);

AttrList::AttrList(char *name)
{
  for(int i = 0; i < MAX_ATTRLIST_SIZE; i++)
    _attrs[i] = NULL;
  _size = 0;
  _name = name;
}

void AttrList::Clear(){
  for(int i = 0; i < MAX_ATTRLIST_SIZE; i++){
    delete _attrs[i];
    _attrs[i] = NULL;
  }
  _size = 0;
}

AttrList::~AttrList()
{
  for(int i = 0; i < _size; i++) {
    if (_attrs[i] != NULL)
      delete _attrs[i];
  }
}

/* Copy constructor */
AttrList::AttrList(AttrList &attrs)
{
  for(int i = 0; i < MAX_ATTRLIST_SIZE; i++)
    _attrs[i] = NULL;
  _size = 0;
  _name = CopyString(attrs.GetName());

  /* copy attributes from attrs to this list */

  attrs.InitIterator();
  while(attrs.More()) {
    AttrInfo *info = attrs.Next();
    InsertAttr(info->attrNum, info->name, info->offset,
	       info->length, info->type, info->hasMatchVal,
	       &info->matchVal, info->isComposite, info->isSorted,
	       info->hasHiVal, &info->hiVal, info->hasLoVal,
	       &info->loVal);
  }
  attrs.DoneIterator();
}

Boolean
AttrList::SetAttrs(const AttrList &newAttrs)
{
  Clear();

  // Bypass interator to preserve 'const' on newAttrs.
  int index = 0;
  while(index < newAttrs._size) {
    AttrInfo *info = newAttrs._attrs[index];
    InsertAttr(info->attrNum, info->name, info->offset,
	       info->length, info->type, info->hasMatchVal,
	       &info->matchVal, info->isComposite, info->isSorted,
	       info->hasHiVal, &info->hiVal, info->hasLoVal,
	       &info->loVal);
    index++;
  }

  return true;
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
    if (info && !strcmp(info->name, name))
      return info;
  }
  return NULL;
}

AttrInfo *AttrList::FindShapeAttr(int i)
{
    char attrName[40];
    sprintf(attrName, "shapeAttr_%d", i);
    return Find(attrName);
}

int AttrList::GetAttrNum(char *name)
{
  for(int index = 0; index < _size; index++) {
    AttrInfo *info = _attrs[index];
    if (info && !strcmp(info->name, name))
      return index;
  }
  return -1;
}


AttrInfo *AttrList::Get(int n)
{
  if (n >= 0  && n < _size) {
    return _attrs[n];
  } else {
    /* Note: -1 is used to mean recId. */
    if (n != -1
#if defined(DEBUG)
      || true
#endif
    )
    {
      char errBuf[256];
      sprintf(errBuf, "Attribute %d does not exist", n);
      reportErrNosys(errBuf);
    }
    return NULL;
  }
}


double AttrList::GetVal(AttrVal *aval, AttrType atype)
{
  switch(atype) {
    case IntAttr:
      return aval->intVal;
    case FloatAttr:
      return aval->floatVal;
      break;
    case DoubleAttr:
      return aval->doubleVal;
    case StringAttr:
      DOASSERT(0, "Cannot get value of string attribute");
      break;
    case DateAttr:
      return aval->dateVal;
    default:
      DOASSERT(0, "Unknown attribute type");
      break;
  }

  /* make compiler happy */
  return -1;
}

void AttrList::Print()
{
  printf("AttrList:\n");
  for (InitIterator(); More(); ){
    AttrInfo *info = Next();
    printf("  name %s, num %d, offset %d, length %d, composite %d, ",
	   info->name, info->attrNum, info->offset, info->length,
	   (info->isComposite? 1 : 0));
    printf("%s", GetTypeString(info->type));
    if (info->hasHiVal) {
      printf(", hi ");
      PrintVal(&(info->hiVal), info->type);
    }
    if (info->hasLoVal) {
      printf(", lo ");
      PrintVal(&(info->loVal), info->type);
    }
    printf("\n");
  }

  DoneIterator();
}

void AttrList::Write(int fd)
{
  for (InitIterator(); More(); )
  {
    AttrInfo *infoP = Next();

    char *string;

    // Sorted?
    if (infoP->isSorted)
    {
      string = "sorted ";
      write (fd, string, strlen(string));
    }

    // Composite or normal attribute?
    if (infoP->isComposite)
    {
      string = "compattr ";
    }
    else
    {
      string = "attr ";
    }
    write (fd, string, strlen(string));

    // Name.
    write (fd, infoP->name, strlen(infoP->name));
    write (fd, " ", 1);

    // Type.
    string = GetTypeString(infoP->type);
    write (fd, string, strlen(string));
    write (fd, " ", 1);

    // Length.
    if (infoP->type == StringAttr)
    {
      char buf[100];
      sprintf(buf, "%d ", infoP->length);
      write (fd, buf, strlen(buf));
    }

    // Match value.  TEMPTEMP -- before or after length??
    if (infoP->hasMatchVal)
    {
      string = "= ";
      write (fd, string, strlen(string));

      WriteVal(fd, &infoP->matchVal, infoP->type);
    }

    // Hi.
    if (infoP->hasHiVal)
    {
      string = "hi ";
      write (fd, string, strlen(string));

      WriteVal(fd, &infoP->hiVal, infoP->type);
    }

    // Lo.
    if (infoP->hasLoVal)
    {
      string = "lo ";
      write (fd, string, strlen(string));

      WriteVal(fd, &infoP->loVal, infoP->type);
    }

    write (fd, "\n", 1);
  }

  DoneIterator();
}

void AttrList::PrintVal(AttrVal *aval, AttrType atype)
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

static char *
GetTypeString(AttrType type)
{
  switch(type)
  {
  case IntAttr:
    return "int";
    break;

  case FloatAttr:
    return "float";
    break;

  case DoubleAttr:
    return "double";
    break;

  case StringAttr:
    return "string";
    break;

  case DateAttr:
    return "date";
    break;

  default:
    DOASSERT(0, "Unknown attribute type");
    return ""; // So compiler is happy.
    break;
  }
}

static void
WriteVal(int fd, AttrVal *aval, AttrType atype)
{
  char buf[100];

  switch(atype)
  {
  case IntAttr:
    sprintf(buf, "%d ", aval->intVal);
    break;

  case FloatAttr:
    sprintf(buf, "%f ", aval->floatVal);
    break;

  case DoubleAttr:
    sprintf(buf, "%f ", aval->doubleVal);
    break;

  case StringAttr:
    sprintf(buf, "%s ", aval->strVal);
    break;

  case DateAttr:
    sprintf(buf, "%ld ", (long)aval->dateVal);//TEMPTEMP?
    break;

  default:
    DOASSERT(0, "Unknown attribute type");
    break;
  }

  write (fd, buf, strlen(buf));

  return;
}
