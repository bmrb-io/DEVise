#include "ExecOp.h"
#include "listop.h"
#include "RTreeRead.h"

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
		inputIt->setOffset(offset);
		input = inputIt->getNext();
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
