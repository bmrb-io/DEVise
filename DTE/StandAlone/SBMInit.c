#include "SBMInit.h"
#include "SBufMgr.h"
#include "InitShut.h"

char* bulk_temp_file_name="./bulk_temp";

int _debug = 0;

MemMgr *memMgr;

CacheMgr *cacheMgr;

void initialize_system(){
  int status;
  memMgr = new MemMgr(poolSize, pageSize, true, status);
  assert(memMgr);
  if (status < 0) {
    fprintf(stderr, "Cannot create memory manager\n");
    exit(1);
  }

  cacheMgr = new CacheMgrLRU(*memMgr, poolSize, status);
  assert(cacheMgr && status >= 0);
}

void shutdown_system(){
	delete cacheMgr;
	delete memMgr;
	cacheMgr = NULL;
	memMgr = NULL;
}
