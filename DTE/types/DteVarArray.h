#ifndef _DTEVARARRAY_H_
#define _DTEVARARRAY_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"


class DteVarArrayAdt
: public DteAdt
{
  DteAdt* type;
  int maxLength;

public:

  //kb: need BasicType for VarArray? typedef int4 BasicType;
  //kb: need MangagedType for VarArray?: typedef int4 ManagedType;

  const static int typeID = 6;
  const static char* const typeName = "vararray";

  //kb: should alignment be max alignment?
  //kb: not a problem with everything being variable length...
  DteVarArrayAdt(const DteAdt& type, int maxLen, bool nullable = false)
    : DteAdt(nullable, INT4_ALIGN), type(type.clone()), maxLength(maxLen) {}

  virtual ~DteVarArrayAdt() { delete type; }

  int getTypeID() const { return typeID; }

  string getTypeName() const { return typeName; }

  string getTypeSpec() const {
    ostringstream s;
    s << typeName << '[' << maxLength << "] " << type->getTypeSpec();
    return s.str();
  }

  virtual DteAdt* clone() const {
    return new DteVarArrayAdt(*type, maxLength, nullable);
  }

  static int4& length(void* x) { return *(int4*)x; }
  static const int4& length(const void* x) { return *(int4*)x; }

  static void** pointers(void* x) { return &((void**)x)[1]; }
  static const void** pointers(const void* x) { return &((const void**)x)[1]; }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    int n = length(x);
    const void** tup = pointers(x);

    char* endTo = to + size;

    int4* VarArrayLen = (int4*)to;
    *VarArrayLen = n;
    to += sizeof(int4);
    void** ptrs = (void**)to;
    to += n * sizeof(void*);

    if( to >= endTo ) {
      return -1;
    }
    
    for(int i = 0 ; i < n ; i++ ) {
      assert(to <= endTo);
      if( tup[i] == NULL ) {
        ptrs[i] = NULL;
      } else {
        to = type->align(to);
        ptrs[i] = to;
        int u = type->copyNonNull(tup[i], to, endTo - to);
        if( u == -1 ) return -1;
        to += u;
      }
    }
    return to - (char*)VarArrayLen;
  }

  virtual bool canCompare() const { return type->canCompare(); }

  virtual int compareNonNull(const void* x, const void* y) const {
    int xn = length(x);
    int yn = length(x);
    int n = (xn < yn) ? xn : yn;
    const void** xtup = pointers(x);
    const void** ytup = pointers(y);
    for(int i = 0 ; i < n ; i++) {
      int r = type->compare(xtup[i], ytup[i]);
      if( r != 0 ) return r;
    }
    if( xn == yn ) return 0;
    if( xn < yn ) return -1;
    return 1;
  }

  virtual bool canHash() const { return type->canHash(); }

  virtual int hashNonNull(const void* x) const {
    int n = length(x);
    int h = 0;
    const void** xtup = pointers(x);
    for(int i = 0 ; i < n ; i++) {
      h = 5 * h + type->hash(xtup[i]);
    }
    return h;
  }

  virtual void swizzle(void* x, char* page) const {
    int n = length(x);
    void** xtup = pointers(x);
    for(int i = 0 ; i < n ; i++) {
      if( xtup[i] ) {
	type->swizzle(xtup[i], page);
	xtup[i] = (void*)((char*)(xtup[i]) - page);
      }
    }
  }

  virtual void unswizzle(void* x, char* page) const {
    int n = length(x);
    void** xtup = pointers(x);
    for(int i = 0 ; i < n ; i++) {
      if( xtup[i] ) {
	xtup[i] = page + (int)(xtup[i]);
	type->unswizzle(xtup[i], page);
      }
    }
  }

  virtual void print(ostream& s, const void* x) const {
    int n = length(x);
    const void** xtup = pointers(x);
    s << "[";
    if( n > 0 ) {
      if( xtup[0] == NULL ) {
	s << "null";
      } else {
	type->print(xtup[0], s);
      }
      for(int i = 1 ; i < n ; i++) {
	s << ", ";
	if( xtup[i] == NULL ) {
	  s << "null";
	} else {
	  type->print(xtup[i], s);
	}
      }
    }
    s << "]";
  }

  virtual void toAscii(ostream& s, const void* x) const {
    int4 n = length(x);
    const void** xtup = pointers(x);
    s << n;
    for(int i = 0 ; i < n ; i++) {
      if( type->isNullable() ) { // 0 = non-null, 1 = null
        s << ' ' << (xtup[i] ? '0' : '1');
      }
      if( xtup[i] ) {
        s << ' ';
        type->toAscii(xtup[i], s);
      }
    }
  }

  virtual int fromAscii(istream& s, char* to, int size) const {
    int4 n;
    s >> n;
    assert(n >= 0 && n <= maxLength);
    char* endTo = to + size;
    int4* VarArrayLen = (int4*)to;
    to += sizeof(int4);
    void** ptrs = (void**)to;
    to += n * sizeof(void*);

    if( to >= endTo ) {
      return -1;
    }
    *VarArrayLen = n;
    
    for(int i = 0 ; i < n ; i++ ) {
      if( type->isNullable() ) {
        char ni;
        s >> ni;
        if( !s ) return -1;
        if( ni == '1' ) {
          ptrs[i] = NULL;
          continue;
        }
        assert( ni == '0' );
      }
      // non-null
      assert(to <= endTo);
      to = type->align(to);
      ptrs[i] = to;
      int u = type->fromAscii(s, to, endTo - to);
      if( u == -1 ) return -1;
      to += u;
    }
    return to - (char*)VarArrayLen;
  }


  virtual void toBinary(ostream& s, const void* x) const {
    int4 n = length(x);
    const void** xtup = pointers(x);
    s.write(&n, sizeof(int4));
    for(int i = 0 ; i < n ; i++) {
      if( type->isNullable() ) { // 0 = non-null, 1 = null
	s.put(xtup[i] ? '\x00' : '\x01');
      }
      if( xtup[i] ) {
	type->toBinary(xtup[i], s);
      }
    }
  }

  virtual int fromBinary(istream& s, char* to, int size) const {
    int4 n;
    s.read(&n, sizeof(int4));
    assert(n >= 0 && n <= maxLength);
    char* endTo = to + size;
    int4* VarArrayLen = (int4*)to;
    to += sizeof(int4);
    void** ptrs = (void**)to;
    to += n * sizeof(void*);

    if( to >= endTo ) {
      return -1;
    }
    *VarArrayLen = n;
    
    for(int i = 0 ; i < n ; i++ ) {
      if( type->isNullable() ) { // 0 = non-null, 1 = null
	char ni;
	s.get(ni);
        if( !s ) return -1;
	if( ni == '\x01' ) {
	  ptrs[i] = NULL;
	  continue;
	}
	assert( ni == '\x00' );
      }
      assert(to <= endTo);
      to = type->align(to);
      ptrs[i] = to;
      int u = type->fromBinary(s, to, endTo - to);
      if( u == -1 ) return -1;
      to += u;
    }
    return to - (char*)VarArrayLen;
  }


  virtual void toNet(ostream& s, const void* x) const {
    int4 n = length(x);
    const void** xtup = pointers(x);
    int4 n2 = htonl(n);
    s.write(&n2, sizeof(int4));
    for(int i = 0 ; i < n ; i++) {
      if( type->isNullable() ) { // 0 = non-null, 1 = null
	s.put(xtup[i] ? '\x00' : '\x01');
      }
      if( xtup[i] ) {
	type->toNet(xtup[i], s);
      }
    }
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    int4 n;
    s.read(&n, sizeof(int4));
    n = ntohl(n);
    assert(n >= 0 && n <= maxLength);

    char* endTo = to + size;
    int4* VarArrayLen = (int4*)to;
    to += sizeof(int4);
    void** ptrs = (void**)to;
    to += n * sizeof(void*);

    if( to >= endTo ) {
      return -1;
    }
    *VarArrayLen = n;
    
    for(int i = 0 ; i < n ; i++ ) {
      if( type->isNullable() ) { // 0 = non-null, 1 = null
	char ni;
	s.get(ni);
        if( !s ) return -1;
	if( ni == '\x01' ) {
	  ptrs[i] = NULL;
	  continue;
	}
	assert( ni == '\x00' );
      }
      assert(to <= endTo);
      to = type->align(to);
      ptrs[i] = to;
      int u = type->fromNet(s, to, endTo - to);
      if( u == -1 ) return -1;
      to += u;
    }
    return to - (char*)VarArrayLen;
  }

  //virtual int getAlignment() const    { return INT4_ALIGN; }

  //kb: not working yet
  virtual int getMaxSize() const { return 0; }

  virtual bool isVariableSize() const { return type->isVariableSize(); }

private:

  DteVarArrayAdt(const DteVarArrayAdt& x);
  DteVarArrayAdt& operator=(const DteVarArrayAdt& x);
};



#endif // _DTEVARARRAY_H_
