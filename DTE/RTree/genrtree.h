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

#ifndef GENRTREE_H
#define GENRTREE_H

#include"port_genrtree.h"
#include"hfpage.h"

class genrtree_p : public port_genrtree_p , public HFPage
{
  public:

    // Insert the char array entry on the page. This method must, in conjunction
    // with all other slot operations, guarantee that the first item inserted
    // on a page, if never deleted, will always have index 0
    virtual rc_t insert_slot(void *entry,
			     int  entry_sz);


    // removes the slot associated with idx_num and does whatever work is necessary
    // to ensure the access requirements specified by slot_val.
    virtual rc_t remove_slot(int idx_num);


    // Overwrite the information is slot idx_num with the given information.
    // The new information is guaranteed to be the same size as the old.
    // Note that this method must keep slot 0 as slot 0 if slot 0 is overwritten.
    virtual rc_t overwrite_slot(int  idx_num,
				void *entry,
				int  entry_sz);


    // This method returns the information in slot idx_num on the page. Note that
    // the valid slot values are the number between 1 and slots_filled(). Also note
    // that the value of slot 0 must not change as long as slot 0 isn't deleted. And
    // the value for slot 0 is inserted before any other(see insert). Also, the
    // association between indices and slot values must be constant under read only
    // access patterns.
    virtual void *slot_val(int idx_num);


    // Returns the size, in bytes, of the data contained in slot idx_num.
    virtual int slot_size(int idx_num);


    // Returns the number of slots allocated on the page.
    virtual int slots_filled();


    // Returns the maximum number of entries per page given the current entry size
    virtual int max_entries_per_page();


    // returns the pointer to the page
    virtual page_id_t page_addr();


    // Retrieve the page associated with new_pid and make this class be the interface
    // to it. In addition, pin the page in memory and lock it according to lock_req.
    virtual rc_t assoc_with_page_id_pin_lock(page_id_t new_pid,
	       			             lock_t    lock_req);


    virtual port_genrtree_p *alloc_page();
};


class genrtree_m : public port_genrtree_m
{
  private:
    virtual int max_entries_on_page(int num_dim,
				    int elem_size,
				    int level);

    virtual port_genrtree_p *alloc_page();

};
#endif
