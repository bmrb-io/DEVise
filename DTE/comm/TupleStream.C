// (C) 1999- by Kevin S. Beyer

#include "TupleStream.h"


void OTupleStream::getNextPage()
{
  PageBuf* pb = pageStream.getNext();
  //cerr << "got page " << (void*)pb << endl;
  if( pb ) {                  // got a page
    page.initPage(pb);
  } else {               // no room in pageStream
    eof = true;
  }
}


OTupleStream::OTupleStream(PageStream& pageStream)
  : pageStream(pageStream), page(pageStream.getAdt()), eof(false)
{
  getNextPage();
}


OTupleStream::~OTupleStream()
{
}


void OTupleStream::rewind()
{
  pageStream.rewind();
  getNextPage();
}


const Tuple* OTupleStream::insert(const Tuple* t)
{
  if( eof ) {
    return NULL;
  }
  const Tuple* ret = page.addTuple(t);
  //cerr << "added tuple " << (void*)ret << endl;
  if( !ret ) {                // t did not fit on the page
    //cerr << "getting page\n";
    getNextPage();
    if( eof ) {
      return NULL;
    }
    ret = page.addTuple(t);
    //cerr << "readded tuple " << (void*)ret << endl;
    assert(ret && "tuple did not fit on a page!");
  }
  //cerr << "returning " << (void*)ret << endl;
  return ret;
}

//---------------------------------------------------------------------------


ITupleStream::ITupleStream(PageStream& pageStream)
  : pageStream(pageStream), page(pageStream.getAdt()), slot(0)
{
}


ITupleStream::~ITupleStream()
{
}


void ITupleStream::rewind()
{
  pageStream.rewind();
  slot = 0;
}


const Tuple* ITupleStream::getNext()
{
  if( slot < page.getNumSlots() ) {
    int i = slot++;
    return page.getTuple(i);
  }
  do {
    PageBuf* pb = pageStream.getNext();
    if( !pb ) {               // eof
      return NULL;
    }
    page.setPage(pb);
  } while( page.getNumSlots() == 0 );   // empty page
  slot = 1;
  return page.getTuple(0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
