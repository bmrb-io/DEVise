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
  Revision 1.11  1997/04/08 01:47:32  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.10  1997/03/28 16:07:25  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "catalog.h"

Catalog* getRootCatalog();
// String getIndexCatName();

class ParseTree {
public:
	virtual Site* createSite() = 0;	// throws exception
};

class QueryTree : public ParseTree {
	List<BaseSelection*>* selectList;
	List<TableAlias*>* tableList;
	BaseSelection* predicates;
	String* sequencebyTable;
	BaseSelection* withPredicate;
	List<BaseSelection*>* groupBy;
	List<BaseSelection*>* orderBy;
	List<String*>* namesToResolve;
	void resolveNames();	// throws exception
public:	
	QueryTree(
		List<BaseSelection*>* selectList,
		List<TableAlias*>* tableList,
		BaseSelection* predicates,
		String *sequencebyTable,
		BaseSelection* withPredicate,
		List<BaseSelection *>*groupBy,
		List<BaseSelection*>* orderBy,
		List<String*>* namesToResolve) :
		selectList(selectList), tableList(tableList), 
		predicates(predicates), sequencebyTable(sequencebyTable),
		withPredicate(withPredicate),groupBy(groupBy), orderBy(orderBy),
		namesToResolve(namesToResolve) {}
	
	virtual Site* createSite();	// throws exception
	virtual ~QueryTree(){
		delete selectList;      // destroy list too
		delete tableList;

		// predicates should be deleted in createSite
	}
	
};

class IndexParse : public ParseTree {
	String* indexName;
	TableName* tableName;
	List<String*>* keyAttrs;
	List<String*>* additionalAttrs;
	bool standAlone;
public:	
	IndexParse(
		String* indexType,
		String* indexName,
		List<String*>* tableName,
		List<String*>* keyAttrs,
		List<String*>* additionalAttrs) :
		indexName(indexName), tableName(new TableName(tableName)),
		keyAttrs(keyAttrs), additionalAttrs(additionalAttrs) {
		standAlone = false;
		if(indexType){
			standAlone = true;
		}
	}
	virtual Site* createSite();	// throws exception
	virtual ~IndexParse(){
		delete indexName;
		delete tableName;	// destroy too
		delete keyAttrs;	// destroy too
		delete additionalAttrs;	// destroy too
	}
};

class InsertParse : public ParseTree {
	TableName* tableName;
	List<ConstantSelection*>* fieldList;
public:	
	InsertParse(
		List<String*>* tableName,
		List<ConstantSelection*>* fieldList) :
		tableName(new TableName(tableName)),
		fieldList(fieldList) {}
	virtual Site* createSite();	// throws exception
	virtual ~InsertParse(){
		delete tableName;	// destroy too
		delete fieldList;	// destroy too
	}
};

class DeleteParse : public ParseTree {
	TableName* tableName;
	String* alias;
	BaseSelection* predicate;
public:	
	DeleteParse(
		List<String*>* tableName, String* alias,
		BaseSelection* predicate) :
		tableName(new TableName(tableName)),
		alias(alias), predicate(predicate) {}
	virtual Site* createSite();	// throws exception
	virtual ~DeleteParse(){
		delete tableName;	// destroy too
		delete predicate;	// destroy too
	}
};

class DropIndexParse : public ParseTree {
	TableName* tableName;
	String* indexName;
public:
	DropIndexParse(List<String*>* tableName, String* indexName) :
		tableName(new TableName(tableName)), indexName(indexName) {}
	virtual Site* createSite();	// throws exception
	virtual ~DropIndexParse(){
		delete tableName;	// destroy too
		delete indexName;
	}
};

class ISchemaParse : public ParseTree {
	TableName* tableName;
public:
	ISchemaParse(List<String*>* tableName) :
		tableName(new TableName(tableName)) {}
	virtual Site* createSite();	// throws exception
	virtual ~ISchemaParse(){
		delete tableName;	// destroy too
	}
};

class UnionParse : public ParseTree {
	ParseTree* query1;
	ParseTree* query2;
public:
	UnionParse(ParseTree* query1, ParseTree* query2) :
		query1(query1), query2(query2) {}
	virtual Site* createSite();	// throws exception
	virtual ~UnionParse(){
		delete query1;
		delete query2;
	}
};

#endif
