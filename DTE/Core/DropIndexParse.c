#include "ParseTree.h"
#include <assert.h>
#include "Engine.h"

Site* DropIndexParse::createSite(){
	String query = String("delete .sysind as t where t.table = \"") +
		tableName->toString() + "\" and t.name = \"" + *indexName + "\"";
	cout << "in drop index with query:\n" << query << endl;
	Engine engine(query);
     TRY(engine.optimize(), NULL);
	return new Site();
}
