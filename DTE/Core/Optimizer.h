#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <map.h>

#include <string>

#include "types.h"
#include "StringLess.h"
#include "TableUtils.h"

typedef float Cost;
typedef float PageCount;

const Cost MAX_COST = DBL_MAX;
const int MEMORY_PGS = 100;
const int TABLE_CARD = 1000;
const int FIELD_SZ = 100;
const int NUM_FLDS = 2;
const float SELECTIVITY = 0.001;
const int PAGE_SZ = 4000;
const float CUT_OFF_COST = 1e6;

class OptNode;
class LogicalProp;
class BaseSelection;
class TableAlias;
class Iterator;
class JoinMethod;

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
};

typedef vector<LogicalProp> LogPropTable;

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
	virtual void display(ostream& out = cout);
	virtual Cost getCost(const LogPropTable&);
	static Cost getBaseCost(
		const LogicalProp& leftLP, const LogicalProp& rightLP);
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

class AccessMethod {
public:
//	vector<BaseSelection*> getProjectList();
	virtual Iterator* createExec() const = 0;
	virtual Cost getCost() const = 0;
	virtual void display(ostream& out = cout) = 0;
};

class FileScan : public AccessMethod {
	Cost cost;
public:
	FileScan(const NewStat& stat);
//	vector<BaseSelection*> getProjectList();
	virtual void display(ostream& out = cout);
	virtual Iterator* createExec() const;
	virtual Cost getCost() const;
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
	virtual Iterator* createExec() const = 0;
	virtual Cost getCost(const LogPropTable& logPropTab) = 0;
	virtual void display(ostream& out = cout) const = 0;
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
	virtual Iterator* createExec() const;
	virtual void display(ostream& out = cout) const;
	virtual Cost getCost(const LogPropTable& logPropTab);
};

class SPQueryProduced : public OptNode {
	AccessMethod* bestAlt;
public:
	SPQueryProduced(TableMap tableMap, const SiteDesc* siteDesc);
	SPQueryProduced::~SPQueryProduced();
	virtual bool expand(
		const Query& q, 
		NodeTable& nodeTab, 
		const LogPropTable& logPropTab);
	virtual Iterator* createExec() const;
	virtual void display(ostream& out = cout) const;
	virtual Cost getCost(const LogPropTable& logPropTab);
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
