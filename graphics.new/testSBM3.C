/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.3  1996/12/13 21:33:35  jussi
  Updated to use SemaphoreV::maxNumSemaphores().

  Revision 1.2  1996/12/12 21:05:15  jussi
  Removed calls to Semaphore::DestroyAll() and SharedMemory::DestroyAll().
  Added missing call to delete memory manager.

  Revision 1.1  1996/12/03 20:30:46  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "SBufMgr.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s url     fetches a Web resource\n",
                argv[0]);
        exit(1);
    }

    // create buffer manager

    const int poolSize = 64;
    const int pageSize = 32 * 1024;
    int status;
    MemMgr *memMgr = new MemMgr(poolSize, pageSize, status);
    assert(memMgr);
    if (status < 0) {
        fprintf(stderr, "Cannot create memory manager\n");
        exit(1);
    }

    char *url = argv[1];

    WebIOTask task(status, url, true);
    if (status < 0) {
        fprintf(stderr, "Cannot initiate Web data transfer\n");
        exit(1);
    }
    status = task.ReadStream(0, 0, poolSize);
    if (status < 0) {
        fprintf(stderr, "Cannot create data transfer process\n");
        exit(1);
    }

    fprintf(stderr, "Fetching %s...\n", url);

    struct timeval start;
    gettimeofday(&start, 0);

    streampos_t offset = 0;
    bytecount_t len = 0;

    while (1) {
        char *page;
        streampos_t off;
        iosize_t bytes;
        int status = task.Consume(page, off, bytes);
        if (status < 0) {
            perror("Reader read");
            exit(1);
        }
        if (!bytes)
            break;
        fwrite(page, bytes, 1, stdout);
        assert(page && offset == off);
        offset += pageSize;
        len += pageSize;
        if (MemMgr::Instance()->Deallocate(MemMgr::Buffer, page) < 0) {
            perror("Reader dealloc");
            exit(1);
        }
    }

    fprintf(stderr, "Read %lu bytes from %s\n", (unsigned long)len, url);

    struct timeval stop;
    gettimeofday(&stop, 0);
    double secs = stop.tv_sec - start.tv_sec
                  + (stop.tv_usec - start.tv_usec) / 1e6;
    fprintf(stderr, "Elapsed time %.2f seconds, %.2f MB/s\n", secs,
            len / 1048576.0 / secs);

    task.Terminate();

    delete memMgr;

    return 0;
}
