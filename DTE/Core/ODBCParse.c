#include "ParseTree.h"
#include <assert.h>
#include "Engine.h"
#include "ODBCInfo.h"
#include "ODBCList.h"
#include "types.h"
#include "Iterator.h"

Iterator* ODBCTableAddParse::createExec() {

	string query = "insert into . values(\'\"" + *DTE_Table_Name + "\" ODBC " + *DSN_Name + " " + *ODBC_Table_Name + " ;\')" ;

	Engine engine(query);
	TRY(engine.optimize(), NULL);
	return NULL;
}

Iterator* ODBCDSNAddParse::createExec() {

	ODBCInfo myInfo;
	myInfo.dsnInsert(*DSN_Name, *DSN_Info);

	return NULL;
}

Iterator* ODBCTableDeleteParse::createExec() {
	string query = "delete . as t where t.name = \'" + *DTE_Table_Name + "\'" ;

	Engine engine(query);
	TRY(engine.optimize(),NULL);
	return NULL;
}

Iterator* ODBCDSNDeleteParse::createExec() {

	ODBCInfo myInfo;
	myInfo.dsnDelete(*DSN_Name);

	return NULL;
}

DSNEntriesParse::DSNEntriesParse() {
	myIterator = new DSNListIterator();
}

Iterator* DSNEntriesParse::createExec() {
	return myIterator;
}

const ISchema* DSNEntriesParse::getISchema() {
	int nCol;
	TypeID* fTypes;
	string* attrNames;
	((DSNListIterator*)myIterator)->getInfo(nCol, fTypes, attrNames);
	return new ISchema(nCol, fTypes, attrNames);
}

DSNEntriesParse::~DSNEntriesParse() {
	delete myIterator;
}

ODBCTablesParse::ODBCTablesParse(string* DN) {
	myIterator = new TableListIterator(*DN);
}

Iterator* ODBCTablesParse::createExec() {
	return myIterator;
}

const ISchema* ODBCTablesParse::getISchema() {
	int nCol;
	TypeID* fTypes;
	string* attrNames;
	((TableListIterator*)myIterator)->getInfo(nCol, fTypes, attrNames);
	return new ISchema(nCol, fTypes, attrNames);
}

ODBCTablesParse::~ODBCTablesParse() {
	delete myIterator;
}
