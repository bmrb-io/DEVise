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
#include <strstream.h>

class Engine : public PlanOp {
protected:
	String query;
	Site* topNode;
	Iterator* topNodeIt;
public:
	Engine() :  topNode(NULL), topNodeIt(NULL) {}
	Engine(String query) : query(query), topNode(NULL),
		topNodeIt(NULL) {}
	virtual ~Engine(){
		delete topNode;	// should delete all the sites
		delete topNodeIt;
	}
	int optimize();	// throws
	int optimize(String query){	// throws
		delete topNode;	// should delete all the sites
		topNode = NULL;
		delete topNodeIt;
		topNodeIt = NULL;
		this->query = query;
		return optimize();
	}
     virtual int getNumFlds(){
		return topNode->getNumFlds();
     }
     virtual const String* getTypeIDs(){
          return topNode->getTypeIDs();
     }
	const Tuple* getNext(){
		assert(topNodeIt || !"Initialize engine before calling getNext");
		return topNodeIt->getNext();
	}
	virtual const String* getAttributeNames(){
		return topNode->getAttributeNames();
	}
     String* getOrderingAttrib(){
		assert(topNode);
		return topNode->getOrderingAttrib();
     }
	virtual Stats* getStats(){
		return topNode->getStats();
	}
	void reset(int lowRid, int highRid){
		TRY(topNodeIt->reset(lowRid, highRid), );
	}
	void initialize(){
		assert(getNumFlds() != 0);
		assert(topNodeIt == NULL);
		TRY(topNodeIt = topNode->createExec(), );
		assert(topNodeIt);
		topNodeIt->initialize();
	}
	void finalize(){
		topNodeIt->finalize();
	}
	virtual Iterator* createExec(){
		assert(0);
		return topNode->createExec();
	}
};

class ViewEngine : public Engine {
	String* attributeNames;
	TypeID* typeIDs;
	int numFlds;
public:
	ViewEngine(String query, const String* attrs, int numInpFlds) : 
		Engine(query), attributeNames(NULL), typeIDs(NULL) {

		numFlds = numInpFlds + 1;
		attributeNames = new String[numFlds];
		attributeNames[0] = "recId";
		for(int i = 1; i < numFlds; i++){
			assert(attrs[i - 1] != "recId");
			attributeNames[i] = attrs[i - 1];
		}
	}
	~ViewEngine(){
		delete [] attributeNames;
		delete [] typeIDs;
	}
     const TypeID* getTypeIDs(){
		if(typeIDs){
			return typeIDs;
		}
		assert(topNode);
		assert(topNode->getNumFlds() + 1 == numFlds);
          const TypeID* inTypes = topNode->getTypeIDs();
		typeIDs = new TypeID[numFlds];
		typeIDs[0] = "int";
		for(int i = 1; i < numFlds; i++){
			typeIDs[i] = inTypes[i - 1];
		}
		return typeIDs;
     }
	const String* getAttributeNames(){
		return attributeNames;
	}
     int getNumFlds(){
		assert(topNode);
		assert(topNode->getNumFlds() + 1 == numFlds);
		return numFlds;
     }
	virtual Stats* getStats(){
		return new Stats(numFlds);
	}
	Iterator* createExec();
};

#endif
