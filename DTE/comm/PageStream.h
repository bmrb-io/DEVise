#ifndef _PAGESTREAM_H_
#define _PAGESTREAM_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/misc/DteBasicTypes.h"
#include "DTE/util/DteAlgo.h"


class PageStream
{
protected:

  DteTupleAdt tupleAdt;

public:

  PageStream(const DteTupleAdt& tupAdt) : tupleAdt(tupAdt) {}

  virtual ~PageStream() {}

  const DteTupleAdt& getAdt() { return tupleAdt; }

  // A stream does not have to support rewind().
  // The caller is required to know what is supported and use the
  // stream properly.

  // Start at the first page.
  virtual void rewind() = 0;

  // Get the getNext page, and release the previous page.
  // The page is DTE_PAGESIZE bytes.
  //kb: should this return a const DteAdtPage& instead?
  virtual PageBuf* getNext() = 0;

private:

  PageStream(const PageStream& x);
  PageStream& operator=(const PageStream& x);
};


//---------------------------------------------------------------------------
//kb: move these some where else

// This class will continue to allocate pages in memory as needed
// until the maxPages are allocated, or endWrite() is called. When
// endWrite() is called, the stream is ready for reading once
// rewind() is called.  getNext() will then return all of the pages
// upto the point where endWrite() was called.  Calling rewrite()
// will erase the stream and switch it to write mode.

class MemoryPageStream
: public PageStream
{
public:

  static const int MAX_PAGES = INT_MAX;

  MemoryPageStream(const DteTupleAdt& tupAdt, int maxPages = MAX_PAGES)
    : PageStream(tupAdt), maxPages(maxPages) {
    rewrite();
  }

  virtual ~MemoryPageStream();

  virtual void rewind();

  virtual PageBuf* getNext();

  void endWrite() { writing = false; endWritePage = curPage; endWritePage++; }

  void rewrite() { writing = true; curPage = pages.begin(); }

protected:

  vector<PageBuf*> pages;
  vector<PageBuf*>::iterator curPage;
  vector<PageBuf*>::iterator endWritePage;
  int maxPages;
  bool writing;

  MemoryPageStream(const MemoryPageStream& x);
  MemoryPageStream& operator=(const MemoryPageStream& x);
};



inline
MemoryPageStream::~MemoryPageStream()
{
  delete_all(pages);
}


inline
void MemoryPageStream::rewind()
{
  curPage = pages.begin();
}

inline
PageBuf* MemoryPageStream::getNext()
{
  //cerr << "writing? " << writing << endl;
  if( writing ) {
    if( curPage == pages.end() ) {
      if( (int)pages.size() == maxPages ) {
        //cerr << "hit max\n";
        return NULL;
      }
      //cerr << "adding new page\n";
      curPage = pages.insert(pages.end(), new PageBuf);
    }
    PageBuf* pb = *curPage;
    curPage++;
    return pb;
  }

  // reading
  if( curPage == endWritePage ) {
    return NULL;
  }
  curPage++;
  return *curPage;
}


//---------------------------------------------------------------------------

//kb: Fd Reader/Writer need to swizzle on i/o

class FdReaderPageStream
: public PageStream
{
public:

  FdReaderPageStream(const DteTupleAdt& tupAdt, int fd);

  virtual ~FdReaderPageStream();

  virtual void rewind();

  virtual PageBuf* getNext();

protected:

  int fd;
  PageBuf pageBuf;
  DteAdtPage page;

  FdReaderPageStream(const FdReaderPageStream& x);
  FdReaderPageStream& operator=(const FdReaderPageStream& x);
};


#include <errno.h>
#include <string.h> // for strerror
#include <sys/types.h>
#include <unistd.h>


inline
FdReaderPageStream::FdReaderPageStream(const DteTupleAdt& tupAdt, int fd)
: PageStream(tupAdt), fd(fd), page(tupAdt)
{
  assert(fd >= 0);
  page.initPage(&pageBuf);
}


inline
FdReaderPageStream::~FdReaderPageStream()
{
}


inline
void FdReaderPageStream::rewind()
{
  int rc = lseek(fd, 0, SEEK_SET);
  assert( rc >= 0 );
}


inline
PageBuf* FdReaderPageStream::getNext()
{
  page.read(fd);
  return &pageBuf;
}


//---------------------------------------------------------------------------


class FdWriterPageStream
: public PageStream
{
public:

  FdWriterPageStream(const DteTupleAdt& tupAdt, int fd);

  virtual ~FdWriterPageStream();

  virtual void rewind();

  virtual PageBuf* getNext();

protected:

  int fd;
  PageBuf pageBuf;
  DteAdtPage page;
  bool firstTime;

private:

  FdWriterPageStream(const FdWriterPageStream& x);
  FdWriterPageStream& operator=(const FdWriterPageStream& x);
};



inline
FdWriterPageStream::FdWriterPageStream(const DteTupleAdt& tupAdt, int fd)
: PageStream(tupAdt), fd(fd), page(tupAdt), firstTime(true)
{
  assert(fd >= 0);
  page.initPage(&pageBuf);
}


inline
FdWriterPageStream::~FdWriterPageStream()
{
  if( !firstTime ) getNext();     // send last page
}


inline
void FdWriterPageStream::rewind()
{
  int rc = lseek(fd, 0, SEEK_SET);
  assert( rc >= 0 );
}


inline
PageBuf* FdWriterPageStream::getNext()
{
  if( firstTime ) {
    firstTime = false;
    return &pageBuf;
  }
  page.write(fd);
  return &pageBuf;
}



#endif // _PAGESTREAM_H_
