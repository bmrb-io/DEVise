#ifndef AGGREGATES_H
#define AGGREGATES_H

#include "types.h"
#include "myopt.h"
#include "site.h"

class ExecAggregate {
public:
  virtual void initialize(const Type* input) = 0;
  virtual void update(const Type* input) = 0;
  virtual Type* getValue() = 0;
};

class ExecGroupAttr : public ExecAggregate {
	ADTCopyPtr copyPtr;
	OperatorPtr eqPtr;
	Type* prevGroup;
	size_t valueSize;

public:
	ExecGroupAttr(ADTCopyPtr copyPtr, OperatorPtr eqPtr, Type* value, 
		size_t valueSize) :
		copyPtr(copyPtr), eqPtr(eqPtr), prevGroup(value), 
		valueSize(valueSize) {}

	bool isDifferent(const Type* newVal){
		Type* cmp;
		eqPtr(prevGroup, newVal, cmp);
		return !((bool) cmp);
	}

	virtual void initialize(const Type* input){
		copyPtr(input, prevGroup, valueSize);	    
	}

	virtual void update(const Type* input){
		assert(0);
	}

	virtual Type* getValue(){
		return prevGroup;
	}
};

class ExecMinMax : public ExecAggregate {
  OperatorPtr opPtr;
  ADTCopyPtr copyPtr;
  Type* minMax;
  size_t valueSize;

public:
  ExecMinMax(OperatorPtr opPtr, ADTCopyPtr copyPtr, Type* value, 
	     size_t valueSize) :
    opPtr(opPtr), copyPtr(copyPtr), minMax(value), valueSize(valueSize) {}

  void initialize(const Type* input) {
    copyPtr(input, minMax, valueSize);	// copy first value as current minMax
  }

  void update(const Type* input)  {
    Type* boolVal;
    opPtr(input, minMax, boolVal); // boolVal = input (opPtr) minMax
    if(boolVal)
      copyPtr(input, minMax, valueSize); // change value of current minMax
  }
	
  Type* getValue() {
    return minMax;
  }

  virtual ~ExecMinMax(){
//    delete minMax;	// need destroyPtr here
  }

};


class ExecCount : public ExecAggregate {
  int count;
  Type* result;
  DestroyPtr intDest;
  
public:
  ExecCount() {
  	intDest = getDestroyPtr(INT_TP);
	assert(intDest);
	count = 0;
  }

  void initialize(const Type* input) {
    count = 1;	
  }

  void update(const Type* input)  {
    count++;
  }
	
  Type* getValue() {
    result = (Type*) count;  // assumes int is inlined
    return result;
  }

  virtual ~ExecCount() {intDest(result);}

};


class ExecSum : public ExecAggregate {
  OperatorPtr addPtr;
  ADTCopyPtr copyPtr;
  Type* sum;
  size_t valueSize;

public:
  ExecSum(OperatorPtr addPtr, ADTCopyPtr copyPtr, Type* value, 
	     size_t valueSize) :
    addPtr(addPtr), copyPtr(copyPtr), sum(value), valueSize(valueSize) {}

  void initialize(const Type* input) {
    copyPtr(input, sum, valueSize);	
  }

  void update(const Type* input)  {
    addPtr(input, sum, sum);
  }
	
  Type* getValue() {
    return sum;
  }

  virtual ~ExecSum(){
    delete sum;
  }

};


class ExecAverage : public ExecAggregate {
  OperatorPtr addPtr, divPtr;
  PromotePtr promotePtr;
  DestroyPtr sumDestroy;
  ADTCopyPtr copyPtr;
  Type* sum;
  Type *result;
  int count;
  size_t valueSize;

public:
  ExecAverage(OperatorPtr addPtr,OperatorPtr divPtr, PromotePtr promotePtr,
	      DestroyPtr sumDestroy, ADTCopyPtr copyPtr,
		 Type* value, size_t valueSize) : 
    addPtr(addPtr), divPtr(divPtr), promotePtr(promotePtr),
    sumDestroy(sumDestroy), copyPtr(copyPtr),
    sum(value), valueSize(valueSize) {
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
	
  Type* getValue() {
    if (count == 0)
      return (Type *) NULL; // What should be returned here?

    Type *sumDouble = &IDouble(0.0);
    promotePtr(sum, sumDouble);
    divPtr(sumDouble, &IDouble(count), result);
    return result;
  }

  virtual ~ExecAverage() {
    sumDestroy(sum);
    DestroyPtr dp = getDestroyPtr("double");
    assert(dp);
    dp(result);
  }

};


class Aggregate {
public:
  virtual TypeID typify(TypeID inputT) = 0;
  virtual ExecAggregate* createExec() = 0;
};

class MinAggregate : public Aggregate {
protected:
  OperatorPtr opPtr;
  TypeID typeID;

public:
  MinAggregate() : opPtr(NULL) {}

  TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;
    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("<", inputT, inputT, boolT), UNKN_TYPE);
    assert(genPtr);
    opPtr = genPtr->opPtr;
    assert(boolT == "bool");
    return typeID;
  }

  ExecMinMax* createExec(){
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    return new ExecMinMax(opPtr, copyPtr, value, valueSize);
  }

};

class MaxAggregate : public MinAggregate {
public:

  TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;
    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr(">", inputT, inputT, boolT), UNKN_TYPE);
    assert(genPtr);
    opPtr = genPtr->opPtr;
    assert(boolT == "bool");
    return inputT;
  }

};

class CountAggregate : public Aggregate {
public:
  TypeID typify (TypeID inputT) {
    return "int";
  }

  ExecAggregate* createExec() {
    ExecAggregate* retVal = new ExecCount();
    return retVal;
  }
};

class SumAggregate : public Aggregate {
private:
  OperatorPtr opPtr;
  TypeID typeID;

public:
  SumAggregate() : opPtr(NULL) {}

  TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;

    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("+", inputT, inputT, boolT), UNKN_TYPE);
    assert(genPtr);
    opPtr = genPtr->opPtr;
    assert(boolT == inputT);
    return typeID;
  }

  ExecAggregate* createExec(){
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    ExecAggregate* retVal = new ExecSum(opPtr, copyPtr, value, valueSize);
    return retVal;
  }

};

class AvgAggregate : public Aggregate {
private:
  OperatorPtr addPtr, divPtr;
  PromotePtr promotePtr;
  TypeID typeID;

public:
  AvgAggregate() : addPtr(NULL), divPtr(NULL) {}

  TypeID typify(TypeID inputT){	// throws
    TypeID resultT;
    typeID = inputT;

    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("+", inputT, inputT, resultT), UNKN_TYPE);
    assert(genPtr);
    addPtr = genPtr->opPtr;
    assert(resultT == inputT);

    genPtr = NULL;
    TRY(genPtr = getOperatorPtr("/", "double", "double", resultT), UNKN_TYPE);
    assert(genPtr);
    divPtr = genPtr->opPtr;
    assert(resultT == "double");

    promotePtr = NULL;
    TRY(promotePtr = getPromotePtr(inputT, "double"), UNKN_TYPE);
    assert(promotePtr);
    return resultT;
  }

  ExecAggregate* createExec(){
    DestroyPtr sumDestroy;
    ADTCopyPtr copyPtr;
    TRY(sumDestroy = getDestroyPtr(typeID), NULL);
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    ExecAggregate* retVal = new ExecAverage(addPtr, divPtr, promotePtr, 
					    sumDestroy, copyPtr, value, valueSize);
    return retVal;
  }

};

class GroupAttribute : public Aggregate {
protected:
  TypeID typeID;
  OperatorPtr eqPtr;
  
public:
  GroupAttribute() {}

  virtual TypeID typify(TypeID inputT){	// throws
    TypeID boolT;
    typeID = inputT;

    TypeID retVal;  // is a dummy	       
    GeneralPtr* genPtr;
    TRY(genPtr = getOperatorPtr("=", inputT, inputT, retVal), NULL); 
    assert(genPtr);
    eqPtr = genPtr->opPtr;
    
    return typeID;
  }

  virtual ExecGroupAttr* createExec(){
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);
    
    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    return new ExecGroupAttr(copyPtr, eqPtr, value, valueSize);
  }

};

class StandAggsExec : public Iterator {
	Iterator* inputIter;
	ExecAggregate** aggExecs;
	int numFlds;
	Tuple* retTuple;
public:
	StandAggsExec(Iterator* inputIter, ExecAggregate** aggExecs, int numFlds) :
	  inputIter(inputIter), aggExecs(aggExecs), numFlds(numFlds){
		  retTuple = new Tuple[numFlds];
	}

	virtual ~StandAggsExec(){
		delete [] retTuple;
	}

	virtual void initialize();
	virtual const Tuple* getNext();

	// Need to check this..
	virtual void reset(int lowRid, int highRid){
		TRY(inputIter->reset(lowRid, highRid), );
	}
};

class StandGroupByExec : public Iterator {
  Iterator* inputIter; // Assumes is sorted on grouping attributes by optimizer
  ExecAggregate** aggExecs;
  int numFlds;
  Tuple* retTuple;
  int* grpByPos;		// positions of group by attributes
  int grpByPosLen;
  int* aggPos;
  int numAggs;
  const Tuple* currInTup;
  
public:
  
	StandGroupByExec(Iterator* inputIter, ExecAggregate** aggExecs,
		int numFlds, int* grpByPos, 
		int grpByPosLen,
		int* aggPos, int numAggs) :

		inputIter(inputIter), aggExecs(aggExecs),
		numFlds(numFlds),  grpByPos(grpByPos), grpByPosLen(grpByPosLen),
		aggPos(aggPos), numAggs(numAggs) {

		retTuple = new Tuple[numFlds];
		currInTup = NULL;
		assert(numAggs + grpByPosLen == numFlds);
	}

	virtual ~StandGroupByExec(){
		for(int i = 0; i < numFlds; i++){
			delete aggExecs[i];
		}
		delete [] aggExecs;
		delete [] grpByPos;
		delete [] aggPos;
		delete [] retTuple;
		delete inputIter;
	}

	virtual void initialize();

	virtual const Tuple* getNext();
  
  // Need to check this..
  virtual void reset(int lowRid, int highRid){
    TRY(inputIter->reset(lowRid, highRid), );
  }

private:
	bool isNewGroup(const Tuple* tup){
		for (int i = 0; i < grpByPosLen; i++){
			ExecGroupAttr* curr = (ExecGroupAttr*) aggExecs[grpByPos[i]];
			if(curr->isDifferent(tup[grpByPos[i]])){
			 	return true;
			}
		}
		return false;
	}
};

class Aggregates : public Site {
	List<BaseSelection*>* selList;
	List<BaseSelection*>* filteredSelList;
	BaseSelection * sequenceAttr;
	bool isApplicableValue;
	bool alreadyChecked;
	TypeID seqAttrType;
	int seqAttrPos;
	BaseSelection* withPredicate;	
	Site* inputPlanOp;
	int withPredicatePos;	
	List<BaseSelection*>* groupBy;
	int* positions; // positions of groupBy fields
	int* aggPos;		// positions of aggregate fields
	TypeID* typeIDs;
	Aggregate** aggFuncs;
	int numGrpByFlds;
	int numAggs;
public:
	Aggregates(
		List<BaseSelection*>* selectClause,	// queries select clause
		BaseSelection * sequenceby,	// queries select clause
		BaseSelection* withPredicate,
		List<BaseSelection*>* groupBy ,	// group by clause
		BaseSelection* having = NULL			// having clause
	) : Site(), selList(selectClause),sequenceAttr(sequenceby),
			withPredicate(withPredicate),groupBy(groupBy){
		
		Site::mySelect = selList;
		if(selList){
			numFlds = selList->cardinality();
			aggFuncs = new (Aggregate*)[numFlds];
			typeIDs = new TypeID[numFlds];
			for(int i = 0; i < numFlds; i++){
			  aggFuncs[i] = NULL;
			  typeIDs[i] = UNKN_TYPE;
			}
		}
		else {
			numFlds = 0;
			aggFuncs = NULL;
		}
		isApplicableValue = false;
		alreadyChecked = false;
		inputPlanOp = NULL;
		filteredSelList = NULL;
		positions = NULL;
		aggPos = NULL;
		numGrpByFlds = numAggs = 0;
	}
	
	virtual ~Aggregates(){
		// do not delete selList;
		// couses problems
		// delete sequenceAttr;
		// delete inputPlanOp;
		// delete filteredSelList;		// do not destroy
	}
	
	bool isApplicable();
	virtual List<BaseSelection*>* getSelectList(){
	     return selList;
	}
	List<BaseSelection*>* filterList(){
		return filteredSelList;
	}
	virtual void typify(Site* inputPlanOp);
	
	virtual TypeID* getTypeIDs(){
		return typeIDs;
	}

	virtual String *getOrderingAttrib(){
		return iterator->getOrderingAttrib();
	}
	Iterator* createExec();
};

#endif
