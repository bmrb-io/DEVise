#ifndef RELATION_MANAGER_H 
#define RELATION_MANAGER_H

#include <string>

#include "sysdep.h"
#include "Interface.h"
#include "RelationId.h"

class RelationManager {
	fstream idStream;
	fstream defStream;
	string err;	// error in constructor
public:
	RelationManager();
	RelationId registerNewRelation(const Interface&);
	void deleteRelation(const RelationId&);
	Interface* createInterface(const RelationId&);	
};

extern RelationManager RELATION_MNGR;

#endif
