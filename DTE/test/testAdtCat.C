// (C) 1999- by Kevin S. Beyer

#include "DTE/types/DteInt4Adt.h"
#include "DTE/types/DteFloat8Adt.h"
#include "DTE/types/DteVarCharAdt.h"
#include "DTE/types/DteTupleAdt.h"
#include "DTE/types/DteCatalogAdt.h"
#include "DTE/comm/DteAdtPage.h"


int main()
{
  DteAdtList adt;
  adt.push_back(new DteVarCharAdt(128));
  adt.push_back(new DteCatalogAdt());
  DteTupleAdt rec(adt);

  cerr << "typespec: " << rec.getTypeSpec() << endl;

  char readbuf[DTE_PAGESIZE];

  while(1) {
    int used = rec.fromAscii(cin, readbuf, sizeof(readbuf));
    if( !cin ) break;
    assert(used > 0);
    rec.toAscii(cout, readbuf);
    cout << '\n';
  }

  return 0;
}
