#ifndef _RTREE_H
#define _RTREE_H

#include "index.h"
#include "int_keyed_p.h"
#include "stackpage.h"
#include "stack_t.h"
#include "bulk_data.h"

const int max_depth = 10;

class rtree_t;

class rtree_cursor_t:index_specific_cursor
{
  friend rtree_t;
  public:
    rtree_cursor_t(const int_key_t &in_key,
		         int       in_sz):page_path(max_depth),loc_path(max_depth),
                                          eof(false),key(in_key), data_sz(in_sz){}
  private:
    jon_stack_t<int_keyed_p_t *> page_path;
    jon_stack_t<ikp_cursor_t *> loc_path;
    bool eof;
    const int_key_t &key;
    int data_sz;
};


class rtree_hdr_p_t : public stack_page_t
{
  public:
    // Inherited:
    //   page_id_t *page (need this for unpinning)

    rtree_hdr_p_t(db_page_t *in_page):stack_page_t(in_page) {}

    virtual ~rtree_hdr_p_t() {}

    virtual rc init_p(int        num_dim,
		      int        data_size,
		      bool       pt_keys,
		      page_id_t  root_page,
		      const char *in_type_info);

    virtual int dim();

    virtual int data_sz();

    virtual bool point_keys();
    
    virtual char *type_info();

    virtual page_id_t root();
  
    virtual int levels();

    virtual void add_level();
};


class rtree_t : mdindex_t
{
  public:

    rtree_t(db_mgr_t *my_mgr);

    virtual ~rtree_t();

    virtual rc create(page_id_t &hdr_pid, 
		      int       num_dim);

    virtual rc create(page_id_t  &hdr_pid, 
		      int        num_dim,
		      int        data_sz,
		      bool       point_keys,
		      const char *key_type_info="");

    virtual rc open(const page_id_t& hdr_pid);

    virtual rc close();

    virtual rc bulk_load(int        data_file,
			 page_id_t& hdr_pid,
			 const char *key_type_info="");

    virtual int dim() const;

    virtual rc insert(const int_key_t &key, 
		      const void      *data,
		            size_t    elem_size);

    virtual rc remove(const int_key_t & key);

    // key must exist for the duration of the query.
    virtual rc fetch_init(cursor_t         &cursor, 
			  const int_key_t  &key, 
			  char             *key_type_info="") const;

    virtual rc fetch(cursor_t     &cursor, 
		     int_key_t *  &key, 
		     void *       &data, 
		     size_t       &data_sz,
		     bool         &eof) const;

    virtual rc debug(FILE *outfile) const;       /* human readable */

    virtual rc devise_dump(FILE *outfile) const; /* for devise */

  protected:
    virtual rc split(int_keyed_p_t   *split_p,
		     int_keyed_p_t   *parent_p,
		     const int_key_t *new_key,
		     void            *new_data);

    virtual rc insert_into_tree(const int_key_t &key, 
				const void      *data,
				int             entry_level,
				bool            level_reinserted[]);

    virtual rc overflow_treatment(int_keyed_p_t   *overflowing_p,
				  int_keyed_p_t   *parent_p,
				  const int_key_t *key,
				  void            *data,
				  int             entry_level,
				  bool            level_reinserted[]);

    virtual void reinsert(int_keyed_p_t   *overflowing_p,
			  int_keyed_p_t   *parent_p,
			  const int_key_t *key,
			  void            *data,
			  int             entry_level,
			  bool            level_reinserted[]);

    virtual rc choose_sub_tree(jon_stack_t<int_keyed_p_t *> &path_stack,
			       page_id_t                root_pid,
			       const int_key_t          &ins_key,
			       int                      num_levels);
    
    virtual rc sort_dim(bulk_data_t &bulk_data,
			int         elem_to_start,
			int         num_elem,
			int         dim_to_sort,
			int         outfile);

    virtual rc sort_into_pages(bulk_data_t &bulk_data,
		       int         outfile);

    virtual rc copy_into_pages(bulk_data_t &bulk_data,
			       int         start_idx,
			       int         num_elem,
			       int         outfile);

    virtual rc level_bulkload(int             infile,
			      int             outfile,
			      bool            &made_root);

    virtual rc attach_root(int        data_file,
			   page_id_t  &hdr_pid,
			   int        bulk_elem_sz,
			   bool       bulk_point_data,
			   int        levels,
			   const char *type_info);

    virtual void get_bulk_param(int data_file,
				int &bulk_num_dim,
				int &bulk_elem_size,
				int &bulk_point_data);

    virtual rc get_and_pin_node(      int_keyed_p_t * &node_ptr,
				      const page_id_t       &node_pid) const;

    virtual void free_unpin_stack(jon_stack_t<int_keyed_p_t *> &path_stack);

    virtual void free_and_unpin_node(int_keyed_p_t * &node_ptr) const;

    virtual bulk_data_t *alloc_bulk_data_hldr();

    static int elem_per_page(bulk_data_t &bulk_data,
			     int         elem_to_start,
			     int         num_elem,
			     int         page_size,
			     int_key_t   &bbox);

    virtual rc debug_node(FILE      *outfile,
			  page_id_t &node_id,
			  int       level,
			  bool      devise_output=false) const;

    rtree_hdr_p_t *rtree_hdr;

    static const double p = 0.35;
};

#endif
