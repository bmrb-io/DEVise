#ifndef _HEAPRANGEREADER_H_
#define _HEAPRANGEREADER_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/am/HeapReader.h"

class HeapRangeReader : public HeapReader
{
  MyOffset start;
  MyOffset end;

public:

  HeapRangeReader(const DteTupleAdt& adt, int fd,
                  Offset start, Offset end)
    : HeapReader(adt, fd) {
    this->start.offset = start.offset;
    this->end.offset = end.offset;
  }

  virtual ~HeapRangeReader() {}

  virtual void initialize() {
    HeapReader::seek(start.offset);
  }

  virtual const Tuple* getNext() {
    const Tuple* tup = HeapReader::getNext();
    if( currentOffset.split.pageId > end.split.pageId
        || (currentOffset.split.pageId == end.split.pageId
            && currentOffset.split.slot > end.split.slot) ) {
      return NULL;
    }
    assert(tup);
    return tup;
  }

};

#endif // _HEAPRANGEREADER_H_
