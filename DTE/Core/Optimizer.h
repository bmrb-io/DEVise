#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <string>
#include <vector>
#include <map.h>

#include "StringLess.h"

typedef double Cost;
const Cost MAX_COST = DBL_MAX;

class OptNode;
class LogicalProp;
class BaseSelection;
class TableAlias;
class Iterator;

enum Ordering {ASC, DESC};
typedef map<string, OptNode*, StringLess> NodeTable;
typedef map<string, LogicalProp*, StringLess> LogPropTable;
typedef pair<BaseSelection*, Ordering> SortCriterion;

string& operator<<(string& s, const SortCriterion&);
string& operator<<(string& s, const vector<SortCriterion>&);
string& operator<<(string& s, const LogicalProp&);

class LogicalProp {
	vector<BaseSelection*> selectList;
	vector<TableAlias*> tableList;
	vector<BaseSelection*> predList;
public:
	LogicalProp(const vector<BaseSelection*>& selectList,
		const vector<TableAlias*>& tableList,
		const vector<BaseSelection*>& predList) :

		selectList(selectList), tableList(tableList), predList(predList){
	}
	const vector<TableAlias*>& getTableList() const {
		return tableList;
	}
};

class Query {
	LogicalProp logicalProp;
	vector<SortCriterion> orderBy;
public:
	Query(const vector<BaseSelection*>& selectList,
		const vector<TableAlias*>& tableList,
		const vector<BaseSelection*>& predList,
		const vector<SortCriterion>& orderBy) :

		logicalProp(selectList, tableList, predList), orderBy(orderBy) {
	}
	const LogicalProp& getLogProp() const {
		return logicalProp;
	}
	const vector<SortCriterion>& getOrderBy() const {
		return orderBy;
	}
};

class Alternative {
	bool doneFlag;
public:
	virtual Cost expand(NodeTable& nodeTab, LogPropTable& logPropTab) = 0;
	bool done() const {
		return doneFlag;
	}
};

class AccessMethod {
	Cost cost;
public:
	Cost getCost() const {
		return cost;
	}
//	vector<BaseSelection*> getProjectList();
	virtual void applyTo(const LogicalProp& logicalProp) = 0;
	virtual Iterator* createExec() const = 0;
};

class FileScan : public AccessMethod {
public:
//	vector<BaseSelection*> getProjectList();
	virtual void applyTo(const LogicalProp& logicalProp);
	virtual Iterator* createExec() const;
};

class SiteDesc {
public:
	virtual vector<AccessMethod*> 
		getAccessMethods(const TableAlias& table) const = 0;
};

class DTESite : public SiteDesc {
public:
	virtual vector<AccessMethod*> 
		getAccessMethods(const TableAlias& table) const;
};

extern DTESite THIS_SITE;
// sites should be maintained with other typechecking info

class OptNode {
protected:
	const LogicalProp* logicalProp;
	vector<SortCriterion> orderBy;
	const SiteDesc* siteDesc;
	bool doneFlag;
public:
	OptNode(const LogicalProp* logicalProp,
		const vector<SortCriterion>& orderBy, const SiteDesc* siteDesc);
	virtual Cost expand(NodeTable& nodeTab, LogPropTable& logPropTab) = 0;
	virtual Iterator* createExec() const = 0;
	virtual void printTo(string&) const;
	bool done() const {
		return doneFlag;
	}
};

class SPQueryProduced : public OptNode {
	AccessMethod* bestAlt;
public:
	SPQueryProduced(const LogicalProp* logicalProp, const SiteDesc* siteDesc);
	SPQueryProduced::~SPQueryProduced();
	virtual Cost expand(NodeTable& nodeTab, LogPropTable& logPropTab);
	virtual Iterator* createExec() const;
	virtual void printTo(string& s) const;
};

class QueryNeeded : public OptNode {
public:
	QueryNeeded(const LogicalProp* logicalProp,
		const vector<SortCriterion>& orderBy, const SiteDesc* siteDesc);
	virtual Cost expand(NodeTable& nodeTab, LogPropTable& logPropTab);
	virtual void printTo(string& s) const;
};

class Optimizer {
	Query query;
	OptNode* root;
	NodeTable nodeTab;
	LogPropTable logPropTab;
private:
	void run();
public:
	Optimizer(const Query& query);
	Iterator* createExec() const;
	~Optimizer();
};

#endif
