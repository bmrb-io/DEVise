#ifndef ACCESS_METHOD_H
#define ACCESS_METHOD_H

#include <vector>
#include <string>

#include "types.h"

typedef float Cost;

class BaseSelection;
class Iterator;

class AccessMethod {
public:
	virtual vector<BaseSelection*> getProjectList(const string& alias) const = 0;
	virtual Iterator* createExec() const = 0;
	virtual Cost getCost() const = 0;
	virtual string getName() const = 0;
};

class FileScan : public AccessMethod {
	Cost cost;
public:
	FileScan(const NewStat& stat);
	virtual string getName() const {return "FileScan";} 
	virtual Iterator* createExec() const;
	virtual Cost getCost() const;
	virtual vector<BaseSelection*> getProjectList(const string& alias) const {assert(0);}
};

class StandardAM : public AccessMethod {
	ISchema schemaM;
	string url;
public:
	StandardAM(const ISchema& schema, const string& url, const Stats& stats);
	virtual Iterator* createExec() const;
	virtual Cost getCost() const;
	virtual string getName() const {return "StandardRead";};
	virtual vector<BaseSelection*> getProjectList(const string& alias) const;
};

// *** YL
/*
class GestaltAM : public AccessMethod {
	TypeIDList typeIDs;
	string url;
	vector<string> memberList;
	
public:
	GestaltAM(const ISchema& schema, const string& url, vector<string> member,
		  const Stats& stats);
	virtual Iterator* createExec() const;
	virtual Cost getCost() const;
	virtual string getName() const {return "GestaltRead";};
};
*/

#endif
