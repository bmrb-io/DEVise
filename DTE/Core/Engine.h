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

  Revision 1.9  1997/03/23 23:45:19  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.8  1997/03/20 20:42:19  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

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
#include <strstream.h>

class Engine : public Iterator {
	String query;
	Site* topNode;
	String* attributeNames;
public:
	Engine(String query) : query(query), topNode(NULL),
		attributeNames(NULL) {}
	virtual ~Engine(){
		delete topNode;	// should delete all the sites
		delete [] attributeNames;
	}
	int optimize();	// throws
     virtual int getNumFlds(){
		return topNode->getNumFlds();
     }
     virtual String* getTypeIDs(){
          return topNode->getTypeIDs();
     }
	virtual const Tuple* getNext(){
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
     virtual String* getOrderingAttrib(){
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
