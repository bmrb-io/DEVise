#include "types.h"
#include "Interface.h"
#include "site.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "Engine.h"
#include "catalog.h"
#include <string>

const ISchema* ODBCInterface::getISchema(TableName* table){
	
	cerr << "ODBC driver is not installed" << endl;
	exit(1);

}
