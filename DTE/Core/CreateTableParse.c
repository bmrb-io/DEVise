#include <string>

#include <stdlib.h>
#include <sys/types.h>

#include "ParseTree.h"
#include "ExecOp.h"
#include "Utility.h"
#include "Interface.h"
#include "RelationManager.h"
#include "DTE/util/Del.h"
#include "DTE/types/DteStringAdt.h"

CreateTableParse::CreateTableParse(vector<IdentType>* identTypePairs)
	: identTypePairs(*identTypePairs) { delete identTypePairs; }

CreateTableParse::~CreateTableParse()
{
}

const ISchema* CreateTableParse::getISchema()
{
	return &STRING_SCHEMA;
}

Iterator* CreateTableParse::createExec()
{
	// Create a schema of this file that should be written to the catalog

  //int numFlds = identTypePairs.size();
        DteTupleAdt tupadt;
	vector<string> names;
	vector<IdentType>::const_iterator it;
	for(it = identTypePairs.begin(); it != identTypePairs.end() ; ++it) {
          names.push_back((*it).first);
          Del<DteAdt> adt = DteAdt::createAdt((*it).second);
          tupadt.push_back(*adt);
	}
	ISchema schema(tupadt, names);

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
        string relstr = relId.toString();
	char* ans = strdup(relstr.c_str());
	return new SingleAnswerIt(ans, DteStringAdt(relstr.length()));
}
