// (C) 1999- by Kevin S. Beyer

#include "DTE/comm/TupleLoader.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteIntAdt.h"

int main()
{
  DteTupleAdt adt;
  adt.push_back(DteStringAdt(10));
  adt.push_back(DteIntAdt());
  TupleLoader tl(adt);
  void* tup[2];
  tup[0] = new char[11];
  memcpy(tup[0], "cuckoo", 7);
  tup[1] = new int(5);

  for(int i = 0 ; i < 100000 ; i++) {
    cerr << "xx " << i << ' ' << (void*)tl.insert(tup) << endl;
  }
  return 0;
}
