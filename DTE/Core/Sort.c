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
  Revision 1.5  1997/04/04 23:10:25  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.4  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "Sort.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "types.h"

#define MAX_MEM		16000000
#define RUNS		10


void Sort::initialize(){
	// creates all the runs
	// Generate run using input->getNext
	// when ready to output use inserter->insert(tuple) 
	// To create temp files use Inserter(), open ...
	// MemorySize is a constant (compile time)

	// use function pointers for sorting (types.h: getOperatorPtr).

	input->initialize();
}

bool Sort::getNext(Tuple* next){
	// open all the runs
	// To read temp file use StandardRead ...
	// use heap to get max/min
	// copy the next tuple over "next"

	return input->getNext(next);
}
