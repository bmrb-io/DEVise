#include "TableUtils.h"
#include "myopt.h"
#include "Iterator.h"
#include "Optimizer.h"

#include <math.h>

#include <heap.h>
#include <algorithm>

DTESite THIS_SITE;

string& operator<<(string& s, const LogicalProp&){
	return s;
}

OptNode::OptNode(TableMap tableMap, const SiteDesc* siteDesc) : 
		tableMap(tableMap) //,	siteDesc(siteDesc)
{}

LogicalProp OptNode::getLogProp(const LogPropTable& logPropTab) const
{
	return logPropTab[tableMap.getBitMap()];
}

string& operator<<(string& s, const vector<SortCriterion>& arg){
	assert(0);
	return s;
}

Optimizer::Optimizer(const Query& query) : query(query), root(NULL) {}

Optimizer::~Optimizer(){
	delete root;
	/*
     NodeTable::iterator it;
     for(it = nodeTab.begin(); it != nodeTab.end(); it++){
          delete (*it).second;
     }
	*/
}

SPQueryProduced::SPQueryProduced(TableMap tableMap,
          const SiteDesc* siteDesc)
		: OptNode(tableMap, siteDesc), bestAlt(NULL) {}

SPQueryProduced::~SPQueryProduced(){
	delete bestAlt;
}

SPJQueryProduced::SPJQueryProduced(TableMap tableMap,
          const SiteDesc* siteDesc)
		: OptNode(tableMap, siteDesc)
{
}

SPJQueryProduced::~SPJQueryProduced(){
}

void printAlts(const AltEntry& a, ostream& out = cout)
{
	cout << "[ c = " << a.first << "   ";
	a.second->display(cout);
	cout << "]";
	cout << endl;
}

bool SPJQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{

	const SiteDesc* siteDesc = NULL;	// remove this

	if(alts.empty()){

		int numAlts = (tableMap.getBitMap() + 1) / 2 - 1;

		//   Allocate the space here to speed up the things

//		alts.reserve(numAlts);	// no don't know how many alts

//		cerr << "Creating alternatives ...\n";

		TableMap::Iterator it;
		int tmp = 0;
//		cerr << "parent = " << tableMap << ", " << endl;
		for(it = tableMap.begin(); !(it == tableMap.end()); ++it){

			// the weird construction above is to avoid bug in 
			// function.h stl header

			TableMap current = *it;
			TableMap complement = it.getComplement();
//			cerr << "current = " << current << ", compl = " << complement << endl;
			int left = current.getBitMap();
			int right = complement.getBitMap();
			Cost baseCost = JoinMethod::getBaseCost(
				logPropTab[left], logPropTab[right]);

			if(baseCost < CUT_OFF_COST){

				if(!nodeTab[left]){
					if(current.isSinglet()){
						nodeTab[left] = 
							new SPQueryProduced(current, siteDesc);
					}
					else{
						nodeTab[left] = 
							new SPJQueryProduced(current, siteDesc);
					}
				}
				if(!nodeTab[right]){
					if(complement.isSinglet()){
						nodeTab[right] = 
							new SPQueryProduced(complement, siteDesc);
					}
					else{
						nodeTab[right] = 
							new SPJQueryProduced(complement, siteDesc);
					}
				}

				// create join from these two nodes

				JoinMethod* a = new JoinMethod(nodeTab[left], nodeTab[right]);
				alts.push_back(AltEntry(a->getCost(logPropTab), a));

				tmp++;
			}
		}
//		cerr << "Created " << tmp << " alternatives\n";
//		cerr << "making heap ...";

		make_heap(alts.begin(), alts.end(), AlternativeLess());

//		cerr << " done" << endl;

		#if defined(DEBUG)
			cout << "listing ..." << endl;
			sort_heap(alts.begin(), alts.end(), AlternativeLess());
			for_each(alts.begin(), alts.end(), printAlts);
			cout <<  endl;
		#endif

		return true;
	}

	// recheck the cost before expanding, may be old 

	while(true){
		pop_heap(alts.begin(), alts.end(), AlternativeLess());
		alts.back().first = alts.back().second->getCost(logPropTab);
		if(alts.back().first <= alts.front().first){
			break;
		}
		else{
			push_heap(alts.begin(), alts.end(), AlternativeLess());
		}
	}
	bool topExp = alts.back().second->expand(q, nodeTab, logPropTab);
	alts.back().first = alts.back().second->getCost(logPropTab);
	push_heap(alts.begin(), alts.end(), AlternativeLess());
//	cerr << "first now " << alts.front().first << endl;
	return topExp;
}

Cost SPJQueryProduced::getCost(const LogPropTable& logPropTab) {
	if(alts.empty()){
		return 0;
	}

	// reevaluate the cost because the subtrees may have been expanded.

	while(true){
		pop_heap(alts.begin(), alts.end(), AlternativeLess());
		alts.back().first = alts.back().second->getCost(logPropTab);
		if(alts.back().first <= alts.front().first){
			break;
		}
		else{
			push_heap(alts.begin(), alts.end(), AlternativeLess());
		}
	}
	push_heap(alts.begin(), alts.end(), AlternativeLess());
	return alts.front().first;
}

void SPJQueryProduced::display(ostream& out) const {
	if(alts.empty()){
		out << tableMap;
		return;
	}
#if defined(DEBUG) || 1
//	out << "listing ..." << endl;
//	sort_heap(alts.begin(), alts.end(), AlternativeLess());
//	for_each(alts.begin(), alts.end(), printAlts);
	vector<AltEntry>::const_iterator it;
	for(it = alts.begin(); it != alts.end(); ++it){
		printAlts(*it, out);
	}
	out <<  endl;
#endif
//	printAlts(alts.front(), out);
}

Cost SPQueryProduced::getCost(const LogPropTable&) {
	if(!bestAlt){
		return 0;
	}
	return bestAlt->getCost();
}

bool SPQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{
	if(bestAlt){
		return false;
	}
	SiteDesc* siteDesc = &THIS_SITE;	// remove later
	bestAlt = siteDesc->getBestAM(tableMap, q);
	return true;
}

Iterator* SPQueryProduced::createExec() const {
	assert(!"not implemented");
	return NULL;
}

Iterator* SPJQueryProduced::createExec() const {
	assert(!"not implemented");
	return NULL;
}

void SPQueryProduced::display(ostream& out) const {
	out << tableMap;
	if(!bestAlt){
		return;
	}
	bestAlt->display(out);
}

void Optimizer::run(){
	const vector<TableAlias*>& tableList = query.getTableList();
	int numTables = tableList.size();
	int numComb = (1 << numTables);
	const vector<BaseSelection*>& preds = query.getPredicateList();
	vector<TableMap> predMaps;
	predMaps.reserve(preds.size());
	vector<BaseSelection*>::const_iterator pi;
	for(pi = preds.begin(); pi != preds.end(); ++pi){
		TableMap tmp = (*pi)->getTableMap(tableList);
		predMaps.push_back(tmp);
	}

	logPropTab.reserve(numComb);
	nodeTab.reserve(numComb);
	logPropTab.push_back(LogicalProp(0));	// first position is unused
	nodeTab.push_back((OptNode*) 0);	// first position is unused

	cerr << "Creating log prop table of size " << numComb -1 << " ... ";

	for(int i = 1; i < numComb; i++){

		nodeTab.push_back((OptNode*) 0);	// initialize the node table

		const TableMap currTabSet(i);
		double card = 1;
		int tupSz = 0;
		int bitCount = 1;
		for(int k = 0; k < tableList.size(); k++){
			if(currTabSet.contains(TableMap(bitCount))){
				card *= TABLE_CARD;
				tupSz += NUM_FLDS * FIELD_SZ;		// assumes select *
			}
			bitCount <<= 1;
		}
		double selectivity = 1;
		for(int j = 0; j < predMaps.size(); j++){
			if(currTabSet.contains(predMaps[j])){
				selectivity *= SELECTIVITY;
			}
		}
		PageCount numPgs = card * selectivity * tupSz / PAGE_SZ;
		logPropTab.push_back(LogicalProp(numPgs));
	}

	cerr << "done\n";

#if defined(DEBUG)

	for(int i = 1; i < numComb; i++){
		cerr << i << "  " << logPropTab[i] << endl;
	}

#endif

//	root = new QueryNeeded(query, &THIS_SITE);
//	for now, test the simple stuff

	TableMap allTables(numComb - 1);
	if(allTables.isSinglet()){
		root = new SPQueryProduced(allTables, &THIS_SITE);
	}
	else{
		root = new SPJQueryProduced(allTables, &THIS_SITE);
	}

	int expansionCnt_debug = 1;
	cerr << "First iteration ... ";
	root->expand(query, nodeTab, logPropTab);
	cerr << "done\n";
     while(root->expand(query, nodeTab, logPropTab)){
	/*
		cerr << "---------------" << endl;
		display(cerr);
		cerr << endl;
	*/
		expansionCnt_debug++;
     }

	cerr << "Optimization completed after " 
		<< expansionCnt_debug << " iterations\n";
	cerr << "Cost = " << root->getCost(logPropTab) << endl;
	exit(1);
}

void Optimizer::display(ostream& out) const {
	assert(root);
	root->display(out);
}

Iterator* Optimizer::createExec() {
	if(!root){
		run();
	}
	return root->createExec();
}

Iterator* FileScan::createExec() const {
	assert(!"not implemented");
	return NULL;
}

FileScan::FileScan(const NewStat& stat)
{
//	cost = stat.getNumPgs();
	cost = TABLE_CARD * NUM_FLDS * FIELD_SZ / PAGE_SZ;
}

void FileScan::display(ostream& out)
{
//	out << "File Scan (cost = " << cost << ")";
}

Cost FileScan::getCost() const
{
	return cost;
}

AccessMethod* DTESite::getBestAM(TableMap tableMap, const Query& q) const
{
	return new FileScan(DEFAULT_STAT);
/*
	vector<AccessMethod*>::iterator it;
	assert(bestAlt == 0);
	Cost minCost = MAX_COST;
	for(it = alts.begin(); it != alts.end(); ++it){
		AccessMethod* curr = *it;
		if(curr->getCost() < minCost){
			delete bestAlt;
			bestAlt = curr;
			minCost = curr->getCost();
		}
		else{
			delete curr;
		}
	}
	const vector<TableAlias*>& table = logicalProp.getTableList();
	assert(table.size() == 1);
	TableName* tableName = table.front()->getTable();
     string tableNm = tableName->toString();
     Iterator* statIt;

//   istream* statStream = getStatTableStream();

	istream* statStream = 0;
     if(!statStream || !statStream->good()){
          return new FileScan(logicalProp, DEFAULT_STAT);
     }
     TRY(statIt = createIteratorFor(STAT_SCHEMA, statStream, tableNm), 0);
     statIt->initialize();
     const Tuple* tup;
	FileScan* retVal = 0;
     if((tup = statIt->getNext())){
          retVal = new FileScan(logicalProp, *((NewStat*) tup[1]));
	}
	else{
		retVal = new FileScan(logicalProp, DEFAULT_STAT);
	}
	delete statIt;
	return retVal;
*/
}

JoinMethod::JoinMethod(OptNode* left, OptNode* right) :
	left(left), right(right)
{
}

bool JoinMethod::expand(
	const Query& q, NodeTable& nodeTab, const LogPropTable& logPropTab)
{
	bool retVal1 = left->expand(q, nodeTab, logPropTab);
	bool retVal2 =	right->expand(q, nodeTab, logPropTab);
	return retVal1 || retVal2;
}

Cost JoinMethod::getCost(const LogPropTable& logPropTab)
{
	LogicalProp lp = left->getLogProp(logPropTab);
	LogicalProp rp = right->getLogProp(logPropTab);

//	int numBlocks = (int) ceil(lp / double(MEMORY_PGS));

	// saving the right stream to disk if necessary +
	// reading it for each block except first

//	Cost baseCost =  (numBlocks > 1 ? rp : 0) + (numBlocks - 1) * rp;
	Cost baseCost = JoinMethod::getBaseCost(lp, rp); 

	return baseCost + left->getCost(logPropTab) + right->getCost(logPropTab);
}

void JoinMethod::display(ostream& out)
{
	out << "join(";
	left->display(out);
	out << " + ";
	right->display(out);
	out << ")";
}

ostream& operator<<(ostream& out, const LogicalProp& x) {
	return out << x.numPgs;	
}

// ostream& operator<<(ostream& out, const OptNode& x)
