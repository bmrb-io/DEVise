/* -*- C++ -*- */
/*
 * stackpage.h - page class providing simple stack (push, pop) access for
 *               variable-length records on an in-memory page.  Uses 
 *               a db_page_t, presumably one returned from a db_mgr_t.
 * 
 * Gideon Glass   970303  <gid@cs.wisc.edu>   UW-Madison CS
 */

#ifndef _STACKPAGE_H
#define _STACKPAGE_H

#include "dbinterface.h"


class stack_page_t {

 protected:
  char * datastart;
  unsigned short * pslotct;	// points into page
  dbword_t * pmagic;	// points into page

  // slot numbering starts at 2
  char * slot_addr (unsigned short slot_no) const { 
    return ((char*) datastart) + ((unsigned short*) datastart)[slot_no];
  }

    
 public:
  db_page_t * page;

  enum open_method {
    ALREADY_EXISTS,
    INITIALIZE_NEW
    };

  // slot numbering starts at 1
  char * slot_ptr (unsigned short slot_no) const { 
    return ((char*) datastart) + ((unsigned short*) datastart)[slot_no+1];
  }

  stack_page_t(db_page_t * thepage, open_method = ALREADY_EXISTS);
  virtual ~stack_page_t();

  void dump();

  void init_st_p();

  rc push_slot(const void * entry, const int len);

  rc pop_slot(void * entry, int & len);

  rc pop_slot();

  // slot numbering starts at 1
  rc overwrite_slot(int entry_number, const void * entry);
              /* rewrites entry #entry -- length of new == length of old */

  const void * slot_val(int entry_number) const;

  int slot_size(int entry_number) const;

  int slots_filled() const;  /* number of slots used */

  db_page_t *db_page() {return page;}

  page_id_t page_id() const 
  {
    return page->get_page_id();
  }

  int space_free() const;

  void clear_dirty_bit() {page->dirty=false;}
};

#endif
