#include "types.h"
#include "StandardRead.h"
#include "AccessMethod.h"
#include "myopt.h"
#include "catalog.h" 	// for root catalog
#include "ExecOp.h"
#include "Interface.h"

Iterator* FileScan::createExec() const {
	assert(!"not implemented");
	return NULL;
}

FileScan::FileScan(const NewStat& stat)
{
//	cost = stat.getNumPgs();
//	cost = TABLE_CARD * NUM_FLDS * FIELD_SZ / PAGE_SZ;
}

Cost FileScan::getCost() const
{
	return cost;
}

StandardAM::StandardAM
	(const ISchema& schema, const string& url, const Stats& stats) : schemaM(schema), url(url)
{
}

Cost StandardAM::getCost() const
{
	// fix this later

	return 0;
}

Iterator* StandardAM::createExec() const
{
	TypeIDList typeIDs = schemaM.getTypeIDs();
	return new StandReadExec(typeIDs, url);
}

vector<BaseSelection*> StandardAM::getProjectList(const string& alias) const
{
	vector<BaseSelection*> retVal;
	const string* atts = schemaM.getAttributeNames();
	for(int i = 0; i < schemaM.getNumFlds(); i++){
		retVal.push_back(new PrimeSelection(alias, atts[i]));
	}
	return retVal;
}

/*
// *** YL
GestaltAM::GestaltAM(const ISchema& schema, const string& url, 
		     vector<string> member, const Stats& stats)
{
	typeIDs = schema.getTypeIDs();
	this->url = url;
	this->memberList = member;
}

Cost GestaltAM::getCost() const
{
	// fix this later

	return 0;
}

Iterator* GestaltAM::createExec() const
{
	TableName* tableName;
	vector<Iterator*> vec;
	Interface * interf;
	vector<AccessMethod*> acc;
	
	vector<string>::iterator i;
	
	for (i = (vector<string>::iterator) memberList.begin(); 
	     i != (vector<string>::iterator) memberList.end(); i++)
	  {
	    tableName = new TableName(i->c_str());
	    
	    TRY(interf = ROOT_CATALOG.createInterface(tableName),0);

	    acc = interf->createAccessMethods();
	    assert (acc.size() == 1);
	    vec.push_back(acc[0]->createExec());
	  }
	
	return new UnionExec(vec);
}
*/

