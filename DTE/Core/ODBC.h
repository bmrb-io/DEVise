#ifndef ODBC_H
#define ODBC_H

#include "exception.h"
#include <string>
#include "sysdep.h"

#if defined(_WINDOWS) || defined(_CONSOLE)
	#include <windows.h>
	#include "SQL.H"
	#include <sqlext.h>
#endif

#ifndef __GNUG__
using namespace std;
#endif

class ODBC_Data {
	
	string dataSourceName;
	string userName;
	string passwd;
	string tableName;


#if defined(_WINDOWS) || defined(_CONSOLE)
	HENV ODBC_Handle;
	HDBC Connect_Handle;
	SQLHSTMT Stmt_Handle;
	SQLRETURN SQL_Result;
#endif

public:
	ODBC_Data(const string& dSN, const string& uN,const string& pw,const string& tN) {

	dataSourceName = dSN ;
	userName = uN;
	passwd = pw;
	tableName = tN;
	
	}

	int ODBC_Connect();
	void ODBC_disConnect();
	int ODBC_Stmt_Handle();

	#if defined(_WINDOWS) || defined(_CONSOLE)
		int ODBC_Get_Rec(short* types,SQLINTEGER* len,void** results,short col_Num);
		int ODBC_Error(SQLRETURN err_Stat,string err_msg);
		void ODBC_Column_Info(short* types,string* attrs,SQLINTEGER* len, short col_Num);
	#endif

	short ODBC_Get_ColNums();
	virtual ~ODBC_Data(){};
};

#endif
