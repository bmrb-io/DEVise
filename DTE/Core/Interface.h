#ifndef DTE_INTERFACE_H
#define DTE_INTERFACE_H

#include <string>
//#include <iostream.h>   erased for sysdep.h
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

class Inserter;
class TableName;
class Site;
class ISchema;

class Interface{
public:
	enum Type {UNKNOWN, CATALOG, QUERY, VIEW};
	Interface() {}
	virtual Interface* duplicate() const = 0;
	virtual ~Interface(){}
	virtual Site* getSite() = 0;
	virtual istream& read(istream& in) = 0;
	virtual ostream* getOutStream(){
		assert(0);
		return NULL;
	}
	virtual string getFileName(){
		assert(0);
		string tmp;
		return tmp;
	}
	virtual void write(ostream& out) const {
		out << " ;";
	}
	virtual Type getType(){
		return UNKNOWN;
	}
	virtual const ISchema* getISchema(TableName* table) = 0;	// throws
	virtual Inserter* getInserter(TableName* table){ // throws
		THROW(new Exception("Insertion not supported"), NULL);
	}
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
	virtual Site* getSite(){
		string err = "Operations on old DEVise table are not supported";
		THROW(new Exception(err), NULL);
	}
	virtual istream& read(istream& in){
		if(!in){
			THROW(new Exception("Wrong format"), in);
		}
		in >> key >> schemaType >> schemaFile;
		TRY(stripQuotes(in, cacheFile), in);
		in >> evaluation >> priority;
		TRY(stripQuotes(in, command), in);
		TRY(stripQuotes(in, segment), in);
		return in;
	}
	virtual void write(ostream& out) const {
		out << typeName;
		out << " " << key << " " << schemaType << " " << schemaFile; 
		out << " " << addQuotes(cacheFile) << " " << evaluation;
		out << " " << priority << " " << addQuotes(command);
		out << " " << addQuotes(segment);
		Interface::write(out);
	}
	virtual const ISchema* getISchema(TableName* table){
		string msg = "ISchema lookup not supported for UNIXFILEs";
		THROW(new Exception(msg), NULL);
	}
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
	virtual istream& read(istream& in){
		in >> schema;
		return in >> urlString;
	}
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

class MaterViewInterface : public ViewInterface, public StandardInterface {
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
		return (ViewInterface*) new MaterViewInterface(*this);
	}
	virtual Site* getSite();
	virtual istream& read(istream& in);
	virtual void write(ostream& out) const;
	virtual const ISchema* getISchema(TableName* table){
		return ViewInterface::getISchema(table);
	}
	virtual Type getType(){
		return ViewInterface::getType();
	}
	virtual const string* getAttributeNames() const {
		return StandardInterface::schema.getAttributeNames();
	}
	virtual Interface* copyTo(void* space){
		return (ViewInterface*) new (space) MaterViewInterface(*this);
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
	virtual istream& read(istream& in){
		in >> schemaNm >> dataNm;
		stripQuotes(in, viewNm);
		return in;
	}
	virtual void write(ostream& out) const{
		out << typeName;
		out << " " << schemaNm << endl;
		out << "\t" << dataNm << endl;
		out << "\t" << addQuotes(viewNm);
		Interface::write(out);
	}
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
	virtual istream& read(istream& in){
		in >> urlString;
		return in;
	}
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
	virtual const ISchema* getISchema(TableName* table){
		string msg = "ISchema lookup not yet implemented for CGIs";
		THROW(new Exception(msg), NULL);
	}
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
	virtual string getFileName(){
		return fileName;
	}
	virtual Site* getSite();
	virtual istream& read(istream& in){
		return in >> fileName;
	}
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

#endif
