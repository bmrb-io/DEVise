#ifndef _HEAPREADER_H_
#define _HEAPREADER_H_

// (C) 1999- by Kevin S. Beyer

#include "Iterator.h"


class HeapReader
: public RandomAccessIterator
{
public:

  HeapReader(const DteTupleAdt& adt, int fd);

  HeapReader(const DteTupleAdt& adt, const string& filename);

  ~HeapReader();

  virtual void initialize();

  virtual const Tuple* getNext();

  void seek(Offset offset);

  virtual const Tuple* getThis(Offset offset); //kb: replace with seek

  virtual Offset getOffset();

protected:

  DteTupleAdt adt;

  string filename;

  int fd;

  bool closeFd;

  PageBuf pageBuf[2];

  int bufId;

  int prevPageId;

  int endPageId;

  DteAdtPage page;

  union MyOffset {
    struct {
      short pageId;
      short slot;
    } split;
    int offset;
  } currentOffset;
  //kb: make class Offset bigger

private:

  HeapReader(const HeapReader& x);
  HeapReader& operator=(const HeapReader& x);
};

//kb: move these fuctions to a .C file
inline
HeapReader::HeapReader(const DteTupleAdt& adt, int fd)
: RandomAccessIterator(adt), fd(fd), closeFd(false), page(adt)
{
  assert(fd >= 0 && "bad fd passed to HeapReader");
  off_t seekrc = ::lseek(fd, 0, SEEK_END);
  assert( seekrc >= 0 );
  endPageId = seekrc / DTE_PAGESIZE;
  bufId = 0;
  prevPageId = -2;
  page.initPage(&pageBuf[0]);
  page.initPage(&pageBuf[1]);
  currentOffset.split.pageId = -1;
  currentOffset.split.slot = 0;
}

inline
HeapReader::HeapReader(const DteTupleAdt& adt, const string& filename)
: RandomAccessIterator(adt), filename(filename), page(adt)
{
  fd = ::open(filename.c_str(), O_RDONLY);
  assert(fd >= 0 && "error opening file");
  closeFd = true;
  off_t seekrc = ::lseek(fd, 0, SEEK_END);
  assert( seekrc >= 0 );
  endPageId = seekrc / DTE_PAGESIZE;
  bufId = 0;
  prevPageId = -2;
  page.initPage(&pageBuf[0]);
  page.initPage(&pageBuf[1]);
  currentOffset.split.pageId = -1;
  currentOffset.split.slot = 0;
}


inline
HeapReader::~HeapReader()
{
  if( closeFd ) close(fd);
}

inline
void HeapReader::initialize()
{
  seek(0);
}


inline
void HeapReader::seek(Offset offset)
{
  MyOffset off;
  off.offset = offset.offset;
  //cerr << "seek: " << off.split.pageId << ' ' << off.split.slot << ' ' << currentOffset.split.pageId << endl;
  if( off.split.pageId < endPageId ) {
    if( off.split.pageId != currentOffset.split.pageId ) {
      //cerr << "swapping " << currentOffset.split.pageId << " " << prevPageId << endl;
      bufId = !bufId;             // swap buffers
      page.setPage(&pageBuf[bufId]);
      if( off.split.pageId != prevPageId ) { // need a read
        //cerr << "seeking: " << off.split.pageId << endl;
        off_t seekrc = ::lseek(fd, off.split.pageId * DTE_PAGESIZE, SEEK_SET);
        assert(seekrc >= 0);
				bool ok = page.read(fd);
        assert(ok && "failed to read page");
        //cerr << ' ' << page.getNumSlots() << endl;
      }
      prevPageId = currentOffset.split.pageId;
    }
  }
  currentOffset = off;
}


inline
Offset HeapReader::getOffset()
{
  //cerr << "getOffset: " << currentOffset.split.pageId << ' ' << currentOffset.split.slot << endl;
  MyOffset off = currentOffset;
  if( off.split.slot > 0 ) {    // want previous record, if getNext() was called
    off.split.slot--;
  }
  return Offset(off.offset);
}


inline
const Tuple* HeapReader::getNext()
{
  //cerr << "slot: " << currentOffset.split.slot << "  numSlots: " << page.getNumSlots() << endl;
  while( currentOffset.split.slot >= page.getNumSlots() ) {
    MyOffset off;
		off.offset = 0; // to prevent compile warning
    off.split.pageId = currentOffset.split.pageId + 1;
    off.split.slot = 0;
    if( off.split.pageId >= endPageId ) { // eof
      currentOffset.split.slot = page.getNumSlots() + 1;
      return NULL;
    }
    seek( off.offset );
    //currentOffset.split.pageId++;
    //currentOffset.split.slot = 0;
    //off_t seekrc = ::lseek(fd, currentOffset.split.pageId * DTE_PAGESIZE,
    //SEEK_SET);
    //assert(seekrc >= 0);
    //if( !page.read(fd) ) {
    //return NULL;
    //}
  }
  return page.getTuple(currentOffset.split.slot++);
}


inline
const Tuple* HeapReader::getThis(Offset offset)
{
  seek(offset);
  return getNext();
}


#endif // _HEAPREADER_H_
