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


#include  <iostream.h>

#include  "udIndex.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
udIndex::udIndex(off_t filesize, off_t data_init)
{
    _filesize  = filesize;
    _data_init = data_init;

    _size = UDINDX_DEFSZE;
    _next = 0;

    offsets = new off_t[_size];
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
udIndex::~udIndex()
{
    delete [] offsets;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Insert a new offset.
int udIndex::insert(RecId rid, off_t off)
{
    if (!this) {
        cerr <<"Don't push that button again. The index is not being used.\n";
        return 0;
    }

      // need to expand the index?
    if (rid >= _size) {

        RecId initsze = _size;
        RecId newsze = _size;
        int try2 = 0;

        do {
            if (_filesize && !try2) {
                // Guess how large to expand to interpolating from how
                // many records we already have.
    
                int avgsze;
                if (offsets[_next-1] != NO_OFFSET)
                    avgsze = (offsets[_next-1] - _data_init) / (_next-1);
                else
                    avgsze = 64;    // A wild guess.
    
                // 16 is just fudge, it would be a shame to be that close.
                newsze = (_filesize - _data_init) / avgsze + 16;
            } else {
                // No information, double the index size.
                newsze += newsze;
            }

            try2++;

        } while (rid >= newsze);

        _size = newsze;

        off_t *old_offs = offsets;
        offsets = new off_t[_size];

        memcpy(offsets, old_offs, initsze * sizeof(off_t));
        delete [] old_offs;
    }

    if (rid == _next) {
        // just what we were expecting, bliss.
        offsets[_next++] = off;

    } else if (rid > _next) {
        // need to insert blank index entries.
        do
            offsets[_next++] = NO_OFFSET;
        while (rid > _next);

        offsets[_next++] = off;

    } else {
        // a correction of a blank entry?
        offsets[rid] = off;
    }

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Lookup an offset.
int udIndex::lookup(RecId rid, off_t& off)
{
    if (!this) {
        cerr <<"Don't push that button again. The index is not being used.\n";
        off = NO_OFFSET;
        return 0;
    }

    if (rid < _next) {
        off = offsets[rid];
        return 1;
    } else {
        off = NO_OFFSET;
        return 0;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Use binary search to find an rid, return 0 if not found.
int udIndex::binfind(off_t off, RecId left, RecId rght, RecId& rid)
{
    RecId mid = (left + rght) / 2;

    if (off < offsets[mid]) {
        if (mid == left)
            return 0;
        return binfind(off,left,mid,rid);
    } else if (off > offsets[mid]) {
        return binfind(off,mid,rght,rid);
    } else {
        rid = mid;
        return 1;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Reverse lookup, an offset to an rid. (expensive)
int udIndex::revlookup(off_t off, RecId& rid)
{
    if ((off < offsets[0]) || (off > offsets[_next-1]))
        return 0;
    else
        return binfind(off, 0, _next, rid);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (prints state, for debugging)
ostream& operator<< (ostream& out, const udIndex& udi)
{
    out << "Index:  Size: " << udi._size << "   Next: " << udi._next << endl;

    for (RecId j=0; j < udi._next; j++)
        out << "Offset["<<j<<"]: " << udi.offsets[j] << endl;

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
