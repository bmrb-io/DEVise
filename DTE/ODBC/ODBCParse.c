#include "ParseTree.h"
#include <assert.h>
#include "ODBCInfo.h"
#include "ODBCList.h"
#include "types.h"
#include "Iterator.h"

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
