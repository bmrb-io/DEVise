#ifndef ODBC_SITE_H
#define ODBC_SITE_H

#include "site.h"

class ODBCSite : public Site {
     string connectString;
	 string tableName;
public:
	ODBCSite(const string& connectString, const string& tableName) 
		: connectString(connectString), tableName(tableName) {}

	virtual Iterator* createExec();
};

#endif
