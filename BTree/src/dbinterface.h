/*
 * dbinterface.h - top-level interface for generic database pages and
 *                 database managers. 
 * 
 * 961102  UW-Madison CS  Glass/Goldstein/Prock
 * 970303                 rewrite - slid down db_page_t
 */

#ifndef _DBINTERFACE_H
#define _DBINTERFACE_H

#include <stddef.h>
#include "setup.h"

enum rc {
  RC_OK,  
  RC_ERROR
};


#ifdef _DEBUG
#define JTRY(x)            \
{                         \
  rc myrc;                \
  if ((myrc=(x))!=RC_OK)  \
  {                       \
    printf("error at %s:%d '%s'\n", __FILE__, __LINE__, #x); \
    return myrc;          \
  }                       \
}
#else
#define JTRY(x) {x;}
#endif


class db_page_t {
  
  /*
   * page interface; index code invokes the following members
   * on pages it gets from its storage manager.
   *
   * More realistically, index code can use a higher-level page type
   * built using just these primites.
   */

 public:

  virtual size_t get_space_size () = 0;
  virtual char * get_space_start () = 0; /* returns an aligned address */
  
  virtual page_id_t get_page_id () = 0;
  
  virtual ~db_page_t() {}	

  bool dirty; // Will always be initialized to true.
};


class db_mgr_t {
  
  /*
   * database manager interface: index code uses this to 
   * get resources from underlying storage manager (of
   * arbitrary type).
   */

 public:
  
  virtual rc pin(const page_id_t & page_to_pin, db_page_t * & return_page) = 0;
				/* free/unpin by delete return_page */
  virtual rc alloc_page(page_id_t & return_page_id) = 0;

  virtual rc unalloc(page_id_t &id) = 0;

  virtual size_t get_pagesize() = 0;

  virtual ~db_mgr_t() {}

  virtual void reset_stats(void * oldstatbuf) = 0;

};

#endif
