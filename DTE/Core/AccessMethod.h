#ifndef ACCESS_METHOD_H
#define ACCESS_METHOD_H

#include <vector>
#include <string>

#include "types.h"

typedef float Cost;

class OptExpr;
class Iterator;

class AccessMethod {
public:
        virtual ~AccessMethod() {}
	virtual vector<OptExpr*> getProjectList(const string& alias) const = 0;
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
        ~FileScan() {}
	FileScan(const NewStat& stat);
	virtual string getName() const {return "FileScan";} 
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual vector<OptExpr*> getProjectList(const string& alias) const {assert(0);}
};

class StandardAM : public AccessMethod {
	ISchema schemaM;
	string url;
public:
	StandardAM(const ISchema& schema, const string& url, const Stats& stats);
        ~StandardAM() {}
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual string getName() const {return "StandardRead";};
	virtual vector<OptExpr*> getProjectList(const string& alias) const;
};

class ViewAM: public AccessMethod 
{
  string query;
  int numFlds;
  vector<string> attributeNames;
  
public:
  ViewAM(const string& q, int n, string *attrNames) : query(q), numFlds (n)
    {
      for (int i = 0; i < numFlds; attributeNames.push_back (attrNames [i ++]));
    }
      
  ~ViewAM () {}
  virtual string getName () const { return "ViewAcess"; }
  virtual Iterator* createExec ();
  virtual Cost getCost() const { return 0; }
  virtual vector<OptExpr*> getProjectList (const string& alias) const;
};

class DataReader;
class Attr;

class DataReaderAM : public AccessMethod {
  DataReader* dr;
  ISchema schema;
  string order;
private:
  //void translateUDInfo();
  //void translateDRInfo();
  // result must be deleted by caller
  //DteAdt* translateUDType(Attr* at);
public:
	DataReaderAM(const string& schemaFile, const string& dataFile); // throws
	virtual ~DataReaderAM();
	virtual Iterator* createExec();
	virtual Cost getCost() const;
	virtual string getName() const {return "DataReader";};
	virtual vector<OptExpr*> getProjectList(const string& alias) const;
        const ISchema& getSchema() const { return schema; }
public:
  //int getNumFields() const { return tupAdt.getNumFields(); }
  //const DteTupleAdt& getAdt(){ return tupAdt; }
  //const vector<string>& getAttributeNames(){ return attributeNames; }
};

#endif

