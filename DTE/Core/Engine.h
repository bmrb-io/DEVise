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
  Revision 1.21  1998/04/09 20:26:17  donjerko
  *** empty log message ***

  Revision 1.20  1997/12/04 04:05:09  donjerko
  *** empty log message ***

  Revision 1.19  1997/11/12 23:17:22  donjerko
  Improved error checking.

  Revision 1.18  1997/09/05 22:20:02  donjerko
  Made changes for port to NT.

  Revision 1.17  1997/08/21 21:04:17  donjerko
  Implemented view materialization

  Revision 1.16  1997/08/12 19:58:38  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.15  1997/08/09 00:54:43  donjerko
  Added indexing of select-project unmaterialized views.

  Revision 1.14  1997/07/30 21:39:15  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.13  1997/07/22 15:00:51  donjerko
  *** empty log message ***

  Revision 1.12  1997/06/21 22:47:57  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.11  1997/06/16 16:04:40  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.9  1997/03/23 23:45:19  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.7  1997/03/14 18:36:10  donjerko
  Making space for the SQL UNION operator.

  Revision 1.6  1997/02/03 04:11:27  donjerko
  Catalog management moved to DTE

  Revision 1.5  1996/12/21 22:21:43  donjerko
  Added hierarchical namespace.

  Revision 1.4  1996/12/15 06:41:04  donjerko
  Added support for RTree indexes

  Revision 1.3  1996/12/07 15:14:25  donjerko
  Introduced new files to support indexes.

  Revision 1.2  1996/12/05 16:05:59  wenger
  Added standard Devise file headers.

 */

#ifndef ENGINE_H
#define ENGINE_H

#include "types.h"
#include "site.h"
#include "Iterator.h"
//#include <strstream.h>   erased for sysdep.h

#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

class ParseTree;

class Engine : public PlanOp {
protected:
	string query;
	Iterator* topNodeIt;
	const ISchema* schema;
	ParseTree* parseTree;
public:
	Engine(const string& query) : query(query),
		topNodeIt(0), schema(0), parseTree(0) {}
	virtual ~Engine();
	const ParseTree* parse();	// throws
	const ISchema* typeCheck();	// throws
	virtual int optimize();	// throws
	/*
	int optimize(string query){	// throws
		delete topNode; // should delete all the sites
		topNode = NULL;
		delete topNodeIt;
		topNodeIt = NULL;

		this->query = query;
		return optimize();
	}
	*/
     virtual int getNumFlds(){
		assert(schema);
		return schema->getNumFlds();
     }
     virtual const string* getTypeIDs(){
		assert(schema);
          return schema->getTypeIDs();
     }
	const Tuple* getFirst(){
		assert(getNumFlds() != 0);
		assert(topNodeIt);
		return topNodeIt->getFirst();
	}
	const Tuple* getNext(){
		assert(topNodeIt || !"Initialize engine before calling getNext");
		return topNodeIt->getNext();
	}
        void finalize() {}      //kb: is this needed?
	virtual const string* getAttributeNames(){
		assert(schema);
		return schema->getAttributeNames();
	}
     string* getOrderingAttrib(){
		assert(!"not implemented");
		return 0;
     }
	virtual Stats* getStats(){
		assert(!"not implemented");
		return 0;
	}
        /*
	void reset(int lowRid, int highRid){
		TRY(topNodeIt->reset(lowRid, highRid), NVOID );
	}
	void finalize(){
		topNodeIt->finalize();
	}
        */

	virtual Iterator* createExec(){
		assert(0);
		return 0;
	}
};

class ViewEngine : public Engine {
	string* attributeNames;
	TypeID* typeIDs;
	int numFlds;
public:
	ViewEngine(string query, const string* attrs, int numInpFlds);
	~ViewEngine(){
		delete [] attributeNames;
		delete [] typeIDs;
	}
     const TypeID* getTypeIDs(){
		if(typeIDs){
			return typeIDs;
		}
		assert(schema);
		assert(schema->getNumFlds() + 1 == numFlds);
          const TypeID* inTypes = schema->getTypeIDs();
		typeIDs = new TypeID[numFlds];
		typeIDs[0] = "int";
		for(int i = 1; i < numFlds; i++){
			typeIDs[i] = inTypes[i - 1];
		}
		return typeIDs;
     }
	const string* getAttributeNames(){
		return attributeNames;
	}
     int getNumFlds(){
		assert(schema);
		assert(schema->getNumFlds() + 1 == numFlds);
		return numFlds;
     }
	virtual Stats* getStats(){
		return new Stats(numFlds);
	}
	virtual int optimize();	// throws
	virtual Iterator* createExec();
};

#endif
