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

class ExecMinMax : public ExecAggregate {
     OperatorPtr opPtr;
	ADTCopyPtr copyPtr;
     Type* value;
     size_t valueSize;
public:
	ExecMinMax(OperatorPtr opPtr, ADTCopyPtr copyPtr, Type* value, 
		size_t valueSize) :
		opPtr(opPtr), copyPtr(copyPtr), value(value), valueSize(valueSize) {}
	void initialize(const Type* input){
		copyPtr(input, value, valueSize);	
	}
	void update(const Type* input){
		Type* boolVal;
		opPtr(input, value, boolVal);
		if(boolVal){
			copyPtr(input, value, valueSize);	
		}
	}
	Type* getValue(){
		return value;
	}
	virtual ~ExecMinMax(){
		delete value;
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
	ExecAggregate* createExec(){
		Type* value;
		size_t valueSize;
		ADTCopyPtr copyPtr;
		TRY(copyPtr = getADTCopyPtr(typeID), NULL);
		value = allocateSpace(typeID, valueSize);
		ExecAggregate* retVal;
		retVal = new ExecMinMax(opPtr, copyPtr, value, valueSize);
		return retVal;
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

class Aggregates : public Site {
public:
	Aggregates(
		List<BaseSelection*>* selectClause,	// queries select clause
		BaseSelection * sequenceby,	// queries select clause
		BaseSelection* withPredicate,
		List<BaseSelection*>* groupBy ,	// group by clause
		BaseSelection* having = NULL			// having clause
	) : Site(), selList(selectClause),sequenceAttr(sequenceby),
			withPredicate(withPredicate),groupBy(groupBy)
	{
		
		Site::mySelect = selList;
		if(selList){
			numFlds = selList->cardinality();
			aggFuncs = new (Aggregate*)[numFlds];
			aggExecs = new (ExecAggregate*)[numFlds];
			typeIDs = new TypeID[numFlds];
			for(int i = 0; i < numFlds; i++){
				aggFuncs[i] = NULL;
				aggExecs[i] = NULL;
				typeIDs[i] = UNKN_TYPE;
			}
			retTuple = new Tuple[numFlds];
		}
		else {
			numFlds = 0;
			aggFuncs = NULL;
			aggExecs = NULL;
			retTuple = NULL;
		}
		isApplicableValue = false;
		alreadyChecked = false;
		iterator = NULL;
		done = false;
	}
	
	virtual ~Aggregates(){
		// do not delete selList;
		delete sequenceAttr;
		delete iterator;
		delete [] retTuple;
		delete filteredSelList;		// do not destroy
	}
	
	virtual bool isApplicable();
	virtual List<BaseSelection*>* getSelectList(){
	     return selList;
	}
	virtual List<BaseSelection*>* filterList(){
		return filteredSelList;
	}
	virtual void typify(Site* inputIterator);
	
	virtual void enumerate();

	virtual void initialize();

	virtual const Tuple* getNext();
	
	virtual TypeID* getTypeIDs(){
		return typeIDs;
	}

	virtual String *getOrderingAttrib(){
		return iterator->getOrderingAttrib();
	}
	// Need to check this..
	virtual void reset(int lowRid, int highRid){
			TRY(iterator->reset(lowRid, highRid), );
	}


private:
	
	List<BaseSelection*>* selList;
	List<BaseSelection*>* filteredSelList;
	BaseSelection * sequenceAttr;
	bool isApplicableValue;
	bool alreadyChecked;
	TypeID seqAttrType;
	int seqAttrPos;
	BaseSelection* withPredicate;	
	Site * iterator;
	int withPredicatePos;	
	List<BaseSelection*>*groupBy;

	int *positions;
	int *taboo;
	TypeID* typeIDs;

	Tuple* retTuple;
	Aggregate** aggFuncs;
	ExecAggregate** aggExecs;
	bool done;
};

#endif
