#ifndef _DTETUPLEADT_H_
#define _DTETUPLEADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"


class DteTupleAdt
: public DteAdt
{
  int numFields;

  DteAdtList types;

  int pointerSize;

  int maxSize;

  bool allNonNull;
  bool allFixedSize;
  bool allCanCompare;
  bool allCanHash;

  void precompute();

public:

  typedef Tuple* BasicType;
  //kb: need MangedType for Tuple?: typedef ?? ManagedType;

  const static int typeID = 4;
  const static char* const typeName = "record";

  DteTupleAdt();

  DteTupleAdt(const DteTupleAdt& x);

  DteTupleAdt(const DteAdtList& types, bool nullable = false);

  DteTupleAdt& operator=(const DteTupleAdt& x);

  void operator=(const DteAdtList& types);
    
  void push_front(const DteAdt& x);

  void push_back(const DteAdt& x);

  void append(const DteTupleAdt& x);

  virtual ~DteTupleAdt();

  virtual int getTypeID() const;

  virtual string getTypeName() const;

  virtual string getTypeSpec() const;

  virtual DteAdt* clone() const;

  static Type** pointers(Type* x) { return (Type**)x; }
  static const Tuple* pointers(const Type* x) { return (const Tuple*)x; }

  static Type** cast(Type* x) { return pointers(x); }
  static const Tuple* cast(const Type* x) { return pointers(x); }

  int getNumFields() const { return numFields; }

  const DteAdt& getAdt(int i) const {
    assert(i>=0 && i < numFields); 
    return *(types[i]);
  }

  const Type* getField(Type* x, int i) { return pointers(x)[i]; }

  virtual int copyNonNull(const Type* x, char* to, int size) const;

  const Tuple* copyNonNull(const Tuple* x, PageBuf& to) const {
    int used = DteTupleAdt::copyNonNull(x, to.data, DTE_PAGESIZE);
    assert(used >= 0 && "Tuple did not fit on a page!");
    return (Tuple*)(to.data);
  }

  virtual void deallocate(void* x) const;

  virtual Type* allocateCopy(const void* x) const;

  virtual bool canCompare() const;

  virtual int compareNonNull(const Type* x, const Type* y) const;

  virtual bool canHash() const;

  virtual int hashNonNull(const Type* x) const;

  virtual void swizzle(Type* x, char* page) const;

  virtual void unswizzle(Type* x, char* page) const;

  // prints the tuple without braces
  void print(ostream& s, const Tuple* tup) const;

  virtual void printNonNull(ostream& s, const Type* x) const;

  virtual void toAscii(ostream& s, const Type* x) const;

  virtual int fromAscii(istream& s, char* to, int size) const;

  const Tuple* fromAscii(istream& s, PageBuf& to) const {
    int used = DteTupleAdt::fromAscii(s, to.data, DTE_PAGESIZE);
    assert(used >= 0 && "Tuple did not fit on a page!");
    return (Tuple*)(to.data);
  }


  virtual void toBinary(ostream& s, const Type* x) const;

  virtual int fromBinary(istream& s, char* to, int size) const;

  const Tuple* fromBinary(istream& s, PageBuf& to) const {
    int used = DteTupleAdt::fromBinary(s, to.data, DTE_PAGESIZE);
    assert(used >= 0 && "Tuple did not fit on a page!");
    return (Tuple*)(to.data);
  }

  virtual void toNet(ostream& s, const Type* x) const;

  virtual int fromNet(istream& s, char* to, int size) const;

  const Tuple* fromNet(istream& s, PageBuf& to) const {
    int used = DteTupleAdt::fromNet(s, to.data, DTE_PAGESIZE);
    assert(used >= 0 && "Tuple did not fit on a page!");
    return (Tuple*)(to.data);
  }

  virtual int getMaxSize() const;

  virtual bool isVariableSize() const;
};


#endif // _DTETUPLEADT_H_
