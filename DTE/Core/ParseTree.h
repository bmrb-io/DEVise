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
  Revision 1.32  1998/10/01 20:59:51  yunrui
  add class CreateGestaltParse class and RegisterIntoGestaltParse

  Revision 1.31  1998/07/06 21:07:03  wenger
  More memory leak hunting -- partly tracked down some in the DTE.

  Revision 1.30  1998/06/28 21:47:40  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.29  1998/06/24 05:02:58  okan
  Added ODBC List & DSN List commands
  Added insert & delete commands to update ODBC entries in related catalogs

  Revision 1.28  1998/03/17 17:18:58  donjerko
  Added new namespace management through relation ids.

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

//#include "queue.h"
#include "myopt.h"
#include "TypeCheck.h"

#ifndef __GNUG__
using namespace std;
#endif

class Catalog;
class Site;
class Iterator;

class ParseTree {
public:
	virtual ~ParseTree() {}
	virtual const ISchema* getISchema(){
		return &EMPTY_SCHEMA;
	}
	virtual Iterator* createExec() = 0; // throws exception
	virtual string guiRepresentation() const {assert(0); return "";} // throws
protected:
  static bool typeCheckVec(OptExprList&, const DteSymbolTable& symtab);

};

class Aggregates;

class QueryTree : public ParseTree {
  OptExprList selectVec;
  vector<TableAlias*> tableVec;
  OptExprList predicateVec;
  OptExprList groupByVec;
  OptExprList sequenceByVec;
  OptExpr* withPredicate;
  OptExpr* havingPredicate;
  OptExprList orderByVec;
  string sortOrdering;   //kb: order should be per attribute

  // internally created vars
  //Aggregates* aggregates;
  ISchema* schema;

private:
  void addSelectStar();
  void setupSchema();
  bool typeCheck();
public:	
  QueryTree(OptExprList* selectList, // SELECT
            vector<TableAlias*>* tableVec, // FROM
            OptExpr* predicates,     // WHERE
            OptExprList* groupBy,
            OptExprList* sequenceby,
            OptExpr* withPredicate,
            OptExpr* havingPredicate,
            OptExprList* orderBy,
            string* sortOrdering);

  virtual ~QueryTree();
  virtual Iterator* createExec(); // throws exception
  virtual const ISchema* getISchema();
  virtual string guiRepresentation() const;	// throws
};

class IndexParse : public ParseTree {
	string indexName;
	TableName tableName;
	vector<string> keyAttrs;
	vector<string> additionalAttrs;
	bool standAlone;
public:	
	IndexParse(
		string* indexType,
		string* indexName,
		TableName* tableName,
		vector<string>* keyAttrs,
                bool includePointer,
		vector<string>* additionalAttrs)
          : indexName(*indexName), tableName(*tableName),
            keyAttrs(*keyAttrs), additionalAttrs(*additionalAttrs) {
          standAlone = !includePointer;
          assert(*indexType == "rtree");
          delete indexType;
          delete indexName;
          delete keyAttrs;
          delete additionalAttrs;
	}
	virtual Iterator* createExec();	// throws exception
	virtual ~IndexParse() {}
};

class InsertParse : public ParseTree {
	TableName tableName;
	OptExprList fieldList;
public:	
	InsertParse(TableName* tableName, OptExprList* fieldList)
          : tableName(*tableName), fieldList(*fieldList) {
          delete tableName;
          delete fieldList;
        }
	virtual Iterator* createExec();	// throws exception
	virtual ~InsertParse(){
          delete_all(fieldList);
	}
};

class DeleteParse : public ParseTree {
	TableAlias* tableAlias;
	OptExpr* predicate;
public:	
	DeleteParse(TableAlias* tableAlias, OptExpr* predicate)
          : tableAlias(tableAlias), predicate(predicate) {
        }
	virtual Iterator* createExec();	// throws exception
	virtual ~DeleteParse() {
          delete tableAlias;
          delete predicate;
	}
};

class DropIndexParse : public ParseTree {
	TableName tableName;
	string indexName;
public:
	DropIndexParse(TableName* tableName, string* indexName)
          : tableName(*tableName), indexName(*indexName) {
          delete tableName;
          delete indexName;
        }
	virtual Iterator* createExec();	// throws exception
	virtual ~DropIndexParse() {}
};

class ISchemaParse : public ParseTree {
	TableName tableName;
public:
	ISchemaParse(TableName* tableName)
          : tableName(*tableName) {
          delete tableName;
        }
	virtual Iterator* createExec();	// throws exception
	virtual ~ISchemaParse() {}
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
	TableName tableName;
public:
	MaterializeParse(TableName* tableName) :
		tableName(*tableName) { delete tableName; }
	virtual Iterator* createExec();	// throws exception
	virtual ~MaterializeParse() { }
};

typedef pair<string, string> IdentType;

class CreateTableParse : public ParseTree {
	vector<IdentType> identTypePairs;
public:
	CreateTableParse(vector<IdentType>* identTypePairs);
	virtual Iterator* createExec();	// throws exception
	virtual ~CreateTableParse();
	virtual const ISchema* getISchema();
};

class CreateGestaltParse : public ParseTree {
  	vector<IdentType> identTypePairs;
	TableName gestaltName;
	string fileName;
public:
	CreateGestaltParse(TableName* gestaltName,
          vector<IdentType>* identTypePairs, string* fileName);
 	virtual Iterator* createExec();	// throws exception
	virtual ~CreateGestaltParse();
}; 

class RegisterIntoGestaltParse : public ParseTree {
        TableName gestaltName;
	TableName tableName;
public:
	RegisterIntoGestaltParse(TableName* gestaltName, TableName* tableName);
 	virtual Iterator* createExec();	// throws exception
	virtual ~RegisterIntoGestaltParse() {}
};

class UnregisterFromGestaltParse : public ParseTree {
	TableName gestaltName;
	TableName tableName;
public:
	UnregisterFromGestaltParse(TableName* gestaltName, TableName* tableName);
 	virtual Iterator* createExec();	// throws exception
	virtual ~UnregisterFromGestaltParse() {}
};

class ODBCTableAddParse : public ParseTree {
	string DTE_Table_Name;
	string DSN_Name;
	string ODBC_Table_Name;
public:
	ODBCTableAddParse(string* DTN, string* DN, string* OTN)
          : DTE_Table_Name(*DTN), DSN_Name(*DN), ODBC_Table_Name(*OTN) {
          delete DTN;
          delete DN;
          delete OTN;
        }
	virtual Iterator* createExec();
        virtual ~ODBCTableAddParse() {}

};

class ODBCDSNAddParse : public ParseTree {
	string DSN_Name;
	string DSN_Info;
public:
	ODBCDSNAddParse(string* DN, string* DI)
          : DSN_Name(*DN), DSN_Info(*DI) {
          delete DN;
          delete DI;
        }
	virtual Iterator* createExec();
	virtual ~ODBCDSNAddParse() {}
};

class ODBCTableDeleteParse : public ParseTree {
	string DTE_Table_Name;
public:
	ODBCTableDeleteParse(string* DTN) : DTE_Table_Name(*DTN) {
          delete DTN;
        }
	virtual Iterator* createExec();
	virtual ~ODBCTableDeleteParse() {}
};

class ODBCDSNDeleteParse : public ParseTree {
	string DSN_Name;
public:
	ODBCDSNDeleteParse(string* DN) : DSN_Name(*DN) { delete DN; }
	virtual Iterator* createExec();
	virtual ~ODBCDSNDeleteParse() {}
};

class DSNEntriesParse : public ParseTree {
	Iterator* myIterator;
public:
	DSNEntriesParse();
	virtual Iterator* createExec();
	virtual const ISchema* getISchema();
	virtual ~DSNEntriesParse();
};

class ODBCTablesParse : public ParseTree {
	Iterator* myIterator;
	string DSN_Name;
public:
	ODBCTablesParse(string* DN);
	virtual Iterator* createExec();
	virtual const ISchema* getISchema();
	virtual ~ODBCTablesParse();
};

#endif
