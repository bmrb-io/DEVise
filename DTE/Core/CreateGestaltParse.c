#include <string>

#include <stdlib.h>
#include <sys/types.h>

#include "ParseTree.h"
#include "catalog.h"
//#include "ExecOp.h"
#include "Utility.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/util/Del.h"


CreateGestaltParse::CreateGestaltParse(TableName* _gestaltName, 
				       vector<IdentType>* _identTypePairs) :
  identTypePairs(*_identTypePairs), gestaltName (*_gestaltName)
{
  delete _gestaltName;
  delete _identTypePairs;
}

CreateGestaltParse::~CreateGestaltParse()
{
}

Iterator* CreateGestaltParse::createExec()
{
        string nameStr = gestaltName.fileName();
        string materFile = DTE_ENV_VARS.valueOf(DTE_ENV_VARS.materViewDir);
        if(materFile.empty()){
                string err = "Please set the env var '"
                        + DTE_ENV_VARS.materViewDir + "' to point to a directory";
                THROW(new Exception(err), NULL);
        }
        materFile += "/" + nameStr;

	// create empty file
	ofstream (materFile.c_str(), ios::out).close ();
	
	cerr << "Create Gestalt member file: " << materFile << endl;

	OptExprList* lconst = new OptExprList;
	string s;
	char buf [100];
	sprintf (buf, "%d", (int) (identTypePairs.size()) );
	s = s + "\"" + gestaltName.fileName() + "\" Gestalt " + buf + " "; 
	
	vector<IdentType>::iterator i;
	for(i = identTypePairs.begin(); i != identTypePairs.end(); ++i)
	        s += i->first + " " + i->second + " ";
	s = s + materFile + " ;";

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
  // Insert into file instead of using "insert into ..." command,
  // This saves schema checking trouble. YL
  string s = tableName.toString ();
  Del<Interface> interface (ROOT_CATALOG.createInterface (gestaltName));
  assert (interface);
  if (! (interface->isGestalt ()))
    {
      string msg = gestaltName.toString () + " is not a gestalt";
      cerr << msg << endl;
      THROW (new Exception (msg), NULL);
    }
  
  string path = ((GestaltInterface*)(Interface*)interface) -> getUrlString ();
  ofstream f (path.c_str (), ios::app);
  f << s.c_str () << endl;
  f.close ();
  
  return NULL;

  /* YL
        string s = tableName.toString();
	OptExprList* lconst = new OptExprList;
	lconst->push_back(new OptConstant(new DteStringAdt(s.length()),
                                          strdup(s.c_str())));

	Del<InsertParse> ip = new InsertParse(new TableName(gestaltName),
                                              lconst);
	return ip -> createExec ();
  */
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
  Del<Interface> interface (ROOT_CATALOG.createInterface (gestaltName));
  assert (interface);
  if (! (interface->isGestalt ()))
    {
      string msg = gestaltName.toString () + " is not a gestalt";
      cerr << msg << endl;
      THROW (new Exception (msg), NULL);
    }
  
  string path = ((GestaltInterface*)(Interface*)interface) -> getUrlString ();
  vector<string> v = ((GestaltInterface*)(Interface*)interface) -> getMemberNames ();
  
  ofstream f (path.c_str (), ios::out|ios::trunc);
  for (int i = 0; i < v.size (); i ++)
    {
      if (strcmp (v[i].c_str (), tableName.toString ().c_str ()))
	f << v[i] << endl;
    }
  
  f.close ();

  return NULL;
}


