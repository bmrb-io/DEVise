/*
 * stackpage.cc - page class providing simple stack (push, pop) access for
 *               variable-length records on an in-memory page.  Uses 
 *               a db_page_t, presumably one returned from a db_mgr_t.
 * 
 * Gideon Glass   970303  <gid@cs.wisc.edu>   UW-Madison CS
 */


#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "dbinterface.h"
#include "stackpage.h"

#define PAGEMAGIC 0xfeedface

/* 
 * Slotted page structure.  Each slot entry holds the byte offset into
 * the page corresponding to the starting address of that slot's data.
 * The first slot entry points to PAGEMAGIC at the end of the page
 * (this simplifies slot manipulation routines below).  The magic
 * constant 2 (in the code below) reflects two 'extra' slots: the slot-count 
 * at the start of the page and the reserved-for-next slot entry that would
 * be required to push a new slot entry.  
 *
 * (Addresses increase with increasing line number.)
 *
 * +=======================+
 * | number of slots used  |  1
 * | <offset to page end>  |
 * | <first slot entry>    |
 * | <second slot entry>   |
 * |    ...                |
 * | <nth slot entry>      |
 * | resrvd-for-next entry |  2
 * | <empty space>         |
 * |    ...                |
 * | <nth slot data>       |
 * |    ...                | 
 * | <second slot data>    |    
 * | <first slot data>     | <--- page end (right before PAGEMAGIC)
 * | PAGEMAGIC             | 
 * +=======================+
 *    
 *   Figure 1.  A happy page.
 *
 */ 


stack_page_t::stack_page_t (db_page_t * thepage, open_method mthd)
{
  page = thepage;
  datastart = page->get_space_start();
  pslotct = (unsigned short *) datastart;
  pmagic = (dbword_t *) (datastart + page->get_space_size() - sizeof(*pmagic));

  if (mthd == INITIALIZE_NEW) {
    init_st_p();
  } else {
//    assert(*pmagic == PAGEMAGIC);
  }
}


void stack_page_t::dump()
{
  for(int i=1; i <= slots_filled(); i++)
    {
      printf("SLOT %d:\n", i);
      for(int j=0; j < (int)(slot_size(i)/sizeof(dbword_t)); j++)
	printf(/*DBWORD_FMT*/"%016llx ", ((dbword_t *)slot_ptr(i))[j]);
      printf("\n");
    }
}


stack_page_t::~stack_page_t ()
{
//  assert( *pmagic == PAGEMAGIC );
  /* don't unpin page here - let user of this stack_page object worry about that;
     he might not want to. */
}


void stack_page_t::init_st_p ()
{
  pslotct[0] = 1;
  pslotct[1] = page->get_space_size() - sizeof(*pmagic); // dummy first slot
  *pmagic = PAGEMAGIC;
}


int stack_page_t::space_free() const
{
  return ((unsigned short *) datastart)[*pslotct] - (*pslotct + 2) * sizeof(unsigned short);
}

rc stack_page_t::push_slot(const void * entry, const int len)
{
  int freespace = space_free();

  unsigned short * plastold; 

  if (len > freespace) {
    DEBUG(("stack_page_t: push_slot: len (%d) > freespace (%d)\n", len, freespace));
    return RC_ERROR;
  }

  
  plastold = ((unsigned short *) datastart) + *pslotct;
  plastold[1] = plastold[0] - len;

/*  DEBUG(("push_slot: adding data to page %ld, slot #%d, [%d..%d]\n", pageno, *pslotct, plastold[1], plastold[0]-1)); */

  *pslotct += 1;
  memcpy((char *) datastart + plastold[1], entry, len);
  return RC_OK;
}

rc stack_page_t::pop_slot(void * entry, int & len)
{
  assert (*pslotct>=1);

  if (entry) {
    char* pstart = slot_addr(*pslotct);
    char* pend   = slot_addr(*pslotct -1);

    len = pend-pstart;
    memcpy(entry, pstart, len);
  }

  *pslotct = *pslotct - 1;

  return RC_OK;
}

rc stack_page_t::pop_slot()
{
  assert(*pslotct >= 1);
  
  *pslotct = *pslotct - 1;
  return RC_OK;
}
 
rc stack_page_t::overwrite_slot(int entry_number, const void * entry)
{
  assert (entry_number>= 1 && entry_number < *pslotct);

  entry_number++;

  char* pstart = slot_addr(entry_number);
  char* pend   = slot_addr(entry_number-1);

  memcpy (pstart, entry, pend-pstart);

  return RC_OK;
}

const void * stack_page_t::slot_val(int entry_number) const
{
  assert (entry_number>=1 && entry_number < *pslotct);

  entry_number++;

  return slot_addr(entry_number);
}

int stack_page_t::slot_size(int entry_number) const
{
  assert (entry_number>=1 && entry_number < *pslotct);

  entry_number++;

  return slot_addr(entry_number-1) - slot_addr(entry_number);
}

int stack_page_t::slots_filled() const
{
  return *pslotct-1;
}
