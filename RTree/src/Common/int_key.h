#ifndef _INT_KEY_H
#define _INT_KEY_H

#include "setup.h"
#include<stdio.h>
#include<math.h>
#include"bit_vec.h"


template<class T> inline T ret_min(T i,
				   T j)
{
  if (i<=j)
    return i;
  else
    return j;
}


template<class T> inline T ret_max(T i,
				   T j)
{
  if (i>=j)
    return i;
  else
    return j;
}


class typed_key_t;


class int_key_t
{
  friend typed_key_t;

  public:
    int_key_t() {assert(0); /* shouldn't be here */};

    int_key_t(int dim_num);

    int_key_t(const int_key_t& key_to_copy);

    int_key_t(dbword_t *bounds,
	      int      dim_num,
	      bool     point_key);

    int_key_t(char *bounds_str,
	      bool point_key);

    virtual int_key_t::~int_key_t();

    int_key_t& operator=(const int_key_t& key_to_copy);
    void assign(const dbword_t *bounds, int dim_num, bool point_key);
    void assign_min(int dim_num, dbword_t value);
    void assign_max(int dim_num, dbword_t value);

    void interpret(bit_vec_t &in_vec, bool point_key);        // inlined later

    void page_repr(bit_vec_t &out_vec, bool point_key) const; // inlined later

    void expand(const int_key_t &key_to_combine);

    int area_enlg(const int_key_t &key_to_combine);

    double center_dist(int_key_t &comp_key) const;

    bool overlaps (const int_key_t &cmp_key) const;

    // dim_num is >= 1
    dbword_t min(int dim_num) const;

    // dim_num is >= 1
    dbword_t max(int dim_num) const;

    int num_dim() const;

    void print(FILE *outfile) const;

  protected:
    int dim;
    dbword_t *key_val;
};

inline int_key_t& int_key_t::operator=(const int_key_t& key_to_copy)
{
  if(this != &key_to_copy)
    {
      if(dim != key_to_copy.dim)
	{
	  dim = key_to_copy.dim;
	  delete []key_val;
	  key_val = new dbword_t[2 * dim];
	}
      memcpy(key_val, key_to_copy.key_val, 2 * dim * sizeof(dbword_t));
    }
  return *this;
}

inline void int_key_t::assign(const dbword_t *bounds, int dim_num, bool point_key)
{
  if(dim != dim_num)
    {
      dim = dim_num;
      delete []key_val;
      key_val = new dbword_t[2 * dim];
    }
  if(point_key)
    {
      memcpy(key_val, bounds, dim * sizeof(dbword_t));
      memcpy(key_val + dim, bounds, dim * sizeof(dbword_t));
    }
  else
     memcpy(key_val, bounds, 2 * dim * sizeof(dbword_t));
}


inline void int_key_t::assign_min(int dim_num, dbword_t value)
{
  assert(dim_num > 0 && dim_num <= dim);
  key_val[dim_num - 1] = value;
}


inline void int_key_t::assign_max(int dim_num, dbword_t value)
{
  assert(dim_num > 0 && dim_num <= dim);
  key_val[dim + dim_num - 1] = value;
}


inline bool int_key_t::overlaps(const int_key_t &cmp_key) const
{
  assert(cmp_key.dim == dim);
  for(int i=0; i<dim; i++)
    if ((key_val[i] > cmp_key.key_val[i+dim]) ||
	(key_val[i+dim] < cmp_key.key_val[i]))
      return false;
  return true;
}


inline dbword_t int_key_t::min(int dim_num) const
{
  return key_val[(dim_num-1)];
}


inline dbword_t int_key_t::max(int dim_num) const
{
  return key_val[(dim_num-1)+dim];
}


inline int int_key_t::num_dim() const
{
  return dim;
}


inline int int_key_t::area_enlg(const int_key_t &key_to_combine)
{
  int orig_area = 1;
  int new_area = 1;
  for(int i=0; i<dim; i++)
    {
      orig_area *= key_val[i+dim] - key_val[i];
      new_area *= ret_max(key_val[i+dim], key_to_combine.key_val[i+dim]) -
	          ret_min(key_val[i], key_to_combine.key_val[i]);
    }
  return new_area - orig_area;
}


inline double int_key_t::center_dist(int_key_t &comp_key) const
{
  double sum=0;
  for(int i=0; i<dim; i++)
    {
      sum += pow(( ((double)(key_val[i+dim]-key_val[i]))/2.0 ) -
	         ( ((double)(comp_key.key_val[i+dim]-comp_key.key_val[i]))/2.0 ), 2);
    }
  return sqrt(sum);
}


inline void int_key_t::expand(const int_key_t &key_to_combine)
{
  for(int i=0; i<dim; i++)
    {
      key_val[i] = ret_min(key_val[i], key_to_combine.key_val[i]);
      key_val[i+dim] = ret_max(key_val[i+dim], key_to_combine.key_val[i+dim]);
    }
}


inline void int_key_t::interpret(bit_vec_t &in_vec,
				 bool      point_key)
{
  if (point_key)
    for (int i=0; i<dim; i++)
      {
	dbword_t extracted_val = in_vec.extract(sizeof(dbword_t)*8);
	key_val[i]=key_val[i+dim]=extracted_val;
      }
  else
    for (int i=0; i<2*dim; i++)
      {
	dbword_t extracted_val = in_vec.extract(sizeof(dbword_t)*8);
	key_val[i]=extracted_val;
      }
}


inline void int_key_t::page_repr(bit_vec_t &out_vec,
				 bool      point_key) const
{
  if (point_key)
    for (int i=0; i<dim; i++)
      out_vec.append(key_val[i], sizeof(dbword_t)*8);
  else
    for (int i=0; i<2*dim; i++)
      out_vec.append(key_val[i], sizeof(dbword_t)*8);    
}
#endif
