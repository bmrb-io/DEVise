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

#ifndef __GNUG__
using namespace std;
#endif

class Inserter;
class TableName;
class Site;
class ISchema;
class ODBC_Data;

class Interface{
public:
	enum Type {UNKNOWN, CATALOG, QUERY, VIEW, ODBC, DB_SERVER};
	Interface() {}
	virtual Interface* duplicate() const = 0;
	virtual ~Interface(){}
	virtual Site* getSite() = 0;
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << " ;";
	}
	virtual string guiRepresentation() const {
		ostringstream os;
		write(os);
		os << ends;
		return os.str();
	}
	virtual Type getType(){
		return UNKNOWN;
	}
	virtual const ISchema* getISchema(TableName* table) = 0;	// throws
	virtual Inserter* getInserter(TableName* table); // throws
	virtual string getTypeNm() = 0;
	virtual Interface* copyTo(void* space) = 0;
};

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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DummyInterface(*this);	
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table);
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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new StandardInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << schema;
		out << " " << urlString;
		Interface::write(out);
	}
	virtual const ISchema* getISchema(TableName* table){ // throws
		return &schema;
	}
	virtual Inserter* getInserter(TableName* table); // throws
	virtual Interface* copyTo(void* space){
		return new (space) StandardInterface(*this);
	}
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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new ViewInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual string guiRepresentation() const;
	virtual const ISchema* getISchema(TableName* table);
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
	MaterViewInterface(
		int numFlds, const TypeID* typeIDs, const string* attributeNames,
		const string& urlStr, const string& query);
	MaterViewInterface(const MaterViewInterface& a);
	virtual ~MaterViewInterface(){}
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new MaterViewInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table){
		return &schema;
	}
	virtual Type getType(){
		return VIEW;
	}
	virtual const string* getAttributeNames() const {
		return schema.getAttributeNames();
	}
	virtual Interface* copyTo(void* space){
		return new (space) MaterViewInterface(*this);
	}
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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DeviseInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table);
	virtual Interface* copyTo(void* space){
		return new (space) DeviseInterface(*this);
	}
};

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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new QueryInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << urlString;
		Interface::write(out);
	}
	virtual Type getType(){
		return QUERY;
	}
	virtual const ISchema* getISchema(TableName* table);	// throws exception
	virtual Interface* copyTo(void* space){
		return new (space) QueryInterface(*this);
	}
};


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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new CGIInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);  // throws
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table);
	virtual Interface* copyTo(void* space){
		return new (space) CGIInterface(*this);
	}
};

class CatalogInterface : public Interface {
	string fileName;
public:
	static string typeName;
	CatalogInterface() {}
	CatalogInterface(const string& fileName) : fileName(fileName) {}
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const; 
	string getFileName(){
		return fileName;
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << fileName;
		Interface::write(out);
	}
	virtual string getCatalogName(){	// throws
		return fileName;
	}
	virtual Type getType(){
		return CATALOG;
	}
	virtual const ISchema* getISchema(TableName* table);	// throws exception
	virtual Interface* copyTo(void* space){
		return new (space) CatalogInterface(*this);
	}
};

class DBServerInterface : public Interface {
	string host;
	unsigned short port;
	ISchema* schema;
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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new DBServerInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table);
	virtual Interface* copyTo(void* space){
		return new (space) DBServerInterface(*this);
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
	virtual string getTypeNm(){
		return typeName;
	}
	virtual Interface* duplicate() const {
		return new ODBCInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table);
	virtual Interface* copyTo(void* space){
		return new (space) ODBCInterface(*this);
	}
};

#endif
