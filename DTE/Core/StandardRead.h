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
  Revision 1.16  1997/08/12 19:58:43  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.15  1997/07/30 21:39:19  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.14  1997/07/22 15:00:54  donjerko
  *** empty log message ***

  Revision 1.13  1997/06/30 23:05:04  donjerko
  CVS:

  Revision 1.12  1997/06/21 22:48:05  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.11  1997/06/16 16:04:46  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.10  1997/04/10 21:50:25  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.8  1997/03/23 23:45:21  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.7  1997/02/18 18:06:02  donjerko
  Added skeleton files for sorting.

  Revision 1.6  1997/02/03 04:11:32  donjerko
  Catalog management moved to DTE

  Revision 1.5  1996/12/21 22:21:46  donjerko
  Added hierarchical namespace.

  Revision 1.4  1996/12/16 11:13:08  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.3  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.2  1996/12/05 16:06:00  wenger
  Added standard Devise file headers.

 */

#ifndef STANDARD_READ_H
#define STANDARD_READ_H

#include <iostream.h>
#include <String.h>
#include "types.h"
#include "Iterator.h"

class StandReadExec : public Iterator {
	istream* in;
	ReadPtr* readPtrs;
	DestroyPtr* destroyPtrs;
	Tuple* tuple;
	size_t* currentSz;
	int numFlds;
public:
	StandReadExec(istream* in, ReadPtr* readPtrs,
		DestroyPtr* destroyPtrs, Tuple* tuple,
		size_t* currentSz, int numFlds) : 
		in(in), readPtrs(readPtrs),
		destroyPtrs(destroyPtrs), tuple(tuple), currentSz(currentSz),
		numFlds(numFlds) {}
	StandReadExec(const ISchema& schema, istream* in);
	virtual ~StandReadExec(){
		delete [] currentSz;
		delete [] readPtrs;
		destroyTuple(tuple, numFlds, destroyPtrs);
		delete [] destroyPtrs;
		delete in;
	}
	virtual const Tuple* getNext(streampos& pos){
		assert(in);
		pos = in->tellg();
		return getNext();
	}
	virtual const Tuple* getNext(){
		assert(in);
          for(int i = 0; i < numFlds; i++){
			TRY((readPtrs[i])(*in, tuple[i]), NULL);
		}
		if(in->good()){
			return tuple;
		}
		else {
			return NULL;
		}
	}
};

class StandardRead : public PlanOp {
protected:
	istream* in;
	int numFlds;
	String* typeIDs;
	String* attributeNames;
	String* order;
	Stats* stats;
public:
     StandardRead() : 
		in(NULL), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		order(NULL), stats(NULL) {}

	virtual ~StandardRead(){
		delete [] typeIDs;
		delete [] attributeNames;
		delete stats;
	}
//	virtual void open(istream* in);	// Throws exception
	void open(istream* in, int numFlds, const TypeID* typeIDs); 
		// throws, used for tmp tables

	void open(const ISchema& schema, istream* in); 

	void writeTo(ofstream* outfile);
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual const String *getTypeIDs(){
		assert(typeIDs);
		return typeIDs;
	}
	virtual const String* getAttributeNames(){
		assert(attributeNames);
		return attributeNames;
	}
	virtual String * getOrderingAttrib(){
		return order;
	}
     virtual Stats* getStats(){
		if(stats){
			return stats;
		}
		else{
			return PlanOp::getStats();
		}
     }
	virtual ostream& display(ostream& out){
		out << "Num fields: " << numFlds << endl;
		out << "(";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " " << attributeNames[i];
			out << (i == numFlds - 1 ? "" : ", ");
		}
		out << ")";
		stats->display(out);
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
	virtual Iterator* createExec(){
		ReadPtr* readPtrs = new ReadPtr[numFlds];
		DestroyPtr* destroyPtrs = new DestroyPtr[numFlds];
		size_t* currentSz = new size_t[numFlds];
		Tuple* tuple = new Tuple[numFlds];
		for(int i = 0; i < numFlds; i++){
			readPtrs[i] = getReadPtr(typeIDs[i]);
			destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
			assert(destroyPtrs[i]);
			tuple[i] = allocateSpace(typeIDs[i], currentSz[i]);
		}
		return new StandReadExec(
			in, readPtrs, destroyPtrs, tuple, currentSz, numFlds);
	}
};


class NCDCRead : public StandardRead {
public:
     NCDCRead() : StandardRead() {}
	virtual void open(istream* in);	// Throws exception
};

#endif
