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
  Revision 1.27  1998/03/12 18:23:22  donjerko
  *** empty log message ***

  Revision 1.26  1998/02/09 21:12:15  donjerko
  Added Bin by clause and implementation.

  Revision 1.25  1997/12/10 02:31:14  okan
  *** empty log message ***

  Revision 1.24  1997/12/04 04:05:13  donjerko
  *** empty log message ***

  Revision 1.23  1997/11/23 21:23:29  donjerko
  Added ODBC stuff.

  Revision 1.22  1997/11/12 23:17:29  donjerko
  Improved error checking.

  Revision 1.21  1997/11/08 21:02:26  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.20  1997/11/05 00:19:39  donjerko
  Separated typechecking from optimization.

  Revision 1.19  1997/10/15 02:22:58  arvind
  Sequence by handles multiple attributes

  Revision 1.18  1997/10/14 05:16:29  arvind
  Implemented a first version of moving aggregates (without group bys).

  Revision 1.17  1997/10/07 18:33:36  donjerko
  *** empty log message ***

  Revision 1.16  1997/09/05 22:20:08  donjerko
  Made changes for port to NT.

  Revision 1.15  1997/08/21 21:04:24  donjerko
  Implemented view materialization

  Revision 1.14  1997/08/14 02:08:52  donjerko
  Index catalog is now an independent file.

  Revision 1.13  1997/06/21 22:47:59  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.12  1997/06/16 16:04:43  donjerko
  New memory management in exec phase. Unidata included.

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
#include "TypeCheck.h"

#ifndef __GNUG__
using namespace std;
#endif

class Catalog;
class Site;
class Iterator;

const ISchema EMPTY_SCHEMA("0");
const ISchema SCHEMA_SCHEMA("1 " + SCHEMA_STR + " " + SCHEMA_STR);

class ParseTree {
public:
	virtual const ISchema* getISchema(){
		return &EMPTY_SCHEMA;
	}
	virtual Iterator* createExec() = 0; // throws exception
	virtual string guiRepresentation() const {assert(0); return NULL;}	// throws
};

class Aggregates;

class QueryTree : public ParseTree {
	List<BaseSelection*>* selectList;
	List<TableAlias*>* tableList;
	BaseSelection* predicates;
	List<BaseSelection*>* groupBy;
	BaseSelection* havingPredicate;
	List<BaseSelection*>* sequenceby;
	BaseSelection* withPredicate;
	List<BaseSelection*>* orderBy;
	string* sortOrdering;
	List<BaseSelection*>* grpAndSeqFields; // concat of grp and seq fields

	// these new variables will eventually replace the old List style ones

     vector<TableAlias*>& tableVec;
     vector<BaseSelection*>& selectVec;
     vector<BaseSelection*> predicateVec;
     vector<BaseSelection*>& groupByVec;
     vector<BaseSelection*>& sequenceByVec;
     vector<BaseSelection*>& orderByVec;
     vector<BaseSelection*>& binByVec;

	bool isSelectStar;

private:

	// internally created vars

	List<BaseSelection*>* predicateList;
	TypeCheck typeChecker;
	Aggregates* aggregates;
	ISchema* schema;

private:
	void setupSchema();
public:	
	QueryTree(
		vector<BaseSelection*>* selectList,
		vector<TableAlias*>* tableVec,
		BaseSelection* predicates,
		vector<BaseSelection *>*groupBy,
		vector<BaseSelection *>* binBy,
		BaseSelection* havingPredicate,
		vector<BaseSelection*>* sequenceby,
		BaseSelection* withPredicate,
		vector<BaseSelection*>* orderBy,
		string* sortOrdering, bool isSelectStar) :
		selectVec(*selectList), tableVec(*tableVec), 
		predicates(predicates), groupByVec(*groupBy), 
		binByVec(*binBy),
		havingPredicate(havingPredicate),
		sequenceByVec(*sequenceby), withPredicate(withPredicate), 
		orderByVec(*orderBy), sortOrdering(sortOrdering),
		grpAndSeqFields(NULL),
		predicateList(NULL),
		aggregates(NULL),
		schema(0), isSelectStar(isSelectStar) {}
	
	virtual Iterator* createExec(); // throws exception
	virtual ~QueryTree(){
		delete selectList;      // destroy list too
		delete tableList;
		delete grpAndSeqFields;
		delete schema;
	}
	virtual const ISchema* getISchema();
	virtual string guiRepresentation() const;	// throws
};

class IndexParse : public ParseTree {
	string* indexName;
	TableName* tableName;
	List<string*>* keyAttrs;
	List<string*>* additionalAttrs;
	bool standAlone;
public:	
	IndexParse(
		string* indexType,
		string* indexName,
		List<string*>* tableName,
		List<string*>* keyAttrs,
		List<string*>* additionalAttrs) :
		indexName(indexName), tableName(new TableName(tableName)),
		keyAttrs(keyAttrs), additionalAttrs(additionalAttrs) {
		standAlone = false;
		if(indexType){
			standAlone = true;
		}
	}
	virtual Iterator* createExec();	// throws exception
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
		List<string*>* tableName,
		List<ConstantSelection*>* fieldList) :
		tableName(new TableName(tableName)),
		fieldList(fieldList) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~InsertParse(){
		delete tableName;	// destroy too
		delete fieldList;	// destroy too
	}
};

class DeleteParse : public ParseTree {
	TableName* tableName;
	string* alias;
	BaseSelection* predicate;
public:	
	DeleteParse(
		List<string*>* tableName, string* alias,
		BaseSelection* predicate) :
		tableName(new TableName(tableName)),
		alias(alias), predicate(predicate) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~DeleteParse(){
		delete tableName;	// destroy too
		delete predicate;	// destroy too
	}
};

class DropIndexParse : public ParseTree {
	TableName* tableName;
	string* indexName;
public:
	DropIndexParse(List<string*>* tableName, string* indexName) :
		tableName(new TableName(tableName)), indexName(indexName) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~DropIndexParse(){
		delete tableName;	// destroy too
		delete indexName;
	}
};

class ISchemaParse : public ParseTree {
	TableName* tableName;
public:
	ISchemaParse(List<string*>* tableName) :
		tableName(new TableName(tableName)) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~ISchemaParse(){
		delete tableName;	// destroy too
	}
	virtual const ISchema* getISchema(){
		return &SCHEMA_SCHEMA;
	}
};

class UnionParse : public ParseTree {
	ParseTree* query1;
	ParseTree* query2;
public:
	UnionParse(ParseTree* query1, ParseTree* query2) :
		query1(query1), query2(query2) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~UnionParse(){
		delete query1;
		delete query2;
	}
};

class MaterializeParse : public ParseTree {
	TableName* tableName;
public:
	MaterializeParse(List<string*>* tableName) :
		tableName(new TableName(tableName)) {}
	virtual Iterator* createExec();	// throws exception
	virtual ~MaterializeParse(){
		delete tableName;	// destroy too
	}
};

typedef pair<string*, string*> IdentType;

class CreateTableParse : public ParseTree {
	vector<IdentType*>* identTypePairs;
public:
	CreateTableParse(vector<IdentType*>* identTypePairs);
	virtual Iterator* createExec();	// throws exception
	virtual ~CreateTableParse();
	virtual const ISchema* getISchema();
};

#endif
