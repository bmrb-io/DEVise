/*
 * dbunix.cc - db implementation on top of unix filesystem
 *             classes db_mgr_unix and db_page_unix.
 *
 * Gideon Glass/Andrew Prock  961111  UW-Madison CS
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <memory.h>

#include "dbunix.h"
#include "dbinterface.h"


db_mgr_unix::db_mgr_unix(const char * filename, const unsigned long filesize)
{
  failed = true;
  filebase = fileend = 0;	// trigger segfault if used
  bool doformat = false;
  struct header_page_info h, * ph;
  unsigned long frags = (PROT_READ | PROT_WRITE);

  DEBUG(("db_mgr_unix: opening %s, %ld bytes\n", filename, filesize));

  pagesize = _getpagesize();
  pins = unpins = 0;
  
  fd = open(filename, O_RDWR);
  if (fd == -1) {
    if (errno == ENOENT) {
      fd = open(filename, O_RDWR | O_CREAT, 0666);
      if (fd == -1) {
	DEBUG(("db_mgr_unix::db_mgr_unix: open(..., | O_CREAT) failed\n"));
	return;
      } else {
	char ch = 0;
	if (lseek(fd, filesize-1, SEEK_SET) == -1) {
	  PERROR("lseek");
	  DEBUG(("lseek: bailed formatting file, returning\n"));
	}
	if (write(fd, &ch, 1) == -1) {
	  PERROR("write");
	  DEBUG(("write: bailed formatting file, returning\n"));
	  return;
	}
	doformat = true;
      }
    } else {
      DEBUG(("db_mgr_unix::db_mgr_unix: errno != 0 && errno != ENOENT (%d)\n", errno));
      return;
    }
  } else {
    DEBUG(("db_mgr_unix::db_mgr_unix: %s exists, opened fine\n", filename));
  }


  filebase = (char *) mmap(0, filesize, frags , MAP_SHARED
#if !defined(SGI)
      | MAP_NORESERVE
#endif
      , fd, 0);
  if (filebase == (char *) -1) {
    DEBUG(("db_mgr_unix::db_mgr_unix: mmap failed\n"));
    PERROR("db_mgr_unix:mmap");
        
    fileend = NULL;
    return;
  } else {
    DEBUG(("filebase = %p\n", filebase));
  }

  fileend = filebase + filesize;
  failed = false;

  ph = (struct header_page_info *) filebase;
  
  if (doformat) {
    h.magic = UNIXHEADERMAGIC;
    h.firstfreepage = 1;
    *ph = h;
  }

  pnextfreepage = &ph->firstfreepage;
  DEBUG(("pnextfreepage points into file at offset %d\n", (char *) pnextfreepage - (char *) filebase));
  DEBUG(("read *pnextfreepage = %ld\n", *pnextfreepage));

}


db_mgr_unix::~db_mgr_unix()
{
  unsigned long pagecnt = *pnextfreepage;
  if (munmap(filebase, fileend - filebase) == -1) {
    DEBUG(("db_mgr_unix::~db_mgr_unix: munmap(%p, %d) failed\n", filebase, fileend - filebase)); 
  }

  if (close(fd) == -1) {
    DEBUG(("db_mgr_unix::~db_mgr_unix: close failed\n"));
  }

  if (unpins < pins) 
    DEBUG(("bad pin count: number of pinned pages -> %ld pages are PINNED\n", pins - unpins));

  if (unpins != pins) {
    DEBUG(("~db_mgr_unix: BAD STUFF HAPPENED\n"));
  } else {
    DEBUG(("~db_mgr_unix: closed stuff without bad stuff happenin\n"));
  }
  
  printf("database manager activity: %ld pages pinned, %ld pages unpinned\n", pins, unpins);
  printf("next free page = %lu\n", pagecnt);
  printf("\nmoritori te salutamus\n\n");
}

rc db_mgr_unix::pin(const page_id_t & page_to_pin, db_page_t * & return_page)
{
  unsigned long pageno;
  // int len;

  // page_to_pin.page_repr(&pageno, len);
  memcpy(&pageno, page_to_pin.data, page_id_t::PAGE_ID_SIZE);
  // assert(len == sizeof(unsigned long));
  
  DEBUG(("db_mgr_unix::pin: pinning page %ld\n", pageno));

  if (pageno >= *pnextfreepage) {
    DEBUG(("ERROR! trying to pin page >= *pnextfreepage (%ld)\n", *pnextfreepage));
  }
  
  return_page = new db_page_unix(this, pageno, 
				 filebase + pagesize * pageno, 
				 pagesize);

  assert(return_page != NULL);

  pins++;
  return RC_OK;
}

rc db_mgr_unix::alloc_page(page_id_t & pageid)
{
  unsigned long newpage = *pnextfreepage;
  char * data;
  
  *pnextfreepage = *pnextfreepage + 1;
  //  pageid.interp_page_id_str(&newpage, sizeof(newpage));
  memcpy(pageid.data, &newpage, page_id_t::PAGE_ID_SIZE);

  DEBUG(("db_mgr_unix::alloc_page: giving new page id %ld\n", newpage));

  data = filebase + newpage * pagesize;
  memset(data, '\0', pagesize);
  
  return RC_OK;
}

void db_mgr_unix::reset_stats(void * oldstatbuf)
{
  struct dbunix_stats * stbuf = (struct dbunix_stats *) oldstatbuf;

  if (stbuf) {
    stbuf->pins = pins;
    stbuf->unpins = unpins;
  }

  pins = 0;
  unpins = 0;
}


db_page_unix::db_page_unix(db_mgr_unix* db_mgr, unsigned long page_no, char * page_data, unsigned long page_size)
{
  dbmgr = db_mgr;
  pageno = page_no;
  data = page_data;
  pagesize = page_size;
}


db_page_unix::~db_page_unix ()
{
  dbmgr->unpin(pageno);
}
  
page_id_t db_page_unix::get_page_id ()
{
  page_id_t id;

  // id.interp_page_id_str (&pageno, sizeof(pageno));
  memcpy(id.data, &pageno, page_id_t::PAGE_ID_SIZE);

  return id;
}

size_t db_page_unix::get_space_size () 
{
  return pagesize;
}

char * db_page_unix::get_space_start ()
{
  return data;
}
