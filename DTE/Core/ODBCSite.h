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

	virtual Iterator* createExec(){
		ostrstream tmp;
		tmp << "select ";
		displayList(tmp, mySelect, ", ");
		tmp << " from ";
		myFrom->rewind();
		tmp << tableName << *myFrom->get()->getAlias();
		if(!myWhere->isEmpty()){
			tmp << " where ";
			displayList(tmp, myWhere, " and ");
		}
		tmp << ends;
		string query = tmp.str();
		cerr << "query is: " << query << endl;
		int numFlds = getNumFlds();
		const TypeID* types = getTypeIDs();
		return new ODBCIterator(
			dataSourceName, userName, passwd, query, numFlds, types);
	}
};

#endif
