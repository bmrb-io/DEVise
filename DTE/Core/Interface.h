#ifndef DTE_INTERFACE_H
#define DTE_INTERFACE_H

#include <new.h>

#include <string>
//#include <iostream.h>   erased for sysdep.h
#include "sysdep.h"
//#if defined(_WINDOWS) || defined(_CONSOLE)
//	#include <windows.h>
//	#include "SQL.H"
//	#include <sqlext.h>
//#endif

#include "types.h"
#include "Stats.h"
#include "TableName.h"

#ifndef __GNUG__
using namespace std;
#endif

class Inserter;
class Deleter;
//class Site;
class ISchema;
class ODBC_Data;
class AccessMethod;

class Interface{
protected:
	Stats* stats;
public:
	enum Type {UNKNOWN, CATALOG, QUERY, VIEW, ODBC, DB_SERVER};
	Interface() {stats = 0;}
	virtual Interface* duplicate() const = 0;
	virtual ~Interface(){delete stats;}
  //virtual Site* getSite() = 0;

  static Interface* create(const char* type, istream& args);
  static Interface* create(const string& type, istream& args);
  static Interface* create(istream& typeArgs);

  // type must be of type DteStringAdt
  // args must be of type DteEolStringAdt
  static Interface* create(const ::Type* type, const ::Type* args);

	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << " ;";
	}
	virtual string guiRepresentation() const {
		ostringstream os;
		write(os);
		return os.str();
	}
	virtual Type getType(){
		return UNKNOWN;
	}
	virtual const ISchema* getISchema() = 0;	// throws
  int getNumFields() {
    const ISchema* schema = getISchema();
    assert(schema);
    return schema->getNumFields();
  }

  // result must be deleted by caller
  virtual Inserter* createInserter() const; // throws

  // result must be deleted by caller
  virtual Deleter* createDeleter() const; // throws

	virtual const string& getTypeNm() const = 0;
	virtual Interface* copyTo(void* space) = 0;
	const Stats* getStats(){
		return stats;
	}
	virtual vector<AccessMethod*> createAccessMethods();
	virtual bool isGestalt() const {return false;}
	virtual bool isRemote() const {return false;}
	virtual void setRemoteTableName(const TableName& name) {
          assert(name.isEmpty());
        }
	virtual const TableName& getRemoteTableName() const {assert(0);}
};

//kb: move other Interface classes to new .h files

class DummyInterface : public Interface {
	string key;
	string schemaType;
	string schemaFile;
	string cacheFile;	// quoted
	int evaluation;
	int priority;
	string command;	// quoted
	string segment;	// quoted
public:
	static string typeName;
	DummyInterface() {}
	virtual ~DummyInterface(){}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DummyInterface(*this);	
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema();
	virtual Interface* copyTo(void* space){
		return new (space) DummyInterface(*this);
	}
};

class StandardInterface : public Interface{
protected:
	ISchema schema;
	string urlString;
public:
	static string typeName;
	StandardInterface(){}
	StandardInterface(const ISchema& schema, const string& urlString) 
		: schema(schema), urlString(urlString){}
	StandardInterface(const StandardInterface& x) :
		schema(x.schema), urlString(x.urlString) {}
	virtual ~StandardInterface(){}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new StandardInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << schema;
		out << " " << urlString;
		Interface::write(out);
	}
	virtual const ISchema* getISchema(){ // throws
		return &schema;
	}
	virtual Inserter* createInserter() const; // throws
        virtual Deleter* createDeleter() const; // throws
	virtual Interface* copyTo(void* space){
		return new (space) StandardInterface(*this);
	}
	virtual vector<AccessMethod*> createAccessMethods();
};

class GestaltInterface: public Interface { // *** YL			       
protected:
 	ISchema schema;
	string urlString;
public:
	static string typeName;
	GestaltInterface(){}
	GestaltInterface(const ISchema& schema, const string& urlString) 
		: schema(schema), urlString(urlString){}
	GestaltInterface(const GestaltInterface& x) :
		schema(x.schema), urlString(x.urlString) {}
	virtual ~GestaltInterface(){}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new GestaltInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << schema;
		out << " " << urlString; 
		Interface::write(out);
	}
	virtual const ISchema* getISchema(){ // throws
		return &schema;
	}
	virtual Inserter* createInserter() const; // throws
	virtual Interface* copyTo(void* space){
		return new (space) GestaltInterface(*this);
	}

        virtual vector<AccessMethod*> createAccessMethods();

	virtual bool isGestalt() const {return true;}
	vector<string> getMemberNames() const;
};
 
class ViewInterface : public Interface {
protected:
	int numFlds;
	string* attributeNames;
	string query;
	ISchema* schema;
public:
	static string typeName;
	ViewInterface() : 
		attributeNames(NULL), schema(NULL) {
	}
	ViewInterface(int numFlds, string* attributeNames, const string& query) 
		: numFlds(numFlds), attributeNames(attributeNames), 
		query(query), schema(NULL) {
		// cout << "ViewInterface constructor" << endl;
	}
	ViewInterface(const ViewInterface& a);
	virtual ~ViewInterface(){
		delete [] attributeNames;
		delete schema;
	}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new ViewInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual string guiRepresentation() const;
	virtual const ISchema* getISchema();
	virtual Type getType(){
		return VIEW;
	}
	string getQuery() const {
		return query;
	}
	virtual const string* getAttributeNames() const {
		return attributeNames;
	}
	virtual Interface* copyTo(void* space){
		return new (space) ViewInterface(*this);
	}

	virtual vector<AccessMethod*> createAccessMethods();
};

class MaterViewInterface : public Interface {
	ISchema schema;
	string urlString;

	int numFlds;
	string* attributeNames;
	string query;
public:
	static string typeName;
public:
	MaterViewInterface() {}
	MaterViewInterface(const DteTupleAdt& tupAdt,
                           const vector<string>& attributeNames,
                           const string& urlStr, const string& query);
	MaterViewInterface(const MaterViewInterface& a);
	virtual ~MaterViewInterface(){}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new MaterViewInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(){
		return &schema;
	}
	virtual Type getType(){
		return VIEW;
	}
	virtual const vector<string>& getAttributeNames() const {
		return schema.getAttributeNames();
	}
	virtual Interface* copyTo(void* space){
		return new (space) MaterViewInterface(*this);
	}
	virtual vector<AccessMethod*> createAccessMethods();
};

class DeviseInterface : public Interface{
	string schemaNm;
	string dataNm;
	string viewNm;
	ISchema* schema;
public:
	static string typeName;
	DeviseInterface() : schema(NULL) {}
	DeviseInterface(const DeviseInterface& x){
		schemaNm = x.schemaNm;
		dataNm = x.dataNm;
		viewNm = x.viewNm;
		if(x.schema){
			schema = new ISchema(*x.schema);
		}
		else{
			schema = NULL;
		}
	}
	virtual ~DeviseInterface(){
		delete schema;
	}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DeviseInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema();
	virtual Interface* copyTo(void* space){
		return new (space) DeviseInterface(*this);
	}
	virtual vector<AccessMethod*> createAccessMethods();
};

#if 0
//kb: QueryInterface class not used -- remove it
class QueryInterface : public Interface{
	string urlString;
	ISchema* schema;
public:
	static string typeName;
	QueryInterface() : schema(NULL) {}
	QueryInterface(const QueryInterface& x){
		urlString = x.urlString;
		if(x.schema){
			schema = new ISchema(*x.schema);
		}
		else{
			schema = NULL;
		}
	}
	virtual ~QueryInterface(){
		delete schema;
	}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new QueryInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << urlString;
		Interface::write(out);
	}
	virtual Type getType(){
		return QUERY;
	}
	virtual const ISchema* getISchema();	// throws exception
	virtual Interface* copyTo(void* space){
		return new (space) QueryInterface(*this);
	}
};
#endif


class CGIEntry{
public:
	string option;
	string value;
	istream& read(istream& in);	// throws
	void write(ostream& out) const;
};

class CGIInterface : public Interface{
	string urlString;
	int entryLen;
	CGIEntry* entries;
public:
	static string typeName;
	CGIInterface() : entries(NULL) {}
	virtual ~CGIInterface();
	CGIInterface(const CGIInterface& a);
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new CGIInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);  // throws
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema();
	virtual Interface* copyTo(void* space){
		return new (space) CGIInterface(*this);
	}
};

class CatalogInterface : public StandardInterface {
public:
	static string typeName;
	CatalogInterface() {}
	CatalogInterface(const string& fileName) 
		: StandardInterface(DIR_SCHEMA, fileName) {}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const; 
	string getFileName(){
		return urlString;
	}
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << urlString;
		Interface::write(out);
	}
	virtual string getCatalogName(){	// throws
		return urlString;
	}
	virtual Type getType(){
		return CATALOG;
	}
	virtual Interface* copyTo(void* space){
		return new (space) CatalogInterface(*this);
	}
};

// this really represents a remote table
class DBServerInterface : public Interface {
	string host;
	unsigned short port;
	ISchema* schema;
	TableName remTableName;
public:
	static string typeName;
	DBServerInterface() : schema(0) {}
	DBServerInterface(const DBServerInterface& x){
		host = x.host;
		port = x.port;
		schema = 0;
	}
	virtual ~DBServerInterface(){
		delete schema;
	}
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DBServerInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema();
	virtual Interface* copyTo(void* space){
		return new (space) DBServerInterface(*this);
	}
	virtual bool isRemote() const {return true;}
	virtual void setRemoteTableName(const TableName& name){
		remTableName = name;
	}
	virtual const TableName& getRemoteTableName() const {
		return remTableName;
	}
	const string& getHost() const {
		return host;
	}
	int getPort() const {
		return port;
	}
};

class ODBCInterface : public Interface {
	string connectString;
	string tableName;
	ODBC_Data* myODBC;
	int ODBC_Exist;

#if defined(_WINDOWS) || defined(_CONSOLE)
//	SQLRETURN SQL_Result;
#endif

	ISchema tmp;
public:
	static string typeName;
	ODBCInterface() {
		ODBC_Exist = 0;
	}
	virtual ~ODBCInterface();
	virtual const string& getTypeNm() const {
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new ODBCInterface(*this);
	}
  //virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema();
	virtual Interface* copyTo(void* space){
		return new (space) ODBCInterface(*this);
	}
};

#endif
