#include<stdlib.h>
#include"int_keyed_p.h"
#include"assert.h"


// ****************************************************************
// ************************* ikp_hdr_t ****************************
// ****************************************************************


void *ikp_hdr_t::page_repr()
{
  void *ret_val=(void *)malloc(page_repr_size());
  void *fep_part=fep_hdr_t::page_repr();
  memcpy(ret_val, fep_part, fep_hdr_t::unaligned_page_repr_size());
  free(fep_part);
  char *ret_val_pl=(char *)ret_val + fep_hdr_t::unaligned_page_repr_size();

  memcpy(ret_val_pl, &dim, sizeof(int2));
  ret_val_pl += sizeof(int2);
  memcpy(ret_val_pl, &point_keys, sizeof(bool));
  ret_val_pl += sizeof(bool);

#ifdef _DEBUG
  memcpy((char *)ret_val+unaligned_page_repr_size(), "                ", 
	 page_repr_size()-unaligned_page_repr_size());
#endif

  return ret_val;
}


int ikp_hdr_t::unaligned_page_repr_size()
{
  return fep_hdr_t::unaligned_page_repr_size()+sizeof(int2)+sizeof(bool);
}


void ikp_hdr_t::interp_hdr_str(const void *hdr_str)
{
  fep_hdr_t::interp_hdr_str(hdr_str);
  const char *temp_str=(const char *)hdr_str + fep_hdr_t::unaligned_page_repr_size();

  memcpy(&dim, temp_str, sizeof(int2));
  temp_str += sizeof(int2);
  memcpy(&point_keys, temp_str, sizeof(bool));
  temp_str += sizeof(bool);
}


// ****************************************************************
// *********************** ikp_cursor_t ***************************
// ****************************************************************


ikp_cursor_t::ikp_cursor_t() : fep_cursor_t()
{
  cur_key = NULL;
}


void ikp_cursor_t::connect_to_ikp(int_keyed_p_t &page)
{
  connect_to_page(page);
  dim = page.dim();
  point_keys = page.point_keys();
  cur_key = new int_key_t(dim);
  place_in_sec -= sec_adv_amt;
}


ikp_cursor_t::~ikp_cursor_t()
{
  if (cur_key != NULL)
    delete cur_key;
}


// ****************************************************************
// *********************** int_keyed_p_t **************************
// ****************************************************************


int_keyed_p_t::int_keyed_p_t(db_page_t &in_page) : fixed_entrysz_p_t(in_page)
{
}


int_keyed_p_t::~int_keyed_p_t()
{
}


void int_keyed_p_t::init_p(int num_dim,
			   int data_size,
			   bool pt_keys)
{
  if (pt_keys)
    fixed_entrysz_p_t::init_p(num_dim*sizeof(dbword_t)*8, data_size);
  else
    fixed_entrysz_p_t::init_p(2*num_dim*sizeof(dbword_t)*8, data_size);
  ikp_hdr_t *hdr = get_ikp_hdr();
  hdr->dim = num_dim;
  hdr->point_keys = pt_keys;
  set_hdr(hdr);
  delete hdr;
}


int int_keyed_p_t::insert_entry(const int_key_t &key,
				const void      *data)
{
  assert(num_entries() < max_num_entries());
  ikp_hdr_t *hdr = get_ikp_hdr();

  int ret_val = hdr->num_entries;
  hdr->num_entries++;
  set_hdr(hdr);
  delete hdr;
  modify_entry(ret_val, key, data);
  return ret_val;
}


void int_keyed_p_t::modify_entry(      int       index_num,
				 const int_key_t &new_key,
				 const void      *new_data)
{
  modify_key(index_num, new_key);
  modify_data(index_num, new_data);
}


void int_keyed_p_t::modify_key(      int       index_num,
			       const int_key_t &new_key)
{
  assert(index_num < num_entries());
  bit_vec_t entry_vec((u_dbword_t *)slot_ptr(2), index_num*first_sz());
  new_key.page_repr(entry_vec, point_keys());
}


void int_keyed_p_t::modify_data(      int  index_num,
				const void *new_data)
{
  sec_modify_entry(index_num, new_data);
}


bool int_keyed_p_t::fix_key(const int_key_t &new_key,
			    void            *data)
{
  ikp_cursor_t cursor;

  cursor.connect_to_ikp(*this);
  int index_num=0;
  bool done = false;
  while (cursor.more_entries() && !done)
    {
      cursor.next();
      if (memcmp((char *)cursor.data(), (char *)data, data_sz()) == 0)
	done = true;
      else
	index_num++;
    }
  if (index_num == num_entries())
    return false;
  modify_key(index_num, new_key);
  return true;
}


int int_keyed_p_t::dim()
{
  ikp_hdr_t *hdr = get_ikp_hdr();
  int ret_val = hdr->dim;
  delete hdr;
  return ret_val;
}


bool int_keyed_p_t::point_keys()
{
  ikp_hdr_t *hdr = get_ikp_hdr();
  bool ret_val = hdr->point_keys;
  delete hdr;
  return ret_val;
}


int int_keyed_p_t::data_sz()
{
  return sec_sz();
}


int_key_t int_keyed_p_t::bbox()
{
  int_key_t ret_val(dim());
  
  assert(num_entries() > 0);
  get_key(0, ret_val);
  for (int i=1; i<num_entries(); i++)
    {
      int_key_t temp_key(dim());
      get_key(i, temp_key);
      ret_val.expand(temp_key);
    }
  return ret_val;
}


fep_hdr_t *int_keyed_p_t::get_fep_hdr()
{
  fep_hdr_t *hdr = get_ikp_hdr();
  return hdr;
}


ikp_hdr_t *int_keyed_p_t::get_ikp_hdr()
{
  ikp_hdr_t *hdr = alloc_ikp_hdr();
  hdr->interp_hdr_str(slot_val(1));
  return hdr;
}


void int_keyed_p_t::get_entry(int       index_num,
			      int_key_t &key,
			      void      *data)
{
  assert(index_num < num_entries());
  get_key(index_num, key);
  get_data(index_num, data);
}


void int_keyed_p_t::get_key(int       index_num,
			    int_key_t &key)
{
  assert(index_num < num_entries());
  bit_vec_t entry_vec((u_dbword_t *)slot_ptr(2), index_num*first_sz());
  key.interpret(entry_vec, point_keys());
}


void int_keyed_p_t::get_data(int  index_num,
			     void *data)
{
  assert(index_num < num_entries());
  sec_get_entry(index_num, data);
}


fep_hdr_t *int_keyed_p_t::alloc_fep_hdr()
{
  fep_hdr_t *hdr = alloc_ikp_hdr();
  return hdr;
}


ikp_hdr_t *int_keyed_p_t::alloc_ikp_hdr()
{
  ikp_hdr_t *hdr = new ikp_hdr_t;
  return hdr;
}