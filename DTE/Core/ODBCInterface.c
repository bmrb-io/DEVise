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

	string Table_N=tableName ;


	cerr << "table = " << Table_N << endl;

	int aa;
	short Col_Max;    //Number of Columns

	myODBC = new ODBC_Data(dataSourceName,userName,passwd,Table_N);

	TRY(aa = myODBC->ODBC_Connect(),NULL);  //Connect to ODBC driver
	TRY(aa = myODBC->ODBC_Stmt_Handle(),NULL);  //Allocate handle for statement

	Col_Max = TRY(myODBC->ODBC_Get_ColNums(),NULL);  //Get Number of Columns
	
	short* Col_Types;
	string* DTE_Types;
	string* Col_Attr_Names;
	SQLINTEGER* Col_Lengths;
	Col_Types = new short [Col_Max];
	DTE_Types = new string [Col_Max];
	Col_Attr_Names = new string [Col_Max];
	Col_Lengths = new SQLINTEGER [Col_Max];

	TRY(myODBC->ODBC_Column_Info(Col_Types,Col_Attr_Names,Col_Lengths,Col_Max),NULL);
	
	for (aa = 0; aa < Col_Max;aa++) {
		if (Col_Types[aa] == SQL_C_CHAR) {
			ostringstream os;
			os << Col_Lengths[aa] << ends;
			DTE_Types[aa] = "string" + os.str();
		} 
		if (Col_Types[aa] == SQL_C_DOUBLE) {
			DTE_Types[aa] = "double" ;
		}
		if (Col_Types[aa] == SQL_C_SLONG) {
			DTE_Types[aa] = "int" ;
		}
		if (Col_Types[aa] == SQL_C_TIMESTAMP) {
			DTE_Types[aa] = "date" ;
		}
	}

	tmp = ISchema(Col_Max, DTE_Types, Col_Attr_Names);

	TRY(myODBC->ODBC_disConnect(),NULL);   //Disconnect from ODBC

	return &tmp;
}

Site* ODBCInterface::getSite(){
	return new ODBCSite(dataSourceName, userName, passwd, tableName);
}
