#ifndef _DTEINTADT_H_
#define _DTEINTADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"

//kb: make this a template & combine with Float8Adt,Bool; add typedefs?

class DteIntAdt
: public DteAdt
{
public:

  //typedef Refc<DteIntAdt> Ref;
  typedef int4 BasicType;
  typedef int4 ManagedType;

  const static int typeID = 1;
  const static char* const typeName = "int";

  DteIntAdt(bool nullable = false) : DteAdt(nullable, INT4_ALIGN) {}

  virtual ~DteIntAdt() {}

  virtual int getTypeID() const { return typeID; }

  virtual string getTypeName() const { return typeName; }

  virtual string getTypeSpec() const { return typeName; }

  virtual DteAdt* clone() const { return new DteIntAdt(nullable); }

  static int4& cast(void* x) { return *(int4*)x; }
  static const int4& cast(const void* x) { return *(const int4*)x; }

  static const Type* getTypePtr(int4& x) { return &x; }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    cast(to) = cast(x);
    return sizeof(int4);
  }

  virtual void deallocate(void* x) const { delete &cast(x); }

  static Type* allocateCopy(int4 x) {
    int4* y = new int4;
    *y = x;
    return y;
  }

  virtual Type* allocateCopy(const void* x) const {
    return allocateCopy(cast(x));
  }

  virtual bool canCompare() const { return true; }

  virtual int compareNonNull(const void* x, const void* y) const {
    if( cast(x) < cast(y) ) return -1;
    if( cast(x) > cast(y) ) return 1;
    return 0;
  }

  virtual bool canHash() const { return true; }

  virtual int hashNonNull(const void* x) const { return cast(x); }

  // integers don't need to swizzle
  virtual void swizzle(void* x, char* page) const {}
  virtual void unswizzle(void* x, char* page) const {}

  virtual void printNonNull(ostream& s, const void* x) const
    { s << cast(x); }

  virtual void toAscii(ostream& s, const void* x) const
    { s << cast(x); }

  virtual int fromAscii(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    s >> cast(to);
    return sizeof(int4);
  }

  virtual void toBinary(ostream& s, const void* x) const
    { s.write(x, sizeof(int4)); }

  virtual int fromBinary(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    s.read(to, sizeof(int4));
    return sizeof(int4);
  }

  virtual void toNet(ostream& s, const void* x) const { 
    int4 y = htonl(cast(x));
    s.write(&y, sizeof(int4));
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    if( size < (int)sizeof(int4) ) return -1;
    int4 x;
    s.read(&x, sizeof(int4));
    cast(to) = ntohl(x);
    return sizeof(int4);
  }

  virtual int getMaxSize() const           { return sizeof(int4); }
  virtual bool isVariableSize() const      { return false; }

private:

  DteIntAdt(const DteIntAdt& x); // : DteAdt(x) {}
  DteIntAdt& operator=(const DteIntAdt& x); // { DteAdt::operator=(x); return *this; }
};


#endif // _DTEINTADT_H_
