#include "types.h"
#include "Interface.h"
#include "site.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "Engine.h"
#include "catalog.h"
#include <string>

string ViewInterface::typeName = "SQLView";
string MaterViewInterface::typeName = "MaterView";
string CGIInterface::typeName = "CGIInterface";
string QueryInterface::typeName = "DEVise";
string StandardInterface::typeName = "StandardTable";
string CatalogInterface::typeName = "Directory";
string DeviseInterface::typeName = "Table";
string DummyInterface::typeName = "UNIXFILE";

inline int MAX_VAL(int i, int j){
	return (i > j ? i : j);
}

static int findMaxInterfSize(){
	int max = 0;
	max = MAX_VAL(sizeof(ViewInterface), max);
	max = MAX_VAL(sizeof(MaterViewInterface), max);
	max = MAX_VAL(sizeof(CGIInterface), max);
	max = MAX_VAL(sizeof(QueryInterface), max);
	max = MAX_VAL(sizeof(StandardInterface), max);
	max = MAX_VAL(sizeof(CatalogInterface), max);
	max = MAX_VAL(sizeof(DeviseInterface), max);
	max = MAX_VAL(sizeof(DummyInterface), max);
//	cerr << "INITIAL_INTERFACE_SIZE = " << max << endl;
	return max;
}

const int INITIAL_INTERFACE_SIZE = findMaxInterfSize();

ViewInterface::ViewInterface(const ViewInterface& a){
	numFlds = a.numFlds;
	if(a.attributeNames){
		attributeNames = new string[numFlds];
		for(int i = 0; i < numFlds; i++){
			attributeNames[i] = a.attributeNames[i];
		}
	}
	else{
		attributeNames = NULL;
	}
	query = a.query;
	if(a.schema){
		schema = new ISchema(*a.schema);
	}
	else{
		schema = NULL;
	}
}

MaterViewInterface::MaterViewInterface(
	int numFlds, const TypeID* typeIDs, const string* attributeNames,
	const string& url, const string& query)
{
	StandardInterface::schema = ISchema(numFlds, typeIDs, attributeNames);
	StandardInterface::urlString = url;
	ViewInterface::numFlds = numFlds;
	ViewInterface::attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		ViewInterface::attributeNames[i] = attributeNames[i];
	}
	ViewInterface::schema = NULL;
	ViewInterface::query = query;
}

MaterViewInterface::MaterViewInterface(const MaterViewInterface& a){
	StandardInterface::schema = a.StandardInterface::schema;
	StandardInterface::urlString = a.StandardInterface::urlString;
	ViewInterface::numFlds = a.ViewInterface::numFlds;
	ViewInterface::attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		ViewInterface::attributeNames[i] = a.attributeNames[i];
	}
	ViewInterface::schema = NULL;
	ViewInterface::query = a.ViewInterface::query;
}

istream& MaterViewInterface::read(istream& in){
	StandardInterface::read(in);
	numFlds = StandardInterface::schema.getNumFlds();
	const string* attrs = StandardInterface::schema.getAttributeNames();
	ViewInterface::attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		ViewInterface::attributeNames[i] = attrs[i];
	}
	CHECK(stripQuotes(in, query), 
		"Incorrect MaterViewInterface format", in);
	ViewInterface::schema = NULL;
	return in;
}

void MaterViewInterface::write(ostream& out) const {
	out << typeName;
	out << " " << StandardInterface::schema;
	out << " " << urlString;
	out << " " << addQuotes(query);
//	ViewInterface::Interface::write(out);	// does not work for some reason
	out << " ;";
}

Site* MaterViewInterface::getSite(){ // Throws a exception

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	delete url;

	StandardRead* sr = new StandardRead();
	TRY(sr->open(StandardInterface::schema, in, urlString), NULL);

	RidAdder* planOp = new RidAdder(sr);
     return new LocalTable("", planOp, urlString);	
}

Site* DeviseInterface::getSite(){
	char* schemaFile = strdup(schemaNm.c_str());
	char* data = strdup(dataNm.c_str());
	DevRead* unmarshal = new DevRead();
	TRY(unmarshal->Open(schemaFile, data), NULL);
	if(viewNm == ""){
		return new LocalTable("", unmarshal);	
	}

	assert(!"single table views are broken");
	return NULL;
/*
	// Need to apply view

	string select;
	string* attributeNames;
	string where;
	int numFlds;

	TRY(readFilter(viewNm, select, attributeNames, numFlds, where), NULL);

	string from = "Table " + schemaNm +  " " + dataNm + " \"\" ;";
	string query = "select " + select + " from " + addQuotes(from) + " as t";
	if(!where.empty()){
		query += " where " + where;
	}
	ViewInterface tmpView(numFlds, attributeNames, query);

	// attributeNames are deleted in ViewInterface destructor

	TRY(Site* retVal = tmpView.getSite(), NULL);
	return retVal;
*/
}

const ISchema* DeviseInterface::getISchema(TableName* table){
	if(schema){
		return schema;
	}
	assert(table);
	assert(table->isEmpty());
	int numFlds;
	string* attributeNames;
	TypeID* typeIDs;
	if(viewNm.empty()){
		char* schemaFile = strdup(schemaNm.c_str());
		char* data = strdup(dataNm.c_str());
		DevRead tmp;
		TRY(tmp.Open(schemaFile, data), NULL);
		numFlds = tmp.getNumFlds();
		attributeNames = tmp.stealAttributeNames();
		typeIDs = tmp.stealTypeIDs();
	}
	else {
		assert(!"single table views not implemented");
		/*
		string select;
		string where;
		TRY(readFilter(viewNm, select, attributeNames, numFlds, where), NULL);
		*/
	}
	schema = new ISchema(typeIDs, attributeNames, numFlds);
	return schema;
}

Site* StandardInterface::getSite(){ // Throws a exception

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	StandardRead* unmarshal = new StandardRead();
	TRY(unmarshal->open(schema, in, urlString), NULL);

	delete url;
     return new LocalTable("", unmarshal, urlString);	
}

Inserter* StandardInterface::getInserter(TableName* table){ // Throws 
	assert(table);
	assert(table->isEmpty());

	LOG(logFile << "Inserting into " << urlString << endl);

	Inserter* inserter = new Inserter();
	TRY(inserter->open(schema, urlString), NULL);

     return inserter;
}

Site* CatalogInterface::getSite(){ // Throws a exception
	Interface* interf = new StandardInterface(DIR_SCHEMA, fileName);
	Site* retVal = interf->getSite();
	delete interf;
	return retVal;
}

const ISchema* CatalogInterface::getISchema(TableName* table){
	assert(table);
	assert(table->isEmpty());
	return &DIR_SCHEMA;
}

istream& CGIInterface::read(istream& in){  // throws
	in >> urlString;
	in >> entryLen;
	if(!in){
		string e = "Catalog error: number of entries expected";
		THROW(new Exception(e), in);
		// throw Exception(e);
	}

	entries = new CGIEntry[entryLen];
	for(int i = 0; i < entryLen; i++){
		TRY(entries[i].read(in), in);
	}
	return in;
}

istream& ViewInterface::read(istream& in){ // throws
	in >> numFlds;
	if(!in){
		string e = "Number of attributes expected";
		THROW(new Exception(e), in);
		// throw Exception(e);
	}
	attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		in >> attributeNames[i];
	}
	CHECK(stripQuotes(in, query), 
		"Incorrect ViewInterface format", in);
	schema = NULL;
	return in;
}

void ViewInterface::write(ostream& out) const {
	out << typeName << " " << numFlds;
	for(int i = 0; i < numFlds; i++){
		out << " " << attributeNames[i];
	}
	out << " " << addQuotes(query);
	Interface::write(out);
}
 
Site* ViewInterface::getSite(){ 
	// Throws a exception
	
	// This is just a temporary hack because it does not provide
	// any optimization. View is executed unmodified.

	Engine* engine = new ViewEngine(query, 
		(const string *) attributeNames, numFlds);
	TRY(engine->optimize(), NULL);
	int numEngFlds = engine->getNumFlds();
	if(numEngFlds != numFlds + 1){
		ostringstream estr;
		estr << "Number of fields in the view (" << numFlds << ") ";
		estr << "is not equal to the number in the query ";
		estr << "(" << numEngFlds << ")" << ends;
		string except = estr.str();
		THROW(new Exception(except), NULL);
		// throw Exception(except);
	}
	return new LocalTable("", engine); 
}

const ISchema* QueryInterface::getISchema(TableName* table){

	// throws exception

	if(schema){
		return schema;
	}
	int count = 2;
	string* options = new string[count];
	string* values = new string[count];
	options[0] = "query";
	options[1] = "execute";
	stringstream tmp;
	tmp << "schema ";
	table->display(tmp);
	tmp << ends;
	values[0] = tmp.str() ;
	values[1] = "true";
	istream* in;
	TRY(in = contactURL(urlString, options, values, count), NULL);
	delete [] options;
	delete [] values;
	schema = new ISchema();
	*in >> *schema;
	return schema;
}

Site* QueryInterface::getSite(){
	return new Site(urlString);
}

Site* CGIInterface::getSite(){
	assert(entries);
	Site* site = new CGISite(urlString, entries, entryLen);
	// CGISite is the owner of the entries
	return site;
}

CGIInterface::~CGIInterface(){
	delete [] entries;
}

CGIInterface::CGIInterface(const CGIInterface& a){
	urlString = a.urlString;
	entryLen = a.entryLen;
	entries = new CGIEntry[entryLen];
	for(int i = 0; i < entryLen; i++){
		entries[i] = a.entries[i];
	}
}

void CGIInterface::write(ostream& out) const {
	assert(entries);
	out << typeName << " " << urlString << " " << entryLen;
	for(int i = 0; i < entryLen; i++){
		out << " ";
		entries[i].write(out);
	}
	Interface::write(out);
}

const ISchema* ViewInterface::getISchema(TableName* table){
	if(schema){
		return schema;
	}
	assert(table);
	assert(table->isEmpty());
	assert(attributeNames);
	string* retVal = new string[numFlds + 1];
	retVal[0] = "recId";
	for(int i = 0; i < numFlds; i++){
		retVal[i + 1] = attributeNames[i];
	}
	// to do: typecheck the query

	schema = new ISchema(NULL, retVal, numFlds + 1);
	return schema;
}

istream& CGIEntry::read(istream& in){	// throws
	in >> option;
	char tmp;
	in >> tmp;
	if(tmp != '"'){
		THROW(new Exception("Wrong format in the CGIInterface"), in);
		// throw Exception("Wrong format in the CGIInterface");
	}
	bool escape = false;
	while(1){
		in.get(tmp);
		if(!in){
			string e = "Catalog ends while reading CGIInterface";
			THROW(new Exception(e), in);
			// throw Exception(e);
		}
		if(tmp == '\\'){
			if(escape){
				value += '\\';
				escape = false;
			}
			else{
				escape = true;
			}
		}
		else if(tmp == '"'){
			if(escape){
				value += '"';
				escape = false;
			}
			else{
				break;
			}
		}
		else{
			if(escape){
				value += '\\' + tmp;
				escape = false;
			}
			else{
				value += tmp;
			}
		}
	}
	return in;
}

void CGIEntry::write(ostream& out) const {
	assert(value.find('"') == string::npos);	// encode these characters!
	assert(value.find('\\') == string::npos);
	out << option << " " << "\"" << value << "\"";
}

/*
void insert(string tableStr, Tuple* tuple){	// throws exception
	assert(!"broken");
	Catalog* catalog = getRootCatalog();
	assert(catalog);
	TableName tableName(tableStr.c_str());
	TRY(Interface* interf = catalog->createInterface(&tableName), NVOID );
	delete catalog;
	TRY(Inserter* inserter = interf->getInserter(&tableName), NVOID );
	delete interf;
	inserter->insert(tuple);
	delete inserter;
}
*/

Interface* CatalogInterface::duplicate() const {
	return new CatalogInterface(*this);
}

istream& DummyInterface::read(istream& in){
	if(!in){
		THROW(new Exception("Wrong format"), in);
		// throw Exception("Wrong format");
	}
	in >> key >> schemaType >> schemaFile;
	CHECK(stripQuotes(in, cacheFile), 
		"Incorrect DummyInterface format", in);
	in >> evaluation >> priority;
	CHECK(stripQuotes(in, command), 
		"Incorrect DummyInterface format", in);
	CHECK(stripQuotes(in, segment), 
		"Incorrect DummyInterface format", in);
	return in;
}

istream& DeviseInterface::read(istream& in){
	in >> schemaNm >> dataNm;
	CHECK(stripQuotes(in, viewNm), 
		"Incorrect ViewInterface format", in);
	return in;
}

Inserter* Interface::getInserter(TableName* table){ // throws
	THROW(new Exception("Insertion not supported"), NULL);
//	throw Exception("Insertion not supported");
}

Site* DummyInterface::getSite(){
	string err = "Operations on old DEVise table are not supported";
	THROW(new Exception(err), NULL);
//	throw Exception(err);
}

const ISchema* DummyInterface::getISchema(TableName* table){
	string msg = "ISchema lookup not supported for UNIXFILEs";
	THROW(new Exception(msg), NULL);
//	throw Exception(msg);
}

const ISchema* CGIInterface::getISchema(TableName* table){
	string msg = "ISchema lookup not yet implemented for CGIs";
	THROW(new Exception(msg), NULL);
//	throw Exception(msg);
}
