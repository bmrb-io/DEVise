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
  $Id$

  $Log$
  Revision 1.10  1997/07/22 15:00:53  donjerko
  *** empty log message ***

  Revision 1.9  1997/06/21 22:48:00  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.8  1997/06/16 16:04:43  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.6  1997/03/28 16:07:25  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "RTreeRead.h"

RTreeReadExec::RTreeReadExec(
	genrtree_m* rtree_m, gen_rt_cursor_t* cursor, int dataSize,
	int numKeyFlds, int numAddFlds, Tuple* tuple,
	UnmarshalPtr* unmarshalPtrs, int* rtreeFldLens, int ridPosition) :
	rtree_m(rtree_m), cursor(cursor), dataSize(dataSize),
	numKeyFlds(numKeyFlds), numAddFlds(numAddFlds),
	tuple(tuple), unmarshalPtrs(unmarshalPtrs), 
	rtreeFldLens(rtreeFldLens){
	
	assert(!"rtree is not linked in");
}

RTreeReadExec::~RTreeReadExec(){
}

RTreeIndex::~RTreeIndex(){
}

Iterator* RTreeIndex::createExec(){
	return NULL;
}

int RTreeIndex::queryBoxSize(){
	return 0;
}

const Tuple* RTreeReadExec::getNext(){
	return NULL;
}

Offset RTreeReadExec::getNextOffset(){
	return Offset();
}

RecId RTreeReadExec::getRecId(){
	return 0;
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	return false;
}
