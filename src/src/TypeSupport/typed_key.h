#ifndef _TYPED_KEY_H
#define _TYPED_KEY_H

#include<strings.h>
#include"int_key.h"

class typed_key_t
{
  public:
    typed_key_t() {assert(0);}

    typed_key_t(const typed_key_t &key_to_copy);

    typed_key_t(char *bounds,
		int  in_dim,
		const char *in_types,
		const bool point_data);

    // Converts "integerized" key back to a typed key
    typed_key_t(int_key_t  &key,
		char       *prepended_data,
		const char *in_types);

    ~typed_key_t();

    void min(int dim_num,
	     void * &data_ptr,
	     char * &data_type);

    void max(int dim_num,
	     void * &data_ptr,
	     char * &data_type);

    void debug();

    int dim() const;

    int_key_t *conv_int_key() const;

    bool overlaps(const typed_key_t &key_to_comp);

    char *type_info;

  private:
    int num_dim;
    void **values;
};


inline void typed_key_t::min(int dim_num,
			     void * &data_ptr,
			     char * &data_type)
{
  char *type_info_pl = type_info;
  for(int dim=1; dim<dim_num; dim++)
    {
      switch (type_info_pl[0]) 
	{
	case 'i': case 'f': case'v': case'd':
	  break;
	case 's':
	  type_info_pl = strpbrk(type_info_pl, "]");
	  break;
	default:
	  printf("bad type info in min\n");
	  assert(0);
	}
      type_info_pl++;
    }
  data_ptr = values[dim_num-1];
  data_type = type_info_pl;
}


inline void typed_key_t::max(int dim_num,
			     void * &data_ptr,
			     char * &data_type)
{
  char *type_info_pl = type_info;
  for(int rep=0; rep<2; rep++)
    for(int dim=1; dim<dim_num; dim++)
      {
	switch (type_info_pl[0]) 
	  {
	  case 'i': case 'f': case'v': case'd':
	    break;
	  case 's':
	    type_info_pl = strpbrk(type_info_pl, "]");
	    break;
	  default:
	    printf("bad type info in max\n");
	    assert(0);
	  }
	type_info_pl++;
      }
  data_ptr = (void *) values[dim_num-1+num_dim];
  data_type = type_info_pl;
}

#endif
