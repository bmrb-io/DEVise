#ifdef __GNUG__
#pragma implementation "Array.h"
#endif

#include "ExecOp.h"
#include "listop.h"
#include "RTreeRead.h"

Array<ExecExpr*>* dummy3; // Just needed for pragma implementation

void IndexScanExec::initialize(){
	assert(index);
	index->initialize();
	assert(inputIt);
	inputIt->initialize();
}

const Tuple* IndexScanExec::getNext(){
	bool cond = false;
	const Tuple* input;
	assert(next);
	while(!cond){
		Offset offset = index->getNextOffset();
		if(offset.isNull()){
			return NULL;
		}
		RecId recId = index->getRecId();
		input = inputIt->getThis(offset, recId);
		assert(input);
		cond = evaluateList(myWhere, input);
	}
	tupleFromList(next, mySelect, input);
	return next;
}

const Tuple* SelProjExec::getNext(){
	bool cond = false;
	const Tuple* input;
	assert(next);
	while(!cond){
		input = inputIt->getNext();
		if(!input){
			return NULL;
		}
		cond = evaluateList(myWhere, input);
	}
	tupleFromList(next, mySelect, input);
	return next;
}

const Tuple* NLJoinExec::getNext(){
	bool cond = false;
	const Tuple* innerTup = NULL;
	if(firstEntry){
		outerTup = left->getNext();
		firstEntry = false;
	}
	while(cond == false){
		if(firstPass){
			if((innerTup = right->getNext())){
				innerRel.append((Tuple*)innerTup);	// must store the values!
				// need to fix appendt to take const Tuple*
			}
			else{
				firstPass = false;
				innerRel.rewind();
				if(innerRel.atEnd()){
					return NULL;
				}
				innerTup = innerRel.get();
				innerRel.step();
				outerTup = left->getNext();
			}
		}
		else{
			if(innerRel.atEnd()){
				innerRel.rewind();
				outerTup = left->getNext();
			}
			innerTup = innerRel.get();
			innerRel.step();
		}
		assert(innerTup);
		if(!outerTup){
			return NULL;
		}
		cond = evaluateList(myWhere, outerTup, innerTup);
	}
	assert(outerTup);
	assert(next);
	tupleFromList(next, mySelect, outerTup, innerTup);
	return next;
}

const Tuple* UnionExec::getNext(){
	const Tuple* next;
	if(runningFirst){
		if((next = iter1->getNext())){
			return next;
		}
		else{
			runningFirst = false;
			iter2->initialize();
			return iter2->getNext();
		}
	}
	else{
		return iter2->getNext();
	}
}

IndexScanExec::~IndexScanExec(){
	delete index;
	delete inputIt;
	destroyArray(*mySelect);
	delete mySelect;
	destroyArray(*myWhere);
	delete myWhere;
	delete [] next;
}
