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
#include <strstream.h>

class Engine : public Iterator {
	String query;
	Site* topNode;
	String* attributeNames;
	WritePtr* writePtrs;
public:
	Engine(String query) : query(query), topNode(NULL),
		attributeNames(NULL), writePtrs(NULL) {}
	virtual ~Engine(){
		delete topNode;	// should delete all the sites
		delete [] writePtrs;
	}
	int optimize();	// throws
     virtual int getNumFlds(){
		return topNode->getNumFlds();
     }
     virtual String* getTypeIDs(){
          return topNode->getTypeIDs();
     }
	virtual Tuple* getNext(){
		return topNode->getNext();
	}
	virtual String* getAttributeNames(){
		if(attributeNames){
			return attributeNames;
		}
		else{
			return topNode->getAttributeNames();
		}
	}
	void renameAttributes(String* newAttributeNames){
		attributeNames =  newAttributeNames;
	}
	Stats* getStats(){
		return topNode->getStats();
	}
	WritePtr* getWritePtrs(){
		if(writePtrs){
			return writePtrs;
		}
		int numFlds = getNumFlds();
		TypeID* types = getTypeIDs();
		writePtrs = new WritePtr[numFlds];
		for(int i = 0; i < numFlds; i++){
			TRY(writePtrs[i] = getWritePtr(types[i]), NULL);
		}
		return writePtrs;
	}
	void reset(int lowRid, int highRid){
		TRY(topNode->reset(lowRid, highRid), );
	}
	virtual void initialize(){
		assert(getNumFlds() != 0);
		topNode->initialize();
	}
	virtual void finalize(){
		topNode->finalize();
	}
};

#endif
