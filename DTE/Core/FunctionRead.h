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

  Revision 1.2  1997/02/03 04:11:27  donjerko
  Catalog management moved to DTE

 */

#ifndef FUNCTION_READ_H
#define FUNCTION_READ_H

#include <iostream.h>
#include <String.h>
#include "Iterator.h"
#include "types.h"
#include "queue.h"

class FunctionRead : public Iterator {
protected:
	Iterator * iterator;
	String * function;
public:
     FunctionRead(Iterator * iterator,String * function,int offset) : 
		iterator(iterator),function(function),offset(offset){
			TupleList = new List<Tuple *>;
			TupleListList = new List<List<Tuple*>*>;
			nextTup = NULL;
			presentTupVal = nextTupVal = currentTupVal = 0;
			comparePtr = 0;
	}
	virtual void initialize();	// Throws exception
	virtual int getNumFlds(){
		return iterator->getNumFlds();
	}
	virtual String *getTypeIDs(){
		return iterator->getTypeIDs();
	}
	virtual String *getAttributeNames(){
		return iterator->getAttributeNames();
	}
	virtual String * getOrderingAttrib(){
		return iterator->getOrderingAttrib();
	}
	virtual const Tuple* getNext(){
		return funcPtr();
	}
	virtual ostream& display(ostream& out){
		return iterator->display(out);
	}

	Tuple * previous();
	Tuple * next();
	Tuple * pos_offset();
	Tuple * neg_value_offset();
	Tuple * pos_value_offset();
	Tuple * neg_pos_offset();
	Tuple * pos_pos_offset();
private:	
	
	Tuple * (FunctionRead::* funcPtr) ();
	GeneralPtr * comparePtr;
	List<Tuple*>* TupleList;
	List<List<Tuple*>*>* TupleListList;
	int seqAttrPos;
	Tuple * nextTup;
	bool moreTup;
	int currentTupVal,nextTupVal,presentTupVal;
	int offset;
	int numFlds;
	int inputNumFlds; 	// happens to be the same as numFlds
};

#endif
