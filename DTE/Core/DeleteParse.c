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

#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>	// for perror
#include<errno.h>

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "listop.h"
#include "ParseTree.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

Site* DeleteParse::createSite(){
	LOG(logFile << "Deleting from ");
	LOG(tableName->display(logFile));
	LOG(logFile << " tuples satisfying ";)
	LOG(predicate->display(logFile));
	LOG(logFile << endl;)

	Catalog* catalog = getRootCatalog();
	assert(catalog);
	TRY(Site* site = catalog->find(tableName), NULL);
	delete catalog;
	assert(site);
	TableAlias ta(tableName, alias);
	site->addTable(&ta);

	TRY(site->typify(""), NULL);
	List<Tuple*> tupleList;
	Tuple* tuple;
	bool cond;
	List<Site*> sites;
	sites.append(site);
	TRY(TypeID type = predicate->typify(&sites), NULL);
	if(type != "bool"){
		String msg = "Predicate has type " +  type + " (bool expected)";
		THROW(new Exception(msg), NULL);
	}
	List<BaseSelection*>* siteSchema = site->getSelectList();
	assert(siteSchema);
	TRY(predicate = predicate->enumerate(*alias, siteSchema, "", NULL), NULL); 
	assert(predicate);
	predicate->display(cout);
	TRY(site->enumerate(), NULL);
	site->initialize();
	while((tuple = site->getNext())){
		cond = predicate->evaluate(tuple, NULL);
		if(!cond){
			tupleList.append(tuple);
		}
	}
	TRY(site->writeOpen(ios::out), NULL);
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		tuple = tupleList.get();
		site->write(tuple);
	}
	site->writeClose();
	delete site;
	return new Site();
}
