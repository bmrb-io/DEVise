#include "Aggregates.h"
#include "MemoryMgr.h"

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
	
	// need to extend for multiple seq by fields
	if (sequenceAttr) {
	  seqByPos = new int[1];
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
			if (numArgs == 1){
			  // simple aggregation
			  assert(!hasMoving);
			  isApplicableValue = true;
			  curr = args->get();
			  if (*name == "min")
			    aggFuncs[i] = new MinAggregate();
			  else if (*name == "max")
			    aggFuncs[i] = new MaxAggregate();
			  else if (*name == "avg")
			    aggFuncs[i] = new AvgAggregate();
			  else if (*name == "sum")
			    aggFuncs[i] = new SumAggregate();
			  else if (*name == "count")
			    aggFuncs[i] = new CountAggregate();
			}
			else if (numArgs == 3) {
			  // moving aggregates
			  isApplicableValue = true;
			  hasMoving = true;			  
			  ConstantSelection *windowArg;

			  curr = args->get();
			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowLow = (int) windowArg->getValue();

			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowHigh = (int)windowArg->getValue();

			  assert(windowHigh > windowLow);

			  if (*name == "min")
			    aggFuncs[i] = new MovMinAggregate();
			  else if (*name == "max")
			    aggFuncs[i] = new MovMaxAggregate();
			  else if (*name == "avg")
			    aggFuncs[i] = new MovAvgAggregate();
			  else if (*name == "sum")
			    aggFuncs[i] = new MovSumAggregate();
			  else if (*name == "count")
			    aggFuncs[i] = new MovCountAggregate();
			}
			// else error?? 
		}
		else if(groupBy && !groupBy->isEmpty()){

		  // grouping and aggregation cannot be on same field
		  
			isApplicableValue = true;
			assert(numGrpByFlds < groupBy->cardinality());
			grpByPos[numGrpByFlds++] = i;
			aggFuncs[i] = new GroupAttribute();
		}
		else if (sequenceAttr) { 
		  seqByPos[numSeqByFlds++] = i;
		  aggFuncs[i] = new SequenceAttribute();  
		}
		filteredSelList->append(curr);
		selList->step();
		i++;
	}


	// check there is no sequence by and grouping together
	//assert (!(groupBy && sequenceAttr));

	// Need to add a check that groupbys and select clauses match 

	if(groupBy && !groupBy->isEmpty()){
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
	}
	
	if (sequenceAttr) {
	  assert (numSeqByFlds > 0);
	  numAggs = 0;
	  int tmp = 0;
	  aggPos = new int[numFlds - numSeqByFlds];
	  for(int k = 0; k < numFlds; k++){
	    if(tmp >= numSeqByFlds || seqByPos[tmp] != k){
	      assert(numAggs < numFlds - numSeqByFlds);
	      aggPos[numAggs++] = k;
	    }
	    else{
	      tmp++;
	    }
	  }

	}
	/*
	assert((numAggs + numGrpByFlds == numFlds) || 
	       (numAggs + numSeqByFlds == numFlds));
	       */
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
	if(!currInTup){
	  return;
	}

	for(int i = 0; i < numFlds; i++){
	  aggExecs[i]->initialize(currInTup[i]);
	}

	currWindowHeight = 1;
	numTuplesToBeDropped[nextDrop] = 1; // for the first tuple read

	currInTup = inputIter->getNext();
	while(currInTup) { 
	  if (isNewGroup(currInTup)) {
	    nextDrop = (nextDrop+1) % fullWindowHeight; 

	    if (currWindowHeight == fullWindowHeight) {
	      break; // end of window
	    }

	    // Add the currInTuple to end of the seq by queue
	    for (int i = 0; i < grpByPosLen; i++){
	      aggExecs[grpByPos[i]]->update(currInTup[grpByPos[i]]); 
	    }    
	    currWindowHeight++;
	  } 

	  for(int i = 0; i < numAggs; i++){
	    aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
	  }

	  currInTup = inputIter->getNext();
	  numTuplesToBeDropped[nextDrop]++;
	}

	for(int i = 0; i < numFlds; i++){ 
		retTuple[i] = aggExecs[i]->getValue();
	}

	// if window is greater than num of tuples, 
	// set window height to number of tuples
	
	if (currWindowHeight < fullWindowHeight) {
	  fullWindowHeight = currWindowHeight;
	  nextDrop = 0;
	}

	firstTime = true;
	assert (currWindowHeight == fullWindowHeight); 
}

const Tuple* MovAggsExec::getNext(){

	// the first window has been calculated before a call to this function

	if (firstTime) {
	  firstTime = false;
	  return retTuple;
	}
  
	int toDeque = numTuplesToBeDropped[nextDrop];

	// no more tuples left in the stream
	if (!currInTup) { 

	  if (!currWindowHeight)
	    return NULL;

	  for(int i = 0; i < numAggs; i++){
	    aggExecs[aggPos[i]]->dequeue(toDeque);  
	  }

	  for (int i = 0; i < grpByPosLen; i++){
	    aggExecs[grpByPos[i]]->dequeue(1);// values in seqby Q are unique
	  }    

	  currWindowHeight--;
	  nextDrop = (nextDrop+1) % fullWindowHeight;
	  return retTuple;
	}

	// else update aggs with currInTup and read in tuples till next value
	for(int i = 0; i < numAggs; i++){
	  aggExecs[aggPos[i]]->dequeue(toDeque);  
	  aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
	}

	for (int i = 0; i < grpByPosLen; i++){
	  aggExecs[grpByPos[i]]->dequeue(1);
	  aggExecs[grpByPos[i]]->update(currInTup[grpByPos[i]]); 
	}    

	numTuplesToBeDropped[nextDrop] = 1; // for currInTup
	currInTup = inputIter->getNext();
	while(currInTup && !isNewGroup(currInTup)) { 
	  for(int i = 0; i < numAggs; i++){
	    aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
	  }
	  currInTup = inputIter->getNext();
	  numTuplesToBeDropped[nextDrop]++; 
	}
	nextDrop = (nextDrop+1) % fullWindowHeight;

	for(int i = 0; i < numFlds; i++){ 
		retTuple[i] = aggExecs[i]->getValue();
	}

	return retTuple;
}

void ExecMovMinMax::dequeue(int n){
	bool recalculate = false;
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		Type* boolVal;
		eqPtr(front[0], minMax, boolVal);
		if(boolVal){
			recalculate = true;
		}
		tupLoad->pop_front();
	}
	if(!tupLoad->empty() && recalculate){
		TupleLoader::iterator it = tupLoad->begin();
		ExecMinMax::initialize((*it)[0]);
		++it;
		while(!(it == tupLoad->end())){
			ExecMinMax::update((*it)[0]);
			++it;
		}
	}
}

void ExecMovSum::dequeue(int n){
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		subPtr(sum, front[0], sum);
		tupLoad->pop_front();
	}
}

void ExecMovAverage::dequeue(int n){
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		subPtr(sum, front[0], sum);
		count--;
		tupLoad->pop_front();
	}
}

