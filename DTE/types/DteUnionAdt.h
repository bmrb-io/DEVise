#ifndef _DTEUNIONADT_H_
#define _DTEUNIONADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"


//kb: should I limit unions to 256 elements?
class DteUnionAdt
: public DteAdt
{
  DteAdtList types;
  vector<string> tagNames;

  int maxSize;

  bool allFixedSize;
  bool allCanCompare;
  bool allCanHash;

public:

  //kb: need BasicType for Union? typedef int4 BasicType;
  //kb: need MangagedType for Union?: typedef int4 ManagedType;

  const static int typeID = 5;
  const static char* const typeName = "union";

  //kb: should alignment be max alignment?
  //kb: not a problem with everything being variable length...

  // tags cannot contain spaces!
  DteUnionAdt(const DteAdtList& types, vector<string> tags,
              bool nullable = false)
    : DteAdt(nullable, INT4_ALIGN), types(types), tagNames(tags) {

      assert(types.size() == tags.size());
      assert(types.size() > 0);

      maxSize = 0;
      allFixedSize = true;
      allCanCompare = true;
      allCanHash = true;

      int n = types.size();
      for(int i = 0 ; i < n ; i++ ) {

	int size = types[i]->getMaxSize();
	if( size > maxSize ) maxSize = size;

        assert( ! types[i]->isNullable() ); // union can be null, but not elements
        if( types[i]->isVariableSize() ) {
          allFixedSize = false;
        }
        if( ! types[i]->canCompare() ) {
          allCanCompare = false;
        }
        if( ! types[i]->canHash() ) {
          allCanHash = false;
        }
      }

      maxSize = Align(maxSize + sizeof(int4), INT4_ALIGN);
    }

  virtual ~DteUnionAdt() {}

  int getTypeID() const { return typeID; }

  string getTypeName() const { return typeName; }

  string getTypeSpec() const {
    string s(typeName);
    s += "(";
    int n = types.size();
    if( n > 0 ) {
      s += tagNames[0];
      s += ' ';
      s += types[0]->getTypeSpec();
      for(int i = 1 ; i < n ; i++) {
        s += ", ";
        s += tagNames[i];
        s += ' ';
        s += types[i]->getTypeSpec();
      }
    }
    s += ")";
    return s;
  }

  virtual DteAdt* clone() const { 
    return new DteUnionAdt(types, tagNames, nullable);
  }

  static int4& tag(void* x) { return *(int4*)x; }
  static const int4& tag(const void* x) { return *(int4*)x; }

  const string& getTagName(int i) const {
    assert(i >= 0 && i < (int)tagNames.size());
    return tagNames[i];
  }

  int4 getTagID(const string& t) const {
    //cerr << "lookup up tag: '" << t << "'\n";
    int n = tagNames.size();
    for(int i = 0 ; i < n ; i++) {
      if( t == tagNames[i] ) return i;
    }
    return -1;
  }

  char* pointer(void* x, int tag) const
    { return types[tag]->align(((char*)x) + sizeof(int4)); }

  const char* pointer(const void* x, int tag) const
    { return types[tag]->align(((const char*)x) + sizeof(int4)); }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    char* startTo = to;
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    tag(to) = t;
    to = pointer(to, t);
    int used = to - startTo;
    int u = types[t]->copyNonNull(pointer(x,t), to, size - used);
    if( u < 0 ) return -1;
    return used + u;
  }

  // note: union compare uses order of type IDs
  virtual bool canCompare() const { return allCanCompare; }

  virtual int compareNonNull(const void* x, const void* y) const {
    int4 xt = tag(x);
    int4 yt = tag(x);
    assert( xt >= 0 && xt < (int4)types.size() );
    assert( yt >= 0 && yt < (int4)types.size() );

    if( xt == yt ) {
      return types[xt]->compare(pointer(x,xt), pointer(y,xt));
    }
    if( xt < yt ) {
      return -1;
    }
    // xt > yt
    return 1;
  }

  virtual bool canHash() const { return allCanHash; }

  virtual int hashNonNull(const void* x) const {
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    return types[t]->hash(pointer(x,t));
  }

  virtual void swizzle(void* x, char* page) const {
    // just pass it on
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    types[t]->swizzle(pointer(x,t), page);
  }

  virtual void unswizzle(void* x, char* page) const {
    // just pass it on
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    types[t]->unswizzle(pointer(x,t), page);
  }

  virtual void print(ostream& s, const void* x) const {
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    s << tagNames[t] << ' ';
    types[t]->print(s, pointer(x,t));
  }

  virtual void toAscii(ostream& s, const void* x) const {
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    s << tagNames[t] << ' ';
    types[t]->toAscii(s, pointer(x,t));
  }

  virtual int fromAscii(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    char* startTo = to;
    string tn;
    s >> tn;
    if( !s ) return -2;
    int4 t = getTagID(tn);
    assert( t >= 0 && t < (int4)types.size() );
    tag(to) = t;
    to = types[t]->align(to + sizeof(int4));
    int used = to - startTo;
    int u = types[t]->fromAscii(s, to, size - used);
    if( u < 0 ) return -1;
    return used + u;
  }

  virtual void toBinary(ostream& s, const void* x) const {
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    s.write(&t, sizeof(int4));
    types[t]->toBinary(s, pointer(x,t));
  }

  virtual int fromBinary(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    char* startTo = to;
    int4 t;
    s.read(&t, sizeof(int4));
    assert( t >= 0 && t < (int4)types.size() );
    tag(to) = t;
    to = types[t]->align(to + sizeof(int4));
    int used = to - startTo;
    int u = types[t]->fromBinary(s, to, size - used);
    if( u < 0 ) return -1;
    return used + u;
  }


  virtual void toNet(ostream& s, const void* x) const {
    int4 t = tag(x);
    assert( t >= 0 && t < (int4)types.size() );
    int4 t2 = htonl(t);
    s.write(&t2, sizeof(int4));
    types[t]->toNet(s, pointer(x,t));
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    char* startTo = to;
    int4 t;
    s.read(&t, sizeof(int4));
    t = ntohl(t);
    assert( t >= 0 && t < (int4)types.size() );
    tag(to) = t;
    to = types[t]->align(to + sizeof(int4));
    int used = to - startTo;
    int u = types[t]->fromNet(s, to, size - used);
    if( u < 0 ) return -1;
    return used + u;
  }

  //virtual int getAlignment() const    { return INT4_ALIGN; }

  virtual int getMaxSize() const { return maxSize; }

  virtual bool isVariableSize() const { return !allFixedSize; }

private:

  DteUnionAdt(const DteUnionAdt& x);
  DteUnionAdt& operator=(const DteUnionAdt& x);
};


#endif // _DTEUNIONADT_H_
