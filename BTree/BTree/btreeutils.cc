#include <stdio.h>
#include "setup.h"
#include "dbinterface.h"
#include "btreeutils.h"
#include "int_key.h"

// The following constant is the maximal number of dimensions displayed 
// by the debugging procedures.
#define MAX_DIM_DISPLAY 4
// The following constant is the maximal number of levels displayed 
// in the debugging methods
#define MAX_LEVEL_DISPLAY 4


//
//
//  Methods for class btree_entry_t
//
//

btree_entry_t::btree_entry_t(const int_key_t &key, const void* elem, 
			     unsigned int elem_size, bool use_max)
{
  dims = key.num_dim();
  entry_size = dims * sizeof(dbword_t) + elem_size;
  entry = new char[entry_size];
  if(elem != NULL)
    memcpy(entry + (dims * sizeof(dbword_t)), elem, elem_size);
  if(use_max)
    for(int i = 0; i < dims; i++)
      *((dbword_t*)entry + i) = key.max(i+1);
  else
    for(int i = 0; i < dims; i++)
      *((dbword_t*)entry + i) = key.min(i+1);
  free_memory = true;
}

btree_entry_t::btree_entry_t(const btree_entry_t &key, const void* elem, 
			     unsigned int elem_size)
{
  dims = key.dims;
  entry_size = dims * sizeof(dbword_t) + elem_size;
  entry = new char[entry_size];
  memcpy(entry, key.entry, dims * sizeof(dbword_t));
  if(elem != NULL)
    memcpy(entry + (dims * sizeof(dbword_t)), elem, elem_size);
  free_memory = true;
}

btree_entry_t::btree_entry_t(const dbword_t* key, int dims,
			     const void* elem, unsigned int elem_size)
{
  btree_entry_t::dims = dims;
  entry_size = dims * sizeof(dbword_t) + elem_size;
  entry = new char[entry_size];
  memcpy(entry, key, dims * sizeof(dbword_t));
  if(elem != NULL)
    memcpy(entry + (dims * sizeof(dbword_t)), elem, elem_size);
  free_memory = true;
}


btree_entry_t::btree_entry_t(void* e, int e_size, int dims, bool copy_space, bool free_memory)
{
  assert(e_size >= (int)(dims * sizeof(dbword_t)));
  btree_entry_t::dims = dims;
  entry_size = e_size;
  if(copy_space)
    {
      entry = new char[e_size];
      memcpy(entry, e, e_size);
      btree_entry_t::free_memory = true;
    }
  else
    {
      entry = (char*)e;
      btree_entry_t::free_memory = free_memory;
    }
}

btree_entry_t::~btree_entry_t()
{
  if(free_memory)
    delete []entry;
}


bool btree_entry_t::operator <=(const btree_entry_t & e) const
{
  assert(dims == e.dims);
  const dbword_t* arg1 = (dbword_t*)entry;
  const dbword_t* arg2 = (dbword_t*)(e.entry);
  for(int i = 0; i < dims; i++)
    if(arg1[i] > arg2[i])
      return false;
    else if(arg1[i] < arg2[i])
      return true;
  return true;
}

bool btree_entry_t::operator ==(const btree_entry_t & e) const
{
  assert(dims == e.dims);
  const dbword_t* arg1 = (dbword_t*)entry;
  const dbword_t* arg2 = (dbword_t*)(e.entry);
  for(int i = 0; i < dims; i++)
    if(arg1[i] != arg2[i])
      return false;
  return true;
}

bool btree_entry_t::equals(const btree_entry_t& e) const
{
  if(dims != e.dims || entry_size != e.entry_size)
    return false;
  return (memcmp(entry, e.entry, entry_size) == 0);
}

bool btree_entry_t::inside(const btree_entry_t& low, const btree_entry_t& high) const
{
  assert(dims == low.dims && dims == high.dims);
  const dbword_t* A = (dbword_t*)entry;
  const dbword_t* L = (dbword_t*)(low.entry);
  const dbword_t* H = (dbword_t*)(high.entry);
  for(int i = 0; i < dims; i++)
    if(A[i] < L[i] || A[i] > H[i])
      return false;
  return true;
}

void btree_entry_t::print(FILE *fd, bool is_internal) // const
{
  assert(((int)entry % sizeof(dbword_t)) == 0);
  dbword_t* k = (dbword_t*)entry;
  for(int i = 0; i < dims && i < MAX_DIM_DISPLAY; i++)
    {
      fprintf(fd,DBWORD_FMT,k[i]);
      fprintf(fd,"\t");
    }
  if(is_internal)
    {
      unsigned char* s = (unsigned char*)(element());
      fprintf(fd,"<");
      int j;
      for(j = 0; j < page_id_t::PAGE_ID_SIZE - 1; j++)
	fprintf(fd,"%d,",(int)(s[j]));
      fprintf(fd,"%d>",(int)(s[j]));    
    }
  else
    {
      for(int j = sizeof(dbword_t) * dims; j < entry_size; j++)
	fprintf(fd,"%c", entry[j]);
    }
}

//
//
// Methods for class btree_page_t 
//
// 

btree_page_t::btree_page_t(db_page_t *thepage, int num_dims, bool is_new_page)
  : aligned_sorted_page_t(thepage, is_new_page ? INITIALIZE_NEW : ALREADY_EXISTS) 
{
  dims = num_dims;
}

btree_page_t::~btree_page_t(){}

int btree_page_t::num_entries() const
{
  return slots_filled();
}

btree_entry_t* btree_page_t::entry(int entry_number, bool make_copy) 
{
  return new btree_entry_t(slot_val(entry_number), slot_size(entry_number), 
			   dims, make_copy, make_copy);  
}
  

bool btree_page_t::should_split(btree_entry_t &entry) const
{
  return (entry.size() > space_free());
}

// Methods for comparison for insertion.
static int comp_entries(const void* a, const void* b, int dims)
{
  const dbword_t* A = (dbword_t*)a;
  const dbword_t* B = (dbword_t*)b;
  for(int i = 0; i < dims; i++)
    if(A[i] < B[i]) return -1;
    else if(A[i] > B[i]) return 1;
  return 0;
}

rc btree_page_t::insert(btree_entry_t &entry)
{
  assert(! should_split(entry));
  return insert_slot(entry.page_repr(), entry.size(), entry.num_dims(),
		     comp_entries);
}

rc btree_page_t::split(btree_entry_t &entry, btree_page_t &new_page)
{
  return split_insert(entry.page_repr(), entry.size(), entry.num_dims(),
		      comp_entries, new_page);
}

rc btree_page_t::remove(int entry_number)
{
  return remove_slot(entry_number);
}


bool btree_page_t::try_to_merge() const
{
  return (space_free() >= space_used());
}


rc btree_page_t::merge(btree_page_t &rpage)
{
  return redistribute(rpage);
} 

int btree_page_t::search(/*const*/ btree_entry_t &entry) // const
{
  // check for empty page
  if(slots_filled() == 0)
    return 0;
  // check for first entry
  if(comp_entries(slot_val(1), entry.page_repr(), dims) > 0)
    return 0;
  // check for single entry
  int high = slots_filled(); 
  if(high == 1)
    return 1;
  // check the last entry
  if(comp_entries(slot_val(high), entry.page_repr(), dims) <= 0)
    return high;
  int low = 1;
  while(high - low > 1)
    {
      int middle = (high + low) / 2;
      if(comp_entries(slot_val(middle), entry.page_repr(), dims) <= 0)
	low = middle;
      else
	high = middle;
    }
  return low;
}


int btree_page_t::exact_search(/*const*/ btree_entry_t &entry)// const
{
  if(slots_filled() == 0)
    return 0;
  int c = comp_entries(slot_val(1), entry.page_repr(), dims);
  if(c == 0) 
    return 1;
  if(c > 0) 
    return 0;
  int low = 1;
  int high = slots_filled();
  if(high == 1)
    return 0;
  if(comp_entries(slot_val(high), entry.page_repr(), dims) < 0)
    return 0;
  while(high - low > 1)
    {
      int middle = (low + high) / 2;
      c = comp_entries(slot_val(middle), entry.page_repr(), dims);
      if(c < 0) 
	low = middle;
      else high = middle;
    }
  if(comp_entries(slot_val(low), entry.page_repr(), dims) == 0)
    return low;
  if(high != low && comp_entries(slot_val(high), entry.page_repr(), dims) == 0)
    return high;
  return 0;
}


void btree_page_t::info(FILE *fd) // const
{
  page_id_t id = page->get_page_id();
  fprintf(fd,"Btree page info <%d",(int)(id.data[0]));
  for(int I = 1; I < page_id_t::PAGE_ID_SIZE; I++)
    fprintf(fd,",%d",(int)(id.data[I]));
  fprintf(fd,">\n");
  fprintf(fd,"----------------------------------------\n");
  fprintf(fd,"height = (leaf page)\n");
  fprintf(fd,"%d entries\n",num_entries());
  fprintf(fd,"%d bytes used\n",space_used());
  fprintf(fd,"%d bytes free for one entry\n",space_free());
  for(int i = 1; i <= slots_filled(); i++)
    {
      fprintf(fd,"slot %d\t:  ",i);
      btree_entry_t* e = entry(i,false);
      e->print(fd,false);
      fprintf(fd,"\n");
      delete e;
    }
  fprintf(fd,"\n");
}

rc btree_page_t::debug(FILE* fd, int height, int max_height, db_mgr_t* mgr)
{
  if(height == max_height)
    {
      info(fd);
      return RC_OK;
    }
  page_id_t id = page->get_page_id();
  fprintf(fd,"Btree page info <%d",(int)(id.data[0]));
  for(int I = 1; I < page_id_t::PAGE_ID_SIZE; I++)
    fprintf(fd,",%d",(int)(id.data[I]));
  fprintf(fd,">\n");
  fprintf(fd,"----------------------------------------\n");
  fprintf(fd,"height = %d\n",height);
  fprintf(fd,"%d entries\n",num_entries());
  fprintf(fd,"%d bytes used\n",space_used());
  fprintf(fd,"%d bytes free for one entry\n",space_free());
  for(int i = 1; i <= slots_filled(); i++)
    {
      fprintf(fd,"slot %d\t:  ",i);
      btree_entry_t* e = entry(i,false);
      e->print(fd,true);
      delete e;
      fprintf(fd,"\n");
    }
  fprintf(fd,"\n");
  if(height < MAX_LEVEL_DISPLAY)
    {
      for(int i = 1; i <= slots_filled(); i++)
	{
	  btree_entry_t* e = entry(i,false);
	  page_id_t* Id = (page_id_t*)(e->element());
	  db_page_t* thepage;
	  JTRY(mgr->pin(*Id, thepage));
	  btree_page_t* p = new btree_page_t(thepage, dims, false);
	  p->debug(fd, height+1, max_height, mgr);
	  delete p;
	  delete e;
	}
    }  
  return RC_OK;
}
