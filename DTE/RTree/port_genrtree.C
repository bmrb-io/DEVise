#ifdef __GNUG__
#pragma implementation "pqueue.h"
#endif

#include "port_genrtree.h"
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include"tools/bitmap.h"
#include"tools/bit_vec.h"
#include<stdio.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include"globals.h"
#include "solaris_stats.h"


#define W_DO(x)           \
{                         \
  rc_t rc;                \
  if ((rc=(x))!=RCOK) {   \
    printf("error at %s:%d '%s'\n", __FILE__, __LINE__, #x); \
    return rc;            \
  }                       \
}


#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))



genrtree_p_hdr::~genrtree_p_hdr()
{
}


char *genrtree_p_hdr::page_repr() const
{
  char *result = (char *) malloc(page_repr_size());
  memcpy(result, (void *) (&root[0]), page_repr_size());
  return result;
}


int genrtree_p_hdr::page_repr_size() const
{
  return ( (((int) (&(elem_size)))+2) - ((int) (&(root[0]))) );
}


void genrtree_p_hdr::interp_hdr_str(char *hdr_str)
{
  memcpy((void *) (&root[0]), hdr_str, page_repr_size());
}


rc_t port_genrtree_p::set_hdr(const genrtree_p_hdr& new_hdr)
{
  char *hdr = new_hdr.page_repr();
  if (slots_filled()==0)
    W_DO( insert_slot((void *) hdr, new_hdr.page_repr_size()) )
  else
    W_DO( overwrite_slot(0, (void *) hdr, new_hdr.page_repr_size()) )
  free (hdr);
  return RCOK;
}


void port_genrtree_p::get_hdr(genrtree_p_hdr &hdr)
{
  hdr.interp_hdr_str((char *)slot_val(0));
}


void port_genrtree_p::read_whole_key (int       entry_num,
				      gen_key_t &key,
				      char      *type_info)
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  key.add_extra((char *)entry_elem_ptr(entry_num), elem_size(), type_info,
		(hdr.level == 0) || (hdr.level == -2));
  delete(&hdr);
}


void port_genrtree_p::read_key(int       entry_num,
			       gen_key_t &key)
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  if (hdr.level != 0)
    key.interpret_key(slot_val(2)+2*dim()*sizeof(int)*(entry_num-1), dim());
  else
    key.interpret_point(slot_val(2)+dim()*sizeof(int)*(entry_num-1), dim());
  delete(&hdr);
}


void port_genrtree_p::get_key_repr(      char      *buf,
				   const gen_key_t &key)
{
  char *key_val = (char *) key.page_repr();
  memcpy(buf, key_val, (key_size()+7)/8);
  free(key_val);
}


genrtree_p_hdr *port_genrtree_p::alloc_hdr()
{
  return (new genrtree_p_hdr);
}


bool port_genrtree_p::is_leaf()
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  bool result = (hdr.level<=1);
  delete(&hdr);
  return (result);
}


int port_genrtree_p::num_page_entries()
{
  return bm_num_set((unsigned char *)slot_val(3), max_entries_per_page());
}


bool port_genrtree_p::is_root ()
{
  page_id_t tree_root;
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  tree_root.interp_pid_str(hdr.root);
  delete(&hdr);
  return (page_addr()==tree_root);
}


int port_genrtree_p::dim ()
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  int result = hdr.num_dim;
  delete(&hdr);
  return (result);
}


int port_genrtree_p::elem_size()
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  int result = hdr.elem_size;
  delete(&hdr);
  return (result);
}


int port_genrtree_p::key_size()
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

  get_hdr(hdr);
  int level = hdr.level;
  delete(&hdr);
  if (level != 0)
    return 2*dim()*sizeof(int)*8;
  else
    return dim()*sizeof(int)*8;
}


void *port_genrtree_p::entry_elem_ptr(int idx)
{
  return(slot_val(1)+(idx-1)*elem_size());
}


int port_genrtree_p::m()
{
  return int(max_entries_per_page()*m_perc);
}


rc_t port_genrtree_p::init_page(genrtree_p_hdr &hdr)
{
  while(slots_filled() > 0)
    {
      printf("removing slot\n");
      W_DO( remove_slot(slots_filled()) )
    }
  W_DO( set_hdr(hdr))
  {
    int elem_arr_sz = elem_size()*max_entries_per_page();
    char elem_arr[elem_arr_sz];
    for(int ArrIndex=0 ; ArrIndex < elem_arr_sz ; ArrIndex++)
      elem_arr[ArrIndex] = 0;
    W_DO( insert_slot(elem_arr, elem_arr_sz) )
  }
  {
    int key_arr_sz = (key_size()*max_entries_per_page()+7)/8;
    char key_arr[key_arr_sz];
    for(int ArrIndex=0 ; ArrIndex < key_arr_sz ; ArrIndex++)
      key_arr[ArrIndex] = 0;
    W_DO( insert_slot(key_arr, key_arr_sz) )
  }
  {
    int bit_arr_sz = (max_entries_per_page()+7)/8;
    char bit_arr[bit_arr_sz];
    for(int ArrIndex=0 ; ArrIndex < bit_arr_sz ; ArrIndex++)
      bit_arr[ArrIndex] = 0;
    W_DO( insert_slot(bit_arr, bit_arr_sz) )
  }
  return RCOK;
}


rc_t port_genrtree_p::find_best_child_for_insert (const gen_key_t& key, 
						        page_id_t& best_pid)
{
  long long   current_badness, candidate_badness, current_area;
  int         current_index;
  gen_key_t   entry_key, expanded_key;
  static char buf[4096];

  {
    read_key(1, entry_key);
    expanded_key = entry_key;
    expanded_key.expand_enclose(key);
    current_area = entry_key.area();
    current_badness = expanded_key.area()-current_area;
    current_index = 1;
    best_pid.interp_pid_str(entry_elem_ptr(1));
  }
  for (int index_num=2; index_num<=num_page_entries(); index_num++)
    {
      read_key(index_num, entry_key);
      expanded_key = entry_key;
      expanded_key.expand_enclose(key);
      candidate_badness = expanded_key.area()-entry_key.area();
      if ((candidate_badness < current_badness) ||
	  ((candidate_badness == current_badness) && (entry_key.area() < current_area)))
	{
	  current_index = index_num;
	  current_badness = candidate_badness;
	  current_area = entry_key.area();
	  best_pid.interp_pid_str(entry_elem_ptr(index_num));
	}
    }
  // Do bookkeeping to keep bboxes in non leaf levels accurate
  read_key(current_index, entry_key);
  entry_key.expand_enclose(key);
  get_key_repr(buf, entry_key);
  W_DO ( overwrite_slot(current_index, buf, (key_size()+7)/8) )
  return RCOK;
}


bool port_genrtree_p::can_insert()
{
  return (num_page_entries() < max_entries_per_page());
}


rc_t port_genrtree_p::insert(const gen_key_t& key,
			     void             *elem,
			     int              elem_sz)
{
  static char buf[4096];
  int  empty_entry;

  if (elem_sz != elem_size())
    return RCERR;
  get_key_repr(buf, key);

  unsigned char *page_dir = (unsigned char *)slot_val(3);
  if ((empty_entry = bm_first_clr(page_dir, max_entries_per_page(), 0)) == -1)
    return RCERR;
  bm_set(page_dir, empty_entry);

  bit_vec entry_vec(slot_val(1), empty_entry*elem_size()*8);
  if (elem_sz != 0)
    entry_vec.append(elem, elem_sz*8);

  bit_vec key_vec(slot_val(2), empty_entry*key_size());
  key_vec.append(buf, key_size());

  return RCOK;
}


rc_t port_genrtree_p::insert (const gen_key_t& key,       
			      const page_id_t& page_id)   
{
  W_DO( insert(key, page_id.page_repr(), page_id.page_repr_size) )
  return RCOK;
}


void swap_entries (sort_entry *sort_array,
		   int a, 
		   int b)
{
  sort_entry c;

  c = sort_array[a];
  sort_array[a] = sort_array[b];
  sort_array[b] = c;
}


// Create the array of entries on a page using only one dimension
void port_genrtree_p::assign_sort_array (      sort_entry *sort_array,
				               int        dim_num,
					 const gen_key_t& key_to_add)
{
  gen_key_t entry_key;

  for (int2 index_num=1; index_num<=num_page_entries(); index_num++)
    {
      read_key(index_num, entry_key);
      sort_array[index_num].loc = index_num;
      sort_array[index_num].min = entry_key.min(dim_num);
      sort_array[index_num].max = entry_key.max(dim_num);
    }
  sort_array[num_page_entries()+1].loc = 0;
  sort_array[num_page_entries()+1].min = key_to_add.min(dim_num);
  sort_array[num_page_entries()+1].max = key_to_add.max(dim_num);
}


void sort_entries (sort_entry *sort_array,
		   int        num_entries)
{
  for (int index_num=1; index_num<=num_entries; index_num++)
    for (int inner_num=num_entries-1; inner_num>=index_num; inner_num--)
      if (sort_array[inner_num].min > sort_array[inner_num+1].min)
	swap_entries (sort_array, inner_num, inner_num+1);
      else if ((sort_array[inner_num].min == sort_array[inner_num+1].min) &&
	       (sort_array[inner_num].max > sort_array[inner_num+1].max))
	swap_entries (sort_array, inner_num, inner_num+1);
}


void port_genrtree_p::generate_bbox_from_sort_array(      int        first_entry,
						          int        last_entry,
						          sort_entry *sort_array, 
						    const gen_key_t& key_to_add, 
						          gen_key_t& bbox)
{
  gen_key_t entry_key;

  if (sort_array[first_entry].loc > 0)
    read_key(sort_array[first_entry].loc, bbox);
  else
    bbox=key_to_add;
  for (int index_num=first_entry+1; index_num<=last_entry; index_num++)
    {
      if (sort_array[index_num].loc > 0)
	{
	  read_key(sort_array[index_num].loc, entry_key);
	  bbox.expand_enclose(entry_key);
	}
      else
	bbox.expand_enclose(key_to_add);
    }
}


long long port_genrtree_p::compute_axis_goodness (      sort_entry *sort_array,
						  const gen_key_t& key_to_add)
{
  gen_key_t bbox1, bbox2;
  long long total_goodness=0;

  for (int k=1; k<=(num_page_entries()+2-2*m()); k++)
    {
      generate_bbox_from_sort_array(1, m()-1+k, sort_array, key_to_add, bbox1);
      generate_bbox_from_sort_array(m()+k, num_page_entries()+1, sort_array, key_to_add, bbox2);
      if (bbox1.area() < bbox2.area())
	total_goodness += bbox1.squareness()+bbox2.squareness(); //+
/*	  (int) (0*((1.0*bbox1.overlap_area(bbox2))/bbox1.area())); */
      else
	total_goodness += bbox1.squareness()+bbox2.squareness(); //+
/*	  (int) (0*((1.0*bbox1.overlap_area(bbox2))/bbox2.area()));*/
    }
  return total_goodness;
}


int port_genrtree_p::choose_split_axis (const gen_key_t& key_to_add)
{
  sort_entry *sort_array = new sort_entry[max_num_entries];
  int current_dim;
  long long candidate_goodness, current_goodness;

  assign_sort_array (sort_array, 1, key_to_add);
  sort_entries (sort_array, num_page_entries()+1);
  current_goodness = compute_axis_goodness (sort_array, key_to_add);
  current_dim = 1;
  for (int dim_num=2; dim_num <= dim(); dim_num++)
    {
      assign_sort_array (sort_array, dim_num, key_to_add);
      sort_entries (sort_array, num_page_entries()+1);
      candidate_goodness = compute_axis_goodness (sort_array, key_to_add);
      if (candidate_goodness < current_goodness)
	{
	  current_dim = dim_num;
	  current_goodness = candidate_goodness;
	}
    }
  delete []sort_array;
  return current_dim;
}


void port_genrtree_p::choose_split_index(      pqueue_t<int>& one_half,
					 const gen_key_t&     key_to_add,
					       int            split_axis)
{
  sort_entry *sort_array = new sort_entry[max_num_entries];
  int num_entries;
  long long current_overlap, current_area;
  int start_of_page_2;

  num_entries = num_page_entries()+1;
  assign_sort_array (sort_array, split_axis, key_to_add);
  sort_entries (sort_array, num_entries);
  {
    gen_key_t bbox1, bbox2;

    generate_bbox_from_sort_array(1, m(), sort_array, key_to_add, bbox1);
    generate_bbox_from_sort_array(m()+1, num_entries, sort_array, key_to_add, bbox2);
    current_overlap = bbox1.overlap_area(bbox2);
    current_area = bbox1.area()+bbox2.area();
    start_of_page_2 = m()+1;
  }
  for (int can_start_of_page2=m()+2; can_start_of_page2<=num_entries-m(); can_start_of_page2++)
    {
      gen_key_t bbox1, bbox2;
      long long candidate_overlap, candidate_area;

      generate_bbox_from_sort_array(1, can_start_of_page2-1, sort_array, key_to_add, bbox1);
      generate_bbox_from_sort_array(can_start_of_page2, num_entries, sort_array, key_to_add, bbox2);
      candidate_overlap = bbox1.overlap_area(bbox2);
      candidate_area = bbox1.area()+bbox2.area();
      if ((candidate_overlap < current_overlap) ||
	  ((candidate_overlap == current_overlap) && (candidate_area < current_area)))
	{
	  start_of_page_2 = can_start_of_page2;
	  current_area = candidate_area;
	  current_overlap = candidate_overlap;
	}
    }
  for (int place_in_array = 1; place_in_array <= start_of_page_2-1; place_in_array++)
    one_half.put(sort_array[place_in_array].loc, sort_array[place_in_array].loc);
  delete []sort_array;
}


rc_t port_genrtree_p::split (const gen_key_t&       key_to_add,  
			           void             *elem_to_add,
			           int              elem_sz,
	   		           port_genrtree_p& new_page)    
{
  int split_axis;
  pqueue_t<int> one_half(max_num_entries, pqueue_max_first);
  bool found_new = false;

  split_axis = choose_split_axis (key_to_add);
  choose_split_index(one_half, key_to_add, split_axis);
  while (!one_half.is_empty())
    {
      int elem_num;

      one_half.get(elem_num);
      if (elem_num == 0)
	{
	  W_DO( new_page.insert(key_to_add, elem_to_add, elem_sz) )
	  found_new = true;
	}
      else
	{
	  W_DO( new_page.insert_slot(slot_val(elem_num), slot_size(elem_num)) )
	  W_DO( remove_slot(elem_num) )
	}
    }
  if (!found_new)
    W_DO( insert (key_to_add, elem_to_add, elem_sz) )
  return RCOK;
}


rc_t port_genrtree_p::split_root (const gen_key_t&  key_to_add,  
				        void        *elem_to_add,
				        int         elem_sz)
{
  int index_num;
  page_id_t temp_pid;
  port_genrtree_p& new_page1 = (*(alloc_page()));
  port_genrtree_p& new_page2 = (*(alloc_page()));

  // Allocate and grab a new page and make it a copy of the current
  W_DO( allocate_and_clear_new_page (temp_pid) )
  W_DO( new_page1.assoc_with_page_id_pin_lock(temp_pid, WRITE_LOCK) )
  for (index_num = 0; index_num<=num_page_entries(); index_num++)
    W_DO( new_page1.insert_slot(slot_val(index_num), slot_size(index_num)) )

  // Allocate and grab and set the header for another page
  W_DO( allocate_and_clear_new_page (temp_pid) )
  W_DO( new_page2.assoc_with_page_id_pin_lock(temp_pid, WRITE_LOCK) )
  W_DO( new_page2.insert_slot(slot_val(0), slot_size(0)) )

  // Do the split
  W_DO( new_page1.split(key_to_add, elem_to_add, elem_sz, new_page2) )

  // Setup new root (which is, of course, this page)
  {
    genrtree_p_hdr &hdr = (*(alloc_hdr()));

    get_hdr(hdr);
    hdr.level++;
    W_DO( set_hdr(hdr) )
    delete(&hdr);
  }
  for(index_num=num_page_entries(); index_num>=1; index_num--)
    W_DO( remove_slot(index_num) )
  W_DO( insert(new_page1.key(), new_page1.page_addr()) )
  W_DO( insert(new_page2.key(), new_page2.page_addr()) )

  new_page2.key().print();
  delete (&new_page1);
  delete (&new_page2);
  return RCOK;
}


rc_t port_genrtree_p::fix_key_for_entry(gen_key_t new_key, 
					page_id_t pid)   
{
  for (int index_num=1; index_num<=num_page_entries(); index_num++)
    {
      if (memcmp(entry_elem_ptr(index_num), pid.page_repr(), pid.page_repr_size)==0)
	{
	  W_DO( remove_slot(index_num) )
	  W_DO( insert(new_key, pid) )
	}
    }
  return RCOK;
}


gen_key_t port_genrtree_p::key()
{
  gen_key_t entry_key, ret_val;
  
  read_key(1, ret_val);
  for (int index_num=2; index_num<=num_page_entries(); index_num++)
    {
      read_key(index_num, entry_key);
      ret_val.expand_enclose(entry_key);
    }
  return ret_val;
}


extern int num_of_clip_iter;


int port_genrtree_p::find_valid_slot_starting_at(      int        start_idx,
						 const gen_key_t& key,
						       dbl_box&   Bp)
{
  gen_key_t entry_key;
  dbl_box old_Bp;

  old_Bp = Bp;
  int page_entries=num_page_entries();
  for (int entry_idx=start_idx; entry_idx <= page_entries; entry_idx++)
    {
      read_key(entry_idx, entry_key);
//      entry_key.print();
      if (is_leaf())
	read_whole_key(entry_idx, entry_key, key.types);
//      if (key.simple_overlaps(entry_key))
      if (key.overlaps(entry_key, Bp, num_of_clip_iter))
	return entry_idx;
      Bp = old_Bp;
    }
  return 0;
}


/********************** port_genrtree_m public methods ************************/


// Initialize the root page of a tree
rc_t port_genrtree_m::create(page_id_t& root, 
			     int2       dim)    
{
  port_genrtree_p& page = (*(alloc_page()));
  genrtree_p_hdr hdr;

  W_DO( allocate_and_clear_new_page (root) )
  W_DO( page.assoc_with_page_id_pin_lock(root, WRITE_LOCK) )
  memcpy(&(hdr.root[0]), root.page_repr(), root.page_repr_size);
  hdr.level = 1;
  hdr.num_dim = dim;
  hdr.elem_size = 4;
  W_DO( page.set_hdr(hdr) )
  delete (&page);
  return RCOK;
}


// Insert an entry
rc_t port_genrtree_m::insert(const page_id_t& root,     
			     const gen_key_t& key,      
			           void       *elem,   
			           int        elem_sz) 
{
  port_genrtree_p *ancestors[16];
  bool            inserted=false;
  gen_key_t       new_key=key;
  page_id_t       new_pid;
  void            *new_elem=elem;
  int             new_elem_sz=elem_sz, num_ances;

  printf("in insert\n");
  W_DO( find_leaf_to_insert_into(key, root, ancestors, num_ances) )
  printf("found leaf\n");
  while (!inserted)
    {
      port_genrtree_p &current_page = (*(ancestors[num_ances-1]));
      if (current_page.can_insert()) // insert the element
	{
	  printf("good\n");
	  W_DO (current_page.insert(new_key, new_elem, new_elem_sz) )
	  printf("out\n");
	  inserted = true;
	}
      else // must perform a split
	{
	  printf("uhoh\n");
	  if (!current_page.is_root()) // splitting non-root
	    {
	      page_id_t temp_pid;
	      port_genrtree_p& new_page=(*(alloc_page()));
	      port_genrtree_p& ancestor_page = (*(ancestors[num_ances-2]));

	      // Allocate, grab, and set the header for a new page
	      W_DO( allocate_and_clear_new_page (temp_pid) )
	      W_DO( new_page.assoc_with_page_id_pin_lock(temp_pid, WRITE_LOCK) )
	      W_DO( new_page.insert_slot(current_page.slot_val(0),
					 current_page.slot_size(0)))

	      W_DO( current_page.split(new_key, new_elem, new_elem_sz, new_page) )

	      // Fix the pointer to current page on it's parent's page
	      W_DO( ancestor_page.fix_key_for_entry(current_page.key(), 
						    current_page.page_addr()) )

	      // Setup variables for next iteration
	      new_key = new_page.key();
	      new_pid = new_page.page_addr();
	      new_elem = new_pid.page_repr();
	      new_elem_sz = new_pid.page_repr_size;

	      delete (&new_page);
	    }
	  else // split root
	    {
	      W_DO( current_page.split_root(new_key, new_elem, new_elem_sz) )
	      inserted = true; // since there is nothing to insert higher in the tree
	    }
	}
      delete (&current_page);
      num_ances--;
    }
  for (int entry_num = num_ances-1; entry_num >= 0; entry_num--)
    delete ancestors[entry_num];
  return RCOK;
  printf("out of insert\n");
}


// Initialize the fetch
rc_t port_genrtree_m::fetch_init(const page_id_t&       root,  
				       gen_rt_cursor_t& cursor)
{
  int stack_index;

  cursor.clear_stack();
  if (cursor.initialized)
    for (stack_index = 0; stack_index < 16; stack_index++)
      delete cursor.ancestors[stack_index];
  for (stack_index = 0; stack_index < 16; stack_index++)
    cursor.ancestors[stack_index] = new gen_rt_cursor_place_holder(*alloc_page());
  cursor.initialized = true;

  W_DO( cursor.add_and_pin_page (root, READ_LOCK) )
  if (cursor.curr_page().num_page_entries() == 0)
    (cursor.curr_next_entry_num()) = 0;
  else
    {
      (cursor.curr_next_entry_num()) = 1;
      cursor.curr_Bp().set_box(cursor.key);
      W_DO( find_next_valid_tuple(cursor) )
    }
  return RCOK;
}


// Find the next entry overlapping a key. Copies the entry value to the el buffer.
rc_t port_genrtree_m::fetch(gen_rt_cursor_t& cursor,
			    gen_key_t&       key,   
			    void*            el,    
			    int&             elen,  
			    bool&            eof) 
{
  eof = false;
  elen = 0;
  if (cursor.curr_next_entry_num() == 0)
    {
      eof = true;
      return RCOK;
    }
  else
    {
      cursor.curr_page().read_key(cursor.curr_next_entry_num(), key);
      cursor.curr_page().read_whole_key(cursor.curr_next_entry_num(), key, 
					cursor.key.types);
      elen = cursor.curr_page().elem_size();
      memcpy(el, 
	     (void *)cursor.curr_page().entry_elem_ptr(cursor.curr_next_entry_num()),
	     elen);
      (cursor.curr_next_entry_num())++;
      W_DO( find_next_valid_tuple(cursor) )
    }
  return RCOK;
}


int color_map[]={2,3,4,5,6,7,10,12,15,17,19,21,24,28,35,41};


rc_t port_genrtree_m::draw(const page_id_t& root,
		                 FILE*      outfile,
			         int        level)
{
  port_genrtree_p& page = (*(alloc_page()));

  W_DO( page.assoc_with_page_id_pin_lock(root, READ_LOCK) )
  int dummy = page.num_page_entries();
  int color=color_map[(rand()%8) + (level%2)*8];
  for(int page_index = 1; page_index <= page.num_page_entries(); page_index++)
    {
      gen_key_t entry_key;

      page.read_key(page_index, entry_key);
      fprintf(outfile, "%d %d %f %f %f %f %d\n", 
	      *((int *)(page.page_addr().page_repr())), level, 
	      (entry_key.max(1)+entry_key.min(1))/2.0, 
	      (entry_key.max(2)+entry_key.min(2))/2.0,
	      1.0*(entry_key.max(1)-entry_key.min(1)),
	      1.0*(entry_key.max(2)-entry_key.min(2)), color);
      if (!page.is_leaf())
	{
	  page_id_t child = root;

	  child.interp_pid_str(page.entry_elem_ptr(page_index));
	  W_DO( draw (child, outfile, level+1) )
	}
    }
  delete (&page);
  return RCOK;
}


rc_t port_genrtree_m::olddraw(const page_id_t& root,
			            FILE*      outfile)
{
  port_genrtree_p& page = (*(alloc_page()));

  W_DO( page.assoc_with_page_id_pin_lock(root, READ_LOCK) )
  page.key().print_visualize_info_to_file(outfile);
  fprintf(outfile, "\nBEGIN\n");
  for(int page_index = 1; page_index <= page.num_page_entries(); page_index++)
    {
      gen_key_t entry_key;

      page.read_key(page_index, entry_key);
      if (!page.is_leaf())
	{
	  page_id_t child = root;

	  child.interp_pid_str(page.entry_elem_ptr(page_index));
	  W_DO( olddraw (child, outfile) )
	}
      else
	{
	  entry_key.print_visualize_info_to_file(outfile);
	  fprintf(outfile, "\n");
	}
    }
  fprintf(outfile, "END\n");
  delete (&page);
  return RCOK;
}


rc_t port_genrtree_m::adjust_root(const page_id_t& new_root,
				  const page_id_t& current_node)
{
  port_genrtree_p& page = (*(alloc_page()));

  W_DO( page.assoc_with_page_id_pin_lock(current_node, READ_LOCK) )
  genrtree_p_hdr& hdr = (*(page.alloc_hdr()));
  page.get_hdr(hdr);
  memcpy(&(hdr.root[0]), new_root.page_repr(), new_root.page_repr_size);
  W_DO( page.set_hdr(hdr) )
  for(int page_index = 1; page_index <= page.num_page_entries(); page_index++)
    if (!page.is_leaf())
      {
	page_id_t child = new_root;
	
	child.interp_pid_str(page.entry_elem_ptr(page_index));
	W_DO( adjust_root (new_root, child) )
      }
  delete (&hdr);
  delete (&page);
  return RCOK;
}


void get_line(char *line, 
	      int  infile)
{
  char read_ret=0;
  for(int place=0; read_ret != '\n'; place++)
    {
      read(infile, &read_ret, 1);
      line[place]=read_ret;
      line[place+1]='\0';
    }
}


int port_genrtree_m::scan_pages()
{
  port_genrtree_p *page_arr[101];
  genrtree_p_hdr hdr;

  for(int i=1; i<=100; i++)
    {
      page_id_t page_id;
      page_id.pid = i;
      page_arr[i] = alloc_page();
      page_arr[i]->assoc_with_page_id_pin_lock(page_id, READ_LOCK);
    }
  gen_key_t entry_key;
  unix_stats my_timer;
  my_timer.start();
  for(int i=1; i<=100; i++)
    {
      int page_entries=page_arr[i]->num_page_entries();
      page_arr[i]->get_hdr(hdr);
      for (int entry_idx=1; entry_idx <= page_entries; entry_idx++)
	if (hdr.level != 0)
	  entry_key.interpret_key(page_arr[i]->slot_val(2)+2*hdr.num_dim*sizeof(int)*(entry_idx-1), hdr.num_dim);
	else
	  entry_key.interpret_point(page_arr[i]->slot_val(2)+hdr.num_dim*sizeof(int)*(entry_idx-1), hdr.num_dim);
      for (int entry_idx=1; entry_idx <= page_entries; entry_idx++)
	if (hdr.level != 0)
	  entry_key.interpret_key(page_arr[i]->slot_val(2)+2*hdr.num_dim*sizeof(int)*(entry_idx-1), hdr.num_dim);
	else
	  entry_key.interpret_point(page_arr[i]->slot_val(2)+hdr.num_dim*sizeof(int)*(entry_idx-1), hdr.num_dim);
    }
  my_timer.stop();
  return my_timer.usertime();
}


rc_t port_genrtree_m::bulk_load(int        data_file, 
				page_id_t  &root_pid,
				int        secondary_index)
{
  int inter_file1 = open("bulkinter1", O_WRONLY|O_CREAT|O_TRUNC, 0600);
  int inter_file2;
  bool root_in_1, made_root;
  int level=2;
  int init_level = 1 - 2*secondary_index;

  W_DO(level_bulkload(data_file, inter_file1, made_root, init_level));
  close(inter_file1);
  
  root_in_1 = true;
  while (!made_root)
    {
      inter_file1 = open("bulkinter1", O_RDWR, 0600);
      inter_file2 = open("bulkinter2", O_WRONLY|O_CREAT|O_TRUNC, 0600);
      W_DO(level_bulkload(inter_file1, inter_file2, made_root, level++))
      close(inter_file1);
      close(inter_file2);
      root_in_1 = false;
      if (!made_root)
	{
	  inter_file1 = open("bulkinter1", O_WRONLY|O_CREAT|O_TRUNC, 0600);
	  inter_file2 = open("bulkinter2", O_RDWR, 0600);
	  W_DO(level_bulkload(inter_file2, inter_file1, made_root, level++))
          close(inter_file1);
          close(inter_file2);
	  root_in_1 = true;
	}
    }

  if (root_in_1)
    inter_file1 = open("bulkinter1", O_RDONLY, 0600);
  else
    inter_file1 = open("bulkinter2", O_RDONLY, 0600);
  char line[160];
  int num_dim, elem_size, point_data, line_size;
  get_line(line, inter_file1);
  sscanf(line, "%d %d %d", &num_dim, &elem_size, &point_data);
  if (point_data)
    line_size = num_dim*sizeof(int)+elem_size;  
  else
    line_size = 2*num_dim*sizeof(int)+elem_size;
  read(inter_file1, line, line_size);
  close(inter_file1);
  root_pid.interp_pid_str((char *) &(line[line_size-elem_size]));
  adjust_root(root_pid, root_pid);
  return RCOK;
}


void port_genrtree_m::read_data(bulk_data_t &bulk_data,
				int         infile,
				int         &level)
{
  char line[160];
  int point_data, num_elem, num_dim, data_size;

  get_line(line, infile);
  sscanf(line, "%d %d %d", &num_dim, &data_size, &point_data);
  if (point_data)
    level--;
  int curr_place = lseek(infile, 0, SEEK_CUR);
  lseek(infile, 0, SEEK_END);
  int last_place = lseek(infile, 0, SEEK_CUR);
  lseek(infile, curr_place, SEEK_SET);
  if (point_data)
    num_elem = (last_place-curr_place)/(num_dim*sizeof(int)+data_size);
  else
    num_elem = (last_place-curr_place)/(2*num_dim*sizeof(int)+data_size);
  lseek(infile, 0, SEEK_SET);
  bulk_data.init(infile, curr_place, num_dim, (point_data==1), num_elem, 
		 data_size);
}


rc_t port_genrtree_m::sort_into_pages(bulk_data_t &bulk_data,
				      int         outfile,
				      int         level)
{
  if (using_sort)
    sort_dim(bulk_data, 0, bulk_data.num_el, 1, outfile, level);
//  bulk_data.btreesort(0, bulk_data.num_el, 1);
  return RCOK;
}


rc_t port_genrtree_m::sort_dim(bulk_data_t &bulk_data,
			       int         elem_to_start,
			       int         num_elem,
			       int         dim_to_sort,
			       int         outfile,
			       int         level)
{
  bulk_data.sort(elem_to_start, num_elem, dim_to_sort);

  if (dim_to_sort < bulk_data.dim)
    {
      int num_pages = (num_elem+bulk_data.elem_per_page(elem_to_start, num_elem)-1)/
	              bulk_data.elem_per_page(elem_to_start, num_elem);
      cut_map_t cut_map((int) ( pow(num_pages, 
				    1.0/(bulk_data.dim+1-dim_to_sort))+0.5 ));
      bulk_data.calc_cuts(cut_map, elem_to_start, num_elem, dim_to_sort);

//      printf("%d from %d to %d\n", dim_to_sort, elem_to_start, 
//	     elem_to_start+num_elem-1);
//      bulk_data.display();
//      cut_map.display();
//      char ch;printf("hit enter\n"); scanf("%c", &ch);printf("left\n");

      for(int cut_num=0; cut_num < cut_map.num_cuts; cut_num++)
	sort_dim(bulk_data, cut_map.first(cut_num), cut_map.size(cut_num),
		 dim_to_sort+1, outfile, level);
    }
//  else
//    W_DO( copy_into_pages(bulk_data, elem_to_start, num_elem, outfile, level) );
  return RCOK;
}


rc_t port_genrtree_m::copy_into_pages(bulk_data_t &bulk_data,
				      int         start_idx,
				      int         num_elem,
				      int         outfile, 
				      int         level)
{
  if (level<0)
    level += 2;

  port_genrtree_p& page = (*(alloc_page()));
  page_id_t new_page_id;
  genrtree_p_hdr hdr;
  int elem_num=start_idx;
  int new_bound[bulk_data.dim*2];

  hdr.level = level;
  hdr.num_dim = bulk_data.dim;
  hdr.elem_size = bulk_data.data_sz;
  
  while (elem_num < start_idx+num_elem)
    {
      int curr_elem;
      W_DO( allocate_and_clear_new_page (new_page_id) )
      W_DO( page.assoc_with_page_id_pin_lock(new_page_id, WRITE_LOCK) )
      W_DO( page.init_page(hdr) )
      for(curr_elem = elem_num;
	  (curr_elem < elem_num + 
	              max_entries_on_page(hdr.num_dim, hdr.elem_size, level)) &&
	  (curr_elem < start_idx + num_elem); 
	  curr_elem++)
	{
	  gen_key_t key((int *) bulk_data.key(curr_elem), bulk_data.dim, 
			bulk_data.pt_data);
	  W_DO(page.insert(key, bulk_data.data(curr_elem), bulk_data.data_sz));
	}
      elem_num = curr_elem;

      gen_key_t page_key = page.key();
      for(int dim = 0; dim < hdr.num_dim; dim++)
	{
	  new_bound[dim]=page_key.min(dim+1);
	  new_bound[hdr.num_dim+dim]=page_key.max(dim+1);
	}
      write(outfile, (void *)new_bound, 2*hdr.num_dim*sizeof(int));
//      printf("%d.%d - %d.%d    %d\n", new_bound[0], new_bound[1], new_bound[2], 
//	     new_bound[3], new_page_id.pid);
      write(outfile, new_page_id.page_repr(), new_page_id.page_repr_size);
      bulk_data.pages_created++;
    }
  delete(&page);
  return RCOK;
}


rc_t port_genrtree_m::level_bulkload(int             infile,
				     int             outfile,
				     bool            &made_root,
				     int             level)
{
  char line[160];
  bulk_data_t &bulk_data = *(alloc_bulk_data_hldr());

  read_data(bulk_data, infile, level);
  bulk_data.pages_created=0;
  int size=sprintf(line, "%d %d 0", bulk_data.dim, page_id_t::page_repr_size);
  strncat(line, "          ", (8-((size+1)%8))%8);
  strncat(line, "\n", 1);
  write(outfile, line, size+1+(8-((size+1)%8))%8);

  W_DO( sort_into_pages(bulk_data, outfile, level) );

  W_DO( copy_into_pages(bulk_data, 0, bulk_data.num_el, outfile, level) );

//  if (level <= 1)
//    printf("%d", bulk_data.pages_created);
  made_root = (bulk_data.pages_created==1);
  delete(&bulk_data);
  return RCOK;
}


bulk_data_t *port_genrtree_m::alloc_bulk_data_hldr()
{
  return (new bulk_data_t);
}


/********************** port_genrtree_m private methods *************************/


// Sets up child and the ancestor stack deriving from a DFS search to get
// to leaf using key as the key.
rc_t port_genrtree_m::find_leaf_to_insert_into(const gen_key_t&      key,
					       const page_id_t&      root,
					             port_genrtree_p *ancestors[16],
					             int             &stack_size)
{
  bool      done = false;
  page_id_t descend_pid = root;

  stack_size = 0;
  while (!done)
    {
      ancestors[stack_size] = alloc_page();
      W_DO( 
	 ancestors[stack_size]->assoc_with_page_id_pin_lock(descend_pid, WRITE_LOCK) 
	   )
      stack_size++;
      if (ancestors[stack_size-1]->is_leaf())
	done = true;
      else
	  W_DO( ancestors[stack_size-1]->find_best_child_for_insert(key, descend_pid) )
    }
  return RCOK;
}


// Finds the next tuple in the rtree specified in the cursor
// which satisfies the query contained in the cursor. If there is no next
// tuple, cursor.page is the root and cursor.next_entry_num is 0
rc_t port_genrtree_m::find_next_valid_tuple(gen_rt_cursor_t& cursor)
{
  int slot_num;
  dbl_box new_Bp;

  while (1)
    {
      new_Bp = cursor.curr_Bp();
      slot_num = 
	cursor.curr_page().find_valid_slot_starting_at(cursor.curr_next_entry_num(), 
						       cursor.key, new_Bp);
      if ((slot_num == 0) && (cursor.list_sz == 1)) // done search
	{
          (cursor.curr_next_entry_num()) = 0;
          return RCOK;
        }

      if ((slot_num == 0) && 
	  (cursor.list_sz > 1)) // page is empty, move up the tree
        {
	  cursor.del_top();
	  continue;
	}

      if (slot_num != 0) // found an entry on the page
	{
          if (cursor.curr_page().is_leaf()) // found the next valid entry
	    {
	      (cursor.curr_next_entry_num()) = slot_num;
	      return RCOK;
	    }
	  else // move down the tree
	    {
	      page_id_t child_pid;

	      genrtree_p_hdr &hdr = (*(cursor.curr_page().alloc_hdr()));
	      cursor.curr_page().get_hdr(hdr);
	      int level=hdr.level;
	      delete(&hdr);

//	      if (level==2)
//		{
//		  page_io++;
//		  child_page_io++;
//		  (cursor.curr_next_entry_num()) = slot_num+1;
//		  continue;
//		}
	      (cursor.curr_next_entry_num()) = slot_num+1;
	      child_pid.interp_pid_str(cursor.curr_page().entry_elem_ptr(slot_num));
	      W_DO( cursor.add_and_pin_page(child_pid, READ_LOCK) )
	      (cursor.curr_Bp()) = new_Bp;
	      (cursor.curr_next_entry_num()) = 1;
	      continue;
	    }
      }
      return RCOK;
    }
  return RCOK;
}


rc_t gen_rt_cursor_t::add_and_pin_page (const page_id_t& page,  
					      lock_t     lock) 
{ 
  W_DO( ancestors[list_sz]->page.assoc_with_page_id_pin_lock(page, lock) )
  list_sz++;
  return RCOK;
}

int port_genrtree_m::page_io;
int port_genrtree_m::child_page_io;
template class pqueue_t<int>;
