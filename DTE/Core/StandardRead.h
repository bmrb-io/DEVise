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

class StandardRead : public Iterator {
protected:
	istream* in;
	int numFlds;
	String* typeIDs;
	String* attributeNames;
	ReadPtr* readPtrs;
	String* order;
	Stats* stats;
	Tuple* tuple;
	size_t* currentSz;
public:
     StandardRead() : 
		in(NULL), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		readPtrs(NULL), order(NULL), stats(NULL), tuple(NULL),
		currentSz(NULL) {}

	virtual ~StandardRead(){
		delete in;
		destroyTuple(tuple, numFlds, typeIDs);
		delete [] tuple;
		delete [] typeIDs;
		delete [] attributeNames;
		delete [] readPtrs;
		delete stats;
		delete [] currentSz;
	}
	virtual void open(istream* in);	// Throws exception
	void open(istream* in, int numFlds, const TypeID* typeIDs); // throws
		// used for tmp tables

	void writeTo(ofstream* outfile);
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
	virtual String * getOrderingAttrib(){
		return order;
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
     virtual Stats* getStats(){
          return stats;
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
};


class NCDCRead : public StandardRead {
public:
     NCDCRead() : StandardRead() {}
	virtual void open(istream* in);	// Throws exception
};

#endif
