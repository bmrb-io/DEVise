#include "Optimizer.h"

DTESite THIS_SITE;

string& operator<<(string& s, const LogicalProp&){
	return s;
}

OptNode::OptNode(
	const LogicalProp* logicalProp, 
	const vector<SortCriterion>& orderBy, 
	const SiteDesc* siteDesc) : 
		logicalProp(logicalProp),
		orderBy(orderBy),
		siteDesc(siteDesc),
		doneFlag(false)
{}

void OptNode::printTo(string& s) const {
	s << *logicalProp;
	s += " order by ";
	s << orderBy;
}

string& operator<<(string& s, const vector<SortCriterion>& arg){
	assert(0);
	return s;
}

Optimizer::Optimizer(const Query& query) : query(query), root(NULL) {}

Optimizer::~Optimizer(){
	delete root;
     NodeTable::iterator it;
     for(it = nodeTab.begin(); it != nodeTab.end(); it++){
          delete (*it).second;
     }
     LogPropTable::iterator lit;
     for(lit = logPropTab.begin(); lit != logPropTab.end(); lit++){
          delete (*lit).second;
     }
}

QueryNeeded::QueryNeeded(const LogicalProp* logicalProp,
          const vector<SortCriterion>& orderBy, const SiteDesc* siteDesc)
		: OptNode(logicalProp, orderBy, siteDesc) {}

void QueryNeeded::printTo(string& s) const {
	s += "Need "; 
	OptNode::printTo(s);
}

Cost QueryNeeded::expand(NodeTable& nodeTab, LogPropTable& logPropTab){
	return 0;
}

SPQueryProduced::SPQueryProduced(const LogicalProp* logicalProp,
          const SiteDesc* siteDesc)
		: OptNode(logicalProp, orderBy, siteDesc), bestAlt(NULL) {}

SPQueryProduced::~SPQueryProduced(){
	delete bestAlt;
}

void SPQueryProduced::printTo(string& s) const {
	s += "Produce "; 
	OptNode::printTo(s);
}

Cost SPQueryProduced::expand(NodeTable& nodeTab, LogPropTable& logPropTab)
{
	if(doneFlag){
		assert(bestAlt);
		return bestAlt->getCost();
	}
	const vector<TableAlias*>& tableList = logicalProp->getTableList();
	assert(tableList.size() == 1);
	const TableAlias& table = *tableList.front();
	vector<AccessMethod*> alts = siteDesc->getAccessMethods(table);
	assert(alts.size() > 0);
	vector<AccessMethod*>::iterator it;
	assert(bestAlt == 0);
	Cost minCost = MAX_COST;
	for(it = alts.begin(); it != alts.end(); ++it){
		AccessMethod* curr = *it;
		curr->applyTo(*logicalProp);
		if(curr->getCost() < minCost){
			delete bestAlt;
			bestAlt = curr;
			minCost = curr->getCost();
		}
		else{
			delete curr;
		}
	}
	doneFlag = true;
	return minCost;
}

void Optimizer::run(){
	const LogicalProp& logicalProp = query.getLogProp();
	const vector<SortCriterion>& orderBy = query.getOrderBy();
	string strRep;
	strRep << logicalProp;
	LogicalProp* lp = new LogicalProp(logicalProp);
	logPropTab[strRep] = lp;

//	root = new QueryNeeded(lp, orderBy, &THIS_SITE);
//	for now, test simple stuff

	root = new SPQueryProduced(lp, &THIS_SITE);
     while(!root->done()){
		root->expand(nodeTab, logPropTab);
     }
}

Iterator* Optimizer::createExec() const {
	if(!root){
		run();
	}
	return root->createExec();
}

void FileScan::applyTo(const LogicalProp& logicalProp){
}

Iterator* FileScan::createExec() const {
	return NULL;
}

vector<AccessMethod*> DTESite::getAccessMethods(const TableAlias& table) const
{
	vector<AccessMethod*> retVal;
	return retVal;
}
