// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)
    based on code by Marc Shapiro (shapiro@sor.inria.fr)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _TuplePtrXPlex_h
#ifdef __GNUG__
#pragma interface
#endif
#define _TuplePtrXPlex_h 1

#ifdef OK
     #undef OK
#endif

#include "TuplePtr.Plex.h"

class TuplePtrXPlex: public TuplePtrPlex
{
  TuplePtrIChunk*       ch;           // cached chunk

  void             make_initial_chunks(int up = 1);

  void             cache(int idx) const;
  void             cache(const TuplePtr* p) const;

  TuplePtr*             dopred(const TuplePtr* p) const;
  TuplePtr*             dosucc(const TuplePtr* p) const;

  inline void             set_cache(const TuplePtrIChunk* t) const; // logically, 
                                               // not physically const
public:
                   TuplePtrXPlex();                 // set low = 0;
                                               // fence = 0;
                                               // csize = default

                   TuplePtrXPlex(int ch_size);      // low = 0; 
                                               // fence = 0;
                                               // csize = ch_size

                   TuplePtrXPlex(int lo,            // low = lo; 
                            int ch_size);      // fence=lo
                                               // csize = ch_size

                   TuplePtrXPlex(int lo,            // low = lo
                            int hi,            // fence = hi+1
                            const TuplePtr  initval,// fill with initval,
                            int ch_size = 0);  // csize= ch_size
                                               // or fence-lo if 0

                   TuplePtrXPlex(const TuplePtrXPlex&);
  
  void             operator= (const TuplePtrXPlex&);

// virtuals


  inline TuplePtr&             high_element ();
  inline TuplePtr&             low_element ();

  inline const TuplePtr&       high_element () const;
  inline const TuplePtr&       low_element () const;

  inline Pix              first() const;
  inline Pix              last() const;
  inline void             prev(Pix& ptr) const;
  inline void             next(Pix& ptr) const;
  int              owns(Pix p) const;
  inline TuplePtr&             operator () (Pix p);
  inline const TuplePtr&       operator () (Pix p) const;

  inline int              low() const; 
  inline int              high() const;
  inline int              valid(int idx) const;
  inline void             prev(int& idx) const;
  inline void             next(int& x) const;
  inline TuplePtr&             operator [] (int index);
  inline const TuplePtr&       operator [] (int index) const;
    
  inline int              Pix_to_index(Pix p) const;
  inline Pix              index_to_Pix(int idx) const;    

  inline int              can_add_high() const;
  inline int              can_add_low() const;
  inline int              full() const;

  int              add_high(const TuplePtr  elem);
  int              del_high ();
  int              add_low (const TuplePtr  elem);
  int              del_low ();

  void             fill(const TuplePtr  x);
  void             fill(const TuplePtr  x, int from, int to);
  void             clear();
  void             reverse();
    
  int              OK () const; 

};


inline void TuplePtrXPlex::prev(int& idx) const
{
  --idx;
}

inline void TuplePtrXPlex::next(int& idx) const
{
  ++idx;
}

inline  int TuplePtrXPlex::full () const
{
  return 0;
}

inline int TuplePtrXPlex::can_add_high() const
{
  return 1;
}

inline int TuplePtrXPlex::can_add_low() const
{
  return 1;
}

inline  int TuplePtrXPlex::valid (int idx) const
{
  return idx >= lo && idx < fnc;
}

inline int TuplePtrXPlex::low() const
{
  return lo;
}

inline int TuplePtrXPlex::high() const
{
  return fnc - 1;
}

inline TuplePtr& TuplePtrXPlex:: operator [] (int idx)
{
  if (!ch->actual_index(idx)) cache(idx);
  return *(ch->pointer_to(idx));
}

inline const TuplePtr& TuplePtrXPlex:: operator [] (int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return *((const TuplePtr*)(ch->pointer_to(idx)));
}

inline  TuplePtr& TuplePtrXPlex::low_element ()
{
  if (empty()) index_error();
  return *(hd->pointer_to(lo));
}

inline  const TuplePtr& TuplePtrXPlex::low_element () const
{
  if (empty()) index_error();
  return *((const TuplePtr*)(hd->pointer_to(lo)));
}

inline  TuplePtr& TuplePtrXPlex::high_element ()
{
  if (empty()) index_error();
  return *(tl()->pointer_to(fnc - 1));
}

inline const TuplePtr& TuplePtrXPlex::high_element () const
{
  if (empty()) index_error();
  return *((const TuplePtr*)(tl()->pointer_to(fnc - 1)));
}

inline  int TuplePtrXPlex::Pix_to_index(Pix px) const
{
  TuplePtr* p = (TuplePtr*)px;
  if (!ch->actual_pointer(p)) cache(p);
  return ch->index_of(p);
}

inline  Pix TuplePtrXPlex::index_to_Pix(int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return (Pix)(ch->pointer_to(idx));
}

inline Pix TuplePtrXPlex::first() const
{
  return Pix(hd->TuplePtrIChunk::first_pointer());
}

inline Pix TuplePtrXPlex::last() const
{
  return Pix(tl()->TuplePtrIChunk::last_pointer());
}

inline void TuplePtrXPlex::prev(Pix& p) const
{
  Pix q = Pix(ch->TuplePtrIChunk::pred((TuplePtr*) p));
  p = (q == 0)? Pix(dopred((const TuplePtr*) p)) : q;
}

inline void TuplePtrXPlex::next(Pix& p) const
{
  Pix q = Pix(ch->TuplePtrIChunk::succ((TuplePtr*) p));
  p = (q == 0)? Pix(dosucc((const TuplePtr*)p)) : q;
}

inline TuplePtr& TuplePtrXPlex:: operator () (Pix p)
{
  return *((TuplePtr*)p);
}

inline const TuplePtr& TuplePtrXPlex:: operator () (Pix p) const
{
  return *((const TuplePtr*)p);
}

inline void TuplePtrXPlex::set_cache(const TuplePtrIChunk* t) const
{
  ((TuplePtrXPlex*)(this))->ch = (TuplePtrIChunk*)t;
}

#endif
