/* -*- C++ -*- */
/*
 * B-tree.
 *
 * Uri Shaft   970919  <uri@cs.wisc.edu>   UW-Madison CS
 *
 */

#ifndef BTREE_H__
#define BTREE_H__

#include <stdio.h>
#include "setup.h"
#include "index.h"
#include "dbinterface.h"
#include "alignedsortedpage.h"
#include "int_key.h"
#include "btreeutils.h"

class btree_cursor_t;

class btree_t : public mdindex_t {
 public:
  btree_t(db_mgr_t * my_mgr);
  ~btree_t();
  
  virtual rc create(page_id_t& header, int num_dims);
  virtual rc open(const page_id_t& header);
  virtual rc close();

  virtual int dim() const;

  virtual rc insert(const int_key_t& key, const void* elem, unsigned int elem_size);
  // remove all entries with specific key (allow duplicate keys).
  virtual rc remove(const int_key_t& key); 
  // remove one specific entry that matches the <key,element> pair.
  // Assumption: there are no duplicate pairs of <key,element>. If assumption doesn't hold
  //  then only one of the duplicates is removed.
  virtual rc remove(const int_key_t& key, const void* elem, unsigned int elem_size);

  // Multidimensional box query. Returned entries should be inside the box "key"
  virtual rc fetch_init(cursor_t& cursor, const int_key_t& key, char* key_type_info = NULL) const;
  virtual rc fetch(cursor_t& cursor, int_key_t*& key, void*& elem, 
		   unsigned int& elem_size, bool& eof) const;

  // Normal one dimensional multiple attribute range query. 
  virtual rc init_range_query(cursor_t& cursor, const int_key_t& key,
			      char* key_type_info = NULL);
  virtual rc get_next(cursor_t& cursor, int_key_t*& key, void*& elem, 
		      unsigned int& elem_size, bool& eof);

  virtual rc debug(FILE * outfile) const;	/* human readable */
  virtual rc devise_dump(FILE * outfile) const  /* for devise , not implemented */
    {return RC_ERROR;}

 protected:
  page_id_t header;
  page_id_t* root;
  int* dims;
  int* height;

  db_page_t* hdr_page;
  btree_page_t* root_page;

 private:
  // Checks and assertions
  rc assert_created(const char* s) const;
  rc assert_not_created(const char* s) const;
  rc check_magic(int magic) const;

  // Follow a pointer
  rc get_page(btree_page_t* from_page, int entry_num, btree_page_t*& result_page, page_id_t& id) const; 

  // search initializes the cursor
  rc search(const int_key_t& key, btree_cursor_t* &cursor) const;
  // next_entry moves the cursor forward by one element.
  rc next_entry(btree_cursor_t* cursor, bool& eof) const;

  rc insert(btree_cursor_t* cursor, btree_entry_t& entry, int h);
  rc new_low(btree_cursor_t* cursor,  btree_entry_t& entry, int h);
  rc remove(btree_cursor_t* cursor, int h);
};

#endif







