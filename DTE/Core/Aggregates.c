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
			const String* name = curr->getPath()->getPathName();
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
			    retVal->append(curr->getPath()->getSelectList());
				cout << " The select List prepended is " << 
					curr->getPath()->getSelectList()->toString() << endl;
			}
			else
		  		retVal->append(curr);
		}
		/*else if (!groupBy->isEmpty() ){
			
				THROW(new 
			Exception("Cannot use non-aggregating attributes with a groupBy clause"),0);
		}*/
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
	if (selectMatch == false && sequenceAttr){
		if (!groupBy->isEmpty()){
			groupBy->append(sequenceAttr);
		}
		else{
			selList->append(sequenceAttr);
			retVal->append(sequenceAttr);
		}
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
			retVal->append(groupBy->get());
			selList->append(groupBy->get());
			cout << " Appended the groupBy attribute --- ";
			cout << groupBy->get()->toString() << endl;
		}
		groupBy->step();

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
	List<BaseSelection*>*selectList = inputIterator->getSelectList();
	TypeID * TypeIDList = inputIterator->getTypeIDs();
	int countFlds = inputIterator->getNumFlds();
	for(int i = 0; i < countFlds;i++)
		cout << " Pos " << i << " " << AttribNameList[i] << endl; 
	
	// Now get the position and type of the sequencing attribute..
	getSeqAttrType(AttribNameList,TypeIDList,countFlds);
	
	// Now do the work of finding all the functions and the window etc.
	// Counts the function ptr 
	int i = 0;
	positions = new int[groupBy->cardinality()];
	taboo = new int[groupBy->cardinality()];
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
						arg1= int(args->get()->evaluate(NULL,NULL));
						args->step();
			       		arg2= int(args->get()->evaluate(NULL,NULL));
					
						if (arg1 > arg2){
							THROW(new Exception("Window hi lo values reversed"),);
						}
					}
					else{ 
						// This is relational average, no high low
						full = 1;
					}
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
	
	groupBy->rewind();
	while(!groupBy->atEnd()){
		
		bool match = false;
		selectList->rewind();
		while(!selectList->atEnd()){
			
			Path *path;
			if (selectList->get()->matchFlat(groupBy->get(),path)){
				if (sequenceAttr && 
					selectList->get()->match(sequenceAttr,path)){
					taboo[groupBy->getCurrPos()] =1; 
				}
				else{
					taboo[groupBy->getCurrPos()] = 0; 
				}
						
				positions[groupBy->getCurrPos()] = 
					selectList->getCurrPos();
				types[groupBy->getCurrPos()] = 
					TypeIDList[selectList->getCurrPos()];
				match = true;
			}
			selectList->step();
		}
		if (!match){
			ostrstream tmp;
			tmp << "Selection list (";
			displayList(tmp, selectList);
			tmp << ") does not have GROUP BY attribute ";
			groupBy->get()->display(tmp);
			tmp << ends;
			char* tmpc = tmp.str();
			String msg(tmpc);
			delete tmpc;
			THROW(new Exception(msg),);
		}
		groupBy->step();
	}
	
	// Now all that is done ...
	// Form the Grouping class;;
	groupIterator = new Grouping(
		iterator,positions,taboo,types,seqAttrPos,groupBy->cardinality());
}

void Aggregates::getSeqAttrType(
	String * AttribNameList,TypeID *TypeIDList ,int countFlds)
{
	if (!sequenceAttr)	{
		fillNextFunc = new AggWindow(this,"",-1);
		seqAttrPos = -1;
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

bool Aggregates::getNext(Tuple* retVal)
{
	
	fillNextFunc->fillWindow();
	
	if (fillNextFunc->getTupleAtCurrPos() == NULL)
		return false;
	
	int i;
	for(i = 0; i < numFlds;i++)
		if (funcPtr[i] != NULL )
			//Get the aggregated value and stuff it.
			retVal[i] = funcPtr[i]->computeAggregate();
		else
			//Get the actual value and stuff it.
			retVal[i] = fillNextFunc->getTupleAtCurrPos()[i];
	return true;

}

void AggWindow::fillWindow()
{

	// First fill the last of the list and then remove the unwanted stuff.
	if (windowFull){

		// windowFull is set during typechecking and never changed!
		// relatinonal aggregates have windowFull = true always!

		if (presentPos >=  0){
			aggregate->groupIterator->nextGroup();
			init();
		}
		while((nextStoredTuple = aggregate->groupIterator->getNext()))
			TupleList.add_high(nextStoredTuple);
		presentPos = 0;
		return;
	}
		
	if (nextStoredTuple == NULL)
		nextStoredTuple = aggregate->groupIterator->getNext();
	
	if (!nextStoredTuple)
		state = FINAL;

	if (nextStoredTuple != NULL){
		
		TupleList.add_high(nextStoredTuple);
		bool match = true;
		
		while(match == true){
			
			Tuple *nextTuple = aggregate->groupIterator->getNext();
			if (nextTuple && equalAttr(nextTuple,nextStoredTuple))
				TupleList.add_high(nextTuple);
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
		
		if (TupleList.empty()){
			// Nothin much to do	
			state = FINAL;
			presentPos = -1;
			return;
		}
		int i;
		for(i = 0; i < windowHigh; i++){
			
			// The next tuple is in the  next..
			if (!nextStoredTuple)
				nextStoredTuple = aggregate->groupIterator->getNext();
			
			if (!nextStoredTuple)
				return;

			TupleList.add_high(nextStoredTuple);

			// Now get all the tuples that match next..
			bool match = true;

			while(match == true){
			
				Tuple *nextTuple = aggregate->groupIterator->getNext();
				if (nextTuple && equalAttr(nextTuple,nextStoredTuple)){
					TupleList.add_high(nextTuple);
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
		
		if (!TupleList.valid(presentPos)){
			presentPos = -1;
			state = FINAL;
			return ;
		}
		assert(TupleList.valid(presentPos));
		Tuple * presentTuple = TupleList[presentPos];
		
		if (!TupleList.valid(presentPos+1)){
			presentPos = -1;
			state = FINAL;
			return;
		}
		valuesBeforePresent ++;
		presentPos ++;

		bool match = true;
		while(match == true){
			
			assert(TupleList.valid(presentPos));
			if (equalAttr(presentTuple,TupleList[presentPos]))
				;
			else{
				match = false;
				break;
			}

			presentPos ++;
			if (!TupleList.valid(presentPos)){
				match = false;
				presentPos = -1;
				state = FINAL;
				return;
			}
		}

		if (valuesBeforePresent > -windowLow){
			// Then remove one stuff
			valuesBeforePresent --;
			
			Tuple* curr = NULL;
			if(!TupleList.empty()){
				curr = TupleList.low_element();
			}
			else{
				return;
			}
			
			bool match = true;
			while(match == true){
				
				TupleList.del_low();
				TupleList.reset_low(0);

				presentPos -- ;	

				Tuple* nextTuple;
				if(!TupleList.empty()){
					nextTuple = TupleList.low_element();
					if(!equalAttr(nextTuple,curr)){
						match = false;
					}
				}
				else{
					match = false;
				}
			}
		}	
	}
}

Tuple *AggWindow::getTupleAtCurrPos(){	
	
	Tuple* next = NULL;
	if(TupleList.valid(presentPos)){
		next = TupleList[presentPos];
	}

	if (!windowFull && !next){
		// This tells the iterator to go to the next group..
		aggregate->groupIterator->nextGroup();
		init();
		fillWindow();
		if(!TupleList.valid(presentPos)){
			next = NULL;
		}
		else {
			next = TupleList[presentPos];
		}
	}
	return next;
}

int AggWindow::getLowPos(int windowPos,bool byPosition )
{
	
	if (windowFull)
		return 0;
	if (type != "int" && byPosition == true)
		return -1;

	int tupleCounter = TupleList.low();

	if (!byPosition){	
		
		// No problem.. Scan the list keeping track of the
		// number of position visited so far..
		// We know valuesBeforPresent holds the number of unique
		// values before current position..
		// So simply scan the counting the number of distince positions
		// as u go along and when the diff between urs and 
		// valuesBeforePresent  == required Window return;;
		
		if(!TupleList.valid(tupleCounter)){
			return -1;
		}
		Tuple *  curr = TupleList[tupleCounter++];
		assert(curr);

		int numberUniqueVals = 1;
		while(numberUniqueVals - valuesBeforePresent <= windowPos ) {
		
			bool match = true;
			
			// Get rid of duplicates..
			while(match){
			
				if(!TupleList.valid(tupleCounter)){
					return - 1;
				}
				Tuple * Next = TupleList[tupleCounter++];
				assert(Next);

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
			int i = TupleList.low();
			for(; i <= TupleList.high(); i++){
				if(!TupleList.valid(i)){
					cout << "i = " << i << " is invalid\n";
					cout << "low = " << TupleList.low() 
						<< " high = " << TupleList.high() << endl;
				}
				assert(TupleList.valid(i));
				if(TupleList[i] == curr){
					return i;
				}
			}
			return -1;
		}
		else
			return 0;
	}
	else{
		
		// It is by value...

		Tuple* presentTup = NULL;
		if(TupleList.valid(presentPos)){
			presentTup = TupleList[presentPos];
		}

		if(!TupleList.valid(tupleCounter)){
			return -1;
		}
		Tuple* curr = TupleList[tupleCounter++];

		while(compareAttr(presentTup,curr) < windowPos ){
			bool match = true;

			// Get rid of duplicates..

			if(!TupleList.valid(tupleCounter)){
				return -1;
			}
			Tuple* curr = TupleList[tupleCounter++];
		}
		int i = TupleList.low();
		for(; i <= TupleList.high(); i++){
			assert(TupleList.valid(i));
			if(TupleList[i] == curr){
				return i;
			}
		}
		return -1;
	}
}


int AggWindow::getHighPos(int windowPos,bool byPosition )
{
	
	if (windowFull ){
		return TupleList.high();
	}
	if (type != "int" && byPosition == true)
		return -1;

	int currentTuple = 1;

	if (!byPosition){	
		
		// No problem.. Scan the list keeping track of the
		// number of position visited so far..
		// We know valuesBeforPresent holds the number of unique
		// values before current position..
		// So simply scan the counting the number of distince positions
		// as u go along and when the diff between urs and 
		// valuesBeforePresent  == required Window return;;
		
		if(TupleList.empty()){
			return -1;
		}
		Tuple* curr = TupleList.low_element();
		assert(curr);

		int numberUniqueVals = 1;
		while(numberUniqueVals - valuesBeforePresent <= windowPos ) {
		
			bool match = true;
			// Get rid of duplicates..
			while(match){
			
				if(!TupleList.valid(currentTuple)){
					return TupleList.high();
				}
				Tuple* Next = TupleList[currentTuple++];

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

			bool isValid = TupleList.valid(currentTuple);
			Tuple* Next = NULL;
			if(isValid){
				Next = TupleList[currentTuple++];
			}
			
			while(isValid && equalAttr(Next,curr)){
				curr = Next;	
				isValid = TupleList.valid(currentTuple);
				if(isValid){
					Next = TupleList[currentTuple++];
				}
			}
			int i = TupleList.low();
			for(; i <= TupleList.high(); i++){
				assert(TupleList.valid(i));
				if(TupleList[i] == curr){
					return i;
				}
			}
			return -1;
		}
		else
			return TupleList.high();
	}
	else{
		
		// It is by value...
		Tuple* presentTup = NULL;
		if(TupleList.valid(presentPos)){
			presentTup = TupleList[presentPos];
		}

		if(!TupleList.valid(currentTuple)){
			return -1;
		}
		Tuple *  curr = TupleList[currentTuple++];
		
		while(compareAttr(presentTup,curr) < windowPos){
		
			bool match = true;
			// Get rid of duplicates..
			if(!TupleList.valid(currentTuple)){
				return -1;
			}
			Tuple *  curr = TupleList[currentTuple++];
		}
		
		bool isValid = TupleList.valid(currentTuple);
		Tuple* Next = NULL;
		if(isValid){
			Next = TupleList[currentTuple++];
		}
		
		while(isValid && equalAttr(Next,curr)){
			curr = Next;	
			isValid = TupleList.valid(currentTuple);
			if(isValid){
				Next = TupleList[currentTuple++];
			}
		}
		int i = TupleList.low();
		for(; i <= TupleList.high(); i++){
			assert(TupleList.valid(i));
			if(TupleList[i] == curr){
				return i;
			}
		}
		return -1;
	}
}

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
		
		if (grtrPtr->opPtr(minimum,next[pos])){
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
		
		if (lessPtr->opPtr(maximum ,next[pos]))
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
	return (Type*) count;
}

Type * GenFunction::sum()
{
	
	Tuple* next = scanNew();

	Type* currentSum = NULL;

	if (next){
		currentSum = next[pos];
	}

	next = scanNext(); 
	while(next != NULL){
		currentSum = addPtr->opPtr(currentSum, next[pos]);
		next = scanNext(); 
	}

	return currentSum;
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
		
		if(!filler->TupleList.valid(currPos)){
			return NULL;
		}
		curr = filler->TupleList[currPos];
		
		if (!filler->aggregate->withPredicate)
			selected = true;
		else 
			// Evaluate the boolean result..
		 if (curr[filler->aggregate->withPredicatePos])
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
	i = TupleList.low();
	for(; i <= TupleList.high(); i++){
		
		for(int j = i+1; j <= TupleList.high(); j++){
			Tuple * left = TupleList[i];
			Tuple * right = TupleList[j];
			assert(left);
			assert(right);
			if (tupleCompare(positions,lessPtrs,equalPtrs,count,left,right)>0){
				
				assert(TupleList.valid(i));
				TupleList[i] = right;
				assert(TupleList.valid(j));
				TupleList[j] = left;
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
		
		if (positions[i] < 0 ){
			continue;
		}
		if (lessPtr[i]->opPtr(left[positions[i]],right[positions[i]]))
			return -1;
		else if (! equalPtr[i]->opPtr(left[positions[i]],right[positions[i]]))
			return 1;	
	}
	return 0;

}

Tuple* Grouping::getNext(){

	int inputNumFlds = iterator->getNumFlds();
	
	if (!count){
		Tuple* nextTuple = new Tuple[inputNumFlds];
		if(iterator->getNext(nextTuple)){
			return nextTuple;
		}
		else{
			return NULL;
		}
	}

	if (TupleList.empty()){
		next = new Tuple[inputNumFlds];	
		while((iterator->getNext(next))){
			TupleList.add_high(next);
			next = new Tuple[inputNumFlds];	
		}
		next = NULL;
		
		if (TupleList.empty()){
			next = NULL;
			state = NORMAL;
			return NULL;
		}
		sort();
		if (seqAttrPos >= 0){
			int i;
			for(i = 0; i < count; i++)
				if (taboo[i])
					positions[i] = -1;
		}

		next = TupleList.low_element();
		TupleList.del_low();
		TupleList.reset_low(0);
		state = NORMAL;
		return next;
	}
	else{
		
		if (state == GROUPEND)
			return NULL;
		
		Tuple* nextInList = NULL;
		if(!TupleList.empty()){
			nextInList = TupleList.low_element();
		}
		if (tupleCompare(positions,lessPtrs,equalPtrs,count,next,nextInList)== 0){
			TupleList.del_low();
			TupleList.reset_low(0);
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
