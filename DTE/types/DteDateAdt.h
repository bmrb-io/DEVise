#ifndef _DTEDATEADT_H_
#define _DTEDATEADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"
#include "../../DataReader/DateTime.h"

class DteDateAdt
: public DteAdt
{
public:

  typedef EncodedDTF BasicType;
  typedef EncodedDTF ManagedType;

  const static int typeID = 9;
  const static char* const typeName = "date";

  DteDateAdt(bool nullable = false) : DteAdt(nullable, INT4_ALIGN) {}

  virtual ~DteDateAdt() {}

  int getTypeID() const { return typeID; }

  string getTypeName() const { return typeName; }

  string getTypeSpec() const { return typeName; }

  virtual DteAdt* clone() const { return new DteDateAdt(nullable); }

  static EncodedDTF& cast(void* x) { return *(EncodedDTF*)x; }
  static const EncodedDTF& cast(const void* x) { return *(const EncodedDTF*)x; }

  static const Type* getTypePtr(EncodedDTF& x) { return &x; }

  static time_t toUnixTime(const EncodedDTF& x) {
    struct tm stm;
    x.maketm(stm);
    return mktime(&stm);
  }

  static time_t toUnixTime(const void* x) {
    return toUnixTime(cast(x));
  }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    if( size < (int)sizeof(EncodedDTF) ) return -1;
    cast(to) = cast(x);
    return sizeof(EncodedDTF);
  }

  virtual void deallocate(void* x) const { delete &cast(x); }

  static Type* allocateCopy(const EncodedDTF& x) {
    EncodedDTF* y = new EncodedDTF(x);
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
    const EncodedDTF& d = cast(x);
    return d.date * 11 + d.time * 5 + d.nanosec;
  }

  // don't need to swizzle
  virtual void swizzle(void* x, char* page) const {}
  virtual void unswizzle(void* x, char* page) const {}

  virtual void printNonNull(ostream& s, const void* x) const
    { s << cast(x); }

  virtual void toAscii(ostream& s, const void* x) const
    { s << cast(x); }

  virtual int fromAscii(istream& s, char* to, int size) const {
    if( size < (int)sizeof(EncodedDTF) ) return -1;
    s >> cast(to);
    return sizeof(EncodedDTF);
  }

  virtual void toBinary(ostream& s, const void* x) const
    { s.write(x, sizeof(EncodedDTF)); }

  virtual int fromBinary(istream& s, char* to, int size) const {
    if( size < (int)sizeof(EncodedDTF) ) return -1;
    s.read(to, sizeof(EncodedDTF));
    return sizeof(EncodedDTF);
  }

  virtual void toNet(ostream& s, const void* x) const { 
    const EncodedDTF& d = cast(x);
    int4 y;
    y = htonl(d.date);
    s.write(&y, sizeof(int4));
    y = htonl(d.time);
    s.write(&y, sizeof(int4));
    y = htonl(d.nanosec);
    s.write(&y, sizeof(int4));
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    if( size < (int)sizeof(EncodedDTF) ) return -1;
    EncodedDTF& d = cast(to);
    int4 x;
    s.read(&x, sizeof(int4));
    d.date = ntohl(x);
    s.read(&x, sizeof(int4));
    d.time = ntohl(x);
    s.read(&x, sizeof(int4));
    d.nanosec = ntohl(x);
    return sizeof(EncodedDTF);
  }

  virtual int getMaxSize() const           { return sizeof(EncodedDTF); }
  virtual bool isVariableSize() const      { return false; }

private:

  DteDateAdt(const DteDateAdt& x);
  DteDateAdt& operator=(const DteDateAdt& x);
};


#endif // _DTEDATEADT_H_
