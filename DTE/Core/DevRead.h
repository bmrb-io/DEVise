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

//#include <iostream.h>   erased for sysdep.h
#include <string>
#include "types.h"
#include "Iterator.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

const int BUFSZE = 2048;

class Attr;
class UniData;
typedef long off_t;

TypeID translateUDType(Attr* at);

class DevReadExec : public Iterator {
	char* buff;	// this has to be aligned!
	int buffSize;
	UniData* ud;
	UnmarshalPtr* unmarshalPtrs;
	DestroyPtr* destroyPtrs;
	Type** tuple;
	off_t off;
	int* offsets;
	int numFlds;
	int recId;
public:
	DevReadExec(UniData* ud, UnmarshalPtr* unmarshalPtrs,
		DestroyPtr* destroyPtrs,
		Type** tuple, int* offsets, int numFlds);

	virtual ~DevReadExec();

	virtual const Tuple* getNext(streampos& pos){
		assert(! "not implemented");
		return getNext();
	}

	virtual const Tuple* getNext();

	virtual const Tuple* getThis(Offset offset, RecId recId);

     virtual Offset getOffset(){
		return off;
     }
};

class DevRead : public PlanOp {
	UniData* ud;
	int numFlds;
protected:
	string* typeIDs;
	string* attributeNames;
	string* order;
public:
     DevRead() : 
		ud(NULL), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		order(NULL){

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
	virtual const TypeID* getTypeIDs(){
		assert(typeIDs);
		return typeIDs;
	}
	virtual TypeID* stealTypeIDs(){
		TypeID* retVal = typeIDs;
		typeIDs = NULL;
		return retVal;
	}
	virtual const string* getAttributeNames(){
		assert(attributeNames);
		return attributeNames;
	}
	virtual string* stealAttributeNames(){
		string* retVal = attributeNames;
		attributeNames = NULL;
		return retVal;
	}
	virtual string* getOrderingAttrib(){
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
	virtual Iterator* createExec();
};

#endif
