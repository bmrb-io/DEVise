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
#include "STuple.h"

#ifndef __GNUG__
using namespace std;
#endif

class Attr;
class DataReader;
typedef long off_t;


//---------------------------------------------------------------------------


class DataReadExec : public RandomAccessIterator
{
public:

  DataReadExec(const string& schemaFile, const string& dataFile); // throws

  DataReadExec::DataReadExec(DataReader* dr);
  
  virtual ~DataReadExec();

  virtual void initialize();

  virtual const Tuple* getNext();
  
  virtual const Tuple* getThis(Offset offset);
  
  virtual Offset getOffset();

  static void translateSchema(DataReader* dr, ISchema& schema);

protected:

  void init();                  // completes construction

  void translateTypes();

  char* buff;	// this has to be aligned!
  int buffSize;
  DataReader* dr;
  STuple tuple;
  int currentOffset;
  int numFlds;
};


//---------------------------------------------------------------------------

#if 0
class DataRead : public PlanOp {
	DataReader* dr;
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
		return typeIDs;
	}
	virtual const string* getAttributeNames(){
		return attributeNames; // may be NULL
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

#endif
