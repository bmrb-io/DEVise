#include"int_key.h"
#include"stdlib.h"
#include"stdio.h"
#include"strings.h"

int_key_t::int_key_t(int dim_num)
{
  dim = dim_num;
  key_val = (dbword_t *)malloc(sizeof(dbword_t)*2*dim);
}


int_key_t::int_key_t(const int_key_t& key_to_copy)
{
  dim = key_to_copy.dim;
  key_val = (dbword_t *) malloc(dim*2*sizeof(dbword_t));
  memcpy (key_val, key_to_copy.key_val, dim*2*sizeof(dbword_t));
}


int_key_t::int_key_t(dbword_t *bounds, 
		     int      dim_num,
		     bool     point_key)
{
  dim = dim_num;
  key_val = (dbword_t *) malloc(dim_num*2*sizeof(dbword_t));
  if (point_key)
    {
      memcpy (key_val, bounds, dim*sizeof(dbword_t));
      memcpy (key_val+dim, bounds, dim*sizeof(dbword_t));
    }
  else
    memcpy (key_val, bounds, dim*2*sizeof(dbword_t));
}


int_key_t::int_key_t(char *box_str,
		     bool point_key)
{
  char *unmatched = box_str;
  if (sscanf(unmatched, "%d", &dim) == 0)
    assert(0);
  key_val = (dbword_t *) malloc(dim*2*sizeof(dbword_t));

  if (point_key)
    for (int dim_num = 0; dim_num < dim; dim_num++)
      {
	dbword_t val_read;
	unmatched = strstr(unmatched, ".\0")+1;
	if (sscanf(unmatched, DBWORD_FMT, &val_read) == 0)
	  assert(0);
	key_val[dim_num] = key_val[dim_num+dim] = val_read;
      }
  else
    for (int int_num = 0; int_num < 2*dim; int_num++)
      {
	dbword_t val_read;
	unmatched = strstr(unmatched, ".\0")+1;
	if (sscanf(unmatched, DBWORD_FMT, &val_read) == 0)
	  assert(0);
	key_val[int_num] = val_read;
      }
}


int_key_t::~int_key_t()
{
  free(key_val);
}


void int_key_t::print(FILE *outfile) const
{
  int val_num;

  fprintf(outfile, "(");
  for (val_num=0; val_num<dim-1; val_num++)
    fprintf(outfile, DBWORD_FMT ",", key_val[val_num]);
  fprintf(outfile, DBWORD_FMT ")-(", key_val[dim-1]);
  for (val_num=dim; val_num<2*dim-1; val_num++)
    fprintf(outfile, DBWORD_FMT ",", key_val[val_num]);
  fprintf(outfile, DBWORD_FMT ")", key_val[2*dim-1]);
}
