#include "btree.h"
#include "bit_vec.h"

class btree_cursor_t : public index_specific_cursor {
  friend class btree_t;
 public:
  ~btree_cursor_t()
    {
      // path[0] is the root. Do not delete it.
      for(height--; height > 0; height--)
	delete path[height];
      delete []path;
      delete []ids;
      delete low;
      delete high;
      delete []position;
      delete key;
    }
 protected:
  btree_cursor_t(int height, page_id_t& root, btree_page_t* root_page, 
		 const int_key_t& key, const void* elem = NULL, unsigned int elem_size = 0)
    {
      path = new (btree_page_t*)[height];
      path[0] = root_page;
      for(int i = 1; i < height; i++)
	path[i] = NULL;
      btree_cursor_t::height = height;
      ids = new (page_id_t)[height];
      *ids = root;
      low = new btree_entry_t(key,elem,elem_size);
      high = new btree_entry_t(key, NULL, 0, true);
      position = new int[height];
      btree_cursor_t::key = new int_key_t(key);
    }

  void debug(FILE* fd)
    {
      fprintf(fd,"Cursor info\n");
      fprintf(fd,"-----------\n");
      fprintf(fd," height = %d\n", height);
      for(int i = 0; i < height; i++)
	{
	  fprintf(fd," id[%d] = <%d",i,ids[i].data[0]);
	  for(int j = 1; j < page_id_t::PAGE_ID_SIZE; j++)
	    fprintf(fd,",%d",ids[i].data[j]);
	  fprintf(fd,">  position[%d] = %d\n",i,position[i]);
	}
      fprintf(fd,"\n");
    }

  btree_page_t** path;
  page_id_t* ids;
  int* position;
  int height;
  btree_entry_t* low;
  btree_entry_t* high;
  int_key_t* key;
};

//
//
// Implementation of B-tree class. 
//
// Leaf nodes contain <key,element> pairs (sorted).
//
// Internal nodes contain <key,pointer> pairs (sorted by key). 
// The first key in an internal node is always a dummy key (minimal value possible).
// (It is there to hold the pointer to the first entry.)
// For all other <key,pointer> pairs: the key is the minimal key pointed by the pointer.
//
//

#define BTREE_MAGIC_ 0xbeeeeeee

rc btree_t::assert_created(const char* s) const
{
  if(dims != NULL)
    return RC_OK;
  ERROR("btree_t::%s - B-tree already created",s); 
  return RC_ERROR;
}
rc btree_t::assert_not_created(const char* s) const
{
  if(dims == NULL)
    return RC_OK;
  ERROR("btree_t::%s - B-tree not created yet",s);   
  return RC_ERROR;
}
rc btree_t::check_magic(int magic) const
{
  if(magic == (int)(BTREE_MAGIC_))
    return RC_OK;
  ERROR("btree_t: Page is not a B-tree header page");
  return RC_ERROR;
}
rc btree_t::get_page(btree_page_t* from_page, int entry_num, btree_page_t*& result_page, page_id_t& id) const
{
  btree_entry_t* e = from_page->entry(entry_num, false);
  id = *((page_id_t*)(e->element()));
  delete e;
  db_page_t* thepage;
  JTRY(mgr->pin(id, thepage));
  result_page = new btree_page_t(thepage, *dims, false);
  return RC_OK;
}

btree_t::btree_t(db_mgr_t * my_mgr) : mdindex_t(my_mgr)
{
  dims = NULL; // means "not created yet"
}

btree_t::~btree_t()
{
  if(dims != NULL)
    close();
}

rc btree_t::create(page_id_t& header, int num_dims)
{
  JTRY(assert_not_created("create"));
  assert(num_dims > 0);

  // Get the header page
  btree_t::header = header;
  JTRY(mgr->pin(header, hdr_page));
  int* magic = (int*)(hdr_page->get_space_start());
  dims = magic + 1;
  height = dims + 1;
  root = (page_id_t*)(height + 1);

  // Fill the information
  *magic = BTREE_MAGIC_;
  *dims = num_dims;
  JTRY(mgr->alloc_page(*root));
  db_page_t* theroot;
  JTRY(mgr->pin(*root, theroot));
  root_page = new btree_page_t(theroot, *dims, true);
  *height = 1;
  return RC_OK;
}

rc btree_t::open(const page_id_t& header)
{
  JTRY(assert_not_created("open"));

  // Get the header page
  btree_t::header = header;
  JTRY(mgr->pin(btree_t::header, hdr_page));
  int* magic = (int*)(hdr_page->get_space_start());
  dims = magic + 1;
  height = dims + 1;
  root = (page_id_t*)(height + 1);

printf("dims %d\n",*dims);
printf("height %d\n",*height);
printf("root id <%d,%d,%d,%d>\n",(int)(root->data[0]),(int)(root->data[1]),(int)(root->data[2]),(int)(root->data[3]));

  // Fill the information
  JTRY(check_magic(*magic));
printf("Magic number OK\n");
  db_page_t* theroot;
  JTRY(mgr->pin(*root, theroot));
printf("Got root page\n");
  root_page = new btree_page_t(theroot, *dims, false);
printf("Made root a B-tree page\n");
  return RC_OK;
}

rc btree_t::close()
{
  if(dims != NULL)
    {
      delete root_page;
      delete hdr_page;
    }
  dims = NULL;
  return RC_OK;
}

rc btree_t::search(const int_key_t& key, btree_cursor_t* &cursor) const
{
  cursor = new btree_cursor_t(*height, *root, root_page, key);
  for(int h = 1; h < *height; h++)
    {
      cursor->position[h-1] = cursor->path[h-1]->search(*(cursor->low));
      if(cursor->position[h-1] == 0)
	cursor->position[h-1] = 1;
      JTRY(get_page(cursor->path[h-1], cursor->position[h-1], 
		   cursor->path[h], cursor->ids[h]));
    }
  cursor->position[*height - 1] = cursor->path[*height - 1]->search(*(cursor->low));
  return RC_OK;
}

rc btree_t::next_entry(btree_cursor_t* cursor, bool& eof) const
{
  eof = false;
  int h = *height - 1;
  btree_page_t* page = cursor->path[h];
  int* pos = &(cursor->position[h]);
  // Go up the tree
  while(h >= 0)
    {
      if(page->num_entries() > *pos)
	{
	  (*pos)++;
	  break;
	}
      if(h == 0)
	{
	  eof = true;
	  return RC_OK;
	}
      h--;
      page = cursor->path[h];
      pos--;
    }
  // Go down the tree
  for(h++; h < *height; h++)
    {
      delete cursor->path[h];
      JTRY(get_page(page, *pos, cursor->path[h], cursor->ids[h]));
      page = cursor->path[h];
      pos++;
      *pos = 1;
    }
  btree_entry_t* e = page->entry(*pos, false);
  if(! (*e <= *(cursor->high)))
    eof = true;
  delete e;
  return RC_OK;
}

rc btree_t::new_low(btree_cursor_t* cursor, btree_entry_t& entry, int h)
{
  h--;
  int pos = cursor->position[h];
  btree_page_t* page = cursor->path[h];
  btree_entry_t* e = page->entry(pos, false);
  memcpy(e->key(), entry.key(), sizeof(dbword_t) * (*dims));
  delete e;
  if(pos == 1 && h > 0)
    JTRY(new_low(cursor, entry, h));
  return RC_OK;
}

rc btree_t::insert(btree_cursor_t* cursor, btree_entry_t& entry, int h)
{
  if(h > 0)
    {
      btree_entry_t* e = cursor->path[h]->entry(1,false);
      if(entry <= *e)
	JTRY(new_low(cursor, entry, h));
      delete e;
    }
  if(cursor->path[h]->should_split(entry))
    {
      page_id_t new_id;
      JTRY(mgr->alloc_page(new_id));
      db_page_t* thepage;
      JTRY(mgr->pin(new_id, thepage));
      btree_page_t new_page(thepage, *dims, true);
      JTRY(cursor->path[h]->split(entry, new_page));
      btree_entry_t* e = new_page.entry(1, false);
      btree_entry_t new_entry(*e, (void*)(new_id.data), page_id_t::PAGE_ID_SIZE);
      delete e;
      if(h > 0)
	JTRY(insert(cursor, new_entry, h-1))
      else // split the root
	{
	  e = root_page->entry(1, false);	  
	  btree_entry_t low_entry(*e, (void*)(root->data), page_id_t::PAGE_ID_SIZE);
	  delete e;
	  JTRY(mgr->alloc_page(*root));
	  JTRY(mgr->pin(*root, thepage));
	  delete root_page;
	  root_page = new btree_page_t(thepage, *dims, true);
	  JTRY(root_page->insert(low_entry));
	  JTRY(root_page->insert(new_entry));
	  (*height)++;
	}
    }
  else
    JTRY(cursor->path[h]->insert(entry));
  return RC_OK;
}

rc btree_t::insert(const int_key_t& key, const void* elem, unsigned int elem_size)
{
  JTRY(assert_created("insert"));
  btree_cursor_t* cursor;
  JTRY(search(key, cursor));
  btree_entry_t entry(key, elem, elem_size);
  JTRY(insert(cursor, entry, *height - 1));
  delete cursor;
  return RC_OK;
}

// Cursor is deleted only where the remove function finishes work. 
// If remove(cursor, level) is called, it deletes the cursor.
rc btree_t::remove(const int_key_t& key, const void* elem, unsigned int elem_size)
{
  JTRY(assert_created("remove"));
  btree_cursor_t* cursor;
  JTRY(search(key, cursor)); // create the cursor
  btree_entry_t entry(key, elem, elem_size);
  int leaf = *height - 1;
  cursor->position[leaf] = cursor->path[leaf]->exact_search(entry);
  if(cursor->position[leaf] == 0)
    {
      delete cursor;
      return RC_OK;
    }
  // find exact entry to delete
  bool eof = false;
  bool found = false;
  do {
    btree_entry_t* e = cursor->path[leaf]->entry(cursor->position[leaf], false);
    if(e->equals(entry))
      {
	found = true;
	eof = true;
      }
    else
      JTRY(next_entry(cursor,eof));
    delete e;
  } while(! eof);
  if(found)
    JTRY(remove(cursor, leaf))
  else
    delete cursor;
  return RC_OK;
}

rc btree_t::remove(const int_key_t& key)
{
  JTRY(assert_created("remove"));
  btree_entry_t entry(key);
  bool finished = false;
  while(! finished)
    {
      btree_cursor_t* cursor;
      JTRY(search(key, cursor));
      int leaf = *height - 1;
      cursor->position[leaf] = cursor->path[leaf]->exact_search(entry);
      if(cursor->position[leaf] != 0)
	JTRY(remove(cursor, leaf))
      else 
	{
	  finished = true;
	  delete cursor;
	}
    }
  return RC_OK;
}

// This function deletes the cursor (unless it calls itself recursively).
rc btree_t::remove(btree_cursor_t* cursor, int h)
{
  // remove entry at level h
  int pos = cursor->position[h];
  btree_page_t* page = cursor->path[h];
  JTRY(page->remove(pos));

  // Deal with a new low bound.
  if(pos == 1 && h > 0)
    {
      btree_entry_t* e = page->entry(1,false);
      JTRY(new_low(cursor, *e, h));
      delete e;
    }

  // 1. Page doesn't need merging.
  if(! page->try_to_merge())
    {
      delete cursor;
      return RC_OK;
    }
  
  // 2. Entry removed at root (merging not possible - no sibling)
  if(h == 0)
    {
      delete cursor; // Not needed anymore since we do not go further up the tree
  //   a. Height is 1 or there are more than 1 entry on the page
      if(*height == 1 || page->num_entries() > 1)
	return RC_OK;
  //   b. Height is more than 1 and only one pointer on the page.
  //      reduce the height of the tree.
      btree_entry_t* e = page->entry(1,false);
      page_id_t id = *((page_id_t*)(e->element()));
      delete e;
      delete root_page;
      JTRY(mgr->unalloc(*root));
      *root = id;
      (*height)--;
      db_page_t* thepage;
      JTRY(mgr->pin(*root, thepage));
      root_page = new btree_page_t(thepage, *dims, false);
      return RC_OK;
    }

  // 3. Entry removed below root (Redistribute entries)
  //    Start merging.
  //    Prefer to merge with the page on the right. However, if current page 
  //    is the last entry pointed by the parent, merge with the node to the left.
  cursor->path[h] = NULL;
  btree_page_t* parent = cursor->path[h-1];
  btree_page_t* rpage = page;
  if(cursor->position[h-1] == parent->num_entries())
    JTRY(get_page(parent, cursor->position[h-1] - 1, page, cursor->ids[h]))
  else
    {
      (cursor->position[h-1])++;
      JTRY(get_page(parent, cursor->position[h-1], rpage, cursor->ids[h])); 
    }
  // Now: the cursor should point towards to right page entry.
  JTRY(page->merge(*rpage));
  delete page;
  //   a. Merge did not succeed. No need to remove from parent.
  if(rpage->num_entries() > 0)
    {
  //      i. Update low bound and delete cursor.
      btree_entry_t* e = rpage->entry(1,false);
      JTRY(new_low(cursor, *e, h));
      delete e;
      delete cursor;
      delete rpage;
      return RC_OK;
    }
  //   b. Merge succeeds
  //      i. Delete pages
  delete rpage;
  JTRY(mgr->unalloc(cursor->ids[h]));
  //     ii. Recursive call to update parent
  JTRY(remove(cursor, h-1));
  return RC_OK;
}

rc btree_t::fetch_init(cursor_t& cursor, const int_key_t& key, char* key_type_info) const
{
  JTRY(assert_created("fetch_init"));
  btree_cursor_t* C;
  JTRY(search(key, C));
  cursor.p = C;
  return RC_OK;
}

rc btree_t::fetch(cursor_t& cursor, int_key_t*& key, void*& elem, 
		  unsigned int& elem_size, bool& eof) const
{
  JTRY(assert_created("fetch"));
  btree_cursor_t* C = (btree_cursor_t*)(cursor.p);
  bool found = false;
  while(! found)
    {
      JTRY(next_entry(C, eof));
      if(eof)
	{
	  delete C;
	  cursor.p = NULL;
	  return RC_OK;
	}
       btree_entry_t* e = C->path[*height - 1]->entry(C->position[*height - 1], false);
       if(e->inside(*(C->low), *(C->high)))
	 {
	   found = true;
	   key = C->key;
	   key->assign(e->key(), *dims, true);
	   elem = e->element();
	   elem_size = e->elem_size();
	 }
       delete e;
    }
  return RC_OK;
}

rc btree_t::init_range_query(cursor_t& cursor, const int_key_t& key, char* key_type_info)
{
  JTRY(assert_created("init_range_query"));
  btree_cursor_t* C;
  JTRY(search(key, C));
  cursor.p = C;
  return RC_OK;
}

rc btree_t::get_next(cursor_t& cursor, int_key_t*& key, void*& elem, 
		     unsigned int& elem_size, bool& eof)
{
  JTRY(assert_created("get_next"));
  btree_cursor_t* C = (btree_cursor_t*)(cursor.p);
  JTRY(next_entry(C, eof));
  if(eof)
    {
      delete C;
      cursor.p = NULL;
      return RC_OK;
    }
  btree_entry_t* e = C->path[*height - 1]->entry(C->position[*height - 1], false);
  key = C->key;
  key->assign(e->key(), *dims, true);
  elem = e->element();
  elem_size = e->elem_size();
  delete e;
  return RC_OK;
}

int btree_t::dim() const
{
  return *dims;
}

static void print(const page_id_t& id, FILE* fd)
{
  const unsigned char* s = id.data;
  fprintf(fd,"<");
  int i;
  for(i = 0; i < page_id_t::PAGE_ID_SIZE - 1; i++)
    fprintf(fd,"%d,",s[i]);
  fprintf(fd,"%d>",s[i]);
}

rc btree_t::debug(FILE* outfile) const
{
  fprintf(outfile,"B-tree header:\n");
  fprintf(outfile,"--------------\n");
  fprintf(outfile," header id = ");
  print(header,outfile);
  fprintf(outfile,"\n");
  fprintf(outfile," root id   = ");
  print(*root,outfile);
  fprintf(outfile,"\n");
  fprintf(outfile," dims      = %d\n",*dims);
  fprintf(outfile," height    = %d\n",*height);
  fprintf(outfile,"\n");
  root_page->debug(outfile, 1, *height, mgr);
  return RC_OK;
}










