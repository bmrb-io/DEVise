#include <string>
#include <windows.h>
#include "SQL.H"
#include <sqlext.h>
#include "ODBC.h"
#include "exception.h"
#include "sysdep.h"
#include "DateTime.h"
#include "types.h"

ODBCGetHandle myHandles;

void ODBC_Data::ODBC_Connect() {

TRY(ConnectHandle* tempHand=myHandles.getHandle(connectString),NVOID);
ODBC_Handle = tempHand->ODBC_Handle;
Connect_Handle = tempHand->Connect_Handle;
	
SQL_Result = SQLAllocHandle(SQL_HANDLE_STMT,Connect_Handle,&Stmt_Handle);
TRY(ODBC_Error(SQL_Result,"Cannot Allocate Statement Handle for ODBC"),NVOID);
TRY(checkSpecific(),NVOID);

} 

void ODBC_Data::ODBC_Error(SQLRETURN err_Stat,string err_msg) {
	if (err_Stat != SQL_SUCCESS) {
		THROW(new Exception(err_msg),NVOID);
	}
}
void ODBC_Data::checkSpecific() {
	
	SQLPOINTER dbtype ;
	dbtype = new UCHAR[100];
	short int retlen;
	SQL_Result = SQLGetInfo(Connect_Handle,SQL_DBMS_VER,dbtype,100,&retlen); //GET DB TYPE

	TRY(ODBC_Error(SQL_Result,"Cannot get database type from ODBC"),NVOID);

	int wtype;
	wtype = memcmp(dbtype,(UCHAR*)("SQL Server"),10); //Check if it is Sybase

	if (wtype == 0) {
		
		UCHAR* tempstm = (unsigned char*)("set quoted_identifier on");
		SQL_Result = SQLExecDirect(Stmt_Handle,tempstm,SQL_NTS); //Set quoted_identifier)
		TRY(ODBC_Error(SQL_Result,"Can't sent DB Specific attribute for ODBC"),NVOID);
		
		SQL_Result = SQLFreeHandle(SQL_HANDLE_STMT,Stmt_Handle);  //Free Current Handle
		TRY(ODBC_Error(SQL_Result,"Can't free Statement Handle of ODBC"),NVOID);

		SQL_Result = SQLAllocHandle(SQL_HANDLE_STMT,Connect_Handle,&Stmt_Handle);  //Get New Handle
		TRY(ODBC_Error(SQL_Result,"Cannot Allocate Statement Handle for ODBC"),NVOID);

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

	//	SQLFreeStmt(Stmt_Handle, SQL_UNBIND);   //Free Priveous Handle if exists
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



int ODBC_Data::ODBC_Get_Rec(Type** results) {

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
		} else {
			if (types[i] == SQL_C_SLONG) 
				*((int*)results[i]) = NULL;
			if (types[i] == SQL_C_DOUBLE) 
				*((double*)results[i]) = NULL;
			if (types[i] == SQL_C_CHAR) 
				*((char*)results[i]) = NULL; 
			if (types[i] == SQL_C_TIMESTAMP) {
				TIMESTAMP_STRUCT tDate;
				tDate.year = 1; tDate.month = 1; tDate.day = 1;
				tDate.hour = 0; tDate.minute = 0; tDate.second = 0;
				*((TIMESTAMP_STRUCT*)results[i]) = tDate;
			}
		}
	}
	return(1);
}

ODBC_Data::~ODBC_Data(){
	if (Stmt_Handle != SQL_NULL_HANDLE) {
		SQL_Result = SQLFreeHandle(SQL_HANDLE_STMT,Stmt_Handle);
		TRY(ODBC_Error(SQL_Result,"Can't free Statement Handle of ODBC"),NVOID);
	} 
	delete [] adtCopyPtrs;
	delete [] types;
	delete [] attrs;
	delete [] DTE_Types;
	delete [] len;
}
