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
  Revision 1.1  1996/08/01 22:45:27  jussi
  Initial revision.
*/

#include <memory.h>
#include <fcntl.h>
#include <signal.h>

#ifdef PROCESS_TASK
#include <sys/wait.h>
#endif

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include "ExtBufMgr.h"
#include "TData.h"
#include "Exit.h"

#define DEBUGLVL 0

static int readn(int fd, char *buf, int nbytes)
{
#if DEBUGLVL >= 9
    printf("readn reading %d bytes...", nbytes);
#endif

    int nleft = nbytes;
    while (nleft > 0) {
        int nread = read(fd, buf, nleft);
        if (nread < 0) {                // error?
            if (errno == EINTR)
                continue;
#if DEBUGLVL >= 9
            printf(" error\n");
#endif
            return nread;
        }
        if (nread == 0)                 // EOF?
            break;
        nleft -= nread;
        buf   += nread;
    }
    
#if DEBUGLVL >= 9
    printf(" done\n");
#endif

    return nbytes - nleft;
}
  
static int writen(int fd, char *buf, int nbytes)
{
    int nleft = nbytes;
    while (nleft > 0) {
        int nwritten = write(fd, buf, nleft);
        if (nwritten < 0) {             // error?
            if (errno == EINTR)
                continue;
            return nwritten;
        }
        nleft -= nwritten;
        buf   += nwritten;
    }

    return nbytes - nleft;
}

UnixIOTask::UnixIOTask() :
	_readAhead(false), _writeBehind(false), _readBuff(false),
        _buffPause(false), _buffPages(0), _buffNext(0),
        _totalCount(0), _buffReadCount(0), _buffWriteCount(0)
{
    _pageSize = ExtMemPool::Instance()->PageSize();
}

int UnixIOTask::Initialize()
{
    if (pipe(_reqFd) < 0) {
        perror("UnixIOTask::Initialize: pipe");
        return -1;
    }
#ifdef SOLARIS
    // Pipes are bi-directional in Solaris
    _replyFd[0] = _reqFd[1];
    _replyFd[1] = _reqFd[0];
#else
    if (pipe(_replyFd) < 0) {
        perror("UnixIOTask::Initialize: pipe");
        return -1;
    }
#endif

#if DEBUGLVL >= 1
    printf("Creating I/O task process/thread...\n");
#endif

#ifdef PROCESS_TASK
    _child = fork();
    if (_child < 0) {
        perror("UnixIOTask::Initialize: fork");
        return -1;
    }

    if (!_child) {
        (void)ProcessReq(this);
        close(_reqFd[0]);
        close(_reqFd[1]);
#ifndef SOLARIS
        close(_replyFd[0]);
        close(_replyFd[1]);
#endif
        exit(1);
    }
#endif

#ifdef THREAD_TASK
    if (pthread_create(&_child, 0, ProcessReq, this)) {
        perror("UnixIOTask::Initialize: pthread_create");
        return -1;
    }
#endif

#if DEBUGLVL >= 1
    printf("Created I/O task process/thread %ld\n", (long int)_child);
#endif

    return 0;
}

UnixIOTask::~UnixIOTask()
{
#if DEBUGLVL >= 1
    printf("Terminating child process/thread %ld...\n", (long int)_child);
#endif

    Request req = { TerminateReq, 0, 0, 0, 0 };
    if (writen(_reqFd[1], (char *)&req, sizeof req) < (int)sizeof req) {
        perror("UnixIOTask::~UnixIOTask: write");
    } else {
#ifdef PROCESS_TASK
        while(1) {
            int status;
            pid_t child = wait(&status);
            if (child < 0) {
                if (errno == EINTR)
                    continue;
                if (errno != ECHILD) {
                    perror("UnixIOTask::~UnixIOTask: wait");
                    break;
                }
            } else
                break;
        }
#endif
#ifdef THREAD_TASK
        (void)pthread_join(_child, 0);
#endif
    }
    
    close(_reqFd[0]);
    close(_reqFd[1]);
#ifndef SOLARIS
    close(_replyFd[0]);
    close(_replyFd[1]);
#endif

#if DEBUGLVL >= 1
    printf("Child process/thread terminated\n");
#endif
}

int UnixIOTask::Read(unsigned long offset,
                     unsigned long bytes,
                     char *&addr)
{
    Request req = { ReadReq, offset, bytes, addr };
    if (writen(_reqFd[1], (char *)&req, sizeof req) < (int)sizeof req) {
        perror("UnixIOTask::Read: write");
        return -1;
    }
    Request reply;
    if (readn(_replyFd[0], (char *)&reply, sizeof reply) < (int)sizeof reply) {
        perror("UnixIOTask::Read: read");
        return -1;
    }
#ifndef PIPE_DATE
    addr = reply.addr;
#endif
    if (reply.result < 0)
        return reply.result;
#ifdef PIPE_DATA
    if (readn(_replyFd[0], addr, reply.bytes) < (int)reply.bytes) {
        perror("UnixIOTask::Read: read");
        return -1;
    }
#endif
    return reply.bytes;
}

int UnixIOTask::Write(unsigned long offset,
                      unsigned long bytes,
                      char *&addr)
{
    Request req = { WriteReq, offset, bytes, addr };
    if (writen(_reqFd[1], (char *)&req, sizeof req) < (int)sizeof req) {
        perror("UnixIOTask::Write: write");
        return -1;
    }
#ifdef PIPE_DATA
    if (writen(_reqFd[1], addr, bytes) < (int)bytes) {
        perror("UnixIOTask::Write: write");
        return -1;
    }
#endif
    Request reply;
    if (readn(_replyFd[0], (char *)&reply, sizeof reply) < (int)sizeof reply) {
        perror("UnixIOTask::Write: read");
        return -1;
    }
#ifndef PIPE_DATA
    addr = reply.addr;
#endif
    if (reply.result < 0)
        return reply.result;
    return reply.bytes;
}

int UnixIOTask::SetBuffering(Boolean readAhead,
                             Boolean writeBehind,
                             unsigned int buffBytes)
{
    Request req = { SetBufferingReq, (readAhead ? 1 : 0), buffBytes,
                    (writeBehind ? (char *)1 : 0) };
    if (writen(_reqFd[1], (char *)&req, sizeof req) < (int)sizeof req) {
        perror("UnixIOTask::SetBuffering: write");
        return -1;
    }
    Request reply;
    if (readn(_replyFd[0], (char *)&reply, sizeof reply) < (int)sizeof reply) {
        perror("UnixIOTask::SetBuffering: read");
        return -1;
    }
    if (reply.result < 0)
        return reply.result;

    return 0;
}

void *UnixIOTask::ProcessReq(void *arg)
{
    UnixIOTask &me = *(UnixIOTask *)arg;
    return me.ProcessReq();
}

void *UnixIOTask::ProcessReq()
{
#if defined(PROCESS_TASK) && DEBUGLVL >= 9
    char fname[64];
    sprintf(fname, "task.fd.%d", _reqFd[0]);
    printf("Task 0x%p log file is %s\n", this, fname);
    FILE *out = freopen(fname, "w", stdout);
    if (!out)
        perror("freopen");
#endif

    while(1) {
#ifdef BUFFER
        Boolean nonBlock = (_readAhead && _readBuff &&
                            _buffer.Size() < _buffPages);
        nonBlock = (nonBlock ||
                    (_writeBehind && !_readBuff && _buffer.Size() > 0));
        nonBlock = (nonBlock && !_buffPause);
        if (nonBlock && SetNonBlockMode(_reqFd[0]) < 0)
            return (void *)-1;
#endif

        Request req;
        int status = readn(_reqFd[0], (char *)&req, sizeof req);

#ifdef BUFFER
        if (nonBlock && SetBlockMode(_reqFd[0]) < 0)
            return (void *)-1;
        if (!status) {
#if DEBUGLVL >= 5
            printf("\nNo command received, continuing buffering\n");
#endif
            if (Buffer() < 0)
                return (void *)-1;
            continue;
        }
#endif

        if (status < (int)sizeof req) {
            perror("UnixIOTask::ProcessReq: read");
            break;
        }

        // A pause in buffering ends when the next request is received
        _buffPause = false;

        if (req.type == TerminateReq) {
#if DEBUGLVL >= 1
            printf("\nTask 0x%p received quit command\n", this);
#endif
#ifdef BUFFER
            (void)BufferFlush(-1);
#endif            
            printf("I/O task: %d requests, %.2f%% read buffered, %.2f%% write buffered\n",
                   _totalCount, 100.0 * _buffReadCount / _totalCount,
                   100.0 * _buffWriteCount / _totalCount);
            break;
        }

#if DEBUGLVL >= 3
        printf("\nTask 0x%p received request: %d, %lu, %lu, 0x%p\n",
               this, req.type, req.offset, req.bytes, req.addr);
#endif

        _totalCount++;

        DOASSERT(req.bytes % _pageSize == 0, "Invalid page request");

        if (req.type == SetBufferingReq) {
            _readAhead = (req.offset ? true : false);
            _writeBehind = (req.addr ? true : false);
            _buffPages = req.bytes / _pageSize;
#if DEBUGLVL >= 1
            printf("Task 0x%p setting buffering to %d, %d, %d\n",
                   this, _readAhead, _writeBehind, _buffPages);
#endif
            int newBuffSize = 0;
            if ((_readBuff && _readAhead) || (!_readBuff && _writeBehind))
                newBuffSize = _buffPages;
            if (newBuffSize < _buffer.Size()) {
#if DEBUGLVL >= 1
                printf("Task 0x%p reducing buffer size from %d to %d\n",
                       this, _buffer.Size(), newBuffSize);
#endif
                (void)BufferFlush(_buffer.Size() - newBuffSize);
            }
            status = writen(_replyFd[1], (char *)&req, sizeof req);
            if (status < (int)sizeof req) {
                perror("UnixIOTask::ProcessReq: write");
                break;
            }
            continue;
        }

        DOASSERT(req.offset % _pageSize == 0, "Invalid page offset");

        Request reply;

#ifdef BUFFER
        // See if we need to switch from read buffering to write buffering
        if (req.type == WriteReq) {
            if (_readBuff) {
                (void)BufferFlush(-1);
                _readBuff = false;
                _buffNext = req.offset / _pageSize + 1;
#if DEBUGLVL >= 1
                printf("Task 0x%p switches to write buffer, next is %d\n",
                       this, _buffNext);
#endif
            }
        }

        // See if we need to retarget current buffering location or to
        // switch from write buffering to read buffering
        if (req.type == ReadReq) {
            if (!_readBuff) {
                (void)BufferFlush(-1);
                _readBuff = true;
                _buffNext = req.offset / _pageSize + 1;
#if DEBUGLVL >= 1
                printf("Task 0x%p switches to read buffer, next is %d\n",
                       this, _buffNext);
#endif
            }
        }

#if DEBUGLVL >= 5
        printf("Buffer: %s, %s, %s, %d total, %d next\n",
               (_readAhead ? "ON" : "OFF"), (_writeBehind ? "ON" : "OFF"),
               (_readBuff ? "read" : "write"), _buffPages, _buffNext);
#endif

        // See if requested page has been buffered (reads) or can be
        // buffered (writes)
        status = BufferCheck(req, reply);
        if (status < 0)
            break;
        if (status > 0) {
            if (_readBuff) {
#if DEBUGLVL >= 1
                printf("Task 0x%p read request satisfied from buffer\n", this);
#endif
                _buffReadCount++;
            } else {
#if DEBUGLVL >= 1
                printf("Task 0x%p write request stored in buffer\n", this);
#endif
                _buffWriteCount++;
            }
            status = writen(_replyFd[1], (char *)&reply, sizeof reply);
            if (status < (int)sizeof reply) {
                perror("UnixIOTask::ProcessReq: write");
                break;
            }
            continue;
        }
#endif

#ifdef PIPE_DATA
        char buffer[req.bytes];
        req.addr = buffer;

        if (req.type == WriteReq) {
            status = readn(_reqFd[0], req.addr, req.bytes);
            if (status < (int)req.bytes) {
                perror("UnixIOTask::ProcessReq: read");
                break;
            }
        }
#endif

        if (!req.addr) {
#if DEBUGLVL >= 3
            printf("Task 0x%p allocating cache page\n", this);
#endif
            int status = ExtMemPool::Instance()->AllocateP(ExtMemPool::Cache,
                                                           req.addr);
            if (status < 0 || !req.addr) {
                fprintf(stderr, "Failed to allocate cache space\n");
                break;
            }
        }

        DeviceIO(req, reply);

        if (req.type == WriteReq) {
            DOASSERT(req.addr == reply.addr, "Invalid page address");
            // Deallocate page since it was not buffered
            if (BufferDealloc(ExtMemPool::Cache, req.addr) < 0)
                break;
            reply.addr = 0;
        }

        status = writen(_replyFd[1], (char *)&reply, sizeof reply);
        if (status < (int)sizeof reply) {
            perror("UnixIOTask::ProcessReq: write");
            break;
        }
#ifdef PIPE_DATA
        if (req.type == ReadReq && reply.result >= 0) {
            status = writen(_replyFd[1], reply.addr, reply.bytes);
            if (status < (int)reply.bytes) {
                perror("UnixIOTask::ProcessReq: write");
                break;
            }
        }
#endif

#if DEBUGLVL >= 5
        printf("Task 0x%p request completed\n", this);
#endif
    }

    return 0;
}

int UnixIOTask::SetBlockMode(int fd)
{
  int result = fcntl(fd, F_SETFL, 0);
  if (result < 0) {
      perror("UnixIOTask::SetBlockMode: fcntl");
      return -1;
  }
  return 0;
}

int UnixIOTask::SetNonBlockMode(int fd)
{
#ifdef SUN
    int result = fcntl(fd, F_SETFL, FNDELAY);
#else
    int result = fcntl(fd, F_SETFL, O_NDELAY);
#endif
    if (result < 0) {
        perror("UnixIOTask::SetNonBlockMode: fcntl");
        return -1;
    }
    return 0;
}

#ifdef BUFFER
int UnixIOTask::Buffer()
{
    if (!_readBuff) {
        DOASSERT(_buffer.Size() > 0, "Invalid buffer");
        return BufferFlush(1);
    }

#if DEBUGLVL >= 3
    printf("Task 0x%p allocating a page for buffering\n", this);
#endif

    char *page = 0;
    int status = ExtMemPool::Instance()->AllocateP(ExtMemPool::Buffer, page);
    if (status < 0 || !page) {
#if DEBUGLVL >= 3
        printf("Task 0x%p pausing buffering because of lack of space\n",
               this);
#endif
        _buffPause = true;
        return 0;
    }

#if DEBUGLVL >= 5
    printf("Task 0x%p allocated page 0x%p for buffering\n", this, page);
#endif

    DOASSERT(_buffNext >= 0, "Invalid buffer value");

    off_t offset = _buffNext * _pageSize;
    Request ioreq = { ReadReq, offset, _pageSize, page };
    Request *newr = new Request(ioreq);
    if (!newr) {
#if DEBUGLVL >= 1
        printf("Cannot allocate memory for request\n");
#endif
        _buffPause = true;
        return -1;
    }

    Request ioreply;
    DeviceIO(ioreq, ioreply);

    // Turn buffering off if end of file reached or an error occurred
    if (ioreply.result < _pageSize) {
#if DEBUGLVL >= 1
        printf("Task 0x%p turns buffering off because of %s (%d)\n",
               this, (ioreply.result < 0 ? "error" : "EOF"),
               ioreply.result);
#endif
        delete newr;
        _buffPause = true;
        if (BufferDealloc(ExtMemPool::Buffer, page) < 0)
            return -1;
    } else {
        _buffer.Append(newr);
        _buffNext++;
    }

    return 0;
}

int UnixIOTask::BufferFlush(int num)
{
#if DEBUGLVL >= 1
    printf("Task 0x%p flushing %d buffered pages (%d)\n", this, num,
           _buffer.Size());
#endif

    int i = 0;
    int index = _buffer.InitIterator();
    while((num < 0 || i < num) && _buffer.More(index)) {
        Request *req = _buffer.Next(index);
        off_t offset = req->offset;
        char *page = req->addr;
#if DEBUGLVL >= 1
        printf("Task 0x%p flushing buffer page of offset %ld\n",
               this, offset);
#endif
        if (!_readBuff) {
            DOASSERT(req->type == WriteReq, "Invalid request in buffer");
            Request reply;
            DeviceIO(*req, reply);
            if (reply.result < (int)req->bytes)
                fprintf(stderr,
                        "Failed to write buffered page at offset %ld\n",
                        offset);
        }
        if (BufferDealloc(ExtMemPool::Buffer, page) < 0)
            fprintf(stderr, "Cannot release buffer page 0x%p\n", page);
        delete req;
        _buffer.DeleteCurrent(index);
        i++;
    }
    _buffer.DoneIterator(index);

    if (!_buffer.Size())
        _buffNext = 0;

    return 0;
}

int UnixIOTask::BufferCheck(Request &req, Request &reply)
{
#if DEBUGLVL >= 5
    printf("Task 0x%p performs buffer check for incoming request\n", this);
#endif

    if (req.type == WriteReq) {
        if (!_writeBehind)
            return 0;
        DOASSERT(!_readBuff, "Wrong buffer mode");
        if (_buffer.Size() >= _buffPages) {
#if DEBUGLVL >= 1
            printf("No more space for write buffering\n");
#endif
            return 0;
        }
        Request *newr = new Request(req);
        if (!newr) {
#if DEBUGLVL >= 1
            printf("Cannot allocate memory for request\n");
#endif
            return 0;
        }
        off_t offset = req.offset;
        char *page = req.addr;
        // See if cache page can be converted to a buffer page
        ExtMemPool::PageType ntype = ExtMemPool::Buffer;
        if (BufferConvert(page, ExtMemPool::Cache, ntype) < 0) {
            delete newr;
            return -1;
        }
        if (ntype != ExtMemPool::Buffer) {
#if DEBUGLVL >= 3
            printf("Task 0x%p unable to convert page 0x%p to buffer page\n",
                   this, page);
#endif
            delete newr;
            return 0;
        }
        // Converted, now just store page information in buffer
        _buffer.Append(newr);
        // Tell requestor that it no longer owns the page
        reply = req;
        reply.addr = 0;
        return 1;
    }

    // See if requested page is in buffer
    DOASSERT(_readBuff, "Wrong buffer mode");

    Request *oldr = 0;
    int index = _buffer.InitIterator();
    while(_buffer.More(index)) {
        oldr = _buffer.Next(index);
        _buffer.DeleteCurrent(index);
        DOASSERT(oldr, "Invalid request in buffer");
        DOASSERT((int)oldr->bytes == _pageSize, "Invalid buffer page");
        if (oldr->offset == req.offset && oldr->bytes == req.bytes)
            break;
        if (BufferDealloc(ExtMemPool::Buffer, oldr->addr) < 0) {
            delete oldr;
            _buffer.DoneIterator(index);
            return -1;
        }
        delete oldr;
        oldr = 0;
    }
    _buffer.DoneIterator(index);

    if (!oldr) {
        // Retarget next buffer page
        _buffNext = req.offset / _pageSize + 1;
#if DEBUGLVL >= 1
        printf("Task 0x%p retargets read buffer, next is %d\n",
               this, _buffNext);
#endif
        return 0;
    }

    // Deallocate caller's page if not needed anymore
    if (req.addr) {
#if DEBUGLVL >= 5
        printf("Task 0x%p deallocating buffer manager's page 0%p\n",
               this, req.addr);
#endif
        if (BufferDealloc(ExtMemPool::Cache, req.addr) < 0)
            return -1;
    }

#if DEBUGLVL >= 5
    printf("Task 0x%p converting buffer page 0x%p\n", this, oldr->addr);
#endif
    ExtMemPool::PageType ntype = ExtMemPool::Cache;
    if (BufferConvert(oldr->addr, ExtMemPool::Buffer, ntype) < 0
        || ntype != ExtMemPool::Cache)
        return -1;

    reply = req;
    reply.addr = oldr->addr;

    delete oldr;

    return 1;
}

int UnixIOTask::BufferDealloc(ExtMemPool::PageType type, char *page)
{
#if DEBUGLVL >= 5
    printf("Task 0x%p deallocating page 0x%p\n", this, page);
#endif

    int status = ExtMemPool::Instance()->DeallocateP(type, page);
    if (status < 0) {
        fprintf(stderr, "Failed to communicate with buffer pool\n");
        return -1;
    }

    return 0;
}

int UnixIOTask::BufferConvert(char *page, ExtMemPool::PageType oldType,
                                ExtMemPool::PageType &newType)
{
#if DEBUGLVL >= 5
    printf("Task 0x%p converting page 0x%p\n", this, page);
#endif

    int status = ExtMemPool::Instance()->ConvertP(page, oldType, newType);
    if (status < 0) {
        fprintf(stderr, "Failed to communicate with buffer pool\n");
        return -1;
    }

    return 0;
}
#endif

UnixFdIOTask::UnixFdIOTask(int fd) : _fd(fd)
{
    _offset = lseek(fd, SEEK_CUR, 0);
}

void UnixFdIOTask::DeviceIO(Request &req, Request &reply)
{
#if DEBUGLVL >= 3
    printf("Task 0x%p performing %s fd %d, offset %lu, %lu bytes\n",
           this, (req.type == ReadReq ? "read" : "write"), _fd,
           req.offset, req.bytes);
#endif

    reply = req;

    if (req.offset != (unsigned long)_offset) {
#if DEBUGLVL >= 9
        printf("Task 0x%p seeking to %ld\n", this, (off_t)req.offset);
#endif
        _offset = lseek(_fd, (off_t)req.offset, SEEK_SET);
        if (_offset < 0) {
            perror("UnixFdIOTask::DeviceIO: lseek");
            reply.result = _offset;
            return;
        }
    }

    switch(req.type) {
      case ReadReq:
#if DEBUGLVL >= 9
        printf("Task 0x%p reading %lu bytes\n", this, req.bytes);
#endif
        reply.result = readn(_fd, req.addr, req.bytes);
        if (reply.result < 0)
            perror("UnixFdIOTask::DeviceIO: read");
        break;

      case WriteReq:
#if DEBUGLVL >= 9
        printf("Task 0x%p writing %lu bytes\n", this, req.bytes);
#endif
        reply.result = writen(_fd, req.addr, req.bytes);
        if (reply.result < 0)
            perror("UnixFdIOTask::DeviceIO: write");
        break;

      case SetBufferingReq:
      case TerminateReq:
        break;
    }

    reply.bytes = reply.result;

    if (reply.result >= 0)
        _offset += reply.bytes;
}

UnixTapeIOTask::UnixTapeIOTask(TapeDrive &tape) : _tape(tape)
{
    _offset = _tape.tell();
}

void UnixTapeIOTask::DeviceIO(Request &req, Request &reply)
{
    reply = req;

    if (req.offset != (unsigned long)_offset) {
        _offset = _tape.seek((long)req.offset);
        if (_offset < 0) {
            reply.result = _offset;
            return;
        }
    }

    if (req.type == ReadReq) {
        reply.result = _tape.read(req.addr, req.bytes);
        if (reply.result < 0)
            perror("UnixTapeIOTask::DeviceIO: read");
    } else {
        DOASSERT(0, "Random writes not supported on tapes");
    }

    reply.bytes = reply.result;
    _offset += reply.bytes;
}

ExtMemPool *ExtMemPool::_instance = 0;

ExtMemPool::ExtMemPool(int numPages, int pageSize, int &status) :
	_numPages(numPages), _pageSize(pageSize),
        _cache(0), _buffer(0), _maxBuff(0.3)
{
    _instance = this;

    status = Initialize();
}

int ExtMemPool::Initialize()
{
    _cache = new HashTable<char *, int>(103, AddrHash);
    _buffer = new HashTable<char *, int>(103, AddrHash);
    if (!_cache || !_buffer) {
        fprintf(stderr, "Cannot allocate cache/buffer lists\n");
        return -1;
    }

    int status;
    _semKey = Semaphore::newKey();
    _sem = new Semaphore(_semKey, status, 1);
    DOASSERT(_sem, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _sem->setValue(1);

#ifdef PROCESS_TASK
    if (pipe(_reqFd) < 0) {
        perror("ExtMemPool::Initialize: pipe");
        return -1;
    }
#ifdef SOLARIS
    // Pipes are bi-directional in Solaris
    _replyFd[0] = _reqFd[1];
    _replyFd[1] = _reqFd[0];
#else
    if (pipe(_replyFd) < 0) {
        perror("ExtMemPool::Initialize: pipe");
        return -1;
    }
#endif

    _pid = getpid();
    DOASSERT(_pid >= 0, "Invalid process ID");
    status = (int)signal(SIGUSR1, ProcessReq);
    DOASSERT(status >= 0, "Cannot set interrupt handler");
#endif

#ifdef SHARED_MEMORY
    _shmKey = SharedMemory::newKey();
    int created = 0;
    _shm = new SharedMemory(_shmKey, _numPages * _pageSize, _buf, created);
    DOASSERT(_shm, "Out of memory");
    if (!created)
        printf("Warning: pre-existing shared memory initialized\n");
#if DEBUGLVL >= 1
    printf("Created a %d-byte shared memory segment at 0x%p\n",
           _numPages * _pageSize, _buf);
#endif
#else
    _buf = new char [_numPages * _pageSize];
#if DEBUGLVL >= 1
    printf("Created a %d-byte local memory area at 0x%p\n",
           _numPages * _pageSize, _buf);
#endif
#endif

    DOASSERT(_buf, "Out of memory");
    memset(_buf, 0, _numPages * _pageSize);

    for(int i = 0; i < _numPages; i++)
        _free.Append(_buf + i * _pageSize);

    return 0;
}

ExtMemPool::~ExtMemPool()
{
#ifdef PROCESS_TASK
    close(_reqFd[0]);
    close(_reqFd[1]);
#ifndef SOLARIS
    close(_replyFd[0]);
    close(_replyFd[1]);
#endif
#endif

    if (_cache->num() > 0)
        fprintf(stderr, "Warning: %d cache pages still in use\n",
                _cache->num());
    delete _cache;

    if (_buffer->num() > 0)
        fprintf(stderr, "Warning: %d buffer pages still in use\n",
                _buffer->num());
    delete _buffer;

    delete _sem;

#ifdef SHARED_MEMORY
    delete _shm;
#else
    delete _buf;
#endif
}

#ifdef PROCESS_TASK
int ExtMemPool::SendReq(Request &req, Request &reply)
{
#if DEBUGLVL >= 5
    printf("Submitting request to memory pool\n");
#endif

    AcquireMutex();

    if (writen(_reqFd[1], (char *)&req, sizeof req) < (int)sizeof req) {
        perror("ExtMemPool::SendReq: write");
        ReleaseMutex();
        return -1;
    }
    if (kill(_pid, SIGUSR1) < 0) {
        perror("ExtMemPool::SendReq: kill");
        ReleaseMutex();
        return -1;
    }
#if DEBUGLVL >= 5
    printf("Signaled memory pool (pid %ld), waiting for reply\n",
           (long)_pid);
#endif
    if (readn(_replyFd[0], (char *)&reply, sizeof reply) < (int)sizeof reply) {
        perror("ExtMemPool::SendReq: read");
        ReleaseMutex();
        return -1;
    }

    ReleaseMutex();

    return 0;
}

void ExtMemPool::ProcessReq(int caller)
{
    ExtMemPool *me = ExtMemPool::Instance();
    me->ProcessReq();
}

void ExtMemPool::ProcessReq()
{
    int status = (int)signal(SIGUSR1, ProcessReq);
    if (status < 0) {
        perror("ExtMemPool::ProcessReq: signal");
        return;
    }

    Request req;
    status = readn(_reqFd[0], (char *)&req, sizeof req);
    if (status < (int)sizeof req) {
        perror("ExtMemPool::ProcessReq: read");
        return;
    }

#if DEBUGLVL >= 1
    printf("Processing memory pool request: %d, %d, %d, 0x%p\n",
           req.type, req.ptype, req.nptype, req.addr);
#endif

    Request reply = req;

    switch(req.type) {
      case AllocateReq:
        status = _Allocate(req.ptype, reply.addr);
        break;
      case ReleaseReq:
        status = _Release(req.ptype, reply.addr);
        break;
      case DeallocateReq:
        status = _Deallocate(req.ptype, reply.addr);
        break;
      case ConvertReq:
        status = _Convert(reply.addr, req.ptype, reply.nptype);
        break;
    }

    status = writen(_replyFd[1], (char *)&reply, sizeof reply);
    if (status < (int)sizeof reply) {
        perror("ExtMemPool::ProcessReq: write");
        return;
    }
}
#endif

int ExtMemPool::_Allocate(PageType type, char *&page)
{
    if (!_free.Size()) {
#if DEBUGLVL >= 5
        printf("Out of free pages: %d cache, %d buffer\n",
               _cache->num(), _buffer->num());
#endif
        return -1;
    }

    HashTable<char *, int> *ht = _cache;
    if (type == Buffer)
        ht = _buffer;

    if (type == Buffer && _free.Size() < 4) {
        page = 0;
        return -1;
    }

    if (type == Buffer && _buffer->num() >= _maxBuff * _numPages) {
        page = 0;
        return -1;
    }

    page = _free.GetFirst();
    int status = _free.Delete(page);
    DOASSERT(status > 0, "Failed to remove free page");

    int dummy;
    status = ht->lookup(page, dummy);
    DOASSERT(status < 0, "Duplicate page in page list");
    dummy = 0;
    status = ht->insert(page, dummy);
    DOASSERT(status >= 0, "Could not insert page to list");

#if DEBUGLVL >= 3
    printf("Allocated %s page 0x%p (%d in list)\n",
           (type == Cache ? "cache" : "buffer"), page, ht->num());
#endif

    return 0;
}

int ExtMemPool::_Release(PageType type, char *&page)
{
    // Make decision whether to reclaim page or not; decision
    // based on ratio of cache/buffer pages, for example

    if (1) {
        _Deallocate(type, page);
        page = 0;
    }

    return 0;
}

int ExtMemPool::_Deallocate(PageType type, char *page)
{
    HashTable<char *, int> *ht = _cache;
    if (type == Buffer)
        ht = _buffer;

    int status = ht->remove(page);
    DOASSERT(status >= 0, "Failed to remove used page");

#if DEBUGLVL >= 3
    printf("List of %s pages now has %d entries\n",
           (type == Cache ? "cache" : "buffer"), ht->num());
#endif

    _free.Append(page);

    return 0;
}

int ExtMemPool::_Convert(char *page, PageType oldType, PageType &newType)
{
    if (oldType == newType)
        return 0;

    if (newType == Buffer && _free.Size() < 4) {
#if DEBUGLVL >= 1
        printf("Refusing to convert cache page to buffer page\n");
#endif
        newType = oldType;
        return 0;
    }

    if (oldType == Cache) {
        int status = _cache->remove(page);
        DOASSERT(status >= 0, "Failed to remove page from old list");
        int dummy = 0;
        status = _buffer->insert(page, dummy);
        DOASSERT(status >= 0, "Failed to insert page to new list");
    } else {
        int status = _buffer->remove(page);
        DOASSERT(status >= 0, "Failed to remove page from old list");
        int dummy = 0;
        status = _cache->insert(page, dummy);
        DOASSERT(status >= 0, "Failed to insert page to new list");
    }

#if DEBUGLVL >= 3
    printf("Memory pool now has %d cache pages, %d buffer pages\n",
           _cache->num(), _buffer->num());
#endif

    return 0;
}

int ExtMemPool::AddrHash(char *&addr, int numBuckets)
{
    return ((unsigned long)addr) % numBuckets;
}

ExtBufMgr::ExtBufMgr(ExtMemPool &pool) :
	_pool(pool), _numFrames(pool.NumPages()),
        _pageSize(pool.PageSize()), _ht(103, AddrHash)
{
    _frames = new PageFrame [_numFrames];
    DOASSERT(_frames, "Out of memory");

    int i;
    for(i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        frame.Clear();
        frame.page = 0;
        frame.size = 0;
    }
}

ExtBufMgr::~ExtBufMgr()
{
    // handle pinned pages

    UnPin(0, false);

    // deallocate pages and frames

    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.page)
            _pool.Deallocate(ExtMemPool::Cache, frame.page);
    }

    delete [] _frames;
}

int ExtBufMgr::PinPage(IOTask *stream, int pageNo, char *&page, Boolean read)
{
    if (!stream)
        return -1;

    int index;
    PageAddr addr = { stream, pageNo };
    int status = _ht.lookup(addr, index);

    Boolean newPage = false;

    if (status >= 0) {                  // frame found
        PageFrame &frame = _frames[index];
        DOASSERT(frame.valid, "Invalid page frame");
        frame.pinCnt++;
        if (!frame.page)
            newPage = true;
    } else {
        if ((index = AllocFrame()) < 0)
            return index;
        PageFrame &frame = _frames[index];
        newPage = true;
        if ((status = _ht.insert(addr, index)) < 0)
            return status;
        frame.Set(stream, pageNo);
    }

    PageFrame &frame = _frames[index];
    DOASSERT(frame.valid, "Invalid page frame");
    frame.refbit = true;

    // When data is piped from I/O devices, the I/O tasks will not allocate
    // memory space for us; with shared or local memory, I/O tasks allocate
    // memory, so we have to do it only we're not going to call the
    // I/O task here

#ifndef PIPE_DATA
    if (!read || !newPage) {
#endif
        if (!frame.page) {
            if ((status = _pool.Allocate(ExtMemPool::Cache, frame.page)) < 0)
                return status;
            frame.size = _pageSize;
        }
#ifndef PIPE_DATA
    }
#endif

#ifdef PIPE_DATA
    DOASSERT(frame.page, "Invalid page address");
#endif

    if (read && newPage) {
        if ((status = stream->Read(pageNo * _pageSize,
                                   _pageSize, frame.page)) < 0)
            return status;
        frame.size = status;
    }

    page = frame.page;
    DOASSERT(page, "Invalid page address");

    if (!read && newPage) {
        memset(page, 0, _pageSize);
    }

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d pinned in frame %d, addr 0x%p, pinCnt is %d\n",
           stream, pageNo, index, frame.page, frame.pinCnt);
#endif

    return frame.size;
}

int ExtBufMgr::UnPinPage(IOTask *stream, int pageNo, Boolean dirty, int size)
{
    int index;
    PageAddr addr = { stream, pageNo };
    int status = _ht.lookup(addr, index);
    if (status < 0)
        return status;

    PageFrame &frame = _frames[index];
    DOASSERT(frame.valid, "Invalid page frame");

    if (!frame.pinCnt)
        return -1;

    frame.pinCnt--;

    if (dirty)
        frame.dirty = true;

    if (size >= 0)
        frame.size = size;

    // Make decision whether to release page to memory pool or not
    if (!frame.pinCnt && !frame.dirty) {
#if DEBUGLVL >= 5
        printf("Releasing page 0x%p to memory pool\n", frame.page);
#endif
        _pool.Release(ExtMemPool::Cache, frame.page);
    }

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d unpinned from frame %d, pinCnt is %d\n",
           stream, pageNo, index, frame.pinCnt);
#endif

    return 0;
}

int ExtBufMgr::UnPin(IOTask *stream, Boolean dirty)
{
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.valid && (!stream || frame.addr.stream == stream)) {
#if DEBUGLVL >= 5
            printf("Releasing frame %d\n", i);
#endif
            int status;
            if ((status = _ht.remove(frame.addr)) < 0)
                return status;
            if (dirty)
                frame.dirty = true;
            if (frame.dirty) {
                DOASSERT(frame.page, "Invalid page address");
                status = frame.addr.stream->Write(frame.addr.pageNo *
                                                  _pageSize, frame.size,
                                                  frame.page);
                if (status < 0)
                    return status;
                if (status < _pageSize) {
                    fprintf(stderr, "Wrote %d bytes out of %d\n",
                            status, _pageSize);
                    return -1;
                }
            }
            if (frame.page)
                _pool.Release(ExtMemPool::Cache, frame.page);
            frame.Clear();
        }
    }

    return 0;
}

int ExtBufMgr::AllocFrame()
{
    int i = PickVictim();
    if (i < 0) {
        fprintf(stderr, "Could not find free frame for page\n");
        return i;
    }

    PageFrame &frame = _frames[i];
    DOASSERT(!frame.valid, "Invalid page frame");

    return i;
}

int ExtBufMgr::AddrHash(PageAddr &addr, int numBuckets)
{
    return (((long)addr.stream) + addr.pageNo) % numBuckets;
}

void ExtBufMgr::Dump()
{
    printf("Buffer frames:\n");
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        printf("%d:", i);
        if (frame.valid)
            printf(" valid, pinCnt is %d\n", frame.pinCnt);
        else
            printf(" invalid\n");
    }
}

ExtBufMgrLRU::ExtBufMgrLRU(ExtMemPool &pool) :
	ExtBufMgr(pool), _clockHand(_numFrames - 1)
{
}

int ExtBufMgrLRU::PickVictim()
{
    int freePool = _pool.FreeLeft();

    while(1) {

        // Test if the page pointed by the clockhand is pinned down. If all
        // the pages are pinned down, return error message.

        int i;
        for(i = 0; i < _numFrames; i++) {
            _clockHand = (_clockHand + 1) % _numFrames;
#if DEBUGLVL >= 9
            printf("Clock hand now at %d\n", _clockHand);
#endif
            PageFrame &frame = _frames[_clockHand];
            if (!freePool) {
                if (frame.valid && !frame.pinCnt && frame.page)
                    break;
            } else {
                if (!frame.valid || !frame.pinCnt)
                    break;
#if DEBUGLVL >= 9
                printf("Frame %d pinned with pinCnt %d\n", i, frame.pinCnt);
#endif
            }
        }

        if (i >= _numFrames)            // all pages pinned down?
            return -1;

        PageFrame &frame = _frames[_clockHand];

        if (!frame.valid)
            return _clockHand;

        if (frame.refbit) {             // page recently referenced?
            frame.refbit = false;
            continue;
        }

        // found victim page (might be dirty)

        int status = _ht.remove(frame.addr);
        if (status < 0)
            return status;

        if (frame.dirty) {              // page has unwritten data on it
            DOASSERT(frame.page, "Invalid page address");
#if DEBUGLVL >= 5
            printf("Flushing page from frame %d\n", _clockHand);
#endif
            int status = frame.addr.stream->Write(frame.addr.pageNo *
                                                  _pageSize, frame.size,
                                                  frame.page);
            if (status < 0)
                return status;
            if (status < _pageSize) {
                fprintf(stderr, "Wrote %d bytes out of %d\n",
                        status, _pageSize);
                return -1;
            }
        }

        if (frame.page)
            _pool.Release(ExtMemPool::Cache, frame.page);
        frame.Clear();

        return _clockHand;
    }
}
