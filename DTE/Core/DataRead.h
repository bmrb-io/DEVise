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

#ifndef DATAREADER_READ_H
#define DATAREADER_READ_H

//#include <iostream.h>   erased for sysdep.h
#include <string>
#include "types.h"
#include "Iterator.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

class Attr;
class DataReader;
typedef long off_t;

class DataReadExec : public Iterator {
	char* buff;	// this has to be aligned!
	int buffSize;
	DataReader* ud;
	UnmarshalPtr* unmarshalPtrs;
	DestroyPtr* destroyPtrs;
	Type** tuple;
	off_t off;
	int* offsets;
	int numFlds;
	int recId;
public:
	DataReadExec(DataReader* ud, UnmarshalPtr* unmarshalPtrs,
		DestroyPtr* destroyPtrs,
		Type** tuple, int* offsets, int numFlds);

	virtual ~DataReadExec();

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

class DataRead : public PlanOp {
	DataReader* ud;
	int numFlds;
protected:
	TypeID* typeIDs;
	string* attributeNames;
	string* order;
public:
     DataRead(const string& schemaFile, const string& dataFile); // throws

	virtual ~DataRead() { Close(); }

	virtual void open(istream* in){	// Throws exception
		assert(0);
	}

	void translateUDInfo();

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
