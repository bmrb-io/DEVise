#include "types.h"
#include "Interface.h"
#include "site.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "Engine.h"
#include "catalog.h"
#include <string>
#include <windows.h>
#include "SQL.H"
#include <sqlext.h>
#include "ODBCSite.h"
#include "ODBC.h"

const ISchema* ODBCInterface::getISchema(TableName* table){

	string myQuery = "Select * from " + tableName + " where 1=2"; //construct query for getting info, you can retrieve column info by sending a query that returns nothing

	cerr << "table = " << tableName << endl;

	myODBC = new ODBC_Data(connectString,myQuery);
	ODBC_Exist = 1;

	TRY(myODBC->ODBC_Connect(),NULL);  //Connect to ODBC driver

	TRY(myODBC->ODBC_Column_Info(),NULL);
	
	tmp = ISchema(myODBC->col_Num, myODBC->DTE_Types, myODBC->attrs);

	return &tmp;
}

Site* ODBCInterface::getSite(){
	return new ODBCSite(connectString , tableName);
}

ODBCInterface::~ODBCInterface(){
	if (ODBC_Exist > 0)
		delete myODBC;
}