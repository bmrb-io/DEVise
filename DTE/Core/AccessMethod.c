#include "StandardRead.h"
#include "AccessMethod.h"

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
	(const ISchema& schema, const string& url, const Stats& stats)
{
	typeIDs = schema.getTypeIDs();
	this->url = url;
}

Cost StandardAM::getCost() const
{
	// fix this later

	return 0;
}

Iterator* StandardAM::createExec() const
{
	return new StandReadExec(typeIDs, url);
}
