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
		grpByPos = new int[groupBy->cardinality()];
	}
	
	bool hasMoving = false; //to check moving and normal aggs are not mixed
	int i = 0;
	numGrpByFlds = 0;
	while(!selList->atEnd()){
		
		BaseSelection* curr = selList->get();

		if(curr->selectID() == CONSTRUCTOR_ID){
			Constructor* function = (Constructor*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const string* name = function->getName();
			args->rewind();
			if(*name == "min"){
				if(numArgs == 1){
					assert(!hasMoving);
					isApplicableValue = true;
					curr = args->get();
					aggFuncs[i] = new MinAggregate();
				}
				else if(numArgs == 3){
					isApplicableValue = true;
					curr = args->get();
					windowLow = *(int *) args->get();
					windowHigh = *(int *) args->get();
					assert(windowHigh >= windowLow);
					aggFuncs[i] = new MovMinAggregate();
					hasMoving = true;
				}
			}
			else if(*name == "max"){
				if(numArgs == 1){
					assert(!hasMoving);
					isApplicableValue = true;
					curr = args->get();
					aggFuncs[i] = new MaxAggregate();
				}
				else if(numArgs == 3){
					isApplicableValue = true;
					curr = args->get();
					windowLow = *(int *) args->get();
					windowHigh = *(int *) args->get();
					assert(windowHigh >= windowLow);
					aggFuncs[i] = new MovMaxAggregate();
					hasMoving = true;
				}
			}
			else if(*name == "count"){
				if(numArgs == 1){
					assert(!hasMoving);
					isApplicableValue = true;
					curr = args->get();
					aggFuncs[i] = new CountAggregate();
				}
				else if(numArgs == 3){
					isApplicableValue = true;
					curr = args->get();
					windowLow = *(int *) args->get();
					windowHigh = *(int *) args->get();
					assert(windowHigh >= windowLow);
					aggFuncs[i] = new MovCountAggregate();
					hasMoving = true;
				}
			}
			else if(*name == "sum"){
				if(numArgs == 1){
					assert(!hasMoving);
					isApplicableValue = true;
					curr = args->get();
					aggFuncs[i] = new SumAggregate();
				}
				else if(numArgs == 3){
					isApplicableValue = true;
					curr = args->get();
					windowLow = *(int *) args->get();
					windowHigh = *(int *) args->get();
					assert(windowHigh >= windowLow);
					aggFuncs[i] = new MovSumAggregate();
					hasMoving = true;
				}
			}
			else if(*name == "avg"){
				if(numArgs == 1){
					assert(!hasMoving);
					isApplicableValue = true;
					curr = args->get();
					aggFuncs[i] = new AvgAggregate();
				}
				else if(numArgs == 3){
					isApplicableValue = true;
					curr = args->get();
					windowLow = *(int *) args->get();
					windowHigh = *(int *) args->get();
					assert(windowHigh >= windowLow);
					aggFuncs[i] = new MovAvgAggregate();
					hasMoving = true;
				}
			}
		}
		else if(groupBy && !groupBy->isEmpty()){

		  // grouping and aggregation cannot be on same field
		  
			isApplicableValue = true;
			assert(numGrpByFlds < groupBy->cardinality());
			grpByPos[numGrpByFlds++] = i;
			aggFuncs[i] = new GroupAttribute();
		}
		else if (sequenceAttr){
		  // must be sequence vy attribute
		  // since groupby and sequence by cannot be together
		  seqByPos[numSeqByFlds++] = i; 
		}
		filteredSelList->append(curr);
		selList->step();
		i++;
	}


	// check there is no sequence by and grouping together
	assert (!(groupBy && sequenceAttr));

	// Need to add a check that groupbys and select clauses match 

	if(!groupBy || groupBy->isEmpty()){
		return isApplicableValue;
	}

	assert(numGrpByFlds > 0);
	numAggs = 0;
	int tmp = 0;
	aggPos = new int[numFlds - numGrpByFlds];
	for(int k = 0; k < numFlds; k++){
		if(tmp >= numGrpByFlds || grpByPos[tmp] != k){
			assert(numAggs < numFlds - numGrpByFlds);
			aggPos[numAggs++] = k;
		}
		else{
			tmp++;
		}
	}
	assert(numAggs + numGrpByFlds == numFlds);

	return isApplicableValue;
}

void Aggregates::typify(const string& name, Site* inputPlanOp){
//	this->name = name;
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
	TRY(Iterator* inputIter = inputPlanOp->createExec(), NULL);
	assert(inputIter);
	ExecAggregate** aggExecs = new ExecAggregate*[numFlds];
	for(int i = 0; i < numFlds; i++){
		assert(aggFuncs[i]);
		aggExecs[i] = aggFuncs[i]->createExec();
	}

	if(numGrpByFlds > 0){
		return new StandGroupByExec(inputIter, aggExecs, numFlds,
			grpByPos, numGrpByFlds, aggPos, numAggs);
	}
	else if(numSeqByFlds > 0){
		return new MovAggsExec(inputIter, aggExecs, numFlds, seqByPos,
			numSeqByFlds, aggPos, numAggs, windowLow, windowHigh);
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
	int i;
	if(!currt){
		return NULL;
	}
	for(i = 0; i < numFlds; i++){
		aggExecs[i]->initialize(currt[i]);
	}
	while((currt = inputIter->getNext())){
		for(i = 0; i < numFlds; i++){
			aggExecs[i]->update(currt[i]);
		}
	}

	for(i = 0; i < numFlds; i++){
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

	int i;
	if(!currInTup){
		return NULL;
	}
	for(i = 0; i < numFlds; i++){
		aggExecs[i]->initialize(currInTup[i]);
	}
	currInTup = inputIter->getNext();
	while(currInTup && !isNewGroup(currInTup)){
		for(i = 0; i < numAggs; i++){
			aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
		}
		currInTup = inputIter->getNext();
	}

	for(i = 0; i < numFlds; i++){
		retTuple[i] = aggExecs[i]->getValue();
	}
	return retTuple;
}

void MovAggsExec::initialize(){
	assert(inputIter);
	inputIter->initialize();
	currInTup = inputIter->getNext();
	numUniqueTuples = 1;
}

const Tuple* MovAggsExec::getNext(){

  
	int i;
	
	if(!currInTup){
		return NULL;
	}

	int toDeque = (windowHeight) ? 0 : numTuplesToBeDropped[nextDrop];

	for(i = 0; i < numFlds; i++){
		aggExecs[i]->dequeue(toDeque);
		aggExecs[i]->initialize(currInTup[i]);
	}

	int numTuples = 1; // currInTup has a tuple already
	currInTup = inputIter->getNext();
	//	while(currInTup && !isNewGroup(currInTup) ){
	while(currInTup && !isEndWindow(currInTup)){
		for(i = 0; i < numAggs; i++){
			aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
		}
		currInTup = inputIter->getNext();
		numTuples++;
	}

	if (windowHeight) {
	  numTuplesToBeDropped[nextDrop] =  numTuples;
	  nextDrop = (nextDrop +1) % windowHeight; 
	}

	for(i = 0; i < numFlds; i++){
		retTuple[i] = aggExecs[i]->getValue();
	}
	return retTuple;
}
