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
  Revision 1.3  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.2  1996/12/05 16:06:00  wenger
  Added standard Devise file headers.

 */

#ifndef STANDARD_READ_H
#define STANDARD_READ_H

#include <iostream.h>
#include <String.h>
#include "Iterator.h"
#include "types.h"

class StandardRead : public Iterator {
protected:
	istream* in;
	int numFlds;
	String* typeIDs;
	String* attributeNames;
	ReadPtr* readPtrs;
	String order;
	Stats* stats;
public:
     StandardRead(istream* in) : 
		in(in), numFlds(0), typeIDs(NULL), attributeNames(NULL),
		readPtrs(NULL), stats(NULL) {}
	void open();	// Throws exception
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual String *getTypeIDs(){
		return typeIDs;
	}
	virtual String *getAttributeNames(){
		return attributeNames;
	}
	virtual String * getOrderingAttrib(){
		return &order;
	}
	virtual Tuple* getNext(){
		Tuple* tuple = new Tuple[numFlds];
          for(int i = 0; i < numFlds; i++){
			tuple[i] = (readPtrs[i])(*in);
		}
		if(*in){
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
};

class NCDCRead : public StandardRead {
public:
     NCDCRead(istream* in) : StandardRead(in) {}
	void open();	// Throws exception
};

#endif
