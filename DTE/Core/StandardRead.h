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
  Revision 1.9  1997/04/04 23:10:26  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

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
public:
     StandardRead() : 
		in(NULL), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		readPtrs(NULL), order(NULL), stats(NULL) {}

	virtual ~StandardRead(){
		delete in;
		delete [] typeIDs;
		delete [] attributeNames;
		delete [] readPtrs;
		delete [] stats;
	}
	virtual void open(istream* in);	// Throws exception
	void open(istream* in, int numFlds, TypeID* typeIDs); // throws
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
	virtual WritePtr* getWritePtrs(){	// throws
          WritePtr* writePtrs = new WritePtr[numFlds];
          for(int i = 0; i < numFlds; i++){
               TRY(writePtrs[i] = getWritePtr(typeIDs[i]), NULL);
          }
          return writePtrs;
	}
	virtual String * getOrderingAttrib(){
		return order;
	}
	virtual bool getNext(streampos& pos, Tuple* next){
		assert(in);
		pos = in->tellg();
		return getNext(next);
	}
	virtual bool getNext(Tuple* tuple){
		assert(in);
          for(int i = 0; i < numFlds; i++){
			TRY((readPtrs[i])(*in, tuple[i]), false);
		}
		return in->good();
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
