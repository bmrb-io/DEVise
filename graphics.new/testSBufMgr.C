#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "SBufMgr.h"

#define CALL(c) { int res = c; if (res < 0) goto error; }

/*
   The following three definitions are not really needed except
   to make this thing link properly. Init.c and Exit.c reference
   these variables and functions.
*/

int RTreeFile = -1;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize) {}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize) {}

int main()
{
    const int poolSize = 64;
    const int pageSize = 32 * 1024;
    const int numFiles = 2;
    const int fileSize = 512;
    const float buffRatio = 0.3;
    const int buffSize = (int)(buffRatio * poolSize / numFiles);
    const int bufPages = poolSize - buffSize * numFiles;

    int i,j,f,iter;
    iosize_t len;

    char *page, *url;
    char cmp[pageSize];

    struct timeval start;
    struct timeval stop;
    double secs;

    MemMgr *memMgr;
    CacheMgr *cacheMgr;

    time_t now = time(0);
    printf("Random number seed = %ld\n", now);
    srand(now);

    // create buffer manager

    int status;
    memMgr = new MemMgr(poolSize, pageSize, status);
    assert(memMgr);
    if (status < 0) {
        fprintf(stderr, "Cannot create memory manager\n");
        exit(1);
    }

    printf("\nTesting memory manager...\n");
    char *ptrs[poolSize];
    // Allocate every page in memory
    for(i = 0; i < poolSize; i++)
        CALL(memMgr->Allocate(MemMgr::Cache, ptrs[i]));
    // Release every other page
    for(i = 0; i < poolSize; i += 2)
        CALL(memMgr->Deallocate(MemMgr::Cache, ptrs[i]));
    // Should not be able to allocate 2 contiguous pages
    j = 2;
    CALL(memMgr->Allocate(MemMgr::Cache, page, j));
    if (j != 1) {
        printf("Allocation returned %d pages, not 1\n", j);
        goto error;
    }
    CALL(memMgr->Deallocate(MemMgr::Cache, page, j));
    // Release page between two previously released pages
    CALL(memMgr->Deallocate(MemMgr::Cache, ptrs[1]));
    // Should now be able to allocate 3 contiguous pages
    j = 3;
    CALL(memMgr->Allocate(MemMgr::Cache, page, j));
    if (j != 3) {
        printf("Allocation returned %d pages, not 3\n", j);
        goto error;
    }
    CALL(memMgr->Deallocate(MemMgr::Cache, page, j));
    // Release all remaining pages
    for(i = 3; i < poolSize; i += 2)
        CALL(memMgr->Deallocate(MemMgr::Cache, ptrs[i]));
    // Should now be able to allocate all pages in one chunk
    j = poolSize;
    CALL(memMgr->Allocate(MemMgr::Cache, page, j));
    if (j != poolSize) {
        printf("Allocation returned %d pages, not %d\n", j, poolSize);
        goto error;
    }
    CALL(memMgr->Deallocate(MemMgr::Cache, page, j));

    cacheMgr = new CacheMgrLRU(*memMgr, memMgr->NumPages(), status);
    assert(cacheMgr && status >= 0);

    // test buffer manager

    IOTask *task[numFiles];

    printf("\nTesting Web read data access...\n");

    url = "http://www.cs.wisc.edu/~devise/devise/spie96.ps.gz";
    task[0] = new WebIOTask(status, url, true);
    assert(task[0]);
    if (status < 0) {
        fprintf(stderr, "Cannot open URL %s\n", url);
        exit(1);
    }
        
    gettimeofday(&start, 0);
    len = 0;

    int pageNum;
    for(pageNum = 0;; pageNum++) {
        char *addr;
        int bytes = cacheMgr->PinPage(task[0], pageNum, addr);
        CALL(bytes);
        CALL(cacheMgr->UnPinPage(task[0], pageNum, false));
        len += bytes;
        if (bytes < pageSize)
            break;
    }

    printf("Read %lu bytes from %s\n", len, url);

    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           len / 1048576.0 / secs);

    delete task[0];

    printf("\nTesting Web write data access...\n");

    url = "http://cgi.cs.wisc.edu/scripts/jussi/echotest";
    task[0] = new WebIOTask(status, url, false);
    assert(task[0]);
    if (status < 0) {
        fprintf(stderr, "Cannot open URL %s\n", url);
        exit(1);
    }
        
    gettimeofday(&start, 0);

    j = 4;
    for(pageNum = 0; pageNum < j; pageNum++) {
        char *addr;
        CALL(cacheMgr->AllocPage(task[0], pageNum, addr));
        sprintf(addr, "Page %d\n", pageNum);
        CALL(cacheMgr->UnPinPage(task[0], pageNum, true, pageSize, true));
    }

    status = task[0]->WriteEOF();

    len = j * pageSize;
    printf("Wrote %lu bytes to %s\n", len, url);

    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           len / 1048576.0 / secs);

    len = 0;

    for(pageNum = 0;; pageNum++) {
        char *addr;
        int bytes = cacheMgr->PinPage(task[0], pageNum, addr);
        CALL(bytes);
#if 0
        printf("Page %d (%d bytes): \"%s\"\n", pageNum, bytes, addr);
        sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
        assert(strcmp(addr, cmp) == 0);
#endif
        CALL(cacheMgr->UnPinPage(task[0], pageNum, false));
        len += bytes;
        if (bytes < pageSize)
            break;
    }

    printf("Read %lu bytes from %s\n", len, url);

    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           len / 1048576.0 / secs);

    delete task[0];

    // create files and I/O tasks

    FILE *fp[numFiles];
    for(i = 0; i < numFiles; i++) {
        char buf[64];
        sprintf(buf, "/tmp/cacheMgr.%d", i);
        fp[i] = fopen(buf, "w+");
        if (!fp[i]) {
            fprintf(stderr, "Could not create %s\n", buf);
            exit(1);
        }
        task[i] = new FdIOTask(status, fileno(fp[i]));
        assert(task[i]);
        if (status < 0) {
            fprintf(stderr, "Cannot create I/O task %d\n", i);
            exit(1);
        }
    }
        
    printf("\nAllocating pages...\n");
    gettimeofday(&start, 0);
    for(f = 0; f < numFiles; f++) {
        for(i = 0; i < fileSize; i++) {
            CALL(cacheMgr->AllocPage(task[f], i, page));
            sprintf(page, "test.%d Page %d %7.1f", f, i, (float)i);
            CALL(cacheMgr->UnPinPage(task[f], i, true));
        }
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (numFiles * fileSize * pageSize) / 1048576.0 / secs);
    
    printf("\nReading pages back...\n");
    for(f = 0; f < numFiles; f++) {
        for(i = 0; i < 2; i++) {
            CALL(cacheMgr->PinPage(task[f], i, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
            CALL(cacheMgr->UnPinPage(task[f], i, false));
        }
    }
    gettimeofday(&start, 0);
    for(f = 0; f < numFiles; f++) {
        for(i = 2; i < fileSize; i++) {
            CALL(cacheMgr->PinPage(task[f], i, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
            CALL(cacheMgr->UnPinPage(task[f], i, false));
        }
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (numFiles * fileSize * pageSize) / 1048576.0 / secs);

    printf("\nRandomly reading pages...\n");
    gettimeofday(&start, 0);
    for(iter = 0; iter < 10; iter++) {
        for(i = 0; i < bufPages; i++) {
            f = rand() % numFiles;
            j = rand() % fileSize;
            CALL(cacheMgr->PinPage(task[f], j, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, j, (float)j);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
        }
        for(f = 0; f < numFiles; f++)
            CALL(cacheMgr->UnPin(task[f], false));
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (10 * bufPages * pageSize) / 1048576.0 / secs);
    
    printf("\nOccupying all pages...\n");
    for(i = 0; i < bufPages; i++) {
        f = i / fileSize;
        j = i % fileSize;
        CALL(cacheMgr->PinPage(task[f], j, page));
        sprintf(cmp, "test.%d Page %d %7.1f", f, j, (float)j);
        assert(memcmp(page, cmp, strlen(cmp)) == 0);
    }

    printf("\nTesting false positives...\n");
    if (cacheMgr->PinPage(task[0], fileSize + 1, page) > 0) {
        printf("Buffer manager succeeded when it shouldn't have\n");
        goto error;
    }

    printf("\nTesting multiple pinnings...\n");
    CALL(cacheMgr->PinPage(task[0], 0, page));
    for(i = 0; i < 100; i++) {
        char *page2;
        CALL(cacheMgr->PinPage(task[0], 0, page2));
        assert(page == page2);
    }
    for(i = 0; i < 101; i++)
        CALL(cacheMgr->UnPinPage(task[0], 0, false));

    for(f = 0; f < numFiles; f++)
        CALL(cacheMgr->UnPin(task[f], false));
    
    if (numFiles >= 2) {
        printf("\nCopy file 1 to file 2...\n");
        gettimeofday(&start, 0);
        for(i = 0; i < fileSize; i++) {
            CALL(cacheMgr->PinPage(task[0], i, page));
            char *page2;
            CALL(cacheMgr->PinPage(task[1], i, page2));
            memcpy(page2, page, pageSize);
            CALL(cacheMgr->UnPinPage(task[1], i, true));
            CALL(cacheMgr->UnPinPage(task[0], i, false));
        }

        gettimeofday(&stop, 0);
        secs = stop.tv_sec - start.tv_sec
               + (stop.tv_usec - start.tv_usec) / 1e6;
        printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
               (2 * fileSize * pageSize) / 1048576.0 / secs);

        printf("\nComparing files 1 and 2...\n");
        gettimeofday(&start, 0);
        for(i = 0; i < fileSize; i++) {
            CALL(cacheMgr->PinPage(task[0], i, page));
            char *page2;
            CALL(cacheMgr->PinPage(task[1], i, page2));
            assert(memcmp(page, page2, strlen(page2)) == 0);
            CALL(cacheMgr->UnPinPage(task[0], i, false));
            CALL(cacheMgr->UnPinPage(task[1], i, false));
        }

        gettimeofday(&stop, 0);
        secs = stop.tv_sec - start.tv_sec
               + (stop.tv_usec - start.tv_usec) / 1e6;
        printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
               (2 * fileSize * pageSize) / 1048576.0 / secs);
    }

    for(f = 0; f < numFiles; f++)
        CALL(cacheMgr->UnPin(task[f], false));
    
    printf("\n");

    for(i = 0; i < numFiles; i++) {
        delete task[i];
        fclose(fp[i]);
        char buf[64];
        sprintf(buf, "/tmp/cacheMgr.%d", i);
        (void)unlink(buf);
    }

#if 0
    for(i = 0; i < numFiles; i++) {
        char buf[64];
        sprintf(buf, "/tmp/cacheMgr.%d", i);
        fp[i] = fopen(buf, "w+");
        if (!fp[i]) {
            fprintf(stderr, "Could not create %s\n", buf);
            exit(1);
        }
        task[i] = new FdIOTask(status, fileno(fp[i]));
        assert(task[i]);
        if (status < 0) {
            fprintf(stderr, "Cannot create I/O task %d\n", i);
            exit(1);
        }
    }

    printf("\nTesting odd-sized files...");
    int sizes[numFiles];
    for(f = 0; f < numFiles; f++) {
        i = 0;
        sizes[f] = rand() % pageSize;
        printf(" %d", sizes[f]);
        if (sizes[f] < 128) sizes[f] = 128;
        CALL(cacheMgr->PinPage(task[f], i, page));
        sprintf(page, "test.%d Page %d %7.1f", f, i, (float)i);
        CALL(cacheMgr->UnPinPage(task[f], i, true, sizes[f]));
    }
    for(f = 0; f < numFiles; f++) {
        i = 0;
        int status = cacheMgr->PinPage(task[f], i, page);
        CALL(status);
        if (status != sizes[f]) {
            printf("Error reading back %d-byte file page\n", sizes[f]);
            goto error;
        }
        sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
        assert(memcmp(page, cmp, strlen(cmp)) == 0);
        CALL(cacheMgr->UnPinPage(task[f], i, false));
    }

    for(i = 0; i < numFiles; i++) {
        delete task[i];
        fclose(fp[i]);
        char buf[64];
        sprintf(buf, "/tmp/cacheMgr.%d", i);
        (void)unlink(buf);
    }
#endif

    delete cacheMgr;
    delete memMgr;

    printf("\nPassed all tests.\n");

    return 1;

  error:

    printf("\nThe tests failed.\n");

    for(i = 0; i < numFiles; i++) {
        char buf[64];
        sprintf(buf, "tmp/cacheMgr.%d", i);
        (void)unlink(buf);
    }

    return 0;
}
