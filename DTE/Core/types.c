//kb: move ISchema stuff
#include "types.h"
#include "DTE/util/Del.h"

ISchema::ISchema(const string& str)
{
  //cerr << "reading schema: " << str << endl;
  istringstream in(str);
  read(in);
}


ISchema::ISchema(const DteTupleAdt& adt, const vector<string>& attrNames)
  : tupAdt(adt), attributeNames(attrNames)
{
  assert(tupAdt.getNumFields() == (int)attributeNames.size());
}

ISchema::ISchema(const DteAdtList& adts, const vector<string>& attrNames)
  : tupAdt(adts), attributeNames(attrNames)
{
  assert(tupAdt.getNumFields() == (int)attributeNames.size());
}

ISchema::ISchema(const ISchema& x)
  : tupAdt(x.tupAdt), attributeNames(x.attributeNames)
{
}

ISchema& ISchema::operator=(const ISchema& x)
{
  tupAdt = x.tupAdt;
  attributeNames = x.attributeNames;
  return *this;
}

void ISchema::read(istream& in) // Throws Exception
{
  int numFields;
  in >> numFields;
  if( !in ) return;

  DteTupleAdt newAdt;
  vector<string> newNames(numFields);
  string typeName;

  for(int i = 0 ; i < numFields ; i++) {
    in >> typeName;
    if( !in ) return;
    Del<DteAdt> adt = DteAdt::createAdt(typeName);
    newAdt.push_back(*adt);

    in >> newNames[i];
    if( !in ) return;
  }
  tupAdt = newAdt;
  attributeNames = newNames;
}

void ISchema::write(ostream& out) const
{
  int N = tupAdt.getNumFields(); 
  out << N;
  for(int i = 0 ; i < N ; i++) {
    out << ' ' << tupAdt.getAdt(i).getTypeSpec() << ' ' << attributeNames[i];
  }
}


istream& operator>>(istream& in, ISchema& s)
{
  s.read(in);
  return in;
}

ostream& operator<<(ostream& out, const ISchema& s)
{
  s.write(out);
  return out;
}


//---------------------------------------------------------------------------


string DteEnvVars::getDirectory(const string& envVar) const {
	const char* dmd = NULL;
	if((dmd = getenv(envVar.c_str()))){
	}
	else if((dmd = getenv("PWD"))){
	}
	else{
		string err("Could not read env. var ");
		err += envVar + " nor PWD";
		cerr << err << endl;
		exit(1);
	}
	assert(dmd);
	return dmd;
}

string DteEnvVars::getFile(const string& env, const string& def) const {
	char* nm = getenv(env.c_str());
	if(nm){
		return string(nm);
	}
	else{
		return def;
	}
}

DteEnvVars::DteEnvVars(){
	materViewDir = "DEVISE_MATER_DIR";
	minmaxDirN = "DEVISE_MINMAX_DIR";
	rootCatalogN = "DEVISE_HOME_TABLE";
	indexTableN = "DEVISE_INDEX_TABLE";
	minmaxCatalogN = "DEVISE_MINMAX_TABLE";
	definitionFile = "DEVISE_DEF_FILE";
	idFile = "DEVISE_ID_FILE";
	convert_bulk = "DEVISE_CONVERT_BULK";
	tempFileDir = "DEVISE_TEMP_FILE_DIR";

	minmaxDir = getDirectory(minmaxDirN);
	rootCatalog = getFile(rootCatalogN, "./catalog.dte");
	indexTable = getFile(indexTableN, "./sysind.dte");
	minmaxCatalog = getFile(minmaxCatalogN, "./minmax.dte");
}

string DteEnvVars::valueOf(const string& envVar) const {
	if(envVar == convert_bulk){
		return getFile(envVar, "./RTree/convert_bulk");
	}
	else if(envVar == definitionFile){
		return getFile(envVar);
	}
	else{
		return getFile(envVar);
	}
}


//---------------------------------------------------------------------------

#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteEolStringAdt.h"
#include "DTE/types/DteDateAdt.h"


//kb:move this
static char getEscapeChar(char c)
{
  switch(c) {
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  }
  return c;
}

//kb:move this
#include <ctype.h>
void toLower(string& x)
{
  for(string::iterator i = x.begin() ; i != x.end() ; i++) {
    *i = tolower(*i);
  }
}

//kb:move this
DteAdt* DteAdt::createAdt(const string& type) //kb: move this
{
  //kb: createAdt is somewhat of a hack
  string t(type);
  toLower(t);

  if( t == "int" ) {
    return new DteIntAdt();
  }
  if( t == "double" ) {
    return new DteDoubleAdt();
  }
  if(t.substr(0, 6) == "string") {
    const char* cp = t.c_str() + 6;
    if( *cp == '[' || *cp == '(' ) cp++;
    int len = atoi(cp);
    if( len < 1 ) {
      cerr << "warning: no string length, assuming maximum of 100 bytes\n";
      len = 100;
    }
    return new DteStringAdt(len);
  }
  if(t.substr(0, 9) == "eolstring") {
    istringstream in(t);
    in.ignore(100, '(');
    int len;
    in >> len;
    char c;
    in >> c;
    assert(c == ',');
    in >> c;
    assert(c == '\'');
    char terminator;
    in >> terminator;
    if( terminator == '\\' ) {
      in >> c;
      terminator = getEscapeChar(c);
    }
    in >> c;
    assert(c == '\'');
    in >> c;
    assert(c == ')');
    if( !in ) {
      cerr << "error parsing type: " << t << endl;
      assert(0);
    }
    return new DteEolStringAdt(len, terminator);
  }
  if( t == "date" ) {
    return new DteDateAdt();
  }
  cerr << "Type " << t << " is not supported\n";
  assert(!"bad type");
  return NULL;
}
