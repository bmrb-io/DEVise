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
	while(!selList->atEnd()){
		
		BaseSelection* curr = selList->get();

		Path * newPath;

		if(curr->isGlobal() && curr->getPath()->isFunction()){
			String* name = curr->getPath()->getPathName();
			isApplicableValue = true;
		}
		selList->step();
	}
	return isApplicableValue;
}

List<BaseSelection*>* Aggregates::getSelectList(){
	
	return selList;
}

List<BaseSelection*>* Aggregates::filterList(){
	
	
	List <BaseSelection*> *retVal = new List<BaseSelection*>();
	
	// For checking the match between selectList and the sequenceBy clause
	bool selectMatch = false;

	selList->rewind();
	while(!selList->atEnd()){
		
		Path * newPath;
		BaseSelection *curr = selList->get();
		if (sequenceAttr)
			if (curr->match(sequenceAttr,newPath) && newPath == NULL )
				selectMatch = true;
		
		if (curr->isGlobal() && curr->getPath()->isFunction()){
			
			// Need to check if our signature match..
		    List<BaseSelection*>* args = curr->getPath()->getArgs();

			if (checkAggFunc(args)){
				 // It is a function. So replace the list..
			    retVal->prepend(curr->getPath()->getSelectList());
				cout << " The select List prepended is " << 
					curr->getPath()->getSelectList()->toString() << endl;
			}
			else
		  		retVal->append(curr);
		}
		else if (!groupBy->isEmpty() ){
			
				THROW(new 
			Exception("Cannot use non-aggregating attributes with a groupBy clause"),0);
		}
		else
		  retVal->append(curr);

		// Get the next select list..
		selList->step();
	}		
	
	// The with clause is treated as a select condition and passed down.
	// Then it is treated as a where clause when getting the values.. 

	if (withPredicate){
		retVal->append(withPredicate);
	}
	groupBy->rewind();
	while(!groupBy->atEnd()){
		
		bool match = false;
		selList->rewind();
		while(!selList->atEnd()){
			Path *path;
			if (selList->get()->match(groupBy->get(),path)){
				match = true;
				break;
			}
			selList->step();
		}	
		if (!match){
			retVal->prepend(groupBy->get());
			selList->prepend(groupBy->get());
			cout << " Appended the groupBy attribute --- ";
			cout << groupBy->get()->toString() << endl;
		}
		groupBy->step();

	}
	if (selectMatch == false && sequenceAttr){
		selList->prepend(sequenceAttr);
		retVal->prepend(sequenceAttr);
	}	
	withPredicatePos = retVal->getPos(withPredicate);
	return retVal;
	
}

void Aggregates::typify(Site* inputIterator){
	
	iterator = inputIterator;

	Site::numFlds = selList->cardinality();
	
	// Create an array of what to do..
	funcPtr = new (GenFunction *)[numFlds];
	for(int i = 0; i < numFlds; i++)
		funcPtr[i] = NULL;
		
	String *  AttribNameList = getStringsFrom(inputIterator->getSelectList());
	TypeID * TypeIDList = inputIterator->getTypeIDs();
	int countFlds = inputIterator->getNumFlds();

	// Now get the position and type of the sequencing attribute..
	getSeqAttrType(AttribNameList,TypeIDList,countFlds);
	
	// Now do the work of finding all the functions and the window etc.
	// Counts the function ptr 
	int i = 0;
	positions = new int[groupBy->cardinality()];
	types = new TypeID[groupBy->cardinality()];
	selList->rewind();

	while(!selList->atEnd()){
		
		BaseSelection* curr = selList->get();
		if(curr->isGlobal() && curr->getPath()->isFunction()){
			
			List <BaseSelection *> *args = curr->getPath()->getArgs();
			if (!checkAggFunc(args))
				break;

			args->rewind();
			BaseSelection * selArgument = args->get();

			// Do a quick loop of the inner checking everything..
			int j;
			for(j = 0; j < countFlds ; j++){
			    int arg1,arg2;	
				Path *newPath;
				if (selArgument->toString() == AttribNameList[j]){
					
					//	Now that u have set the name..
					args->step();
					
					// For window size to be full
					int full = 0;
					if (!args->atEnd()){
			       		
						if (!sequenceAttr){
								String msg = " sequenceby clause missing ";
								THROW(new Exception(msg),);
						}
						arg1=((IInt*)(args->get()->evaluate(NULL,NULL)))->getValue();
						args->step();
			       		arg2=((IInt*)(args->get()->evaluate(NULL,NULL)))->getValue();
					
						if (arg1 > arg2){
							THROW(new Exception("Window hi lo values reversed"),);
						}
					}
					else 
						full = 1;
					// Set the function ptr to be called .
					TypeID ret = setFunctionPtr(funcPtr[i], 
					  *(curr->getPath()->getPathName()),j,TypeIDList[j],arg1,arg2,full);
					curr->setTypeID(ret);
					break;
				}			
			}	
		}
		selList->step();
		i++;
	}
	List<BaseSelection*>* selectList =  inputIterator->getSelectList();
	
	groupBy->rewind();
	while(!groupBy->atEnd()){
		
		bool match = false;
		selectList->rewind();
		while(!selectList->atEnd()){
			
			Path *path;
	    	if (selectList->get()->match(groupBy->get(),path)){
			
				positions[groupBy->getCurrPos()] = selectList->getCurrPos();
				types[groupBy->getCurrPos()] = TypeIDList[selectList->getCurrPos()];
				match = true;
			}
			selectList->step();
		}
		if (!match){
			THROW(new Exception(" Unable to find the groupBy attribute "),);
		}
		groupBy->step();
	}
	// Now all that is done ...
	// Form the Grouping class;;
	groupIterator = new Grouping(iterator,positions,types,groupBy->cardinality());
}

void Aggregates::getSeqAttrType(String * AttribNameList,TypeID *TypeIDList ,int countFlds)
{
	if (!sequenceAttr)	{
		fillNextFunc = new AggWindow(this,"",-1);
		return ;
	}

	seqAttrPos = 0;
	seqAttrType = "int";
	
	bool found = false;
	
	int i;
	for(i = 0; i < countFlds;i++){
		
		Path * newPath = NULL;		
		if (sequenceAttr->toString() == AttribNameList[i]){
			
			// Get the type ID for the ordering attribute
			seqAttrType = TypeIDList[i]; 
			found = true;
			break;
		}
		seqAttrPos ++;
	}
	
	// Panic: Seq attribute not found 
	if (!found )
		assert(0);

	// Need to figure out to do a fill for int (in case of
	// avgByPos, minByPos etc.. where a fill is necessary)
	
	fillNextFunc = new AggWindow(this,seqAttrType,seqAttrPos);
	
}
bool Aggregates::checkAggFunc(List<BaseSelection*>* args)
{
	args->rewind();

	if (args->atEnd() || (args->get()->selectID() != CONST_ID && 
		args->get()->selectID() != SELECT_ID))
		return false;
	args->step();
	if (args->atEnd())
		return true;
	if (args->atEnd() || args->get()->selectID() != CONST_ID )
		return false;
	args->step();
	if (args->atEnd() || args->get()->selectID() != CONST_ID)
		return false;

	return true;
}

TypeID &Aggregates::setFunctionPtr(GenFunction*&functionPtr,String funcName,int pos,TypeID type,int low,int high,int full) 
{
	
	fillNextFunc->updateWindow(low,high,full);
	functionPtr = new GenFunction(fillNextFunc,funcName,type,pos,low,high,full);
	return functionPtr->getTypeID();
}

Tuple* Aggregates::getNext()
{
	
	Tuple * retVal = new (Type*)[numFlds];
	fillNextFunc->fillWindow();
	
	if (fillNextFunc->getTupleAtCurrPos() == NULL)
		return NULL;
	
	int i;
	for(i = 0; i < numFlds;i++)
		if (funcPtr[i] != NULL )
			//Get the aggregated value and stuff it.
			retVal[i] = funcPtr[i]->computeAggregate();
		else
			//Get the actual value and stuff it.
			retVal[i] = fillNextFunc->getTupleAtCurrPos()[i];
	
	return retVal;

}

void AggWindow::fillWindow()
{

	// First fill the last of the list and then remove the unwanted stuff.
	if (windowFull){
		if (presentPos >=  0){
			aggregate->groupIterator->nextGroup();
			init();
			//TupleList->removeAll();
		}
		while((nextStoredTuple = aggregate->groupIterator->getNext()))
			TupleList->append(nextStoredTuple);
		presentPos = 0;
		return;
	}
		
	if (nextStoredTuple == NULL)
		nextStoredTuple = aggregate->groupIterator->getNext();
	
	if (!nextStoredTuple)
		state = FINAL;

	if (nextStoredTuple != NULL){
		
		TupleList->append(nextStoredTuple);
		bool match = true;
		
		while(match == true){
			
			Tuple *nextTuple = aggregate->groupIterator->getNext();
			if (nextTuple && equalAttr(nextTuple,nextStoredTuple))
				TupleList->append(nextTuple);
			else{
				match = false;
				nextStoredTuple = nextTuple;
			}
		}	
	}	
	
	// Now to get rid of unwanted stuff and adjust the position..
	// First adjust the position..
	
	if (state == INITIAL){
		
		presentPos = 0;
		// Read in as many tuples as possible till the window 
		// is satisfied....	
		state = MIDDLE;
		
		if (TupleList->setPos(0) == false){
			// Nothin much to do	
			state = FINAL;
			presentPos = -1;
			return;
		}
		int i;
		for(i = 0; i < windowHigh; i++){
			
			// The next tuple is in the  next..
			TupleList->append(nextStoredTuple);
			// Now get all the tuples that match next..
			bool match = true;

			while(match == true){
			
				Tuple *nextTuple = aggregate->groupIterator->getNext();
				if (nextTuple && equalAttr(nextTuple,nextStoredTuple)){
					TupleList->append(nextTuple);
				}
				else{
					match = false;
					nextStoredTuple = nextTuple;
					if (nextStoredTuple == NULL){
						state = FINAL;
						break;
					}
				}
			}	
			if (nextStoredTuple == NULL)
				break;
		}
	}
	else if (state == MIDDLE || state == FINAL){

		// Check for the window Low .. If it is fine then 
		// go ahead and remove stuff..
		
		// Shift the present position
		TupleList->rewind();
		
		if (TupleList->setPos(presentPos) == false){
			presentPos = -1;
			state = FINAL;
			return ;
		}
		Tuple * presentTuple = TupleList->getVal(presentPos);
		
		if (TupleList->setPos(presentPos+1) == false){
			presentPos = -1;
			state = FINAL;
			return;
		}
		valuesBeforePresent ++;
		presentPos ++;

		bool match = true;
		while(match == true){
			
			if (equalAttr(presentTuple,TupleList->get()))
				;
			else{
				match = false;
				break;
			}

			TupleList->step();
			presentPos ++;
			if (TupleList->atEnd()){
				match = false;
				presentPos = -1;
				state = FINAL;
				return;
			}
		}

		if (valuesBeforePresent > -windowLow){
			// Then remove one stuff
			valuesBeforePresent --;
			
			TupleList->rewind();
			Tuple * curr = TupleList->get();
			
			bool match = true;
			while(match == true){
				
				TupleList->remove();
				presentPos -- ;	

				Tuple * nextTuple = TupleList->get();
				
				if (nextTuple &&  equalAttr(nextTuple,curr))
					;
				else{
					match = false;
				}
			}
		}	
	}
}

Tuple *AggWindow::getTupleAtCurrPos(){	
	
	//if (windowFull && presentPos > 0 )
	//	return NULL;
	Tuple * next = TupleList->getVal(presentPos);
	if (!windowFull && !next){
		// This tells the iterator to go to the next group..
		aggregate->groupIterator->nextGroup();
		/*state = INITIAL;
		TupleList->removeAll();
		presentPos = -1;*/
		init();
		fillWindow();
		next = TupleList->getVal(presentPos);
	}
	return next;
}

int AggWindow::getLowPos(int windowPos,bool byPosition )
{
	
	if (windowFull)
		return 0;
	if (type != "int" && byPosition == true)
		return -1;

	TupleList->rewind();

	if (!byPosition){	
		
		// No problem.. Scan the list keeping track of the
		// number of position visited so far..
		// We know valuesBeforPresent holds the number of unique
		// values before current position..
		// So simply scan the counting the number of distince positions
		// as u go along and when the diff between urs and 
		// valuesBeforePresent  == required Window return;;
		
		Tuple *  curr = TupleList->get();
		TupleList->step();

		if (!curr)
			return -1;

		int numberUniqueVals = 1;
		while(numberUniqueVals - valuesBeforePresent <= windowPos ) {
		
			bool match = true;
			
			// Get rid of duplicates..
			while(match){
			
				Tuple * Next = TupleList->get();
				TupleList->step();	
				if(!Next)
					return -1;
				
				if(equalAttr(Next,curr)) 
					;
				else{
					match = false;
					curr = Next;
					numberUniqueVals ++;
				}
			}
		}
		if (numberUniqueVals - valuesBeforePresent > windowPos)
			return TupleList->getPos(curr);
		else
			return 0;
	}
	else{
		
		// It is by value...
		Tuple * presentTup = TupleList->getVal(presentPos);
		Tuple *  curr = TupleList->get();
		TupleList->step();

		if (!curr)
			return -1;
		
		while(compareAttr(presentTup,curr) < windowPos ){
			bool match = true;
			// Get rid of duplicates..
			Tuple * curr = TupleList->get();
			if(!curr )
				return -1;
			TupleList->step();	
		}
		return TupleList->getPos(curr);
	}
}


int AggWindow::getHighPos(int windowPos,bool byPosition )
{
	
	if (windowFull )
		return TupleList->cardinality()-1;
	if (type != "int" && byPosition == true)
		return -1;

	TupleList->rewind();

	if (!byPosition){	
		
		// No problem.. Scan the list keeping track of the
		// number of position visited so far..
		// We know valuesBeforPresent holds the number of unique
		// values before current position..
		// So simply scan the counting the number of distince positions
		// as u go along and when the diff between urs and 
		// valuesBeforePresent  == required Window return;;
		
		Tuple *  curr = TupleList->get();
		TupleList->step();

		if (!curr)
			return -1;

		int numberUniqueVals = 1;
		while(numberUniqueVals - valuesBeforePresent <= windowPos ) {
		
			bool match = true;
			// Get rid of duplicates..
			while(match){
			
				Tuple * Next = TupleList->get();
				TupleList->step();	
				if(!Next)
					return TupleList->cardinality()-1;
			
				if(equalAttr(Next,curr)) 
					;
				else{
					match = false;
					curr = Next;
					numberUniqueVals ++;
				}
			}
		}
		
		if (numberUniqueVals - valuesBeforePresent > windowPos){
			// Now get the highest tuple matching it..
			Tuple * Next = TupleList->get();
			TupleList->step();
			
			while(Next && equalAttr(Next,curr)){
				curr = Next;	
				Next = TupleList->get();
				TupleList->step();
			}
			return TupleList->getPos(curr);
		}
		else
			return TupleList->cardinality() -1;
	}
	else{
		
		// It is by value...
		Tuple * presentTup = TupleList->getVal(presentPos);

		Tuple *  curr = TupleList->get();
		
		TupleList->step();

		if (!curr)
			return -1;
		
		while(compareAttr(presentTup,curr) < windowPos){
		
			bool match = true;
			// Get rid of duplicates..
			Tuple * curr = TupleList->get();
			if(!curr )
				return -1;
			TupleList->step();	
		}
		
		Tuple * Next = TupleList->get();
		TupleList->step();
		while(Next && equalAttr(Next,curr)){
				curr = Next;	
				Next = TupleList->get();
				TupleList->step();
		}
		return TupleList->getPos(curr);

	}
}

// These functions are for comparison.
/*
int AggWindow::intCompAttr(Tuple * first,Tuple *second)
{
	
	return (((IInt *)first[position])->getValue() - 
		((IInt *)second[position])->getValue());
}

int AggWindow::doubleCompAttr(Tuple * first,Tuple *second)
{
	
	return (int)(((IDouble *)first[position])->getValue() - 
		((IDouble *)second[position])->getValue());
}
*/
// --- Now for the aggregate functions... ------

Type * GenFunction::avg()
{
	
	Tuple * next = scanNew();
	Type * sum = NULL;
	double count = 0;
	
	while(next != NULL){
		
		if (!sum)
			sum = next[pos];
		else
			sum = addPtr->opPtr(sum,next[pos]);
		
		count = count + 1;
		next = scanNext(); 
	}
		
	if (count != 0.0)
		return divPtr->opPtr(sum,& IDouble(count));
	else 
		return getNullValue(attribType);
}

Type * GenFunction::min()
{
	
	Tuple * next = scanNew();
	Type * minimum = 0;
	
	if (next)
		minimum =  next[pos];
	
	while(next != NULL){
		
		if (((IBool * )grtrPtr->opPtr(minimum,next[pos]))->getValue()){
			minimum = next[pos]; 
		}
		next = scanNext(); 
	}
	return minimum;

}

Type * GenFunction::max()
{
	
	Tuple * next = scanNew();
	Tuple * maximum = 0;
	if (next)
		maximum = (Tuple *)next[pos];

	while(next != NULL){
		
		if (((IBool *)lessPtr->opPtr(maximum ,next[pos]))->getValue())
			maximum = (Tuple *)next[pos];
		next = scanNext(); 
	}
	return maximum;

}

Type * GenFunction::count()
{
	
	Tuple * next = scanNew();
	int count = 0;

	while(next != NULL){
		
		count++;
		next = scanNext(); 
	}
	return new IInt(count);
}

Tuple * GenFunction::scanNew()
{
	
	currPos = filler->getLowPos(wLow,byPos);
	highPos = filler->getHighPos(wHigh,byPos);
	return scanNext();
}

Tuple * GenFunction::scanNext()
{
	
	if (currPos > highPos )
		return NULL;
	
	bool selected = false;	
	
	Tuple * curr = NULL;

	while(!selected){
		
		curr = filler->TupleList->getVal(currPos);
		
		if (!curr)
			return NULL;
		
		if (!filler->aggregate->withPredicate)
			selected = true;
		else 
			// Evaluate the boolean result..
		 if (((IBool *)curr[filler->aggregate->withPredicatePos])->getValue())
				selected = true;
		
		if (currPos <= highPos)
			currPos ++;
	}
	return curr;
}

void Grouping::sort(){
	
	lessPtrs = new (GeneralPtr *)[count];
	equalPtrs = new (GeneralPtr *)[count];
	int i;

	for(i = 0;i < count;i++){
		TypeID retVal;	
		TRY(lessPtrs[i] = getOperatorPtr("<",types[i],types[i],retVal),);
		TRY(equalPtrs[i] = getOperatorPtr("=",types[i],types[i],retVal),);
	}
	
	for(i = 0; i < TupleList->cardinality(); i++){
		
		for(int j = i+1; j < TupleList->cardinality(); j++){
			Tuple * left = TupleList->getVal(i);
			Tuple * right = TupleList->getVal(j);
			assert(left);
			assert(right);
			if (tupleCompare(positions,lessPtrs,equalPtrs,count,left,right)>0){
				
				assert(TupleList->setPos(i));
				TupleList->replace(right);
				assert(TupleList->setPos(j));
				TupleList->replace(left);
			}
				
		}
	}
}


int Grouping::tupleCompare(int *positions,GeneralPtr **lessPtr,GeneralPtr ** equalPtr,int count,Tuple * left,Tuple * right)
{
	assert(right);
	assert(left);
	int i;
	for(i = 0; i < count; i++){
		
		if (((IBool*)lessPtr[i]->opPtr(left[positions[i]],right[positions[i]]))->getValue())
			return -1;
		else if (((IBool*)
		equalPtr[i]->opPtr(left[positions[i]],right[positions[i]]))->getValue() == false)
			return 1;	
	}
	return 0;

}

Tuple *Grouping::getNext(){
	
	if (!count)
		return iterator->getNext();

	if (TupleList->isEmpty()){
		
		while((next = iterator->getNext()))
			TupleList->append(next);
	
		//int count = TupleList->cardinality();
		//TupleArray = new Tuple[count];
		sort();;
		TupleList->rewind();
		next = TupleList->get();
		TupleList->remove();
		state = NORMAL;
		return next;
	}
	else{
		
		if (state == GROUPEND)
			return NULL;
		
		Tuple * nextInList = TupleList->get();
		if (tupleCompare(positions,lessPtrs,equalPtrs,count,next,nextInList)== 0){
			TupleList->remove();
			return nextInList;
		}
		else{
			state = GROUPEND;
			next = nextInList;
			return NULL;
		}	
	}
}

void Grouping::nextGroup(){
	if (state == GROUPEND)
		state = NORMAL;
}
