#ifndef _TUPLESTREAM_H_
#define _TUPLESTREAM_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/comm/DteAdtPage.h"
#include "DTE/comm/PageStream.h"


class OTupleStream
{
  PageStream& pageStream;
  DteAdtPage page;
  bool eof;

  void getNextPage();

public:

  // the pageStream must exist while this TupleStream exists
  OTupleStream(PageStream& pageStream);

  ~OTupleStream();

  void rewind();

  // Returns a pointer to the copy of t in the page stream.
  // The pointer can be invalidated on the next call to insert(),
  // depending upon the type of PageStream used.
  // Returns NULL if the tuple couldn't fit in the PageStream.
  const Tuple* insert(const Tuple* t);

private:

  OTupleStream(const OTupleStream& x);
  OTupleStream& operator=(const OTupleStream& other);
};


//---------------------------------------------------------------------------


class ITupleStream
{
  PageStream& pageStream;
  DteAdtPage page;
  int slot;

public:

  // the pageStream must exist while this TupleStream exists
  ITupleStream(PageStream& pageStream);

  ~ITupleStream();

  void rewind();

  // Returns a pointer to the next tuple.
  // All previous pointers are invalid, unless the caller has a guarantee
  // from the PageStream
  const Tuple* getNext();

private:

  ITupleStream(const ITupleStream& x);
  ITupleStream& operator=(const ITupleStream& other);
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // _TUPLESTREAM_H_
