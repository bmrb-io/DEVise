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
  Revision 1.3  1996/11/08 15:42:24  jussi
  Removed IOTask::Initialize() and SetBuffering(). Added support
  for streaming via ReadStream() and WriteStream().

  Revision 1.2  1996/11/07 17:37:39  jussi
  Added -d (direct) command line option.

  Revision 1.1  1996/11/01 20:12:22  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "SBufMgr.h"

#if defined(THREAD_TASK) && defined(SOLARIS)
#include <thread.h>
#endif

#define CALL(c) { int res = c; if (res < 0) goto error; }

static int pageSize = 32 * 1024;

// ===================================================================

struct FileReq {
    SBufMgr *bufMgr;
    IOTask *task;
    int fileSize;
    int pipeSize;
};

// ===================================================================

void sleepms(int ms)
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ms * 1000;
    if (select(0, 0, 0, 0, &timeout) < 0) {
        perror("select");
        exit(1);
    }
}

// ===================================================================

void WriterP(SBufMgr *bufMgr, IOTask *task, int fileSize)
{
    for(int i = 0; i < fileSize; i++) {
        char *page = 0;
        if (!bufMgr) {
            if (MemPool::Instance()->Allocate(MemPool::Buffer, page) < 0) {
                perror("Writer alloc");
                exit(1);
            }
            assert(page);
            if (task->Produce(page, pageSize) < 0) {
                perror("Writer produce");
                exit(1);
            }
#if defined(THREAD_TASK) && defined(SOLARIS)
            // Circumvent stupid scheduling policy in Solaris thread package
            thr_yield();
#endif
        } else {
            if (bufMgr->AllocPage(task, i, page) < 0) {
                perror("Writer pin");
                exit(1);
            }
            sprintf(page, "test.%d Page %d %7.1f", i, i, (float)i);
            if (bufMgr->UnPinPage(task, i, true, pageSize, true) < 0) {
                perror("Writer unpin");
                exit(1);
            }
        }
    }

    if (!bufMgr) {
        char *page = 0;
        if (MemPool::Instance()->Allocate(MemPool::Buffer, page) < 0) {
            perror("Writer alloc");
            exit(1);
        }
        assert(page);
        if (task->Produce(page, 0) < 0) {
            perror("Writer produce");
            exit(1);
        }
    }
}

// ===================================================================

void *Writer(void *arg)
{
    FileReq *req = (FileReq *)arg;
    WriterP(req->bufMgr, req->task, req->fileSize);
    delete arg;
    return (void *)0;
}

// ===================================================================

void ReaderP(SBufMgr *bufMgr, IOTask *task, int fileSize)
{
    for(int i = 0; i < fileSize; i++) {
        char *page = 0;
        if (!bufMgr) {
            int bytes = task->Consume(page);
            if (bytes < 0) {
                perror("Reader read");
                exit(1);
            }
            assert(page);
            if (MemPool::Instance()->Release(MemPool::Buffer, page) < 0) {
                perror("Reader dealloc");
                exit(1);
            }
            assert(!page);
#if defined(THREAD_TASK) && defined(SOLARIS)
            // Circumvent stupid scheduling policy in Solaris thread package
            thr_yield();
#endif
        } else {
            if (bufMgr->PinPage(task, i, page) < 0) {
                perror("Reader pin");
                exit(1);
            }
            if (bufMgr->UnPinPage(task, i, false) < 0) {
                perror("Reader unpin");
                exit(1);
            }
        }
    }
}

// ===================================================================

void *Reader(void *arg)
{
    FileReq *req = (FileReq *)arg;
    ReaderP(req->bufMgr, req->task, req->fileSize);
    delete arg;
    return (void *)0;
}

// ===================================================================

int main(int argc, char **argv)
{
    char *files[argc];
    int numFiles = 0;
    int poolSize = 64;
    int fileSize = 512;
    int buffSize = 0;
    int readOnly = 0;
    int iotaskDirect = 0;

    int i,f;

    for(i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-b")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "No buffer size specified with -b option.\n");
                exit(1);
            }
            buffSize = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-p")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "No pool size specified with -p option.\n");
                exit(1);
            }
            poolSize = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-r")) {
            readOnly = 1;
        } else if (!strcmp(argv[i], "-d")) {
            iotaskDirect = 1;
        } else if (!strcmp(argv[i], "-f")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "No file size specified with -p option.\n");
                exit(1);
            }
            fileSize = atoi(argv[i + 1]);
            i++;
        } else if (argv[i][0] != '-') {
            files[numFiles++] = argv[i];
        } else {
            fprintf(stderr, "Unrecognized option: %s\n", argv[i]);
            exit(1);
        }
    }

    if (!numFiles) {
        fprintf(stderr, "No files specified.\n");
        exit(1);
    }

    int bufPages = poolSize - buffSize * numFiles;

    if (bufPages < numFiles) {
        // Allocate one page per file for caching, remaining pages for
        // buffering.
        bufPages = numFiles;
        buffSize = (poolSize - bufPages) / numFiles;
        fprintf(stderr, "Buffer size adjusted to %d\n", buffSize);
    }

    fprintf(stderr, "Buffer size %d, pool size %d, %d files, file size %d\n",
            buffSize, poolSize, numFiles, fileSize);
    fprintf(stderr, "I/O Task direct: %s, Read-only: %s\n",
            (iotaskDirect ? "YES" : "NO"), (readOnly ? "YES" : "NO"));

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

    SBufMgr *bufMgr = 0;
    if (!iotaskDirect) {
        bufMgr = new SBufMgrLRU(*memPool, bufPages);
        assert(bufMgr);
    }

    // test buffer manager

    time_t now = time(0);
    printf("Random number seed = %ld\n", now);
    srand(now);

    gettimeofday(&start, 0);

    // create files and I/O tasks

    IOTask *task[numFiles];
    FILE *fp[numFiles];

    int pipeSize = poolSize / numFiles;

#ifdef THREAD_TASK
    pthread_t child[numFiles];
#endif

    for(i = 0; i < numFiles; i++) {
        if (readOnly)
            fp[i] = fopen(files[i], "r");
        else
            fp[i] = fopen(files[i], "w+");
        if (!fp[i]) {
            fprintf(stderr, "Could not create %s\n", files[i]);
            exit(1);
        }
        task[i] = new FdIOTask(fileno(fp[i]));
        assert(task[i]);
        if (status < 0) {
            fprintf(stderr, "Cannot create I/O task %d\n", i);
            exit(1);
        }
        if (!bufMgr) {
            if (readOnly) {
                if (task[i]->ReadStream(fileSize * pageSize, pipeSize) < 0) {
                    perror("Reader stream");
                    exit(1);
                }
            } else {
                if (task[i]->WriteStream(pipeSize) < 0) {
                    perror("Writer stream");
                    exit(1);
                }
            }
        }
    }
        
    if (!readOnly) {
        printf("\nAllocating pages...\n");

        for(f = 0; f < numFiles; f++) {
#ifdef PROCESS_TASK
            pid_t child = fork();
            if (child < 0) {
                perror("fork");
                exit(1);
            }
            if (!child) {
                printf("Child %d started...\n", f);
                WriterP(bufMgr, task[f], fileSize);
                exit(1);
            }
#endif
#ifdef THREAD_TASK
            FileReq *req = new FileReq;
            req->bufMgr = bufMgr;
            req->task = task[f];
            req->fileSize = fileSize;
            if (pthread_create(&child[f], 0, Writer, req)) {
                perror("pthread_create");
                return -1;
            }
#endif
        }
    } else {

        printf("\nReading pages...\n");

        for(f = 0; f < numFiles; f++) {
#ifdef PROCESS_TASK
            pid_t child = fork();
            if (child < 0) {
                perror("fork");
                exit(1);
            }
            if (!child) {
                printf("Child %d started...\n", f);
                ReaderP(bufMgr, task[f], fileSize);
                exit(1);
            }
#endif
#ifdef THREAD_TASK
            FileReq *req = new FileReq;
            req->bufMgr = bufMgr;
            req->task = task[f];
            req->fileSize = fileSize;
            if (pthread_create(&child[f], 0, Reader, req)) {
                perror("pthread_create");
                return -1;
            }
#endif
        }
    }

    for(f = 0; f < numFiles; f++) {
#ifdef PROCESS_TASK
        int status;
        pid_t child = wait(&status);
        if (child < 0) {
            if (errno == EINTR)
                continue;
            if (errno != ECHILD) {
                perror("wait");
                exit(1);
            }
        } else {
            printf("Child completed...\n");
        }
#endif
#ifdef THREAD_TASK
        (void)pthread_join(child[f], 0);
        printf("Child completed...\n");
#endif
        if (!bufMgr)
            (void)task[f]->Terminate();
        delete task[f];
    }

    for(i = 0; i < numFiles; i++) {
        fclose(fp[i]);
        if (!readOnly)
            (void)unlink(files[i]);
    }

    gettimeofday(&stop, 0);
    secs = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time %.2f seconds, %.2f MB/s\n", secs,
           (numFiles * fileSize * pageSize) / 1048576.0 / secs);

    delete bufMgr;
    delete memPool;

    printf("\nPassed all tests.\n");

    return 1;
}
