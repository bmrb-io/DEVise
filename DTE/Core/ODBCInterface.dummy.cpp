#include "types.h"
#include "Interface.h"
#include "site.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "Engine.h"
#include "catalog.h"
#include <string>

#define TEST_ODBC

#ifdef TEST_ODBC
	#include "ODBCSite.h"
#endif

const ISchema* ODBCInterface::getISchema(TableName* table){
#ifdef TEST_ODBC
	return &DIR_SCHEMA;
#else
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
#endif
}

Site* ODBCInterface::getSite(){
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}
