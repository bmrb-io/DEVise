#ifndef _DTEADTPAGE_H_
#define _DTEADTPAGE_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/comm/DtePage.h"
#include "DTE/types/DteTupleAdt.h"

// Note that this a handle to a PageBuf; it is not valid until
// initPage() or setPage() is called.

class DteAdtPage
: public DtePage
{
public:

  DteAdtPage(const DteTupleAdt& typeDesc) : DtePage(), typeDesc(typeDesc) {}

  ~DteAdtPage() {}

  const DteTupleAdt& getTypes() const { return typeDesc; }

  const Tuple* getTuple(int slot) const { return (Tuple*)getSlot(slot); }

  Tuple* getTuple(int slot) { return (Tuple*)getSlot(slot); }

protected:

  DteTupleAdt typeDesc;

  const Tuple* finishAdd(char* tup, int used) {
    if( used < 0 ) {            // didn't fit!
      return NULL;
    }
    used = Align(used, POINTER_ALIGN);
    addSlot(used);
    return (Tuple*)tup;
  }

  //kb: should these be public?
  void swizzlePage() {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      typeDesc.swizzle( getTuple(i), page );
    }
  }

  void unswizzlePage() {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      typeDesc.unswizzle( getTuple(i), page );
    }
  }

public:

  // returns a pointer to the copy of t on the page
  // returns NULL if t did NOT fit on the page
  const Tuple* addTuple(const Tuple* t) {
    char* tup = getFreeStart();
    return finishAdd(tup, typeDesc.copyNonNull(t, tup, getBytesFree()));
  }

  void print(ostream& s, int slot) const {
    typeDesc.print(s, getTuple(slot));
  }

  void toAscii(ostream& s, int slot) const {
    typeDesc.toAscii(s, getTuple(slot));
  }

  void toBinary(ostream& s, int slot) const {
    typeDesc.toBinary(s, getTuple(slot));
  }

  void toNet(ostream& s, int slot) const {
    typeDesc.toNet(s, getTuple(slot));
  }

  // convenience functions to process an entire page

  void printPage(ostream& s) const {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      print(s, i);
      s << '\n';
    }
  }

  void toAsciiPage(ostream& s) const {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      toAscii(s, i);
      s << '\n';
    }
  }

  void toBinaryPage(ostream& s) const {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      toBinary(s, i);
    }
  }

  void toNetPage(ostream& s) const {
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      toNet(s, i);
    }
  }

  // These routines read one record onto the page.
  // Returns false on error (failed to fit on page, eof, or error reading)
  // I recommend calling InitPage() before calling these so the object
  // will always fit (otherwise it is an unrecoverable error).  If InitPage
  // is not used before reading, then the caller must seek in the stream
  // to the proper locatation.
  bool addFromAscii(istream& s) {
    char* tup = getFreeStart();
    return finishAdd(tup, typeDesc.fromAscii(s, tup, getBytesFree()));
  }

  bool addFromBinary(istream& s) {
    char* tup = getFreeStart();
    return finishAdd(tup, typeDesc.fromBinary(s, tup, getBytesFree()));
  }

  bool addFromNet(istream& s) {
    char* tup = getFreeStart();
    return finishAdd(tup, typeDesc.fromNet(s, tup, getBytesFree()));
  }


  //kb: should these assume caller will swizzle/unswizzle???
  //kb: should these be on base class? removed all together?
  // returns false on eof, true otherwise
  bool read(int fd) {
    int bytes = DTE_PAGESIZE;
    char* p = page;
    while( bytes > 0 ) {
      int b = ::read(fd, p, bytes);
      if( b == 0 ) {            // eof
        return false;
      }
      assert(b > 0);
      p += b;
      bytes -= b;
    }
    resetPage();
    unswizzlePage();
    return true;
  }

  void write(int fd) {
    swizzlePage();
    int rc = ::write(fd, page, DTE_PAGESIZE);
    unswizzlePage();
    assert(rc == DTE_PAGESIZE);
  }

  // p must have same schema as this page!
  void copy(const void* p) {
    memcpy(page, p, DTE_PAGESIZE);
    resetPage();
    int n = getNumSlots();
    for(int i = 0 ; i < n ; i++) {
      void* p = getTuple(i);
      typeDesc.swizzle(p, (char*)p);
      typeDesc.unswizzle(p, page);
    }
  }

  // x must have the same schema as this page!
  void copy(const DteAdtPage& x) { copy(x.page); }

private:

  DteAdtPage(const DteAdtPage& x);
  DteAdtPage& operator=(const DteAdtPage& x);
};


#endif // _DTEADTPAGE_H_
