/* --------------------------------------------------------------- */
/* -- Copyright (c) 1994, 1995 Computer Sciences Department,    -- */
/* -- University of Wisconsin-Madison, subject to the terms     -- */
/* -- and conditions given in the file COPYRIGHT.  All Rights   -- */
/* -- Reserved.                                                 -- */
/* --------------------------------------------------------------- */

/*
 *  $Header$
 */
#ifndef BITMAP_H
#define BITMAP_H

int bm_first_set(const unsigned char* bm, int size, int start);
int bm_first_clr(const unsigned char* bm, int size, int start);
int bm_num_set(const unsigned char* bm, int size);
int bm_num_clr(const unsigned char* bm, int size);
bool bm_is_set(const unsigned char* bm, long offset);
bool bm_is_clr(const unsigned char* bm, long offset);

void bm_zero(unsigned char* bm, int size);
void bm_fill(unsigned char* bm, int size);
void bm_set(unsigned char* bm, long offset);
void bm_clr(unsigned char* bm, long offset);


inline bool bm_is_clr(const unsigned char* bm, long offset)
{
    return !bm_is_set(bm, offset);
}

inline int bm_num_clr(const unsigned char* bm, int size)
{
    return size - bm_num_set(bm, size);
}

#endif 	// BITMAP_H

