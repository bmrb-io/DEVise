#include"typed_key.h"
#include"correct_math.h"

#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))

#include <sys/types.h>
#include <netinet/in.h> 

#if defined(__LITTLE_ENDIAN) && !defined(_LITTLE_ENDIAN)
  #define _LITTLE_ENDIAN
#endif
#if defined(__BIG_ENDIAN) && !defined(_BIG_ENDIAN)
  #define _BIG_ENDIAN
#endif

inline long long htonll(long long x)
{
#if defined(_LITTLE_ENDIAN)
  int* p = (int*)&x;
  int t = htonl(p[0]);
  p[0] = htonl(p[1]);
  p[1] = t;
  x = *(long long*)p;
#elif defined(_BIG_ENDIAN)
  // nada
#else
# error "what endian??"
#endif;
  return x;
}


typed_key_t::typed_key_t(const typed_key_t &key_to_copy)
{
  num_dim = key_to_copy.num_dim;
  type_info = new char [strlen(key_to_copy.type_info)+1];
  strcpy(type_info, key_to_copy.type_info);
  values = new (void *) [2*num_dim];
  char *type_info_pl = strpbrk(type_info, "ivfds");
  for(int dim_num=0; dim_num<num_dim; dim_num++)
    {
      switch (type_info_pl[0]) 
	{
	case 'i': case 'f':
	  values[dim_num] = new char [4];
	  values[dim_num+num_dim] = new char [4];
	  memcpy(values[dim_num], key_to_copy.values[dim_num], 4);
	  memcpy(values[dim_num+num_dim], key_to_copy.values[dim_num+num_dim], 4);
	  break;
	case 'v': case 'd':
	  values[dim_num] = new char [8];
	  values[dim_num+num_dim] = new char [8];
	  memcpy(values[dim_num], key_to_copy.values[dim_num], 8);
	  memcpy(values[dim_num+num_dim], key_to_copy.values[dim_num+num_dim], 8);
	  break;
	case 's':
	  int len;
	  sscanf(type_info_pl+2, "%d", &len);
	  type_info_pl = strpbrk(type_info_pl, "]");
	  values[dim_num] = new char [len];
	  values[dim_num+num_dim] = new char [len];
	  memcpy(values[dim_num], key_to_copy.values[dim_num], len);
	  memcpy(values[dim_num+num_dim], key_to_copy.values[dim_num+num_dim], len);
	  break;
	default:
	  printf("passed bad key to constructor 1\n");
	  assert(0);
	  break;
	}
      type_info_pl++;
    }
}

#include <iostream.h>
typed_key_t::typed_key_t(char *bounds,
			 int  in_dim,
			 const char *in_types,
			 const bool point_data)
{
  num_dim = in_dim;
  type_info = new char [strlen(in_types)+1];
  strcpy(type_info, in_types);
  values = new (void *) [2*num_dim];
  int max_reps;
  if (point_data)
    max_reps=1;
  else
    max_reps=2;
  char *place_in_bounds=bounds;
  if (point_data)
    {
      char *type_info_pl = strpbrk(type_info,"ivfds");
      for(int dim_num=0; dim_num<num_dim; dim_num++)
	{
	  switch (type_info_pl[0]) 
	    {
	    case 'i': case 'f':
	      values[dim_num] = new char [4];
	      memcpy(values[dim_num], place_in_bounds, 4);
	      values[dim_num+num_dim] = new char [4];
	      memcpy(values[dim_num+num_dim], place_in_bounds, 4);
	      place_in_bounds += 4;
	      break;
	    case 'v': case 'd':
	      values[dim_num] = new char [8];
	      memcpy(values[dim_num], place_in_bounds, 8);
	      values[dim_num+num_dim] = new char [8];
	      memcpy(values[dim_num+num_dim], place_in_bounds, 8);
	      place_in_bounds += 8;
	      break;
	    case 's':
	      int len;
	      sscanf(type_info_pl+2, "%d", &len);
	      type_info_pl = strpbrk(type_info_pl, "]");
	      values[dim_num] = new char [len];
	      memcpy(values[dim_num], place_in_bounds, len);
	      values[dim_num+num_dim] = new char [len];
	      memcpy(values[dim_num+num_dim], place_in_bounds, len);
	      place_in_bounds += len;
	      break;
	    default:
	      printf("passed bad key to constructor 2\n");
	      assert(0);
	      break;
	    }
	  type_info_pl++;
	}
    }
  else
    for(int rep=0; rep<2; rep++)
      {
	char *type_info_pl = strpbrk(type_info,"ivfds");
	for(int dim_num=0; dim_num<num_dim; dim_num++)
	  {
	    switch (type_info_pl[0]) 
	      {
	      case 'i': case 'f':
		if (rep == 0)
		  {
		    values[dim_num] = new char [4];
		    memcpy(values[dim_num], place_in_bounds, 4);
                    // cerr << "tk: " << dim_num << ' ' << rep << ' ' << "got int: " << *(int*)(values[dim_num]) << endl;
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [4];
		    memcpy(values[dim_num+num_dim], place_in_bounds, 4);
                    // cerr << "tk: " << dim_num << ' ' << rep << ' ' << num_dim << ' ' << "got int: " << *(int*)(values[dim_num+num_dim]) << endl;
		  }
		place_in_bounds += 4;
		break;
	      case 'v': case 'd':
		if (rep == 0)
		  {
		    values[dim_num] = new char [8];
		    memcpy(values[dim_num], place_in_bounds, 8);
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [8];
		    memcpy(values[dim_num+num_dim], place_in_bounds, 8);
		  }
		place_in_bounds += 8;
		break;
	      case 's':
		int len;
		sscanf(type_info_pl+2, "%d", &len);
		type_info_pl = strpbrk(type_info_pl, "]");
		if (rep == 0)
		  {
		    values[dim_num] = new char [len];
		    memcpy(values[dim_num], place_in_bounds, len);
                    // cerr << "tk: " << dim_num << ' ' << rep << ' ' << num_dim << ' ' << len << ' ' << "got string: " << (char*)values[dim_num] << endl;
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [len];
		    memcpy(values[dim_num+num_dim], place_in_bounds, len);
                    // cerr << "tk: " << dim_num << ' ' << rep << ' ' << num_dim << ' ' << len << ' ' << "got string: " << (char*)values[dim_num+num_dim] << endl;
		  }
		place_in_bounds += len;
		break;
	      default:
		printf("passed bad key to constructor 2\n");
		assert(0);
		break;
	      }
	    type_info_pl++;
	  }
      }
}


typed_key_t::typed_key_t(int_key_t  &key,
			 char       *prepended_data,
			 const char *in_types)
{
  num_dim = key.num_dim();
  type_info = new char [strlen(in_types)+1];
  strcpy(type_info, in_types);
  values = new (void *) [2*num_dim];
  char *place_in_pre=prepended_data;
  char *place_in_bounds=(char *)key.key_val;
  for(int rep=0; rep<2; rep++)
    {
      const char *type_info_pl = strpbrk(type_info,"ivfds");
      for(int dim_num=0; dim_num<num_dim; dim_num++)
	{
	  switch (type_info_pl[0])
	    {
	    case 'i':
	      {
		int int_value=(int)(*((long long *)place_in_bounds));
		if (rep == 0)
		  {
		    values[dim_num] = new char [4];
		    memcpy(values[dim_num], &int_value, 4);
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [4];
		    memcpy(values[dim_num+num_dim], &int_value, 4);
		  }
	      }
	      break;
	    case 'v':
	      if (rep == 0)
		{
		  values[dim_num] = new char [8];
		  memcpy(values[dim_num], place_in_bounds, 8);
		}
	      else
		{
		  values[dim_num+num_dim] = new char [8];
		  memcpy(values[dim_num+num_dim], place_in_bounds, 8);
		}
	      break;
	    case 'f':
	      {
		double double_val=*((double *)place_in_bounds);
		flip_bits(double_val);
		float float_val = (float)double_val;
		if (rep == 0)
		  {
		    values[dim_num] = new char [4];
		    memcpy(values[dim_num], &float_val, 4);
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [4];
		    memcpy(values[dim_num+num_dim], &float_val, 4);
		  }
	      }
	      break;
	    case 'd':
	      {
		double double_val=*((double *)place_in_bounds);
		flip_bits(double_val);
		if (rep == 0)
		  {
		    values[dim_num] = new char [8];
		    memcpy(values[dim_num], &double_val, 8);
		  }
		else
		  {
		    values[dim_num+num_dim] = new char [8];
		    memcpy(values[dim_num+num_dim], &double_val, 8);
		  }
	      }
	      break;
	    case 's':
	      {
		int len;
		sscanf(type_info_pl+2, "%d", &len);
		type_info_pl = strpbrk(type_info_pl, "]");
		len = maximum(8, len);
                long long x = htonll(*(long long*)place_in_bounds);
                int p = dim_num + num_dim * rep;

                values[p] = new char [minimum(len,8)];
                memcpy(values[p], &x, 8);
                if( len > 8 ) {
                  int rest = len - 8;
                  memcpy(((char *)(values[p]))+8, place_in_pre, rest);
                  place_in_pre += rest;
                }
	      }
	      break;
	    default:
	      printf("passed bad key to constructor 3\n");
	      assert(0);
	      break;
	    }
	  place_in_bounds+=8;
	  type_info_pl++;
	}
    }
}


typed_key_t::~typed_key_t()
{
  for(int i=0; i<2*num_dim; i++)
    delete [] (values[i]);
  delete [] values;
  delete [] type_info;
}


void typed_key_t::debug()
{
  for(int rep=0; rep<2; rep++)
    {
      char *type_info_pl = strpbrk(type_info,"ivfds");
      for(int dim_num=0; dim_num<num_dim; dim_num++)
	{
	  switch (type_info_pl[0]) 
	    {
	    case 'i': 
	      if (rep==0)
		printf("%d ", *((int *)values[dim_num]));
	      else
		printf("%d ", *((int *)values[dim_num+num_dim]));
	      break;
	    case 'v': 
	      if (rep==0)
		printf("%lld ", *((long long *)values[dim_num]));
	      else
		printf("%lld ", *((long long *)values[dim_num+num_dim]));
	      break;
	    case 'f':
	      if (rep==0)
		printf("%g ", *((float *)values[dim_num]));
	      else
		printf("%g ", *((float *)values[dim_num+num_dim]));
	      break;
	    case 'd':
	      if (rep==0)
		printf("%g ", *((double *)values[dim_num]));
	      else
		printf("%g ", *((double *)values[dim_num+num_dim]));
	      break;
	    case 's':
	      if (rep==0)
		printf("%s ", (char *)values[dim_num]);
	      else
		printf("%s ", (char *)values[dim_num+num_dim]);
	      type_info_pl = strpbrk(type_info_pl, "]");
	      break;
	    default:
	      printf("bad type info in debug() rep=%d dim=%d type=x%x type='%c'\n",
                     rep, dim_num, type_info_pl[0], type_info_pl[0]);
	      assert(0);
	      break;
	    }
	  type_info_pl++;
	}
    }
}


int_key_t *typed_key_t::conv_int_key() const
{
  long long bounds[2*num_dim];
  long long *bounds_pl=bounds;
  for(int rep=0; rep<2; rep++)
    {
      char *type_info_pl = strpbrk(type_info,"ivfds");
      for(int dim_num=0; dim_num<num_dim; dim_num++)
	{
	  switch (type_info_pl[0]) 
	    {
	    case 'i':
	      bounds_pl[0] = *((int *)values[dim_num+num_dim*rep]);
	      break;
	    case 'f':
	      {
		double double_entry_val = *((float *)values[dim_num+num_dim*rep]);
		flip_bits(double_entry_val);
		memcpy((char *)bounds_pl, &double_entry_val, 8);
	      }
	      break;
	    case 'v':
	      bounds_pl[0] = *((long long *)values[dim_num+num_dim*rep]);
	      break;
	    case 'd':
	      {
		double double_entry_val = *((double *)values[dim_num+num_dim*rep]);
		flip_bits(double_entry_val);
		memcpy((char *)bounds_pl, &double_entry_val, 8);
	      }
	      break;
	    case 's':
	      {
		int len;
		sscanf(type_info_pl+2, "%d", &len);
		type_info_pl = strpbrk(type_info_pl, "]");
		len = minimum(8, len);
		bounds_pl[0] = 0;
		memcpy((char *)bounds_pl, values[dim_num+num_dim*rep], len);
                bounds_pl[0] = htonll(bounds_pl[0]);
	      }
	      break;
	    default:
	      fprintf(stderr, "error while converting\n");
	      break;
	    }
	  type_info_pl++;
	  bounds_pl++;
	}
    }
  return new int_key_t(bounds, num_dim, 0);
}


bool typed_key_t::overlaps(const typed_key_t &key_to_comp)
{
  char *type_info_pl = strpbrk(type_info,"ivfds");
  for(int dim_num=0; dim_num<num_dim; dim_num++)
    {
      switch (type_info_pl[0]) 
	{
	case 'i': 
	  if ((*((int *)values[dim_num])) > 
	      (*((int *)key_to_comp.values[dim_num+num_dim])))
	    return false;
	  if ((*((int *)values[dim_num+num_dim])) < 
	      (*((int *)key_to_comp.values[dim_num])))
	    return false;
	  break;
	case 'v': 
	  if ((*((long long *)values[dim_num])) > 
	      (*((long long *)key_to_comp.values[dim_num+num_dim])))
	    return false;
	  if ((*((long long *)values[dim_num+num_dim])) < 
	      (*((long long *)key_to_comp.values[dim_num])))
	    return false;
	  break;
	case 'f':
	  if ((*((float *)values[dim_num])) > 
	      (*((float *)key_to_comp.values[dim_num+num_dim])))
	    return false;
	  if ((*((float *)values[dim_num+num_dim])) < 
	      (*((float *)key_to_comp.values[dim_num])))
	    return false;
	  break;
	case 'd':
	  if ((*((double *)values[dim_num])) > 
	      (*((double *)key_to_comp.values[dim_num+num_dim])))
	    return false;
	  if ((*((double *)values[dim_num+num_dim])) < 
	      (*((double *)key_to_comp.values[dim_num])))
	    return false;
	  break;
	case 's':
	  {
	    int len;
	    sscanf(type_info_pl+2, "%d", &len);
	    type_info_pl = strpbrk(type_info_pl, "]");
            //cerr <<"overlaps s[" << len << "] (" << (char*)values[dim_num]<<","<<(char*)values[dim_num+num_dim]<<") and ("<<(char*)key_to_comp.values[dim_num+num_dim]<<","<<(char*)key_to_comp.values[dim_num]<<")\n";
	    if (memcmp(values[dim_num], key_to_comp.values[dim_num+num_dim], len) 
		> 0)
	      return false;
	    if (memcmp(values[dim_num+num_dim], key_to_comp.values[dim_num], len) 
		< 0)
	      return false;
	  }
	  break;
	default:
	  printf("bad type info in overlaps()\n");
	  assert(0);
	  break;
	}
      type_info_pl++;
    }  
  return true;
}
