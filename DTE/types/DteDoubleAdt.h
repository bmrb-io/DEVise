#ifndef _DTEDOUBLEADT_H_
#define _DTEDOUBLEADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"


class DteDoubleAdt
: public DteAdt
{
public:

  typedef float8 BasicType;
  typedef float8 ManagedType;

  const static int typeID = 2;
  const static char* const typeName = "double";

  DteDoubleAdt(bool nullable = false) : DteAdt(nullable, FLOAT8_ALIGN) {}

  virtual ~DteDoubleAdt() {}

  virtual int getTypeID() const { return typeID; }

  virtual string getTypeName() const { return typeName; }

  virtual string getTypeSpec() const { return typeName; }

  virtual DteAdt* clone() const { return new DteDoubleAdt(nullable); }

  static float8& cast(void* x) { return *(float8*)x; }
  static const float8& cast(const void* x) { return *(const float8*)x; }

  static Type* getTypePtr(float8& x) { return &x; }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    if( size < (int)sizeof(float8) ) return -1;
    cast(to) = cast(x);
    return sizeof(float8);
  }

  virtual void deallocate(void* x) const { delete &cast(x); }

  static Type* allocateCopy(float8 x) {
    float8* y = new float8;
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

  virtual int hashNonNull(const void* x) const {
    int4 h;
    char* p = (char*)&h;
    const char* q = (const char*)x;
    p[0] = q[0];
    p[1] = q[1];
    p[2] = q[6];
    p[3] = q[7];
    return h;
  }

  // float8s don't need to swizzle
  virtual void swizzle(void* x, char* page) const {}
  virtual void unswizzle(void* x, char* page) const {}

  virtual void printNonNull(ostream& s, const void* x) const
    { s << cast(x); }

  virtual void toAscii(ostream& s, const void* x) const
    { s << cast(x); }

  virtual int fromAscii(istream& s, char* to, int size) const {
    if( size < (int)sizeof(float8) ) return -1;
    s >> cast(to);
    return sizeof(float8);
  }

  virtual void toBinary(ostream& s, const void* x) const
    { s.write(x, sizeof(float8)); }

  virtual int fromBinary(istream& s, char* to, int size) const {
    if( size < (int)sizeof(float8) ) return -1;
    s.read(to, sizeof(float8));
    return sizeof(float8);
  }

  virtual void toNet(ostream& s, const void* x) const { 
    float8 y = htond(cast(x));
    s.write(&y, sizeof(float8));
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    if( size < (int)sizeof(float8) ) return -1;
    float8 x;
    s.read(&x, sizeof(float8));
    cast(to) = ntohd(x);
    return sizeof(float8);
  }

  //virtual int getAlignment() const         { return INT4_ALIGN; }
  virtual int getMaxSize() const           { return sizeof(float8); }
  virtual bool isVariableSize() const      { return false; }

private:

  DteDoubleAdt(const DteDoubleAdt& x);
  DteDoubleAdt& operator=(const DteDoubleAdt& x);
};


#endif // _DTEDOUBLEADT_H_
