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

#include<stdio.h>
#include<stdlib.h>
#include"key.h"
#include<math.h>
#include<strstream.h>
#include<iomanip.h>
#include"correct_math.C"
#include<string.h>

#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))

gen_key_t::gen_key_t()
{
  data = NULL;
  num_dim = 0;
  types = NULL;
  data_sz=0;
}


gen_key_t::gen_key_t(const gen_key_t& key_to_copy)
{
  num_dim = key_to_copy.dim();
  if (key_to_copy.data != NULL)
    {
      data = (int *) malloc(key_to_copy.data_sz);
      memcpy (data, key_to_copy.data, key_to_copy.data_sz);
    }
  else
    {
      data = NULL;
      printf("found NULL thing to copy in key initializer\n");
    }
  if (key_to_copy.types != NULL)
    {
      types = (char *) malloc(strlen(key_to_copy.types));
      memcpy (types, key_to_copy.types, strlen(key_to_copy.types));
    }
  else
    types = NULL;
  data_sz = key_to_copy.data_sz;
}


gen_key_t::gen_key_t(const int *bounds, 
		     const int dim)
{
  num_dim = dim;
  data = (int *) malloc(num_dim*2*sizeof(int));
  for(int i=0; i<num_dim; i++)
    {
      data[i]=bounds[i];
      data[num_dim+i] = bounds[num_dim+i];
    }
  types = (char *)malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
  data_sz = 2*num_dim*sizeof(int);
}


gen_key_t::gen_key_t(const int *bounds, 
		     const int dim,
		     const bool point_data)
{
  num_dim = dim;
  data = (int *) malloc(num_dim*2*sizeof(int));
  data_sz = 2*num_dim*sizeof(int);
  for(int i=0; i<num_dim; i++)
    {
      if (point_data)
	data[i]=data[num_dim+i]= bounds[i];
      else
	{
	  data[i]=bounds[i];
	  data[num_dim+i] = bounds[num_dim+i];
	}
    }
  types = (char *) malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
}


gen_key_t::gen_key_t(char *bounds,
		     int in_dim,
		     const char *in_types,
		     const bool point_data)
{
  int point_data_sz=0;
  num_dim = in_dim;
  types = (char *)malloc(strlen(in_types) + 1);	// added + 1 DD
  strcpy(types, in_types);
  char *type_info = types;
  for(int dim=0; dim<num_dim; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i': case 'f':
	  point_data_sz += 4;
	  break;
	case 'v': case 'd':
	  point_data_sz += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  point_data_sz += len;
	  break;
	default:
	  printf("error in gen_key_t::gen_key_t(with type info\n");
	}
      type_info++;
    }
  data_sz = 2*point_data_sz;
  data = (int *) malloc(data_sz);
  if (point_data)
    {
      memcpy((char *)data, bounds, point_data_sz);
      memcpy((char *)data+point_data_sz, bounds, point_data_sz);
    }
  else
    memcpy((char *)data, bounds, data_sz);
}


void gen_key_t::add_extra(char *extra_plus, 
			  int  elem_size, 
			  char *in_types,
			  int point_data)
{
  data_sz = 0;
  if (types != NULL)
    free(types);
  types = (char *)malloc(strlen(in_types) + 1);
  strcpy(types, in_types);

  char *type_info = types;
  int extra_sz = 0;
  for(int dim=0; dim<num_dim; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i': case 'f':
	  data_sz += 4;
	  break;
	case 'v': case 'd':
	  extra_sz += 4;
	  data_sz += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  extra_sz += maximum(0, len-4);
	  data_sz += maximum(4, len);
	  break;
	default:
	  printf("error in gen_key_t::add_extra(with type info\n");
	}
      type_info++;
    }

  data_sz *= 2;
  if (!point_data)
    extra_sz *= 2;
  char *extra_place = extra_plus+(elem_size-extra_sz);
  char *new_data = (char *)malloc(data_sz);
  char *new_data_pl = new_data;
  char *old_data_pl = (char *) data;
  for(int Rep=0; Rep<=1; Rep++)
    {
      if (point_data)
	extra_place=extra_plus+(elem_size-extra_sz);
      char *type_info = in_types;
      for(int dim=0; dim<num_dim; dim++)
	{
	  switch (type_info[0]) 
	    {
	    case 'i':
	      memcpy(new_data_pl, old_data_pl, 4);
	      new_data_pl += 4;
	      break;
	    case 'f':
	      memcpy(new_data_pl, old_data_pl, 4);
	      flip_bits(*((float *)new_data_pl));
	      new_data_pl += 4;
	      break;
	    case 'v':
	      memcpy(FIRST_WORD_PTR(new_data_pl), old_data_pl, 4);
	      memcpy(LAST_WORD_PTR(new_data_pl), extra_place, 4);
	      new_data_pl += 8;
	      extra_place += 4;
	      break;
	    case 'd':
	      memcpy(FIRST_WORD_PTR(new_data_pl), old_data_pl, 4);
	      memcpy(LAST_WORD_PTR(new_data_pl), extra_place, 4);
	      flip_bits(*((double *)new_data_pl));
	      new_data_pl += 8;
	      extra_place += 4;
	      break;
	    case 's':
	      int len;
	      sscanf(type_info+2, "%d", &len);
	      type_info = strpbrk(type_info, "]");
	      if (len > 4)
		{
		  memcpy(new_data_pl, old_data_pl, 4);
		  memcpy(new_data_pl+4, extra_place, len-4);
		  extra_place += len-4;
		}
	      else
		memcpy(new_data_pl, old_data_pl+4-len, len);
	      new_data_pl += len;
	      break;
	    default:
	      printf("error\n");
	    }
	  old_data_pl += 4;
	  type_info++;
	}
    }
  free(data);
  data = (int *) new_data;
}


void gen_key_t::SetMaxBox()
{
  if (data != NULL)
    delete(data);
  data = (int *) malloc(num_dim*2*sizeof(int));
  for (int dim_num = 0; dim_num < num_dim; dim_num++)
    {
      data[dim_num]=0;
      data[dim_num+num_dim]=((1<<30)-1);
    }
  types = (char *) malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
}


void gen_key_t::InitBoxPart (const char *BoxStr)
{
  if (data != NULL)
    free(data);
  if (types != NULL)
    free(types);
  const char *unmatched = BoxStr;
  if (sscanf(unmatched, "%d", &num_dim) == 0)
    {
      num_dim = 0;
      data = NULL;
      return;
    }
  data = (int *) malloc(num_dim*2*sizeof(int));
  data_sz = 2*num_dim*sizeof(int);
  types = (char *) malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';

// new code
  for (int int_num = 0; int_num < num_dim; int_num++)
    {
      unmatched = strstr(unmatched, ".\0")+1;
      if (sscanf(unmatched, "%d", &(data[int_num])) == 0)
	{
	  num_dim = 0;
	  free(data);
	  data = NULL;
	  return;
	}
    }
  for (int int_num = 0; int_num < num_dim; int_num++)
    {
      unmatched = strstr(unmatched, ".\0")+1;
      if (sscanf(unmatched, "%d", &(data[num_dim+int_num])) == 0)
	{
	  num_dim = 0;
	  free(data);
	  data = NULL;
	  return;
	}
    }
}


gen_key_t::gen_key_t(char *box_str)
{
  data = NULL;
  types = NULL;
  InitBoxPart(box_str);
}


gen_key_t::~gen_key_t()
{
  if (data != NULL)
    free(data);
  if (types != NULL)
    free(types);
}


void gen_key_t::min(int dim_num,
		    void * &data_ptr,
		    char * &data_type)
{
  int data_offset=0;
  char *type_info = types;
  for(int dim=1; dim<dim_num; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i': case 'f':
	  data_offset += 4;
	  break;
	case 'v': case 'd':
	  data_offset += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  data_offset += len;
	  break;
	default:
	  printf("error in gen_key_t::min\n");
	}
      type_info++;
    }
  data_ptr = (char *)data+data_offset;
  data_type = type_info;
}


void gen_key_t::max(int dim_num,
		    void * &data_ptr,
		    char * &data_type)
{
  int data_offset=data_sz/2;
  char *type_info = types;
  for(int dim=1; dim<dim_num; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i': case 'f':
	  data_offset += 4;
	  break;
	case 'v': case 'd':
	  data_offset += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  data_offset += len;
	  break;
	default:
	  printf("error in gen_key_t::max\n");
	}
      type_info++;
    }
  data_ptr = (char *)data+data_offset;
  data_type = type_info;
}


void gen_key_t::print() const
{
  int data_offset=0;
  char *type_info = types;
  printf("( ");
  for(int dim=0; dim<num_dim; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i':
	  printf("i:%d ", *((int *)((char *)data+data_offset)));
	  data_offset += 4;
	  break;
	case 'f':
	  printf("f:%g ", *((float *)((char *)data+data_offset)));
	  data_offset += 4;
	  break;
	case 'v':
	  printf("v:%lld ", *((long long *)((char *)data+data_offset)));
	  data_offset += 8;
	  break;
	case 'd':
	  printf("d:%g ", *((double *)((char *)data+data_offset)));
	  data_offset += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  printf("s:");
	  fwrite((char *)data+data_offset, len, 1, stdout);
	  printf(" ");
	  data_offset += len;
	  break;
	default:
	  printf("error in gen_key_t::print\n");
	}
      type_info++;
    }
  printf(") - ( ");
  type_info = types;
  for(int dim=0; dim<num_dim; dim++)
    {
      switch (type_info[0]) 
	{
	case 'i':
	  printf("i:%d ", *((int *)((char *)data+data_offset)));
	  data_offset += 4;
	  break;
	case 'f':
	  printf("f:%g ", *((float *)((char *)data+data_offset)));
	  data_offset += 4;
	  break;
	case 'v':
	  printf("v:%lld ", *((long long *)((char *)data+data_offset)));
	  data_offset += 8;
	  break;
	case 'd':
	  printf("d:%g ", *((double *)((char *)data+data_offset)));
	  data_offset += 8;
	  break;
	case 's':
	  int len;
	  sscanf(type_info+2, "%d", &len);
	  type_info = strpbrk(type_info, "]");
	  printf("s:");
	  fwrite((char *)data+data_offset, len, 1, stdout);
	  printf(" ");
	  data_offset += len;
	  break;
	default:
	  printf("error in gen_key_t::print\n");
	}
      type_info++;
    }
  printf(")\n");
}


// reassign the current value of the key based on the data passed in through
// DataPtr. dim is the number of dimensions
void gen_key_t::interpret_key(void *DataPtr, // Points to data to interpret
			      int  dim)      // number of dimensions represented
{
  if (data == NULL)
    data = (int *) malloc(dim*2*sizeof(int));
  memcpy (data, DataPtr, dim*2*sizeof(int));
  num_dim = dim;
  if (types == NULL)
    types = (char *)malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
  data_sz = 2*num_dim*sizeof(int);
}


void gen_key_t::interpret_key(bit_vec          &bv_ext,          // Points to data to interpret
			      int              dim,              // number of dimensions represented
			      const gen_key_t& frame_of_ref,     // F.O.R. for the page the key is from
			      char             *dim_allocation,  // dimension alloc scheme from a page
			      bool             lossless)
{
  int dim_index, dim_size, data_min, data_max,
      max_bit_repr;

//  frame_of_ref.print();printf("\n");
//  printf("%d %d %d %d %d %d %d %d\n", dim, dim_allocation[0], dim_allocation[1], 
//	 ((unsigned char *)DataPtr)[0], ((unsigned char *)DataPtr)[1], 
//	 ((unsigned char *)DataPtr)[2], ((unsigned char *)DataPtr)[3], 
//	 ((unsigned char *)DataPtr)[4]);

  if (data == NULL)
    data = (int *) malloc(dim*2*sizeof(int));
  if (types == NULL)
    types = (char *)malloc(dim+1);
  for(int type_place=0; type_place < dim; type_place++)
    types[type_place]='i';
  types[dim]='\0';
  data_sz = 2*num_dim*sizeof(int);
  num_dim = dim;
  for (dim_index=1; dim_index <= dim; dim_index++)
    {
      dim_size=dim_allocation[dim_index-1];
      data_min = bv_ext.extract(dim_size);
      data_max = bv_ext.extract(dim_size);

      max_bit_repr = (1 << dim_size) - 1;
      if (dim_size<0)
	{
	  printf("dim_size_error in interpret 1 key %d %d\n", dim_size, lossless);
	  printf("FOR:");frame_of_ref.print();printf("\n");
	}
      if (lossless)
	{
	  data[(dim_index-1)] = frame_of_ref.min(dim_index) + data_min;
	  data[dim+(dim_index-1)] = frame_of_ref.min(dim_index) + data_max;
	}
      else
	{
	  if (max_bit_repr != 0)
	    {
	      data[(dim_index-1)] = frame_of_ref.min(dim_index) +
		((long long)(frame_of_ref.max(dim_index)-
			     frame_of_ref.min(dim_index))*data_min) / max_bit_repr;
	      data[dim+(dim_index-1)] = frame_of_ref.min(dim_index) +
		((long long)(frame_of_ref.max(dim_index)-
			     frame_of_ref.min(dim_index))*data_max) / max_bit_repr;
	      if ((((long long)(frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index))
		    *data_max) % max_bit_repr) != 0)
		data[dim+(dim_index-1)]++;
	    }
	  else
	    {
	      data[(dim_index-1)] = frame_of_ref.min(dim_index);
	      data[dim+(dim_index-1)] = frame_of_ref.max(dim_index);
	    }
	}
    }
}


// the size in bits needed to represent this key on a page
int gen_key_t::page_repr_size() const
{
  return 2*num_dim*sizeof(int)*8;
}


// a pointer to an allocated chunk of memory that is the representation of the key
// to be used on a page.
void *gen_key_t::page_repr() const
{
  void *TempResult;

  if (data == NULL)
    {
      printf("returning NULL in gen page_repr\n");
      return NULL;
    }
  TempResult=(void *) malloc(2*num_dim*sizeof(int));
  memcpy (TempResult, data, 2*num_dim*sizeof(int));
  return TempResult;
}

inline int div8(long x)         { return x >> 3; }

#if 0
void bm_zero(char* bm, int size)
{
    int n = div8(size - 1) + 1;
    for (int i = 0; i < n; i++, bm++)   *bm = 0;
}
#endif

// a pointer to an allocated chunk of memory that is the representation of the key
// to be used on a page.
void *gen_key_t::page_repr(gen_key_t frame_of_ref,           // F.O.R. for the page the key from
			   char      *dim_allocation,
			   bool      lossless) const
{
  int    data_min, data_max, dim_index, total_bits, alloc_size, max_bit_repr;
  char *ret_val;

  if (data == NULL)
    {
      printf("real bad bad bad bad bad bad bad!!!!!!!!!!!!!\n");
      return NULL;
    }
  {
    gen_key_t frame_of_ref_temp=frame_of_ref;
    
    frame_of_ref_temp.expand_enclose(*this);
    if (!(frame_of_ref_temp == frame_of_ref))
      {
	printf("bad in page repr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	frame_of_ref.print();
	printf("\n");
	print();
	printf("\n");
	return NULL;
      }
  }
  for (dim_index=0, total_bits=0; dim_index<num_dim; dim_index++)
    total_bits += 2*dim_allocation[dim_index];
#if 0
  alloc_size=total_bits/32;
  if (total_bits%32 != 0)
    alloc_size++;
  alloc_size*=4;
#endif
  alloc_size = (total_bits + 7) / 8;
  ret_val = (char *) malloc(alloc_size);
  // bm_zero(ret_val, alloc_size*8);
  bit_vec append_engine(ret_val);
  for (dim_index=1; dim_index <= num_dim; dim_index++)
    {
      int dim_size=dim_allocation[dim_index-1];
      max_bit_repr = (1 << dim_size) - 1;
      if (dim_size != 0)
	{
	  if ((frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index)) != 0)
	    if (lossless)
	      {
		data_min = min(dim_index)-frame_of_ref.min(dim_index);
		data_max = max(dim_index)-frame_of_ref.min(dim_index);
	      }
	    else
	      {
		data_min = ((long long)(min(dim_index)-frame_of_ref.min(dim_index))*
			    max_bit_repr) /
			      (frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index));
		data_max = ((long long)(max(dim_index)-frame_of_ref.min(dim_index))*
			    max_bit_repr) /
			      (frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index));
		if ((((long long)(max(dim_index)-frame_of_ref.min(dim_index))*
		      max_bit_repr) %
		     (frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index))) != 0)
		    data_max++;
	      }
	  else
	    {
	      data_min = 0;
	      data_max = 0;
	    }
	  append_engine.append(data_min, dim_size);
	  append_engine.append(data_max, dim_size);
	}
    }
  return ret_val;
}


// a pointer to an allocated chunk of memory that is the representation of the key
// to be used on a page.
void *gen_key_t::point_page_repr(gen_key_t frame_of_ref,          
				 char      *dim_allocation,
				 bool      lossless) const
{
  int    data_min, dim_index, total_bits, alloc_size, max_bit_repr;
  char *ret_val;

  if (data == NULL)
    {
      printf("real bad bad bad bad bad bad bad!!!!!!!!!!!!!\n");
      return NULL;
    }
  {
    gen_key_t frame_of_ref_temp=frame_of_ref;
    
    frame_of_ref_temp.expand_enclose(*this);
    if (!(frame_of_ref_temp == frame_of_ref))
      {
	printf("bad in page repr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	frame_of_ref.print();
	printf("\n");
	print();
	printf("\n");
	return NULL;
      }
  }
  for (dim_index=0, total_bits=0; dim_index<num_dim; dim_index++)
    total_bits += 2*dim_allocation[dim_index];
#if 0
  alloc_size=total_bits/32;
  if (total_bits%32 != 0)
    alloc_size++;
  alloc_size*=4;
#endif
  alloc_size = (total_bits + 7) / 8;
  ret_val = (char *) malloc(alloc_size);
  // bm_zero(ret_val, alloc_size*8);
  bit_vec append_engine(ret_val);
  for (dim_index=1; dim_index <= num_dim; dim_index++)
    {
      int dim_size=dim_allocation[dim_index-1];
      max_bit_repr = (1 << dim_size) - 1;
      if (dim_size != 0)
	{
	  if ((frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index)) != 0)
	    if (lossless)
	      data_min = min(dim_index)-frame_of_ref.min(dim_index);
	    else
	      data_min = ((long long)(min(dim_index)-frame_of_ref.min(dim_index))*
			  max_bit_repr) /
			    (frame_of_ref.max(dim_index)-frame_of_ref.min(dim_index));
	  else
	    data_min = 0;
	  append_engine.append(data_min, dim_size);
	}
    }
  return ret_val;
}


// expand the current key to include key.
void gen_key_t::expand_enclose(const gen_key_t& key) // key to envelope
{
  if (num_dim != key.dim())
    printf("dimension mismatch in expand_enclose\n");
  for (int dim_num=1; dim_num <= num_dim; dim_num++)
    {
      if (min(dim_num) > key.min(dim_num))
	data[(dim_num-1)] = key.min(dim_num);
      if (max(dim_num) < key.max(dim_num))
	data[num_dim+(dim_num-1)] = key.max(dim_num);
    }
}


// calculate the squareness of the current key.
int gen_key_t::squareness() const
{
  int temp_total=0;
  int dim_num;

/*  for (dim_num=1; dim_num <= num_dim; dim_num++)
    if (max_side_len < (max(dim_num)-min(dim_num)))
      max_side_len = max(dim_num)-min(dim_num);
  for (dim_num=1; dim_num <= num_dim; dim_num++)
    temp_total += (int)(((1.0*(max_side_len-(max(dim_num)-min(dim_num))))
			 /max_side_len)*1000);
  temp_total /= num_dim;*/
  for (dim_num=1; dim_num <= num_dim; dim_num++)
    temp_total += max(dim_num)-min(dim_num);
  return temp_total;  
}


// calculate the area of the current key.
long long gen_key_t::area() const
{
  long long temp_total=1;

  for (int dim_num=1; dim_num <= num_dim; dim_num++)
    temp_total *= (max(dim_num) - min(dim_num));
  return temp_total;
}


// calculate the overlap area of the current key and key
long long gen_key_t::overlap_area(const gen_key_t& key) const // key to check overlap with.
{
  long long temp_total=1;
  int temp_max, temp_min;

  for (int dim_num=1; dim_num <= num_dim; dim_num++)
    {
      if (key.min(dim_num) < min(dim_num))
	  temp_min = min(dim_num);
        else
	  temp_min = key.min(dim_num);
      if (key.max(dim_num) > max(dim_num))
	  temp_max = max(dim_num);
        else
	  temp_max = key.max(dim_num);
      if (temp_max > temp_min)
	temp_total *= (temp_max - temp_min);
      else
	return 0;
    }
  return temp_total;  
}


// returns true iff it overlaps with test_key.
bool gen_key_t::overlaps(const gen_key_t& test_key) const
{
  int string_length;
  int imin1,imin2,imax1,imax2;
  long long vmin1,vmin2,vmax1,vmax2;
  float fmin1,fmin2,fmax1,fmax2;
  double dmin1,dmin2,dmax1,dmax2;
//  print();
//  test_key.print();

  int test_type_pos=0;
  int this_type_pos=0;
  char *min_test_data_pos = (char*)(test_key.data);
  char *max_test_data_pos = ((char*)(test_key.data)) + (test_key.data_sz / 2);
  char *min_this_data_pos = ((char*)data);
  char *max_this_data_pos = ((char*)data) + (data_sz / 2);
  for (int dim_num=1; dim_num <= num_dim; dim_num++)
    {
      switch(types[this_type_pos]) {
        case 'i':
	  memcpy((char*)(&imin1),min_this_data_pos,4);
	  min_this_data_pos += 4;
	  memcpy((char*)(&imin2),min_test_data_pos,4);
	  min_test_data_pos += 4;
	  memcpy((char*)(&imax1),max_this_data_pos,4);
	  max_this_data_pos += 4;
	  memcpy((char*)(&imax2),max_test_data_pos,4);
	  max_test_data_pos += 4;
	  if(imin1 > imax2 || imin2 > imax1) return false;
	  this_type_pos++;
	  test_type_pos++;
	  break;
        case 'v':
	  memcpy((char*)(&vmin1),min_this_data_pos,8);
	  min_this_data_pos += 8;
	  memcpy((char*)(&vmax1),max_this_data_pos,8);
	  max_this_data_pos += 8;
	  if(test_key.types[test_type_pos] == 'v')
	    {
	      memcpy((char*)(&vmin2),min_test_data_pos,8);
	      min_test_data_pos += 8;
	      memcpy((char*)(&vmax2),max_test_data_pos,8);
	      max_test_data_pos += 8;
	    }
          else
	    {
	      memcpy((char*)(&imin2),min_test_data_pos,4);
	      vmin2 = reconstruct_long_low(imin2);
	      min_test_data_pos += 4;
	      memcpy((char*)(&imax2),max_test_data_pos,4);
	      vmax2 = reconstruct_long_high(imax2);
	      max_test_data_pos += 4;
	    }
	  if(vmin1 > vmax2 || vmin2 > vmax1) return false;
	  this_type_pos++;
	  test_type_pos++;
	  break;
        case 'f':
	  memcpy((char*)(&fmin1),min_this_data_pos,4);
	  min_this_data_pos += 4;
	  memcpy((char*)(&fmax1),max_this_data_pos,4);
	  max_this_data_pos += 4;
	  if(test_key.types[test_type_pos] == 'f')
	    {
	      memcpy((char*)(&fmin2),min_test_data_pos,4);
	      memcpy((char*)(&fmax2),max_test_data_pos,4);
	    }
          else
	    {
	      memcpy((char*)(&imin2),min_test_data_pos,4);
	      fmin2 = reconstruct_float(imin2);
	      memcpy((char*)(&imax2),max_test_data_pos,4);
	      fmax2 = reconstruct_float(imax2);
	    }
	  min_test_data_pos += 4;
	  max_test_data_pos += 4;
	  if(fmin1 > fmax2 || fmin2 > fmax1) return false;
	  this_type_pos++;
	  test_type_pos++;
	  break;
        case 'd':
	  memcpy((char*)(&dmin1),min_this_data_pos,8);
	  min_this_data_pos += 8;
	  memcpy((char*)(&dmax1),max_this_data_pos,8);
	  max_this_data_pos += 8;
	  if(test_key.types[test_type_pos] == 'd')
	    {
	      memcpy((char*)(&dmin2),min_test_data_pos,8);
	      min_test_data_pos += 8;
	      memcpy((char*)(&dmax2),max_test_data_pos,8);
	      max_test_data_pos += 8;
	    }
          else
	    {
	      memcpy((char*)(&imin2),min_test_data_pos,4);
	      dmin2 = reconstruct_double_low(imin2);
	      min_test_data_pos += 4;
	      memcpy((char*)(&imax2),max_test_data_pos,4);
	      dmax2 = reconstruct_double_high(imax2);
	      max_test_data_pos += 4;
	    }
	  if(dmin1 > dmax2 || dmin2 > dmax1) return false;
	  this_type_pos++;
	  test_type_pos++;
	  break;
        case 's':
	  sscanf(types + (this_type_pos+2),"%d",&string_length);
          while(types[this_type_pos] != ']') this_type_pos++;
          this_type_pos++;
	  if(test_key.types[test_type_pos] == 's')
	    {
              while(test_key.types[test_type_pos] != ']') test_type_pos++;
              test_type_pos++;
	      if(str_gt(min_test_data_pos, string_length, 
		 max_this_data_pos, string_length))
		return false;
	      if(str_gt(min_this_data_pos, string_length, 
		 max_test_data_pos, string_length))
		return false;
              min_this_data_pos += string_length;
              max_this_data_pos += string_length;
              min_test_data_pos += string_length;
              max_test_data_pos += string_length;
	    }
          else
	    {
              test_type_pos++;
	      if(string_length < 4)
		{
                  if(str_gt(min_this_data_pos, string_length, 
		     max_test_data_pos + (4 - string_length), string_length)) 
                    return false;
                  if(str_gt(min_test_data_pos + (4 - string_length), string_length, 
		     max_this_data_pos, string_length)) 
                    return false;
                }
              else
		{
                  if(str_gt(min_this_data_pos, string_length, max_test_data_pos, 4)) 
                    return false;
                  if(str_gt(min_test_data_pos ,4 , max_this_data_pos, string_length)) 
                    return false;
                }
              min_test_data_pos += 4;
              max_test_data_pos += 4;
              min_this_data_pos += string_length;
              max_this_data_pos += string_length;
	    }
	  break;
        default: 
	  return false;
	  break;
      }
    }
  return true;
}


bool gen_key_t::simple_overlaps (const gen_key_t& B) const
{
  return overlaps(B);
}


bool gen_key_t::overlaps(const gen_key_t& test_key,
			       dbl_box&   Bp,
			       int        num_iter,
			       bool       is_point) const
{
  return overlaps(test_key);
}


/*void gen_key_t::print() const
{
  if (data != NULL)
    {
      int dim_num;

      for (dim_num=1; dim_num<=num_dim-1; dim_num++)
	printf("%d.", min(dim_num));
      printf("%d, ", min(num_dim));
      for (dim_num=1; dim_num<=num_dim-1; dim_num++)
	printf("%d.", max(dim_num));
      printf("%d", max(num_dim));
    }      
}
*/

char *gen_key_t::create_value_str() const
{
  ostrstream str;

  if (data != NULL)
    {
      int dim_num;

      str.form("2.");
      for (dim_num=1; dim_num<=num_dim; dim_num++)
	str.form("%d.", min(dim_num));
      for (dim_num=1; dim_num<=num_dim-1; dim_num++)
	str.form("%d.", max(dim_num));
      str.form("%d", max(num_dim));
    }
  str << ends;
  return str.str();
}


void gen_key_t::print_visualize_info_to_file(FILE *outfile)
{
  if (data != NULL)
    {
      int dim_num;

      for (dim_num=1; dim_num<=num_dim; dim_num++){
	 	int tmp = min(dim_num);
		for(int i = 0; i < 4; i++){
			fprintf(outfile, "%x ", *((char*) &tmp + i));
		}
	}
//	fprintf(outfile, "%d ", min(dim_num));
      for (dim_num=1; dim_num<=num_dim; dim_num++){
	 	int tmp = max(dim_num);
		for(int i = 0; i < 4; i++){
			fprintf(outfile, "%x ", *((char*) &tmp + i));
		}
	}

//	fprintf(outfile, "%d ", max(dim_num));
    }        
}


gen_key_t& gen_key_t::operator=(const gen_key_t& key_to_copy)
{
  num_dim = key_to_copy.dim();
  data_sz = key_to_copy.data_sz;

  if (key_to_copy.data != NULL)
    {
      if (data == NULL)
	data = (int *) malloc(data_sz);
      memcpy (data, key_to_copy.data, data_sz);
    }
  else
    {
      data = NULL;
      printf("copying NULL value in assignment operator for key\n");
    }
  if (key_to_copy.types != NULL)
    {
      types = (char *) malloc(strlen(key_to_copy.types));
      memcpy (types, key_to_copy.types, strlen(key_to_copy.types));
    }
  else
    {
      printf("error in =\n");
      types = NULL;
    }
  return (*this);
}


bool gen_key_t::operator==(const gen_key_t& key_to_cmp)
{
  if (num_dim != key_to_cmp.dim())
    return false;
  if (memcmp(data, key_to_cmp.data, num_dim*2*sizeof(int))==0)
    return true;
  return false;
}


long LinOverlap;
long CheckOverlap;
long BBoxOverlap;


LinearConstr::LinearConstr () : gen_key_t()
{
  a = NULL;
}


LinearConstr::LinearConstr (int dim_num, double **Ina, int Inm, int Inn, int Inm1, int Inm2,
			    int Inm3) : gen_key_t()
{
  num_dim = dim_num;
  a = Ina;
  m = Inm;
  n = Inn;
  m1 = Inm1;
  m2 = Inm2;
  m3 = Inm3;
  BP.set_box(*this);
}


LinearConstr::LinearConstr(const LinearConstr &L, const char *Bp) : gen_key_t()
{
  InitBoxPart(Bp);
  // For each equality constraint we make two inequalities
  m1 = L.m1+L.m3;
  m2 = L.m2+L.m3;
  m3 = 0; 
  m = m1 + m2 + m3;
  n = L.n;

  a = (double **)malloc(sizeof(double *) * (m+3));
  if(a == NULL)
    {
      printf("malloc problem in LinearConstr::LinearConstr(LinearConstr,gen_key_t)\n");
      exit(1);
    }
  int i;
  for(i = 0; i < m+2; i++)
    {
      a[i] = (double *)malloc(sizeof(double) * (n+2));
      if(a[i] == NULL)
        {
          printf("malloc problem in LinearConstr::LinearConstr(LinearConstr,gen_key_t)\n");
          exit(1);
        }
    }

  // Now we transfer the constraints from "L" to the array "a"
  // We start with the <= constraints.
  for(i = 2; i <= L.m1+1; i++)
    for(int j = 1; j <= n+1; j++)
      a[i][j] = L.a[i][j];
  // Now we go through the equalities and make a <= out of them
  int index = L.m1+2;
  for(i = L.m1+L.m2+2; i <= L.m1+L.m2+L.m3+1; i++,index++)
    for(int j = 1; j <= n+1; j++)
      a[index][j] = L.a[i][j];
  // Now go through the >= constraints
  for(i = L.m1+2; i <= L.m1+L.m2+1; i++,index++)
    for(int j = 1; j <= n+1; j++)
      a[index][j] = L.a[i][j];
  // Now we go through the equalities and make a >= out of them
  for(i = L.m1+L.m2+2; i <= L.m1+L.m2+L.m3+1; i++,index++)
    for(int j = 1; j <= n+1; j++)
      a[index][j] = L.a[i][j];

//printf("After transfer the constraints are:\n"); display();
    
  // Now we make the constraint conservative and with a short mantisa.
  // The idea is that a constraint A[i][1]X1+...+A[i][n]Xn >= A[i][0]
  // can be written as A'[i][1]X1+..+A'[i][n]Xn + e[i][1]X1+..+e[i][n]Xn >= A[i][0]
  // Where A[i][j] = A'[i][j] + e[i][j] and A'[i][j] has a short mantisa.
  // Consider now A'[i][1]X1+..+A'[i][n]Xn >= A[i][0] - e[i][1]X1-..-e[i][n]Xn
  // All we need to do is change the A[i][j]'s into A'[i][j] and 
  // Change A[i][0] into a low estimate of A[i][0] - e[i][1]X1-..-e[i][n]Xn
  // (We can use the safe dot product function for that purpose).
  double *e = (double*)malloc(sizeof(double) * (n+2));
  double *z = (double*)malloc(sizeof(double) * (n+2));
  if(e == NULL || z == NULL)
    {
      printf("malloc problem in LinearConstr::LinearConstr(LinearConstr,gen_key_t)\n");
      exit(1);
    }
  e[0] = z[0] = 0.0;
  z[1] = 1.0;
  float f;
  double d_up,d_down;

  // First do the <= constraints
  for(i = 2; i <= m1+1; i++)
    {
      int j;
      for(j = 2; j <= n+1; j++)
	{
	  e[j] = a[i][j];
	  // The next two lines cut the mantisa into 23 bits and assumes
	  // the magnitude of the numbers can fit into a float (8 bit exponent
	  // instead of 11).
          f = (float)(a[i][j]);
	  a[i][j] = (double)f;
	  e[j] -= a[i][j];
	}
      e[1] = a[i][1];
      for(j = 2; j <= n+1; j++)
	if(e[j] >= 0.0) 
	  z[j] = (double)(max(j-1));
        else 
	  z[j] = (double)(min(j-1));
      dot(e+1,z+1,n+1,d_up,d_down);
      a[i][1] = d_up;
    }

  // Then do the >= constraints
  for(i = m1+2; i <= m1+m2+1; i++)
    {
      int j;
      for(j = 2; j <= n+1; j++)
	{
	  e[j] = a[i][j];
	  // The next two lines cut the mantisa into 23 bits and assumes
	  // the magnitude of the numbers can fit into a float (8 bit exponent
	  // instead of 11).
          f = (float)(a[i][j]);
	  a[i][j] = (double)f;
	  e[j] -= a[i][j];
	}
      e[1] = a[i][1];
      for(j = 2; j <= n+1; j++)
	if(e[j] >= 0.0) 
	  z[j] = (double)(min(j-1));
        else 
	  z[j] = (double)(max(j-1));
      dot(e+1,z+1,n+1,d_up,d_down);
      a[i][1] = d_down;
    }
  free((char*)e);
  free((char*)z);
  BP.set_box(*this);
}


LinearConstr::~LinearConstr()
{
}

bool LinearConstr::overlaps (const gen_key_t& box) const
{ 
  int RowNum;

  for (RowNum = 2; RowNum <= 2+m1-1; RowNum++)
    {
double d = a[RowNum][1];
for(int j = 2; j <= n+1; j++) d += a[RowNum][j]*box.min(j-1);
      if ((a[RowNum][2]*box.min(1) + a[RowNum][3]*box.min(2) + a[RowNum][1] <= 0) &&
	  (a[RowNum][2]*box.min(1) + a[RowNum][3]*box.max(2) + a[RowNum][1] <= 0) &&
	  (a[RowNum][2]*box.max(1) + a[RowNum][3]*box.min(2) + a[RowNum][1] <= 0) &&
	  (a[RowNum][2]*box.max(1) + a[RowNum][3]*box.max(2) + a[RowNum][1] <= 0))
	return false;
    }
  for (RowNum = 2+m1; RowNum <= 2+m1+m2-1; RowNum++)
    {
      if ((a[RowNum][2]*box.min(1) + a[RowNum][3]*box.min(2) + a[RowNum][1] >= 0) &&
	  (a[RowNum][2]*box.min(1) + a[RowNum][3]*box.max(2) + a[RowNum][1] >= 0) &&
	  (a[RowNum][2]*box.max(1) + a[RowNum][3]*box.min(2) + a[RowNum][1] >= 0) &&
	  (a[RowNum][2]*box.max(1) + a[RowNum][3]*box.max(2) + a[RowNum][1] >= 0))
	return false;
    }
  return(true);
}


bool LinearConstr::overlaps(const gen_key_t &B, dbl_box &Bp, 
			    int max_iter, bool more_conservative) const
{
  // Do the clipping algorithm

  // Set "Bp" to be the intersection of "B" and "Bp". 
  // If the intersection is empty than return a "no intersection" result
  dbl_box b;
  if(strcmp(B.types,types) == 0)
    b.set_box(B);
  else
    b.set_box(B, types);
  int empty_flag = Bp.intersect(b);
  if(empty_flag) return false;

  // Start the iterations.
  int i;
  int change_flag,j;
  double *x = new double[n+2];
  x[1] = 1.0;
  double d_up, d_down, temp;
  float f;
  do {
    change_flag = 0;
    max_iter--;
    // One iteration here

    // Go through the <= constraints
    for(i = 2; i <= m1+1; i++)
      {
        for(j = 2; j <= n+1; j++)
	  if(a[i][j] >= 0.0) 
	    x[j] = Bp.max(j-1);
          else
	    x[j] = Bp.min(j-1);
        dot(a[i]+1,x+1,n+1,d_up,d_down);
        if(d_up < 0.0)
          {
            delete []x;
            return false;
          }
	temp = -d_up;
	d_up = -d_down;
	d_down = temp;
        // clip each dimension
	for(j = 2; j <= n+1; j++)
	  {
            if(a[i][j] > 0.0)
	      {
	        temp = sum_down(Bp.max(j-1), div_down(d_down, a[i][j]));
		if(temp > Bp.min(j-1)) 
		  {
		    if(more_conservative)
		      {
			f = (float)temp;
			if(temp < 0.0) next_down(f);
			Bp.set_min(j-1,(double)f);
		      }
		    else
		      Bp.set_min(j-1,temp);
		    change_flag = 1;
                  }
              }
            if(a[i][j] < 0.0)
	      {
	        temp = sum_up(Bp.min(j-1), div_up(d_down, a[i][j]));
		if(temp < Bp.max(j-1)) 
		  {
		    if(more_conservative)
		      {
			f = (float)temp;
			if(temp > 0.0) next_up(f);
			Bp.set_max(j-1,(double)f);
		      }
		    else
		      Bp.set_max(j-1,temp);
		    change_flag = 1;
                  }
	      }
	  }
      }

    // Go through the >= constraints
    for(i = m1+2; i < m1+m2+1; i++)
      {
        for(j = 2; j <= n+1; j++)
	  if(a[i][j] >= 0.0) 
	    x[j] = Bp.min(j-1);
          else
	    x[j] = Bp.max(j-1);
        dot(a[i]+1,x+1,n+1,d_up,d_down);
        // The simple test :
	if(d_down > 0.0) 
	  {
	    delete []x;
	    return false;
          }
        // clip each dimension
	for(j = 2; j <= n+1; j++)
	  {
            if(a[i][j] < 0.0)
	      {
	        temp = sub_down(Bp.max(j-1), div_up(d_up, a[i][j]));
		if(temp > Bp.min(j-1)) 
		  {
		    if(more_conservative)
		      {
			f = (float)temp;
			if(temp < 0.0) next_down(f);
			Bp.set_min(j-1,(double)f);
		      }
		    else
		      Bp.set_min(j-1,temp);
		    change_flag = 1;
                  }
              }
            if(a[i][j] > 0.0)
	      {
	        temp = sub_up(Bp.min(j-1), div_down(d_down, a[i][j]));
		if(temp < Bp.max(j-1)) 
		  {
		    if(more_conservative)
		      {
			f = (float)temp;
			if(temp > 0.0) next_up(f);
			Bp.set_max(j-1,(double)f);
		      }
		    else
		      Bp.set_max(j-1,temp);
		    change_flag = 1;
                  }
	      }
	  }
      }
  } while(max_iter > 0 && change_flag);
  delete []x;
  return true;
}


bool LinearConstr::simple_overlaps(const gen_key_t &B) const
{
  // Set the intersection of B and Bp
  dbl_box b;
  static double z[256];
  if(strcmp(B.types,types) == 0)
    b.set_box(B);
  else
    b.set_box(B, types);

  b.intersect(BP);

  int i;
  // Check each constraint
  z[1] = 1.0;
  // Check the <= constraints
  for(i = 2; i <= m1+1; i++)
    {
      for(int j = 2; j <= n+1; j++)
	if(a[i][j] >= 0.0) 
	  z[j] = b.max(j-1);
        else
	  z[j] = b.min(j-1);
      if(sign_of_dot(a[i]+1,z+1,n+1) < 0) 
	return false;
    }
  // Check the >= constraints 
  for(i = m1+2; i <= m1+m2+1; i++)
    {
      for(int j = 2; j <= n+1; j++)
	if(a[i][j] >= 0.0) 
	  z[j] = b.min(j-1);
        else
	  z[j] = b.max(j-1);
      if(sign_of_dot(a[i]+1,z+1,n+1) > 0) 
        return false;
    }
  return true;
}

void LinearConstr::print() const
{
  int i,j;

  printf ("m : %d\nn : %d\nm1 : %d\nm2 : %d\nm3 : %d\na:\n", 
	  m, n, m1, m2, m3);
  for (i=1; i<=m+1; i++)
    {
      for (j=1; j<=n+1; j++)
	printf("%f ", a[i][j]);
      printf("\n");
    }
}

void LinearConstr::display() const
{
  int i,j,first_flag;
  for(i = 2; i <= m1+m2+m3+1; i++)
    {
      first_flag = 1;
      for(j = 2; j <= n+1; j++)
        if(a[i][j] != 0.0) 
	  if(first_flag)
	    {
              first_flag = 0;
	      printf("%g*X%d ",-a[i][j],j-1);
	    }
          else if(a[i][j] > 0.0)
	    printf("- %gX%d ",a[i][j],j-1);
          else printf("+ %gX%d ",-a[i][j],j-1);
      if(i <= m1+1)
	printf("<= %g\n",a[i][1]);
      else if(i <= m1+m2+1)
	printf(">= %g\n",a[i][1]);
      else printf("= %g\n",a[i][1]);
    }
}


