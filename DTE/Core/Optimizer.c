#include "TableUtils.h"
#include "myopt.h"
#include "Iterator.h"
#include "Optimizer.h"
#include "Stats.h"
#include "Interface.h"
#include "ExecOp.h"
#include "listop.h"
#include "DteProtocol.h"
#include "StandardRead.h"

#include <math.h>

#include <algorithm>

DTESite THIS_SITE;

void deleteFun(BaseSelection* x){
	delete x;
}

ExprList* createExecExprs(const vector<BaseSelection*>& projList, const SqlExprLists& inputLists)
{
	ExprList* retVal = new ExprList();
	vector<BaseSelection*>::const_iterator it;
	for(it = projList.begin(); it != projList.end(); ++it){
		TRY(ExecExpr* ee = (*it)->createExec(inputLists), 0);
		assert(ee);
		retVal->push_back(ee);
	}
	return retVal;
}

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
	// do not delete bestAlt, not an owner;
}

GestaltQueryProduced::GestaltQueryProduced(TableMap tableMap,
          const SiteDesc* siteDesc)
		: SPQueryProduced(tableMap, siteDesc) 
{
	initialized = false;
}

GestaltQueryProduced::~GestaltQueryProduced(){
}

SPJQueryProduced::SPJQueryProduced(TableMap tableMap,
          const SiteDesc* siteDesc)
		: OptNode(tableMap, siteDesc)
{
}

SPJQueryProduced::~SPJQueryProduced(){
}

void printAlts(const AltEntry& a, ostream& out)
{
/*
	cout << "[ c = " << a.first << "   ";
	a.second->display(cout);
	cout << "]";
	cout << endl;
*/
}

static int debugExpressCnt = 0;

bool SPJQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{

	const SiteDesc* siteDesc = NULL;	// remove this

	if(alts.empty()){

		int numAlts = (1 << (tableMap.count() - 1)) - 1;

		//   Allocate the space here to speed up the things

		debugExpressCnt += numAlts;

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

Cost SPJQueryProduced::getCostConst() const {
	assert(!alts.empty());
	return alts.front().first;
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

string SPJQueryProduced::toString() const
{
	assert(!alts.empty());
	
	JoinMethod* join = alts.front().second;
	ostringstream ostr;
	ostr << join->getName();
	ostr << "(" << tableMap << ")";
	string tmps = ostr.str();
	return tmps;

/*
	if(alts.empty()){
		out << tableMap;
		return;
	}
*/
#if defined(DEBUG)
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

Cost SPQueryProduced::getCostConst() const {
	return cost;
//	assert(bestAlt);
//	return bestAlt->getCost();
}

Cost SPQueryProduced::getCost(const LogPropTable& lpt) {

	// does not work for single selections

//	cost = lpt[tableMap.getBitMap()].getNumPgs(); 

	cost = 0;		// does not work for interesting orders
	return cost;

/*
	if(!bestAlt){
		return 0;
	}
	return bestAlt->getCost();
*/
}

bool SPQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{
	if(bestAlt){
		return false;
	}
	const vector<TableAlias*>& tl = q.getTableList();

	assert(tableMap.isSinglet());

	const TableAlias* ta = tl[tableMap.index()];
	const vector<AccessMethod*>& amethods = ta->getAccessMethods();

	assert(amethods.size() == 1 || !"not implemented");
	// choose the cheapest access method (depends on predicates available)

	aliasM = *(ta->getAlias());

	bestAlt = amethods[0];

	return true;
}

bool GestaltQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{
	if(!initialized){
		initialized = true;

		const vector<TableAlias*>& tl = q.getTableList();
		assert(tl.size() == 1);
		const TableAlias* ta = tl[0];

		const Interface* interf = ta->getInterface();

		assert(interf->isGestalt());
		const GestaltInterface* gi = (const GestaltInterface*) interf;
		vector<string> memNames = gi->getMemberNames();
		vector<string>::const_iterator it;
		for(it = memNames.begin(); it != memNames.end(); ++it){
			string* alias = new string(*q.getTableList().front()->getAlias());
			vector<TableAlias*> newList;
			newList.push_back(new TableAlias(new TableName((*it).c_str()), alias));
			Query* query = new Query(q.getSelectList(), newList, q.getPredicateList());
			OptNode* node = new QueryNeeded(*query, tableMap, &THIS_SITE);
			gestMembers.push_back(NodeQueryPair(node, query));
		}
	}

	bool retVal = false;

	vector<NodeQueryPair>::iterator it;
	for(it = gestMembers.begin(); it != gestMembers.end(); ++it){
		retVal = retVal || (*it).first->expand(*(*it).second, nodeTab, logPropTab);
	}
	return retVal;
}

Iterator* GestaltQueryProduced::createExec(const Query& q) const 
{
	vector<NodeQueryPair>::const_iterator i;
	vector<Iterator*> ite_vec;
	
	for (i = gestMembers.begin(); i != gestMembers.end(); ++i){
		ite_vec.push_back ((*i).first->createExec(*(*i).second));
	}
	
	Iterator* retVal = new UnionExec(ite_vec);

	return retVal;
}

string GestaltQueryProduced::toString() const
{
	return "gestalt";
}

Iterator* SPQueryProduced::createExec(const Query& q) const 
{
	assert(bestAlt);

	vector<BaseSelection*> inputProj = bestAlt->getProjectList(aliasM);
	
	vector<vector<BaseSelection*> > inputs;
	inputs.push_back(inputProj);

	vector<BaseSelection*> projList = getProjectList(q);

	const vector<BaseSelection*>& preds = q.getPredicateList();

	ExprList* myWhere = new ExprList();

	vector<BaseSelection*>::const_iterator pi;
	for(pi = preds.begin(); pi != preds.end(); ++pi){
		if((*pi)->containedIn(tableMap)){
			TRY(ExecExpr* execPred = (*pi)->createExec(inputs), 0);
			assert(execPred);
			myWhere->push_back(execPred);
		}
	}

	TRY(ExprList* myProject = createExecExprs(projList, inputs), 0);

	Iterator* inputIt = bestAlt->createExec();

	Iterator* retVal = new SelProjExec(inputIt, myWhere, myProject);

	for_each(inputProj.begin(), inputProj.end(), deleteFun);

	return retVal;
}

Iterator* SPJQueryProduced::createExec(const Query& q) const {

	JoinMethod* jm = alts.front().second;

	TRY(Iterator* leftIt = jm->getLeft()->createExec(q), 0);
	assert(leftIt);
	TRY(Iterator* rightIt = jm->getRight()->createExec(q), 0);
	assert(rightIt);

	cerr << "MADE INPUTS\n";

	ExprList* execPreds = new ExprList();


	const vector<BaseSelection*>& preds = q.getPredicateList();
	const vector<TableAlias*>& tableList = q.getTableList();

	vector<BaseSelection*> leftProjLst = jm->getLeft()->getProjectList(q);
	vector<BaseSelection*> rightProjLst = jm->getRight()->getProjectList(q);

	// there should be no duplicates in proj list (for efficiency)
/*
	cerr << "left: ";
	displayVec(cerr, leftProjLst);
	cerr << "\n right: ";
	displayVec(cerr, rightProjLst);
	cerr << endl;
*/

	SqlExprLists inputs;
	inputs.push_back(leftProjLst);
	inputs.push_back(rightProjLst);


	TableMap leftMap = jm->getLeft()->getTableMap();
	TableMap rightMap = jm->getRight()->getTableMap();

	// walk the where clause and identify the predicates that 
	// can only be done at this level

	vector<BaseSelection*>::const_iterator pi;
	for(pi = preds.begin(); pi != preds.end(); ++pi){
		if((*pi)->containedIn(tableMap) && !(*pi)->containedIn(leftMap) &&
			!(*pi)->containedIn(rightMap)){

			TRY(ExecExpr* execPred = (*pi)->createExec(inputs), 0);
			assert(execPred);
			execPreds->push_back(execPred);
		}
	}

	ExprList* project = new ExprList();
	vector<BaseSelection*> myProjLst = getProjectList(q);
	vector<BaseSelection*>::const_iterator it;
	for(it = myProjLst.begin(); it != myProjLst.end(); ++it){
		project->push_back((*it)->createExec(inputs));
	}

	return new NLJoinExec(leftIt, rightIt, execPreds, project);
}

string SPQueryProduced::toString() const
{
//	out << tableMap;
	assert(bestAlt);
	string retVal = bestAlt->getName() + "(" + tableMap.toString() + ")";
	return retVal;
}

AggQueryNeeded::AggQueryNeeded(const Query& query, TableMap tableMap, const SiteDesc* siteDesc) : OptNode(tableMap, siteDesc)
{
	vector<BaseSelection*>::const_iterator it;
	vector<BaseSelection*> aggLessSelList;
	aggs = new AggList();

	vector<BaseSelection*> selectList = query.getSelectList();

	int pos = 0;
	for(it = selectList.begin(); it != selectList.end(); ++it){
		BaseSelection* curr = *it;
		if(curr->selectID() == CONSTRUCTOR_ID){
			Constructor* function = (Constructor*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const string* name = function->getName();
			args->rewind();
			if (numArgs == 1 && (*name == "min" || *name == "max")){
				args->rewind();
				aggLessSelList.push_back(args->get());
				TypeID type = curr->getTypeID();
				aggs->push_back(AggFn(*name, Field(type, pos)));
			}
		}
		pos++;
	}

	vector<BaseSelection*> predList = query.getPredicateList();
	vector<TableAlias*> tas = query.getTableList();

	aggLessQuery = new Query(aggLessSelList, tas, predList);

	root = new QueryNeeded(*aggLessQuery, tableMap, siteDesc);
}

Iterator* AggQueryNeeded::createExec(const Query& q) const
{
	Iterator* input = root->createExec(*aggLessQuery);
	return new StandAggsExec(input, aggs);
}

vector<OptNode*> AggQueryNeeded::getLevel(int level)
{
	vector<OptNode*> retVal;
	if(level == 0){
		retVal.push_back(this);
	}
	else{
		level--;
		retVal = root->getLevel(level);
	}
	return retVal;
}

QueryNeeded::QueryNeeded(const Query& query, TableMap tableMap, const SiteDesc* siteDesc) : OptNode(tableMap, siteDesc)
{
	const vector<TableAlias*>& tableList = query.getTableList();
	int numTables = tableList.size();
	int numComb = (1 << numTables);
	TableMap allTables(numComb - 1);

	if(query.hasAggregates()){
		root = new AggQueryNeeded(query, allTables, &THIS_SITE);
	}
	else if(numTables == 1){
		if(tableList.front()->isGestalt()){
			root = new GestaltQueryProduced(allTables, &THIS_SITE);
		}
		else if(tableList.front()->isRemote()){
			root = new RemQueryProduced(query, allTables, &THIS_SITE);
		}
		else{
			root = new SPQueryProduced(allTables, &THIS_SITE);
		}
	}
	else{
		root = new SPJQueryProduced(allTables, &THIS_SITE);
	}
}

RemQueryProduced::RemQueryProduced(const Query& query, TableMap tableMap, const SiteDesc* siteDesc) : OptNode(tableMap, siteDesc)
{
	vector<BaseSelection*> selectList = query.getSelectList();
	vector<BaseSelection*> predList = query.getPredicateList();
	const TableAlias* oldta = query.getTableList().front();

	typeIDs = getTypesFromVec(selectList);

	const Interface* interf = oldta->getInterface();
	assert(interf->getTypeNm() == DBServerInterface::typeName);
	const DBServerInterface* dbInterf = (DBServerInterface*) interf;

	host = dbInterf->getHost();
	port = dbInterf->getPort();
	const string& remName = dbInterf->getRemoteTableName();

	const string* alias = oldta->getAlias();

	ostringstream out;
	out << "select ";
	displayVec(out, selectList);
	out << " from ";
	out << remName << " as " << *alias;
	out << " where ";
	displayVec(out, predList, " and ");
	out << ends;
	queryToShip = out.str();
}

bool RemQueryProduced::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{
	return false;
}

vector<OptNode*> RemQueryProduced::getLevel(int level)
{
	vector<OptNode*> retVal;
	if(level == 0){
		retVal.push_back(this);
	}
	return retVal;
}

Iterator* RemQueryProduced::createExec(const Query& q) const
{
//	cerr << "Shipping: " << queryToShip;

	DteProtocol* dteProt = new DteProtocol(host, port);

	TRY(ostream& out = dteProt->getOutStream(), 0);
	out << queryToShip << ";" << flush;

	TRY(istream& in = dteProt->getInStream(), 0);

	// StandReadExec2 becomes the owner of dteProt

	Iterator* retVal = new StandReadExec2(typeIDs, &in, dteProt);
	return retVal;
}

bool QueryNeeded::expand(
	const Query& q, 
	NodeTable& nodeTab,
	const LogPropTable& logPropTab)
{
	return root->expand(q, nodeTab, logPropTab);
}

vector<OptNode*> QueryNeeded::getLevel(int level)
{
	vector<OptNode*> retVal;
	if(level == 0){
		retVal.push_back(this);
	}
	else{
		level--;
		retVal = root->getLevel(level);
	}
	return retVal;
}

void Optimizer::run(){
	const vector<TableAlias*>& tableList = query.getTableList();
	int numTables = tableList.size();
	int numComb = (1 << numTables);

	nodeTab.reserve(numComb);
	nodeTab.push_back((OptNode*) 0);	// first position is unused

	for(int i = 1; i < numComb; i++){

		nodeTab.push_back((OptNode*) 0);	// initialize the node table

	}

	logPropTab.initialize(query);

#if defined(DEBUG)

	for(int i = 1; i < numComb; i++){
		cerr << i << "  " << logPropTab[i] << endl;
	}

#endif

	TableMap allTables(numComb - 1);
	root = new QueryNeeded(query, allTables, &THIS_SITE);

	int expansionCnt_debug = 1;
//	cerr << "First iteration ... ";
	root->expand(query, nodeTab, logPropTab);
//	cerr << "done\n";
     while(root->expand(query, nodeTab, logPropTab)){
	/*
		cerr << "---------------" << endl;
		display(cerr);
		cerr << endl;
	*/
		expansionCnt_debug++;
     }

//	cerr << "Optimization completed after " << expansionCnt_debug << " iterations\n";
//	cerr << "Cost = " << root->getCost(logPropTab) << endl;
//	cerr << "Total Expressions expanded: " << debugExpressCnt << endl;
//	cerr << "Root expanded " << root->getNumExpandedNodes() << " out of ";
//	cerr << root->getTotalNumNodes() << " nodes " << endl;
//	cerr << debugExpressCnt << endl;
}

void Optimizer::display(ostream& out) const {
	assert(root);
	int level = 0;
	const LINE_LEN = 128;
	int nodeCnt = 0;
	vector<OptNode*> nodes;
	vector<int> oldHooks;
	nodes.push_back(root);
	oldHooks.push_back(LINE_LEN / 2);
	vector<string> nodeDescriptions;

	while(!nodes.empty()){

		string linepp(LINE_LEN, ' ');
		string linep(LINE_LEN, ' ');
		string line(LINE_LEN, ' ');

		vector<int> newHooks;

		assert(oldHooks.size() == nodes.size());
		for(int i = 1; i <= nodes.size(); i++){

			string desc = nodes[i - 1]->toString();
			desc += " " + nodes[i - 1]->getLogProp(logPropTab).toString();
			desc += " Cost: " + 
				costToString(nodes[i - 1]->getCostConst());
			nodeDescriptions.push_back(desc);

			ostringstream nodeCntStr;
			nodeCntStr << nodeCnt;
			string tmps = nodeCntStr.str();
			int offset = (1 + 2 *(i - 1)) * LINE_LEN / (2 * nodes.size());
			int startDash = min(offset, oldHooks[i - 1]);
			int endDash = max(offset, oldHooks[i - 1]);
			linepp[oldHooks[i - 1]] = '|';
			for(int j = startDash + 1; j < endDash; j++){
				if(linepp[j] == ' '){
					linepp[j] = '_';
				}
			}
			linep[offset] = '|';
			line.replace(offset - tmps.size() / 2, tmps.size(), tmps);
			nodeCnt++;
			if(nodes[i - 1]->getNumOfChilds() == 2){
				newHooks.push_back(offset);
				newHooks.push_back(offset + 1);
			}
			else if(nodes[i - 1]->getNumOfChilds() == 1){
				newHooks.push_back(offset);
			}
		}

		out << linepp << endl;
		out << linep << endl;
		out << line << endl;

		oldHooks = newHooks;
		level++;
		nodes = root->getLevel(level);
	}
	out << endl;
	for(int i = 0; i < nodeDescriptions.size(); i++){
		out << i << ". " << nodeDescriptions[i] << endl;
	}
}

Iterator* Optimizer::createExec() {
	if(!root){
		run();
	}
	return root->createExec(query);
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
#ifdef DEBUG_STAT
	isExpandedM = false;
#endif
}

bool JoinMethod::expand(
	const Query& q, NodeTable& nodeTab, const LogPropTable& logPropTab)
{

#ifdef DEBUG_STAT
	isExpandedM = true;
#endif

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

ostream& operator<<(ostream& out, const LogicalProp& x) {
	return out << x.numPgs;	
}

// ostream& operator<<(ostream& out, const OptNode& x)

vector<OptNode*> SPJQueryProduced::getLevel(int level)
{
	vector<OptNode*> retVal;
	if(level == 0){
		retVal.push_back(this);
	}
	else{
		level--;
		OptNode* left = alts.front().second->getLeft();
		OptNode* right = alts.front().second->getRight();
		retVal = left->getLevel(level);
		vector<OptNode*> tmp = right->getLevel(level);
		retVal.insert(retVal.end(), tmp.begin(), tmp.end());
	}
	return retVal;
}

vector<OptNode*> SPQueryProduced::getLevel(int level)
{
	vector<OptNode*> retVal;
	if(level == 0){
		retVal.push_back(this);
	}
	return retVal;
}

string LogicalProp::toString() const
{
	ostringstream tmp;
	tmp << "Pgs: " << *this << '\0';
	return tmp.str();
}

void LogPropTable::initialize(const Query& query)
{
	tableList = query.getTableList();
	int numTables = tableList.size();
	int numComb = (1 << numTables);
	int i = 0;
	const vector<BaseSelection*>& preds = query.getPredicateList();
	predMaps.reserve(preds.size());
	vector<BaseSelection*>::const_iterator pi;
	for(pi = preds.begin(); pi != preds.end(); ++pi){
		TableMap tmp = (*pi)->getTableMap();
		predMaps.push_back(tmp);
	}

/*
	// This should be done in the perl script that will enter these
	// values into the catalog.
	// calculating cardinalities

	char* muS = getenv("MU");
	assert(muS || !"please set env var MU");
	int mu = atoi(muS);

	char* ratioS = getenv("RATIO");
	assert(ratioS || !"please set env var RATIO");
	double ratio = atof(ratioS);

	double singleRatio = pow(ratio, double(1.0) / numTables);
	cardinalities.push_back(mu * 
		pow(ratio, double(numTables - 1) / (numTables*numTables)));

	for(i = 1; i < numTables; i++){
//		cerr << "card of " << i << " " << cardinalities[i - 1] << endl;
		cardinalities.push_back(cardinalities[i - 1] / singleRatio);
	}
//	cerr << "card of " << numTables << " " << cardinalities[numTables - 1] << endl;

*/
	for(i = 0; i < numTables; i++){
		const Stats* stats = tableList[i]->getStats();
		assert(stats);
		cardinalities.push_back(stats->getCardinality());
//		cerr << "card of " << i + 1 << " " << cardinalities[i] << endl;
	}

	for(pi = preds.begin(); pi != preds.end(); ++pi){
		selectivities.push_back((*pi)->getSelectivity());
//		cerr << "selectivity = " << (*pi)->getSelectivity() << endl;
	}

	logPropTab.reserve(numComb);
	logPropTab.push_back(LogicalProp(0));   // first position is unused

//	cerr << "Creating log prop table of size " << numComb -1 << " ... ";

	for(i = 1; i < numComb; i++){

		logPropTab.push_back(logPropFor(i));
//		cerr << "_________ logProp " << logPropTab.back() << endl;

	}

//	cerr << "done\n";
}

LogicalProp LogPropTable::logPropFor(int i) const
{
	const TableMap currTabSet(i);
	double card = 1;
	int tupSz = 0;

	// all the intermediate tables are of the same width

	tupSz += NUM_FLDS * FIELD_SZ;		
	int bitCount = 1;
	for(int k = 0; k < tableList.size(); k++){
		if(currTabSet.contains(TableMap(bitCount))){
			card *= cardinalities[k];
		}
		bitCount <<= 1;
	}
	double selectivity = 1;
	for(int j = 0; j < predMaps.size(); j++){
		if(currTabSet.contains(predMaps[j])){
			selectivity *= selectivities[j];
		}
	}
	PageCount numPgs = card * selectivity * tupSz / PAGE_SZ;
	return LogicalProp(numPgs);
}

ostream& Query::display(ostream& out) const
{
	out << "select ";
	displayVec(out, selectList);
	out << " from ";
	displayVec(out, tableList);
	out << " where ";
	displayVec(out, predList, " and ");
	return out;
}

bool Query::hasAggregates() const
{
	vector<BaseSelection*>::const_iterator it;

	for(it = selectList.begin(); it != selectList.end(); ++it){
		BaseSelection* curr = *it;
		if(curr->selectID() == CONSTRUCTOR_ID){
			Constructor* function = (Constructor*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const string* name = function->getName();
			args->rewind();
			if (numArgs == 1 && (*name == "min" || *name == "max")){
				return true;
			}
		}
	}
	return false;
}

vector<BaseSelection*> OptNode::getProjectList(const Query& query) const
{
	vector<BaseSelection*> retVal;

	const vector<BaseSelection*>& projects = query.getSelectList();
	const vector<BaseSelection*>& preds = query.getPredicateList();
	vector<BaseSelection*>::const_iterator it;
	for(it = projects.begin(); it != projects.end(); ++it){
		(*it)->collect(tableMap, retVal);
	}
	for(it = preds.begin(); it != preds.end(); ++it){
		if(!(*it)->containedIn(tableMap)){
			(*it)->collect(tableMap, retVal);
		}
	}
	return retVal;
}

#ifdef DEBUG_STAT

int SPJQueryProduced::getTotalNumNodes() const
{
	return alts.size();
}

int SPJQueryProduced::getNumExpandedNodes() const
{
	vector<AltEntry>::const_iterator it;
	int retVal = 0;
	for(it = alts.begin(); it != alts.end(); ++it){
		if((*it).second->isExpanded()){
			retVal++;
		}
	}
	return retVal;
}

#endif
