#ifndef ODBC_SITE_H
#define ODBC_SITE_H

#include "site.h"

class ODBCSite : public Site {
     string dataSourceName;
     string userName;
     string passwd;
	string tableName;
public:
	ODBCSite(const string& dataSourceName, const string& userName,
		const string& passwd, const string& tableName) 
		: dataSourceName(dataSourceName), userName(userName),
		passwd(passwd), tableName(tableName) {}

	virtual Iterator* createExec();
};

#endif
