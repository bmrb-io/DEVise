#include <string>

#include "ParseTree.h"
#include "ExecOp.h"

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
	// does everything here
	
	string idFile = DTE_ENV_VARS.idFile;
	if(idFile.empty()){
		string err = "Please set up the environment variable \'" +
			DTE_ENV_VARS.idFileN + 
			"\' to point to an empty file" +
			" or a file that contains table id entries.";
		THROW(new Exception(err), 0);
	}
	string definitionFile = DTE_ENV_VARS.definitionFile;
	if(definitionFile.empty()){
		string err = "Please set up the environment variable \'" +
			DTE_ENV_VARS.definitionFileN + 
			"\' to point to an empty file" +
			" or a file that contains table definition entries.";
		THROW(new Exception(err), 0);
	}
	ifstream idStream(idFile.c_str());
	if(!idStream){
		string err = "Could not open file: " + idFile + " pointed by " +
			" env var " + DTE_ENV_VARS.idFileN;
		THROW(new Exception(err), 0);
	}
	char* answer = strdup("1.2");
	return new SingleAnswerIt(answer, stringDestroy);
}
