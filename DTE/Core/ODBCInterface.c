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

const ISchema* ODBCInterface::getISchema(TableName* table){

	string Table_N ;
	Table_N = table->toString();

	cerr << "table = " << Table_N << endl;

	Table_N.erase(0,1);   //Get rid of .

	if(table->isEmpty()){
		return &DIR_SCHEMA;
	}

	int aa;
	short col_Num;    //Number of Columns
	short col_D_Len;  //Length of Column Name returned from database
	short col_Type;   // Column Type
	unsigned long col_Len;   // Length of Column
	short col_Dec;         //Precision of Column
	short col_Null_Ok;     //is NULL value allowed in Column ?
	SQLCHAR col_Name[40];  //Name of Column

	TRY(aa = ODBC_Connect(),NULL);  //Connect to ODBC driver
	TRY(aa = ODBC_Stmt_Handle(),NULL);  //Allocate handle for statement

	string Query_String;
	Query_String = "select * from " + Table_N + ";";  //Create a query that retrieves data from all columns, where statement can be false

	SQL_Result = SQLExecDirect(Stmt_Handle,(UCHAR*)(Query_String.c_str()),SQL_NTS);  //Send this Query to ODBC
	if (SQL_Result != SQL_SUCCESS) {
		string msg_ed = "Cannot send SQL to ODBC" ;
		THROW(new Exception(msg_ed),NULL);
	}
	
	SQL_Result = SQLNumResultCols(Stmt_Handle,&col_Num);   //Get number of Columns
	if (SQL_Result != SQL_SUCCESS) {
		string msg_cn = "Cannot get number of columns from ODBC" ;
		THROW(new Exception(msg_cn),NULL);
	}
	string* types;
	string* attrs;
	types = new string [col_Num];
	attrs = new string [col_Num];
	for (short i=0;i<col_Num;i++) {
		
		SQL_Result = SQLDescribeCol(Stmt_Handle,(i+1),col_Name,40,&col_D_Len,&col_Type,&col_Len,&col_Dec,&col_Null_Ok);
		if (SQL_Result != SQL_SUCCESS) {
			string msg_ca = "Cannot get column properties from ODBC" ;
			THROW(new Exception(msg_ca),NULL);
		}
		types[i] = (char*)(col_Name);
		if ((col_Type == SQL_CHAR) || (col_Type == SQL_VARCHAR) || (col_Type == SQL_LONGVARCHAR)) {
			ostringstream os;
			os << col_Len << ends;
			attrs[i] = "string" + os.str();
		} else if ((col_Type == SQL_DECIMAL) || (col_Type == SQL_REAL) || (col_Type == SQL_FLOAT) || (col_Type == SQL_DOUBLE)) {
			attrs[i] = "double";
		} else if ((col_Type == SQL_SMALLINT) || (col_Type == SQL_INTEGER) || (col_Type == SQL_TINYINT) || (col_Type == SQL_BIGINT)) {
			attrs[i] = "int";
		} else if ((col_Type == SQL_DATE) || (col_Type == SQL_TIME) || (col_Type == SQL_TIMESTAMP)) {
			attrs[i] = "datetime";
		} else if (col_Type == SQL_NUMERIC) {
			if (col_Dec == 0) {
				attrs[i] = "int";
			} else {
				attrs[i] = "double";
			}
		} else { 
			cerr << col_Type << " " << col_Name << endl ;
			string msg_tnd = "SQL Type not Defined in DTE" ;
			THROW(new Exception(msg_tnd),NULL);
		}
	}
	tmp = ISchema(col_Num, types, attrs);

	ODBC_disConnect();   //Disconnect from ODBC

	return &tmp;
}

int ODBCInterface::ODBC_Connect() {
	dataSourceName = "devise_oracle";
	userName = "scott";
	passwd = "tiger";

	//Allocate Handle for ODBC
	SQL_Result = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&ODBC_Handle);
	if (SQL_Result != SQL_SUCCESS) {
		string msg_nh = "Cannot Allocate Handle for ODBC" ;
		THROW(new Exception(msg_nh),NULL);
	}

	//Set ODBC Version to 2 (Our Oracle Driver is 2.5)
	SQL_Result = SQLSetEnvAttr(ODBC_Handle,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
	if (SQL_Result != SQL_SUCCESS) {
		string msg_nv = "Cannot Set Version for ODBC" ;
		THROW(new Exception(msg_nv),NULL);
	}

	//Allocate Handle for Connection
	SQL_Result = SQLAllocHandle(SQL_HANDLE_DBC,ODBC_Handle,&Connect_Handle);
	if (SQL_Result != SQL_SUCCESS) {
		string msg_ch = "Cannot Allocate Connect Handle for ODBC" ;
		THROW(new Exception(msg_ch),NULL);
	}


	//Connect to ODBC 
	SQL_Result = SQLConnect(Connect_Handle,(UCHAR*)(dataSourceName.c_str()),SQL_NTS,(UCHAR*)(userName.c_str()),SQL_NTS,(UCHAR*)(passwd.c_str()),SQL_NTS);
	if (SQL_Result != SQL_SUCCESS) {
		string msg_nc = "Cannot Connect to ODBC" ;
		THROW(new Exception(msg_nc),NULL);
	}
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
	if (SQL_Result != SQL_SUCCESS) {
		string msg_sh = "Cannot Allocate Statement Handle for ODBC" ;
		THROW(new Exception(msg_sh),NULL);
	}
	return 1;
}
