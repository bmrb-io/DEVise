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
  Revision 1.2  1997/03/28 16:07:23  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "ParseTree.h"
#include <assert.h>
#include "Engine.h"

Site* DropIndexParse::createSite(){
	string query = string("delete .sysind as t where t.table = \"") +
		tableName->toString() + "\" and t.name = \"" + *indexName + "\"";
	cout << "in drop index with query:\n" << query << endl;
	Engine engine(query);
     TRY(engine.optimize(), NULL);
	return new Site();
}
