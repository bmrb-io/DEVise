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
  Revision 1.7  1997/09/05 22:20:04  donjerko
  Made changes for port to NT.

  Revision 1.6  1997/08/22 23:13:02  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.5  1997/08/21 21:04:20  donjerko
  Implemented view materialization

  Revision 1.4  1997/08/04 14:50:47  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.3  1997/07/30 21:39:17  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.2  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
//#include<stdio.h>	// for perror   erased for sysdep.h
//#include<errno.h>   erased for sysdep.h

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "listop.h"
#include "ParseTree.h"
#include "sysdep.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

#define DEBUG

Site* InsertParse::createSite(){
	LOG(logFile << "Inserting into ");
	LOG(tableName->display(logFile));
	LOG(logFile << " (";)
	LOG(displayList(logFile, (List<BaseSelection*>*) fieldList, ", ");)
	LOG(logFile << ")" << endl;)

     TRY(Site* site = ROOT_CATALOG.find(tableName), NULL);

	assert(site);

	TRY(site->typify(""), NULL);
	int numFlds = site->getNumFlds();
/*
	// this is not satisfied because we are inserting strings
	if(numFlds != fieldList->cardinality()){
		THROW(new Exception("Number of fields do not match"), NULL);
	}
*/
	const string* types = site->getTypeIDs();

// What follows is just a temporary kludge. 
// One should construct a tuple and inset it

	fieldList->rewind();
	assert(fieldList->cardinality() == 1);
	ConstantSelection* sel = fieldList->get();
	stringstream tmp;
	sel->display(tmp);
	string inStr;
	stripQuotes(tmp, inStr);
#if defined(DEBUG)
	cerr << "Appending to the file: " << inStr << endl;
#endif
	TRY(site->writeOpen(), NULL);
	site->write(inStr);
	site->writeClose();
	delete site;
	return new Site();
}
