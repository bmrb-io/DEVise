/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
 */

#include <sys/types.h>
#include <unistd.h>
#include"genrtree.h"
#include"globals.h"
#include <stdlib.h>


const trunc8const = 0xfffffff8;
const trunc4const = 0xfffffffc;

//#define aligned_size(size) (((size)+7) & trunc8const)
#define aligned_size(size) (size)


// Insert the char array entry on the page. This method must, in conjunction
// with all other slot operations, guarantee that the first item inserted
// on a page, if never deleted, will always have index 0
rc_t genrtree_p::insert_slot(void *entry,
			     int  entry_sz)
{
  RID DummyId;
  int new_size = aligned_size(entry_sz);
  char *actual_entry = ((char *) malloc(new_size));

  memcpy(actual_entry, entry, entry_sz);
/* for purify */  for(int i = entry_sz; i<new_size; i++) actual_entry[i]=0;
  insertRecord(actual_entry, new_size, DummyId);
  free(actual_entry);
  return RCOK;
}


// removes the slot associated with idx_num and does whatever work is necessary
// to ensure the access requirements specified by slot_val.
rc_t genrtree_p::remove_slot(int idx_num)
{
  deleteRecord(idx_to_rid(idx_num));
  compact_slot_dir();
  return RCOK;
}


// Overwrite the information in slot idx_num with the given information.
// The new information is guaranteed to be the same size as the old.
// Note that this method must keep slot 0 as slot 0 if slot 0 is overwritten.
rc_t genrtree_p::overwrite_slot(int  idx_num,
				void *entry,
				int  entry_sz)
{
  memcpy(&(data[slot[-idx_num].offset]), entry, entry_sz);
  return RCOK;
}


// This method returns the information in slot idx_num on the page. Note that
// the valid slot values are the number between 0 and slots_filled() (slot 0 is
// the header). Also note that the value of slot 0 must not change as long as 
// slot 0 isn't deleted. And the value for slot 0 is inserted before any 
// other(see insert). Also, the association between indices and slot values must 
// be constant under read only access patterns.
void *genrtree_p::slot_val(int idx_num)
{
  return &(data[slot[-idx_num].offset]);
}

// Returns the size, in bytes, of the data contained in slot idx_num.
int genrtree_p::slot_size(int idx_num)
{
  return (slot[-idx_num].length);
}


// Returns the number of slots allocated on the page.
int genrtree_p::slots_filled()
{
  return (-(*pSlotCnt));
}


// Returns the maximum number of non header entries per page given the current 
// element and key sizes
int genrtree_p::max_entries_per_page()
{
  genrtree_p_hdr &hdr = (*(alloc_hdr()));

//	cout << "elem_size() = " << elem_size() << endl;
//	cout << "key_size() = " << key_size() << endl;

  get_hdr(hdr);
  int result = ((PAGE_SIZE - DPFIXED - 5*sizeof(slot_t) - hdr.page_repr_size())*8) /
               (elem_size()*8+key_size()+1);
  result -= 1;
  delete(&hdr);
  return result;
}


// returns the pointer to the page
page_id_t genrtree_p::page_addr()
{
  page_id_t Self_pid;

  Self_pid.interp_pid_str(pCurPage);
  return Self_pid;
}


// Retrieve the page associated with new_pid and make this object be the interface
// to it. In addition, pin the page in memory and lock it according to lock_req.
rc_t genrtree_p::assoc_with_page_id_pin_lock(page_id_t new_pid,
					     lock_t    lock_req)
{
  port_genrtree_m::page_io++;
  setup_data(FileArr + new_pid.pid*PAGE_SIZE);
  if (is_leaf())
    port_genrtree_m::child_page_io++;
  return RCOK;
}


port_genrtree_p *genrtree_p::alloc_page()
{
  return (new genrtree_p);
}


int genrtree_m::max_entries_on_page(int num_dim,
				    int elem_size,
				    int level)
{
  int result;
  if (level != 0)
    result = ((PAGE_SIZE - HFPage::DPFIXED - 5*sizeof(slot_t) - 
	       page_id_t::page_repr_size-3*sizeof(int2))*8) /
	     (elem_size*8 + num_dim*2*sizeof(int)*8 + 1);
  else
    result = ((PAGE_SIZE - HFPage::DPFIXED - 5*sizeof(slot_t) - 
	       page_id_t::page_repr_size-3*sizeof(int2))*8) /
	     (elem_size*8 + num_dim*sizeof(int)*8 + 1);
  result -= 1;
  return result;
}


port_genrtree_p *genrtree_m::alloc_page()
{
  return (new genrtree_p);
}
