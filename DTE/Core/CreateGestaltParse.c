#include <string>

#include <stdlib.h>
#include <sys/types.h>

#include "ParseTree.h"
//#include "ExecOp.h"
#include "Utility.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/util/Del.h"


CreateGestaltParse::CreateGestaltParse(TableName* _gestaltName, 
				       vector<IdentType>* _identTypePairs,
				       string* _fileName) : 
  gestaltName(*_gestaltName), identTypePairs(*_identTypePairs),
  fileName(*_fileName)
{
  delete _gestaltName;
  delete _identTypePairs;
  delete _fileName;
}

CreateGestaltParse::~CreateGestaltParse()
{
}

Iterator* CreateGestaltParse::createExec()
{
	OptExprList* lconst = new OptExprList;
	string s;
	char buf [100];
	sprintf (buf, "%d", (int) (identTypePairs.size()) );
	s = s + "\"" + gestaltName.fileName() + "\" Gestalt " + buf + " "; 
	
	vector<IdentType>::iterator i;
	for(i = identTypePairs.begin(); i != identTypePairs.end(); ++i)
	        s += i->first + " " + i->second + " ";
	s = s + fileName + " ;";

	lconst->push_back(new OptConstant(new DteStringAdt(s.length()),
                                          strdup(s.c_str())));
	Del<InsertParse> ip = new InsertParse(
          new TableName(gestaltName.dirName()), lconst);
	return ip -> createExec();
}

RegisterIntoGestaltParse::RegisterIntoGestaltParse(TableName* gestaltName, 
						   TableName* tableName)
  :  gestaltName(*gestaltName), tableName(*tableName)
{
  delete gestaltName;
  delete tableName;
}


Iterator* RegisterIntoGestaltParse::createExec()
{
        string s = tableName.toString();
	OptExprList* lconst = new OptExprList;
	lconst->push_back(new OptConstant(new DteStringAdt(s.length()),
                                          strdup(s.c_str())));

	Del<InsertParse> ip = new InsertParse(new TableName(gestaltName),
                                              lconst);
	return ip -> createExec ();
}

UnregisterFromGestaltParse::UnregisterFromGestaltParse(TableName* gestaltName, 
                                                       TableName* tableName)
  : gestaltName(*gestaltName), tableName(*tableName)
{
  delete gestaltName;
  delete tableName;
}


Iterator* UnregisterFromGestaltParse::createExec()
{
  cerr << "UnregisterFromGestaltParse::createExec is not yet implemented\n";
  return NULL;
}


