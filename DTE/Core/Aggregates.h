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
#include "myopt.h"
#include "site.h"
#include "MemoryMgr.h"
//#include "SeqSimVecAggregate.h"
#include "TypeCheck.h"
#include "FixedLenQueue.h"

#ifndef __GNUG__
using namespace std;
#endif


//---------------------------------------------------------------------------

class ExecAggregate
{
public:
  ExecAggregate(const TypeID& type) : _type(type) {}
  virtual ~ExecAggregate() {}
  virtual void initialize(const Type* input) = 0;
  virtual void update(const Type* input) = 0;
  virtual bool isDifferent(const Type* newVal)
    {assert(!"Used only in grouping aggregates"); return false; }
  virtual const Type* getValue() = 0;
  virtual void dequeue(int n)
    {assert(!"Used only by moving aggregates");}
  const TypeID& getType() const { return _type; }
protected:
  TypeID _type;
};


//---------------------------------------------------------------------------


class AggFn
{
public:
  
  AggFn(const string& fn_name, const Field& field);
  
  AggFn(ExecAggregate* fn, int pos)
    : _fn(fn), _pos(pos) {}
  
  AggFn(const AggFn& x)
    : _fn(x._fn), _pos(x._pos) {}
  
  AggFn& operator=(const AggFn& x)
    { _fn = x._fn; _pos = x._pos; return *this; }
  
  void initialize(const Tuple* t)
    { _fn->initialize(t[_pos]); }
  
  void update(const Tuple* t)
    { _fn->update(t[_pos]); }
  
  bool isDifferent(const Tuple* t)
    { return _fn->isDifferent(t[_pos]); }
  
  const Type* getValue()
    { return _fn->getValue(); }
  
  void dequeue(int n)
    { _fn->dequeue(n); }

  const TypeID& getType() const
    { return _fn->getType(); }

protected:
  
  ExecAggregate* _fn;
  
  int _pos;
  
  friend class AggList;

  // needed for gcc 2.7
  AggFn() : _fn(0), _pos(0) {}
  friend class vector<AggFn>;
};


//---------------------------------------------------------------------------


class AggList
{
public:

  AggList() {}

  ~AggList() {
    int k = _aggs.size();
    for(int i = 0 ; i < k ; i++) {
      delete _aggs[i]._fn;
    }
  }

  void push_back(const AggFn& fn)
    { _aggs.push_back( fn ); _types.push_back(fn.getType()); }

  size_t size() const { return _aggs.size(); }

  AggFn* operator[](int i) { return &_aggs[i]; }

  const TypeIDList& getTypes() const { return _types; }

protected:

  vector<AggFn> _aggs;
  TypeIDList _types;

private:

  AggList(const AggList& x);
  AggList& operator=(const AggList& x);
};


//---------------------------------------------------------------------------
// Standard aggregate execs

class ExecMinMax : public ExecAggregate {
protected:
  OperatorPtr opPtr;
  ADTCopyPtr copyPtr;
  DestroyPtr destroyPtr;
  Type* minMax;
  size_t valueSize;

public:

  enum AggType { MIN_AGG, MAX_AGG };

  ExecMinMax(const TypeID& type, AggType min_max)
    : ExecAggregate(type) {
      GeneralPtr* genPtr = NULL;
      TypeID ret_type;
      const char* fn = (min_max == MIN_AGG ? "<" : ">");
      TRY(genPtr = getOperatorPtr(fn, type, type, ret_type),);
      assert(genPtr);
      opPtr = genPtr->opPtr;
      assert(ret_type == "bool");
      TRY(destroyPtr = getDestroyPtr(type),);
      TRY(copyPtr = getADTCopyPtr(type),);
      minMax = allocateSpace(type, valueSize);
    }

  void initialize(const Type* input) {
    copyPtr(input, minMax, valueSize);	// copy first value as current minMax
  }

  void update(const Type* input)  {
    Type* boolVal;
    opPtr(input, minMax, boolVal); // boolVal = input (opPtr) minMax
    if(boolVal)
      copyPtr(input, minMax, valueSize); // change value of current minMax
  }
	
  const Type* getValue() {
    return minMax;
  }

  virtual ~ExecMinMax(){
    destroyPtr(minMax);	
  }

};


//---------------------------------------------------------------------------


class ExecCount : public ExecAggregate {
protected:
  int count;
  
public:

  ExecCount() : ExecAggregate(INT_TP) { count = 0; }

  void initialize(const Type* input) { count = 1; }

  void update(const Type* input)  { count++; }
	
  const Type* getValue() {
    return (Type*)count;  // assumes int is inlined
  }

};


//---------------------------------------------------------------------------


class ExecSum : public ExecAggregate {
protected:
  OperatorPtr addPtr;
  ADTCopyPtr copyPtr;
  DestroyPtr sumDestroy;
  Type* sum;
  size_t valueSize;

public:
  ExecSum(const TypeID& type) : ExecAggregate(type) {
    GeneralPtr* genPtr = NULL;
    TypeID ret_type;
    TRY(genPtr = getOperatorPtr("+", type, type, ret_type),);
    assert(genPtr);
    addPtr = genPtr->opPtr;
    assert(ret_type == type);
    TRY(sumDestroy = getDestroyPtr(type),);
    TRY(copyPtr = getADTCopyPtr(type),);
    sum = allocateSpace(type, valueSize);
  }

  void initialize(const Type* input) {
    copyPtr(input, sum, valueSize);	
  }

  void update(const Type* input)  {
    addPtr(input, sum, sum);
  }
	
  const Type* getValue() {
    return sum;
  }

  virtual ~ExecSum(){
   sumDestroy(sum);
  }

};


//---------------------------------------------------------------------------


class ExecAverage : public ExecAggregate {
protected:
  OperatorPtr addPtr, divPtr;
  PromotePtr promotePtr;
  DestroyPtr sumDestroy;
  ADTCopyPtr copyPtr;
  Type* sum;
  Type *result;
  int count;
  size_t valueSize;

public:

  ExecAverage(const TypeID& type) : ExecAggregate(type) {
    GeneralPtr* genPtr = NULL;
    TypeID ret_type;

    TRY(genPtr = getOperatorPtr("+", type, type, ret_type),);
    assert(genPtr);
    addPtr = genPtr->opPtr;
    assert(ret_type == type);

    TRY(promotePtr = getPromotePtr(type, DOUBLE_TP),);
    assert(promotePtr);

    TRY(genPtr = getOperatorPtr("/", DOUBLE_TP, DOUBLE_TP, ret_type),);
    assert(genPtr);
    divPtr = genPtr->opPtr;
    assert(ret_type == DOUBLE_TP);

    TRY(sumDestroy = getDestroyPtr(type),);
    TRY(copyPtr = getADTCopyPtr(type),);

    sum = allocateSpace(type, valueSize);
    count = 0;
    result = allocateSpace("double");
  }

  void initialize(const Type* input) {
    copyPtr(input, sum, valueSize);	
    count = 1;
  }

  void update(const Type* input)  {
    addPtr(input, sum, sum);
    count++;
  }
	
  const Type* getValue() {
    if (count == 0) assert(!"average of no tuples?");
      //return (Type *) NULL; // What should be returned here?

    IDouble tmp(0);
    Type *sumDouble = &tmp;
    promotePtr(sum, sumDouble);
    IDouble count2(count);
    divPtr(sumDouble, &count2, result);
    return result;
  }

  virtual ~ExecAverage() {
    sumDestroy(sum);
    DestroyPtr dp = getDestroyPtr("double");
    assert(dp);
    dp(result);
  }

};


//---------------------------------------------------------------------------
// Moving aggregate Execs


class ExecMovMinMax : public ExecMinMax {
  TupleLoader* tupLoad;
  OperatorPtr eqPtr;
public:
  ExecMovMinMax(const TypeID& type, AggType min_max)
    : ExecMinMax(type, min_max) {

      GeneralPtr* genPtr = NULL;
      TypeID ret_type;
      TRY(genPtr = getOperatorPtr("=", type, type, ret_type),);
      assert(genPtr);
      eqPtr = genPtr->opPtr;
      assert(ret_type == "bool");

      int numFlds = 1;
      tupLoad = new TupleLoader;
      TRY(tupLoad->open(numFlds, &type),);
    }

  virtual void initialize(const Type* input){
    ExecMinMax::initialize(input);
    tupLoad->insert(&input);
  }

  virtual void update(const Type* input){
    if (tupLoad->empty()) {	// is this ever true? DD
      ExecMinMax::initialize(input);
    }
    else {
      ExecMinMax::update(input);
    }
    tupLoad->insert(&input);
  }

  const Type* getValue(){
    assert(!tupLoad->empty());
    return minMax;
  }

  virtual void dequeue(int n);
};



//---------------------------------------------------------------------------


class ExecMovCount : public ExecCount {
public:
  ExecMovCount() : ExecCount() {}

  virtual void dequeue(int n){ 
     count -= n;
  }
};


//---------------------------------------------------------------------------


class ExecMovSum : public ExecSum {
  TupleLoader* tupLoad;
  OperatorPtr subPtr;
  
public:

  ExecMovSum(const TypeID& type) : ExecSum(type) {
    GeneralPtr* genPtr = NULL;
    TypeID ret_type;
    TRY(genPtr = getOperatorPtr("-", type, type, ret_type),);
    assert(genPtr);
    subPtr = genPtr->opPtr;
    assert(ret_type == type);

    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &type),);
  }

  virtual void initialize(const Type* input){
    ExecSum::initialize(input);
    tupLoad->insert(&input);
  }

  virtual void update(const Type* input){
    if (tupLoad->empty()) {
      ExecSum::initialize(input);
    }
    else {
      ExecSum::update(input);
    }
    tupLoad->insert(&input);
  }

  const Type* getValue(){
    assert(!tupLoad->empty());
    return sum;
  }
  virtual void dequeue(int n);
};


//---------------------------------------------------------------------------


class ExecMovAverage : public ExecAverage {
  TupleLoader* tupLoad;
  OperatorPtr subPtr;
public:

  ExecMovAverage(const TypeID& type) : ExecAverage(type) {
    GeneralPtr* genPtr = NULL;
    TypeID ret_type;
    TRY(genPtr = getOperatorPtr("-", type, type, ret_type),);
    assert(genPtr);
    subPtr = genPtr->opPtr;
    assert(ret_type == type);
    
    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &type),);
  }
        
  virtual void initialize(const Type* input){
    ExecAverage::initialize(input);
    tupLoad->insert(&input);
  }

  virtual void update(const Type* input){
    if (tupLoad->empty()) {
      ExecAverage::initialize(input);
    }
    else {
      ExecAverage::update(input);
    }
    tupLoad->insert(&input);
  }

  const Type* getValue(){
    assert(!tupLoad->empty());
    return ExecAverage::getValue();
  }
  virtual void dequeue(int n);
};


//---------------------------------------------------------------------------


class ExecGroupAttr : public ExecAggregate {
protected:
  ADTCopyPtr copyPtr;
  OperatorPtr eqPtr;
  Type* prevGroup;
  size_t valueSize;

public:

  ExecGroupAttr(const TypeID& type) : ExecAggregate(type) {
    GeneralPtr* genPtr = NULL;
    TypeID ret_type;
    TRY(genPtr = getOperatorPtr("=", type, type, ret_type),);
    assert(genPtr);
    eqPtr = genPtr->opPtr;
    assert(ret_type == "bool");
    TRY(copyPtr = getADTCopyPtr(type),);
    prevGroup = allocateSpace(type, valueSize);
  }

  ~ExecGroupAttr() {
    DestroyPtr destroyPtr = getDestroyPtr(_type);
    destroyPtr(prevGroup);
  }

  virtual bool isDifferent(const Type* newVal){
    Type* cmp;
    eqPtr(prevGroup, newVal, cmp);
    return (int)cmp == 0;
  }

  virtual void initialize(const Type* input){
    copyPtr(input, prevGroup, valueSize);	    
  }

  virtual void update(const Type* input){
    assert(0);
  }

  virtual const Type* getValue(){
    return prevGroup;
  }
};


//---------------------------------------------------------------------------


class ExecSeqAttr : public ExecGroupAttr {
  TupleLoader* tupLoad;

public:

  ExecSeqAttr(const TypeID& type) : ExecGroupAttr(type) {
    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &type),);
  }

  virtual void initialize(const Type* input) {
    tupLoad->insert(&input);
    ExecGroupAttr::initialize(input);
  }

  virtual void update(const Type* input){
    tupLoad->insert(&input);
    ExecGroupAttr::initialize(input);
  }

  virtual const Type* getValue(){
    assert(!tupLoad->empty());
    const Tuple* front = tupLoad->front();
    return front[0];
  }

  virtual void dequeue(int n){
    for(int i = 0; i < n; i++){
      assert(!tupLoad->empty());
      tupLoad->pop_front();
    }
  }

};


//---------------------------------------------------------------------------
// aggregate type checking classes

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


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

 
class StandAggsExec : public Iterator {
public:
  
  StandAggsExec(Iterator* input, AggList* aggs);
  
  virtual ~StandAggsExec();
  
  virtual void initialize();
  
  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:
   
  Iterator* inputIter;
  AggList& aggExecs;
  int numFlds;
  TypeIDList types;
  Tuple* retTuple;
};


//---------------------------------------------------------------------------

// Assumes input is sorted on grouping attributes

class StandGroupByExec : public Iterator {
public:
  
  StandGroupByExec(Iterator* input, FieldList* group_fields, AggList* aggs);

  virtual ~StandGroupByExec();

  virtual void initialize();
  
  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  bool sameGroup(const Tuple* tup);

  Iterator* inputIter;
  AggList& aggExecs;
  AggList groupby;
  TypeIDList types;
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
              AggList* aggs, int winLow, int winHigh);

  virtual ~SeqAggsExec();

  virtual void initialize();
  
  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

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
  AggList& aggExecs;
  AggList groupby;
  AggList seqby;
  TypeIDList types;
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
	virtual int getNumFlds(){
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


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
