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
  Revision 1.10  1998/07/24 04:37:57  donjerko
  *** empty log message ***

  Revision 1.9  1997/12/04 04:05:17  donjerko
  *** empty log message ***

  Revision 1.8  1997/11/12 23:17:37  donjerko
  Improved error checking.

  Revision 1.7  1997/10/02 02:27:29  donjerko
  Implementing moving aggregates.

  Revision 1.6  1997/08/21 21:04:27  donjerko
  Implemented view materialization

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
//#include "site.h"
#include "Iterator.h"
#include "ExecOp.h"
#include "DTE/util/Del.h"

Iterator* UnionParse::createExec(){	// throws exception;

	string msg = "UNION queries are temporarily broken";
	THROW(new Exception(msg), NULL);

	assert(query1);
	assert(query2);
        Del<Iterator> iter1 = query1->createExec();
        assert(iter1);
        Del<Iterator> iter2 = query2->createExec();
        assert(iter2);

        DteTupleAdt adt1 = iter1->getAdt();
        DteTupleAdt adt2 = iter2->getAdt();

        if( ! adt1.isExactSameType(adt2) ) {
          string msg = "Cannot do UNION because types do not match";
          THROW(new Exception(msg), NULL);
        }
        return new UnionExec(iter1.steal(), iter2.steal());

#if 0
        //kb: old stuf
	Site* iter1;
	Site* iter2;
//	TRY(iter1 = query1->createSite(), NULL);
//	TRY(iter2 = query2->createSite(), NULL);
	assert(iter1);
	assert(iter2);
	int numFlds1 = iter1->getNumFields();
	int numFlds2 = iter2->getNumFields();
	if(numFlds1 != numFlds2){
		string msg = 
			"Cannot do UNION because numbers of fields do not match";
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}
	const TypeID* types1 = iter1->getTypeIDs();
	const TypeID* types2 = iter2->getTypeIDs();
	for(int i = 0; i < numFlds1; i++){
		if(!(types1[i] == types2[i])){
			string msg = "Cannot do UNION because types do not match";
		}
	}
//	return new UnionSite(iter1, iter2);
	return 0;
#endif
}
