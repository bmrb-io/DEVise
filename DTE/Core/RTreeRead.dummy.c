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
  Revision 1.8  1998/06/08 04:09:52  okan
  *** empty log message ***

  Revision 1.7  1998/06/08 03:47:52  okan
  *** empty log message ***

  Revision 1.6  1997/11/18 19:49:20  okan
  Made several changes for NT compilation

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

RTreeReadExec::RTreeReadExec(const string& _filename, int _root_page,
                BoundingBox* _bbox, const TypeIDList& _added_types)
{
	assert(!"rtree is not linked in");
}

RTreeReadExec::~RTreeReadExec(){
}

RTreeIndex::~RTreeIndex(){
}

Iterator* RTreeIndex::createExec(){
	return NULL;
}

const Tuple* RTreeReadExec::getNext(){
	return NULL;
}

void RTreeReadExec::initialize(){
	return;
}

const TypeIDList& RTreeReadExec::getTypes()
{
	TypeIDList retVal;
	return retVal;
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	return false;
}
