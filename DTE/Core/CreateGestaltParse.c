#include <string>

#include <stdlib.h>
#include <sys/types.h>

#include "ParseTree.h"
#include "ExecOp.h"
#include "Utility.h"

CreateGestaltParse::CreateGestaltParse(List<string*>* _gestaltName, 
				       vector<IdentType*>* _identTypePairs,
				       string* _fileName) : 
  gestaltName(new TableName(_gestaltName)), identTypePairs(_identTypePairs), fileName(_fileName) {}

CreateGestaltParse::~CreateGestaltParse()
{
	vector<IdentType*>::iterator i;
	for(i = identTypePairs->begin(); i != identTypePairs->end(); ++i){
		delete (*i)->first;
		delete (*i)->second;
		delete (*i);
	}
	delete identTypePairs;
	delete gestaltName;
	delete fileName;
}

Iterator* CreateGestaltParse::createExec()
{
	List<ConstantSelection*>* lconst = new List<ConstantSelection*>;
	string* s = new string();
	char buf [100];
	sprintf (buf, "%d", (int) (identTypePairs -> size ()));
	*s = *s + "\"" + gestaltName -> fileName() + "\" Gestalt " + buf + " "; 
	
	vector<IdentType*>::iterator i;
	for(i = identTypePairs->begin(); i != identTypePairs->end(); ++i)
	        *s = *s + *((*i)->first) + " " + *((*i)->second) + " ";
	*s = *s + *fileName + " ;";

	lconst -> append(new ConstantSelection("string", strdup(s -> c_str())));
	InsertParse* ip = new InsertParse(gestaltName -> dirName().returnAsList(), lconst);
	ip -> createExec();
	return NULL;
}

RegisterIntoGestaltParse::RegisterIntoGestaltParse(List<string*>* gestaltName, 
						   List<string*>* tableName) :
  gestaltName(gestaltName), tableName(new TableName(tableName)) {}


Iterator* RegisterIntoGestaltParse::createExec()
{
        string s = tableName->toString();
	List<ConstantSelection*>* lconst = new List<ConstantSelection*>;
	lconst -> append(new ConstantSelection("string", strdup(s.c_str())));

        InsertParse* ip = new InsertParse(gestaltName, lconst);
	ip -> createExec ();
	return NULL;
}

UnregisterFromGestaltParse::UnregisterFromGestaltParse(List<string*>* gestaltName, 
						   List<string*>* tableName) :
  gestaltName(new TableName(gestaltName)), tableName(new TableName(tableName)) {}


Iterator* UnregisterFromGestaltParse::createExec()
{
  return NULL;
}


