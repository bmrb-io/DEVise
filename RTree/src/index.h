/*
 * index.h - top-level interface to all multidimensional index classes.
 * 
 * 961102  UW-Madison CS  Glass/Goldstein/Prock
 */


#ifndef _INDEX_H
#define _INDEX_H

#include "dbinterface.h"
#include "int_key.h"

class index_specific_cursor {

 public:
  virtual ~index_specific_cursor() {}
};

class cursor_t {

 public:
  index_specific_cursor * p;	/* for index structure-specific garbage */

  cursor_t() { p=NULL; }
  virtual ~cursor_t() {}
};

class mdindex_t {
  
 protected:
  db_mgr_t * mgr;

  /*
   * interface for all multi-dimension index structures.
   *
   * key interface is through gen_key_t.
   */

 public:

  mdindex_t(db_mgr_t * my_mgr) : mgr(my_mgr) {} ;

  virtual rc create(page_id_t& new_root, int nr_dimensions) = 0;
  virtual rc open(const page_id_t& root_pid) = 0;
  virtual rc close() = 0;

  virtual int dim() const = 0;
  virtual rc insert(const int_key_t & key, const void * elem, 
		    size_t elem_size) = 0;
  virtual rc remove(const int_key_t & key) = 0;

  virtual rc fetch_init(cursor_t & cursor, const int_key_t& key, 
			char * key_type_info=NULL) const = 0;

  virtual rc fetch(cursor_t & cursor, int_key_t * & key, void * & elem, 
		   size_t& elem_size, bool& eof) const = 0;

  virtual rc debug(FILE * outfile) const = 0;	/* human readable */
  virtual rc devise_dump(FILE * outfile) const = 0; /* for devise */

  virtual ~mdindex_t() {};
};


#endif





