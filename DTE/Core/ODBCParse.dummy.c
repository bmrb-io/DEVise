#include "ParseTree.h"

DSNEntriesParse::DSNEntriesParse() {
}

Iterator* DSNEntriesParse::createExec() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

const ISchema* DSNEntriesParse::getISchema() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

DSNEntriesParse::~DSNEntriesParse() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}

ODBCTablesParse::ODBCTablesParse(string* DN) {
}

Iterator* ODBCTablesParse::createExec() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

const ISchema* ODBCTablesParse::getISchema() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

ODBCTablesParse::~ODBCTablesParse() {
}
