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

	if(groupBy && !groupBy->isEmpty()){
		positions = new int[groupBy->cardinality()];
	}

	int i = 0;
	numGrpByFlds = 0;
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
		else if(groupBy && !groupBy->isEmpty()){

			// must be grouping field

			isApplicableValue = true;
			assert(numGrpByFlds < groupBy->cardinality());
			positions[numGrpByFlds++] = i;
			aggFuncs[i] = new GroupAttribute();
		}
		filteredSelList->append(curr);
		selList->step();
		i++;
	}

	if(!groupBy || groupBy->isEmpty()){
		return isApplicableValue;
	}

	assert(numGrpByFlds > 0);
	numAggs = 0;
	int tmp = 0;
	aggPos = new int[numFlds - numGrpByFlds];
	for(int k = 0; k < numFlds; k++){
		if(tmp >= numGrpByFlds || positions[tmp] != k){
			assert(numAggs < numFlds - numGrpByFlds);
			aggPos[numAggs++] = k;
		}
		else{
			tmp++;
		}
	}
	assert(numAggs + numGrpByFlds == numFlds);

	// Check if groupbys are there - find out positions and num of flds
	// Create a new GroupAttribute object
	// Error checking should include matching of select & groupby clauses
	
	i=0;
	groupBy->rewind();
	while(!groupBy->atEnd()){
		bool match = false;
		selList->rewind();
		int j = 0;
		while(!selList->atEnd()){
			if (selList->get()->match(groupBy->get())){
				match = true;
				break;
			}
			selList->step();
			j++;
		}	
		assert(match);
		
		assert(positions[i] == j); // position in the select list
		i++;
		groupBy->step();
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
		assert(aggFuncs[i]);
		aggExecs[i] = aggFuncs[i]->createExec();
	}

	if(numGrpByFlds > 0){
		return new StandGroupByExec(inputIter, aggExecs, numFlds,
			positions, numGrpByFlds, aggPos, numAggs);
	}
	else{ 
		// No group by 
		return new StandAggsExec(inputIter, aggExecs, numFlds);
	}
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

void StandGroupByExec::initialize(){
	assert(inputIter);
	inputIter->initialize();
	currInTup = inputIter->getNext();
}

const Tuple* StandGroupByExec::getNext(){

	if(!currInTup){
		return NULL;
	}
	for(int i = 0; i < numFlds; i++){
		aggExecs[i]->initialize(currInTup[i]);
	}
	currInTup = inputIter->getNext();
	while(currInTup && !isNewGroup(currInTup)){
		for(int i = 0; i < numAggs; i++){
			aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
		}
		currInTup = inputIter->getNext();
	}

	for(int i = 0; i < numFlds; i++){
		retTuple[i] = aggExecs[i]->getValue();
	}
	return retTuple;
}
