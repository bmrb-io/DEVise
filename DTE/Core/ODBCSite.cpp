#include "ODBCSite.h"
#include "ODBCIterator.h"


Iterator* ODBCSite::createExec(){
	ostringstream tmp;
	tmp << "select ";
	displayList(tmp, mySelect, ", ");
	tmp << " from ";
	myFrom->rewind();
	tmp << tableName << " " << *myFrom->get()->getAlias();
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
		dataSourceName, userName, passwd, tableName, query, numFlds, types);
}
