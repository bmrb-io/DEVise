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

#ifndef __GNUG__
using namespace std;
#endif

class ExecAggregate {
public:
  virtual void initialize(const Type* input) = 0;
  virtual void update(const Type* input) = 0;
  virtual bool isDifferent(const Type* newVal){
  	assert(!"Used only in grouping aggregates");
	return false;
  }
  virtual const Type* getValue() = 0;
  virtual void dequeue(int n){assert(!"Used only by moving aggregates");}
};

// Standard aggregate execs

class ExecMinMax : public ExecAggregate {
protected:
  OperatorPtr opPtr;
  ADTCopyPtr copyPtr;
  DestroyPtr destroyPtr;
  Type* minMax;
  size_t valueSize;

public:
  ExecMinMax(OperatorPtr opPtr, ADTCopyPtr copyPtr, DestroyPtr destroyPtr,
	     Type* value, size_t valueSize) : 
    opPtr(opPtr), copyPtr(copyPtr), destroyPtr(destroyPtr), 
    minMax(value), valueSize(valueSize) {}

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


class ExecCount : public ExecAggregate {
protected:
  int count;
  Type* result;
  DestroyPtr intDestroy;
  
public:
  ExecCount(DestroyPtr intDestroy) : intDestroy(intDestroy){
	count = 0;
  }

  void initialize(const Type* input) {
    count = 1;	
  }

  void update(const Type* input)  {
    count++;
  }
	
  const Type* getValue() {
    result = (Type*) count;  // assumes int is inlined
    return result;
  }

  virtual ~ExecCount() {
    intDestroy(result);
  }

};


class ExecSum : public ExecAggregate {
protected:
  OperatorPtr addPtr;
  ADTCopyPtr copyPtr;
  DestroyPtr sumDestroy;
  Type* sum;
  size_t valueSize;

public:
  ExecSum(OperatorPtr addPtr, ADTCopyPtr copyPtr, DestroyPtr sumDestroy,
	  Type* value, size_t valueSize) :
    addPtr(addPtr), copyPtr(copyPtr), sumDestroy(sumDestroy), 
    sum(value), valueSize(valueSize) {}

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
	
  const Type* getValue() {
    if (count == 0)
      return (Type *) NULL; // What should be returned here?

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

// Moving aggregate Execs

class ExecMovMinMax : public ExecMinMax {
	TupleLoader* tupLoad;
	OperatorPtr eqPtr;
public:
	ExecMovMinMax(OperatorPtr opPtr, ADTCopyPtr copyPtr, 
		      DestroyPtr destroyPtr,Type* value, size_t valueSize, 
		      TupleLoader* tupLoad, OperatorPtr eqPtr) : 
		ExecMinMax(opPtr, copyPtr, destroyPtr, value, valueSize),
		tupLoad(tupLoad), eqPtr(eqPtr) {}
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

class ExecMovCount : public ExecCount {
public:
  ExecMovCount(DestroyPtr destroyPtr) : ExecCount(destroyPtr) {}

  virtual void dequeue(int n){ 
     count -= n;
  }
};

class ExecMovSum : public ExecSum {
  TupleLoader* tupLoad;
  OperatorPtr subPtr;
  OperatorPtr eqPtr;
  
public:
  ExecMovSum(OperatorPtr addPtr, ADTCopyPtr copyPtr, DestroyPtr sumDestroy,
	     Type* value, size_t valueSize, TupleLoader* tupLoad, 
	     OperatorPtr subPtr, OperatorPtr eqPtr) : 
    ExecSum(addPtr, copyPtr, sumDestroy, value, valueSize), tupLoad(tupLoad), 
    subPtr(subPtr), eqPtr(eqPtr) {}

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

class ExecMovAverage : public ExecAverage {
	TupleLoader* tupLoad;
	OperatorPtr eqPtr, subPtr;
public:
  ExecMovAverage(OperatorPtr addPtr,OperatorPtr divPtr,PromotePtr promotePtr,
		 DestroyPtr sumDestroy, ADTCopyPtr copyPtr, Type* value, 
		 size_t valueSize, TupleLoader* tupLoad, 
		 OperatorPtr subPtr,OperatorPtr eqPtr) : 
    ExecAverage(addPtr, divPtr, promotePtr, sumDestroy, copyPtr, value, 
		valueSize), tupLoad(tupLoad), subPtr(subPtr), eqPtr(eqPtr) {}
    	
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

class ExecGroupAttr : public ExecAggregate {
protected:
	ADTCopyPtr copyPtr;
	OperatorPtr eqPtr;
	Type* prevGroup;
	size_t valueSize;

public:
	ExecGroupAttr(ADTCopyPtr copyPtr, OperatorPtr eqPtr, Type* value, 
		size_t valueSize) :
		copyPtr(copyPtr), eqPtr(eqPtr), prevGroup(value), 
		valueSize(valueSize) {}

	virtual bool isDifferent(const Type* newVal){
		Type* cmp;
		eqPtr(prevGroup, newVal, cmp);
		return !(cmp ? true : false);
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

class ExecBinAttr : public ExecAggregate {
protected:
	Type* step;
	ADTCopyPtr copyPtr;
	OperatorPtr ltPtr;
	OperatorPtr addPtr;
	DestroyPtr destroyPtr;
	Type* low;
	Type* high;
	size_t valueSize;

public:
	ExecBinAttr(ConstantSelection* init, ConstantSelection* cs_step,
		TypeID inputT)
	{
		assert(inputT == init->getTypeID());
		assert(inputT == cs_step->getTypeID());
		GeneralPtr* genPtr = NULL;
		TypeID boolT;
		genPtr = getOperatorPtr("<", inputT, inputT, boolT);
		assert(genPtr);
		ltPtr = genPtr->opPtr;
		genPtr = getOperatorPtr("+", inputT, inputT, boolT);
		assert(genPtr);
		addPtr = genPtr->opPtr;

		copyPtr = getADTCopyPtr(inputT);
		assert(copyPtr);
		step = allocateSpace(inputT, valueSize);
		low = allocateSpace(inputT, valueSize);
		high = allocateSpace(inputT, valueSize);

		destroyPtr = getDestroyPtr(inputT);
		assert(destroyPtr);

		copyPtr(init->getValue(), high);
		copyPtr(cs_step->getValue(), step);
	}

	~ExecBinAttr(){
		destroyPtr(step);
		destroyPtr(low);
		destroyPtr(high);
	}

	virtual bool isDifferent(const Type* newVal){
		Type* cmp;
		ltPtr(newVal, high, cmp);
		return !(cmp ? true : false);
	}

	virtual void initialize(const Type* input){
		copyPtr(high, low, valueSize);
		addPtr(low, step, high);
		return;
	}

	virtual void update(const Type* input){
		assert(0);
	}

	virtual const Type* getValue(){
		return low;
	}
};

class ExecSeqAttr : public ExecGroupAttr {
  TupleLoader* tupLoad;

public:
  ExecSeqAttr(ADTCopyPtr copyPtr, OperatorPtr eqPtr, Type* value, 
	      size_t valueSize, TupleLoader* tupLoad) :
    ExecGroupAttr(copyPtr, eqPtr, value, valueSize), tupLoad(tupLoad) {}

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

// aggregate type checking classes

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

  ExecAggregate* createExec(){
    DestroyPtr destroyPtr;
    TRY(destroyPtr = getDestroyPtr(typeID), NULL);
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    return new ExecMinMax(opPtr, copyPtr, destroyPtr, value, valueSize);
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
    DestroyPtr intDestroy = getDestroyPtr(INT_TP);
    assert(intDestroy);
    return new ExecCount(intDestroy);
  }
};

class SumAggregate : public Aggregate {
protected:
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
    DestroyPtr sumDestroy;
    TRY(sumDestroy = getDestroyPtr(typeID), NULL);
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    return new ExecSum(opPtr, copyPtr, sumDestroy, value, valueSize);
  }

};

class AvgAggregate : public Aggregate {
protected:
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

    return new ExecAverage(addPtr, divPtr, promotePtr, 
			   sumDestroy, copyPtr, value, valueSize);
  }

};

// moving aggregate type checking classes - derived from their "normal" classes

class MovMinAggregate : public MinAggregate {
  OperatorPtr eqPtr;
  TupleLoader *tupLoad;
public:

  ExecAggregate* createExec(){
    DestroyPtr destroyPtr;
    TRY(destroyPtr = getDestroyPtr(typeID), NULL);
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    TypeID retVal;  // is a dummy	       
    GeneralPtr* genPtr;
    TRY(genPtr = getOperatorPtr("=", typeID, typeID, retVal), NULL); 
    assert(genPtr);
    eqPtr = genPtr->opPtr;

    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &typeID), NULL);

    return new ExecMovMinMax(opPtr, copyPtr, destroyPtr, value, 
			     valueSize, tupLoad, eqPtr);
  }
};

class MovMaxAggregate : public MaxAggregate {
  OperatorPtr eqPtr;
  TupleLoader *tupLoad;
public:

  ExecAggregate* createExec(){
    DestroyPtr destroyPtr;
    TRY(destroyPtr = getDestroyPtr(typeID), NULL);
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    TypeID retVal;  // is a dummy	       
    GeneralPtr* genPtr;
    TRY(genPtr = getOperatorPtr("=", typeID, typeID,retVal), NULL); 
    assert(genPtr);
    eqPtr = genPtr->opPtr;

    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &typeID), NULL);

    return new ExecMovMinMax(opPtr, copyPtr, destroyPtr, value, 
			     valueSize, tupLoad, eqPtr);
  }
}; 

class MovCountAggregate : public CountAggregate {
  OperatorPtr eqPtr;
  TupleLoader *tupLoad;
public:
  ExecAggregate* createExec() {
    DestroyPtr destroyPtr = getDestroyPtr(INT_TP);
    assert(destroyPtr);
    ExecAggregate* retVal = new ExecMovCount(destroyPtr);
    return retVal;
  }
};

class MovSumAggregate : public SumAggregate {
  OperatorPtr eqPtr, subPtr;
  TupleLoader *tupLoad;
public:
  ExecAggregate* createExec(){
    DestroyPtr sumDestroy;
    TRY(sumDestroy = getDestroyPtr(typeID), NULL);
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    TypeID boolT;
    GeneralPtr* genPtr = NULL;
    TRY(genPtr = getOperatorPtr("+", typeID, typeID, boolT), NULL);
    assert(genPtr);
    opPtr = genPtr->opPtr;
    assert(boolT == typeID);

    TRY(genPtr = getOperatorPtr("-", typeID, typeID, boolT), NULL);
    assert(genPtr);
    subPtr = genPtr->opPtr;
    assert(boolT == typeID);
    
    TypeID retVal;  // is a dummy	       
    TRY(genPtr = getOperatorPtr("=", typeID, typeID,retVal), NULL); 
    assert(genPtr);
    eqPtr = genPtr->opPtr;

    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &typeID), NULL);
    return new ExecMovSum(opPtr, copyPtr, sumDestroy, value, valueSize, 
			  tupLoad, subPtr, eqPtr);
  }

};

class MovAvgAggregate : public AvgAggregate {
  TupleLoader *tupLoad;
  OperatorPtr subPtr;
  OperatorPtr eqPtr;
public:
  ExecAggregate* createExec(){
    DestroyPtr sumDestroy;
    ADTCopyPtr copyPtr;
    TRY(sumDestroy = getDestroyPtr(typeID), NULL);
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);

    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    GeneralPtr* genPtr;
    TypeID boolT;
    TRY(genPtr = getOperatorPtr("-", typeID, typeID, boolT), NULL);
    assert(genPtr);
    subPtr = genPtr->opPtr;
    assert(boolT == typeID);

    TypeID retVal;  // is a dummy	       
    TRY(genPtr = getOperatorPtr("=", typeID, typeID,retVal), NULL); 
    assert(genPtr);
    eqPtr = genPtr->opPtr;

    int numFlds = 1;
    tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &typeID), NULL);
 
    return new ExecMovAverage(addPtr, divPtr, promotePtr,sumDestroy, copyPtr, 
			      value, valueSize, tupLoad, subPtr, eqPtr);
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

  virtual ExecAggregate* createExec(){
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);
    
    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    return new ExecGroupAttr(copyPtr, eqPtr, value, valueSize);
  }

};

class BinAttribute : public Aggregate {
protected:
	TypeID typeID;
	ConstantSelection* init;
	ConstantSelection* step;
  
public:
  BinAttribute(ConstantSelection* init, ConstantSelection* step) 
  	: init(init), step(step) {}

  virtual TypeID typify(TypeID inputT){	// throws
  	typeID = inputT;
  	return inputT;
  }

  virtual ExecAggregate* createExec(){
  	return new ExecBinAttr(init, step, typeID);
  }

};

class SequenceAttribute : public GroupAttribute{

  virtual ExecAggregate* createExec(){
    ADTCopyPtr copyPtr;
    TRY(copyPtr = getADTCopyPtr(typeID), NULL);
    
    size_t valueSize;
    Type *value = allocateSpace(typeID, valueSize);

    // get a new tupleLoader
    int numFlds = 1;
    TupleLoader* tupLoad = new TupleLoader;
    TRY(tupLoad->open(numFlds, &typeID), NULL);

    return new ExecSeqAttr(copyPtr, eqPtr, value, valueSize, tupLoad);
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
		delete inputIter;
		if(aggExecs){
			for(int i = 0; i < numFlds; i++){
				delete aggExecs[i];
			}
		}
		delete [] aggExecs;
		delete [] retTuple;
	}

	virtual void initialize();
	virtual const Tuple* getNext();

	// Need to check this..
	virtual void reset(int lowRid, int highRid){
		TRY(inputIter->reset(lowRid, highRid), NVOID );
	}
};

class StandGroupByExec : public Iterator {
protected:
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
		numFlds(numFlds),  grpByPos(grpByPos),grpByPosLen(grpByPosLen),
		aggPos(aggPos), numAggs(numAggs) {

		retTuple = new Tuple[numFlds];
		currInTup = NULL;
		//assert(numAggs + grpByPosLen == numFlds);
	}

	virtual ~StandGroupByExec(){
		if(aggExecs){
			for(int i = 0; i < numFlds; i++){
				delete aggExecs[i];
			}
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
    TRY(inputIter->reset(lowRid, highRid), NVOID );
  }

protected:
	bool isNewGroup(const Tuple* tup){
		for (int i = 0; i < grpByPosLen; i++){
			ExecAggregate* curr = aggExecs[grpByPos[i]];
			if(curr->isDifferent(tup[grpByPos[i]])){
			 	return true;
			}
		}
		return false;
	}
};


class MovAggsExec : public StandGroupByExec {
protected:
  int* seqByPos;
  int seqByPosLen;
  int windowLow, windowHigh, fullWindowHeight; // window measurements
  int *numTuplesToBeDropped; // array of size windowHeight 
  int nextDrop; // pointer to element in array numTuplesToBeDropped
  int toDeque; // numver of Tuples to be dropped
  int currWindowHeight;
  const Tuple* currInTup;
  bool firstTime, endOfGroup; 

public:
  
	MovAggsExec(Iterator* inputIter, ExecAggregate** aggExecs,
		int numFlds, int* seqByPos, 
		int seqByPosLen,
		int* aggPos, int numAggs,
		int windowLow, int windowHigh) : 
	  StandGroupByExec (inputIter, aggExecs, numFlds, seqByPos, 
			    seqByPosLen, aggPos, numAggs), 
	  seqByPos(seqByPos), seqByPosLen(seqByPosLen), 
	  windowLow(windowLow), windowHigh(windowHigh) {
	    // seqBypos and grpByPos are same for this class
	    // similarly seqByPosLen and grpByPosLen are same
		retTuple = new Tuple[numFlds];
		currInTup = NULL;
		fullWindowHeight = windowHigh - windowLow;
		assert (fullWindowHeight > 0); 
		numTuplesToBeDropped = new int[fullWindowHeight];
		for (int i = 0; i < fullWindowHeight; i++){
		  numTuplesToBeDropped[i] = 0;   
		}
		nextDrop = currWindowHeight = 0;
	}

	virtual ~MovAggsExec() {
		delete [] numTuplesToBeDropped;
	}

  virtual void initialize();
  virtual const Tuple* getNext();

protected:

  bool isNewSeqVal(const Tuple* tup){
    for (int i = 0; i < seqByPosLen; i++){
      ExecAggregate* curr = aggExecs[seqByPos[i]];
      if(curr->isDifferent(tup[seqByPos[i]])){
	return true;
      }
    }
    return false;
  }

  virtual const Tuple* flushWindow();
  virtual void setupFirst();
  virtual bool currInTupIsValid() { return (currInTup != NULL); }
};

class MovSeqAggsExec : public MovAggsExec {
public :
	MovSeqAggsExec(Iterator* inputIter, ExecAggregate** aggExecs,
		int numFlds, int* seqByPos, 
		int seqByPosLen,
		int* aggPos, int numAggs,
		int windowLow, int windowHigh) : 
	MovAggsExec(inputIter, aggExecs,
		numFlds, seqByPos, 
		seqByPosLen,
		aggPos, numAggs,
		windowLow, windowHigh) {} 

protected :
  virtual const Tuple* flushWindow();
};

class MovGroupByExec : public MovAggsExec {
public:
  
  MovGroupByExec(Iterator* inputIter, ExecAggregate** aggExecs,
		 int numFlds, int* seqByPos, int seqByPosLen, 
		 int* grpByPos, int grpByPosLen,
		 int* aggPos, int numAggs,
		 int windowLow, int windowHigh) : 
    MovAggsExec (inputIter, aggExecs, numFlds, grpByPos, 
		 grpByPosLen, aggPos, numAggs, windowLow, windowHigh) {
      // override assignment of seqByPos and seqByPosLen 
      // done by MovAggsExec constructor
      this->seqByPos = seqByPos;
      this->seqByPosLen = seqByPosLen;
  }
  
protected:
  virtual const Tuple* flushWindow();
  virtual bool currInTupIsValid() { 
    return (currInTup && !isNewGroup(currInTup));
  }
};

class MovSeqGroupByExec : public MovGroupByExec {
public:
  
  MovSeqGroupByExec(Iterator* inputIter, ExecAggregate** aggExecs,
		 int numFlds, int* seqByPos, int seqByPosLen, 
		 int* grpByPos, int grpByPosLen,
		 int* aggPos, int numAggs,
		 int windowLow, int windowHigh) : 
  MovGroupByExec(inputIter, aggExecs,
		 numFlds, seqByPos, seqByPosLen, 
		 grpByPos, grpByPosLen,
		 aggPos, numAggs,
		 windowLow, windowHigh) {}

protected :
  virtual const Tuple* flushWindow();
};

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
	vector<BaseSelection*>& binBy;
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
		vector<BaseSelection*>& binBy,	
		BaseSelection* havingPredicate=NULL)
	  : Site(), selList(selectClause),sequenceBy(sequenceBy),
	    withPredicate(withPredicate),groupBy(groupBy), binBy(binBy),
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

#endif
