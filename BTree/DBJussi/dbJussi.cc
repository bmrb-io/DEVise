#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>      
#include <sys/stat.h>
#include <fcntl.h>

#include"dbJussi.h"


db_mgr_jussi::db_mgr_jussi(const char     *filename,
			         CacheMgr *mgr) 
{
  jussi_mgr = mgr;
  fd = open(filename, O_RDWR | O_CREAT, 0600);
  if (fd==-1)
    assert(0);
  next_page=lseek(fd, 0, SEEK_END)/page_size;
  int status;
  my_task = new FdIOTask(status,fd);
  assert(my_task && status >= 0);
}


db_mgr_jussi::~db_mgr_jussi()
{
  delete(my_task);
  close(fd);
}
