#ifndef _HEAPINSERTER_H_
#define _HEAPINSERTER_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/comm/DteAdtPage.h"

class HeapInserter
{
  DteAdtPage page;
  PageBuf* pageBuf;             // use pointer so not too big on stack
  int fd;
  bool closeFd;

  union MyOffset {
    struct {
      short pageId;
      short slot;
    } split;
    int offset;
  } currentOffset;
  //kb: make class Offset bigger

public:

  // the fd should not be used while this class is using it!
  HeapInserter(int fd, const DteTupleAdt& tupAdt, int mode = ios::app)
    : page(tupAdt), fd(fd) { 
    assert(fd >= 0);
    closeFd = false;
    pageBuf = new PageBuf;
    if( mode & ios::app ) {
      // could try to append to last page, rather than new page...
      //off_t rc = lseek(fd, -sizeof(PageBuf), SEEK_END);
      off_t rc = lseek(fd, 0, SEEK_END);
      assert(rc >= 0 && "error seeking to end of heap");
    }
    page.initPage(pageBuf);
    currentOffset.split.pageId = 0;
  }

  HeapInserter(const string& filename, const DteTupleAdt& tupAdt, 
               int mode = ios::app)
    : page(tupAdt) {
    int openMode = O_WRONLY;
    if( !(mode & ios::nocreate) ) openMode |= O_CREAT;
    if( !(mode & ios::noreplace) ) openMode |= O_EXCL;
    if( mode & (ios::app|ios::ate) ) openMode |= O_APPEND;
    if( mode & ios::trunc ) openMode |= O_TRUNC;
    
    fd = ::open(filename.c_str(), openMode, 0600);
    assert(fd >= 0 && "error opening output file");
    closeFd = true;
    pageBuf = new PageBuf;
    page.initPage(pageBuf);
    currentOffset.split.pageId = 0;
  }

  ~HeapInserter() {
    if( page.getNumSlots() > 0 ) { // write last page
      page.write(fd);
    }
    if( closeFd ) close(fd);
    delete pageBuf;
  }

  void insert(const Tuple* tup) {
    const Tuple* newTup = page.addTuple(tup);
    if( !newTup ) {             // tuple didn't fit on page
      page.write(fd);
      page.reinitPage();
      newTup = page.addTuple(tup);
      assert(newTup && "tuple didn't fit on an empty page!");
      currentOffset.split.pageId++;
    }
  }

  // returns the current offset, after any inserts
  Offset getOffset() {
    currentOffset.split.slot = page.getNumSlots();
    //cout << "pageId: " << currentOffset.split.pageId << endl;
    //cout << "slot: " << currentOffset.split.slot << endl;
    //cout << "offset: " << currentOffset.offset << endl;
    return currentOffset.offset;
  }
};

#endif // _HEAPINSERTER_H_
