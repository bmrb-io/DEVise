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
  Revision 1.5  1997/07/30 21:39:21  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.4  1997/06/21 22:48:05  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <string>
#include "ParseTree.h"
#include "exception.h"
#include "site.h"

Site* UnionParse::createSite(){	// throws exception;
	assert(query1);
	assert(query2);
	TRY(Site* iter1 = query1->createSite(), NULL);
	TRY(Site* iter2 = query2->createSite(), NULL);
	assert(iter1);
	assert(iter2);
	int numFlds1 = iter1->getNumFlds();
	int numFlds2 = iter2->getNumFlds();
	if(numFlds1 != numFlds2){
		string msg = 
			"Cannot do UNION because numbers of fields do not match";
		THROW(new Exception(msg), NULL);
	}
	const TypeID* types1 = iter1->getTypeIDs();
	const TypeID* types2 = iter2->getTypeIDs();
	for(int i = 0; i < numFlds1; i++){
		if(types1[i] != types2[i]){
			string msg = "Cannot do UNION because types do not match";
		}
	}
	return new UnionSite(iter1, iter2);
}
