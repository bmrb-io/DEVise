#include <string>

#include <stdlib.h>
#include <sys/types.h>

#include "ParseTree.h"
#include "ExecOp.h"
#include "Utility.h"
#include "Interface.h"
#include "RelationManager.h"

CreateTableParse::CreateTableParse(vector<IdentType*>* identTypePairs)
	: identTypePairs(identTypePairs) {}

CreateTableParse::~CreateTableParse()
{
	vector<IdentType*>::iterator i;
	for(i = identTypePairs->begin(); i != identTypePairs->end(); ++i){
		delete (*i)->first;
		delete (*i)->second;
		delete (*i);
	}
	delete identTypePairs;
}

const ISchema* CreateTableParse::getISchema()
{
	return &STRING_SCHEMA;
}

Iterator* CreateTableParse::createExec()
{
	// Create a schema of this file that should be written to the catalog

	int numFlds = identTypePairs->size();
	TypeID* const types = new TypeID[numFlds];
	string* const attrs = new string[numFlds];
	vector<IdentType*>::const_iterator it;
	int i = 0;
	for(it = identTypePairs->begin(); i < numFlds; i++, ++it){
		attrs[i] = *((*it)->first);
		types[i] = *((*it)->second);
	}
	ISchema schema(numFlds, types, attrs);
	delete [] types;
	delete [] attrs;

	string dirPath = DTE_ENV_VARS.valueOf(DTE_ENV_VARS.tempFileDir);
	if(dirPath.empty()){
		string err = "Please set up the environment variable \'" +
			DTE_ENV_VARS.tempFileDir + "\' to point to a directory";
		THROW(new Exception(err), 0);
	}
	string filePath = dirPath + "/junk";

	StandardInterface stdInt(schema, filePath);

	RelationId relId;
	TRY(relId = RELATION_MNGR.registerNewRelation(stdInt), NULL);
	char* ans = strdup(relId.toString().c_str());
	return new SingleAnswerIt(ans, STRING_TP);
}
