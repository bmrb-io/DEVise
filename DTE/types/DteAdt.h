#ifndef _DTEADT_H_
#define _DTEADT_H_

// (C) 1999- by Kevin S. Beyer

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>          // for htonl
#include <netinet/in.h>         // for htonl

#include "DTE/misc/DteBasicTypes.h"
#include "DTE/util/CloneObject.h"

/*
Most ADTs define additional types and cast functions for use by templates:
  BasicType: the type of object that is stored on the page (e.g., a char*).
  ManagedType: a type of object that can take a BasicType and manage the space
               needed for it, as well as define relevant comparison, assigment,
               and mathematical operators (e.g., an stl 'string' for String).
               ManagedType m; m = ADT::cast(x); // this should work
               //kb: OOPS! need m.basicType() or void* ADT::toType(ManagedType&)
               //kb: should I have ADT::toType(BasicType&) too?

  static BasicType& cast(void* x): cast the pointer to its basic type
  static const BasicType& cast(const void* x): ditto

  Nulls (x values) are not allowed in the above functions.
  //kb: how to handle nulls (e.g. in generic hash aggregate code)
  //kb: I could add a Nullable<ADT> wrapper around ADT::ManagedType.
*/


class DteAdt
//: public RefcObject
{
protected:

  bool nullable;
  int alignment;
  
public:

  //typedef RefcObject super;      // kb: add similar typedef to children.
  //typedef Refc<DteAdt> Ref;

  DteAdt(const DteAdt& x)
    : nullable(x.nullable), alignment(x.alignment) {}

  DteAdt(bool nullable, int alignment)
    : nullable(nullable), alignment(alignment) {}

  virtual ~DteAdt() {}

  DteAdt& operator=(const DteAdt& x) {
    nullable = x.nullable;
    alignment = x.alignment;
    return *this;
  }

  bool operator==(const DteAdt& x) const {
    return getTypeID() == x.getTypeID();
  }

  // returns a unique id for this type (w/o parameters)
  //kb: I should make a way of organizing the numbers used here...
  virtual int getTypeID() const = 0;

  // returns a unique string for this type (w/o parameters)
  virtual string getTypeName() const = 0;

  // returns a string for this complete type, including parameters
  virtual string getTypeSpec() const = 0;

  // returns a copy of the Adt that must be deleted by caller
  virtual DteAdt* clone() const = 0;

  static bool isNull(const void* x) { return x == NULL; }

  // pack 'x' into the bytes at 'to'.
  // 'size' is the number of bytes available at 'to'.
  // returns the number of bytes used, or -1 if this did not fit within 'size'.
  // Objects must be properly aligned, and pointers can be used within 'to'.
  virtual int copyNonNull(const void* x, char* to, int size) const = 0;

  virtual void deallocate(void* x) const = 0;

  virtual Type* allocateCopy(const void* x) const = 0;

  //kb: keep this?
  // returns true if this ADT and adt have the same type-spec
  bool DteAdt::isExactSameType(const DteAdt& adt) const {
    //kb: should the derived classes perform this test (and not use strings)?
    return getTypeSpec() == adt.getTypeSpec();
  }

  bool DteAdt::isExactSameType(const DteAdt* adt) const {
    if( adt == NULL ) return false;
    return isExactSameType(*adt);
  }

  // if this returns true, compare works fully.
  virtual bool canCompare() const = 0;

  //kb: exactly? or same class?
  // comparison test.  x must be exactly the same type as 'this'.
  // returns <0 if this < x, 0 if this == x, >0 if this > x
  // Nulls are considered equal by this test, and nulls are less than
  // all other values.
  // If canCompare() is false, but canHash() is true, then compare()
  // returns zero for equality and non-zero for inequality.
  virtual int compare(const void* x, const void* y) const {
    if( x == NULL ) {
      if( y == NULL ) return 0;
      return -1;
    }
    if( y == NULL ) return 1;
    return compareNonNull(x, y);
  }

  virtual int compareNonNull(const void* x, const void* y) const = 0;

  //kb: keep these?
  bool equal_to(const void* x, const void* y) const
    { return compare(x,y) == 0; }

  bool less(const void* x, const void* y) const
    { return compare(x,y) < 0; }

  bool less_equal(const void* x, const void* y) const
    { return compare(x,y) <= 0; }

  bool greater_equal(const void* x, const void* y) const
    { return compare(x,y) >= 0; }

  bool greater(const void* x, const void* y) const
    { return compare(x,y) >  0; }

  bool not_equal_to(const void* x, const void* y) const
    { return compare(x,y) != 0; }

  // if this returns true, hash() works, can compare() at least returns
  // zero for equality and non-zero for inequality.
  virtual bool canHash() const = 0;

  // returns a hash value
  int hash(const void* x) const {
    if( x == NULL ) {
      return 0;
    } else {
      return hashNonNull(x);
    }
  }

  virtual int hashNonNull(const void* x) const = 0;

  // swizzle changes pointers to integer offsets.
  // unswizzle changes offsets to pointers.
  // 'x' is a pointer to an object that is placed on 'page'.
  virtual void swizzle(void* x, char* page) const = 0;
  virtual void unswizzle(void* x, char* page) const = 0;

  // print writes this object to the stream in a pretty way.
  // print differs from toAscii in that it is non-reversible.
  // print can be called with null values.
  virtual void print(ostream& s, const void* x) const {
    if( x == NULL ) {
      s << "null";
    } else {
      printNonNull(s, x);
    }
  }

  virtual void printNonNull(ostream& s, const void* x) const = 0;

  // toAscii, fromAscii, toBinary, fromBinary, toNet, and fromNet
  // will only be called for non-null values; nulls are handled
  // by the tuple class.
  // fromAscii, fromBinary, and fromNet all behave like copyNonNull:
  //   they take a pointer to an aligned char buffer to store their result.
  //   They return the number of bytes used, or -1 if it couldn't fit.
  //   NOTE: If -1 is returned, the position of the input stream is
  //         not defined.  So, the caller should either make sure that
  //         a -1 will never be returned (i.e., have enough space), or
  //         use a stream that can be rewound to the proper position.
  //         The problem is some objects will not know if they fit or
  //         not until it reads some of the data from the stream.

  virtual void toAscii(ostream& s, const void* x) const = 0;
  virtual int fromAscii(istream& s, char* to, int size) const = 0;

  virtual void toBinary(ostream& s, const void* x) const = 0;
  virtual int fromBinary(istream& s, char* to, int size) const = 0;

  // write/read x in the common network representation (network byte order)
  virtual void toNet(ostream& s, const void* x) const = 0;
  virtual int fromNet(istream& s, char* to, int size) const = 0;

  void setNullable(bool nullable = true) { this->nullable = nullable; }
  bool isNullable() const { return nullable; }

  int getAlignment() const { return alignment; }
  virtual bool isVariableSize() const = 0;

  // returns the max. number of bytes needed for packing (unaligned)
  //kb: what about internal alignment?
  //kb: keep this?
  virtual int  getMaxSize() const = 0;

  char* align(const void* x) const { return ::Align((char*)x, alignment); }

  //kb: add this function
  static DteAdt* createAdt(const string& typespec);

};


typedef vector< CloneObject<DteAdt> > DteAdtList;


#endif // _DTEADT_H_
