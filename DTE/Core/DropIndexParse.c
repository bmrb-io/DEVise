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
 */

#include "ParseTree.h"
#include <assert.h>
#include "Engine.h"

Site* DropIndexParse::createSite(){
	String query = String("delete .sysind as t where t.table = \"") +
		tableName->toString() + "\" and t.name = \"" + *indexName + "\"";
	cout << "in drop index with query:\n" << query << endl;
	Engine engine(query);
     TRY(engine.optimize(), NULL);
	return new Site();
}
