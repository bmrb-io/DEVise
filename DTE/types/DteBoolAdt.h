#ifndef _DTEBOOLADT_H_
#define _DTEBOOLADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"


class DteBoolAdt
: public DteAdt
{
public:

  //typedef Refc<DteBoolAdt> Ref;
  typedef char BasicType;
  typedef char ManagedType;

  const static int typeID = 10;
  const static char* const typeName = "bool";

  DteBoolAdt(bool nullable = false) : DteAdt(nullable, 1) {}

  virtual ~DteBoolAdt() {}

  int getTypeID() const { return typeID; }

  string getTypeName() const { return typeName; }

  string getTypeSpec() const { return typeName; }

  virtual DteAdt* clone() const { return new DteBoolAdt(nullable); }

  static char& cast(void* x) { return *(char*)x; }
  static const char& cast(const void* x) { return *(const char*)x; }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    if( size < (int)sizeof(char) ) return -1;
    cast(to) = cast(x);
    return sizeof(char);
  }

  virtual void deallocate(void* x) const { delete &cast(x); }

  static Type* allocateCopy(char x) {
    char* y = new char;
    *y = x;
    return y;
  }

  static Type* allocateCopy(bool x) {
    return allocateCopy(char(x));
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
    { s << (cast(x) ? "true" : "false"); }

  virtual void toAscii(ostream& s, const void* x) const
    { s << (int)cast(x); }

  virtual int fromAscii(istream& s, char* to, int size) const {
    if( size < (int)sizeof(char) ) return -1;
    int i;
    s >> i;
    cast(to) = (i != 0);
    return sizeof(char);
  }

  virtual void toBinary(ostream& s, const void* x) const
    { s.write(x, sizeof(char)); }

  virtual int fromBinary(istream& s, char* to, int size) const {
    if( size < (int)sizeof(char) ) return -1;
    s.read(to, sizeof(char));
    return sizeof(char);
  }

  virtual void toNet(ostream& s, const void* x) const {
    DteBoolAdt::toBinary(s, x);
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    return DteBoolAdt::fromBinary(s, to, size);
  }

  virtual int getMaxSize() const           { return sizeof(char); }
  virtual bool isVariableSize() const      { return false; }

private:

  DteBoolAdt(const DteBoolAdt& x);
  DteBoolAdt& operator=(const DteBoolAdt& x);
};

#endif // _DTEBOOLADT_H_
