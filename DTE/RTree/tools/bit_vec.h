/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
 */

#ifndef _BIT_VEC_H
#define _BIT_VEC_H

#include <sys/types.h>
#include <netinet/in.h>
#include <assert.h>


// warning: append & insert share state, do not use both with the same instance
class bit_vec
{
  public:

    bit_vec(void *data_ptr, int bit_offset = 0);

    void extract(void *ptr, int bits);
    void append(void *ptr, int bits);

    void append(unsigned val, int bits);
    unsigned extract(int bits);

    // defaults:
    // bit_vec(const bit_vec& other);
    // bit_vec& operator=(const bit_vec& other);

  protected:

    typedef unsigned char bv_word;

    enum {
      BITS_PER_WORD = sizeof(bv_word)*8,
      MAXBIT = BITS_PER_WORD - 1,
      BITS_PER_INT = sizeof(unsigned)*8,
    };

    bv_word* p;

    int first_bit;
};


inline bit_vec::bit_vec(void *data_ptr, int bit_offset)
{
  int extra_words = 0;
  if( bit_offset > MAXBIT ) {
    extra_words = bit_offset / BITS_PER_WORD;
    bit_offset %= BITS_PER_WORD;
  }
  first_bit = MAXBIT - bit_offset;
  p = ((bv_word*) data_ptr) + extra_words;
}


#ifdef unused_code
  unsigned char* v = (unsigned char*)(&val) + 3;
  int last_bit = first_bit + 1;	// the last bit stored (can be 8)
  unsigned char only_left = 0xff << last_bit;
  unsigned char only_right = ~only_left;
  while(bits > 8) {
    *p = (*p & only_left) | (*v & only_right);
    p++;
    *p = (*p & only_right) | (*v & only_left);
    v--;
    bits -= 8;
  }
  if( bits < last_bit ) {
    unsigned char middle = only_right & (only_right << (last_bit - bits));
    *p = (*p & ~middle) | ((*v << (last_bit - bits)) & middle);
    first_bit -= bits;
  } else {
    *p = (*p & only_left) | (*v & only_right);
    p++;
    bits -= last_bit;
    first_bit = 7 - bits;
    if( bits > 0 ) {
      only_left = 0xff << (8 - bits);
      *p = ((*v << (8 - last_bit - bits)) & only_left) | (*p & ~only_left);
    }
  }
}
#endif


inline void bit_vec::append(void *ptr, int bits)
{
  assert(bits >= 0);

  unsigned char* v = (unsigned char*)ptr;
  int last_bit = first_bit + 1;	// the last bit stored (can be 8)
  unsigned char only_left = 0xff << last_bit;
  unsigned char only_right = ~only_left;
  while(bits > 8) {
    *p = (*p & only_left) | (*v >> (8 - last_bit));
    p++;
    *p = (*p & only_right) | (*v << last_bit);
    v++;
    bits -= 8;
  }
  if( bits < last_bit ) {
    unsigned char middle = only_right & (only_right << (last_bit - bits));
    *p = (*p & ~middle) | ( (*v >> (8 - last_bit)) & middle);
    first_bit -= bits;
  } else {
    *p = (*p & only_left) | (*v >> (8 - last_bit));
    p++;
    bits -= last_bit;
    first_bit = 7 - bits;
    if( bits > 0 ) {
      only_left = 0xff << (8 - bits);
      *p = (*p & ~only_left) | ((*v << last_bit) & only_left);
    }
  }
}


inline void bit_vec::extract(void* ptr, int bits)
{
  //  assert(bits >= 0);

  unsigned char* v = (unsigned char*)ptr;
  int last_bit = first_bit + 1;	// the last store bit (can be 8)
  unsigned char only_left = 0xff << last_bit;
  unsigned char only_right = ~only_left;
  while(bits > 8) {
    *v = *p << (8 - last_bit);
    p++;
    *v |= *p >> last_bit;
    v++;
    bits -= 8;
  }
  if( bits < last_bit ) {
    unsigned char middle = only_right & (only_right << (last_bit - bits));
    *v = (*p & middle) << (8 - last_bit);
    first_bit -= bits;
  } else {
    *v = *p << (8 - last_bit);
    p++;
    bits -= last_bit;
    first_bit = 7 - bits;
    if( bits > 0 ) {
      only_left = 0xff << (8 - bits);
      *v |= (*p & only_left) >> last_bit;
    }
  }
}


inline void bit_vec::append(unsigned val, int bits)
{
  assert(bits >= 0 && bits <= BITS_PER_INT);
  val = htonl(val << (BITS_PER_INT - bits));
  append((void*)&val, bits);
}


inline int fast_ntohl(int x)
{ 
  register int y;
  ((char*)&y)[0]=((char*)&x)[3];
  ((char*)&y)[1]=((char*)&x)[2];
  ((char*)&y)[2]=((char*)&x)[1];
  ((char*)&y)[3]=((char*)&x)[0];
  return y;
}

inline unsigned bit_vec::extract(int bits)
{
//  assert(bits >= 0 && bits <= BITS_PER_INT);
  unsigned val = 0;
  extract(&val, bits);
  return ntohl(val) >> (BITS_PER_INT - bits);
//  return (val) >> (BITS_PER_INT - bits);
}



#endif // _BIT_VEC_H
