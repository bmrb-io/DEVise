/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.9  1997/06/21 22:48:01  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.8  1997/06/16 16:04:44  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.7  1997/06/14 22:34:35  liping
  re-write min/max and recId request with SQL queries

  Revision 1.5  1997/03/28 16:07:26  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef RTREE_READ_H
#define RTREE_READ_H

#include "myopt.h"
#include "RTree.h"
#include "Iterator.h"
#include "StandardRead.h"

struct RTreePred {
	bool bounded[2];
	bool closed[2];
	ConstantSelection* values[2];
	TypeID typeID;
	RTreePred(){
		bounded[0] = bounded[1] = false;
		closed[0] = closed[1] = true;
		values[0] = values[1] = NULL;
	}
	RTreePred(String opName, ConstantSelection* constant){
		bounded[0] = bounded[1] = false;
		closed[0] = closed[1] = true;
		values[0] = values[1] = NULL;
		if(opName == "="){
			bounded[0] = bounded[1] = true;
			values[0] = values[1] = constant;
		}
		else if(opName == "<="){
			bounded[1] = true;
			values[1] = constant;
		}
		else if(opName == ">="){
			bounded[0] = true;
			values[0] = constant;
		}
		else if(opName == "<"){
			bounded[1] = true;
			closed[1] = false;
			values[1] = constant;
		}
		else if(opName == ">"){
			bounded[0] = true;
			closed[0] = false;
			values[0] = constant;
		}
		else {
			cout << "Operator \"" << opName; 
			cout << "\" should not be passed to this function\n";
			assert(0);
		}
	}
	~RTreePred(){
		// do not delete values;
	}
	void setTypeID(TypeID t){
		typeID = t;
		TRY(Type* negInf = createNegInf(typeID), );
		TRY(Type* posInf = createPosInf(typeID), );
		values[0] = new ConstantSelection(typeID, negInf);
		values[1] = new ConstantSelection(typeID, posInf);
	}
	void update(String opName, BaseSelection* constant){ // throws
		assert(constant->selectID() == CONST_ID);
		ConstantSelection* cconstant = (ConstantSelection*) constant;
		TRY(cconstant = cconstant->promote(typeID), );
		assert(cconstant->getTypeID() == typeID);
		RTreePred tmp(opName, cconstant);
		intersect(tmp);
	}
	void intersect(const RTreePred& pred){

		if(pred.bounded[0] && !bounded[0]){
			bounded[0] = true;
			closed[0] = pred.closed[0];
			values[0] = pred.values[0];
		}
		else if(pred.bounded[0] && bounded[0]){
			assert(pred.values[0]);
			assert(values[0]);
			if(*pred.values[0] > *values[0]){
				values[0] = pred.values[0];
				closed[0] = pred.closed[0];
			}
			else if(*pred.values[0] == *values[0]){
				if(!pred.closed[0]){
					closed[0] = false;
				}
			}
		}
		if(pred.bounded[1] && !bounded[1]){
			bounded[1] = true;
			closed[1] = pred.closed[1];
			values[1] = pred.values[1];
		}
		else if(pred.bounded[1] && bounded[1]){
			assert(pred.values[1]);
			assert(values[1]);
			if(*pred.values[1] < *values[1]){
				values[1] = pred.values[1];
				closed[1] = pred.closed[1];
			}
			else if(*pred.values[1] == *values[1]){
				if(!pred.closed[1]){
					closed[1] = false;
				}
			}
		}
	}
	String toString() const {
		String retVal;
		if(closed[0]){
			retVal += "[";
		}
		else {
			retVal += "<";
		}
		if(bounded[0]){
			retVal += values[0]->toString();
		}
		else {
			retVal += "-Infinity";
		}
		retVal += ", ";
		if(bounded[1]){
			retVal += values[1]->toString();
		}
		else {
			retVal += "Infinity";
		}
		if(closed[1]){
			retVal += "]";
		}
		else {
			retVal += ">";
		}
		return retVal;
	}
};

class RTreeReadExec : public Iterator {
	genrtree_m* rtree_m;
	gen_rt_cursor_t* cursor;
	int dataSize;
	int numKeyFlds;
	int numAddFlds;
     Tuple* tuple;
     UnmarshalPtr* unmarshalPtrs;
     int* rtreeFldLens;

	char* dataContent;
	gen_key_t ret_key;
	bool ridInKey;
	int ridOffset;
public:
	RTreeReadExec(genrtree_m* rtree_m, gen_rt_cursor_t* cursor, int dataSize,
		int numKeyFlds, int numAddFlds, Tuple* tuple,
		UnmarshalPtr* unmarshalPtrs, int* rtreeFldLens, int ridPosition) :
		rtree_m(rtree_m), cursor(cursor), dataSize(dataSize),
		numKeyFlds(numKeyFlds), numAddFlds(numAddFlds),
		tuple(tuple), unmarshalPtrs(unmarshalPtrs), 
		rtreeFldLens(rtreeFldLens){
		
		dataContent = new char[dataSize + sizeof(Offset) + 100];
			// This extra space is required because of some bug in RTree.
		
		assert(ridPosition >= 0 && ridPosition < numKeyFlds + numAddFlds);

		ridOffset = 0;
		if(ridPosition < numKeyFlds){
			ridInKey = true;
			for(int i = 0; i < ridPosition; i++){
				ridOffset += rtreeFldLens[i];
			}
		}
		else{
			ridInKey = false;
			for(int i = numKeyFlds; i < ridPosition; i++){
				ridOffset += rtreeFldLens[i];
			}
		}
	}
	virtual ~RTreeReadExec(){
		delete cursor;
		delete [] tuple;
		delete [] unmarshalPtrs;
		delete [] rtreeFldLens;
		delete [] dataContent;
	}
	virtual void initialize() {}
	virtual const Tuple* getNext();
	virtual Offset getNextOffset();
	RecId getRecId();
};

class RTreeIndex : public StandardRead {

	// Private data members like numFlds, attributeNames and typeIDs
	// refer to this index not the original file

	IndexDesc* indexDesc;
	RTreePred* rTreeQuery;
	gen_key_t* queryBox;
	int queryBoxSize();
public:
	RTreeIndex(IndexDesc* indexDesc) : 
		StandardRead(), indexDesc(indexDesc) {
		numFlds = indexDesc->getTotNumFlds();
		stats = new Stats(numFlds);
		typeIDs = indexDesc->getAllTypeIDs();
		attributeNames = indexDesc->getAllAttrNms();
		rTreeQuery = new RTreePred[indexDesc->getNumKeyFlds()];
		for(int i = 0; i < indexDesc->getNumKeyFlds(); i++){
			rTreeQuery[i].setTypeID(typeIDs[i]);
		}
		queryBox = NULL;
	}
	virtual ~RTreeIndex(){
		delete queryBox;
	}
	bool canUse(BaseSelection* predicate);	// Throws exception
	void write(ostream& out){
		out << numFlds << " ";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " " << attributeNames[i] << " ";
		}
	}
     virtual ostream& display(ostream& out){
          String tmp;
		int numKeyFlds = getNumKeyFlds();
          for(int i = 0; i < numKeyFlds; i++){
               tmp += "Attr " + attributeNames[i] + ": ";
               tmp += rTreeQuery[i].toString() + "\n";
          }
          out << tmp;
          return out;
     }
	void setRTreeQuery(RTreePred* rTreeQuery){
		this->rTreeQuery = rTreeQuery;
	}
	int getNumKeyFlds(){
		return indexDesc->getNumKeyFlds();
	}
	int getNumAddFlds(){
		return indexDesc->getNumAddFlds();
	}
	Iterator* createExec();
};

#endif
