// (C) 1999- by Kevin S. Beyer

#include "dte/types/DteAllAdts.h"
#include "dte/comm/DteAdtPage.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  DteFloat8Adt x1;
  DteVarCharAdt x2(13);

  DteAdtList adt(2);
  adt[0] = x1;
  adt[1] = x2;

  vector<string> tag(2);
  tag[0] = "myint";
  tag[1] = "mystring";

  DteUnionAdt union1(adt, tag, true);
  DteTupleAdt rec2(adt, true);
  DteVarArrayAdt array1(rec2, 3, true);
  DteAdtList adt2(2);
  adt2[0] = union1;
  adt2[1] = array1;

  DteTupleAdt rec(adt2);

  cout << "typespec: " << rec.getTypeSpec() << endl;

  char readbuf[DTE_PAGESIZE];
  DteAdtPage readPage(rec);

  char packbuf[DTE_PAGESIZE];
  DteAdtPage packPage(rec);
  packPage.initPage(packbuf);

  int fd = open("/local.loon.1/testfile", O_RDWR|O_CREAT, 0666);
  while(cin) {
    readPage.initPage(readbuf);
    bool fitOnPage = readPage.addFromAscii(cin);
    if( !cin ) break;
    assert(fitOnPage);

    fitOnPage = packPage.addTuple(readPage.getTuple(0));
    if( !fitOnPage ) {
      //packPage.printPage(cout);
      packPage.write(fd);

      packPage.initPage(packbuf);
      fitOnPage = packPage.addTuple(readPage.getTuple(0));
      assert(fitOnPage);
    }
  }

  //packPage.printPage(cout);
  packPage.write(fd);

  lseek(fd, 0, SEEK_SET);
  while( readPage.read(fd) ) {
    readPage.printPage(cout);
  }

  return 0;
}
