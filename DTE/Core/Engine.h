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
 */

#ifndef ENGINE_H
#define ENGINE_H

#include "types.h"
#include "site.h"
#include <strstream.h>

class Engine {
	String query;
	Site* topNode;
	int numFlds;
	String* typeIDs;
	String* attributeNames;
	Stats* stats;
public:
	Engine(String query) : query(query), topNode(NULL) {}
	virtual ~Engine(){
		delete topNode;	// should delete all the sites
	}
	int optimize();	// throws
     virtual int getNumFlds(){
          return numFlds;
     }
     virtual String *getTypeIDs(){
          return typeIDs;
     }
	virtual Tuple* getNext(){
		return topNode->getNext();
	}
	virtual String* getAttributeNames(){
		return attributeNames;
	}
	Stats* getStats(){
		return stats;
	}
	void reset(int lowRid, int highRid){
		TRY(topNode->reset(lowRid, highRid), );
	}
};

#endif
