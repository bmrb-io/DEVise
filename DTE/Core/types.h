#ifndef _TYPES_H_
#define _TYPES_H_

#include "exception.h"
#include "DTE/misc/DteConfig.h"
#include "DTE/types/DteAdt.h"
#include "DTE/types/DteTupleAdt.h"
//kb: types.h no longer used -- clean up stuff in here

//kb: use of TypeID should be changed to DteAdtRef
//typedef int TypeID;
//typedef vector<int> TypeIDList;

const string RID_STRING = "recId";

class ISchema
{
  DteTupleAdt tupAdt;
  vector<string> attributeNames;

public:

  ISchema() {}

  // reads from the string
  ISchema(const string& str); 

  ISchema(const DteTupleAdt& adt, const vector<string>& attrNames);

  ISchema(const DteAdtList& adt, const vector<string>& attrNames);

  ISchema(const ISchema& x);

  ~ISchema() {}

  ISchema& operator=(const ISchema& x);

  ISchema operator+(const ISchema& x) const;

  void read(istream& in); // Throws Exception

  void write(ostream& out) const;

  void push_front(const DteAdt& adt, const string& name) {
    tupAdt.push_front(adt);
    attributeNames.insert(attributeNames.begin(), name);
  }

  void push_back(const DteAdt& adt, const string& name) {
    tupAdt.push_back(adt);
    attributeNames.push_back(name);
  }

  int getNumFields() const {
    return tupAdt.getNumFields();
  }

  const vector<string>& getAttributeNames() const {
    return attributeNames;
  }

  const void setAttributeNames(const vector<string>& names) {
    assert((int)names.size() == tupAdt.getNumFields());
    attributeNames = names;
  }

  const DteTupleAdt& getAdt() const {
    return tupAdt;
  }

  static const ISchema* getISchema(const Type* arg){
    return (const ISchema*) arg;
  }

  friend istream& operator>>(istream& in, ISchema& s);
  friend ostream& operator<<(ostream& out, const ISchema& s);
};

extern const ISchema DIR_SCHEMA;
extern const ISchema INDEX_SCHEMA;
extern const ISchema STAT_SCHEMA;
extern const ISchema STRING_SCHEMA;
extern const ISchema EMPTY_SCHEMA;
extern const ISchema SCHEMA_SCHEMA;


class NewStat{
        int numPgs;
public:
        NewStat(){
                numPgs = 1000;
        }
        int getNumPgs() const {
                return numPgs;
        }
};

extern const NewStat DEFAULT_STAT;


class Offset{
public:
  int offset;
  friend ostream& operator<<(ostream& out, Offset offset);
public:
  Offset() : offset(-1){}
  Offset(int offset) : offset(offset){}
  int getOffset(){
    return offset;
  }
  bool isNull(){
    return offset == -1;
  }
};



class Field
{
public:

  Field(const DteAdt& adt, int pos)
    : adt(adt.clone()), right(false), pos(pos) {}
  
  Field(const DteAdt& adt, bool right, int pos) // right means second tuple
    : adt(adt.clone()), right(right), pos(pos) {}
  
  ~Field() { delete adt; }
  
  Field(const Field& x) : adt(x.adt->clone()), right(x.right), pos(x.pos) {}
  
  Field& operator=(const Field& x) {
    adt = x.adt;
    right = x.right;
    pos = x.pos;
    return *this;
  }
  
  const DteAdt& getAdt() const { return *adt; }
  
  int getPos() const { return pos; }

  bool isLeft() const { return !right; }

  bool isRight() const { return right; }

protected:

  DteAdt* adt;
  bool    right;
  int     pos;

#if 0
  // needed for gcc 2.7
  Field() {}
  friend class vector<Field>;
#endif
};

typedef vector<Field> FieldList;

const int DTE_SERVER_ID = 1;

class DteEnvVars {
public:
	string materViewDir;
	string minmaxDirN;
	string rootCatalogN;
	string indexTableN;
	string minmaxCatalogN;
	string definitionFile;
	string idFile;
	string convert_bulk;
	string tempFileDir;

	string minmaxDir;
	string rootCatalog;
	string indexTable;
	string minmaxCatalog;
public:
	DteEnvVars();
	string valueOf(const string& envVar) const;
private:
	string getDirectory(const string& envVar) const;
	string getFile(const string& env, const string& def = "") const;
};

extern const DteEnvVars DTE_ENV_VARS;


inline
int tupleCompare(const DteTupleAdt& adt, int numSortFields, int* sortFields,
                 const Tuple* t1, const Tuple* t2)
{
  for(int i = 0 ; i < numSortFields ; i++) {
    int k = sortFields[i];
    int r = adt.getAdt(k).compare(t1[k], t2[k]);
    if( r != 0 ) return r;
  }
  return 0;
}

#endif // _TYPES_H_
