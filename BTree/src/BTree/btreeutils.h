/* -*- C++ -*- */
/*
 * B-tree entry type. Converts int_key_t and a void* or a db_page_t into 
 * an entry. Also converts the entry into a page representation (void*).
 *
 * B-tree page. Based on the aligned_sorted_page_t class.
 *
 * Uri Shaft   970919  <uri@cs.wisc.edu>   UW-Madison CS
 */

#ifndef BTREE_UTILS_H__
#define BTREE_UTILS_H__

#include "setup.h"
#include "dbinterface.h"
#include "alignedsortedpage.h"
#include "int_key.h"


//
// Class btree_entry_t takes a <key,elem> pair and combines them into a single
// entry in memory. The element can be either a void* or a page id. 
//

class btree_entry_t {
 public:
  btree_entry_t(const int_key_t &key, const void* elem = NULL, 
		unsigned int elem_size = 0, bool use_max = false);
  btree_entry_t(const btree_entry_t &key, const void* new_elem = NULL, 
		unsigned int new_elem_size = 0);
  btree_entry_t(const dbword_t* key, int dims, const void* new_elem = NULL, 
		unsigned int new_elem_size = 0);
  // If copy_space is false, "e" is not copied.
  // Free memory allows you to choose who handles the memory allocated for "e".
  // It is set to "true" for all cases except when copy_space is false.
  // If it is false then "e" is not freed when the entry is deleted.
  btree_entry_t(void* e, int e_size, int dims, bool copy_space = true, bool free_memory = true);
  ~btree_entry_t();
  
  // Get key 
  int num_dims() const;
  dbword_t* key();

  // Get element 
  int elem_size() const;
  void* element();

  // Get entry
  int size() const; 
  void* page_repr();

  // Comparisons of the key only.
  bool operator <=(const btree_entry_t & e) const;
  bool operator ==(const btree_entry_t & e) const;

  // Full equality (entire entry)
  bool equals(const btree_entry_t& e) const;

  // Box inclusion
  bool inside(const btree_entry_t& low, const btree_entry_t& high) const;

  void print(FILE *fd, bool is_internal = false); // const; 
 private:
  int dims;
  char* entry;
  int entry_size;
  bool free_memory;
};


class btree_page_t : private aligned_sorted_page_t {
 public:
  btree_page_t(db_page_t *thepage, int num_dims, bool is_new_page);
  virtual ~btree_page_t();

  // Information about the page
  int num_entries() const;
  // If make_copy is false, then some of the memory used for the entry is in the page.
  // You should delete the entry in any case. However, do not use the entry after the page
  // is deleted.
  btree_entry_t* entry(int entry_number, bool make_copy = true); // From 1 to num_entries().
  
  // Insertion methods
  // The "should_split" methods needs the next entry for insertion to decide
  //  about splits. This is because we assume variable length entries.
  // Always call "should_split" before inserting an entry.
  virtual bool should_split(btree_entry_t &entry) const;
  virtual rc insert(btree_entry_t &entry);
  // The "split" does insertion with a split at the same time.
  virtual rc split(btree_entry_t &entry, btree_page_t &new_page);

  // Deletion methods
  virtual rc remove(int entry_number);
  virtual bool try_to_merge() const;
  // rpage is to the right of this page (in the tree).
  // If merging is possible it is done and rpage becomes empty.
  // Otherwise, redistribution of entries is possible.
  virtual rc merge(btree_page_t &rpage); 

  // Search returns an entry number "i" s.t. "entry" is <= the i'th entry
  // in the page and "entry" is > the (i+1) entry in the page 
  // (or "i" is the last entry in the page).
  // If the first entry in the page is > than "entry" then the return value is 0.
  virtual int search(/*const*/ btree_entry_t &entry); // const;
  // Exact search finds the first entry in the page with a key that equals "entry".
  // If no such entry exists, the return value is 0.
  virtual int exact_search(/*const*/ btree_entry_t &entry); // const;

  void info(FILE *fd) ; // const; // print info for debugging
  virtual rc debug(FILE* fd, int height, int max_height, db_mgr_t* mgr);
 private:
  int dims;
};


//
//  Inlined methods
//

inline int btree_entry_t::num_dims() const
{
  return dims;
}

inline dbword_t* btree_entry_t::key()
{
  return (dbword_t*)entry;
}

inline int btree_entry_t::elem_size() const
{
  return entry_size - dims * sizeof(dbword_t);
}

inline void* btree_entry_t::element()
{
  return (void*)(entry + (dims * sizeof(dbword_t)));
}

inline int btree_entry_t::size() const
{
  return entry_size;
}

inline void* btree_entry_t::page_repr()
{
  return (void*)entry;
}

#endif





