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
