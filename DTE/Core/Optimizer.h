#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>

#include "types.h"
#include "StringLess.h"
#include "TableUtils.h"
#include "AccessMethod.h"
#include "Aggregates.h"

#define DEBUG_STAT

typedef float Cost;
typedef float PageCount;

inline string costToString(Cost c){
	ostringstream tmp;
	tmp << c;
	return tmp.str();
}

//const Cost MAX_COST = DBL_MAX;

const int MEMORY_PGS = 100;
const int FIELD_SZ = 100;
const int NUM_FLDS = 2;
const int PAGE_SZ = 4000;

// FLT_MAX        3.402823466E+38F  /* max decimal value of a "float" */
//const float CUT_OFF_COST = 1e6;

const float CUT_OFF_COST = 1e10;

class OptNode;
class LogicalProp;
class BaseSelection;
class TableAlias;
class Iterator;
class JoinMethod;
class ExprList;

enum Ordering {ASC, DESC};
typedef vector<OptNode*> NodeTable;
typedef pair<BaseSelection*, Ordering> SortCriterion;
typedef pair<Cost, JoinMethod*> AltEntry;

string& operator<<(string& s, const SortCriterion&);
string& operator<<(string& s, const vector<SortCriterion>&);
string& operator<<(string& s, const LogicalProp&);

class LogicalProp {
	PageCount numPgs;
public:
	LogicalProp(PageCount numPgs = 0) : numPgs(numPgs) {}
	const PageCount getNumPgs() const {
		return numPgs;
	}
	friend ostream& operator<<(ostream&, const LogicalProp&);
	string toString() const;
};

class Query {
	vector<BaseSelection*> selectList;
	vector<TableAlias*> tableList;
	vector<BaseSelection*> predList;
	vector<SortCriterion> orderBy;
public:
/*
	Query(const vector<BaseSelection*>& selectList,
		const vector<TableAlias*>& tableList,
		const vector<BaseSelection*>& predList,
		const vector<SortCriterion>& orderBy) :

		logicalProp(selectList, tableList, predList), orderBy(orderBy) {
	}
*/
	// temporary Query constructor

	Query(const vector<BaseSelection*>& selectList,
		const vector<TableAlias*>& tableList,
		const vector<BaseSelection*>& predList) :

		selectList(selectList), tableList(tableList), predList(predList){
	}
	const vector<BaseSelection*>& getSelectList() const {
		return selectList;
	}
	const vector<BaseSelection*>& getPredicateList() const {
		return predList;
	}
	const vector<TableAlias*>& getTableList() const {
		return tableList;
	}
	const vector<SortCriterion>& getOrderBy() const {
		return orderBy;
	}
	ostream& display(ostream& out) const;
	bool hasAggregates() const;
};

class LogPropTable {
	vector<LogicalProp> logPropTab;
	vector<TableMap> predMaps;
	vector<TableAlias*> tableList;
	vector<int> cardinalities;
	vector<float> selectivities;
public:
	LogPropTable(){}
	void initialize(const Query& query);
	LogicalProp operator[](int i) const {
		return logPropTab[i];
	}
	LogicalProp logPropFor(int i) const;
};

class JoinMethod {
	OptNode* left;
	OptNode* right;
public:
	JoinMethod(OptNode* left, OptNode* right);
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual string getName() const {return "Join";}
	virtual Cost getCost(const LogPropTable&);
	static Cost getBaseCost(
		const LogicalProp& leftLP, const LogicalProp& rightLP);
	OptNode* getLeft(){return left;}
	OptNode* getRight(){return right;}
#if defined(DEBUG_STAT)
private:
	bool isExpandedM;
public:
	bool isExpanded(){return isExpandedM;}
#endif
};

inline Cost JoinMethod::getBaseCost
	(const LogicalProp& leftLP, const LogicalProp& rightLP)
{
	return leftLP.getNumPgs() / double(MEMORY_PGS) * rightLP.getNumPgs();
}
 
class AlternativeLess {
	// rename to AlternativeGreater
public:
     bool operator()(const AltEntry& a, const AltEntry& b) const {
          return a.first > b.first;
     }
};

class SiteDesc {
public:
	virtual AccessMethod*
		getBestAM(TableMap tableMap, const Query& q) const = 0;
};

class DTESite : public SiteDesc {
public:
	virtual AccessMethod*
		getBestAM(TableMap tableMap, const Query& q) const;
};

extern DTESite THIS_SITE;
// sites should be maintained with other typechecking info

class OptNode {
protected:
	TableMap tableMap;
//	vector<SortCriterion> orderBy;	// ignored for now
//	const SiteDesc* siteDesc;
public:
	OptNode(TableMap tableMap, const SiteDesc* siteDesc);
	LogicalProp getLogProp(const LogPropTable& logPropTab) const;
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab) = 0;
	virtual Iterator* createExec(const Query& q) const = 0;
	virtual Cost getCost(const LogPropTable& logPropTab) = 0;
	virtual string toString() const = 0;
	virtual int getNumOfChilds() = 0;
	virtual vector<OptNode*> getLevel(int level) = 0;
	virtual Cost getCostConst() const = 0;
	virtual int getTotalNumNodes() const = 0;	// for debugging
	virtual int getNumExpandedNodes() const = 0;	// for debugging
	vector<BaseSelection*> getProjectList(const Query& query) const;
	TableMap getTableMap() const {
		return tableMap;
	}
};

class SPJQueryProduced : public OptNode {
	vector<AltEntry> alts;
public:
	SPJQueryProduced(TableMap tableMap, const SiteDesc* siteDesc);
	SPJQueryProduced::~SPJQueryProduced();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec(const Query& q) const;
	virtual string toString() const;
	virtual Cost getCost(const LogPropTable& logPropTab);
	virtual int getNumOfChilds(){return 2;}
	virtual vector<OptNode*> getLevel(int level);
	virtual Cost getCostConst() const;
	virtual int getTotalNumNodes() const;	// for debugging
	virtual int getNumExpandedNodes() const;	// for debugging
};

class RemQueryProduced : public OptNode {
	string queryToShip;
	string host;
	int port;
	TypeIDList typeIDs;
public:
	RemQueryProduced(const Query& query, TableMap tableMap, const SiteDesc* siteDesc);
	RemQueryProduced::~RemQueryProduced();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec(const Query& q) const;
	virtual string toString() const {return "Query Shipping";}
	virtual Cost getCost(const LogPropTable& logPropTab) {
		return 0;
	}
	virtual int getNumOfChilds(){return 0;}
	virtual vector<OptNode*> getLevel(int level);
	virtual Cost getCostConst() const {return 0;}
	virtual int getTotalNumNodes() const {return 0;};  // for debugging
	virtual int getNumExpandedNodes() const {return 0;};	// for debugging
};

class QueryNeeded : public OptNode {
	OptNode* root;
public:
	QueryNeeded(const Query& query, TableMap tableMap, const SiteDesc* siteDesc);
	QueryNeeded::~QueryNeeded();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec(const Query& q) const {
		return root->createExec(q);
	}
	virtual string toString() const {return root->toString();}
	virtual Cost getCost(const LogPropTable& logPropTab) {
		return root->getCost(logPropTab);
	}
	virtual int getNumOfChilds(){return 1;}
	virtual vector<OptNode*> getLevel(int level);
	virtual Cost getCostConst() const {return root->getCostConst();}
	virtual int getTotalNumNodes() const {return 1;};  // for debugging
	virtual int getNumExpandedNodes() const {return 1;};	// for debugging
};

class AggQueryNeeded : public OptNode {
	Query* aggLessQuery;
	AggList* aggs;
	OptNode* root;
public:
	AggQueryNeeded(const Query& query, TableMap tableMap, const SiteDesc* siteDesc);
	AggQueryNeeded::~AggQueryNeeded();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab)
	{
		return root->expand(*aggLessQuery, nodeTab, logPropTab);
	}
	virtual Iterator* createExec(const Query& q) const;
	virtual string toString() const {return "aggregates";}
	virtual Cost getCost(const LogPropTable& logPropTab) {
		return root->getCost(logPropTab);
	}
	virtual int getNumOfChilds(){return 1;}
	virtual vector<OptNode*> getLevel(int level);
	virtual Cost getCostConst() const {return root->getCostConst();}
	virtual int getTotalNumNodes() const {return 1;};  // for debugging
	virtual int getNumExpandedNodes() const {return 1;};	// for debugging
};

class SPQueryProduced : public OptNode {
	AccessMethod* bestAlt;
	Cost cost;		// temporarily here
	string aliasM;
public:
	SPQueryProduced(TableMap tableMap, const SiteDesc* siteDesc);
	SPQueryProduced::~SPQueryProduced();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec(const Query& q) const;
	virtual string toString() const;
	virtual Cost getCost(const LogPropTable& logPropTab);
	virtual int getNumOfChilds(){return 0;}
	virtual vector<OptNode*> getLevel(int level);
	virtual Cost getCostConst() const;
	virtual int getTotalNumNodes() const {return 1;};  // for debugging
	virtual int getNumExpandedNodes() const {return 1;};	// for debugging
};

typedef pair<OptNode*, Query*> NodeQueryPair;

class GestaltQueryProduced : public SPQueryProduced {
	bool initialized;
	vector<NodeQueryPair> gestMembers;
public:
	GestaltQueryProduced(TableMap tableMap, const SiteDesc* siteDesc);
	GestaltQueryProduced::~GestaltQueryProduced();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec(const Query& q) const;
	virtual string toString() const;
};

class Optimizer {
	Query query;
	OptNode* root;
	NodeTable nodeTab;
	LogPropTable logPropTab;
public:
	Optimizer(const Query& query);
	void run();
	Iterator* createExec();
	~Optimizer();
	void display(ostream& out = cout) const;
};

void printAlts(const AltEntry& a, ostream& out = cout);
#endif
