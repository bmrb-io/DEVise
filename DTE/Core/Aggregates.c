#include "Aggregates.h"

bool Aggregates::isApplicable(){
	
	// Are there any aggregates in the selectClause?
	
	if (selList == NULL )
		return false;

	assert(!filteredSelList);

	if (alreadyChecked)		// not necessary
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
			else if(*name == "count" && numArgs == 1){
				isApplicableValue = true;
				curr = args->get();
				aggFuncs[i] = new CountAggregate();
			}
			else if(*name == "sum" && numArgs == 1){
				isApplicableValue = true;
				curr = args->get();
				aggFuncs[i] = new SumAggregate();
			}
			else if(*name == "avg" && numArgs == 1){
				isApplicableValue = true;
				curr = args->get();
				aggFuncs[i] = new AvgAggregate();
			}
		}
		filteredSelList->append(curr);
		selList->step();
		i++;
	}
	return isApplicableValue;
}

void Aggregates::typify(Site* inputPlanOp){
	this->inputPlanOp = inputPlanOp;
	int numFlds = inputPlanOp->getNumFlds();
	assert(numFlds == selList->cardinality());
	const TypeID* inptypes = inputPlanOp->getTypeIDs();
	for(int i = 0; i < numFlds; i++){
		typeIDs[i] = aggFuncs[i]->typify(inptypes[i]);
	}
}

Iterator* Aggregates::createExec(){
	assert(inputPlanOp);
	Iterator* inputIter = inputPlanOp->createExec();
	ExecAggregate** aggExecs = new (ExecAggregate*)[numFlds];
	for(int i = 0; i < numFlds; i++){
		aggExecs[i] = aggFuncs[i]->createExec();
	}
	return new StandAggsExec(inputIter, aggExecs, numFlds);
}

void StandAggsExec::initialize(){
	assert(inputIter);
	inputIter->initialize();
}

const Tuple* StandAggsExec::getNext(){
	const Tuple* currt = inputIter->getNext();
	if(!currt){
		return NULL;
	}
	for(int i = 0; i < numFlds; i++){
		aggExecs[i]->initialize(currt[i]);
	}
	while((currt = inputIter->getNext())){
		for(int i = 0; i < numFlds; i++){
			aggExecs[i]->update(currt[i]);
		}
	}
	for(int i = 0; i < numFlds; i++){
		retTuple[i] = aggExecs[i]->getValue();
	}
	return retTuple;
}
