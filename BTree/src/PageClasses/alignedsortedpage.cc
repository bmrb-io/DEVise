/* 

   Implementation of class aligned_sorted_page_t.
   ----------------------------------------------

1. Description of protected members:

  db_page_t * page; 

  struct slot {
    unsigned short offset; 
    unsigned short length; // Original data length. The slot length is a multiple of the alignment. 
  };

  unsigned int begin_offset; // The beginning of the filled slots.
  unsigned int max_offset;   // The offset to the end of the useful space on the page.

  The following are pointers to memory space on the actual page. 
  --------------------------------------------------------------
  struct slot * slotstart;  // The beginning of the slot directory
  unsigned short * pslotct; // Number of filled slots
  unsigned short * pmagic;  // Magic number

2. Implementation details:
  The space taken from the page is allocated as follows:
  offset    purpose
  -------   -------
    0-1     slot count
    2-3     magic number 
    4-x     slot directory
    y-end   data

  x = 3 + 4 * (slot count)
  x < y <= end

  The slot directory keeps the sorting order. The order in which the 
  slots are allocated is the order in which they were inserted. 
  This makes things more efficient for large elements.

  However, removing slots may change the offset of others.

 * Uri Shaft. 970918 <uri@cs.wisc.edu>   UW-Madison CS
 */

#include <stdio.h>
#include <string.h>
#include "setup.h"
#include "dbinterface.h"
#include "alignedsortedpage.h"

#define PAGEMAGIC 0xfeed
#ifndef ALIGNMENT
#define ALIGNMENT 1
#endif

aligned_sorted_page_t::aligned_sorted_page_t(db_page_t * thepage, open_method mthd)
{
  page = thepage;
  pslotct = (unsigned short*)(page->get_space_start());
  assert( ((int)pslotct % ALIGNMENT) == 0 ); // Check for space alignment
  pmagic = pslotct + 1;
  slotstart = (struct slot*)(pmagic + 1);
  max_offset = page->get_space_size();
  max_offset -= (max_offset % ALIGNMENT);
  max_offset -= 2 * sizeof(unsigned short);
  
  if(mthd == INITIALIZE_NEW) 
    {
      clear_contents();
    } 
  else
    {
      assert(*pmagic == PAGEMAGIC); // Check megic number
      begin_offset = max_offset;
      for(unsigned short i = 0; i < *pslotct; i++)
	if(slotstart[i].offset < begin_offset)
	  begin_offset = slotstart[i].offset;
    }
}

aligned_sorted_page_t::~aligned_sorted_page_t()
{
  delete page;
}


rc aligned_sorted_page_t::insert_slot(const void * entry, const int len, 
				      int (*compar)(const void*, const void*))
{
  // Check if there is enough space
  if(len > space_free() || len < 1) 
    {
      ERROR("aligned_sorted_page_t: insert_slot: len (%d) > free space (%d)\n", len, space_free());
      return RC_ERROR;
    }

  // Put the value in the page
  int slot_length = len + ALIGNMENT - 1;
  slot_length -= (slot_length % ALIGNMENT);
  begin_offset -= slot_length;
  memcpy((void*)((char*)slotstart + begin_offset), entry, len); 

  // Put the slot entry into place
  struct slot* s = slotstart + (*pslotct - 1);
  if(compar != NULL)
    while(s >= slotstart && compar(entry, (char*)slotstart + s->offset) < 0)
      {
	*(s + 1) = *s;
	s--;
      }
  else
    while(s >= slotstart)
      {
	int comp_len = s->length;
	if(comp_len > len) comp_len = len;
	int comp = memcmp(entry, (char*)slotstart + s->offset, comp_len);
	if(comp > 0 || (comp == 0 && len >= (int)(s->length)))
	  break;
	*(s + 1) = *s;
	s--;
      }    
  s++;
  s->offset = begin_offset;
  s->length = len;
  (*pslotct)++;
  
  return RC_OK;
}

rc aligned_sorted_page_t::insert_slot(const void * entry, const int len, int comp_len, 
				      int (*compar)(const void*, const void*, int))
{
  // Check if there is enough space
  if(len > space_free() || len < 1) 
    {
      ERROR("aligned_sorted_page_t: insert_slot: len (%d) > free space (%d)\n", len, space_free());
      return RC_ERROR;
    }

  // Put the value in the page
  int slot_length = len + ALIGNMENT - 1;
  slot_length -= (slot_length % ALIGNMENT);
  begin_offset -= slot_length;
  memcpy((void*)((char*)slotstart + begin_offset), entry, len); 

  // Put the slot entry into place
  struct slot* s = slotstart + (*pslotct - 1);
  while(s >= slotstart && compar(entry, (char*)slotstart + s->offset, comp_len) < 0)
    {
      *(s + 1) = *s;
      s--;
    }
  s++;
  s->offset = begin_offset;
  s->length = len;
  (*pslotct)++;

  return RC_OK;
}

rc aligned_sorted_page_t::remove_slot(int entry_number)
{
  // Check for valid entry number
  if(entry_number < 1 || entry_number > *pslotct)
    {
      ERROR("aligned_sorted_page_t: remove_slot: entry_number (%d) not in 1..%d\n", entry_number, *pslotct);
      return RC_ERROR;
    }

  // Find actual length and offset
  int len = slotstart[entry_number-1].length + ALIGNMENT - 1;
  len -= (len % ALIGNMENT);
  unsigned short offset = slotstart[entry_number-1].offset;
  
  // Move slots into place
  memmove((void*)(slotstart + (entry_number-1)) , (void*)(slotstart + entry_number),
	  sizeof(struct slot) * ((*pslotct) - entry_number));
  
  // Move actual data into place
  memmove((char*)slotstart + begin_offset + len, (char*)slotstart + begin_offset,
	  offset - begin_offset);
  begin_offset += len;

  // Fix the offsets of the slots
  (*pslotct)--;
  for(unsigned short j = 0; j < *pslotct; j++)
    if(slotstart[j].offset < offset)
      slotstart[j].offset += len;

  return RC_OK;
}

rc aligned_sorted_page_t::split_insert(const void* entry, const int len, int comp_len,
				       int (*compar)(const void*, const void*, int), 
				       aligned_sorted_page_t &new_page)
{
  if(new_page.slots_filled() != 0)
    {
      ERROR("aligned_sorted_page_t::split:  new page is not empty");
      return RC_ERROR;
    }
  if(len > new_page.space_free())
    {
      ERROR("aligned_sorted_page_t::split:  length of new entry (%d) > free space (%d)",
	    len, new_page.space_free());
      return RC_ERROR;
    }
  // Transfer some minimal number of entries from this page to the new page
  while(space_free() < len)
    {
      const void* e = slot_val(*pslotct);
      int l = slot_size(*pslotct);
      if(compar(entry, e, comp_len) >= 0)
	{
	  JTRY(new_page.insert_slot(entry, len, comp_len, compar));
	  return redistribute(new_page);
	}
      new_page.insert_slot(e,l,comp_len,compar);
      remove_slot(*pslotct);
    }
  JTRY(insert_slot(entry, len, comp_len, compar));
  return redistribute(new_page);
}

rc aligned_sorted_page_t::split_insert(const void* entry, const int len,
				       int (*compar)(const void*, const void*), 
				       aligned_sorted_page_t &new_page)
{
  if(new_page.slots_filled() != 0)
    {
      ERROR("aligned_sorted_page_t::split:  new page is not empty");
      return RC_ERROR;
    }
  if(len > new_page.space_free())
    {
      ERROR("aligned_sorted_page_t::split:  length of new entry (%d) > free space (%d)",
	    len, new_page.space_free());
      return RC_ERROR;
    }
  // Transfer some minimal number of entries from this page to the new page
  while(space_free() < len)
    {
      const void* e = slot_val(*pslotct);
      int l = slot_size(*pslotct);
      if(compar(entry, e) >= 0)
	{
	  JTRY(new_page.insert_slot(entry, len,  compar));
	  return redistribute(new_page);
	}
      new_page.insert_slot(e,l,compar);
      remove_slot(*pslotct);
    }
  JTRY(insert_slot(entry, len, compar));
  return redistribute(new_page);
}

rc aligned_sorted_page_t::redistribute(aligned_sorted_page_t &rpage)
{
  if(rpage.space_used() <= space_free()) // rpage becomes empty
    {
      for(int i = 0; i < *(rpage.pslotct); i++)
	{
	  int len = slotstart[*pslotct].length = rpage.slotstart[i].length;
	  len += ALIGNMENT - 1;
	  len -= (len % ALIGNMENT);
	  begin_offset -= len;
	  slotstart[*pslotct].offset = begin_offset;
	  memcpy((char*)slotstart + begin_offset, 
		 (char*)(rpage.slotstart) + rpage.slotstart[i].offset, slotstart[*pslotct].length);
	  (*pslotct)++;
	}
      rpage.clear_contents();
    }
  else if(space_free() < rpage.space_free()) // move from this page to rpage
    {
      int rfree = rpage.space_free();
      int tfree = space_free();
      while(tfree < rfree)
	{
	  int len = slotstart[*pslotct - 1].length;
	  int full_len = len + ALIGNMENT - 1;
	  full_len -= (full_len % ALIGNMENT);
	  if(rfree + 2 * (int)(full_len + sizeof(struct slot)) < tfree)
	    break;
	  const void* e = slot_val(*pslotct);
	  memmove((char*)(rpage.slotstart + 1), (char*)(rpage.slotstart), 
		  sizeof(struct slot) * (*(rpage.pslotct)));
	  rpage.begin_offset -= full_len;
	  memcpy((char*)(rpage.slotstart) + rpage.begin_offset, e, len);
	  rpage.slotstart[0].length = len;
	  rpage.slotstart[0].offset = rpage.begin_offset;
	  (*(rpage.pslotct))++;
	  remove_slot(*pslotct);
	  rfree = rpage.space_free();
	  tfree = space_free();
	}
    }
  else // move from rpage to this
    {
      int rfree = rpage.space_free();
      int tfree = space_free();
      while(rfree < tfree)
	{
	  int len = rpage.slotstart[0].length;
	  int full_len = len + ALIGNMENT - 1;
	  full_len -= (full_len % ALIGNMENT);
	  if(tfree + 2 * (int)(full_len + sizeof(struct slot)) < rfree)
	    break;
	  const void* e = rpage.slot_val(1);
	  begin_offset -= full_len;
	  memcpy((char*)(slotstart) + begin_offset, e, len);
	  slotstart[*pslotct].length = len;
	  slotstart[*pslotct].offset = begin_offset;
	  (*pslotct)++;
	  rpage.remove_slot(1);
	  rfree = rpage.space_free();
	  tfree = space_free();
	}
    }
  return RC_OK;
}

int aligned_sorted_page_t::space_free () const
{
  int result = (begin_offset - ((*pslotct) + 1) * sizeof(struct slot));
  result -= (result % ALIGNMENT);
  return result;
}

int aligned_sorted_page_t::space_used() const
{
  return (max_offset - begin_offset + (*pslotct) * sizeof(struct slot));
}

void aligned_sorted_page_t::clear_contents ()
{
  *pslotct = 0;
  *pmagic = PAGEMAGIC;
  begin_offset = max_offset;
}

