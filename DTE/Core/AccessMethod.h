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
	virtual Iterator* createExec() = 0;	  
	
	// this should be a const method but cannot be so because of the way 
	// the data reader is presently used.
	// Fix by splitting DataReader class into schema and data reader 

	virtual Cost getCost() const = 0;
	virtual string getName() const = 0;
};

class FileScan : public AccessMethod {
	Cost cost;
public:
	FileScan(const NewStat& stat);
	virtual string getName() const {return "FileScan";} 
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual vector<BaseSelection*> getProjectList(const string& alias) const {assert(0);}
};

class StandardAM : public AccessMethod {
	ISchema schemaM;
	string url;
public:
	StandardAM(const ISchema& schema, const string& url, const Stats& stats);
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual string getName() const {return "StandardRead";};
	virtual vector<BaseSelection*> getProjectList(const string& alias) const;
};

class DataReader;
class UniData;
class Attr;

class DataReaderAM : public AccessMethod {
	DataReader* dr;
	UniData* ud;
	int numFlds;
protected:
	TypeID* typeIDs;
	TypeIDList typeIDlist;   // contains the same thing as typeIDs
	string* attributeNames;
	string* order;
private:
	void translateUDInfo();
	void translateDRInfo();
	TypeID translateUDType(Attr* at);
public:
	DataReaderAM(const string& schemaFile, const string& dataFile); // throws
	virtual ~DataReaderAM();
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual string getName() const {return "DataReader";};
	virtual vector<BaseSelection*> getProjectList(const string& alias) const;
public:
	int getNumFlds() const {
		return numFlds;
	}
	const TypeID* getTypeIDs(){
		return typeIDs;
	}
	const string* getAttributeNames(){
		return attributeNames; // may be NULL
	}
};

#endif
