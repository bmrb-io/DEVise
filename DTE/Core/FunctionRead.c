/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.5  1997/04/10 21:50:23  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.4  1997/04/04 23:10:22  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.3  1997/03/23 23:45:19  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.2  1997/02/03 04:11:27  donjerko
  Catalog management moved to DTE

 */

#include <strstream.h>
#include <string.h>
#include "FunctionRead.h"
#include "exception.h"
#include "assert.h"
#include "url.h"

void FunctionRead::initialize(){	// Throws exception
	
	String * seqAttr = iterator->getOrderingAttrib();
	if (!seqAttr){
		String temp = " Table is not a sequence for using " + *function + " ";
		THROW(new Exception(temp.chars()),);
	}

	// Now get the position of the ordering attribute..

	String * attributeNames = iterator->getAttributeNames();
	TypeID * typeIDs = iterator->getTypeIDs();
    numFlds = iterator->getNumFlds();
    inputNumFlds = numFlds;
	int i;
	TypeID type;
	for (i = 0; i < numFlds; i++){
		if (attributeNames[i] == *seqAttr ){
			seqAttrPos = i;
			type = typeIDs[i];
			if (type != "int" && *function != "pos_offset"){
    			String temp = "Shift operators not implemented ";
				temp += "for non-discreate (" +  typeIDs[i] + ")";
				temp += " ordering attributes ";
				THROW(new Exception(temp.chars()),);
			}
			break;
		}
	}
	if (i > numFlds){
		THROW(new Exception("Sequence Attribute missing ??? "),);
	}


	// Now get the function pointers for the various function and 
	if (*function == "previous")
		funcPtr = previous;
	else if (*function == "next")
		funcPtr = next;
	else if (*function == "value_offset"){
		if (offset < 0 )
			funcPtr = neg_value_offset;
		else 
			funcPtr= pos_value_offset;
		String retType;
		TRY(comparePtr = getOperatorPtr("=",type,type,retType),)
	}
	else if (*function == "pos_offset"){
		if (offset >= 0 )
			funcPtr = pos_pos_offset;
		else 
			funcPtr = neg_pos_offset;
		String retType;
		TRY(comparePtr = getOperatorPtr("=",type,type,retType),)
	}
}

Tuple * FunctionRead::next()
{
	// For the initial case;;;
	if (!moreTup){
		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		if (!moreTup && TupleList->atEnd()) 
			return NULL;
		if (moreTup){
			nextTupVal = int(nextTup[seqAttrPos]);
			presentTupVal = nextTupVal - 1;
			currentTupVal = presentTupVal;
		}
	}
	// Now for the general case..
	if (presentTupVal >  currentTupVal - 1){
		
		currentTupVal = nextTupVal;
		if (!moreTup )
			return NULL;
		
		TupleList->removeAll();
		TupleList->append(nextTup);
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (int(tup[seqAttrPos]) == currentTupVal)
				TupleList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = int(tup[seqAttrPos]); 
				break;
			}
		}
		TupleList->rewind();
	}
	Tuple * temp = TupleList->get();
	TupleList->step();
	temp[seqAttrPos] = (Type*) presentTupVal;
	
	if (TupleList->atEnd()){
		TupleList->rewind();
		presentTupVal++;
	}
	return temp;
}
		

Tuple *FunctionRead::previous()
{

	// For the initial case;;;
	if (!moreTup){
		
		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		if (!moreTup && TupleList->atEnd())
			return NULL;

		if (moreTup){
			
			currentTupVal = int(nextTup[seqAttrPos]) -1;
			nextTupVal    = currentTupVal + 1;
			presentTupVal = currentTupVal +2;
		}
	}
	// Now for the general case..
	if (presentTupVal >  nextTupVal ){
		
		currentTupVal = nextTupVal;
		if (!moreTup )
			return NULL;
		
		TupleList->removeAll();
		TupleList->append(nextTup);
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (int(tup[seqAttrPos]) == currentTupVal)
				TupleList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = int(tup[seqAttrPos]); 
				break;
			}
		}
		TupleList->rewind();
	}
	Tuple * temp = TupleList->get();
	TupleList->step();
	temp[seqAttrPos] = (Type*) presentTupVal;
	
	if (TupleList->atEnd()){
		TupleList->rewind();
		presentTupVal++;
	}
	return temp;
}
Tuple * FunctionRead::neg_value_offset()
{
	// For the initial case;;;
	if (!moreTup){
		int count = 0;
		List<Tuple *> *dupList = NULL;

		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		if (!moreTup && TupleListList->atEnd())
			return NULL;
		if (moreTup){
		
			while(count < -offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);

				if (moreTup){
					currentTupVal = int(nextTup[seqAttrPos]);
				}
				if (moreTup){
				
					while(1){
					
						bool more;
						Tuple* tup = new Tuple[inputNumFlds];
						more = iterator->getNext(tup);
						if (!more ){
							moreTup = false;
							nextTup = NULL;
							nextTupVal = currentTupVal + 1;
							break;
						}
					   if (int(tup[seqAttrPos]) ==currentTupVal)
							dupList->append(tup);
						else{
							nextTup = tup;
							nextTupVal = int(tup[seqAttrPos]); 
							break;
						}
					}
					count ++;
					dupList->rewind();
					if (dupList->get()){
						TupleListList->append(dupList);
						TupleList->rewind();
						dupList->rewind();
					}
				}
			}
			if (moreTup)
				currentTupVal = int(nextTup[seqAttrPos]);
			nextTupVal    = currentTupVal ;
			if (dupList){
				int temp =int((dupList->get())[seqAttrPos]);
				presentTupVal = temp +1 ; 
			}
		}
	}
	// Now for the general case..
	if (presentTupVal >  nextTupVal){
		
		currentTupVal = nextTupVal;
		if (!moreTup )
			return NULL;
		TupleListList->rewind();	
		TupleListList->remove();
		
		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		if (moreTup)
			currentTupVal = int(nextTup[seqAttrPos]);
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (int(tup[seqAttrPos]) == currentTupVal)
				dupList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = int(tup[seqAttrPos]); 
				break;
			}
		}
		TupleListList->append(dupList);
		dupList->rewind();
		TupleListList->rewind();
	}
	List<Tuple*>* temp = TupleListList->get();
	Tuple * tup = temp->get();
	tup[seqAttrPos] = (Type*) (presentTupVal);
	temp->step();	
	
	if (temp->atEnd()){
		temp->rewind();
		presentTupVal++;
	}
	return tup;
}

Tuple * FunctionRead::pos_value_offset()
{

	// For the initial case;;;
	if (nextTup == NULL ){
		int count = 0;
		List<Tuple *> *dupList;

		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		if (!moreTup && TupleListList->atEnd())
			return NULL;

		if (moreTup)
			currentTupVal = int(nextTup[seqAttrPos]);

		if (moreTup){
		
			while(count < offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (moreTup)
					nextTupVal = int(nextTup[seqAttrPos]); 
			
				if (moreTup){
				
					while(1){
					
						bool more;
						Tuple* tup = new Tuple[inputNumFlds];
						more = iterator->getNext(tup);
						if (!more ){
							moreTup = false;
							nextTup = NULL;
							nextTupVal = currentTupVal + 1;
							break;
						}
					   if (int(tup[seqAttrPos]) ==nextTupVal)
							dupList->append(tup);
						else{
							nextTup = tup;
							nextTupVal = int(tup[seqAttrPos]); 
							break;
						}
					}
					count ++;
					dupList->rewind();
					if (dupList->get()){
						TupleListList->append(dupList);
						TupleList->rewind();
						dupList->rewind();
					}
				}
			}
			if (moreTup)
				nextTupVal = int(nextTup[seqAttrPos]); 
			presentTupVal = currentTupVal -1; 
		}
	}
	// Now for the general case..
	if ( presentTupVal >=  currentTupVal ){
		
		if (!moreTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		if (!TupleListList->atEnd()){
			List<Tuple *>* temp = TupleListList->get();
			temp->rewind();
			currentTupVal = int((temp->get())[seqAttrPos]);
		}
		else if (moreTup)
			 currentTupVal = int(nextTup[seqAttrPos]);

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		if (moreTup)
			nextTupVal = int(nextTup[seqAttrPos]); 
		
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (int(tup[seqAttrPos]) == nextTupVal)
				dupList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = int(tup[seqAttrPos]); 
				break;
			}
		}
		TupleListList->append(dupList);
		dupList->rewind();
		TupleListList->rewind();
	}
	
	List<Tuple*>* tempbeg = TupleListList->get();
	List<Tuple*>* tempend = TupleListList->getTail();

	Tuple * tup = tempend->get();
	Tuple * temp = tempbeg->get();
	tempend->step();	
	
	if(tup && temp){
		for(int i = 0;  i< numFlds;i++){
			temp[i] = tup[i];
		}
		temp[seqAttrPos] = (Type*) (presentTupVal);
	}
	if (tempend->atEnd()){
		tempend->rewind();
		presentTupVal++;
	}
	return temp;
}
Tuple *FunctionRead::neg_pos_offset()
{
				
	// For the initial case;;;
	if (!moreTup){
		
		int count = 0;
		List<Tuple *> *dupList;

		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		
		if (!moreTup )
			return NULL;
		
		if (moreTup){
		
			while(count < -offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (moreTup){
				
					while(1){
					
						bool more;
						Tuple* tup = new Tuple[inputNumFlds];
						more = iterator->getNext(tup);
						if (!more ){
							moreTup = false;
							nextTup = NULL;
							break;
						}
					 if (comparePtr->opPtr(nextTup[seqAttrPos],tup[seqAttrPos]))
							dupList->append(tup);
						else{
							nextTup = tup;
							break;
						}
					}
					count ++;
					dupList->rewind();
					if (dupList->get()){
						TupleListList->append(dupList);
						TupleList->rewind();
						dupList->rewind();
					}
				}
			}
		}
	}
	
	TupleListList->rewind();
	List<Tuple*>* temp = TupleListList->get();

	// Now for the general case..
	if ( temp->atEnd()){
		
		if (!moreTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				return NULL;
			}
			if (comparePtr->opPtr(tup[seqAttrPos],nextTup[seqAttrPos]))
				dupList->append(tup);
			else{
				nextTup = tup;
				break;
			}
		}
		TupleListList->append(dupList);
		dupList->rewind();
		TupleListList->rewind();
	}
	
	temp = TupleListList->get();
	Tuple * tup = temp->get();
	tup[seqAttrPos] = nextTup[seqAttrPos];
	temp->step();	
	
	return tup;
}
Tuple * FunctionRead::pos_pos_offset()
{

				
	// For the initial case;;;
	if (nextTup == NULL ){
		
		int count = 0;
		List<Tuple *> *dupList;

		nextTup = new Tuple[inputNumFlds];
		moreTup = iterator->getNext(nextTup);
		
		if (!moreTup && TupleListList->cardinality() < offset+1)
			return NULL;
		
		if (moreTup){
			
			// Note it is offset + 1 to include the last one too.
			while(count < offset+1){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (nextTup){
				
					while(1){
					
						bool more;
						Tuple* tup = new Tuple[inputNumFlds];
						more = iterator->getNext(tup);
						if (!more ){
							moreTup = false;
							nextTup = NULL;
							break;
						}
					 if (comparePtr->opPtr(nextTup[seqAttrPos],
								tup[seqAttrPos]))
							dupList->append(tup);
						else{
							nextTup = tup;
							break;
						}
					}
					count ++;
					dupList->rewind();
					if (dupList->get()){
						TupleListList->append(dupList);
						TupleList->rewind();
						dupList->rewind();
					}
				}
			}
		}
	}
	
	TupleListList->rewind();
	List<Tuple*>* beg = TupleListList->get();
	List<Tuple*>* end;
	end = TupleListList->getTail();
	
	// Now for the general case..
	if ( end->atEnd()){
		
		if (!moreTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		while(1){
			
			bool more;
			Tuple* tup = new Tuple[inputNumFlds];
			more = iterator->getNext(tup);
			if (!more ){
				moreTup = false;
				nextTup = NULL;
				break;
			}
			if (comparePtr->opPtr(tup[seqAttrPos],nextTup[seqAttrPos]))
				dupList->append(tup);
			else{
				nextTup = tup;
				break;
			}
		}
		TupleListList->append(dupList);
		dupList->rewind();
		TupleListList->rewind();
	}
	
	TupleListList->rewind();
	beg = TupleListList->get();
	end = TupleListList->getTail();
	
	beg->rewind();
	Tuple * tempbeg = beg->get();
	Tuple * tempend = end->get();
	end->step();
	
	if (tempend && tempbeg ){
		Type * temp = tempbeg[seqAttrPos];
		for(int i = 0;i < numFlds;i++)
			tempbeg[i] = tempend[i];
		tempbeg[seqAttrPos] = temp;
	}
	else 
		return NULL;

	return tempbeg;
}
