/* -*- C++ -*- */
/*
 * dbunix.h - db implementation on top of unix filesystem
 *
 * Gideon Glass/Andrew Prock  961111  UW-Madison CS
 */


#ifndef _DBUNIX_H
#define _DBUNIX_H

#include "setup.h"
#include "dbinterface.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

class db_mgr_unix : public db_mgr_t {
  /*
   * simple database on top of a regular file. 
   */

 friend class db_page_unix;

 private:
  long pins, unpins;
  int fd;
  bool failed;
  char * filebase;
  char * fileend;
  size_t pagesize;	// system page size

  unsigned long * pnextfreepage; //  page number (not offset) (stored in header page)

  struct header_page_info {
#define UNIXHEADERMAGIC 0xfeedface
    unsigned long magic;
    unsigned long firstfreepage;

  };

  rc unpin(unsigned long 
#ifdef _DEBUG
	   pageno) 
#else
    ) 
#endif
  {
    unpins++;
    DEBUG(("db_mgr_unix: unpinning page %ld \n", pageno));
    return RC_OK;
  }

 public:
  
  db_mgr_unix(const char * filename, const unsigned long filesize);
  ~db_mgr_unix();

  size_t get_pagesize() { return pagesize; }

  rc alloc_page(page_id_t & return_page_id);
  rc unalloc(page_id_t& id) {return RC_OK;} // Does nothing
  bool init_failed() { return failed; }

  rc pin(const page_id_t & page_to_pin, db_page_t * & return_page);

  struct dbunix_stats {
    unsigned long pins;
    unsigned long unpins;
  };

  void reset_stats(void * oldstatbuf); // pass a pointer to dbunix_stats, or NULL
				       // if you don't care

};


class db_page_unix : public db_page_t {

friend class db_mgr_unix;

private:

  db_mgr_unix* dbmgr;
  char * data;
  unsigned long pagesize;
  unsigned long pageno;

  db_page_unix(db_mgr_unix* db_mgr, unsigned long page_no, char * page_data, unsigned long page_size);

public:

  size_t get_space_size ();
  char * get_space_start ();

  page_id_t get_page_id ();
  
  ~db_page_unix();
};
 

#endif
