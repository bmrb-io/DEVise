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
  Revision 1.5  1997/11/13 22:19:24  okan
  Changes about compilation on NT

  Revision 1.4  1997/08/10 20:30:55  donjerko
  Fixed the NO_RTREE option.

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

RTreeReadExec::RTreeReadExec(const IndexDesc& indexDesc,
	int dataSize, Tuple* tuple,
	UnmarshalPtr* unmarshalPtrs, int* rtreeFldLens, int ridPosition,
	typed_key_t* queryBox) {
	
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

void RTreeReadExec::initialize(){
	return;
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	return false;
}
