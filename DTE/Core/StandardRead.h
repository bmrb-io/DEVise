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
  Revision 1.24  1998/03/17 17:19:03  donjerko
  Added new namespace management through relation ids.

  Revision 1.23  1997/11/12 23:17:35  donjerko
  Improved error checking.

  Revision 1.22  1997/11/05 00:19:42  donjerko
  Separated typechecking from optimization.

  Revision 1.21  1997/09/17 02:35:44  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.20  1997/09/09 14:42:16  donjerko
  Bug fix

  Revision 1.19  1997/09/05 22:20:13  donjerko
  Made changes for port to NT.

  Revision 1.18  1997/08/21 21:04:27  donjerko
  Implemented view materialization

  Revision 1.17  1997/08/15 00:17:36  donjerko
  Completed the Iterator destructor code.

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

//#include <iostream.h>   erased for sysdep.h
#include <string>
#include "types.h"
#include "Iterator.h"
#include "sysdep.h"
#include "Stats.h"

#ifndef __GNUG__
using namespace std;
#endif

class StandReadExec : public Iterator {
	istream* in;
	ReadPtr* readPtrs;
	DestroyPtr* destroyPtrs;
	Type** tuple;
	size_t* currentSz;
	int numFlds;
	int currentLine;
	string urlStr;
public:
	StandReadExec(int numFlds, const TypeID* typeIDs, istream* in, 
		string = "");
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
			readPtrs[i](*in, tuple[i]);
			if(currExcept){
				ostringstream tmp;
				tmp << "Line " << currentLine << " of " << urlStr << ends;
				currExcept->append(tmp.str());
				return NULL;
			}
		}
		currentLine++;
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
	string* typeIDs;
	string* attributeNames;
	string* order;
	Stats* stats;
	string urlStr;
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
	void open(istream* in, int numFlds, const TypeID* typeIDs); 
		// throws, used for tmp tables

	void open(const ISchema& schema, istream* in, const string& urlStr); 

	void writeTo(ofstream* outfile);
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual const string *getTypeIDs(){
		assert(typeIDs);
		return typeIDs;
	}
	virtual const string* getAttributeNames(){
		assert(attributeNames);
		return attributeNames;
	}
	virtual string * getOrderingAttrib(){
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
	virtual Iterator* createExec(){
		return new StandReadExec(numFlds, typeIDs, in, urlStr);
	}
};

class RidAdder : public PlanOp {
	PlanOp* input;
	int numFlds;
	TypeID* typeIDs;
	string* attributeNames;
public:
	RidAdder(PlanOp* input);
	virtual ~RidAdder(){
		delete input;
		delete [] typeIDs;
		delete [] attributeNames;
	}
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual const TypeID* getTypeIDs(){
		return typeIDs;
	}
	virtual const string* getAttributeNames(){
		return attributeNames;
	}
	virtual Iterator* createExec();
};

class NCDCRead : public StandardRead {
public:
     NCDCRead() : StandardRead() {}
	virtual void open(istream* in);	// Throws exception
};

#endif
