#include "types.h"
#include "Interface.h"
#include "site.h"
#include "DevRead.h"
#include "DataRead.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "Engine.h"
#include "catalog.h"
#include "SockStream.h"
#include "ParseTree.h"
#include <string>
#include "sysdep.h"
#include "Utility.h"
#include "AccessMethod.h"
#include "DteProtocol.h"

string ViewInterface::typeName = "SQLView";
string MaterViewInterface::typeName = "MaterView";
string CGIInterface::typeName = "CGIInterface";
string QueryInterface::typeName = "DEVise";
string StandardInterface::typeName = "StandardTable";
string CatalogInterface::typeName = "Directory";
string DeviseInterface::typeName = "Table";
string DummyInterface::typeName = "UNIXFILE";
string ODBCInterface::typeName = "ODBC";
string DBServerInterface::typeName = "DBServer";
string GestaltInterface::typeName = "Gestalt"; // *** YL

const string Stats::KEYWD = "stats";

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
	max = MAX_VAL(sizeof(DBServerInterface), max);
	max = MAX_VAL(sizeof(ODBCInterface), max);
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
	schema = ISchema(numFlds, typeIDs, attributeNames);
	urlString = url;
	this->numFlds = numFlds;
	this->attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		this->attributeNames[i] = attributeNames[i];
	}
	this->query = query;
}

MaterViewInterface::MaterViewInterface(const MaterViewInterface& a){
	schema = a.schema;
	urlString = a.urlString;
	numFlds = a.numFlds;
	attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		attributeNames[i] = a.attributeNames[i];
	}
	query = a.query;
}

istream& MaterViewInterface::read(istream& in){
	in >> schema;
	in >> urlString;
	numFlds = schema.getNumFlds();
	const string* attrs = schema.getAttributeNames();
	attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		attributeNames[i] = attrs[i];
	}
	CHECK(stripQuotes(in, query), 
		"Incorrect MaterViewInterface format", in);
	return Interface::read(in);
}

void MaterViewInterface::write(ostream& out) const {
	out << typeName;
	out << " " << schema;
	out << " " << urlString;
	out << " " << addQuotes(query);
	Interface::write(out);
}

Site* MaterViewInterface::getSite(){ // Throws a exception

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	delete url;

	StandardRead* sr = new StandardRead();
	TRY(sr->open(schema, in, urlString), NULL);

	RidAdder* planOp = new RidAdder(sr);
     return new LocalTable("", planOp, urlString);	
}

Site* DeviseInterface::getSite(){
//	cerr << schemaNm.substr(schemaNm.size() - 3) << endl;
	if(schemaNm.substr(schemaNm.size() - 3) == "ddr"){

		// Devise Data Reader

		TRY(DataRead* reader = new DataRead(schemaNm, dataNm), 0);
		return new LocalTable("", reader);

	}

	// Unidata Reader

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
	
	// table is OK to be NULL since it is not used

	if(schema){
		return schema;
	}
	int numFlds;
	const string* attributeNames;
	const TypeID* typeIDs;
	assert(viewNm.empty());		// obsolete feature

	// DataReaderAM incorporates both DataReader and UniData

	TRY(DataReaderAM* reader = new DataReaderAM(schemaNm, dataNm), 0);
	assert(reader);
	numFlds = reader->getNumFlds();
	attributeNames = reader->getAttributeNames();
	typeIDs = reader->getTypeIDs();
	schema = new ISchema(numFlds, typeIDs, attributeNames);
	delete reader;

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

// *** YL
Site* GestaltInterface::getSite(){ // Throws a exception

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	StandardRead* unmarshal = new StandardRead();
	TRY(unmarshal->open(schema, in, urlString), NULL);

	delete url;
     return new LocalTable("", unmarshal, urlString);	
}

Inserter* GestaltInterface::getInserter(TableName* table){ // Throws 
	assert(table);
	assert(table->isEmpty());

	LOG(logFile << "Inserting into " << urlString << endl);

	Inserter* inserter = new GestaltInserter();
	TRY(inserter->open(schema, urlString), NULL);

     return inserter;
}

/*
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
*/

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
	return Interface::read(in);
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
		char tmp;
		in >> tmp;
		in.putback(tmp);
		if(tmp == '"'){
			stripQuotes(in, attributeNames[i]);
		}
		else{
			in >> attributeNames[i];
		}
	}
	CHECK(stripQuotes(in, query), 
		"Incorrect ViewInterface format", in);
	schema = NULL;
	return Interface::read(in);
}

void ViewInterface::write(ostream& out) const {
	out << typeName << " " << numFlds;
	for(int i = 0; i < numFlds; i++){
		out << " " << attributeNames[i];
	}
	out << " " << addQuotes(query);
	Interface::write(out);
}
 
string ViewInterface::guiRepresentation() const {
	string retVal(typeName);
	retVal += " \"";
	assert(numFlds > 0);
	int i = 0;
	for(i = 0; i < numFlds - 1; i++){
		retVal += attributeNames[i] + ", ";
	}
	retVal += attributeNames[i];
	retVal += "\" ";
	Engine engine(query);
	TRY(const ParseTree* parseTree = engine.parse(), "");
	TRY(retVal += parseTree->guiRepresentation(), "");
	return retVal;
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

	// typecheck the query

	Engine engine(query);
	TRY(const ISchema* tmp = engine.typeCheck(), 0);
	const TypeIDList& tmpTypes = tmp->getTypeIDs();

	string* retVal = new string[numFlds + 1];
	TypeID* types = new TypeID[numFlds + 1];
	retVal[0] = "recId";
	types[0] = INT_TP;
	for(int i = 0; i < numFlds; i++){
		retVal[i + 1] = attributeNames[i];
		types[i + 1] = tmpTypes[i];
	}
	schema = new ISchema(numFlds + 1, types, retVal);

	delete [] types;
	delete [] retVal;

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
	return Interface::read(in);
}

istream& DeviseInterface::read(istream& in){
	in >> schemaNm >> dataNm;
	CHECK(stripQuotes(in, viewNm), 
		"Incorrect DeviseInterface format", in);
	return Interface::read(in);
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

istream& DBServerInterface::read(istream& in){
	in >> host;
	in >> port;
	if(!in){
		THROW(new Exception("Incorrect DBServerInterface format"), in);
	}
	return Interface::read(in);
}

void DBServerInterface::write(ostream& out) const {
	out << typeName << " ";
	out << host << " " << port;
	Interface::write(out);
}

const ISchema* DBServerInterface::getISchema(TableName* table){
	if(schema){
		return schema;
	}
	schema = new ISchema;
	DteProtocol dteProt(host, port);
	TRY(ostream& ostr = dteProt.getOutStream(), 0);
	ostr << "schema " << table->toString() << ";" << flush;

	TRY(istream& istr = dteProt.getInStream(), 0);
	istr >> *schema;
	if(!istr){
		string err = "Illegal schema received from the DB server";
		THROW(new Exception(err), NULL);
	}
	return schema;
}

Site* DBServerInterface::getSite(){
	return new DBServerSite(host, port);
}

istream& ODBCInterface::read(istream& in){
	in >> connectString;
	in >> tableName;
	if(!in){
		THROW(new Exception("Incorrect ODBCInterface format"), in);
	}
	return Interface::read(in);
}

void ODBCInterface::write(ostream& out) const {
	out << typeName << " " << connectString << " ";
	out << tableName;
	Interface::write(out);
}

void DummyInterface::write(ostream& out) const {
	out << typeName;
	out << " " << key << " " << schemaType << " " << schemaFile; 
	out << " " << addQuotes(cacheFile) << " " << evaluation;
	out << " " << priority << " " << addQuotes(command);
	out << " " << addQuotes(segment);
	Interface::write(out);
}

void DeviseInterface::write(ostream& out) const{
	out << typeName;
	out << " " << schemaNm;
	out << " " << dataNm;
	out << " " << addQuotes(viewNm);
	Interface::write(out);
}

istream& Interface::read(istream& in)
{
     string next;
     in >> next;
	while(in && !(next == string(";"))){
		if(next == Stats::KEYWD){
		
			// read in statistics

			stats = new Stats();
			stats->read(in);
		}
		else{
			string msg = "Invalid catalog format: " + Stats::KEYWD +
				" expected instead of \"" + next + "\"";
			THROW(new Exception(msg), in);
		}
	}
	if(!in){
		string msg = "Catalog entry must end with semicolon";
		THROW(new Exception(msg), in);
	}
	return in; // Avoid Visual C++ error : not all control paths return a value
}

istream& StandardInterface::read(istream& in){
	in >> schema;
	in >> urlString;
	return Interface::read(in);
}

istream& GestaltInterface::read(istream& in){
	in >> schema;
	in >> urlString;
	return Interface::read(in);
}

vector<AccessMethod*> StandardInterface::createAccessMethods()
{
	vector<AccessMethod*> retVal;
	Stats defStats(schema.getNumFlds());
	Stats* nonNullStats = (stats ? stats : &defStats);
	AccessMethod* sr = new StandardAM(schema, urlString, *nonNullStats);
	retVal.push_back(sr);
	return retVal;
}

vector<AccessMethod*> DeviseInterface::createAccessMethods()
{
	vector<AccessMethod*> retVal;
	const ISchema* tmpSchema = getISchema(0);
	Stats defStats(tmpSchema->getNumFlds());
	Stats* nonNullStats = (stats ? stats : &defStats);
	AccessMethod* sr = new DataReaderAM(schemaNm, dataNm);
	retVal.push_back(sr);
	return retVal;
}

// *** YL 
vector<AccessMethod*> GestaltInterface::createAccessMethods()
{
	vector<AccessMethod*> retVal;
	return retVal;

/*
	Stats defStats(schema.getNumFlds());
	Stats* nonNullStats = (stats ? stats : &defStats);
//	AccessMethod* sr = new GestaltAM(schema, urlString, getMemberNames(), *nonNullStats);
//	retVal.push_back(sr);
	return retVal; 
*/
}


istream& QueryInterface::read(istream& in){
	in >> urlString;
	return Interface::read(in);
}

istream& CatalogInterface::read(istream& in){
	in >> urlString;
	return Interface::read(in);
}

vector<AccessMethod*> Interface::createAccessMethods()
{
	vector<AccessMethod*> emptyVec;
	return emptyVec;
}

vector<string> GestaltInterface::getMemberNames() const
{
	ifstream memStream(urlString.c_str());
	assert(memStream);
	vector<string> retVal;
	while(memStream){
		string tmp;
		memStream >> tmp;
		if(memStream){
			retVal.push_back(tmp);
		}
	}
	return retVal;
}
