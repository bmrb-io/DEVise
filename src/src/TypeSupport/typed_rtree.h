#ifndef _TYPED_RTREE_H
#define _TYPED_RTREE_H

#include"rtree.h"
#include"typed_key.h"

struct typed_cursor_t
{
  cursor_t int_cursor;
  int_key_t *int_ret_key;
  typed_key_t *ret_key;
  char *type_info;
  int_key_t *int_query_key;
  typed_key_t *query_key;
};


class typed_rtree_t
{
  public:

    typed_rtree_t(db_mgr_t *my_mgr);

    virtual ~typed_rtree_t();

    virtual rc create(page_id_t  &hdr_pid, 
		      int        num_dim,
		      int        data_sz,
		      bool       point_keys,
		      const char *key_type_info);

    virtual rc open(const page_id_t& hdr_pid);

    virtual rc close();

    virtual rc bulk_load(int        data_file,
			 page_id_t& hdr_pid,
			 const char *key_type_info);

    virtual rc fetch_init(typed_cursor_t    &cursor, 
			  const typed_key_t &key) const;

    // returning a pointer to an innard of my structure. key is invalid when
    // eof is true. I worry about deallocating key after each fetch.
    virtual rc fetch(typed_cursor_t &cursor, 
		     typed_key_t *  &key, 
		     void *         &data, 
		     size_t         &data_sz,
		     bool           &eof) const;

    virtual rc debug(FILE *outfile) const;       /* human readable */

  private:

    rtree_t *my_int_rtree;
};


#endif
