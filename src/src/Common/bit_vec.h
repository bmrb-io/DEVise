#ifndef _BIT_VEC_H
#define _BIT_VEC_H

#include"setup.h"

typedef u_dbword_t bv_word;

const int BITS_PER_WORD = sizeof(bv_word)*8;
const bv_word MAX_BV_WORD_VAL = MAX_U_DBWORD_VAL;

class bit_vec_t
{
  protected:

    bv_word *p;

    int bit_pos;


  public:
    bit_vec_t() {} /* don't use this unless you use init_vec */
    bit_vec_t(bv_word *data_ptr, int bit_offset = 0);

    void init_vec(bv_word *data_ptr, int bit_offset = 0);
    void extract(bit_vec_t extract_to, int bits);
    void append(bit_vec_t copy_from, int bits);
    void advance(int num_bytes, int num_bits);
    void yank(int bits_to_yank, int vec_len);

    // assumes there are zeroes in all bits not copied
    void append(bv_word val, int bits);
    bv_word extract(int bits);
};


inline bit_vec_t::bit_vec_t(bv_word *data_ptr, int bit_offset)
{
  p = (bv_word *)data_ptr;
  p += bit_offset / BITS_PER_WORD;
  bit_pos = BITS_PER_WORD - bit_offset % BITS_PER_WORD;
}


inline void bit_vec_t::init_vec(bv_word *data_ptr, int bit_offset)
{
  p = (bv_word *)data_ptr;
  p += bit_offset / BITS_PER_WORD;
  bit_pos = BITS_PER_WORD - bit_offset % BITS_PER_WORD;
}


inline void bit_vec_t::advance(int num_bytes, int num_bits)
{
  p+=num_bytes;
  if (bit_pos-num_bits <= 0)
    {
      p++;
      bit_pos = BITS_PER_WORD - (num_bits-bit_pos);
    }
}



inline void bit_vec_t::append(bit_vec_t copy_from, int bits)
{
  if (bits <= 0)
    return;

  while ((bits > bit_pos) || (bits > copy_from.bit_pos))
    {
      bv_word right_mask_1 = (MAX_BV_WORD_VAL >> (BITS_PER_WORD - bit_pos));
      if (bit_pos > copy_from.bit_pos)
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       copy_from.bit_pos) >> 
				   BITS_PER_WORD-bit_pos);
	  *p &= ~(middle_mask_1);
	  *p |= (((*copy_from.p) << (bit_pos - copy_from.bit_pos)) & 
		 right_mask_1);
	  copy_from.p++;
	  bits -= copy_from.bit_pos;
	  bit_pos -= copy_from.bit_pos;
	  copy_from.bit_pos = BITS_PER_WORD;
	}
      else if (bit_pos < copy_from.bit_pos)
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       bit_pos) >> 
				   BITS_PER_WORD-bit_pos);
	  *p &= ~(middle_mask_1);
	  *p |= (((*copy_from.p) >> (copy_from.bit_pos - bit_pos)) & 
		 right_mask_1);
	  p++;
	  bits -= bit_pos;
	  copy_from.bit_pos -= bit_pos;
	  bit_pos = BITS_PER_WORD;
	}
      else // bit_pos == copy_from.bit_pos
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       copy_from.bit_pos) >> 
				   BITS_PER_WORD-bit_pos);
	  *p &= ~(middle_mask_1);
	  *p |= ((*copy_from.p) & right_mask_1);
	  p++;
	  copy_from.p++;
	  bits -= bit_pos;
	  bit_pos = copy_from.bit_pos = BITS_PER_WORD;
	}
    }
  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - bits) >> 
			   BITS_PER_WORD-bit_pos);
  *p &= ~(middle_mask_1);
  if (bit_pos >= copy_from.bit_pos)
    *p |= (((*copy_from.p) << (bit_pos - copy_from.bit_pos)) & 
	   middle_mask_1);
  else
    *p |= (((*copy_from.p) >> (copy_from.bit_pos - bit_pos)) & 
	   middle_mask_1);
  if (bit_pos == bits)
    {
      p++;
      bit_pos = BITS_PER_WORD;
    }
  else
    bit_pos -= bits;
}


inline void bit_vec_t::append(bv_word val, int bits)
{
//  printf("before:%016llx   adding %016llx\n", p[0], val);
//  printf("%d\n", bit_pos);
  if (bits >= bit_pos)
    {
//      printf("mask:%016llx\n", ~(MAX_BV_WORD_VAL >> BITS_PER_WORD-bit_pos));
//      *p &= MAX_BV_WORD_VAL << bit_pos;
      *p &= ~(MAX_BV_WORD_VAL >> BITS_PER_WORD-bit_pos);
//      printf("after and:%016llx\n", p[0]);
      *p |= val >> (bits - bit_pos);
      bits -= bit_pos;
      p++;
      bit_pos = BITS_PER_WORD;
//      printf("after:%016llx\n", p[-1]);
      if (bits == 0)
	return;
      *p &= ~(MAX_BV_WORD_VAL << (BITS_PER_WORD - bits));
      *p |= val << (BITS_PER_WORD - bits);
      bit_pos -= bits;
    }
  else
    {
      *p &= ~(MAX_BV_WORD_VAL >> (BITS_PER_WORD - bits) << (bit_pos - bits));
      *p |= val << (bit_pos-bits);
      bit_pos -= bits;
    }
}


inline bv_word bit_vec_t::extract(int bits)
{
//  printf("%d %d\n", p, bit_pos);
  bv_word ret_val=0;
  if (bits >= bit_pos)
    {
      ret_val |= (*p) << (BITS_PER_WORD-bit_pos) >> (BITS_PER_WORD-bits);
      bits -= bit_pos;
      p++;
      bit_pos = BITS_PER_WORD;
      if (bits == 0)
	return ret_val;
      ret_val |= (*p) >> (BITS_PER_WORD-bits);
      bit_pos -= bits;
    }
  else
    {
      ret_val |= (*p) << (BITS_PER_WORD-bit_pos) >> (BITS_PER_WORD-bits);
      bit_pos -= bits;
    }
  return ret_val;
}


inline void bit_vec_t::yank(int bits_to_yank, int vec_len)
{
  bit_vec_t bv2(p, bits_to_yank);
  append(bv2, vec_len-bits_to_yank);
}


inline void bit_vec_t::extract(bit_vec_t extract_to, int bits)
{
  if (bits <= 0)
    return;

  while ((bits > extract_to.bit_pos) || (bits > bit_pos))
    {
      bv_word right_mask_1 = (MAX_BV_WORD_VAL >> 
			      (BITS_PER_WORD - extract_to.bit_pos));
      if (extract_to.bit_pos > bit_pos)
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       bit_pos) >> 
				   BITS_PER_WORD-extract_to.bit_pos);
	  *extract_to.p &= ~(middle_mask_1);
	  *extract_to.p |= (((*p) << (extract_to.bit_pos - bit_pos)) & 
		 right_mask_1);
	  p++;
	  bits -= bit_pos;
	  extract_to.bit_pos -= bit_pos;
	  bit_pos = BITS_PER_WORD;
	}
      else if (extract_to.bit_pos < bit_pos)
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       extract_to.bit_pos) >> 
				   BITS_PER_WORD-extract_to.bit_pos);
	  *extract_to.p &= ~(middle_mask_1);
	  *extract_to.p |= (((*p) >> (bit_pos - extract_to.bit_pos)) & 
		 right_mask_1);
	  extract_to.p++;
	  bits -= extract_to.bit_pos;
	  bit_pos -= extract_to.bit_pos;
	  extract_to.bit_pos = BITS_PER_WORD;
	}
      else // extract_to.bit_pos == bit_pos
	{
	  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - 
						       bit_pos) >> 
				   BITS_PER_WORD-extract_to.bit_pos);
	  *extract_to.p &= ~(middle_mask_1);
	  *extract_to.p |= ((*p) & right_mask_1);
	  extract_to.p++;
	  p++;
	  bits -= extract_to.bit_pos;
	  extract_to.bit_pos = bit_pos = BITS_PER_WORD;
	}
    }
  bv_word middle_mask_1 = (MAX_BV_WORD_VAL << (BITS_PER_WORD - bits) >> 
			   BITS_PER_WORD-extract_to.bit_pos);
  *extract_to.p &= ~(middle_mask_1);
  if (extract_to.bit_pos >= bit_pos)
    *extract_to.p |= (((*p) << (extract_to.bit_pos - bit_pos)) & 
	   middle_mask_1);
  else
    *extract_to.p |= (((*p) >> (bit_pos - extract_to.bit_pos)) & 
	   middle_mask_1);
  if (bit_pos == bits)
    {
      p++;
      bit_pos = BITS_PER_WORD;
    }
  else
    bit_pos -= bits;
}


#endif // _BIT_VEC_