#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "SBufMgr.h"

#define CALL(c) { int res = c; if (res < 0) goto error; }

int main()
{
    const int poolSize = 64;
    const int pageSize = 32 * 1024;
    const int numFiles = 2;
    const int fileSize = 512;
    const float buffRatio = 0.3;
    const int buffSize = (int)(buffRatio * poolSize / numFiles);
    const int bufPages = poolSize - buffSize * numFiles;

    int i,j,f;

    struct timeval start;
    struct timeval stop;
    double secs;

    // destroy all shared memory segments

    Semaphore::destroyAll();
    SharedMemory::destroyAll();

    // create space for 16 virtual semaphores

    int status = SemaphoreV::create(16);
    assert(status >= 0);

    // create buffer pool and buffer manager

    MemPool *memPool = new MemPool(poolSize, pageSize, status);
    assert(memPool);
    if (status < 0) {
        fprintf(stderr, "Cannot create memory pool\n");
        exit(1);
    }

    SBufMgr *bufMgr = new SBufMgrLRU(*memPool, memPool->NumPages());
    assert(bufMgr);

    // test buffer manager

    char* page;
    char cmp[pageSize];
    int iter;

    time_t now = time(0);
    printf("Random number seed = %ld\n", now);
    srand(now);

    IOTask *task[numFiles];

    printf("\nTesting Web read data access...\n");

    char *url = "http://www.cs.wisc.edu/~devise/devise/spie96.ps.gz";
    task[0] = new UnixWebIOTask(url, true);
    assert(task[0]);
    status = task[0]->Initialize();
    if (status < 0) {
        fprintf(stderr, "Cannot open URL %s\n", url);
        exit(1);
    }
    task[0]->SetBuffering(false, false, 0);
        
    gettimeofday(&start, 0);
    unsigned long int len = 0;

    int pageNum;
    for(pageNum = 0;; pageNum++) {
        char *addr;
        int bytes = bufMgr->PinPage(task[0], pageNum, addr);
        CALL(bytes);
        CALL(bufMgr->UnPinPage(task[0], pageNum, false));
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
    task[0] = new UnixWebIOTask(url, false);
    assert(task[0]);
    status = task[0]->Initialize();
    if (status < 0) {
        fprintf(stderr, "Cannot open URL %s\n", url);
        exit(1);
    }
    task[0]->SetBuffering(false, false, 0);
        
    gettimeofday(&start, 0);

    j = 4;
    for(pageNum = 0; pageNum < j; pageNum++) {
        char *addr;
        CALL(bufMgr->AllocPage(task[0], pageNum, addr));
        sprintf(addr, "Page %d\n", pageNum);
        CALL(bufMgr->UnPinPage(task[0], pageNum, true, pageSize, true));
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
        int bytes = bufMgr->PinPage(task[0], pageNum, addr);
        CALL(bytes);
#if 0
        printf("Page %d (%d bytes): \"%s\"\n", pageNum, bytes, addr);
        sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
        assert(strcmp(addr, cmp) == 0);
#endif
        CALL(bufMgr->UnPinPage(task[0], pageNum, false));
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
        sprintf(buf, "/tmp/bufmgr.%d", i);
        fp[i] = fopen(buf, "w+");
        if (!fp[i]) {
            fprintf(stderr, "Could not create %s\n", buf);
            exit(1);
        }
        task[i] = new UnixFdIOTask(fileno(fp[i]));
        assert(task[i]);
        int status = task[i]->Initialize();
        if (status < 0) {
            fprintf(stderr, "Cannot create I/O task %d\n", i);
            exit(1);
        }
        task[i]->SetBuffering(true, true, buffSize * pageSize);
    }
        
    printf("\nAllocating pages...\n");
    gettimeofday(&start, 0);
    for(f = 0; f < numFiles; f++) {
        for(i = 0; i < fileSize; i++) {
            CALL(bufMgr->AllocPage(task[f], i, page));
            sprintf(page, "test.%d Page %d %7.1f", f, i, (float)i);
            CALL(bufMgr->UnPinPage(task[f], i, true));
        }
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (numFiles * fileSize * pageSize) / 1048576.0 / secs);
    
    printf("\nReading pages back...\n");
    for(f = 0; f < numFiles; f++) {
        for(i = 0; i < 2; i++) {
            CALL(bufMgr->PinPage(task[f], i, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
            CALL(bufMgr->UnPinPage(task[f], i, false));
        }
    }
    gettimeofday(&start, 0);
    for(f = 0; f < numFiles; f++) {
        for(i = 2; i < fileSize; i++) {
            CALL(bufMgr->PinPage(task[f], i, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
            CALL(bufMgr->UnPinPage(task[f], i, false));
        }
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (numFiles * fileSize * pageSize) / 1048576.0 / secs);

    printf("\nRandomly reading pages...\n");
    for(f = 0; f < numFiles; f++)
        task[f]->SetBuffering(false, false, 0);
    gettimeofday(&start, 0);
    for(iter = 0; iter < 10; iter++) {
        for(i = 0; i < bufPages; i++) {
            f = rand() % numFiles;
            j = rand() % fileSize;
            CALL(bufMgr->PinPage(task[f], j, page));
            sprintf(cmp, "test.%d Page %d %7.1f", f, j, (float)j);
            assert(memcmp(page, cmp, strlen(cmp)) == 0);
        }
        for(f = 0; f < numFiles; f++)
            CALL(bufMgr->UnPin(task[f], false));
    }
    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (10 * bufPages * pageSize) / 1048576.0 / secs);
    
    for(f = 0; f < numFiles; f++)
        task[f]->SetBuffering(true, true, buffSize * pageSize);

    printf("\nOccupying all pages...\n");
    for(i = 0; i < bufPages; i++) {
        f = i / fileSize;
        j = i % fileSize;
        CALL(bufMgr->PinPage(task[f], j, page));
        sprintf(cmp, "test.%d Page %d %7.1f", f, j, (float)j);
        assert(memcmp(page, cmp, strlen(cmp)) == 0);
    }

    printf("\nTesting false positives...\n");
    if (bufMgr->PinPage(task[0], fileSize + 1, page) > 0) {
        printf("Buffer manager succeeded when it shouldn't have\n");
        goto error;
    }

    printf("\nTesting multiple pinnings...\n");
    CALL(bufMgr->PinPage(task[0], 0, page));
    for(i = 0; i < 100; i++) {
        char *page2;
        CALL(bufMgr->PinPage(task[0], 0, page2));
        assert(page == page2);
    }
    for(i = 0; i < 101; i++)
        CALL(bufMgr->UnPinPage(task[0], 0, false));

    for(f = 0; f < numFiles; f++)
        CALL(bufMgr->UnPin(task[f], false));
    
    if (numFiles >= 2) {
        printf("\nCopy file 1 to file 2...\n");
        gettimeofday(&start, 0);
        for(i = 0; i < fileSize; i++) {
            CALL(bufMgr->PinPage(task[0], i, page));
            char *page2;
            CALL(bufMgr->PinPage(task[1], i, page2));
            memcpy(page2, page, pageSize);
            CALL(bufMgr->UnPinPage(task[1], i, true));
            CALL(bufMgr->UnPinPage(task[0], i, false));
        }

        gettimeofday(&stop, 0);
        secs = stop.tv_sec - start.tv_sec
               + (stop.tv_usec - start.tv_usec) / 1e6;
        printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
               (2 * fileSize * pageSize) / 1048576.0 / secs);

        printf("\nComparing files 1 and 2...\n");
        gettimeofday(&start, 0);
        for(i = 0; i < fileSize; i++) {
            CALL(bufMgr->PinPage(task[0], i, page));
            char *page2;
            CALL(bufMgr->PinPage(task[1], i, page2));
            assert(memcmp(page, page2, strlen(page2)) == 0);
            CALL(bufMgr->UnPinPage(task[0], i, false));
            CALL(bufMgr->UnPinPage(task[1], i, false));
        }

        gettimeofday(&stop, 0);
        secs = stop.tv_sec - start.tv_sec
               + (stop.tv_usec - start.tv_usec) / 1e6;
        printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
               (2 * fileSize * pageSize) / 1048576.0 / secs);
    }

    for(f = 0; f < numFiles; f++)
        CALL(bufMgr->UnPin(task[f], false));
    
    printf("\n");

    for(i = 0; i < numFiles; i++) {
        delete task[i];
        fclose(fp[i]);
        char buf[64];
        sprintf(buf, "/tmp/bufmgr.%d", i);
        (void)unlink(buf);
    }

#if 0
    for(i = 0; i < numFiles; i++) {
        char buf[64];
        sprintf(buf, "/tmp/bufmgr.%d", i);
        fp[i] = fopen(buf, "w+");
        if (!fp[i]) {
            fprintf(stderr, "Could not create %s\n", buf);
            exit(1);
        }
        task[i] = new UnixFdIOTask(fileno(fp[i]));
        assert(task[i]);
        int status = task[i]->Initialize();
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
        CALL(bufMgr->PinPage(task[f], i, page));
        sprintf(page, "test.%d Page %d %7.1f", f, i, (float)i);
        CALL(bufMgr->UnPinPage(task[f], i, true, sizes[f]));
    }
    for(f = 0; f < numFiles; f++) {
        i = 0;
        int status = bufMgr->PinPage(task[f], i, page);
        CALL(status);
        if (status != sizes[f]) {
            printf("Error reading back %d-byte file page\n", sizes[f]);
            goto error;
        }
        sprintf(cmp, "test.%d Page %d %7.1f", f, i, (float)i);
        assert(memcmp(page, cmp, strlen(cmp)) == 0);
        CALL(bufMgr->UnPinPage(task[f], i, false));
    }

    for(i = 0; i < numFiles; i++) {
        delete task[i];
        fclose(fp[i]);
        char buf[64];
        sprintf(buf, "/tmp/bufmgr.%d", i);
        (void)unlink(buf);
    }
#endif

    delete bufMgr;
    delete memPool;

    printf("\nPassed all tests.\n");

    return 1;

  error:

    printf("\nThe tests failed.\n");

    for(i = 0; i < numFiles; i++) {
        delete task[i];
        fclose(fp[i]);
        char buf[64];
        sprintf(buf, "tmp/bufmgr.%d", i);
        (void)unlink(buf);
    }

    return 0;
}
