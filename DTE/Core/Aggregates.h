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
  Revision 1.51  1998/07/09 19:31:01  wenger
  Fixed bug 374 (Tables failing on SPARC/Solaris).

  Revision 1.50  1998/07/07 20:43:21  beyer
  added missing 'return' in AggFn

  Revision 1.49  1998/06/28 21:47:33  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.48  1998/03/12 18:23:18  donjerko
  *** empty log message ***

  Revision 1.47  1998/02/09 21:12:10  donjerko
  Added Bin by clause and implementation.

  Revision 1.46  1997/12/22 17:54:06  donjerko
  Initial version of Saeed's sequence similarity search.

  Revision 1.45  1997/12/15 16:15:18  wenger
  Backed out most recent changes to fix compile.

  Revision 1.43  1997/12/10 02:31:13  okan
  *** empty log message ***

  Revision 1.42  1997/12/04 04:05:07  donjerko
  *** empty log message ***

  Revision 1.41  1997/11/24 23:13:13  weaver
  Changes for the new ColorManager.

  Revision 1.18.2.1  1997/05/21 20:34:23  weaver
  Changes needed by new ColorManageR

  Revision 1.18  1997/05/07 21:25:13  wenger
  Conditionaled out some debug output.

  Revision 1.17  1997/04/18 23:40:31  donjerko
  Integers are being promoted to doubles when doing aggragates.

  Revision 1.16  1997/04/14 20:44:11  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.15  1997/04/10 21:50:22  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.14  1997/04/04 23:10:20  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.13  1997/03/28 16:07:22  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef AGGREGATES_H
#define AGGREGATES_H

#include "types.h"
//#include "myopt.h"
//#include "site.h"
//#include "TypeCheck.h"
#include "FixedLenQueue.h"
#include "STuple.h"

#ifndef __GNUG__
using namespace std;
#endif


//---------------------------------------------------------------------------

class ExecAgg
{
protected:

  DteAdt* adt;
  int pos;

  void setNullable() { adt->setNullable(); }

public:

  // adt will be deleted by this class
  ExecAgg(const DteAdt& adt, int pos) : adt(adt.clone()), pos(pos) {
    assert(pos >= 0);
  }

  virtual ~ExecAgg() { delete adt; }

  virtual void init() = 0;

  virtual void add(const Tuple* t) = 0;

  // returns true if a scan is needed, false otherwise
  virtual bool remove(const Tuple* t) = 0;

  virtual const Type* getValue() = 0;
  
  static DteAdt* typeCheck(const string& fn, const DteAdt& adt);

  static ExecAgg* create(const string& fn, const DteAdt& adt, int pos);

  const DteAdt& getAdt() const { return *adt; }

private:

  ExecAgg(const ExecAgg&);
  ExecAgg& operator=(const ExecAgg&);
};

typedef vector<ExecAgg*> ExecAggList;


//---------------------------------------------------------------------------
// aggregate type checking classes
#if 0
class Aggregate {
public:
  virtual TypeID typify(TypeID inputT) = 0;
  virtual ExecAggregate* createExec() = 0;
};

class MinAggregate : public Aggregate {
protected:
  TypeID typeID;

public:
  MinAggregate() {}
  ~MinAggregate() {}

  TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;
    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("<", inputT, inputT, boolT), UNKN_TYPE);
    assert(genPtr);
    assert(boolT == "bool");
    return typeID;
  }

  ExecAggregate* createExec() {
    return new ExecMinMax(typeID, ExecMinMax::MIN_AGG);
  }

};

class MaxAggregate : public Aggregate {
protected:
  TypeID typeID;

public:
  MaxAggregate() {}
  ~MaxAggregate() {}

  TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;
    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr(">", inputT, inputT, boolT), UNKN_TYPE);
    assert(genPtr);
    assert(boolT == "bool");
    return typeID;
  }

  ExecAggregate* createExec() {
    return new ExecMinMax(typeID, ExecMinMax::MAX_AGG);
  }

};

class CountAggregate : public Aggregate {
public:
  TypeID typify (TypeID inputT) {
    return INT_TP;
  }

  ExecAggregate* createExec() {
    return new ExecCount();
  }
};

class SumAggregate : public Aggregate {
protected:
  TypeID typeID;

public:
  SumAggregate() {}
  ~SumAggregate() {}

  TypeID typify(TypeID inputT){	// throws
    TypeID ret_type;
    typeID = inputT;

    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("+", inputT, inputT, ret_type), UNKN_TYPE);
    assert(genPtr);
    assert(ret_type == inputT);
    return typeID;
  }

  ExecAggregate* createExec(){
    return new ExecSum(typeID);
  }

};


//---------------------------------------------------------------------------


class AvgAggregate : public Aggregate {
protected:
  TypeID typeID;

public:
  AvgAggregate() {}
  ~AvgAggregate() {}

  TypeID typify(TypeID inputT){	// throws
    TypeID resultT;
    typeID = inputT;

    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("+", inputT, inputT, resultT), UNKN_TYPE);
    assert(genPtr);
    assert(resultT == inputT);

    genPtr = NULL;
    TRY(genPtr = getOperatorPtr("/", "double", "double", resultT), UNKN_TYPE);
    assert(genPtr);
    assert(resultT == "double");

    PromotePtr promotePtr;
    TRY(promotePtr = getPromotePtr(inputT, "double"), UNKN_TYPE);
    assert(promotePtr);
    return resultT;
  }

  ExecAggregate* createExec(){
    return new ExecAverage(typeID);
  }

};


//---------------------------------------------------------------------------
// moving aggregate type checking classes - derived from their "normal" classes

class MovMinAggregate : public MinAggregate {
public:
  ExecAggregate* createExec(){
    return new ExecMovMinMax(typeID, ExecMinMax::MIN_AGG);
  }
};


//---------------------------------------------------------------------------


class MovMaxAggregate : public MaxAggregate {
public:
  ExecAggregate* createExec(){
    return new ExecMovMinMax(typeID, ExecMinMax::MAX_AGG);
  }
}; 


//---------------------------------------------------------------------------


class MovCountAggregate : public CountAggregate {
public:
  ExecAggregate* createExec() {
    return new ExecMovCount();
  }
};


//---------------------------------------------------------------------------


class MovSumAggregate : public SumAggregate {
public:
  ExecAggregate* createExec(){
    return new ExecMovSum(typeID);
  }

};


//---------------------------------------------------------------------------


class MovAvgAggregate : public AvgAggregate {
public:
  ExecAggregate* createExec(){
    return new ExecMovAverage(typeID);
  }

};


//---------------------------------------------------------------------------


class GroupAttribute : public Aggregate {
protected:
  TypeID typeID;
  
public:
  GroupAttribute() {}
  ~GroupAttribute() {}

  virtual TypeID typify(TypeID inputT){	// throws
    typeID = inputT;

    TypeID ret_type;
    GeneralPtr* genPtr;
    TRY(genPtr = getOperatorPtr("=", inputT, inputT, ret_type), NULL); 
    assert(genPtr);
    assert(ret_type == "bool");
    
    return typeID;
  }

  virtual ExecAggregate* createExec(){
    return new ExecGroupAttr(typeID);
  }

};


//---------------------------------------------------------------------------


class SequenceAttribute : public GroupAttribute{
  virtual ExecAggregate* createExec(){
    return new ExecSeqAttr(typeID);
  }
}; 

#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "Iterator.h"
 
class StandAggsExec : public Iterator {
public:

  // input & aggs's elements now belong to this
  StandAggsExec(Iterator* input, const ExecAggList& aggs);
  
  virtual ~StandAggsExec();
  
  virtual void initialize();
  
  virtual const Tuple* getNext();

protected:
   
  Iterator* input;
  ExecAggList aggs;
  STuple retTuple;
};


//---------------------------------------------------------------------------

// Assumes input is sorted on grouping attributes

class StandGroupByExec : public Iterator {
public:
  
  StandGroupByExec(Iterator* input, FieldList* group_fields, ExecAggList* aggs);

  virtual ~StandGroupByExec();

  virtual void initialize();
  
  virtual const Tuple* getNext();

protected:

  bool sameGroup(const Tuple* tup);

  Iterator* inputIter;
  ExecAggList& aggExecs;
  ExecAggList groupby;
  Tuple* retTuple;
  int numAggs;
  int numGrpBys;
  const Tuple* currInTup;
};


//---------------------------------------------------------------------------


// Assumes input is sorted on grouping attributes, seq. attributes
//kb: code assumes that all aggregates use the same window!
//    and that window is passed to constructor...

class SeqAggsExec : public Iterator
{
public:
  
  SeqAggsExec(Iterator* input, FieldList* group_fields, FieldList* seq_fields,
              ExecAggList* aggs, int winLow, int winHigh);

  virtual ~SeqAggsExec();

  virtual void initialize();
  
  virtual const Tuple* getNext();

protected:

  bool sameGroup(const Tuple* tup);
  bool isNewSeqVal(const Tuple* tup);
  void setupFirst();
  void addData();
  void dropData();
  void getResult();
  bool currInTupIsValid() 
    { return (currInTup && sameGroup(currInTup)); }

  Iterator* inputIter; // Assumes is sorted on grouping attributes by optimizer
  ExecAggList& aggExecs;
  ExecAggList groupby;
  ExecAggList seqby;
  Tuple* retTuple;
  int numAggs;
  int numGrpBys;
  int numSeqBys;
  const Tuple* currInTup;

  int windowLow, windowHigh, fullWindowHeight; // window measurements
  FixedLenQueue<int> numTuplesToBeDropped;
  int currWindowHeight;
  bool endOfGroup; 
};



//---------------------------------------------------------------------------

#if 0
//kb: remove class
class Aggregates : public Site {
	vector<BaseSelection*>& selList;
	List<BaseSelection*>* filteredSelList;
	vector<BaseSelection*>& sequenceBy;
	bool isApplicableValue;
	bool alreadyChecked;
	bool sim_query;
	TypeID seqAttrType; // sequence by on only one attribute?
	int seqAttrPos;
	BaseSelection* withPredicate;	
	BaseSelection* havingPredicate;	
	Site* inputPlanOp;
	int withPredicatePos;	
	int havingPredicatePos;	
	vector<BaseSelection*>& groupBy;
	int* grpByPos; // positions of groupBy fields
	int* aggPos;		// positions of aggregate fields
	TypeID* typeIDs;
	Aggregate** aggFuncs;
	int numGrpByFlds;
	int numAggs;
	int numSeqByFlds;
	int* seqByPos;
	int windowLow, windowHigh; 
	int numFlds;

public:
	Aggregates(
		vector<BaseSelection*>& selectClause, // queries select clause
		vector<BaseSelection*>& sequenceBy,   
		BaseSelection* withPredicate,
		vector<BaseSelection*>& groupBy ,	
		BaseSelection* havingPredicate=NULL)
	  : Site(), selList(selectClause),sequenceBy(sequenceBy),
	    withPredicate(withPredicate),groupBy(groupBy),
	    havingPredicate(havingPredicate) {
		
		numFlds = selList.size();
		if(numFlds > 0){
			aggFuncs = new Aggregate*[numFlds];
			typeIDs = new TypeID[numFlds];
			attributeNames = new TypeID[numFlds];
			vector<BaseSelection*>::const_iterator it;
			int i = 0;
			for(it = selList.begin(); it != selList.end(); ++it, ++i){
			  aggFuncs[i] = NULL;
			  typeIDs[i] = UNKN_TYPE;
			  attributeNames[i] = (*it)->toString();
			}
		}
		else {
			aggFuncs = NULL;
			typeIDs = NULL;
			attributeNames = NULL;
		}
		isApplicableValue = false;
		alreadyChecked = false;
		sim_query = false;
		inputPlanOp = NULL;
		filteredSelList = NULL;
		grpByPos = NULL;
		aggPos = NULL;
		seqByPos = NULL;
		numGrpByFlds = numSeqByFlds = numAggs = 0;
		windowLow = windowHigh = 0;
	}
	
	virtual ~Aggregates(){
		// do not delete selList;
		// couses problems
		// delete sequenceBy;
		// delete inputPlanOp;
		// delete filteredSelList;		// do not destroy
	}
	virtual int getNumFields() {
		return numFlds;
	}
	bool isApplicable();
	void Aggregates::typeCheck(TypeCheck& typeCheck);
	virtual List<BaseSelection*>* getSelectList(){
		assert(0);
		return NULL;
	    //return selList;
	}
	List<BaseSelection*>* filterList(){
		return filteredSelList;
	}
	virtual void typify(const string& name, Site* inputPlanOp);
	
	virtual const TypeID* getTypeIDs(){
		return typeIDs;
	}
	virtual string *getOrderingAttrib(){
		return iterat->getOrderingAttrib();
	}
	Iterator* createExec();
};
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
