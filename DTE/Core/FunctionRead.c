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
  Revision 1.3  1996/12/16 11:13:07  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.2  1996/12/05 16:06:00  wenger
  Added standard Devise file headers.

 */

#include <strstream.h>
#include <string.h>
#include "FunctionRead.h"
#include "exception.h"
#include "assert.h"
#include "url.h"

void FunctionRead::open(){	// Throws exception
	iterator->open();
	
	String * seqAttr = iterator->getOrderingAttrib();
	if (!seqAttr){
		String temp = " Table is not a sequence for using " + *function + " ";
		THROW(new Exception(temp.chars()),);
	}

	// Now get the position of the ordering attribute..

	String * attributeNames = iterator->getAttributeNames();
	TypeID * typeIDs = iterator->getTypeIDs();
    numFlds = iterator->getNumFlds();
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
	if (nextTup == NULL ){
		nextTup = iterator->getNext();
		if (!nextTup && TupleList->atEnd()) 
			return NULL;
		if (nextTup){
			nextTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();
			presentTupVal = nextTupVal - 1;
			currentTupVal = presentTupVal;
		}
	}
	// Now for the general case..
	if (presentTupVal >  currentTupVal - 1){
		
		currentTupVal = nextTupVal;
		if (!nextTup )
			return NULL;
		
		TupleList->removeAll();
		TupleList->append(nextTup);
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (((IInt *)tup[seqAttrPos])->getValue() == currentTupVal)
				TupleList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
				break;
			}
		}
		TupleList->rewind();
	}
	Tuple * temp = TupleList->get();
	TupleList->step();
	((IInt *)temp[seqAttrPos])->setValue(presentTupVal);
	
	if (TupleList->atEnd()){
		TupleList->rewind();
		presentTupVal++;
	}
	return temp;
}
		

Tuple *FunctionRead::previous()
{

	// For the initial case;;;
	if (nextTup == NULL ){
		
		nextTup = iterator->getNext();
		if (!nextTup && TupleList->atEnd())
			return NULL;

		if (nextTup){
			
			currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue() -1;
			nextTupVal    = currentTupVal + 1;
			presentTupVal = currentTupVal +2;
		}
	}
	// Now for the general case..
	if (presentTupVal >  nextTupVal ){
		
		currentTupVal = nextTupVal;
		if (!nextTup )
			return NULL;
		
		TupleList->removeAll();
		TupleList->append(nextTup);
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (((IInt *)tup[seqAttrPos])->getValue() == currentTupVal)
				TupleList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
				break;
			}
		}
		TupleList->rewind();
	}
	Tuple * temp = TupleList->get();
	TupleList->step();
	((IInt *)temp[seqAttrPos])->setValue(presentTupVal);
	
	if (TupleList->atEnd()){
		TupleList->rewind();
		presentTupVal++;
	}
	return temp;
}
Tuple * FunctionRead::neg_value_offset()
{
	// For the initial case;;;
	if (nextTup == NULL ){
		int count = 0;
		List<Tuple *> *dupList;

		nextTup = iterator->getNext();
		if (!nextTup && TupleListList->atEnd())
			return NULL;
		if (nextTup){
		
			while(count < -offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
			
				if (nextTup)
					currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();
				if (nextTup){
				
					while(1){
					
						Tuple * tup = iterator->getNext();
						if (!tup ){
							nextTup = NULL;
							nextTupVal = currentTupVal + 1;
							break;
						}
					   if (((IInt *)tup[seqAttrPos])->getValue()==currentTupVal)
							dupList->append(tup);
						else{
							nextTup = tup;
							nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
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
			if (nextTup)
				currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();
			nextTupVal    = currentTupVal ;
			if (dupList){
				int temp =((IInt *)(dupList->get())[seqAttrPos])->getValue();
				presentTupVal = temp +1 ; 
			}
		}
	}
	// Now for the general case..
	if (presentTupVal >  nextTupVal){
		
		currentTupVal = nextTupVal;
		if (!nextTup )
			return NULL;
		TupleListList->rewind();	
		TupleListList->remove();
		
		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		if (nextTup)
			currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (((IInt *)tup[seqAttrPos])->getValue() == currentTupVal)
				dupList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
				break;
			}
		}
		TupleListList->append(dupList);
		dupList->rewind();
		TupleListList->rewind();
	}
	List<Tuple*>* temp = TupleListList->get();
	Tuple * tup = temp->get();
	((IInt *)tup[seqAttrPos])->setValue(presentTupVal);
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

		nextTup = iterator->getNext();
		if (!nextTup && TupleListList->atEnd())
			return NULL;
		if (nextTup)
			currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();

		if (nextTup){
		
			while(count < offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (nextTup)
					nextTupVal = ((IInt *)nextTup[seqAttrPos])->getValue(); 
			
				if (nextTup){
				
					while(1){
					
						Tuple * tup = iterator->getNext();
						if (!tup ){
							nextTup = NULL;
							nextTupVal = currentTupVal + 1;
							break;
						}
					   if (((IInt *)tup[seqAttrPos])->getValue()==nextTupVal)
							dupList->append(tup);
						else{
							nextTup = tup;
							nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
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
			if (nextTup)
				nextTupVal = ((IInt *)nextTup[seqAttrPos])->getValue(); 
			presentTupVal = currentTupVal -1; 
		}
	}
	// Now for the general case..
	if ( presentTupVal >=  currentTupVal ){
		
		if (!nextTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		if (!TupleListList->atEnd()){
			List<Tuple *>* temp = TupleListList->get();
			temp->rewind();
			currentTupVal = ((IInt *)(temp->get())[seqAttrPos])->getValue();
		}
		else if (nextTup)
			 currentTupVal = ((IInt *)nextTup[seqAttrPos])->getValue();

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		if (nextTup)
			nextTupVal = ((IInt *)nextTup[seqAttrPos])->getValue(); 
		
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				nextTupVal = currentTupVal + 1;
				break;
			}
			if (((IInt *)tup[seqAttrPos])->getValue() == nextTupVal)
				dupList->append(tup);
			else{
				nextTup = tup;
				nextTupVal = ((IInt *)tup[seqAttrPos])->getValue(); 
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
		temp[seqAttrPos] = new IInt(presentTupVal);
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
	if (nextTup == NULL ){
		
		int count = 0;
		List<Tuple *> *dupList;

		nextTup = iterator->getNext();
		
		if (!nextTup )
			return NULL;
		
		if (nextTup){
		
			while(count < -offset){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (nextTup){
				
					while(1){
					
						Tuple * tup = iterator->getNext();
						if (!tup ){
							nextTup = NULL;
							break;
						}
					 if (((IBool *)comparePtr->opPtr(nextTup[seqAttrPos],tup[seqAttrPos]))->getValue())
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
		
		if (!nextTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				return NULL;
			}
			if (((IBool*)comparePtr->opPtr(tup[seqAttrPos],nextTup[seqAttrPos]))->getValue())
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

		nextTup = iterator->getNext();
		
		if (!nextTup && TupleListList->cardinality() < offset+1)
			return NULL;
		
		if (nextTup){
			
			// Note it is offset + 1 to include the last one too.
			while(count < offset+1){
			
				dupList = new List<Tuple *>;
				dupList->append(nextTup);
				
				if (nextTup){
				
					while(1){
					
						Tuple * tup = iterator->getNext();
						if (!tup ){
							nextTup = NULL;
							break;
						}
					 if (((IBool *)comparePtr->opPtr(nextTup[seqAttrPos],
								tup[seqAttrPos]))->getValue())
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
		
		if (!nextTup )
			return NULL;
		
		TupleListList->rewind();	
		TupleListList->remove();

		List<Tuple*>* dupList = new List<Tuple*>;
		dupList->append(nextTup);
		
		while(1){
			
			Tuple * tup = iterator->getNext();
			if (!tup ){
				nextTup = NULL;
				break;
			}
			if (((IBool*)comparePtr->opPtr(tup[seqAttrPos],nextTup[seqAttrPos]))->getValue())
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
