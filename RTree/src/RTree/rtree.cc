#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include"rtree.h"


rc rtree_hdr_p_t::init_p(int        num_dim,
			 int        data_size,
			 bool       pt_keys,
			 page_id_t  root_page,
			 const char *in_type_info="")
{
  int rec_size = sizeof(int)+sizeof(int)+sizeof(bool)+sizeof(page_id_t)+
                 sizeof(int)+strlen(in_type_info)+1;
  char buf[rec_size];
  char *buf_place = buf;
  memcpy(buf_place, &num_dim, sizeof(int));
  buf_place += sizeof(int);
  memcpy(buf_place, &data_size, sizeof(int));
  buf_place += sizeof(int);
  int init_levels=1;
  memcpy(buf_place, &init_levels, sizeof(int));
  buf_place += sizeof(int);
  memcpy(buf_place, &pt_keys, sizeof(bool));
  buf_place += sizeof(bool);
  memcpy(buf_place, &root_page.data, page_id_t::PAGE_ID_SIZE);
  buf_place += sizeof(page_id_t);
  memcpy(buf_place, in_type_info, strlen(in_type_info)+1);
  init_st_p();
  JTRY(push_slot(buf, rec_size));
  return RC_OK;
}


int rtree_hdr_p_t::dim()
{
  int temp;
  memcpy((void *)&temp, (void *)slot_ptr(1), sizeof(int));
  return temp;
}


int rtree_hdr_p_t::data_sz()
{
  int temp;
  memcpy((void *)&temp, (void *)(slot_ptr(1)+sizeof(int)), sizeof(int));
  return temp;
}


bool rtree_hdr_p_t::point_keys()
{
  bool temp;
  memcpy((void *)&temp, (void *)(slot_ptr(1)+sizeof(int)+sizeof(int)+sizeof(int)), 
	 sizeof(bool));
  return temp;
}


page_id_t rtree_hdr_p_t::root()
{
  page_id_t temp;
  memcpy((void *)&temp.data, (void *)(slot_ptr(1)+sizeof(int)+sizeof(int)+
				      sizeof(int)+ sizeof(bool)), 
	 page_id_t::PAGE_ID_SIZE);
  return temp;
}


int rtree_hdr_p_t::levels()
{
  int temp;
  memcpy((void *)&temp, (void *)(slot_ptr(1)+sizeof(int)+sizeof(int)), sizeof(int));
  return temp;
}


void rtree_hdr_p_t::add_level()
{
  int temp;
  memcpy((void *)&temp, (void *)(slot_ptr(1)+sizeof(int)+sizeof(int)), sizeof(int));
  temp++;
  memcpy((void *)(slot_ptr(1)+sizeof(int)+sizeof(int)), (&temp), sizeof(int));
}


char *rtree_hdr_p_t::type_info()
{
  int other_space = sizeof(int)+sizeof(int)+sizeof(bool)+sizeof(page_id_t)+
                    sizeof(int);
  char *ret_val = new char[slot_size(1)-other_space];
  memcpy(ret_val, slot_ptr(1)+other_space, slot_size(1)-other_space);
  return ret_val;
}


rtree_t::rtree_t(db_mgr_t *my_mgr) : mdindex_t(my_mgr) 
{
  rtree_hdr = NULL;
}


rtree_t::~rtree_t()
{
  if (rtree_hdr != NULL)
    {
      delete(rtree_hdr->page);
      delete(rtree_hdr);
    }
}


rc rtree_t::create(page_id_t &hdr_pid, 
		   int       num_dim)
{
  JTRY(create(hdr_pid, num_dim, 0, false));
  return RC_OK;
}


rc rtree_t::create(page_id_t  &hdr_pid, 
		   int        num_dim,
		   int        data_sz,
		   bool       point_keys,
		   const char *key_type_info="")
{
  db_page_t *raw_page;
  page_id_t root_pid;

  JTRY(mgr->alloc_page(hdr_pid));
  JTRY(mgr->alloc_page(root_pid));

  JTRY(mgr->pin(hdr_pid, raw_page));
  rtree_hdr_p_t *hdr_p = new rtree_hdr_p_t(raw_page);
  hdr_p->init_p(num_dim, data_sz, point_keys, root_pid, key_type_info);
  delete(hdr_p);
  delete(raw_page);

  int_keyed_p_t *root_p;
  JTRY(get_and_pin_node(root_p, root_pid));
  root_p->init_p(num_dim, data_sz, point_keys);
  free_and_unpin_node(root_p);
  open(hdr_pid);
  return RC_OK;
}


rc rtree_t::open(const page_id_t& hdr_pid)
{
  db_page_t *raw_page;

  JTRY(mgr->pin(hdr_pid, raw_page));
  rtree_hdr = new rtree_hdr_p_t(raw_page);
  return RC_OK;
}


rc rtree_t::close()
{
  assert(rtree_hdr != NULL);
  delete(rtree_hdr->page);
  delete(rtree_hdr);
  return RC_OK;
}


rc rtree_t::attach_root(int        inter_file,
			page_id_t  &hdr_pid,
			int        bulk_elem_sz,
			bool       bulk_point_data,
			int        levels,
			const char *type_info)
{
  char line[160];
  page_id_t root_pid;
  int num_dim, elem_size, point_data, line_size;
  bulk_data_t::get_line(line, inter_file);
  sscanf(line, "%d %d %d", &num_dim, &elem_size, &point_data);
  if (point_data)
    line_size = num_dim*sizeof(dbword_t)+elem_size;  
  else
    line_size = 2*num_dim*sizeof(dbword_t)+elem_size;
  read(inter_file, line, line_size);
  memcpy(root_pid.data, line+(line_size-elem_size), page_id_t::PAGE_ID_SIZE);

  db_page_t *raw_page;
  JTRY(mgr->alloc_page(hdr_pid));
  JTRY(mgr->pin(hdr_pid, raw_page));
  rtree_hdr_p_t *hdr_p = new rtree_hdr_p_t(raw_page);
  hdr_p->init_p(num_dim, bulk_elem_sz, bulk_point_data, root_pid, type_info);
  for (int i=1; i<levels; i++)
    hdr_p->add_level();
  delete(hdr_p);
  delete(raw_page);
  return RC_OK;
}


int rtree_t::elem_per_page(bulk_data_t &bulk_data,
			   int         elem_to_start,
			   int         num_elem,
			   int         page_size,
			   int_key_t   &bbox)
{
  return (page_size-50)/bulk_data.entry_sz;
}


rc rtree_t::sort_dim(bulk_data_t &bulk_data,
		     int         elem_to_start,
		     int         num_elem,
		     int         dim_to_sort,
		     int         outfile)
{
  cut_map_t *cut_map;
  bulk_data.sort_and_cut(elem_to_start, num_elem, dim_to_sort, cut_map,
			 (int (*)(bulk_data_t &, int, int, int, int_key_t &))
			 elem_per_page, mgr->get_pagesize());

  if (dim_to_sort < bulk_data.dim)
    {

//      printf("%d from %d to %d\n", dim_to_sort, elem_to_start, 
//	     elem_to_start+num_elem-1);
//      bulk_data.display();
//      cut_map->display();
//      char ch;printf("hit enter\n"); scanf("%c", &ch);printf("left\n");

      for(int cut_num=0; cut_num < cut_map->num_cuts; cut_num++)
	sort_dim(bulk_data, cut_map->first(cut_num), cut_map->size(cut_num),
		 dim_to_sort+1, outfile);
    }
  else
    JTRY( copy_into_pages(bulk_data, elem_to_start, num_elem, outfile) );
  delete cut_map;
  return RC_OK;
}


rc rtree_t::sort_into_pages(bulk_data_t &bulk_data,
				    int         outfile)
{
  // if using_sort
  sort_dim(bulk_data, 0, bulk_data.num_el, 1, outfile);
  //  bulk_data.btreesort(0, bulk_data.num_el, 1); //Note SortCmp2 is curently broken
  return RC_OK;
}


rc rtree_t::copy_into_pages(bulk_data_t &bulk_data,
			    int         start_idx,
			    int         num_elem,
			    int         outfile)
{
  int_keyed_p_t *page;
  page_id_t new_page_id;
  int elem_num=start_idx;
  dbword_t new_bound[bulk_data.dim*2];

  while (elem_num < start_idx+num_elem)
    {
      int curr_elem;
      JTRY( mgr->alloc_page (new_page_id) );
      JTRY( get_and_pin_node(page, new_page_id) );
      page->init_p(bulk_data.dim, bulk_data.data_sz, bulk_data.pt_data);
      int_key_t page_key((dbword_t *) bulk_data.key(elem_num), bulk_data.dim, 
			 bulk_data.pt_data);
      
      for(curr_elem = elem_num; (curr_elem < elem_num + page->max_num_entries()) &&
	  (curr_elem < start_idx + num_elem); curr_elem++)
	{
	  int_key_t key((dbword_t *) bulk_data.key(curr_elem), bulk_data.dim, 
			bulk_data.pt_data);
	  page_key.expand(key);
	  page->insert_entry(key, bulk_data.data(curr_elem));
	}
      elem_num = curr_elem;

      for(int dim_num = 0; dim_num < bulk_data.dim; dim_num++)
	{
	  new_bound[dim_num]=page_key.min(dim_num+1);
	  new_bound[bulk_data.dim+dim_num]=page_key.max(dim_num+1);
	}
      write(outfile, (void *)new_bound, 2*bulk_data.dim*sizeof(dbword_t));
//      printf("%d.%d - %d.%d    %d\n", new_bound[0], new_bound[1], new_bound[2], 
//	     new_bound[3], new_page_id.pid);
      write(outfile, new_page_id.data, new_page_id.PAGE_ID_SIZE);
      bulk_data.pages_created++;
      free_and_unpin_node(page);
    }
  return RC_OK;
}


rc rtree_t::level_bulkload(int             infile,
			   int             outfile,
			   bool            &made_root)
{
  char line[160];
  bulk_data_t &bulk_data = *(alloc_bulk_data_hldr());

  bulk_data.read_data(infile);
  bulk_data.pages_created=0;
  int size=sprintf(line, "%d %d 0", bulk_data.dim, page_id_t::PAGE_ID_SIZE);
  strncat(line, "          ", (8-((size+1)%8))%8);
  strncat(line, "\n", 1);
  write(outfile, line, size+1+(8-((size+1)%8))%8);

  printf("sorting\n");
  JTRY( sort_into_pages(bulk_data, outfile) );
  printf("done sorting\n");

  //printf("copying\n");
  //JTRY( copy_into_pages(bulk_data, 0, bulk_data.num_el, outfile) );
  //printf("done copying\n");

//  if (level <= 1)
//    printf("%d", bulk_data.pages_created);
  made_root = (bulk_data.pages_created==1);
  delete(&bulk_data);
  return RC_OK;
}


void rtree_t::get_bulk_param(int data_file,
			     int &bulk_num_dim,
			     int &bulk_elem_size,
			     int &bulk_point_data)
{
  char line[160];

  bulk_data_t::get_line(line, data_file);
  sscanf(line, "%d %d %d", &bulk_num_dim, &bulk_elem_size, &bulk_point_data);
  lseek(data_file, 0, SEEK_SET);
}


rc rtree_t::bulk_load(int        data_file,
		      page_id_t& hdr_pid,
		      const char *type_info="")
{
  int inter_file1 = ::open("bulkinter1", O_WRONLY|O_CREAT|O_TRUNC, 0600);
  assert(inter_file1 >= 0);
  int inter_file2, bulk_num_dim, bulk_elem_size, bulk_point_data;
  bool root_in_1, made_root;

  get_bulk_param(data_file, bulk_num_dim, bulk_elem_size, bulk_point_data);
  printf("doing level\n");
  JTRY(level_bulkload(data_file, inter_file1, made_root));
  ::close(inter_file1);

  int levels=1;
  root_in_1 = true;
  while (!made_root)
    {
      printf("doing level2\n");
      levels++;
      inter_file1 = ::open("bulkinter1", O_RDWR, 0600);
      inter_file2 = ::open("bulkinter2", O_WRONLY|O_CREAT|O_TRUNC, 0600);
      JTRY(level_bulkload(inter_file1, inter_file2, made_root))
      ::close(inter_file1);
      ::close(inter_file2);
      root_in_1 = false;
      if (!made_root)
	{
	  printf("doing level3\n");
	  levels++;
	  inter_file1 = ::open("bulkinter1", O_WRONLY|O_CREAT|O_TRUNC, 0600);
	  inter_file2 = ::open("bulkinter2", O_RDWR, 0600);
	  JTRY(level_bulkload(inter_file2, inter_file1, made_root))
          ::close(inter_file1);
          ::close(inter_file2);
	  root_in_1 = true;
	}
    }
  printf("done making levels\n");
  if (root_in_1)
    inter_file1 = ::open("bulkinter1", O_RDONLY, 0600);
  else
    inter_file1 = ::open("bulkinter2", O_RDONLY, 0600);
  JTRY(attach_root(inter_file1, hdr_pid, bulk_elem_size, bulk_point_data, levels, 
		  type_info))
  ::close(inter_file1);
  open(hdr_pid);
  unlink("bulkinter1");
  unlink("bulkinter2");
  return RC_OK;
}


int rtree_t::dim() const
{
  return rtree_hdr->dim();
}


rc rtree_t::insert(const int_key_t &key, 
		   const void      *data,
		         size_t    elem_size)
{
  assert(elem_size == (size_t)rtree_hdr->data_sz());
  bool level_reinserted[rtree_hdr->levels()+1];
  for (int i = 1; i<=rtree_hdr->levels(); i++)
    level_reinserted[i] = false;
  insert_into_tree(key, data, rtree_hdr->levels(), level_reinserted);
  return RC_OK;
}


int best_ins_entry (int_keyed_p_t *page_to_examine,
		    const int_key_t &ins_key)
{
  int_key_t curr_key;
  page_to_examine->get_key(0, curr_key);
  int best_area_enlg = curr_key.area_enlg(ins_key);
  int best_entry = 0;
  for(int i=1; i<page_to_examine->num_entries(); i++)
    {
      page_to_examine->get_key(i, curr_key);
      if (curr_key.area_enlg(ins_key) < best_area_enlg)
	{
	  best_area_enlg = curr_key.area_enlg(ins_key);
	  best_entry = i;
	}
    }
  return best_entry;
}


rc rtree_t::choose_sub_tree(jon_stack_t<int_keyed_p_t *> &path_stack,
			    page_id_t                root_pid,
			    const int_key_t          &ins_key,
			    int                      num_levels)
{
  int_keyed_p_t *next_p;
  JTRY(get_and_pin_node(next_p, root_pid));
  while (num_levels>1)
    {
      path_stack.push(next_p);
      int best_entry = best_ins_entry(next_p, ins_key);
      page_id_t child_pid;
      char data_ptr[next_p->data_sz()];
      next_p->get_data(best_entry, data_ptr);
      memcpy(child_pid.data, data_ptr, child_pid.PAGE_ID_SIZE);
      JTRY(get_and_pin_node(next_p, child_pid));
      num_levels--;
    }
  path_stack.push(next_p);
  return RC_OK;
}


struct dist_elem
{
  double dist;
  int elem_num;
};


int distcomp(const void *elem1, 
	     const void *elem2)
{
  if( ((const dist_elem *)elem1)->dist == ((const dist_elem *)elem2)->dist )
     return 0;
  else if ( ((const dist_elem *)elem1)->dist < ((const dist_elem *)elem2)->dist )
    return -1;
  else
    return 1;
}


void rtree_t::reinsert(int_keyed_p_t   *overflowing_p,
		       int_keyed_p_t   *parent_p,
		       const int_key_t *key,
		       void            *data,
		       int             entry_level,
		       bool            level_reinserted[])
{
  int last_entry = overflowing_p->num_entries();
  dist_elem dist[last_entry+1];
  int_key_t page_box(overflowing_p->bbox());
  page_box.expand(*key);
  int i;
  for(i=0; i<last_entry; i++)
    {
      int_key_t entry_key(overflowing_p->dim());
      overflowing_p->get_key(i, entry_key);
      dist[i].dist = entry_key.center_dist(page_box);
      dist[i].elem_num = i;
    }
  dist[last_entry].dist = key->center_dist(page_box);
  dist[last_entry].elem_num = last_entry;

  qsort(dist, last_entry+1, sizeof(dist_elem), distcomp);

  int num_to_reins = (int) p*overflowing_p->num_entries();
  bool reins_newkey = false;
  int_key_t *reins_ent = new int_key_t[num_to_reins](overflowing_p->dim());
  char reins_data[num_to_reins][overflowing_p->data_sz()];
  for(i = 0; i < num_to_reins; i++)
    {
      if (dist[i].elem_num != last_entry)
	{
	  overflowing_p->get_entry(dist[i].elem_num, reins_ent[i], reins_data[i]);
	  overflowing_p->remove_entry(dist[i].elem_num);
	}
      else
	{
	  reins_ent[i] = *key;
	  memcpy(reins_data[i], data, overflowing_p->data_sz());
	  reins_newkey = true;
	}
    }
  parent_p->fix_key(overflowing_p->bbox(), overflowing_p->page_id().data);

  for(i = 0; i < num_to_reins; i++)
    insert_into_tree(reins_ent[i], reins_data[i], entry_level, level_reinserted);
  delete [] reins_ent;
}


rc rtree_t::split(int_keyed_p_t   *split_p,
		  int_keyed_p_t   *parent_p,
		  const int_key_t *new_key,
		  void            *new_data)
{
  assert(0);
  return RC_OK;
}


rc rtree_t::overflow_treatment(int_keyed_p_t   *overflowing_p,
			       int_keyed_p_t   *parent_p,
			       const int_key_t *key,
			       void            *data,
			       int             entry_level,
			       bool            level_reinserted[])
{
  if (level_reinserted[entry_level])
    JTRY( split(overflowing_p, parent_p, key, data) )
  else
    {
      level_reinserted[entry_level] = true;
      reinsert(overflowing_p, parent_p, key, data, entry_level, level_reinserted);
    }
  return RC_OK;
}


void fix_ancestor_bboxs(jon_stack_t<int_keyed_p_t *> path_stack)
{
  assert(0);
}


rc rtree_t::insert_into_tree(const int_key_t &key, 
			     const void      *data,
			     int             entry_level,
			     bool            level_reinserted[])
{
  jon_stack_t<int_keyed_p_t *> path_stack(max_depth);
  page_id_t root_pid = rtree_hdr->root();

  JTRY( choose_sub_tree(path_stack, root_pid, key, entry_level) )
  const int_key_t *ins_key = &key;
  void      *ins_data = &data;
  while (ins_key != NULL)
    {
      int_keyed_p_t *insertion_p = path_stack.pop();
      if (insertion_p->num_entries() < insertion_p->max_num_entries())
	{
	  insertion_p->insert_entry(key, data);
	  if (entry_level>1)
	    path_stack.top()->fix_key(insertion_p->bbox(), 
				      insertion_p->page_id().data);
	  ins_key = NULL;
	}
      else
	if (entry_level>1)
	  {
	    JTRY( overflow_treatment(insertion_p, path_stack.top(), ins_key, 
				    ins_data, entry_level, level_reinserted) )
	    entry_level--;
	  }
        else // splitting root
	  {
	    page_id_t new_root_pid;
	    JTRY(mgr->alloc_page(new_root_pid))
	    int_keyed_p_t *root_p;
	    JTRY(get_and_pin_node(root_p, new_root_pid));
	    root_p->init_p(key.num_dim(), page_id_t::PAGE_ID_SIZE, false);
	    root_p->insert_entry(insertion_p->bbox(), insertion_p->page_id().data);
	    split(insertion_p, root_p, ins_key, ins_data);
	    path_stack.push(root_p);
	    rtree_hdr->add_level();
	    ins_key = NULL;
	  }
      free_and_unpin_node(insertion_p);
    }
  fix_ancestor_bboxs(path_stack);
  free_unpin_stack(path_stack);
  return RC_OK;
}


rc rtree_t::remove(const int_key_t & key)
{
  assert(0);
  return RC_OK;
}


rc rtree_t::debug_node(FILE      *outfile,
		       page_id_t &node_id,
		       int       level,
		       bool      devise_output=false) const
{
  int_keyed_p_t *next_p;
  JTRY(get_and_pin_node(next_p, node_id));
  next_p->clear_dirty_bit();
  next_p->bbox().print(outfile);
  fprintf(outfile, "\nBEGIN\n");
  for(int i=0; i<next_p->num_entries(); i++)
    {
      if (level==rtree_hdr->levels())
	{
	  int_key_t entry_key(rtree_hdr->dim());
	  next_p->get_key(i, entry_key);
	  entry_key.print(outfile);
	  fprintf(outfile, "\n");
	}
      else
	{
	  page_id_t child_pid;
	  next_p->get_data(i, child_pid.data);
	  debug_node(outfile, child_pid, level+1);
	}
    }
  fprintf(outfile, "END\n");
  free_and_unpin_node(next_p);
  return RC_OK;
}


rc rtree_t::debug(FILE *outfile) const
{
  page_id_t root_pid = rtree_hdr->root();
  debug_node(outfile, root_pid, 1);
  return RC_OK;
}


rc rtree_t::devise_dump(FILE *outfile) const
{
  assert(0);
  return RC_OK;
}


rc rtree_t::get_and_pin_node(      int_keyed_p_t * &node_ptr,
			     const page_id_t       &node_pid) const
{
  db_page_t *raw_page;
  JTRY(mgr->pin(node_pid, raw_page));
  node_ptr = new int_keyed_p_t(*raw_page);
  return RC_OK;
}


void rtree_t::free_and_unpin_node(int_keyed_p_t * &node_ptr) const
{
  delete(node_ptr->db_page());
  delete(node_ptr);
  node_ptr=NULL;
}


void rtree_t::free_unpin_stack(jon_stack_t<int_keyed_p_t *> &path_stack)
{
  while (!path_stack.empty())
    {
      int_keyed_p_t *current = path_stack.pop();
      free_and_unpin_node(current);
    }
}


rc rtree_t::fetch_init(cursor_t         &cursor, 
		       const int_key_t  &key, 
		       char             *key_type_info="") const
{
  rtree_cursor_t *rtree_cursor;
  cursor.p = rtree_cursor = new rtree_cursor_t(key, rtree_hdr->data_sz());
  
  int_keyed_p_t *next_p;
  ikp_cursor_t *cur_ikp_curs = new ikp_cursor_t();
  JTRY(get_and_pin_node(next_p, rtree_hdr->root()));
  next_p->clear_dirty_bit();
  cur_ikp_curs->connect_to_ikp(*next_p);
  rtree_cursor->page_path.push(next_p);
  rtree_cursor->loc_path.push(cur_ikp_curs);
  return RC_OK;
}


rc rtree_t::fetch(cursor_t    &cursor, 
		  int_key_t * &key, 
		  void *      &data, 
		  size_t      &data_sz,
		  bool        &eof) const
{
  int cur_levels = rtree_hdr->levels();
  rtree_cursor_t *rtree_cursor = (rtree_cursor_t *) cursor.p;
  ikp_cursor_t *cur_ikp_curs = rtree_cursor->loc_path.top();
  bool done=false;
  while (!done)
    {
      bool done2=false;
      while (cur_ikp_curs->more_entries() && !done2) // searching page
	if (rtree_cursor->key.overlaps(*cur_ikp_curs->next()))
	  done2 = true;
      if (done2) // found match
	if (cur_levels != rtree_cursor->loc_path.size()) // go down tree
	  {
	    page_id_t child_pid;
	    memcpy(child_pid.data, cur_ikp_curs->data(), child_pid.PAGE_ID_SIZE);
	    int_keyed_p_t *next_p;
	    JTRY(get_and_pin_node(next_p, child_pid));
	    next_p->clear_dirty_bit();
	    cur_ikp_curs = new ikp_cursor_t();
	    cur_ikp_curs->connect_to_ikp(*next_p);
	    rtree_cursor->page_path.push(next_p);
	    rtree_cursor->loc_path.push(cur_ikp_curs);
	    continue;
	  }
        else // found entry to return
	  {
	    key = cur_ikp_curs->key();
	    data = cur_ikp_curs->data();
	    eof = false;
	    data_sz = rtree_cursor->data_sz;
	    done = true;
	    continue;
	  }
      else // didn't find anything, go up the stack to look elsewhere
	{
	  free_and_unpin_node(rtree_cursor->page_path.pop());
	  delete(rtree_cursor->loc_path.pop());
	  if (rtree_cursor->page_path.empty())
	    done=true;
	  else
	    cur_ikp_curs = rtree_cursor->loc_path.top();
	}
    }
  if (rtree_cursor->page_path.empty())
    {
      eof = true;
      key=NULL;
      data_sz=0;
      delete(rtree_cursor);
    }
  return RC_OK;
}


bulk_data_t *rtree_t::alloc_bulk_data_hldr()
{
  return new bulk_data_t;
}
