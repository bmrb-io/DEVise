// (C) 1999- by Kevin S. Beyer

#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"


int main()
{
  DteAdtList adt(5);
  adt[0] = DteInt4Adt();
  adt[1] = DteFloat8Adt();
  adt[2] = DteVarCharAdt(13);
  adt[3] = DteFloat8Adt();
  adt[4] = DteVarCharAdt(5);
  DteTupleAdt rec(adt);

  cout << "typespec: " << rec.getTypeSpec() << endl;

  char readbuf[DTE_PAGESIZE];
  DteAdtPage readPage(rec);

  char packbuf[DTE_PAGESIZE];
  DteAdtPage packPage(rec);
  packPage.initPage(packbuf);

  while(cin) {
    readPage.initPage(readbuf);
    bool fitOnPage = readPage.addFromAscii(cin);
    //int used = rec.fromAscii(cin, readbuf, DTE_PAGESIZE);

    if( !cin ) break;
    assert(fitOnPage);
    //assert(used > 0);

    const Tuple* tup = readPage.getTuple(0);
    fitOnPage = packPage.addTuple(tup);
    if( !fitOnPage ) {
      packPage.printPage(cout);
      packPage.initPage(packbuf);
      fitOnPage = packPage.addTuple(tup);
      assert(fitOnPage);
    }
  }
  packPage.printPage(cout);

  return 0;
}
