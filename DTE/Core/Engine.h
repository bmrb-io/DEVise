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
	String query;
	Site* topNode;
	String* attributeNames;
	Iterator* topNodeIt;
public:
	Engine() :  topNode(NULL), attributeNames(NULL), topNodeIt(NULL) {}
	Engine(String query) : query(query), topNode(NULL),
		attributeNames(NULL), topNodeIt(NULL) {}
	virtual ~Engine(){
		delete topNode;	// should delete all the sites
		delete [] attributeNames;
		delete topNodeIt;
	}
	int optimize();	// throws
	int optimize(String query){	// throws
		delete topNode;	// should delete all the sites
		topNode = NULL;
		delete [] attributeNames;
		attributeNames = NULL;
		delete topNodeIt;
		topNodeIt = NULL;
		this->query = query;
		return optimize();
	}
     int getNumFlds(){
		return topNode->getNumFlds();
     }
     const String* getTypeIDs(){
          return topNode->getTypeIDs();
     }
	const Tuple* getNext(){
		assert(topNodeIt || !"Initialize engine before calling getNext");
		return topNodeIt->getNext();
	}
	const String* getAttributeNames(){
		if(attributeNames){
			return attributeNames;
		}
		else{
			return topNode->getAttributeNames();
		}
	}
     String* getOrderingAttrib(){
		assert(topNode);
		return topNode->getOrderingAttrib();
     }
	void renameAttributes(const String* newAttributeNames){
		assert(attributeNames == NULL);
		int nf = topNode->getNumFlds();
		attributeNames = new String[nf];
		assert(attributeNames);
		for(int i = 0; i < nf; i++){
			attributeNames[i] = newAttributeNames[i];
		}
	}
	Stats* getStats(){
		return topNode->getStats();
	}
	void reset(int lowRid, int highRid){
		TRY(topNodeIt->reset(lowRid, highRid), );
	}
	void initialize(){
		assert(getNumFlds() != 0);
		topNodeIt = createExec();
		topNodeIt->initialize();
	}
	void finalize(){
		topNodeIt->finalize();
	}
	Iterator* createExec(){
		return topNode->createExec();
	}
};

#endif
