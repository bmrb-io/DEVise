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

 */

#ifndef UNIDATA_READ_H
#define UNIDATA_READ_H

#include <iostream.h>
#include <String.h>
#include "types.h"
#include "Iterator.h"

const int BUFSZE = 2048;

class Attr;
class UniData;
typedef long off_t;

TypeID translateUDType(Attr* at);

class DevReadExec : public Iterator {
	char* buff;	// this has to be aligned!
	UniData* ud;
	UnmarshalPtr* unmarshalPtrs;
	DestroyPtr* destroyPtrs;
	Tuple* tuple;
	off_t off;
	int* offsets;
	int numFlds;
	int recId;
public:
	DevReadExec(UniData* ud, UnmarshalPtr* unmarshalPtrs,
		DestroyPtr* destroyPtrs,
		Tuple* tuple, int* offsets, int numFlds) :
		ud(ud), unmarshalPtrs(unmarshalPtrs),
		destroyPtrs(destroyPtrs), tuple(tuple),
		offsets(offsets), numFlds(numFlds) {

		buff = (char*) new double[BUFSZE / sizeof(double)];
		buff[BUFSZE - 1] = '\0';
		recId = 0;
	}

	virtual ~DevReadExec();

	virtual const Tuple* getNext(streampos& pos){
		assert(! "not implemented");
		return getNext();
	}

	virtual const Tuple* getNext();

	virtual void setOffset(Offset offset, RecId recId){
		off = offset.getOffset();
		this->recId = recId;
	}

     virtual Offset getOffset(){
		return off;
     }
};

class DevRead : public PlanOp {
	UniData* ud;
	int numFlds;
protected:
	String* typeIDs;
	String* attributeNames;
	String* order;
	int* offsets;
public:
     DevRead() : 
		ud(NULL), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		order(NULL), offsets(NULL){

	}

	virtual ~DevRead() { Close(); }

	virtual void open(istream* in){	// Throws exception
		assert(0);
	}

	void Open(char* schemaFile, char* dataFile); // throws

	void Close();

	virtual int getNumFlds(){
		return numFlds;
	}
	virtual String *getTypeIDs(){
		assert(typeIDs);
		return typeIDs;
	}
	virtual String* getAttributeNames(){
		assert(attributeNames);
		String* retVal = new String[numFlds];
		for(int i = 0; i < numFlds; i++){
			retVal[i] = attributeNames[i];
		}
		return retVal;
	}
	virtual String* getOrderingAttrib(){
		return order;
	}
	virtual ostream& display(ostream& out){
		out << "Num fields: " << numFlds << endl;
		out << "(";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " " << attributeNames[i];
			out << (i == numFlds - 1 ? "" : ", ");
		}
		out << ")";
		return out;
	}
	virtual void writeHeader(ostream& out){
		out << numFlds << " ";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " ";
		}
		cout << endl;
		for(int i = 0; i < numFlds; i++){
			out << attributeNames[i] << " ";
		}
		out << ";" << endl;
	}
	virtual Iterator* createExec();
};

#endif
