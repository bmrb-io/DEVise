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
  Revision 1.19  1997/12/04 04:05:08  donjerko
  *** empty log message ***

  Revision 1.18  1997/11/24 23:13:14  weaver
  Changes for the new ColorManager.

  Revision 1.17  1997/11/23 21:23:26  donjerko
  Added ODBC stuff.

  Revision 1.16  1997/11/13 22:19:22  okan
  Changes about compilation on NT

  Revision 1.15  1997/11/12 23:17:19  donjerko
  Improved error checking.

  Revision 1.14  1997/11/05 00:19:35  donjerko
  Separated typechecking from optimization.

  Revision 1.13  1997/10/02 02:27:25  donjerko
  Implementing moving aggregates.

  Revision 1.12  1997/09/29 02:51:54  donjerko
  Eliminated class GlobalSelect.

  Revision 1.11  1997/09/05 22:20:01  donjerko
  Made changes for port to NT.

  Revision 1.10  1997/08/22 23:13:00  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.9  1997/08/21 21:04:15  donjerko
  Implemented view materialization

  Revision 1.8  1997/08/04 14:50:46  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.7  1997/07/30 21:39:14  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.6  1997/06/21 22:47:56  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.5  1997/06/16 16:04:38  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.3  1997/03/28 16:07:23  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include<string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
//#include<stdio.h>	// for perror   erased for sysdep.h
// #include<errno.h>   erased for sysdep.h

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "listop.h"
#include "ParseTree.h"
#include "ExecExpr.h"
#include "sysdep.h"
#include "MemoryMgr.h"
#include "TypeCheck.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

Iterator* DeleteParse::createExec(){
	LOG(logFile << "Deleting from ");
	LOG(tableName->display(logFile));
	LOG(logFile << " tuples satisfying ";)
	LOG(predicate->display(logFile));
	LOG(logFile << endl;)

	TableName* tableNameCopy = new TableName(*tableName);

	// the above line is needed because the find function changes its argument

	TRY(Site* site = ROOT_CATALOG.find(tableName), NULL);
	assert(site);
	TableAlias ta(tableNameCopy, alias);
	site->addTable(&ta);

	vector<TableAlias*> tableList;
	tableList.push_back(&ta);
	TypeCheck typeCheck;
	TRY(typeCheck.initialize(tableList), 0);
	vector<BaseSelection*> predicateVec;
	predicateVec.push_back(predicate);
	TRY(typeCheck.resolve(predicateVec), NULL);

	TRY(site->typify(""), NULL);
	bool cond;
	List<Site*> sites;
	sites.append(site);
	TypeID type = predicate->getTypeID();
	if(!(type == "bool")){
		string msg = "Predicate has type " +  type + " (bool expected)";
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}
	List<BaseSelection*>* siteISchema = site->getSelectList();
	assert(siteISchema);
	ExecExpr* execPred;

// 	old thing, not sure if the new works
//	TRY(execPred = predicate->createExec(*alias, siteISchema, "", NULL), NULL); 
	TRY(execPred = predicate->createExec(site, NULL), NULL); 
	assert(execPred);
	int inputNumFlds = site->getNumFlds();
	const TypeID* types = site->getTypeIDs();
	TRY(Iterator* iter = site->createExec(), NULL);
	iter->initialize();
	const Tuple* tmpTuple;
	List<Tuple*> tupleList;
	TupleLoader tupleLoader;
	TRY(tupleLoader.open(inputNumFlds, types), NULL);
	while((tmpTuple = iter->getNext())){
		cond = (execPred->evaluate(tmpTuple, NULL) ? true : false);
		if(!cond){
			Tuple* copy = tupleLoader.insert(tmpTuple);
			tupleList.append(copy);
		}
	}
	TRY(site->writeOpen(ios::out), NULL);
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		Tuple* tuple;
		tuple = tupleList.get();
		site->write(tuple);
	}
	site->writeClose();
	delete site;
	delete execPred;
	return NULL;
}
