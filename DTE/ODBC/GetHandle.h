#ifndef GETHANDLE_H
#define GETHANDLE_H

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include "SQL.H"
#include <sqlext.h>
#include <iostream>
#include "exception.h"

using namespace std;


class ConnectHandle {
public:
	HENV ODBC_Handle;
	HDBC Connect_Handle;
};

class ODBCGetHandle {
	map <string,ConnectHandle*> myHandle;
public:
	ODBCGetHandle() {} ;
	ConnectHandle* getHandle(const string& connectString);
	void Handle_Error(SQLRETURN err_Stat,string err_msg);
	~ODBCGetHandle();
};

#endif
