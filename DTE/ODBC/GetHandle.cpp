#include "GetHandle.h"


ConnectHandle* ODBCGetHandle::getHandle(const string& dataSourceName,const string& userName,const string& passwd) {

	string temComp = dataSourceName + userName + passwd ;
	if (myHandle[temComp]) {
		return myHandle[temComp];
	} else {
		ConnectHandle* newHandle;
		newHandle = new ConnectHandle;
		SQLRETURN SQL_Result;

		//Allocate Handle for ODBC
		SQL_Result = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&(newHandle->ODBC_Handle));
		TRY(Handle_Error(SQL_Result,"Cannot Allocate Handle for ODBC"),NULL);

		//Set ODBC Version to 2 (Our Oracle Driver is 2.5)
		SQL_Result = SQLSetEnvAttr(newHandle->ODBC_Handle,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
		TRY(Handle_Error(SQL_Result,"Cannot Set Version of ODBC"),NULL);

		//Allocate Handle for Connection
		SQL_Result = SQLAllocHandle(SQL_HANDLE_DBC,newHandle->ODBC_Handle,&(newHandle->Connect_Handle));
		TRY(Handle_Error(SQL_Result,"Cannot Allocate Connect Handle for ODBC"),NULL);

		//Connect to ODBC 
		SQL_Result = SQLConnect(newHandle->Connect_Handle,(UCHAR*)(dataSourceName.c_str()),SQL_NTS,(UCHAR*)(userName.c_str()),SQL_NTS,(UCHAR*)(passwd.c_str()),SQL_NTS);
		TRY(Handle_Error(SQL_Result,"Cannot Connect to ODBC"),NULL);

		myHandle[temComp] = newHandle;
		return newHandle;
	}
}

void ODBCGetHandle::Handle_Error(SQLRETURN err_Stat,string err_msg) {
	if (err_Stat != SQL_SUCCESS) {
		THROW(new Exception(err_msg),NVOID);
	}
}

ODBCGetHandle::~ODBCGetHandle() {
	SQLRETURN SQL_Result;
	map <string, ConnectHandle*>::iterator CI;
	for (CI = myHandle.begin(); CI != myHandle.end(); CI++){

		SQL_Result = SQLDisconnect(((CI->second)->Connect_Handle));
		TRY(Handle_Error(SQL_Result,"Can't Disconnect from ODBC"),NVOID)

		if (((CI->second)->Connect_Handle) != SQL_NULL_HANDLE) {
			SQL_Result = SQLFreeHandle(SQL_HANDLE_DBC, ((CI->second)->Connect_Handle));
			TRY(Handle_Error(SQL_Result,"Can't free Connection Handle of ODBC"),NVOID);
		}

		if (((CI->second)->ODBC_Handle) != SQL_NULL_HANDLE) {
			SQL_Result = SQLFreeHandle(SQL_HANDLE_ENV,((CI->second)->ODBC_Handle));
			TRY(Handle_Error(SQL_Result,"Can't free Environment Handle of ODBC"),NVOID);
		}

		delete CI->second;
	}
}
