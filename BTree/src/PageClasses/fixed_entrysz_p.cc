#include<stdlib.h>
#include"fixed_entrysz_p.h"


// ****************************************************************
// ************************* fep_hdr_t ****************************
// ****************************************************************


void *fep_hdr_t::page_repr()
{
  void *ret_val=(void *)malloc(page_repr_size());
  char *ret_val_pl=(char *)ret_val;

  memcpy(ret_val_pl, &first_sz, sizeof(int2));
  ret_val_pl += sizeof(int2);
  memcpy(ret_val_pl, &sec_sz, sizeof(int2));
  ret_val_pl += sizeof(int2);
  memcpy(ret_val_pl, &num_entries, sizeof(int4));
  ret_val_pl += sizeof(int4);
  return ret_val;
}


int fep_hdr_t::unaligned_page_repr_size()
{
  return (sizeof(int2)+sizeof(int2)+sizeof(int4));
}


int fep_hdr_t::page_repr_size()
{
  return ((unaligned_page_repr_size()-1)/sizeof(dbword_t) + 1) *
    sizeof(dbword_t);
}


void fep_hdr_t::interp_hdr_str(const void *hdr_str)
{
  memcpy(&first_sz, hdr_str, sizeof(int2));
  (const char *) hdr_str += sizeof(int2);
  memcpy(&sec_sz, hdr_str, sizeof(int2));
  (const char *) hdr_str += sizeof(int2);
  memcpy(&num_entries, hdr_str, sizeof(int4));
  (const char *) hdr_str += sizeof(int4);
}


// ****************************************************************
// *********************** fep_cursor_t ***************************
// ****************************************************************


fep_cursor_t::fep_cursor_t(int first_offset_amount=0,             // in bits
			   int first_outside_absorption_amount=0, // in bits
			   int sec_offset_amount=0,               // in bytes
			   int sec_outside_absorption_amount=0)   // in bytes
{
  first_off = first_offset_amount;
  first_abs = first_outside_absorption_amount;
  sec_off = sec_offset_amount;
  sec_abs = sec_outside_absorption_amount;
}   


fep_cursor_t::~fep_cursor_t()
{
}


void fep_cursor_t::connect_to_page(fixed_entrysz_p_t &fep_page)
{
  fep_hdr_t *hdr = fep_page.get_fep_hdr();
  first_bytes_adv_amt = (hdr->first_sz - first_abs) / 8;
  first_bits_adv_amt = (hdr->first_sz - first_abs) % 8;
  sec_adv_amt = (hdr->sec_sz - sec_abs);
  first_vec.init_vec((u_dbword_t *)fep_page.slot_ptr(2), first_off);
  place_in_sec = ((char *)fep_page.slot_ptr(3))+sec_off;
  first_entries_left = sec_entries_left = hdr->num_entries;
  delete(hdr);
}


// ****************************************************************
// ********************* fixed_entry_sz_p_t ***********************
// ****************************************************************


fixed_entrysz_p_t::fixed_entrysz_p_t(db_page_t &in_page):stack_page_t(&in_page)
{
}


fixed_entrysz_p_t::~fixed_entrysz_p_t()
{
}


void fixed_entrysz_p_t::init_p(int first_size,
			       int sec_size)
{
  int num_words_free;

  // initialize the page
  stack_page_t::init_st_p();

  // create and push the header
  fep_hdr_t *hdr = alloc_fep_hdr();
  hdr->first_sz = first_size;
  hdr->sec_sz = sec_size;
  hdr->num_entries = 0;
  void *hdr_data = hdr->page_repr();
  push_slot(hdr_data, hdr->page_repr_size());
  delete hdr;
  
  // do size calculations
  push_slot(hdr_data, 0);
  push_slot(hdr_data, 0);
  free(hdr_data);
  num_words_free = space_free()/sizeof(dbword_t);
  pop_slot();
  pop_slot();
  int num_first_words_l = (first_size*num_words_free) / (sec_size*8+first_size);
  int max_num_entries_l = num_first_words_l*sizeof(dbword_t)*8 / first_size;
  int num_first_words_h = (first_size*num_words_free-1)/(sec_size*8+first_size)+1;
  int max_num_entries_h = num_first_words_h*sizeof(dbword_t)*8 / first_size;
  int num_first_words;
  if (max_num_entries_l > max_num_entries_h)
    num_first_words = num_first_words_l;
  else
    num_first_words = num_first_words_h;
  
  // now that I finally know the maximum number of entries...
  char dummy_array[num_words_free*sizeof(dbword_t)];

#ifdef _DEBUG
  for(int i=0; i<num_words_free*sizeof(dbword_t); i++)
    dummy_array[i]=' ';
#endif

  push_slot(dummy_array, num_first_words*sizeof(dbword_t));
  push_slot(dummy_array, (num_words_free-num_first_words)*sizeof(dbword_t));
}


int fixed_entrysz_p_t::insert_entry(bit_vec_t &first_str,
				    void      *sec_str)
{
  fep_hdr_t *hdr = get_fep_hdr();

  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), hdr->num_entries*hdr->first_sz);
  first_vec.append(first_str, hdr->first_sz);
  memcpy((char *)slot_ptr(3) + hdr->num_entries*hdr->sec_sz, sec_str, hdr->sec_sz);
  hdr->num_entries++;
  int ret_val=hdr->num_entries-1;
  set_hdr(hdr);
  delete(hdr);
  return ret_val;
}


void fixed_entrysz_p_t::remove_last_entry()
{
  fep_hdr_t *hdr = get_fep_hdr();
  hdr->num_entries--;
  set_hdr(hdr);
  delete(hdr);
}


void fixed_entrysz_p_t::remove_entry(int index_num)
{
  fep_hdr_t *hdr = get_fep_hdr();

  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), index_num * hdr->first_sz);
  first_vec.yank(hdr->first_sz,
		 hdr->first_sz * (hdr->num_entries - index_num));
  memcpy((char *) slot_ptr(2) + index_num * hdr->sec_sz,
	 (char *) slot_ptr(3) + (index_num+1) * hdr->sec_sz,
	 hdr->sec_sz * (hdr->num_entries - index_num - 1));
  hdr->num_entries--;
  set_hdr(hdr);
  delete(hdr);
}


void fixed_entrysz_p_t::modify_entry(      int       index_num,
				           bit_vec_t &new_first,
				     const void      *new_sec)
{
  fep_hdr_t *hdr = get_fep_hdr();

  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), index_num*hdr->first_sz);
  first_vec.append(new_first, hdr->first_sz);
  memcpy((char *)slot_ptr(3) + index_num*hdr->sec_sz, new_sec, hdr->sec_sz);
  delete(hdr);
}


void fixed_entrysz_p_t::first_modify_entry(int       index_num,
					   bit_vec_t &new_first)
{
  fep_hdr_t *hdr = get_fep_hdr();

  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), index_num*hdr->first_sz);
  first_vec.append(new_first, hdr->first_sz);
  delete(hdr);
}


void fixed_entrysz_p_t::sec_modify_entry(      int  index_num,
					 const void *new_sec)
{
  fep_hdr_t *hdr = get_fep_hdr();

  memcpy((char *)slot_ptr(3) + index_num*hdr->sec_sz, new_sec, hdr->sec_sz);
  delete(hdr);
}


int fixed_entrysz_p_t::num_entries()
{
  fep_hdr_t *hdr = get_fep_hdr();

  int ret_val = hdr->num_entries;
  delete(hdr);  
  return ret_val;
}


int fixed_entrysz_p_t::max_num_entries()
{
  fep_hdr_t *hdr = get_fep_hdr();
  int ret_val;

  if (hdr->sec_sz == 0)
    ret_val = slot_size(2)*8/hdr->first_sz;
  else
    if ((slot_size(3)/hdr->sec_sz) <= (slot_size(2)*8/hdr->first_sz))
      ret_val = slot_size(3)/hdr->sec_sz;
    else
      ret_val = slot_size(2)*8/hdr->first_sz;
  delete(hdr);  
  return ret_val;
}


int fixed_entrysz_p_t::first_sz()
{
  fep_hdr_t *hdr = get_fep_hdr();

  int ret_val = hdr->first_sz;
  delete(hdr);  
  return ret_val;
}


int fixed_entrysz_p_t::sec_sz()
{
  fep_hdr_t *hdr = get_fep_hdr();

  int ret_val = hdr->sec_sz;
  delete(hdr);  
  return ret_val;
}


fep_hdr_t *fixed_entrysz_p_t::get_fep_hdr()
{
  fep_hdr_t *hdr = alloc_fep_hdr();
  hdr->interp_hdr_str(slot_val(1));
  return hdr;
}


void fixed_entrysz_p_t::get_entry(int       index_num,
				  bit_vec_t &first,
				  void      *sec)
{
  fep_hdr_t *hdr = get_fep_hdr();
  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), index_num*hdr->first_sz);

  first_vec.extract(first, hdr->first_sz);
  memcpy(sec, (char *)slot_ptr(3) + index_num*hdr->sec_sz, hdr->sec_sz);
  delete(hdr);
}


void fixed_entrysz_p_t::first_get_entry(int       index_num,
					bit_vec_t &first)
{
  fep_hdr_t *hdr = get_fep_hdr();
  bit_vec_t first_vec((u_dbword_t *)slot_ptr(2), index_num*hdr->first_sz);

  first_vec.extract(first, hdr->first_sz);
  delete(hdr);
}


void fixed_entrysz_p_t::sec_get_entry(int       index_num,
				      void      *sec)
{
  assert(sec != NULL);
  fep_hdr_t *hdr = get_fep_hdr();

  memcpy(sec, (char *)slot_ptr(3) + index_num*hdr->sec_sz, hdr->sec_sz);
  delete(hdr);
}


fep_hdr_t *fixed_entrysz_p_t::alloc_fep_hdr()
{
  return new fep_hdr_t;
}


void fixed_entrysz_p_t::set_hdr(fep_hdr_t *new_hdr)
{
  void *hdr_repr = new_hdr->page_repr();
  overwrite_slot(1, hdr_repr);
  free(hdr_repr);
}
