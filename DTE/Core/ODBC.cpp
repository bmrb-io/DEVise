#include <string>
#include <windows.h>
#include "SQL.H"
#include <sqlext.h>
#include "ODBC.h"
#include "exception.h"
#include "sysdep.h"
#include "DateTime.h"
#include "types.h"

void ODBC_Data::ODBC_Connect() {
	
	//Allocate Handle for ODBC
	SQL_Result = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&ODBC_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Handle for ODBC"),NVOID);

	//Set ODBC Version to 2 (Our Oracle Driver is 2.5)
	SQL_Result = SQLSetEnvAttr(ODBC_Handle,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
	TRY(ODBC_Error(SQL_Result,"Cannot Set Version of ODBC"),NVOID);

	//Allocate Handle for Connection
	SQL_Result = SQLAllocHandle(SQL_HANDLE_DBC,ODBC_Handle,&Connect_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Connect Handle for ODBC"),NVOID);

	//Connect to ODBC 
	SQL_Result = SQLConnect(Connect_Handle,(UCHAR*)(dataSourceName.c_str()),SQL_NTS,(UCHAR*)(userName.c_str()),SQL_NTS,(UCHAR*)(passwd.c_str()),SQL_NTS);
	TRY(ODBC_Error(SQL_Result,"Cannot Connect to ODBC"),NVOID);

} 

void ODBC_Data::ODBC_disConnect() {

	if (Stmt_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_STMT,Stmt_Handle);
	SQL_Result = SQLDisconnect(Connect_Handle);
	if (Connect_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_DBC, Connect_Handle);
	if (ODBC_Handle != SQL_NULL_HANDLE) 
		SQLFreeHandle(SQL_HANDLE_ENV,ODBC_Handle);
}

void ODBC_Data::ODBC_Stmt_Handle() {
	SQL_Result = SQLAllocHandle(SQL_HANDLE_STMT,Connect_Handle,&Stmt_Handle);
	TRY(ODBC_Error(SQL_Result,"Cannot Allocate Statement Handle for ODBC"),NVOID);

}


void ODBC_Data::ODBC_Error(SQLRETURN err_Stat,string err_msg) {
	if (err_Stat != SQL_SUCCESS) {
		THROW(new Exception(err_msg),NVOID);
	}
}

void ODBC_Data::ODBC_Column_Info() {

	short col_D_Len;  //Length of Column Name returned from database
	short col_Type;   // Column Type
	unsigned long col_Len;   // Length of Column
	short col_Dec;         //Precision of Column
	short col_Null_Ok;     //is NULL value allowed in Column ?
	SQLCHAR col_Name[40];  //Name of Column
	short i;

	TRY(ODBC_Get_ColNums(),NVOID);		//Get Number of Columns for given query
	adtCopyPtrs = new ADTCopyPtr[col_Num];

	for (i=0;i<col_Num;i++) {
		
		SQL_Result = SQLDescribeCol(Stmt_Handle,(i+1),col_Name,40,&col_D_Len,&col_Type,&col_Len,&col_Dec,&col_Null_Ok);
		TRY(ODBC_Error(SQL_Result,"Cannot get column properties from ODBC"),NVOID);

		attrs[i] = (char*)(col_Name);
		len[i] = col_Len;

		if ((col_Type == SQL_CHAR) || (col_Type == SQL_VARCHAR) || (col_Type == SQL_LONGVARCHAR)) {
			ostringstream os;
			os << col_Len << ends;
			DTE_Types[i] = "string" + os.str();
			types[i] = SQL_C_CHAR;
			adtCopyPtrs[i] = getADTCopyPtr(STRING_TP);

		} else if ((col_Type == SQL_DECIMAL) || (col_Type == SQL_REAL) || (col_Type == SQL_FLOAT) || (col_Type == SQL_DOUBLE)) {
			types[i] = SQL_C_DOUBLE ;
			DTE_Types[i] = "double";
			adtCopyPtrs[i] = getADTCopyPtr(DOUBLE_TP);

		} else if ((col_Type == SQL_SMALLINT) || (col_Type == SQL_INTEGER) || (col_Type == SQL_TINYINT) || (col_Type == SQL_BIGINT)) {
			types[i] = SQL_C_SLONG;
			DTE_Types[i] = "int";
			adtCopyPtrs[i] = getADTCopyPtr(INT_TP);

		} else if ((col_Type == SQL_DATE) || (col_Type == SQL_TIME) || (col_Type == SQL_TIMESTAMP)) {
			types[i] = SQL_C_TIMESTAMP;
			DTE_Types[i] = "date";
			adtCopyPtrs[i] = getADTCopyPtr(DATE_TP);

		} else if (col_Type == SQL_NUMERIC) {
			
			if (col_Dec == 0) {
				types[i] = SQL_C_SLONG;
				DTE_Types[i] = "int";
				adtCopyPtrs[i] = getADTCopyPtr(INT_TP);
			} else {
				types[i] = SQL_C_DOUBLE;
				DTE_Types[i] = "double";
				adtCopyPtrs[i] = getADTCopyPtr(DOUBLE_TP);
			}
		
		} else { 
			TRY(ODBC_Error(-1,"SQL Type not Defined in DTE"),NVOID);
		}
	}
}

void ODBC_Data::ODBC_Get_ColNums() {
	short numCols;

	SQL_Result = SQLExecDirect(Stmt_Handle,(UCHAR*)(query.c_str()),SQL_NTS);  //Send this Query to ODBC
	TRY(ODBC_Error(SQL_Result,"Cannot send SQL to ODBC"),NVOID);

	SQL_Result = SQLNumResultCols(Stmt_Handle,&numCols);   //Get number of Columns
	TRY(ODBC_Error(SQL_Result,"Cannot get number of columns from ODBC"),NVOID);
	col_Num = numCols;
	types = new short[col_Num];
	attrs = new string[col_Num];
	DTE_Types = new string[col_Num];
	len = new SQLINTEGER[col_Num];
	return;
}



int ODBC_Data::ODBC_Get_Rec(Tuple* results) {

	size_t size;
	SQLINTEGER Data_Status;
	short i;
	
	SQL_Result = SQLFetch(Stmt_Handle) ;
	if (SQL_Result == SQL_NO_DATA) 
		return(-1);

	TRY(ODBC_Error(SQL_Result,"Cannot fetch results"),NULL);
	for (i=0;i<col_Num;i++) {
		size = len[i] + 1;
		SQL_Result = SQLGetData(Stmt_Handle,(i+1),types[i],results[i],size,&Data_Status);
		TRY(ODBC_Error(SQL_Result,"Cannot retrieve data from ODBC"),NULL);

		if (Data_Status != SQL_NULL_DATA) {
			if (!((types[i] == SQL_C_CHAR) || (types[i] == SQL_C_SLONG) || (types[i] == SQL_C_DOUBLE) || (types[i] == SQL_C_TIMESTAMP))) {
				TRY(ODBC_Error(-1,"Can't Find DataType"),NULL);
			} 

			if (types[i] == SQL_C_TIMESTAMP) {
				TIMESTAMP_STRUCT date = *((TIMESTAMP_STRUCT*)results[i]) ;
				EncodedDTF date_DTE(DateTime(0,date.year, date.month, date.day, date.hour, date.minute, date.second,0));
				adtCopyPtrs[i]((&date_DTE), results[i]);
			}
		}
	}
	return(1);
}