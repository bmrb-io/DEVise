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

/* This file defines the classes for calculating the Similarity Vectors as a
 * moving aggregate for all the sub-sequences from the stream of the Sequences 
 * of values.
*/

#include "Aggregates.h"

// This class is to execute the creation of aggregate for a similarity vector
// This is for calculating the simple aggregate for a table.
// It is used as a base class for ExecMovSeqSimVec class only, never alone.

class ExecSeqSimVec : public ExecAggregate {
protected:
  OperatorPtr addPtr;
  ADTCopyPtr copyPtr;
  DestroyPtr seqDestroy;
  Type* seqVec;
  size_t valueSize;

public:
  ExecSeqSimVec(OperatorPtr addPtr, ADTCopyPtr copyPtr, DestroyPtr seqDestroy,
			Type* value, size_t valueSize) : 
	addPtr(addPtr), copyPtr(copyPtr), seqDestroy(seqDestroy),
	seqVec(value), valueSize(valueSize) {}

  void initialize(const Type* input) {
	  copyPtr(input, seqVec, valueSize);
  }

  void update(const Type* input)  {
	  addPtr(seqVec, input, seqVec);
  }
				   
  const Type* getValue() {
	return seqVec;
  }

  virtual ~ExecSeqSimVec(){
	  seqDestroy(seqVec);
  }

};

// This class derived from ExecSeqSimVec is for calculating the moving aggregate
// for SeqSimVec. 
class ExecMovSeqSimVec : public ExecSeqSimVec {
  TupleLoader* tupLoad;
  OperatorPtr subPtr;
  OperatorPtr eqPtr;
  int first_time;

// These parameters were defined to deal to creation of MBR's over the 
// points for a trail of sub-sequences. But now this is not needed as these
// points are directly to be inserted in the RTrees which will create the MBRs
// for it.
/*
  int temp_count;
  double par;
  double old_mbr[4*NFA+4];
  double new_mbr[4*NFA+4];
  double v_new, v_old;
  int starting_offset, ending_offset; */
public:
	ExecMovSeqSimVec(OperatorPtr addPtr, ADTCopyPtr copyPtr, 
				DestroyPtr seqDestroy, Type* value, size_t valueSize,
				TupleLoader* tupLoad, OperatorPtr subPtr, OperatorPtr eqPtr) :
    ExecSeqSimVec(addPtr, copyPtr, seqDestroy, value, valueSize), 
	tupLoad(tupLoad),subPtr(subPtr), eqPtr(eqPtr) {
//		par = 1000*1.6*sqrt(((SeqSimVec*)seqVec)->omega);
//		temp_count = 0;
//		v_old = pow(par, 2*NFA);
		first_time = 1;
	}

// This is called whenever we start the aggregate for a group
	virtual void initialize(const Type* input) {
		ExecSeqSimVec::initialize(input);
		tupLoad->insert(&input);
	}

// For each element in the group this is called.
	virtual void update(const Type* input)  {
		if (tupLoad->empty()) {
			ExecSeqSimVec::initialize(input);
		}
		else {
			ExecSeqSimVec::update(input);
		}
		tupLoad->insert(&input);
	}

// This should return the value of the aggregate i.e. the summary vector
	virtual const Type* getValue() {
		assert(!tupLoad->empty());
		// Call the function to find the MBR and insert it in RTree if new one
		// created. Check for count to see whether this point will
		// qualify to be in MBR or not.
//		CreateMBR();
		return seqVec;
	}

//	void CreateMBR();	// This function is called to create a new MBR from
						// the points in the trail (if necessary). Right now 
						// this is no longer needed to be called as this work
						// is done in RTrees itself.
	virtual void dequeue(int n);

};

// This class does the type checking for the SeqSimVec aggregate operator
// and then creates an Exec for calculating it.
class SeqSimVecAggregate : public Aggregate {
protected:
  OperatorPtr opPtr;
  TypeID typeID;
  int omega;

public:
	SeqSimVecAggregate(int omega_value) : omega(omega_value), opPtr(NULL) {}

	TypeID typify (TypeID inputT) {
		TypeID resultT;
		typeID = inputT;

		GeneralPtr* genPtr = NULL;
		TRY(genPtr = getOperatorPtr("addTup", SEQSV_TP, "double", resultT), NULL); 
		assert(genPtr);
		opPtr = genPtr->opPtr;
		assert(resultT == SEQSV_TP);
		return SEQSV_TP;
	}

	ExecAggregate* createExec() {
		DestroyPtr seqDestroy;
		TRY(seqDestroy = getDestroyPtr(SEQSV_TP), NULL);
		ADTCopyPtr copyPtr;
		TRY(copyPtr = getADTCopyPtr(SEQSV_TP), NULL);

		size_t valueSize;
		Type *value = allocateSpace(SEQSV_TP, valueSize);
		((SeqSimVec*)value)->omega = omega;
		ExecAggregate* retVal = new ExecSeqSimVec(opPtr, copyPtr, seqDestroy, value, valueSize);
		return retVal;

	}
};

// This class derived from SeqSimVecAggregate is used to calculate the moving
// aggregate for the SeqSimVec vector
class MovSeqSimVecAggregate : public SeqSimVecAggregate {
  OperatorPtr eqPtr, subPtr;
  TupleLoader *tupLoad;
public:
  MovSeqSimVecAggregate(int omega_value) : SeqSimVecAggregate(omega_value) {}
  ExecAggregate* createExec(){
	DestroyPtr seqDestroy;
	TRY(seqDestroy = getDestroyPtr(SEQSV_TP), NULL);
	ADTCopyPtr copyPtr;
	TRY(copyPtr = getADTCopyPtr(SEQSV_TP), NULL);

	size_t valueSize;
	Type *value = allocateSpace(SEQSV_TP, valueSize);
	((SeqSimVec*)value)->omega = omega;

	TypeID resultT;
	GeneralPtr* genPtr = NULL;
	TRY(genPtr = getOperatorPtr("addTup", SEQSV_TP, "double", resultT), NULL);
	assert(genPtr);
	opPtr = genPtr->opPtr;
	assert(resultT == SEQSV_TP);

	TRY(genPtr = getOperatorPtr("-", SEQSV_TP, "double", resultT), NULL);
	assert(genPtr);
	subPtr = genPtr->opPtr;
	assert(resultT == SEQSV_TP);

	TypeID retVal;  // is a dummy
	//TRY(genPtr = getOperatorPtr("=", typeID, typeID,retVal), NULL);
	//assert(genPtr);
	//eqPtr = genPtr->opPtr;

	int numFlds = 1;
	tupLoad = new TupleLoader;
	TRY(tupLoad->open(numFlds, &typeID), NULL);
	return new ExecMovSeqSimVec(opPtr, copyPtr, seqDestroy, value, valueSize,
				tupLoad, subPtr, eqPtr);

  }

};
