#include "Aggregates.h"
#include "MemoryMgr.h"
#include "SeqSimVecAggregate.h"
bool Aggregates::isApplicable(){
	return isApplicableValue;
}

void Aggregates::typeCheck(TypeCheck& typeCheck){
	
	// Are there any aggregates in the selectClause?

	assert(!filteredSelList);

	isApplicableValue = false;

	vector<BaseSelection*>::iterator it;
	filteredSelList = new List<BaseSelection*>();

	if(!binBy.empty()){
		assert(groupBy.empty());
		assert(sequenceBy.empty());
		grpByPos = new int[1];
		int i = 0;
		for(it = selList.begin(); it != selList.end(); ++it){
		  	if((*it)->match(binBy.front())){
				grpByPos[0] = i;
				vector<BaseSelection*>::iterator binIt;
				binIt = binBy.begin();
				++binIt;
				assert((*binIt)->selectID() == CONST_ID);
				ConstantSelection* init = (ConstantSelection*) *binIt;
				++binIt;
				assert((*binIt)->selectID() == CONST_ID);
				ConstantSelection* step = (ConstantSelection*) *binIt;
				aggFuncs[i] = new BinAttribute(init, step);
			}
			i++;
		}
		numGrpByFlds = 1;	// to avoid an assertion fail
	}

	if(!groupBy.empty()){
	  numGrpByFlds = groupBy.size();
	  grpByPos = new int[numGrpByFlds];
	}
	
	if (!sequenceBy.empty()) {
	  numSeqByFlds = sequenceBy.size();
	  seqByPos = new int[numSeqByFlds];
	}

	numAggs = numFlds - numGrpByFlds - numSeqByFlds;
	assert (numAggs >= 0);
	aggPos = new int[numAggs];

	bool hasMoving = false; //to check moving and normal aggs are not mixed
	int i = 0;
	numGrpByFlds = numSeqByFlds = numAggs = 0; // use them as counters
	for(it = selList.begin(); it != selList.end(); ++it){
		
		BaseSelection* curr = *it;
		bool resolved = false;

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
			  curr = typeCheck.resolve(curr);
			  args->replace(curr);
			  if (*name == "min"){
			    aggFuncs[i] = new MinAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "max"){
			    aggFuncs[i] = new MaxAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "avg"){
			    aggFuncs[i] = new AvgAggregate();
			    function->setTypeID(DOUBLE_TP);
			    resolved = true;
			  }
			  else if (*name == "sum"){
			    aggFuncs[i] = new SumAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "count"){
			    aggFuncs[i] = new CountAggregate();
			    function->setTypeID(INT_TP);
			    resolved = true;
			  }
			}
			else if (numArgs == 3) {
			  // moving aggregates
			  isApplicableValue = true;
			  hasMoving = true;			  
			  ConstantSelection *windowArg;

			  curr = args->get();
			  curr = typeCheck.resolve(curr);
			  args->replace(curr);
			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowLow = (int) windowArg->getValue();

			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowHigh = (int)windowArg->getValue();

			  assert(windowHigh > windowLow);

			  if (*name == "min"){
			    aggFuncs[i] = new MovMinAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "max"){
			    aggFuncs[i] = new MovMaxAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "avg"){
			    aggFuncs[i] = new MovAvgAggregate();
			    function->setTypeID(DOUBLE_TP);
			    resolved = true;
			  }
			  else if (*name == "sum"){
			    aggFuncs[i] = new MovSumAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "count"){
			    aggFuncs[i] = new MovCountAggregate();
			    function->setTypeID(INT_TP);
			    resolved = true;
			  }
			  else if (*name == "sv")
			  {
			    aggFuncs[i] = new MovSeqSimVecAggregate(windowHigh);
			    function->setTypeID(SEQSV_TP);
			    resolved = true;
				sim_query = true;
			  }
			}
			aggPos[numAggs++] = i;
		}
		else { 
		  // must be a group by or sequence by

		  if(!groupBy.empty()){
		    vector<BaseSelection*>::iterator it;
		    for(it = groupBy.begin(); it != groupBy.end(); ++it){
		      if (curr->match(*it)){
			isApplicableValue = true;
			assert(numGrpByFlds < groupBy.size());
			grpByPos[numGrpByFlds++] = i;
			aggFuncs[i] = new GroupAttribute();
			break;
		      }
		    }
		  }

		  if (!sequenceBy.empty()) { 
		  	vector<BaseSelection*>::iterator it;
			for(it = sequenceBy.begin(); it != sequenceBy.end(); ++it){
		      if (curr->match(*it)){
			isApplicableValue = true;
			assert(numSeqByFlds < sequenceBy.size());
			seqByPos[numSeqByFlds++] = i;
			aggFuncs[i] = new SequenceAttribute();  
			break;
		      }
		    }
		  }

		}
		if(!resolved){
			curr = typeCheck.resolve(curr);
			*it = curr;
		}
		filteredSelList->append(curr);
		i++;
	}

	if (isApplicableValue) {
//	  assert( numAggs + numSeqByFlds + numGrpByFlds == numFlds);
	}
}

void Aggregates::typify(const string&, Site* inputPlanOp){
	this->inputPlanOp = inputPlanOp;
	int numFlds = inputPlanOp->getNumFlds();
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

	if(!binBy.empty()){
		return new StandGroupByExec(inputIter, aggExecs, numFlds,
			grpByPos, 1, aggPos, numAggs);
	}

	if(numGrpByFlds > 0){
	  if (numSeqByFlds > 0) {
	    // group by and seq by
		if (sim_query)
			return new MovSeqGroupByExec(inputIter, aggExecs, numFlds,
				seqByPos, numSeqByFlds, grpByPos, numGrpByFlds, 
					  aggPos, numAggs,windowLow, windowHigh);
		else
			return new MovGroupByExec(inputIter, aggExecs, numFlds,
				seqByPos, numSeqByFlds, grpByPos, numGrpByFlds, 
					  aggPos, numAggs,windowLow, windowHigh);
	  }
	  else {
	    // just group by
		return new StandGroupByExec(inputIter, aggExecs, numFlds,
			grpByPos, numGrpByFlds, aggPos, numAggs);
	  }
	}
	if(numSeqByFlds > 0){
	  // just seq by
		if (sim_query)
			return new MovSeqAggsExec(inputIter, aggExecs, numFlds, seqByPos,
				numSeqByFlds, aggPos, numAggs, windowLow, windowHigh);
		else
			return new MovAggsExec(inputIter, aggExecs, numFlds, seqByPos,
				numSeqByFlds, aggPos, numAggs, windowLow, windowHigh);
	}
	else {
		// No sequence by or group by 
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
/*
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
	    for (int i = 0; i < seqByPosLen; i++){
	      aggExecs[seqByPos[i]]->update(currInTup[seqByPos[i]]); 
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
	}	*/
void MovAggsExec::initialize(){
  assert(inputIter);
  inputIter->initialize();
  currInTup = inputIter->getNext();
  
  if(!currInTup){
    return;
  }
  setupFirst();
}	

void MovAggsExec::setupFirst() {
  int i;

  for(i = 0; i < numFlds; i++){
    aggExecs[i]->initialize(currInTup[i]);
  }
  
  currWindowHeight = 1;
  fullWindowHeight = windowHigh - windowLow;
  nextDrop = 0;
  numTuplesToBeDropped[nextDrop] = 1; // for the first tuple read
  
  for (i = 1; i < fullWindowHeight; i++){
    numTuplesToBeDropped[i] = 0;   
  }

  currInTup = inputIter->getNext();
  endOfGroup = true;
  while(currInTupIsValid()) { 
    
    if (isNewSeqVal(currInTup)) {
      nextDrop = (nextDrop+1) % fullWindowHeight; 
      
      if (currWindowHeight == fullWindowHeight) {
	endOfGroup = false;
	break; // end of window
      }
      
      // Add the currInTuple to end of the seq by queue
      for (i = 0; i < seqByPosLen; i++){
	aggExecs[seqByPos[i]]->update(currInTup[seqByPos[i]]); 
      }    
      currWindowHeight++;
    }
    
    for(i = 0; i < numAggs; i++){
      aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
    }
    
    currInTup = inputIter->getNext();
    numTuplesToBeDropped[nextDrop]++;
  }

  for(i = 0; i < numFlds; i++){ 
    retTuple[i] = aggExecs[i]->getValue();
  }
  
  // if window is greater than num of tuples, 
  // set window height to number of tuples
  
  if (fullWindowHeight > currWindowHeight) {
    fullWindowHeight = currWindowHeight;
    nextDrop = 0;
  }
  
  firstTime = true;
  assert (currWindowHeight == fullWindowHeight); 
}

const Tuple* MovAggsExec::flushWindow(){
  int i;

  if (!currWindowHeight)
    return NULL;
  
  for(i = 0; i < numAggs; i++){
    aggExecs[aggPos[i]]->dequeue(toDeque);  
  }
  
  for (i = 0; i < seqByPosLen; i++){
    aggExecs[seqByPos[i]]->dequeue(1); // values in seqby Q are unique
  }    
  
  nextDrop = (nextDrop+1) % fullWindowHeight;
  
  for(i = 0; i < numFlds; i++){ 
    retTuple[i] = aggExecs[i]->getValue();
  }
  
  return retTuple;
}

const Tuple* MovSeqAggsExec::flushWindow(){
	return NULL;
}

const Tuple* MovAggsExec::getNext(){
	int i;

	// the first window has been calculated before a call to this function

	if (firstTime) {
	  firstTime = false;
	  return retTuple;
	}
  
	toDeque = numTuplesToBeDropped[nextDrop];

	// flushes the rest of the window if no more tuples left in the input
	if (endOfGroup) {
	  currWindowHeight--;
	  return flushWindow(); 
	}

	// else update aggs with currInTup and read in tuples till next value
	for(i = 0; i < numAggs; i++){
	  aggExecs[aggPos[i]]->dequeue(toDeque);  
	  aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
	}

	for (i = 0; i < seqByPosLen; i++){
	  aggExecs[seqByPos[i]]->dequeue(1);
	  aggExecs[seqByPos[i]]->update(currInTup[seqByPos[i]]); 
	}    

	numTuplesToBeDropped[nextDrop] = 1; // for currInTup
	currInTup = inputIter->getNext();
	endOfGroup = true;

	while(currInTupIsValid()) {
	  if (isNewSeqVal(currInTup)) {
	    endOfGroup = false;
	    break;
	  } 
	  for(i = 0; i < numAggs; i++){
	    aggExecs[aggPos[i]]->update(currInTup[aggPos[i]]);
	  }
	  currInTup = inputIter->getNext();
	  numTuplesToBeDropped[nextDrop]++; 
	}

	nextDrop = (nextDrop+1) % fullWindowHeight;

	for(i = 0; i < numFlds; i++){ 
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

const Tuple* MovGroupByExec::flushWindow(){

	int i;

  if (!currWindowHeight) { // window has been flushed
    
    if (!currInTup) {
      return NULL; // end of file
    }

    for(i = 0; i < numAggs; i++){
      aggExecs[aggPos[i]]->dequeue(toDeque);  
    }
    for (i = 0; i < seqByPosLen; i++){
      aggExecs[seqByPos[i]]->dequeue(1);
    }    

    setupFirst();
    firstTime = false;
    return retTuple; 
  }

  for(i = 0; i < numAggs; i++){
    aggExecs[aggPos[i]]->dequeue(toDeque);  
  }
  for (i = 0; i < seqByPosLen; i++){
    aggExecs[seqByPos[i]]->dequeue(1); // values in seqby Q are unique
  }    

  nextDrop = (nextDrop+1) % fullWindowHeight;
  
  for(i = 0; i < numFlds; i++){ 
    retTuple[i] = aggExecs[i]->getValue();
  }

  return retTuple;
}

const Tuple* MovSeqGroupByExec::flushWindow(){
	int i;
  while (currWindowHeight) { // window has been flushed
  	for(i = 0; i < numAggs; i++){
   	 aggExecs[aggPos[i]]->dequeue(toDeque);  
  	}
  	for (i = 0; i < seqByPosLen; i++){
   	 aggExecs[seqByPos[i]]->dequeue(1); // values in seqby Q are unique
  	}    

  	nextDrop = (nextDrop+1) % fullWindowHeight;
	currWindowHeight--;
   }
  
    if (!currInTup) {
      return NULL; // end of file
    }

    for(i = 0; i < numAggs; i++){
      aggExecs[aggPos[i]]->dequeue(toDeque);  
    }
    for (i = 0; i < seqByPosLen; i++){
      aggExecs[seqByPos[i]]->dequeue(1);
    }    

    setupFirst();
    firstTime = false;
    return retTuple; 

}
