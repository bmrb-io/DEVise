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
		
		grpByFuncs[i] = new GroupAttribute();
		positions[i] = j; // position in the select list
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

	if (numGrpByFlds >0){
	  assert(numGrpByFlds == groupBy->cardinality());
	  for (int i = 0; i < numGrpByFlds; i++)
	    grpByFuncs[i]->typify(inptypes[positions[i]]);
	}
}

Iterator* Aggregates::createExec(){
	assert(inputPlanOp);
	Iterator* inputIter = inputPlanOp->createExec();
	ExecAggregate** aggExecs = new (ExecAggregate*)[numFlds];
	for(int i = 0; i < numFlds; i++){
		aggExecs[i] = aggFuncs[i]->createExec();
	}
	
        if(numGrpByFlds > 0){
	  ExecGroupAttr** grpByExecs = new (ExecGroupAttr*)[numGrpByFlds];
	  for(int i = 0; i < numGrpByFlds; i++){
	    grpByExecs[i] = grpByFuncs[i]->createExec();
	  }		
	  return new StandGroupByExec(inputIter, aggExecs, grpByExecs, numFlds,
				      positions, numGrpByFlds);
	}
	// No group by 
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

void StandGroupByExec::initialize(){
	assert(inputIter);
	inputIter->initialize();
	isFirstGroup = true;
}

const Tuple* StandGroupByExec::getNext(){

  const Tuple* currt = inputIter->getNext();
  if(!currt){
    if (isFirstGroup)
      return NULL;
    else{ // the last tuple forms a group of its own
      for(int i = 0; i < numFlds; i++)
	retTuple[i] = aggExecs[i]->getValue();
      return retTuple;
    }
  }
  
  if (isFirstGroup){
    isFirstGroup = false;
    for(int i = 0; i < numFlds; i++)
      aggExecs[i]->initialize(currt[i]);

    for (int i = 0; i < grpByPosLen; i++)
      grpByExecs[i]->initialize(currt[grpByPos[i]]);
    
    while((currt = inputIter->getNext())){
      bool isdiff = false;
      for (int i = 0; i < grpByPosLen && !isdiff; i++){
	isdiff = isdiff || grpByExecs[i]->isdifferent(currt[grpByPos[i]]);
      }
      if (isdiff) // new group is encountered
	break;

      for(int i = 0; i < numFlds; i++){
	aggExecs[i]->update(currt[i]);
      }
    }
    
    if (currt){ // New group needs to be formed
      // Reset the values for aggExecs and grpByExecs with new tuple
      for(int i = 0; i < numFlds; i++)
	aggExecs[i]->initialize(currt[i]);

      for (int i = 0; i < grpByPosLen; i++)
	grpByExecs[i]->initialize(currt[grpByPos[i]]);
    }
    
    for(int i = 0; i < numFlds; i++)
      retTuple[i] = aggExecs[i]->getValue();
    
    return retTuple;
  }

  // Is not the first group
  // Check if the new tuple differs from the first tuple of group
  bool isdiff = false;
  for (int i = 0; i < grpByPosLen && !isdiff; i++){
    isdiff = isdiff || grpByExecs[i]->isdifferent(currt[grpByPos[i]]);
  }

  if (isdiff){ // Prev tuple forms a group of its own
    for(int i = 0; i < numFlds; i++)
      aggExecs[i]->initialize(currt[i]);

    for (int i = 0; i < grpByPosLen; i++)
      grpByExecs[i]->initialize(currt[grpByPos[i]]);
    
    for(int i = 0; i < numFlds; i++)
      retTuple[i] = aggExecs[i]->getValue();
    
    return retTuple;
  }
  
  // Else New tuple is added to the group
  while((currt = inputIter->getNext())){
    bool isdiff = false;
    for (int i = 0; i < grpByPosLen && !isdiff; i++){
      isdiff = isdiff || grpByExecs[i]->isdifferent(currt[grpByPos[i]]);
    } 
    if (isdiff) // new group is encountered
      break;
    
    for(int i = 0; i < numFlds; i++){
      aggExecs[i]->update(currt[i]);
    }
  }
    
  if (currt){ // New group needs to be formed
    // Reset the values for aggExecs and grpByExecs with new tuple
    for(int i = 0; i < numFlds; i++)
      aggExecs[i]->initialize(currt[i]);
    
    for (int i = 0; i < grpByPosLen; i++)
      grpByExecs[i]->initialize(currt[grpByPos[i]]);
  }
    
  for(int i = 0; i < numFlds; i++)
    retTuple[i] = aggExecs[i]->getValue();
  
  return retTuple;
}

