#ifndef _DBJUSSI_H
#define _DBJUSSI_H

#include"setup.h"
#include"dbinterface.h"
#include"SBufMgr.h"

const int page_size=4096;

class db_page_jussi : public db_page_t
{

  public:

    db_page_jussi(CacheMgr  *mgr,
		  IOTask    *task,
		  char      *page_ptr,
		  page_id_t page_id)
      {
	jussi_mgr = mgr;
	my_task=task;
	page_data = page_ptr;
	pid=page_id;
	dirty=true;
      }

    ~db_page_jussi()
      {
	if (jussi_mgr->UnPinPage(my_task, *((int *)pid.data), dirty, -1, true) < 0)
	  printf("Couldn't unpin page in db_page_jussi\n");
      }
    
    virtual size_t get_space_size ()
      {
	return page_size;
      }
    
    virtual char * get_space_start ()
      {
	return page_data;
      }
    
    virtual page_id_t get_page_id ()
      {
	return pid;
      }

    
  private: 
    CacheMgr *jussi_mgr;    
    IOTask *my_task;
    char *page_data;
    page_id_t pid;
};


class db_mgr_jussi : public db_mgr_t
{
  public:
    db_mgr_jussi(const char     *filename,
		       CacheMgr *mgr);

    ~db_mgr_jussi();

    virtual size_t get_pagesize() 
      {
	return page_size;
      }

    virtual rc alloc_page(page_id_t & return_page_id)
      {
	char *page_bytes;
	*((int *)return_page_id.data)=next_page;
	if (jussi_mgr->PinPage(my_task, *((const int *)return_page_id.data), 
			       page_bytes, false) < 0)
	  return RC_ERROR;
	if (jussi_mgr->UnPinPage(my_task, *((const int *)return_page_id.data), 
				 true, -1, true) < 0)
	  return RC_ERROR;
	next_page++;
	return RC_OK;
      }

    virtual rc unalloc(page_id_t &pid) {return RC_OK;} /* Don't reclaim pages */

    virtual rc pin(const page_id_t   &page_to_pin,
		   db_page_t * &return_page)
      {
	char *page_bytes;
	if (jussi_mgr->PinPage(my_task, *((const int *)page_to_pin.data), 
			       page_bytes) < 0)
	  return RC_ERROR;
	return_page = new db_page_jussi(jussi_mgr, my_task, page_bytes, page_to_pin);
	return RC_OK;
      }

    virtual void reset_stats(void *my_stats) {}

  private:
    int    fd;
    int    next_page;
    IOTask *my_task;
    CacheMgr *jussi_mgr;
};



#endif
