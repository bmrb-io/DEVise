#include "Aggregates.h"

bool Aggregates::isApplicable(){
	
	// Is there any aggregates in the selectClause?
	
	if (selList == NULL )
		return false;

	if (alreadyChecked)
		return isApplicableValue;
	
	alreadyChecked = true;
	isApplicableValue = false;

	selList->rewind();
	filteredSelList = new List<BaseSelection*>();

	int i = 0;
	while(!selList->atEnd()){
		
		BaseSelection* curr = selList->get();

		if(curr->isGlobal() && curr->selectID() == METHOD_ID){
			Method* function = (Method*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const String* name = function->getName();
			args->rewind();
			if(*name == "min" && numArgs == 1){
				isApplicableValue = true;
				curr = args->get();
				aggFuncs[i] = new MinAggregate();
			}
			else if(*name == "max" && numArgs == 1){
				isApplicableValue = true;
				curr = args->get();
				aggFuncs[i] = new MaxAggregate();
			}
		}
		filteredSelList->append(curr);
		selList->step();
		i++;
	}
	return isApplicableValue;
}

void Aggregates::typify(Site* inputIterator){
	iterator = inputIterator;
	int numFlds = inputIterator->getNumFlds();
	assert(numFlds == selList->cardinality());
	const TypeID* inptypes = inputIterator->getTypeIDs();
	for(int i = 0; i < numFlds; i++){
		typeIDs[i] = aggFuncs[i]->typify(inptypes[i]);
	}
}

void Aggregates::enumerate(){
	assert(iterator);
	iterator->enumerate();
	for(int i = 0; i < numFlds; i++){
		aggExecs[i] = aggFuncs[i]->createExec();
	}
}

void Aggregates::initialize(){
	assert(iterator);
	iterator->initialize();
}

const Tuple* Aggregates::getNext(){
	if(done){
		return NULL;
	}
	done = true;
	int numFlds = getNumFlds();
	const Tuple* currt = iterator->getNext();
	for(int i = 0; i < numFlds; i++){
		aggExecs[i]->initialize(currt[i]);
	}
	while((currt = iterator->getNext())){
		for(int i = 0; i < numFlds; i++){
			aggExecs[i]->update(currt[i]);
		}
	}
	for(int i = 0; i < numFlds; i++){
		retTuple[i] = aggExecs[i]->getValue();
	}
	return retTuple;
}
