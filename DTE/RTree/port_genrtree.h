#ifndef PORT_GENRTREE_H
#define PORT_GENRTREE_H

#include<stdio.h>
#include"type_compat.h"
#include"tools/stack_t2.h"
#include"tools/ptr_stack.h"
#include"tools/pqueue.h"
#include"tools/key.h"
#include"bulk_data.h"

struct sort_entry
{
  int2 loc;
  int4 min;
  int4 max;

  sort_entry& operator=(const sort_entry& copy_from) {loc = copy_from.loc; 
						      min = copy_from.min;
						      max = copy_from.max;
						      return *this;}
};


// This is the in memory class corresponding to the information stored in the
// header on the rtree page. Note that any variations of the rtree derived from
// this implementation (e.g. compressed R-Trees), assumes that derived versions
// of this class still make full use of the variables in this class. Note that all
// actual header interpretation and representation issues are handled by this class
// through its methods.
class genrtree_p_hdr
{
  public:
    char    root[page_id_t::page_repr_size];
    int2    num_dim;
    int2    level; // Leaf if 1. Non-leaf is >1
    int2    elem_size; // Size of the actual data stored in the page


    virtual ~genrtree_p_hdr();


    // Returns a byte array of the representation of this header stored on a page.
    // Note that memory is allocated in this function and must be freed outside.
    virtual char *page_repr() const;


    // Returns the size of the byte array returned by page_repr()
    virtual int page_repr_size() const;


    // Interprets a page representation of a header passed through hdr_str and stores
    // the information in this object.
    virtual void interp_hdr_str(char *hdr_str);
};


const int max_num_entries = 10000;
const float m_perc = 0.4; // look at R* paper


class port_genrtree_p 
{
  public:

/**********Methods overridden to change the representation of entry keys **********/ 

    void read_whole_key (int       entry_num,
			 gen_key_t &key,
			 char      *type_info);

    // Reads key entry_num off the page and returns the key value in key
    virtual void read_key (int       entry_num,
			   gen_key_t &key);


    // Writes the key page representation into buf and the size(in bits) in size.
    virtual void get_key_repr (      char      *buf,
			       const gen_key_t &key);


    // Allocates the right kind of in memory header for this kind of rtree
    virtual genrtree_p_hdr *alloc_hdr();


    // Returns the size in bits of keys on the page.
    virtual int key_size();

/***************************** Normal public methods *****************************/

    // sets up slots and page directory. Clears previous contents
    rc_t port_genrtree_p::init_page(genrtree_p_hdr &hdr);


    // Sets the header for the page
    rc_t set_hdr (const genrtree_p_hdr& new_hdr);


    // Gets the header of the page
    void get_hdr(genrtree_p_hdr &hdr);


    // Returns true iff the page is a leaf.
    bool is_leaf ();


    // Returns the number of entries on the page
    int num_page_entries();

    
    // Returns true iff the page is the root.
    bool is_root ();


    // Returns #dimensions for entries on this page.
    int dim ();


    // Returns the size (in bytes) of data elements on this page.
    int elem_size();


    // Returns a pointer to the element corrseponding to the idx'th entry.
    // This pointer is a pointer directly into the page buffer in memory and
    // Therefore should not point to allocated memory before the function call.
    void *entry_elem_ptr(int idx);


    // Returns the value of m for splitting (look at R* paper)
    int m();


    // Finds the best path to follow in an internal node to insert a node with
    // a given key.
    rc_t find_best_child_for_insert (const gen_key_t& key, page_id_t& best_pid);


    // Returns true iff another entry can fit on the page
    bool can_insert ();


    // Inserts a (key,elem) pair in the page
    rc_t insert (const gen_key_t& key,       // key for new entry
                       void       *elem,     // element for new entry
		       int        elem_sz);  // element size(in bytes)


    // Inserts a (key, page_id) pair in the page
    rc_t insert (const gen_key_t& key,       // key for new entry
                 const page_id_t& ptr);      // pointer for new entry


    // Uses the entries on the page in addition to the new key and elem and splits it
    // into two pages. The first is this actual page. The second is assumed to be 
    // allocated and is returned through new_page.
    rc_t split (const gen_key_t&       key_to_add,    // Key to add
		      void             *elem_to_add,  // Elem to add
		      int              elem_sz,       // Element size
		      port_genrtree_p& new_page);     // Page created by the split


    // Uses the entries on the root page in addition to the new key and elem and 
    // splits it into three pages. The first is this actual page. The second and 
    // third are allocated locally and are invisible to the caller. The page 
    // pointer to the root of the entire tree is unchanged by this routine.
    rc_t split_root (const gen_key_t&  key_to_add,    // Key to add
		           void        *elem_to_add,  // Element to add
		           int         elem_sz);      // Element size
			       

    // replaces the key corresponding to the entry containing pid with new_key
    rc_t fix_key_for_entry(gen_key_t   new_key, // new key for elem
			   page_id_t   pid);    // pid whose key will be replaced


    // returns the key for the entire page
    gen_key_t key();


    // find the first entry spacially overlapping key starting with start_idx
    // Bp is a bounding box around the query. Used for clipping.
    virtual int find_valid_slot_starting_at(      int        start_idx,
				            const gen_key_t& key,
				                  dbl_box&   Bp);


/*************************** Pure virtual Methods ***************************/
    // Insert the char array entry on the page. Entry size is in bytes.
    // This method must, in conjunction
    // with all other slot operations, guarantee that the first item inserted
    // on a page, if never deleted, will always have index 0
    virtual rc_t insert_slot(void *entry,
			     int  entry_sz) = 0;


    // removes the slot associated with idx_num and does whatever work is necessary
    // to ensure the access requirements specified by slot_val.
    virtual rc_t remove_slot(int idx_num) = 0;


    // Overwrite the information in slot idx_num with the given information.
    // The new information is guaranteed to be the same size as the old.
    // Note that this method must keep slot 0 as slot 0 if slot 0 is overwritten.
    // Entry size is in bytes.
    virtual rc_t overwrite_slot(int  idx_num,
			        void *entry,
			        int  entry_sz) = 0;


    // This method returns a pointer to the data in slot idx_num. Note that
    // the valid slot values are the number between 0 and slots_filled(). Also note
    // that the value of slot 0 must not change as long as slot 0 isn't deleted. And
    // the value for slot 0 is inserted before any other(see insert). Also, the
    // association between indices and slot values must be constant under read only
    // access patterns.
    virtual void *slot_val(int idx_num) = 0;


    // Returns the size, in bytes, of the data contained in slot idx_num.
    virtual int slot_size(int idx_num) = 0;


    // Returns the number of slots allocated on the page.
    virtual int slots_filled() = 0;


    // Returns the maximum number of entries per page given the current entry size
    virtual int max_entries_per_page() = 0;


    // returns the pointer to the page
    virtual page_id_t page_addr() = 0;


    // Retrieve the page associated with new_pid and make this class be the interface
    // to it. In addition, pin the page in memory and lock it according to lock_req.
    virtual rc_t assoc_with_page_id_pin_lock(page_id_t new_pid,
					     lock_t    lock_req) = 0;


    virtual port_genrtree_p *alloc_page() = 0;


  private:
    // Create the array of entries from the page using only one dimension. In 
    // addition, key_to_add is put in the array. A zero is used in the position 
    // field of sort_entry to designate key_to_add instead of an entry on the page.
    void assign_sort_array (      sort_entry *sort_array,  // Array to assign
			          int        dim_num,      // The specific dim used
			    const gen_key_t& key_to_add);  // The new entry


    // Find the bounding box for the entries in the sort_array between first_entry 
    // and last entry and return through bbox. key_to_add is passed since of the 
    // entries in sort_array is an added entry not actually on the page itself.
    void generate_bbox_from_sort_array(      int        first_entry,
				             int        last_entry,
				             sort_entry *sort_array, 
				       const gen_key_t& key_to_add, 
				             gen_key_t& bbox);

    // Computes the goodness of the sort_array,  which is only based on one access. 
    // The notion of goodness used here comes directly from the R* tree paper. 
    // key_to_add is passed because one of the entries in sort_array is an 
    // additional entry not on the page. 
    long long compute_axis_goodness (      sort_entry *sort_array,
				     const gen_key_t& key_to_add);

    // This routine chooses the split access of the entries on the page in addition 
    // to the new entry passed in. The split access is chosen in the manner 
    // specified by the r* tree paper.
    int choose_split_axis (const gen_key_t& key_to_add);

    // This routine decides the actual split of the entries on the page plus 
    // key_to_add. The result is passed back in the priority queue one_half where 
    // the priority is the position on the page of the entry. A zero is used to 
    // designate the key_to_add.
    void choose_split_index(      pqueue_t<int>& one_half,
			    const gen_key_t&     key_to_add,
			          int            split_axis);
};


struct gen_rt_cursor_place_holder
{
  gen_rt_cursor_place_holder(port_genrtree_p &new_page):page(new_page) {};
  ~gen_rt_cursor_place_holder() {delete (&page); };
  dbl_box         Bp;
  port_genrtree_p &page;
  int             next_entry_num;
};


class gen_rt_cursor_t 
{
  public:
    gen_key_t &key;
    int       list_sz;
    gen_rt_cursor_place_holder *ancestors[16];
    bool initialized;

    gen_rt_cursor_t(gen_key_t &new_key) : key(new_key) {list_sz=0;initialized=false;};
    ~gen_rt_cursor_t() 
      { 
	if (initialized)
	  for (int i = 0; i<16; i++) 
	    delete ancestors[i]; 
      }
    port_genrtree_p& curr_page() {return ancestors[list_sz-1]->page;}
    int& curr_next_entry_num() {return ancestors[list_sz-1]->next_entry_num;}
    dbl_box& curr_Bp() {return ancestors[list_sz-1]->Bp;}
    rc_t add_and_pin_page (const page_id_t& page,
			         lock_t     lock);
    void del_top() { list_sz--; }
    void clear_stack() { list_sz = 0; }
};


class port_genrtree_m 
{
  public:
    static int page_io=0;
    static int child_page_io=0;

    // Initialize the root page of a tree
    virtual rc_t create(page_id_t& root,   // The page id of a clean page
			int2       dim);   // The dimensionality of the tree

    // Insert an entry
    rc_t insert(const page_id_t& root,     // The root of the tree to insert into
		const gen_key_t& key,      // Key to guide insertion
		      void       *elem,    // The data to insert 
		      int        elem_sz); // The data size (character array)

/*    // Delete an entry
    rc_t remove(const page_id_t& root,     // The root of the tree to delete from
		const gen_key_t& key,      // The key to guide deletion
		      void       *elem,    // The data to delete
		      int        elem_sz); // The data size
*/
    // Initialize the fetch
    rc_t fetch_init(const page_id_t&       root,    // The root of the tree to search
		          gen_rt_cursor_t& cursor); // the placeholder
			     
    // Find the next entry overlapping a key. Copies the data to the el buffer.
    rc_t fetch(gen_rt_cursor_t& cursor, // the placeholder
	       gen_key_t&       key,    // returned element key
	       void*            el,     // returned element
	       int&             elen,   // element length
	       bool&            eof);   // end of file

    rc_t draw(const page_id_t& root,
	            FILE*      outfile,
	            int        level);


    rc_t olddraw(const page_id_t& root,
	               FILE*      outfile);


    rc_t bulk_load(int       data_file, 
		   page_id_t &root_pid,
		   int       secondary_index);


    virtual int scan_pages();

  protected:
    rc_t adjust_root(const page_id_t& new_root,
		     const page_id_t& current_node);


    void read_data(bulk_data_t &bulk_data,
		   int         infile,
		   int         &level); 


    rc_t sort_into_pages(bulk_data_t &bulk_data,
			 int         outfile,
			 int         level);


    rc_t sort_dim(bulk_data_t &bulk_data,
		  int         elem_to_start,
		  int         num_elem,
		  int         dim_to_sort,
		  int         outfile,
		  int         level);


    virtual rc_t copy_into_pages(bulk_data_t &bulk_data,
				 int         start_idx,
				 int         num_elem,
				 int         outfile, 
				 int         level);


    rc_t level_bulkload(int             infile,
			int             outfile,
			bool            &made_root,
			int             level);

    // Sets up child and the ancestor stack deriving from a DFS search to get
    // to leaf using key as the key.
    virtual rc_t find_leaf_to_insert_into(const gen_key_t&      key,
					  const page_id_t&      root,
					        port_genrtree_p *ancestors[16],
					        int             &stack_size);


    // Finds the next tuple in the rtree specified in the cursor
    // which satisfies the query contained in the cursor. If there is no next
    // tuple, cursor.page is the root and cursor.next_entry_num is 0
    virtual rc_t find_next_valid_tuple(gen_rt_cursor_t& cursor);


    virtual int max_entries_on_page(int num_dim,
				    int elem_size,
				    int level) = 0;


    virtual bulk_data_t *alloc_bulk_data_hldr();

    virtual port_genrtree_p *alloc_page() = 0;

};

#endif
