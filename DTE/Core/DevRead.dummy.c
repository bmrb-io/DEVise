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
  Dummy implementation of DevRead and related classes.
 */

/*
  $Id$

  $Log$
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
//#include <strstream.h>   erased for sysdep.h

#undef assert
#include <assert.h>

#include "types.h"
#include "DevRead.h"


TypeID translateUDType(Attr* at){
   return "unknown";
}

void DevRead::Open(char* schemaFile, char* dataFile){ // throws
}

Iterator* DevRead::createExec(){
	return NULL;
}

DevReadExec::~DevReadExec(){
}

const Tuple* DevReadExec::getNext(){
	return NULL;
}

const Tuple* DevReadExec::getThis(Offset offset) {
	return NULL;
}

void DevRead::Close(){
}
