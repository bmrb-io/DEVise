#include "Interface.h"

//#define TEST_ODBC

#ifdef TEST_ODBC
	#include "ODBCSite.h"
#endif

const ISchema* ODBCInterface::getISchema(){
#ifdef TEST_ODBC
	return &DIR_SCHEMA;
#else
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
#endif
}

#if 0
Site* ODBCInterface::getSite(){
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}
#endif

ODBCInterface::~ODBCInterface() {}
