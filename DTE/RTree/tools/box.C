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

#include <stdio.h>
#include"box.h"
#include"key.h"
#include"correct_math.h"

dbl_box::dbl_box()
{
  size = 0;
  low = high = NULL;
}

dbl_box::~dbl_box()
{
  delete []low;
  delete []high;
}

dbl_box& dbl_box::operator =(dbl_box &box)
{
  if(this == &box) return *this;
  if(size != box.size)
    {
      delete []low;
      delete []high;
      size = box.size;
      low = new double[size+1];
      high = new double[size+1];
    }
  for(int i = 1; i <= size; i++)
    {
      low[i] = box.low[i];
      high[i] = box.high[i];
    }
  return *this;
}

dbl_box& dbl_box::operator =(gen_key_t &box)
{
  if(size != box.dim())
    {
      delete []low;
      delete []high;
      size = box.dim();
      low = new double[size+1];
      high = new double[size+1];
    }
  for(int i = 1; i <= size; i++)
    {
      low[i] = (double)(box.min(i));
      high[i] = (double)(box.max(i));
    }
  return *this;
}

void dbl_box::set_box(const gen_key_t &box)
{
  if(size != box.dim()) 
    {
      delete []low;
      delete []high;
      size = box.dim();
      low = new double[size+1];
      high = new double[size+1];
    }

  int i;
  long long v;
  float f;
  int type_pos = 0;
  char *min_data_pos = (char*)(box.data);
  char *max_data_pos = (char*)(box.data) + (box.data_sz / 2);
  for(int dim_num = 1; dim_num <= size; dim_num++)
    {
      switch(box.types[type_pos]) {
	case 'i':
	  memcpy((char*)(&i),min_data_pos,4);
	  low[dim_num] = (double)(i);
	  memcpy((char*)(&i),max_data_pos,4);
	  high[dim_num] = (double)(i);
	  min_data_pos += 4;
	  max_data_pos += 4;
	  type_pos++;
	  break;
	case 'v':
	  memcpy((char*)(&v),min_data_pos,8);
	  min_data_pos += 8;
	  type_pos++;
	  low[dim_num] = long_to_double_low(v);
	  memcpy((char*)(&v),max_data_pos,8);
	  min_data_pos += 8;
	  high[dim_num] = long_to_double_high(v);
	  break;
	case 'f':
	  memcpy((char*)(&f),min_data_pos,4);
	  low[dim_num] = (double)(f);
	  memcpy((char*)(&f),max_data_pos,4);
	  high[dim_num] = (double)(f);
	  min_data_pos += 4;
	  max_data_pos += 4;
	  type_pos++;
	  break;
	case 'd':
	  memcpy((char*)(&(low[dim_num])),min_data_pos,8);
	  memcpy((char*)(&(high[dim_num])),max_data_pos,8);
	  min_data_pos += 8;
	  max_data_pos += 8;
	  type_pos++;
	  break;
	case 's':
	  sscanf(box.types + (type_pos + 2),"%d",&i);
	  min_data_pos += i;
	  max_data_pos += i;
	  while(box.types[type_pos] != ']') type_pos++;
	  type_pos++;
	  low[dim_num] = 0.0;
	  high[dim_num] = 0.0;
	  break;
        default: break;
      }
    }
}

void dbl_box::set_box(const gen_key_t &box, char *types)
{
  if(size != box.dim()) 
    {
      delete []low;
      delete []high;
      size = box.dim();
      low = new double[size+1];
      high = new double[size+1];
    }

  int i;
  long long v;
  float f;
  int type_pos = 0;
  char *min_data_pos = (char*)(box.data);
  char *max_data_pos = (char*)(box.data) + (box.data_sz / 2);
  for(int dim_num = 1; dim_num <= size; dim_num++)
    {
      switch(types[type_pos]) {
	case 'i':
	  memcpy((char*)(&i),min_data_pos,4);
	  low[dim_num] = (double)(i);
	  memcpy((char*)(&i),max_data_pos,4);
	  high[dim_num] = (double)(i);
	  min_data_pos += 4;
	  max_data_pos += 4;
	  type_pos++;
	  break;
	case 'v':
	  memcpy((char*)(&i),min_data_pos,4);
	  min_data_pos += 4;
	  type_pos++;
	  v = reconstruct_long_low(i);
	  low[dim_num] = (double)(v);
	  memcpy((char*)(&i),max_data_pos,4);
	  max_data_pos += 4;
	  v = reconstruct_long_high(i);
	  high[dim_num] = (double)(v);
	  break;
	case 'f':
	  memcpy((char*)(&i),min_data_pos,4);
	  f = reconstruct_float(i);
	  low[dim_num] = (double)(f);
	  memcpy((char*)(&i),max_data_pos,4);
	  f = reconstruct_float(i);
	  high[dim_num] = (double)(f);
	  min_data_pos += 4;
	  max_data_pos += 4;
	  type_pos++;
	  break;
	case 'd':
	  memcpy((char*)(&i),min_data_pos,4);
	  low[dim_num] = reconstruct_double_low(i);
	  memcpy((char*)(&i),max_data_pos,4);
	  high[dim_num] = reconstruct_double_high(i);
	  min_data_pos += 4;
	  max_data_pos += 4;
	  type_pos++;
	  break;
	case 's':
	  min_data_pos += 4;
	  max_data_pos += 4;
	  while(box.types[type_pos] != ']') type_pos++;
	  type_pos++;
	  low[dim_num] = 0.0;
	  high[dim_num] = 0.0;
	  break;
        default: break;
      }
    }
}

void dbl_box::print()
{
  printf("(%g,%g)",low[1],high[1]);
  for(int i = 2; i <= size; i++)
    printf(" X (%g,%g)",low[i],high[i]);
}

int dbl_box::intersect(const dbl_box& B)
{
  for(int i = 1; i <= size; i++)
    {
      if(low[i] < B.low[i]) low[i] = B.low[i];
      if(high[i] > B.high[i]) high[i] = B.high[i];
      if(low[i] > high[i]) return 1;
    }
  return 0;
}








