// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Universal Data Index Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _UD_INDEX_H
#define    _UD_INDEX_H

#include  <iostream.h>
#include  <unistd.h>

#include  "RecId.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifdef  NO_FUNCNAME
  // Defense in case you're unfortunate enough to not have g++
#    define __FUNCTION__  "<Unknown Function>"
#endif

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define UDINDX_DEFSZE    256        // Will expand after this many.

#define NO_OFFSET    ((off_t)(-1))

class udIndex {

  private:

    off_t     _data_init;   // The data starts here.
    off_t     _filesize;    // zero if not known.

    RecId     _size;        // The size of the current array.
    RecId     _next;        // The next open slot.

    off_t     *offsets;     // The array of offsets, the position in
                            // the array corresponds to the RID.


      // Use binary search to find an rid, return 0 if not found.
    int  binfind(off_t off, RecId left, RecId rght, RecId& rid);

  public:

    udIndex(off_t filesize=0, off_t data_init=0);
   ~udIndex();

      // Insert a new offset.
    int insert(RecId rid, off_t off);

      // Lookup an offset.
    int lookup(RecId rid, off_t& off);

      // Reverse lookup, an offset to an rid. (expensive)
    int revlookup(off_t off, RecId& rid);

      // Stream printing (prints state, for debugging)
    friend ostream& operator<< (ostream& out, const udIndex& udi);
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _UD_INDEX_H
