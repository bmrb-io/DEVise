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
  Revision 1.2  1997/03/28 16:07:52  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include"type_compat.h"
#include"hfpage.h"
#include"globals.h"

// Default constructor
page_id_t::page_id_t()
{
}


// Copy constructor
page_id_t::page_id_t(const page_id_t& pid_to_copy_from)
{
  pid = pid_to_copy_from.pid;
}


// Default destructor
page_id_t::~page_id_t()
{
}


// Returns the bit representation of the page_id. Returns pointer to memory
// allocated in this structure. Will be freed in this structure.
void *page_id_t::page_repr() const
{
  return ((void *) (&pid));
}

// Interprets a bit pattern produced from page_repr() and assigns this page_id_t
// to the appropriate value (ie. the inverse of page_repr())
void page_id_t::interp_pid_str (void *inp_data)
{
	memcpy(&pid, inp_data, sizeof(int));	
//  pid = *((int *) inp_data);	// inp_data was not aligned
}


// Assignment definition
page_id_t& page_id_t::operator =(const page_id_t& pid_to_assign_from)
{
  pid = pid_to_assign_from.pid;
  return *this;
}


// Equality comparison definition
bool page_id_t::operator ==(const page_id_t& pid_to_assign_from)
{
  return (pid == pid_to_assign_from.pid);
}


rc_t allocate_and_clear_new_page (page_id_t& new_page_id)
{
  HFPage new_page;
  
  new_page.setup_data(FileArr + NextEmptyPage*PAGE_SIZE);
  new_page.init(NextEmptyPage);
  NextEmptyPage++;
  new_page_id.pid = *(new_page.pCurPage);
//  if (write (RTreeFile, &(new_page.data[0]), PAGE_SIZE) != PAGE_SIZE)
//    return RCERR;
  return RCOK;
}
