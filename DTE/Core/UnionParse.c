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
  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <String.h>
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
		String msg = 
			"Cannot do UNION because numbers of fields do not match";
		THROW(new Exception(msg), NULL);
	}
	TypeID* types1 = iter1->getTypeIDs();
	TypeID* types2 = iter2->getTypeIDs();
	for(int i = 0; i < numFlds1; i++){
		if(types1[i] != types2[i]){
			String msg = "Cannot do UNION because types do not match";
		}
	}
	return new UnionSite(iter1, iter2);
}
