/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Dummy implementation of DataRead and related classes.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/06/03 22:10:02  okan
  *** empty log message ***

  Revision 1.3  1998/01/07 19:25:56  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.2.4.1  1997/12/17 17:29:18  wenger
  Fixed bugs 235 and 258 (core dump on Tables with long strings -- caused
  by insufficient buffer size in DTE); added headers to some DTE files
  and fixed some compile warnings.

 */
#include "sysdep.h"
#include "types.h"

#if !defined(SGI)
#undef assert
#endif
#include <assert.h>

#include "DataRead.h"


void DataRead::translateUDInfo() {
}

DataRead::DataRead(const string& schemaFile, const string& dataFile) :
          ud(NULL), numFlds(0), typeIDs(NULL),
          attributeNames(NULL),
          order(NULL)
{
}

Iterator* DataRead::createExec(){
	return NULL;
}

DataReadExec::DataReadExec(DataReader* ud, UnmarshalPtr* unmarshalPtrs,
	DestroyPtr* destroyPtrs,
	Type** tuple, int* offsets, int numFlds) :
	ud(ud), unmarshalPtrs(unmarshalPtrs),
	destroyPtrs(destroyPtrs), tuple(tuple),
	offsets(offsets), numFlds(numFlds) {
}

DataReadExec::~DataReadExec(){
}

const Tuple* DataReadExec::getNext(){
	return NULL;
}

const Tuple* DataReadExec::getThis(Offset offset, RecId recId){
	return NULL;
}

void DataRead::Close(){
}

