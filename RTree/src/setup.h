/*
 * setup.h - various miscellaneous things; changeable parameters also.
 */

#ifndef _SETUP_H
#define _SETUP_H

/* 
 * The NDEBUG flag is a standard C flag that turns off debugging code
 * (Specifically, the "assert" macro
 */
#ifndef _DEBUG
#define NDEBUG
#endif

#include<assert.h>

/*
 * system parameters:
 */

#define ALIGNMENT 8
 
typedef long long dbword_t;
typedef unsigned long long u_dbword_t;
#define DBWORD_FMT "%lld"
#define DBW_FMT lld
const u_dbword_t MAX_U_DBWORD_VAL=~0;
const dbword_t MIN_DBWORD_VAL = ((dbword_t)1) << (sizeof(dbword_t) * 8 - 1);

typedef short int2;
typedef int int4;
typedef long long int8;
typedef unsigned short uint2;
typedef unsigned int uint4;
typedef unsigned long long uint8;


/*
 * general support stuff.
 */

void ERROR(const char * fmt, ...);
void PERROR(const char * fmt, ...);
unsigned long _getpagesize();


#ifdef _DEBUG
extern int _debug;
#define DEBUG(args)  do { if (_debug) printf args ; fflush(stdout); } while (0)
#else
#define DEBUG(args)
#endif

/*
 * page_id class (change PAGE_ID_SIZE for your system)
 */

class page_id_t {
  
  /*
   * in-core representation of page ids for arbitrary 
   * storage managers.
   *
   * don't derive from this.
   */

 public:

  /*
   * for writing ourselves out to byte storage:
   */

  static const int PAGE_ID_SIZE=4;

  bool operator==(const page_id_t& pid_to_comp);

  unsigned char data[PAGE_ID_SIZE];
};

inline bool page_id_t::operator==(const page_id_t& pid_to_comp)
{
  return (memcmp((void *)data, (const void *)(pid_to_comp.data), PAGE_ID_SIZE)==0);
}
#endif