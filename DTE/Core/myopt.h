/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.54  1999/01/20 22:46:33  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.52  1998/11/23 19:18:55  donjerko
  Added support for gestalts

  Revision 1.51  1998/10/08 22:27:12  donjerko
  *** empty log message ***

  Revision 1.50  1998/10/01 21:00:53  yunrui
  *** empty log message ***

  Revision 1.49  1998/07/24 04:37:59  donjerko
  *** empty log message ***

  Revision 1.48  1998/06/24 22:14:07  donjerko
  *** empty log message ***

  Revision 1.47  1998/04/09 20:26:24  donjerko
  *** empty log message ***

  Revision 1.46  1998/03/17 17:19:09  donjerko
  Added new namespace management through relation ids.

  Revision 1.45  1998/03/12 18:23:32  donjerko
  *** empty log message ***

  Revision 1.44  1998/02/09 21:12:23  donjerko
  Added Bin by clause and implementation.

  Revision 1.43  1997/12/22 17:54:10  donjerko
  Initial version of Saeed's sequence similarity search.

  Revision 1.42  1997/12/10 00:00:37  okan
  ODBC Interface Changes ...

  Revision 1.41  1997/12/04 04:05:20  donjerko
  *** empty log message ***

  Revision 1.40  1997/11/24 23:13:17  weaver
  Changes for the new ColorManager.

  Revision 1.39  1997/11/23 21:23:33  donjerko
  Added ODBC stuff.

  Revision 1.38  1997/11/12 23:17:40  donjerko
  Improved error checking.

  Revision 1.37  1997/11/05 00:19:47  donjerko
  Separated typechecking from optimization.

  Revision 1.36  1997/10/14 05:17:28  arvind
  Implemented a first version of moving aggregates (without group bys).

  Revision 1.35  1997/10/02 02:27:30  donjerko
  Implementing moving aggregates.

  Revision 1.34  1997/09/29 02:51:57  donjerko
  Eliminated class GlobalSelect.

  Revision 1.33  1997/09/17 02:35:48  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.32  1997/09/05 22:20:18  donjerko
  Made changes for port to NT.

  Revision 1.31  1997/08/25 15:28:14  donjerko
  Added minmax table

  Revision 1.30  1997/08/21 21:04:33  donjerko
  Implemented view materialization

  Revision 1.29  1997/08/14 02:08:55  donjerko
  Index catalog is now an independent file.

  Revision 1.28  1997/07/30 21:39:25  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.27  1997/07/22 15:00:56  donjerko
  *** empty log message ***

  Revision 1.26  1997/06/27 23:17:21  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.25  1997/06/16 16:04:52  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.24.6.1  1997/05/21 20:34:25  weaver
  Changes needed by new ColorManageR

  Revision 1.24  1997/04/18 20:46:19  donjerko
  Added function pointers to marshall types.

  Revision 1.23  1997/04/14 20:44:14  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.22  1997/04/10 21:50:29  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.21  1997/03/25 17:57:41  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.20  1997/03/23 23:45:23  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.19  1997/03/20 20:42:23  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.18  1997/03/14 18:36:13  donjerko
  Making space for the SQL UNION operator.

  Revision 1.17  1997/02/25 22:14:53  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.16  1997/02/21 01:38:07  donjerko
  Fixed some problems with "group by" clause.

  Revision 1.15  1997/02/18 18:06:05  donjerko
  Added skeleton files for sorting.

  Revision 1.14  1997/02/03 04:11:35  donjerko
  Catalog management moved to DTE

  Revision 1.13.4.1  1997/03/18 14:41:20  wenger
  Various minor changes to get 1.3 to compile on SGI.

  Revision 1.13  1996/12/26 03:42:01  kmurli
  MOdified to make joinprev work right

  Revision 1.12  1996/12/24 21:00:53  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.11  1996/12/21 22:21:50  donjerko
  Added hierarchical namespace.

  Revision 1.10  1996/12/18 00:55:58  donjerko
  Introduced the Operator* parent into the ExecOperator so that
  getTypeID work.

  Revision 1.9  1996/12/16 11:13:09  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.8  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

  Revision 1.7  1996/12/09 10:01:53  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.6  1996/12/07 15:14:28  donjerko
  Introduced new files to support indexes.

  Revision 1.5  1996/12/05 16:06:04  wenger
  Added standard Devise file headers.

 */

#ifndef MYOPT_H
#define MYOPT_H

#include <string>
#include <vector>
#include <assert.h>
//#include <iostream.h>   erased for sysdep.h
//#include <fstream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
//#include "queue.h"
#include "types.h"
#include "exception.h"
#include "TableUtils.h"
#include "url.h"
//#include "listop.h"
#include "sysdep.h"
#include "Interface.h"
#include "TableAlias.h"
#include "DTE/util/DteAlgo.h"
//#include "DTE/expr/ExecExpr.h"

#ifndef __GNUG__
using namespace std;
#endif


//===========================================================================



//===========================================================================

class ExecExpr;
class OptExpr;
class DteSymbolTable;
class ExecAgg;

typedef vector<OptExpr*> OptExprList;
typedef vector<OptExprList> OptExprListList;

class OptExpr
{
protected:

  DteAdt* adt;
  TableMap tableMap;            // set during typeCheck()
  int avgSize;	// to estimate result sizes

public:

  enum ExprType {
    CONST_ID,
    VARIABLE_ID,
    FIELD_ID,
    FUNCTION_ID,
  };

  OptExpr() {
    adt = NULL;
    avgSize = 10;
  }

  // adt1 is an allocated object that this OptExpr will delete
  OptExpr(DteAdt* adt1, int avgSize = 10) 
    : adt(adt1), avgSize(avgSize) {}

  virtual ~OptExpr() { delete adt; }

  string toString() const {
    ostringstream os;
    display(os);
    return os.str();
  }

  virtual void display(ostream& out, int detail = 0) const {
    if(detail) {
      out << "% size = " << avgSize;
      out << ", type = " << adt->getTypeSpec() << " %";
    }
  }

  //virtual bool exclusive(Site* site) = 0;

  //virtual bool exclusive(string* attributeNames, int numFlds) = 0;

  virtual string* siteName() {
    assert(0);
    return NULL; // avoid compiler warning
  }

  virtual OptExpr* cnf() {
    return NULL;
  }

  virtual bool distribute(OptExpr* bs) {
    return false;
  }

  virtual OptExpr* duplicate() = 0;

  virtual bool insertConj(OptExprList& predList) {
    predList.push_back(this);
    return false;
  }

  //virtual void collect(Site* s, OptExprList& to) = 0;

  virtual void collect(TableMap group, OptExprList& to) = 0;

  //virtual ExecExpr* createExec(Site* site1, Site* site2);	// obsolete

  // returns true if success, false if failure
  virtual bool typeCheck(const DteSymbolTable& symbols) = 0;

  // only valid after typeCheck() return true
  virtual ExecExpr* createExec(const OptExprListList& inputs) const;

  //virtual OptExprList getChildren() = 0;

  //virtual void setChildren(const OptExprList& children) = 0;

  virtual bool match(const OptExpr* x) const = 0;

  virtual OptExpr::ExprType getExprType() const = 0;

  // only valid after typeCheck()
  const DteAdt& getAdt() const {
    assert(adt);
    return *adt;
  }

  //  TypeID getTypeID() const {
  //    return adt->getTypeID();
  //  }

  void setSize(int size) {
    avgSize = size;
  }

  int getSize() {
    return avgSize;
  }

  virtual double getSelectivity() {
    assert(0);
    return 0.0; // avoid compiler warning
  }

  virtual bool isIndexable(string& attrName, string& opName, OptExpr*& value) {
    // Introduced to recognize the indexable predicates.
    // Only OptFunction can be indexable (and only when it's a compare op)
    return false;
  }

  virtual string toStringAttOnly() const {
    cout << "toStringAttOnly not definded for: " << (int)getExprType() << endl;
    assert(0);
    return ""; // avoid compiler warning
  }

  //virtual TableMap setTableMap(const vector<TableAlias*>&) = 0;

  // only valid after typeCheck() returns true
  TableMap getTableMap() const {
    return tableMap;
  }

  bool containedIn(TableMap group) {
    return group.contains(tableMap);
  }
};


inline ostream& operator<<(ostream& s, const OptExpr& x)
{
  x.display(s);
  return s;
}

//---------------------------------------------------------------------------


class OptConstant : public OptExpr
{
  Type* value;

public:

  OptConstant(DteAdt* adt, Type* val)
    : OptExpr(adt, adt->getMaxSize()), value(val) {}

  virtual ~OptConstant();

  const Type* getValue() { return value; }

  void setValue(Type* v) {
    adt->deallocate(value);
    value = v;
  }

  virtual bool typeCheck(const DteSymbolTable& symbols);

  //virtual OptExprList getChildren() { return OptExprList(0); }

  //virtual void setChildren(const OptExprList& children) {}

  virtual void display(ostream& out, int detail = 0) const;

  //virtual bool exclusive(Site* site) {
  //return true;
  //}

//   virtual bool exclusive(string* attributeNames, int numFlds) {
//     return true;
//   }

  virtual OptExpr* duplicate(); //kb: rename to clone()

  //virtual void collect(Site* s, OptExprList& to) {}

  virtual void collect(TableMap group, OptExprList& to);

  virtual OptExpr::ExprType getExprType() const;

  virtual bool match(const OptExpr* x) const;

  //virtual ExecExpr* createExec(Site* site1, Site* site2) {
  //return ExecExpr::createConstant(adt, adt->allocateCopy(value));
  //}

  ExecExpr* createExec(const OptExprListList& inputs) const;
  
  //virtual TableMap setTableMap(const vector<TableAlias*>&) {
  //TableMap tmp(0);
  //tableMap = tmp;
  //return tmp;
  //}
};


//---------------------------------------------------------------------------

class OptVariable : public OptExpr
{
  string varName;

public:

  OptVariable(const string& varName);

  virtual ~OptVariable();

  //kb: should pass MqlSession to typeCheck()
  virtual bool typeCheck(const DteSymbolTable& symbols);

  virtual void display(ostream& out, int detail = 0) const;

  virtual OptExpr* duplicate();

  virtual void collect(TableMap group, OptExprList& to);

  virtual OptExpr::ExprType getExprType() const;

  virtual bool match(const OptExpr* x) const;

  ExecExpr* createExec(const OptExprListList& inputs) const;
};

//---------------------------------------------------------------------------


class OptField : public OptExpr
{
  string alias;
  string fieldNm;
  int tableId;
  int slot;
  //const TableAlias* parentTable; // kb: delete this?

public:

//   OptField(const string& alias1, const string& fieldNm1,
//                DteAdt* adt = NULL,
//                int avgSize = 0,
//                const TableAlias* parentTable = NULL)
//     : OptExpr(adt, avgSize), alias(alias1), fieldNm(fieldNm1), 
//     parentTable(parentTable) {
//     }

  // adt & tableMap will be set by typeCheck()
  OptField(const string& alias1, const string& fieldNm1)
    : OptExpr(), alias(alias1), fieldNm(fieldNm1), tableId(-1), slot(-1) {}

  // typeCheck() will not be called
//   OptField(const string& alias1, const string& fieldNm1,
//            DteAdt* adt, int tabId)
//     : OptExpr(adt),  alias(alias1), fieldNm(fieldNm1) {
//     tableMap.set(tabId);
//   }

  ~OptField();

  //const TableAlias* getTable() {
  //return parentTable;
  //}

  const string& getFieldNm() {
    return fieldNm;
  }

  //virtual OptExprList getChildren() { return OptExprList(0); }
  //virtual void setChildren(const OptExprList& children) {}

  virtual void display(ostream& out, int detail = 0) const;

  //virtual bool exclusive(Site* site);

  //virtual bool exclusive(string* attributeNames, int numFlds);

  virtual OptExpr* duplicate();

  //virtual void collect(Site* s, OptExprList& to);

  virtual void collect(TableMap group, OptExprList& to);

  virtual ExecExpr* createExec(const OptExprListList& inputs) const;

  //virtual ExecExpr* createExec(Site* site1, Site* site2);

  virtual bool typeCheck(const DteSymbolTable& symbols);

  virtual OptExpr::ExprType getExprType() const;

  virtual bool match(const OptExpr* x) const;

  virtual string toStringAttOnly() const;

  //virtual TableMap setTableMap(const vector<TableAlias*>&);
};


//---------------------------------------------------------------------------


class OptFunction : public OptExpr
{
protected:

  string name;
  OptExprList args;
  bool aggFn;
  bool srqlAggFn;

public:

  OptFunction(const string& name1, const OptExprList& args1)
    : name(name1), args(args1), aggFn(false), srqlAggFn(false) {}

  OptFunction(const string& name1, OptExpr* arg1)
    : name(name1), args(1), aggFn(false), srqlAggFn(false) {
      args[0] = arg1;
    }

  OptFunction(const string& name1, OptExpr* arg1, OptExpr* arg2)
    : name(name1), args(2), aggFn(false), srqlAggFn(false) {
      args[0] = arg1;
      args[1] = arg2;
    }

  virtual ~OptFunction();

  const string& getName() { return name; }

  // these are only valid after typeCheck() returns true
  bool isAggFn() { return aggFn; }
  bool isSrqlAggFn() { return srqlAggFn; }

  //virtual OptExprList getChildren() { return args; }

  //virtual void setChildren(const OptExprList& children) {
    //kb: why does TypeCheck() possibly delete these?
    ////delete_all(args);
  //args = children;
  //}

  virtual void display(ostream& out, int detail = 0) const;

  OptExprList& getArgs() { return args; }

  virtual bool isIndexable(string& attrName, string& opName,
                           OptExpr*& value);

/*
  string getAttribute(){
    string attrNm;
    ostringstream os;
    if(left->getExprType() == FIELD_ID){
      attrNm = ((OptField*) left)->getFieldNm();
    }
    else if(right->getExprType() == FIELD_ID){
      attrNm = ((OptField*) right)->getFieldNm();
    }
    else{
      assert(!"selection expected");
    }
    return attrNm;
  }
  string getValue(){
    ostringstream os;
    if(left->getExprType() == FIELD_ID){
      right->display(os);
    }
    else if(right->getExprType() == FIELD_ID){
      left->display(os);
    }
    else{
      assert(!"selection expected");
    }
    return os.str();
  }
*/

  /*
  virtual bool exclusive(Site* site) {
    int n = args.size();
    for(int i ; i < n ; i++) {
      if( !args[i]->exclusive(site) ) {
        return false;
      }
    }
    return true;
  }
  */

//   virtual bool exclusive(string* attributeNames, int numFlds) {
//     int n = args.size();
//     for(int i ; i < n ; i++) {
//       if( !args[i]->exclusive(attributeNames, numFlds) ) {
//         return false;
//       }
//     }
//     return true;
//   }

  virtual OptExpr* duplicate();

  //virtual void collect(Site* s, OptExprList& to);

  virtual void collect(TableMap group, OptExprList& to);

  virtual OptExpr::ExprType getExprType() const;

  virtual bool match(const OptExpr* x) const;

  virtual bool typeCheck(const DteSymbolTable& symbols);

  //virtual ExecExpr* createExec(Site* site1, Site* site2);
  virtual ExecExpr* createExec(const OptExprListList& inputs) const;

  // This function is called to get the selectivity of a where clause
  // in the similarity query.
  virtual double getSelectivity(); //kb: use Operator:: version in resolve.c

  //virtual TableMap setTableMap(const vector<TableAlias*>& x);

private:

  OptFunction(const OptFunction& a);
  OptFunction& operator=(const OptFunction& a);
};


//---------------------------------------------------------------------------

// the following classes are special cases used to place expressions in cnf

class OptBool : public OptFunction
{
protected:

  OptBool(string s, OptExpr* l, OptExpr* r)
    : OptFunction(s, l, r) {}

  void propagateCnf();
};

class OptOr : public OptBool
{
public:

  OptOr(OptExpr* l, OptExpr* r) 
    : OptBool("or", l, r) {}

  virtual OptExpr* cnf();

  virtual OptExpr* duplicate();
};

class OptAnd : public OptBool
{
public:

  OptAnd(OptExpr* l, OptExpr* r)
    : OptBool("and", l, r){}

  virtual OptExpr* cnf();

  virtual bool distribute(OptExpr* bs);

  virtual OptExpr* duplicate();

  virtual bool insertConj(OptExprList& predList);
};


//===========================================================================
#endif
