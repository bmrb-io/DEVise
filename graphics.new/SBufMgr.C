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
  Revision 1.14  1996/12/18 15:29:13  jussi
  Improved destructor so that all IPC structures are removed.

  Revision 1.13  1996/12/03 20:42:16  jussi
  Moved memory management stuff to MemMgr.C. Introduced better types
  for stream position (streampos_t), byte count (bytecount_t) and
  request size (iosize_t).

  Revision 1.12  1996/11/23 21:05:00  jussi
  MemMgr now manages multi-page chunks instead of single pages.

  Revision 1.11  1996/11/15 20:33:24  jussi
  Removed MemMgr::Release() method. IOTask::Write() no longer deallocates
  the page it gets from the caller. CacheMgr dellocates pages only
  when UnPin() is called.

  Revision 1.10  1996/11/12 17:23:40  jussi
  Renamed SBufMgr class to CacheMgr and MemPool to MemMgr. This is
  to reflect the new terms (cache manager, memory manager) used in
  the documentation.

  Revision 1.9  1996/11/11 15:50:56  jussi
  Removed IOTask virtual base class and substituted UnixIOTask for
  it. When SBufMgr accesses an IOTask, no separate process is
  created. A process is created only when a ReadStream() or WriteStream()
  function of IOTask is called. In Solaris, switched to using processes
  instead of threads.

  Revision 1.8  1996/11/08 15:42:22  jussi
  Removed IOTask::Initialize() and SetBuffering(). Added support
  for streaming via ReadStream() and WriteStream().

  Revision 1.7  1996/11/07 17:36:58  jussi
  Memory pool is no longer a separate process but a reentrant
  function library.

  Revision 1.6  1996/11/04 20:21:00  jussi
  Improved support for parallel writes.

  Revision 1.5  1996/11/01 20:11:58  jussi
  MemPool object now forks a separate memory pool process. Got rid
  of the option of having processes sharing data via pipes. (Pipes
  still used for sharing control information.) SBufMgr object now
  compatible with process/shared memory environment.

  Revision 1.4  1996/10/02 21:52:36  jussi
  Switching from HTTP writing to reading is now done in
  UnixWebIOTask::DeviceIO().

  Revision 1.3  1996/10/02 19:48:32  jussi
  Added support for writing data through an HTTP connection.
  Also made it possible to use simple POST/GET transactions
  using a single IOUnixWebTask.

  Revision 1.2  1996/10/02 15:23:49  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

  Revision 1.1  1996/09/26 19:02:32  jussi
  Renamed file from ExtBufMgr -> SBufMgr. Added Web I/O task and
  changed buffer manager to allow more concurrency between I/Os.

  Revision 1.3  1996/09/07 01:43:24  jussi
  Method BufferDealloc() is used also when BUFFER is not defined.

  Revision 1.2  1996/09/07 00:03:16  jussi
  Fixed problem in UnPinPage. Pages that were still pinned (pinCnt > 0)
  were being released to the memory pool.

  Revision 1.1  1996/08/01 22:45:27  jussi
  Initial revision.
*/

#include <memory.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include "SBufMgr.h"
#include "Exit.h"
#include "DevError.h"
#include "Util.h"

#ifndef ATTRPROJ
#include "Web.h"
#endif

#ifdef SBM_PROCESS
#include <sys/wait.h>
#endif

#define DEBUGLVL 0

IOTask::IOTask(int &status, int blockSize) :
	_pageSize(MemMgr::Instance()->PageSize()), _blockSize(blockSize),
	_dataPipe(0), _readStream(false), _writeStream(false),
        _streamOffset(0), _streamLength(0), _readBytes(0),
        _writeBytes(0), _seekBytes(0), _child(0)
{
    _isBusy = new SemaphoreV(Semaphore::newKey(), status, 1);
    if (!_isBusy || status < 0) {
      fprintf(stderr, "Cannot create semaphore\n");
      status = -1;
      return;
    }
    _isBusy->setValue(1);
}

IOTask::~IOTask()
{
    if (_child) {
        fprintf(stderr, "Calling IOTask::Terminate() first\n");
        (void)Terminate();
    }

    if (!_readStream && !_writeStream) {
        printf("I/O task: read: %.2f MB, write: %.2f MB, seek: %.2f MB\n",
               _readBytes / 1048576.0, _writeBytes / 1048576.0,
               _seekBytes / 1048576.0);
    }

    delete _isBusy;
}

int IOTask::StartChild(int pipeSize)
{
    if (_dataPipe) {
      fprintf(stderr, "Data pipe exists already\n");
      return -1;
    }

    int status;
    _dataPipe = new DataPipe(pipeSize, status);
    if (!_dataPipe || status < 0) {
      fprintf(stderr, "Cannot create data pipe\n");
      return -1;
    }

#if DEBUGLVL >= 1
    printf("Creating I/O task process/thread...\n");
#endif

#ifdef SBM_PROCESS
    _child = fork();
    if (_child < 0) {
        perror("IOTask::StartChild: fork");
        return -1;
    }

    if (!_child) {
        (void)DoStream(this);
        exit(1);
    }
#endif

#ifdef SBM_THREAD
    if (pthread_create(&_child, 0, DoStream, this)) {
        perror("IOTask::StartChild: pthread_create");
        return -1;
    }
#endif

#if DEBUGLVL >= 1
    printf("Created I/O task process/thread %ld\n", (long)_child);
#endif

    return 0;
}

int IOTask::Terminate()
{
    if (!_child) {
        fprintf(stderr, "Child process/thread no longer exists\n");
        return 0;
    }

#if DEBUGLVL >= 1
    printf("Terminating child process/thread %ld...\n", (long)_child);
#endif

#ifdef SBM_PROCESS
    while(1) {
        int status;
        pid_t child = wait(&status);
        if (child < 0) {
            if (errno == EINTR)
                continue;
            if (errno != ECHILD) {
                perror("IOTask::WaitForChild: wait");
                break;
            }
        } else
            break;
    }
#endif
#ifdef SBM_THREAD
    (void)pthread_join(_child, 0);
#endif

    _child = 0;

    delete _dataPipe;
    _dataPipe = 0;

#if DEBUGLVL >= 1
    printf("Child process/thread terminated\n");
#endif

    return 0;
}
    
int IOTask::Read(streampos_t offset, iosize_t bytes, char *&addr)
{
    if (!addr) {
#if DEBUGLVL >= 3
        printf("Task 0x%p allocating cache page\n", this);
#endif
        int status = MemMgr::Instance()->Allocate(MemMgr::Cache, addr);
        if (status < 0 || !addr) {
            fprintf(stderr, "Failed to allocate cache space\n");
            return -1;
        }
    }

    Request req = { ReadReq, offset, bytes, addr };
    Request reply;

    SetDeviceBusy();
    DeviceIO(req, reply);
    SetDeviceIdle();

    if (reply.result < 0)
        return reply.result;

    addr = reply.addr;

    return reply.bytes;
}

int IOTask::Write(streampos_t offset, iosize_t bytes, char *addr)
{
    if (!addr)
        return -1;

    Request req = { WriteReq, offset, bytes, addr };
    Request reply;

    SetDeviceBusy();
    DeviceIO(req, reply);
    SetDeviceIdle();

    if (reply.result < 0)
        return reply.result;

    return reply.bytes;
}

int IOTask::ReadStream(streampos_t offset, iosize_t bytes, int pipeSize)
{
    if (_readStream || _writeStream) {
        fprintf(stderr, "Already participating in a stream\n");
        return -1;
    }

    _streamOffset = offset;
    _streamLength = bytes;
    _readStream = true;

    if (StartChild(pipeSize) < 0)
        return -1;

    return 0;
}

int IOTask::WriteStream(int pipeSize)
{
    if (_readStream || _writeStream) {
        fprintf(stderr, "Already participating in a stream\n");
        return -1;
    }

    _writeStream = true;

    if (StartChild(pipeSize) < 0)
        return -1;

    return 0;
}

void *IOTask::DoStream(void *arg)
{
    IOTask &me = *(IOTask *)arg;
    return me.DoStream();
}

void *IOTask::DoStream()
{
#ifdef SBM_THREAD
    // Find out the priority of this thread
    int policy;
    struct sched_param sparam;
    if (pthread_getschedparam(pthread_self(), &policy, &sparam) < 0) {
        perror("pthread_getschedparam");
        exit(1);
    }
#if DEBUGLVL >= 5
    printf("IOTask::DoStream: Priority %d\n", sparam.sched_priority);
#endif
#endif

    if (_readStream)
        _ReadStream(_streamOffset, _streamLength);
    else
        _WriteStream();

    printf("I/O task: read: %.2f MB, write: %.2f MB, seek: %.2f MB\n",
           _readBytes / 1048576.0, _writeBytes / 1048576.0,
           _seekBytes / 1048576.0);

    return 0;
}

void IOTask::_ReadStream(streampos_t offset, iosize_t totbytes)
{
    SetDeviceBusy();

    iosize_t bytes = 0;

    while (!totbytes || bytes < totbytes) {
        char *page;
        int status = MemMgr::Instance()->Allocate(MemMgr::Buffer, page);
        DOASSERT(status >= 0 && page, "Failed to allocate buffer space\n");
        iosize_t reqsize = _pageSize;
        if (totbytes > 0 && totbytes - bytes < reqsize)
            reqsize = totbytes - bytes;
        Request req = { ReadReq, offset, reqsize, page };
        Request reply;
        DeviceIO(req, reply);
        if (reply.result >= 0) {
            int status = Produce(page, offset, reply.result);
            DOASSERT(status >= 0, "Cannot produce data");
        }
        if (reply.result < _pageSize)
            break;
        offset += _pageSize;
        bytes += _pageSize;
    }

    int status = Produce(0, offset, 0);
    DOASSERT(status >= 0, "Cannot produce data");

    SetDeviceIdle();
}

void IOTask::_WriteStream()
{
    SetDeviceBusy();

    while(1) {
        char *page;
        streampos_t offset;
        iosize_t bytes;
        int status = Consume(page, offset, bytes);
        DOASSERT(status >= 0, "Cannot consume data");
        if (!bytes)
            break;
        Request req = { WriteReq, offset, bytes, page };
        Request reply;
        DeviceIO(req, reply);
        if (reply.result != (int)bytes)
            break;
        status = MemMgr::Instance()->Deallocate(MemMgr::Buffer, page);
        DOASSERT(status >= 0 && page, "Failed to deallocate buffer space\n");
        offset += bytes;
    }

    (void)WriteEOF();

    SetDeviceIdle();
}

Boolean IOTask::IsBusy()
{
    int status = _isBusy->test();
    DOASSERT(status >= 0, "Unable to test semaphore");
    if (!status)
        return true;
    _isBusy->release();
    return false;
}

FdIOTask::FdIOTask(int &status, int fd, int blockSize) :
	IOTask(status, blockSize), _fd(fd)
{
    if (status < 0)
      return;
    
    if (fd >= 0) {
        _offset = lseek(fd, SEEK_CUR, 0);
        status = (int)_offset;
    } else
        status = fd;
}

void FdIOTask::DeviceIO(Request &req, Request &reply)
{
#if DEBUGLVL >= 3
    printf("Task 0x%p performing %s fd %d, offset %llu, %lu bytes\n",
           this, (req.type == ReadReq ? "read" : "write"), _fd,
           req.offset, req.bytes);
#endif

    reply = req;

    if (req.offset != _offset) {
#if DEBUGLVL >= 9
        printf("Task 0x%p seeking to %llu\n", this, req.offset);
#endif
        off_t offset = lseek(_fd, (off_t)req.offset, SEEK_SET);
        if (offset < 0) {
            perror("FdIOTask::DeviceIO: lseek");
            reply.result = offset;
            return;
        }
        // cannot use abs() because these are unsigned numbers
        if (req.offset > _offset)
            _seekBytes += req.offset - _offset;
        else
            _seekBytes += _offset - req.offset;
        _offset = offset;
    }

    switch(req.type) {
      case ReadReq:
#if DEBUGLVL >= 9
        printf("Task 0x%p reading %lu bytes\n", this, req.bytes);
#endif
        reply.result = readn(_fd, req.addr, req.bytes);
        if (reply.result < 0) {
            printf("Cannot read to address 0x%p (%d)\n", req.addr, errno);
            perror("FdIOTask::DeviceIO: read");
        }
        else
            _readBytes += reply.result;
        break;

      case WriteReq:
#if DEBUGLVL >= 9
        printf("Task 0x%p writing %lu bytes\n", this, req.bytes);
#endif
        reply.result = writen(_fd, req.addr, req.bytes);
        if (reply.result < 0)
            perror("FdIOTask::DeviceIO: write");
        else
            _writeBytes += reply.result;
        break;
    }

    reply.bytes = reply.result;

    if (reply.result >= 0)
        _offset += reply.bytes;
}

#ifndef ATTRPROJ
WebIOTask::WebIOTask(int &status, char *url, Boolean isInput, int blockSize) :
	FdIOTask(status, OpenURL(url, isInput), blockSize)
{
}

WebIOTask::~WebIOTask()
{
    // Write cached HTTP file to Web site
    if (!_isInput && _url && !strncmp(_url, "http://", 7))
        WriteHTTP();

    if (_cache) {
#if DEBUGLVL >= 3
        printf("Closing cache file %s\n", _cacheName);
#endif
        fclose(_cache);
        if (_webfd >= 0) {
#if DEBUGLVL >= 3
            printf("Web connection open, removing cache file\n");
#endif
            unlink(_cacheName);
        }
    }

    if (_webfd >= 0) {
#if DEBUGLVL >= 3
        printf("Closing Web connection to %s\n", _url);
#endif
        close(_webfd);
    }

    delete _url;
    delete _cacheName;
}

int WebIOTask::WriteEOF()
{
    if (_isInput || !_url || strncmp(_url, "http://", 7))
        return 0;

    DOASSERT(_webfd < 0 && _cache, "Invalid parameters");

    // Write cached HTTP file to Web site, then reverse direction
    // of data flow
    WriteHTTP();

    if (_webfd < 0)
        return -1;

    return 0;
}

int WebIOTask::OpenURL(char *url, Boolean isInput)
{
    _isInput = isInput;
    _webfd = -1;
    _url = 0;
    _cache = 0;
    _cacheName = 0;
    _cacheSize = 0;

    _url = strdup(url);
    if (!_url)
        return -1;

    // Writing to an FTP site?
    if (!isInput && !strncmp(_url, "ftp://", 6)) {
        _webfd = open_ftp(_url, isInput);
        if (_webfd < 0) {
            fprintf(stderr, "Cannot write to %s\n", _url);
            perror("connect");
        }
        return _webfd;
    }

    _cacheName = MakeCacheName(_url);
    if (!_cacheName)
        return -1;

    // Reading from FTP or HTTP?
    if (isInput) {
        _cache = fopen(_cacheName, "r");
        if (_cache) {
            // Cache file exists -- assume that we don't need to fetch
            // more data from the URL
            printf("Using existing cache file %s\n", _cacheName);
            return fileno(_cache);
        }

        size_t totlen = 0;
        if (!strncmp(_url, "ftp://", 6))
            _webfd = open_ftp(_url, isInput);
        else
            _webfd = open_http(_url, isInput, &totlen);
        if (_webfd < 0) {
            fprintf(stderr, "Cannot read from %s\n", _url);
            perror("connect");
            return -1;
        }
    }

    _cache = fopen(_cacheName, "w+");
    if (!_cache) {
        fprintf(stderr, "Could not create cache file %s\n", _cacheName);
        perror("fopen");
        close(_webfd);
        _webfd = -1;
        return -1;
    }

    return fileno(_cache);
}

char *WebIOTask::MakeCacheName(char *url)
{
    unsigned long sum = 0;
    unsigned long xor = 0;
    for(unsigned int i = 0; i < strlen(url); i++) {
        sum += url[i];
        xor ^= url[i];
    }

    const char *prefix = "/tmp/webcache.";
    char *name = new char [strlen(prefix) + 4 + 4 + 1];
    if (!name)
        return name;

    sprintf(name, "%s%04lx%04lx", prefix, sum, xor);
    return name;
}

void WebIOTask::WriteHTTP()
{
#if DEBUGLVL >= 3
    printf("Writing cached HTTP file (%llu bytes) to %s\n",
           _cacheSize, _url);
#endif

    DOASSERT(!_isInput && _webfd < 0 && _cache, "Invalid parameters");

    size_t totlen = _cacheSize;
    _webfd = open_http(_url, _isInput, &totlen);
    if (_webfd < 0) {
        fprintf(stderr, "Cannot write to %s\n", _url);
        perror("connect");
        return;
    }

    iosize_t sent = 0;

    while (sent < _cacheSize) {
        if (_offset != sent) {
#if DEBUGLVL >= 9
            printf("Seeking to offset %lu in cache file\n", sent);
#endif
            if (fseek(_cache, sent, SEEK_SET) < 0) {
                fprintf(stderr, "Cannot seek to end of cache file\n");
                perror("fseek");
                break;
            }
            _offset = sent;
        }
        char buffer[2048];
        int len = fread(buffer, 1, sizeof buffer, _cache);
#if DEBUGLVL >= 9
        printf("Read %d/%ld bytes from cache file\n", len,
               _cacheSize - sent);
#endif
        if (len == 0 && feof(_cache)) {
#if DEBUGLVL >= 3
            printf("Web transfer to %s complete\n", _url);
#endif
            break;
        }
        if (len == 0) {
            fprintf(stderr, "Cannot read from cache file\n");
            perror("read");
            break;
        }
        if (write(_webfd, buffer, len) != len) {
            fprintf(stderr, "Cannot write to Web\n");
            perror("fwrite");
            break;
        }
        _offset += len;
        sent += len;
    }
}

void WebIOTask::DeviceIO(Request &req, Request &reply)
{
#if DEBUGLVL >= 3
    printf("Task 0x%p performing %s fd %d, offset %llu, %lu bytes\n",
           this, (req.type == ReadReq ? "read" : "write"), _fd,
           req.offset, req.bytes);
#endif

    // If someone tries to read from an output stream, attempt
    // to straighten out situation.
    if (!_isInput && req.type == ReadReq) {
        _isInput = true;
        _cacheSize = 0;
        _cache = freopen(_cacheName, "w+", _cache);
        if (!_cache) {
            fprintf(stderr, "Could not truncate cache file %s\n", _cacheName);
            perror("freopen");
            close(_webfd);
            reply.result = _webfd = -1;
            return;
        }
    }

    // Writing to an FTP or HTTP site?
    if (req.type == WriteReq) {
        DOASSERT(!_isInput, "Inconsistent I/O request");
        // Write to FTP site or HTTP file cache
        FdIOTask::DeviceIO(req, reply);
        if (reply.bytes > 0)
            _cacheSize += reply.bytes;
        return;
    }

    DOASSERT(_isInput, "Inconsistent I/O request");

    // If Web connection still open...
    if (_webfd >= 0) {
        // Wait until enough data has been received
        while (req.offset + req.bytes > _cacheSize) {
            char buffer[2048];
            int len = read(_webfd, buffer, sizeof buffer);
#if DEBUGLVL >= 9
            printf("Read %d/%llu bytes from %s\n", len,
                   req.offset + req.bytes - _cacheSize, _url);
#endif
            if (len == 0) {
#if DEBUGLVL >= 3
                printf("Web transfer from %s complete\n", _url);
#endif
                close(_webfd);
                _webfd = -1;
                break;
            }
            if (len < 0) {
                fprintf(stderr, "Cannot read from Web\n");
                perror("read");
                close(_webfd);
                _webfd = -1;
                break;
            }
            if (_offset != _cacheSize) {
#if DEBUGLVL >= 9
                printf("Seeking to offset %lu in cache file\n", _cacheSize);
#endif
                if (fseek(_cache, _cacheSize, SEEK_SET) < 0) {
                    fprintf(stderr, "Cannot seek to end of cache file\n");
                    perror("fseek");
                    break;
                }
                _offset = _cacheSize;
            }
            if (fwrite(buffer, len, 1, _cache) != 1) {
                reportErrSys("Cannot write to cache file");
                break;
            }
            _offset += len;
            _cacheSize += len;
        }
        fflush(_cache);
    }

    // Perform I/O from cache file
    FdIOTask::DeviceIO(req, reply);
}

TapeIOTask::TapeIOTask(int &status, TapeDrive &tape) :
	IOTask(status, tape.getBlockSize()), _tape(tape)
{
    if (status < 0)
      return;
    _offset = _tape.tell();
}

void TapeIOTask::DeviceIO(Request &req, Request &reply)
{
    reply = req;

    if (req.offset != _offset) {
        DOASSERT(req.type == ReadReq, "Random writes not supported on tapes");
        long long offset = _tape.seek((long long)req.offset);
        if (offset < 0) {
            perror("TapeIOTask::DeviceIO: seek");
            reply.result = offset;
            return;
        }
        // cannot use abs() because these are unsigned numbers
        if (req.offset > _offset)
            _seekBytes += req.offset - _offset;
        else
            _seekBytes += _offset - req.offset;
        _offset = offset;
    }

    if (req.type == ReadReq) {
        reply.result = _tape.read(req.addr, req.bytes);
        if (reply.result < 0)
            perror("TapeIOTask::DeviceIO: read");
        else
            _readBytes += reply.result;
    } else {
        reply.result = _tape.append(req.addr, req.bytes);
        if (reply.result < 0)
            perror("TapeIOTask::DeviceIO: append");
        else
            _writeBytes += reply.result;
    }

    reply.bytes = reply.result;
    _offset += reply.bytes;
}

CacheMgr *CacheMgr::_instance = 0;

CacheMgr::CacheMgr(MemMgr &mgr, int frames, int &status) :
	_mgr(mgr), _numFrames(frames),
        _pageSize(mgr.PageSize())
#ifndef SBM_SHARED_MEMORY
        , _ht(103, AddrHash)
#endif
{
    _instance = this;

    status = Initialize();
}

int CacheMgr::Initialize()
{
    int status;
    _mutex = new SemaphoreV(Semaphore::newKey(), status);
    if (!_mutex || status < 0) {
      fprintf(stderr, "Cannot create semaphore\n");
      return -1;
    }
    _mutex->setValue(1);

#ifdef SBM_SHARED_MEMORY
    printf("Creating buffer manager frame table in shared memory\n");
    key_t _frmShmKey = SharedMemory::newKey();
    int created = 0;
    char *buf;
    _frmShm = new SharedMemory(_frmShmKey, _numFrames * sizeof(PageFrame),
                               buf, created);
    if (!_frmShm) {
      fprintf(stderr, "Cannot create page table in shared memory\n");
      delete _mutex;
      return -1;
    }
    if (!created)
        printf("Warning: pre-existing shared memory initialized\n");
#if DEBUGLVL >= 1
    printf("Created a %d-byte shared memory segment at 0x%p\n",
           _numFrames * sizeof(PageFrame), buf);
#endif
    _frames = (PageFrame *)buf;
#else
    printf("Creating buffer manager frame table in local memory\n");
    _frames = new PageFrame [_numFrames];
    if (!_frames) {
      fprintf(stderr, "Cannot create page table in local memory\n");
      delete _mutex;
      return -1;
    }
#endif
 
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        frame.Clear();
        frame.page = 0;
        frame.size = 0;
    }

    return 0;
}

CacheMgr::~CacheMgr()
{
    _DeallocMemory();

#ifdef SBM_SHARED_MEMORY
    delete _frmShm;
#else
    delete [] _frames;
#endif

    delete _mutex;
}

void CacheMgr::_DeallocMemory()
{
    AcquireMutex();

    // deallocate pages and frames

    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.page)
            _mgr.Deallocate(MemMgr::Cache, frame.page);
    }

    ReleaseMutex();
}

int CacheMgr::_PinPage(IOTask *stream, int pageNo, char *&page, Boolean read)
{
    if (!stream) {
        printf("No stream\n");
        return -1;
    }

    int index;
    PageAddr addr = { stream, pageNo };
#ifdef SBM_SHARED_MEMORY
    for(index = 0; index < _numFrames; index++) {
        PageFrame &frame = _frames[index];
        if (frame.valid && frame.addr == addr)
            break;
    }
    int status = (index < _numFrames ? 0 : -1);
#else
    int status = _ht.lookup(addr, index);
#endif

    if (status >= 0) {                  // frame found
        PageFrame &frame = _frames[index];
        DOASSERT(frame.valid, "Invalid page frame");
        frame.pinCnt++;
    } else {
        if ((index = AllocFrame()) < 0) {
            printf("No free frame\n");
            return index;
        }
        PageFrame &frame = _frames[index];
#ifndef SBM_SHARED_MEMORY
        if ((status = _ht.insert(addr, index)) < 0) {
            fprintf(stderr, "Cannot insert to hash table\n");
            return status;
        }
#endif
        frame.Set(stream, pageNo);
    }

    PageFrame &frame = _frames[index];
    DOASSERT(frame.valid, "Invalid page frame");
    frame.refbit = true;

    if (!frame.page) {
        if ((status = _mgr.Allocate(MemMgr::Cache, frame.page)) < 0) {
            printf("Cannot get free page from manager\n");
            return status;
        }
        frame.size = _pageSize;
    } else {
#if DEBUGLVL >= 3
        printf("Frame %d has page 0x%p\n", index, frame.page);
#endif
    }

    page = frame.page;
    DOASSERT(page, "Invalid page address");

    if (read) {
        frame.iopending = true;
        ReleaseMutex();
        if ((status = stream->Read(((streampos_t)pageNo) *
                                   ((streampos_t)_pageSize),
                                   _pageSize, frame.page)) < 0) {
            printf("Cannot read page\n");
            return status;
        }
        AcquireMutex();
        frame.iopending = false;
        frame.size = status;
    } else {
        memset(frame.page, 0, _pageSize);
    }

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d pinned in frame %d, addr 0x%p, pinCnt is %d\n",
           stream, pageNo, index, frame.page, frame.pinCnt);
#endif

    return frame.size;
}

int CacheMgr::_UnPinPage(IOTask *stream, int pageNo, Boolean dirty,
                        int size, Boolean force)
{
    if (!stream) {
        printf("No stream\n");
        return -1;
    }

    int index;
    PageAddr addr = { stream, pageNo };
#ifdef SBM_SHARED_MEMORY
    for(index = 0; index < _numFrames; index++) {
        PageFrame &frame = _frames[index];
        if (frame.valid && frame.addr == addr)
            break;
    }
    int status = (index < _numFrames ? 0 : -1);
#else
    int status = _ht.lookup(addr, index);
#endif
    if (status < 0) {
        printf("Page not pinned\n");
        return status;
    }

    PageFrame &frame = _frames[index];
    DOASSERT(frame.valid, "Invalid page frame");

    if (!frame.pinCnt) {
        printf("Page pin count zero\n");
        return -1;
    }

    if (dirty)
        frame.dirty = true;

    if (size >= 0)
        frame.size = size;

    // See if this is the last pin of this page

    if (frame.pinCnt == 1) {
        DOASSERT(frame.page, "Invalid page address");
        if (frame.dirty && force) {
#if DEBUGLVL >= 5
            printf("Forcing page %ld to device\n", frame.addr.pageNo);
#endif
            frame.iopending = true;
            ReleaseMutex();
            // Write dirty pages if forced
            int status = frame.addr.stream->Write(
                            ((streampos_t)frame.addr.pageNo) *
                            ((streampos_t)_pageSize),
                                                  frame.size, frame.page);
            if (status < 0)
                return status;
            if (status < _pageSize) {
                fprintf(stderr, "Wrote %d bytes out of %d\n",
                        status, _pageSize);
                return -1;
            }
            AcquireMutex();
            frame.iopending = false;
            frame.dirty = false;
        }
    }

    frame.pinCnt--;

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d unpinned from frame %d, pinCnt is %d\n",
           stream, pageNo, index, frame.pinCnt);
#endif

    return 0;
}

int CacheMgr::_UnPin(IOTask *stream, Boolean dirty)
{
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.valid && (!stream || frame.addr.stream == stream)) {
#if DEBUGLVL >= 5
            printf("Releasing frame %d\n", i);
#endif
            int status;
#ifndef SBM_SHARED_MEMORY
            if ((status = _ht.remove(frame.addr)) < 0)
                return status;
#endif
            if (frame.dirty || dirty) {
#if DEBUGLVL >= 5
                printf("Flushing page %ld to device\n", frame.addr.pageNo);
#endif
                DOASSERT(frame.page, "Invalid page address");
                frame.iopending = true;
                ReleaseMutex();
                status = frame.addr.stream->Write(
                            ((streampos_t)frame.addr.pageNo) *
                            ((streampos_t)_pageSize),
                                                  frame.size, frame.page);
                if (status < 0)
                    return status;
                if (status < _pageSize) {
                    fprintf(stderr, "Wrote %d bytes out of %d\n",
                            status, _pageSize);
                    return -1;
                }
                AcquireMutex();
                frame.iopending = false;
                frame.dirty = false;
            }
            if ((status = _mgr.Deallocate(MemMgr::Cache, frame.page)) < 0)
                return status;
            frame.Clear();
            frame.page = 0;
            frame.size = 0;
        }
    }

    return 0;
}

int CacheMgr::AllocFrame()
{
    int i = PickVictim();
    if (i < 0) {
        fprintf(stderr, "Could not find free frame for page\n");
        return i;
    }

    PageFrame &frame = _frames[i];
    if (frame.valid && frame.dirty)
        Dump();
    DOASSERT(!frame.valid || !frame.dirty, "Invalid page frame");

    return i;
}

int CacheMgr::AddrHash(PageAddr &addr, int numBuckets)
{
    return (((long)addr.stream) + addr.pageNo) % numBuckets;
}

void CacheMgr::Dump()
{
    printf("Buffer frames:\n");
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        printf("%d:", i);
        if (frame.valid)
            printf(" valid, I/O task 0x%p, page %ld, pinCnt is %d\n",
                   frame.addr.stream, frame.addr.pageNo, frame.pinCnt);
        else
            printf(" invalid\n");
    }
}

CacheMgrLRU::CacheMgrLRU(MemMgr &mgr, int frames, int &status) :
	CacheMgr(mgr, frames, status), _clockHand(_numFrames - 1)
{
}

int CacheMgrLRU::PickVictim()
{
    int freeMgr = _mgr.NumFree();

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
            // If no pages available in memory manager, reuse pages
            // in frames that are not pinned; otherwise find an
            // unused frame and allocate page from memory manager.
            if (!freeMgr) {
                if (frame.valid && !frame.pinCnt && frame.page
                    && !frame.iopending)
                    break;
            } else {
                if (!frame.valid || (!frame.pinCnt && !frame.iopending))
                    break;
#if DEBUGLVL >= 9
                printf("Frame %d pinned with pinCnt %d\n", i, frame.pinCnt);
#endif
            }
        }

        if (i >= _numFrames)            // all pages pinned down?
            return -1;

        int selectedFrame = _clockHand;

        PageFrame &frame = _frames[selectedFrame];

        if (!frame.valid)
            return selectedFrame;

        if (frame.refbit) {             // page recently referenced?
            frame.refbit = false;
            continue;
        }

        // found victim page (might be dirty)

#ifndef SBM_SHARED_MEMORY
        int status = _ht.remove(frame.addr);
        if (status < 0)
            return status;
#endif

        int status;

        if (frame.dirty) {              // page has unwritten data on it
            DOASSERT(frame.page, "Invalid page address");
#if DEBUGLVL >= 5
            printf("Flushing page %ld from frame %d\n", frame.addr.pageNo,
                   selectedFrame);
#endif
            frame.iopending = true;
            ReleaseMutex();
            status = frame.addr.stream->Write(
                            ((streampos_t)frame.addr.pageNo) *
                            ((streampos_t)_pageSize),
                                              frame.size, frame.page);
            if (status < 0)
                return status;
            if (status < frame.size) {
                fprintf(stderr, "Wrote %d bytes out of %d\n",
                        status, frame.size);
                return -1;
            }
            AcquireMutex();
            frame.iopending = false;
            frame.dirty = false;
#if DEBUGLVL >= 5
            printf("Flushed page %ld from frame %d\n", frame.addr.pageNo,
                   selectedFrame);
#endif
        }

        return selectedFrame;
    }
}
#endif
