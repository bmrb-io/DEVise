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

const ISchema* ODBCInterface::getISchema(TableName* table){

	string Table_N=tableName ;


	cerr << "table = " << Table_N << endl;

	int aa;
	short Col_Max;    //Number of Columns

	TRY(aa = ODBC_Connect(),NULL);  //Connect to ODBC driver
	TRY(aa = ODBC_Stmt_Handle(),NULL);  //Allocate handle for statement

	string Query_String;
	Query_String = "select * from " + Table_N + ";";  //Create a query that retrieves data from all columns, where statement can be false

	SQL_Result = SQLExecDirect(Stmt_Handle,(UCHAR*)(Query_String.c_str()),SQL_NTS);  //Send this Query to ODBC
	TRY(ODBC_Error(SQL_Result,"Cannot send SQL to ODBC"),NULL);
	
	SQL_Result = SQLNumResultCols(Stmt_Handle,&Col_Max);   //Get number of Columns
	TRY(ODBC_Error(SQL_Result,"Cannot get number of columns from ODBC"),NULL);

	short* Col_Types;
	string* DTE_Types;
	string* Col_Attr_Names;
	SQLINTEGER* Col_Lengths;
	Col_Types = new short [Col_Max];
	DTE_Types = new string [Col_Max];
	Col_Attr_Names = new string [Col_Max];
	Col_Lengths = new SQLINTEGER [Col_Max];
	
	ODBC_Column_Info(Col_Types,Col_Attr_Names,Col_Lengths,Col_Max);
	
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

	ODBC_disConnect();   //Disconnect from ODBC

	return &tmp;
}

int ODBCInterface::ODBC_Connect() {
/*	dataSourceName = "devise_oracle";
	userName = "scott";
	passwd = "tiger"; */

	//Allocate Handle for ODBC
	SQL_Result = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&ODBC_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Handle for ODBC"),NULL);

	//Set ODBC Version to 2 (Our Oracle Driver is 2.5)
	SQL_Result = SQLSetEnvAttr(ODBC_Handle,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
	TRY(ODBC_Error(SQL_Result,"Cannot Set Version of ODBC"),NULL);

	//Allocate Handle for Connection
	SQL_Result = SQLAllocHandle(SQL_HANDLE_DBC,ODBC_Handle,&Connect_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Connect Handle for ODBC"),NULL);

	//Connect to ODBC 
	SQL_Result = SQLConnect(Connect_Handle,(UCHAR*)(dataSourceName.c_str()),SQL_NTS,(UCHAR*)(userName.c_str()),SQL_NTS,(UCHAR*)(passwd.c_str()),SQL_NTS);
	TRY(ODBC_Error(SQL_Result,"Cannot Connect to ODBC"),NULL);

	return 1;
}

void ODBCInterface::ODBC_disConnect() {

	if (Stmt_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_STMT,Stmt_Handle);
	SQL_Result = SQLDisconnect(Connect_Handle);
	if (Connect_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_DBC, Connect_Handle);
	if (ODBC_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_ENV,ODBC_Handle);
}

int ODBCInterface::ODBC_Stmt_Handle() {
	SQL_Result = SQLAllocHandle(SQL_HANDLE_STMT,Connect_Handle,&Stmt_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Statement Handle for ODBC"),NULL);

	return 1;
}


int ODBCInterface::ODBC_Error(SQLRETURN err_Stat,string err_msg) {
	if (err_Stat != SQL_SUCCESS) {
		THROW(new Exception(err_msg),NULL);
	}
	return 1;
}

void ODBCInterface::ODBC_Column_Info(short* types,string* attrs,SQLINTEGER* len, short col_Num) {
	short col_D_Len;  //Length of Column Name returned from database
	short col_Type;   // Column Type
	unsigned long col_Len;   // Length of Column
	short col_Dec;         //Precision of Column
	short col_Null_Ok;     //is NULL value allowed in Column ?
	SQLCHAR col_Name[40];  //Name of Column
	short i;

	for (i=0;i<col_Num;i++) {
		
		SQL_Result = SQLDescribeCol(Stmt_Handle,(i+1),col_Name,40,&col_D_Len,&col_Type,&col_Len,&col_Dec,&col_Null_Ok);
		ODBC_Error(SQL_Result,"Cannot get column properties from ODBC");

		attrs[i] = (char*)(col_Name);
		len[i] = col_Len;

		if ((col_Type == SQL_CHAR) || (col_Type == SQL_VARCHAR) || (col_Type == SQL_LONGVARCHAR)) {
			types[i] = SQL_C_CHAR;

		} else if ((col_Type == SQL_DECIMAL) || (col_Type == SQL_REAL) || (col_Type == SQL_FLOAT) || (col_Type == SQL_DOUBLE)) {
			types[i] = SQL_C_DOUBLE ;

		} else if ((col_Type == SQL_SMALLINT) || (col_Type == SQL_INTEGER) || (col_Type == SQL_TINYINT) || (col_Type == SQL_BIGINT)) {
			types[i] = SQL_C_SLONG;

		} else if ((col_Type == SQL_DATE) || (col_Type == SQL_TIME) || (col_Type == SQL_TIMESTAMP)) {
			types[i] = SQL_C_TIMESTAMP;

		} else if (col_Type == SQL_NUMERIC) {
			
			if (col_Dec == 0) {
				types[i] = SQL_C_SLONG;
			} else {
				types[i] = SQL_C_DOUBLE;
			}
		
		} else { 
			ODBC_Error(-1,"SQL Type not Defined in DTE");
		}
	}
}

Site* ODBCInterface::getSite(){
	return ODBCSite(tableName);
}
