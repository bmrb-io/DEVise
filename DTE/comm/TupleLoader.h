#ifndef _TUPLELOADER_H_
#define _TUPLELOADER_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/comm/TupleStream.h"

class TupleLoader
{
  MemoryPageStream pageStream;
  OTupleStream outTS;
  ITupleStream inTS;
  bool writing;

public:

  TupleLoader(const DteTupleAdt& tupAdt)
    : pageStream(tupAdt), outTS(pageStream), inTS(pageStream), writing(true) {}

  ~TupleLoader() {}

  const Tuple* insert(const Tuple* t) {
    assert(writing);
    //cerr << "adding t " << (void*)t << endl;
    t = outTS.insert(t);
    //cerr << "got t " << (void*)t << endl;
    assert(t);
    return t;
  }

  void endWrite() {
    writing = false;
    pageStream.endWrite();
  }

  const Tuple* getNext() {
    if( writing ) endWrite();
    return inTS.getNext();
  }

  void rewrite() {
    writing = true;
    pageStream.rewrite();
    outTS.rewind();
  }

  void reread() {
    if(writing) endWrite();
    inTS.rewind();
  }

private:

  TupleLoader(const TupleLoader& x);
  TupleLoader& operator=(const TupleLoader& x);
};

#endif // _TUPLELOADER_H_
