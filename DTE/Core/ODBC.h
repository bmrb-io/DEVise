#ifndef ODBC_H
#define ODBC_H

#include "exception.h"
#include <string>
#include "sysdep.h"
#include "types.h"

#if defined(_WINDOWS) || defined(_CONSOLE)
	#include <windows.h>
	#include "SQL.H"
	#include <sqlext.h>
#endif

#ifndef __GNUG__
using namespace std;
#endif

class ODBC_Data {
	
	string dataSourceName;		//Name of ODBC Data Source
	string userName;			//Username for Database
	string passwd;				//Password for Database
	string query;				//Query String (SQL Statement)
	ADTCopyPtr* adtCopyPtrs;	//Needed for Iterator


#if defined(_WINDOWS) || defined(_CONSOLE)
	HENV ODBC_Handle;
	HDBC Connect_Handle;
	SQLHSTMT Stmt_Handle;
	SQLRETURN SQL_Result;
#endif

public:
	short* types;				//C Types for columns (SQL_C_CHAR, etc.)
	short col_Num;				//Number of Columns
	string* attrs;				//Names of Columns
	string* DTE_Types;			//DTE types for Columns  (string, date, etc.)

	ODBC_Data(const string& dSN, const string& uN,const string& pw,const string& qry) {
	dataSourceName = dSN ;
	userName = uN;
	passwd = pw;
	query = qry;
	}

	void ODBC_Connect();
	void ODBC_disConnect();
	void ODBC_Stmt_Handle();
	int ODBC_Get_Rec(Tuple* results);	//Get Next Tuple from database, return -1 if EOF
	
	#if defined(_WINDOWS) || defined(_CONSOLE)
		SQLINTEGER* len;				//length of Columns
		void ODBC_Error(SQLRETURN err_Stat,string err_msg);	//Error Handler (Just THROW)
		void ODBC_Column_Info();		//Get Column info (types,etc.) for given query
	#endif

	void ODBC_Get_ColNums();			//Get Number of Columns and allocate space for related arrays
	virtual ~ODBC_Data(){
		delete [] adtCopyPtrs;
		delete [] types;
		delete [] attrs;
		delete [] DTE_Types;
	#if defined(_WINDOWS) || defined(_CONSOLE)
		delete [] len;
	#endif
	};
};

#endif
