/*
 * setup.cc - various miscellaneous things; changeable parameters also.
 */


#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#ifdef _DEBUG
int dummy_page_shrink = 0;	// 4KB -> 256B
#else
int dummy_page_shrink = 0;
#endif

void ERROR(const char * fmt, ...)
{
  va_list args;
  
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void PERROR(const char * fmt, ...)
{
  va_list args;
  int error = errno;

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  
  errno = error;
  perror("");
}

unsigned long _getpagesize()
{
  unsigned long pgsize = 
#if (defined(__svr4__)) 
    sysconf(_SC_PAGESIZE);
#elif (defined(hpux))
    sysconf(_SC_PAGE_SIZE);
#else
    getpagesize();
#endif
  
  return pgsize >> dummy_page_shrink;
}
