#ifndef _TUPLECOMPARE_H_
#define _TUPLECOMPARE_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/types/DteTupleAdt.h"

class TupleEqual
{
  DteTupleAdt tupAdt;
  int numCompare;

public:

  TupleEqual(const DteTupleAdt& tupAdt, int numCompare)
    : tupAdt(tupAdt), numCompare(numCompare) {}

  ~TupleEqual() {}

  bool operator()(const Tuple* x, const Tuple* y) const {
    for(int i = 0 ; i < numCompare ; i++) {
      if( tupAdt.getAdt(i).not_equal_to(x[i], y[i]) ) {
        return false;
      }
    }
    return true;
  }
};

class TupleLess
{
  DteTupleAdt tupAdt;
  vector<bool> ascDesc;

public:

  // ascDesc = true -> ascdending; false -> descending
  TupleLess(const DteTupleAdt& tupAdt, const vector<bool>& ascDesc)
    : tupAdt(tupAdt), ascDesc(ascDesc) {}

  TupleLess(const TupleLess& x)
    : tupAdt(x.tupAdt), ascDesc(x.ascDesc) {}

  ~TupleLess() {}

  bool operator()(const Tuple* x, const Tuple* y) const {
    int N = ascDesc.size();
    for(int i = 0 ; i < N ; i++) {
      int cmp = tupAdt.getAdt(i).compare(x[i], y[i]);
      if( cmp ) {
        if( cmp < 0 ) {
          return ascDesc[i];
        }
        return !ascDesc[i];
      }
    }
    return false;
  }
};

#endif // _TUPLECOMPARE_H_
