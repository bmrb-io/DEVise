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
     StandardRead(istream* in) : 
		in(in), numFlds(0), typeIDs(NULL), 
		attributeNames(NULL),
		readPtrs(NULL), order(NULL), stats(NULL) {}
	virtual ~StandardRead(){
		delete in;
		delete [] typeIDs;
		delete [] attributeNames;
		delete [] readPtrs;
		delete [] stats;
	}
	void open();	// Throws exception
	void writeTo(ofstream* outfile);
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual String *getTypeIDs(){
		return typeIDs;
	}
	virtual String* getAttributeNames(){
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
	virtual Tuple* getNext(streampos& pos){
		assert(in);
		pos = in->tellg();
		return getNext();
	}
	virtual Tuple* getNext(){
		assert(in);
		Tuple* tuple = new Tuple[numFlds];
          for(int i = 0; i < numFlds; i++){
			TRY(tuple[i] = (readPtrs[i])(*in), NULL);
		}
		if(in->good()){
			return tuple;
		}
		else{
			delete tuple;
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
     NCDCRead(istream* in) : StandardRead(in) {}
	void open();	// Throws exception
};

#endif
