#ifndef MYKEY_H
#define MYKEY_H

#include"stdlib.h"
#include"box.h"
#include<stdio.h>
#include"bit_vec.h"

class gen_key_t 
{
  friend class dbl_box;
  friend class port_genrtree_p;
  friend class port_genrtree_m;
  friend class LinearConstr;

  public:
    gen_key_t();

    gen_key_t(const gen_key_t& key_to_copy);

    gen_key_t(const int *bounds, 
	      const int dim);

    gen_key_t(const int *bounds, 
	      const int dim,
	      const bool point_data);

    gen_key_t(char *box_str);

    gen_key_t(char *bounds,
	      int  in_dim,
	      const char *in_types,
	      const bool point_data);

    void InitBoxPart(const char *BoxStr);

    void SetMaxBox();

    virtual ~gen_key_t();


    void add_extra(char *extra_plus, 
		   int  elem_size, 
		   char *in_types, 
		   int point_data);


    // reassign the current value of the key based on the data passed in through
    // DataPtr. dim is the number of dimensions
    void interpret_key(void *DataPtr, // Points to data to interpret
                       int  dim);     // number of dimensions represented

    void interpret_point(void *DataPtr, // Points to data to interpret
			 int  dim);     // number of dimensions represented

    void interpret_key(bit_vec          &bv_ext,          // Points to data to interpret
                       int              dim,              // number of dimensions represented
		       const gen_key_t& frame_of_ref,     // F.O.R. for the page the key is from
		       char             *dim_allocation,  // dimension alloc scheme from a page
		       bool             lossless);        // is compression lossless


    void interpret_point(bit_vec          &bv_ext,          // Points to data to interpret
			 int              dim,              // number of dimensions represented
			 const gen_key_t& frame_of_ref,     // F.O.R. for the page the key is from
			 char             *dim_allocation,  // dimension alloc scheme from a page
			 bool             lossless,
			 int              width_if_lossy);


    int page_repr_size() const;


    // a pointer to an allocated chunk of memory that is the representation of the key
    // to be used on a page.
    void *page_repr() const;


    // a pointer to an allocated chunk of memory that is the representation of the key
    // to be used on a page.
    void *page_repr(gen_key_t frame_of_ref,           // F.O.R. from the page key
		    char      *dim_allocation,        // dimension alloc scheme
		    bool      lossless) const;


    void *point_page_repr(gen_key_t frame_of_ref,           // F.O.R. from the page
			  char      *dim_allocation,        // dimension alloc scheme
			  bool      lossless) const;


    void min(int dim_num,
	     void * &data_ptr,
	     char * &data_type);


    void max(int dim_num,
	     void * &data_ptr,
	     char * &data_type);


    // return the number of dimensions of the key
    int dim() const;

    // the minimum bound on this key on the dim_num dimension.
    int min(int dim_num) const;

    // the maximum bound on this key on the dim_num dimension.
    int max(int dim_num) const;

    // expand the current key to include key.
    void expand_enclose(const gen_key_t& key); // key to envelope

    // calculate the squareness of the current key.
    int squareness() const;

    // calculate the area of the current key.
    long long area() const;

    // calculate the overlap area of the current key and key
    long long overlap_area(const gen_key_t& key) const; // key to check overlap with.

    // returns true iff it overlaps with test_key.
    virtual bool simple_overlaps (const gen_key_t& B) const; // Safe simple
    virtual bool overlaps(const gen_key_t& test_key) const;
    virtual bool overlaps (const gen_key_t& B, dbl_box& Bp, int max_iter, bool more_conservative = false) const;

    virtual void print() const;

    virtual char *gen_key_t::create_value_str() const;

    void print_visualize_info_to_file(FILE *outfile);

    gen_key_t& operator=(const gen_key_t& key_to_copy);

    bool operator==(const gen_key_t& key_to_cmp);

  public:
    int *data;
    int data_sz;
    char *types;
    int num_dim;
};

extern long LinOverlap;
extern long CheckOverlap;
extern long BBoxOverlap;

class LinearConstr : public gen_key_t {

 public:
   LinearConstr ();
   LinearConstr (int dim_num, double **Ina, int Inm, int Inn, int Inm1, int Inm2, int Inm3); 
   LinearConstr (const LinearConstr &L, const char *Bp);
   virtual ~LinearConstr ();
   virtual void print() const;
   virtual void display() const;
   virtual bool overlaps (const gen_key_t& box) const; // Simplex
   virtual bool simple_overlaps (const gen_key_t& B) const; // Safe simple
   // Clipping
   virtual bool overlaps (const gen_key_t& B, dbl_box& Bp, int max_iter, bool more_conservative = false) const;
   
 private:
   double **a;
   int m, n, m1, m2, m3;
   dbl_box BP;
 };


// return the number of dimensions of the key
inline int gen_key_t::dim() const
{
  return num_dim;
}


// the minimum bound on this key on the dim_num dimension.
inline int gen_key_t::min(int dim_num) const
{
//  if (data == NULL)
//    printf("data was NULL in min\n");
//  if (dim_num > num_dim)
//    printf("out of bounds in min\n");
  return (data[(dim_num-1)]);
}

// the maximum bound on this key on the dim_num dimension.
inline int gen_key_t::max(int dim_num) const
{
//  if (data == NULL)
//    printf("data was NULL in max\n");
//  if (dim_num > num_dim)
//    printf("out of bounds in max\n");
  return (data[num_dim+(dim_num-1)]);
}


inline void gen_key_t::interpret_point(void *DataPtr, // Points to data to interpret
				int  dim)      // number of dimensions represented
{
  if (data == NULL)
    data = (int *) malloc(dim*2*sizeof(int));
  memcpy (data, DataPtr, dim*sizeof(int));
  memcpy (data+dim, DataPtr, dim*sizeof(int));
  num_dim = dim;
  if (types == NULL)
    types = (char *)malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
  data_sz = 2*num_dim*sizeof(int);
}


inline void gen_key_t::interpret_point(bit_vec          &bv_ext,          // Points to data to interpret
				int              dim,              // number of dimensions represented
				const gen_key_t& frame_of_ref,     // F.O.R. for the page the key is from
				char             *dim_allocation,  // dimension alloc scheme from a page
				bool             lossless,
				int              width_if_lossy)
{
  int dim_index, dim_size, data_min,
      max_bit_repr;

//  frame_of_ref.print();printf("\n");
//  printf("%d %d %d %d %d %d %d %d\n", dim, dim_allocation[0], dim_allocation[1], 
//	 ((unsigned char *)DataPtr)[0], ((unsigned char *)DataPtr)[1], 
//	 ((unsigned char *)DataPtr)[2], ((unsigned char *)DataPtr)[3], 
//	 ((unsigned char *)DataPtr)[4]);

  if (data == NULL)
    data = (int *) malloc(dim*2*sizeof(int));
  num_dim = dim;
  if (types == NULL)
    types = (char *)malloc(num_dim+1);
  for(int type_place=0; type_place < num_dim; type_place++)
    types[type_place]='i';
  types[num_dim]='\0';
  data_sz = 2*num_dim*sizeof(int);

  for (dim_index=1; dim_index <= dim; dim_index++)
    {
      dim_size=dim_allocation[dim_index-1];
      data_min = bv_ext.extract(dim_size);

      max_bit_repr = (1 << dim_size) - 1;
//      if (dim_size<0)
//	{
//	  printf("dim_size_error in interpret 2 key %d %d\n", dim_size, lossless);
//	  printf("FOR:");frame_of_ref.print();printf("\n");
//	}
//      if (lossless)
//	{
	  data[(dim_index-1)] = data[dim+(dim_index-1)] = 
	    frame_of_ref.min(dim_index) + data_min;
//	}
//      else
//	{
//	  data[(dim_index-1)] = frame_of_ref.min(dim_index) +
//	    ((long long)(frame_of_ref.max(dim_index)-
//			 frame_of_ref.min(dim_index))*data_min) / max_bit_repr;
//	  data[dim+(dim_index-1)] = frame_of_ref.min(dim_index) +
//	    ((long long)(frame_of_ref.max(dim_index)-
//			 frame_of_ref.min(dim_index))*(data_min+width_if_lossy)) / 
//	     max_bit_repr;
//	}
    }
}

#endif






