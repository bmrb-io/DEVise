/* -*- C++ -*- */
/*
 * aligned_sorted_page.h - page class providing sorted, 
 *   variable length records with "ALIGN_BYTES" byte alignment. 
 * Also, providing some of the same interface as class sorted_page_t (file sortedpage.h).
 
 NOTE: the db_page_t class is assumed to supply with correctly aligned space.
 
 *
 * The interface (and some of the implementation) is taken from sorted_page_t
 * (written by Gideon Glass). 
 * 
 * Uri Shaft. 970918 <uri@cs.wisc.edu>   UW-Madison CS
 */

#ifndef _ALIGNED_SORTED_PAGE_H
#define _ALIGNED_SORTED_PAGE_H

#include "setup.h"
#include "dbinterface.h"

class aligned_sorted_page_t {
 public:

  enum open_method {
    ALREADY_EXISTS,		// normal use
    INITIALIZE_NEW		// to initialize an empty page as a sorted page
    };

  // You should not delete the page "thepage". This is done when this page is deleted.
  aligned_sorted_page_t(db_page_t * thepage, open_method = ALREADY_EXISTS);
  virtual ~aligned_sorted_page_t();

  // The "compar" function for insert should provide 
  //  a. negative number for less than
  //  b. zero for equals
  //  c. positive number for greater than
  //
  // If the function pointer is NULL:
  //  The comparison here is memcmp using the minimal length 
  //  for both elements compared. If the elements are equal, 
  //  then the shorter one is assumed to be less than the longer one.
  rc insert_slot(const void* entry, int len, 
		 int (*compar)(const void*, const void*) = NULL);
  // The comp_len argument is the length for the comparison method. It is passed
  // as the third argument of "compar"
  rc insert_slot(const void* entry, int len, int comp_len, 
		 int (*compar)(const void*, const void*, int));
  // The index range (entry_number) is from 1 to slots_filled(). 
  rc remove_slot(int entry_number);
  // insert an entry and split the contents into two pages
  rc split_insert(const void* entry, int len, int comp_len, 
		  int (*compar)(const void*, const void*, int), 
		  aligned_sorted_page_t &new_page);
  rc split_insert(const void* entry, int len, 
		  int (*compar)(const void*, const void*), 
		  aligned_sorted_page_t &new_page);
  // distribute entries between two pages s.t. the other page's entries are larger than this page and 
  //     1. The other page is empty (prefered option)
  //  or 2. The free space is distributed as evenly as possible between the two pages.
  rc redistribute(aligned_sorted_page_t &rpage);

  int space_free() const;	// length of longest insertable record
  int space_used() const;       // amount of space used (including the slot directory).
  void clear_contents();	// delete all entries
  
  /*
   * methods below are inlined for speed.
   */

  // The index range (entry_number) is from 1 to slots_filled(). 
  int slots_filled() const;
  int slot_size(int entry_number) const	;   // likewise for entry_number here
  
  // slot_val returns a pointer to the space on the page. 
  // If you modify that space, make sure that the semantics of a sorted page
  // remains the same.
  void* slot_val(int entry_number); 
 protected:
  db_page_t * page;
  struct slot {
    unsigned short offset;
    unsigned short length;
  };
  struct slot* slotstart;  
  unsigned int begin_offset;
  unsigned int max_offset;
  unsigned short * pslotct;
  unsigned short * pmagic;
};


/*
 * Inlined methods 
 */


inline int aligned_sorted_page_t::slots_filled() const
{ 
  return *pslotct;
}

// [1..this->slots_filled()] is entry_number index range 
inline void* aligned_sorted_page_t::slot_val(int entry_number) 
{
  assert(entry_number >= 1);
  assert(entry_number <= *pslotct);
  return ((char *) slotstart) + slotstart[entry_number-1].offset;
}

// likewise for entry_number here
inline int aligned_sorted_page_t::slot_size(int entry_number) const
{
  assert(entry_number >= 1 && entry_number <= *pslotct);
  return slotstart[entry_number-1].length;
}

#endif







