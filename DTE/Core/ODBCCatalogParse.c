#include "ParseTree.h"
#include <assert.h>
#include "Engine.h"
#include "ODBCInfo.h"

Iterator* ODBCTableAddParse::createExec() {

	string query = "insert into . values(\'\"" + DTE_Table_Name
          + "\" ODBC " + DSN_Name + " " + ODBC_Table_Name + " ;\')" ;

	Engine engine(query);
	TRY(engine.optimize(),NULL); //kb: this doesn't run the query!?
	return NULL;
}

Iterator* ODBCDSNAddParse::createExec() {

	ODBCInfo myInfo;
	myInfo.dsnInsert(DSN_Name, DSN_Info);

	return NULL;
}

Iterator* ODBCTableDeleteParse::createExec() {
	string query = "delete . as t where t.name = \'" + DTE_Table_Name +"\'";

	Engine engine(query);
	TRY(engine.optimize(),NULL); //kb: this doesn't run the query!?
	return NULL;
}

Iterator* ODBCDSNDeleteParse::createExec() {

	ODBCInfo myInfo;
	myInfo.dsnDelete(DSN_Name);

	return NULL;
}
