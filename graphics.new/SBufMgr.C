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
#include "Web.h"
#include "Exit.h"
#include "DevError.h"

#ifdef PROCESS_TASK
#include <sys/wait.h>
#endif

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

DataPipe::DataPipe(int maxPages) : _maxPages(maxPages)
{
    if (_maxPages < 2) {
        _maxPages = 2;
        fprintf(stderr, "Adjusting pipe size to %d\n", _maxPages);
    }

#if DEBUGLVL >= 1
    printf("Creating new data pipe, size %d\n", _maxPages);
#endif

    int status;
    _sem = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_sem, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _sem->setValue(1);

    _free = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_free, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _free->setValue(0);

    _data = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_data, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _data->setValue(0);

    int size = _maxPages * (sizeof(char *) + sizeof(int)) + 3 * sizeof(int);
    char *buf = 0;

#ifdef SHARED_MEMORY
    key_t _shmKey = SharedMemory::newKey();
    int created = 0;
    _shm = new SharedMemory(_shmKey, size, buf, created);
    DOASSERT(_shm, "Out of memory");
    if (!created)
        printf("Warning: pre-existing shared memory initialized\n");
#if DEBUGLVL >= 1
    printf("Created a %d-byte shared memory segment (key %d) at 0x%p\n",
           size, _shmKey, buf);
#endif
#else
    buf = new char [size];
#if DEBUGLVL >= 1
    printf("Created a %d-byte local memory area at 0x%p\n", size, buf);
#endif
#endif

    DOASSERT(buf, "Out of memory");

    _streamData = (char **)buf;
    _streamBytes = (int *)(buf + _maxPages * sizeof(char *));
    _streamHead = _streamBytes + _maxPages;
    _streamTail = _streamHead + 1;
    _streamFree = _streamTail + 1;

    for(int i = 0; i < _maxPages; i++) {
        _streamData[i] = 0;
        _streamBytes[i] = 0;
    }
    *_streamHead = *_streamTail = 0;
    *_streamFree = _maxPages;
}

DataPipe::~DataPipe()
{
#ifdef SHARED_MEMORY
    delete _shm;
#else
    delete _streamData;
#endif

    _data->destroy();
    delete _data;

    _free->destroy();
    delete _free;

    _sem->destroy();
    delete _sem;
}

int DataPipe::Produce(char *buf, int bytes)
{
    AcquireMutex();

    while (!*_streamFree) {
#if DEBUGLVL >= 3
        printf("Pipe 0x%p has to wait for consumer's free space\n", this);
#endif
        ReleaseMutex();
        AcquireFree();
        AcquireMutex();
    }

    (*_streamFree)--;

#if DEBUGLVL >= 3
    printf("Pipe 0x%p has %d free space left\n", this, *_streamFree);
#endif

    _streamData[*_streamHead] = buf;
    _streamBytes[*_streamHead] = bytes;
    *_streamHead = (*_streamHead + 1) % _maxPages;
    if (*_streamFree == _maxPages - 1) {
#if DEBUGLVL >= 3
        printf("Pipe 0x%p signaling consumer about data\n", this);
#endif
        ReleaseData();
    }

    ReleaseMutex();

    return 0;
}

int DataPipe::Consume(char *&buf)
{
    AcquireMutex();

    while (*_streamFree == _maxPages) {
#if DEBUGLVL >= 3
        printf("Pipe 0x%p has to wait for producer's data\n", this);
#endif
        ReleaseMutex();
        AcquireData();
        AcquireMutex();
    }

    buf = _streamData[*_streamTail];
    int bytes = _streamBytes[*_streamTail];
    *_streamTail = (*_streamTail + 1) % _maxPages;

    (*_streamFree)++;
#if DEBUGLVL >= 3
    printf("Pipe 0x%p has %d free space left\n", this, *_streamFree);
#endif

    if (*_streamFree == 1) {
#if DEBUGLVL >= 3
        printf("Pipe 0x%p signaling producer about free space\n", this);
#endif
        ReleaseFree();
    }

    ReleaseMutex();

    return bytes;
}

IOTask::IOTask(int blockSize) :
	_pageSize(MemPool::Instance()->PageSize()), _blockSize(blockSize),
	_dataPipe(0), _readStream(false), _writeStream(false),
        _readStreamLength(0), _readBytes(0), _writeBytes(0),
        _seekBytes(0), _child(0)
{
    int status;
    _isBusy = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_isBusy, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
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

    _isBusy->destroy();
    delete _isBusy;
}

int IOTask::StartChild(int pipeSize)
{
    DOASSERT(!_dataPipe, "Data pipe exists already");

    _dataPipe = new DataPipe(pipeSize);
    DOASSERT(_dataPipe, "Invalid data pipe");

#if DEBUGLVL >= 1
    printf("Creating I/O task process/thread...\n");
#endif

#ifdef PROCESS_TASK
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

#ifdef THREAD_TASK
    if (pthread_create(&_child, 0, DoStream, this)) {
        perror("IOTask::StartChild: pthread_create");
        return -1;
    }
#endif

#if DEBUGLVL >= 1
    printf("Created I/O task process/thread %ld\n", (long int)_child);
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
    printf("Terminating child process/thread %ld...\n", (long int)_child);
#endif

#ifdef PROCESS_TASK
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
#ifdef THREAD_TASK
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
    
int IOTask::Read(unsigned long long offset,
                     unsigned long bytes,
                     char *&addr)
{
    if (!addr) {
#if DEBUGLVL >= 3
        printf("Task 0x%p allocating cache page\n", this);
#endif
        int status = MemPool::Instance()->Allocate(MemPool::Cache, addr);
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

int IOTask::Write(unsigned long long offset,
                      unsigned long bytes,
                      char *&addr)
{
    Request req = { WriteReq, offset, bytes, addr };
    Request reply;

    SetDeviceBusy();
    DeviceIO(req, reply);
    SetDeviceIdle();

    if (reply.result < 0)
        return reply.result;

    DOASSERT(req.addr == reply.addr, "Invalid page address 1");
    // Deallocate page since it was not buffered
    if (MemPool::Instance()->Deallocate(MemPool::Cache, req.addr) < 0)
        return -1;

    addr = reply.addr = 0;

    return reply.bytes;
}

int IOTask::ReadStream(unsigned long bytes, int pipeSize)
{
    if (_readStream || _writeStream) {
        fprintf(stderr, "Already participating in one stream\n");
        return -1;
    }

    _readStreamLength = bytes;
    _readStream = true;

    if (StartChild(pipeSize) < 0)
        return -1;

    return 0;
}

int IOTask::WriteStream(int pipeSize)
{
    if (_readStream || _writeStream) {
        fprintf(stderr, "Already participating in one stream\n");
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
#ifdef THREAD_TASK
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
        _ReadStream(_readStreamLength);
    else
        _WriteStream();

    printf("I/O task: read: %.2f MB, write: %.2f MB, seek: %.2f MB\n",
           _readBytes / 1048576.0, _writeBytes / 1048576.0,
           _seekBytes / 1048576.0);

    return 0;
}

void IOTask::_ReadStream(unsigned long totbytes)
{
    SetDeviceBusy();

    unsigned long long offset = 0;

    while (!totbytes || offset < totbytes) {
        char *page;
        int status = MemPool::Instance()->Allocate(MemPool::Buffer, page);
        DOASSERT(status >= 0 && page, "Failed to allocate buffer space\n");
        unsigned long reqsize = _pageSize;
        if (totbytes > 0 && totbytes - offset < reqsize)
            reqsize = totbytes - offset;
        Request req = { ReadReq, offset, reqsize, page };
        Request reply;
#if 1
        DeviceIO(req, reply);
#else
        reply.result = req.bytes;
#endif
        if (reply.result >= 0) {
            if (Produce(page, reply.result) < 0)
                break;
        }
        if (reply.result < _pageSize)
            break;
        offset += _pageSize;
    }

    SetDeviceIdle();
}

void IOTask::_WriteStream()
{
    SetDeviceBusy();

    unsigned long long offset = 0;

    while(1) {
        char *page;
        int bytes = Consume(page);
        if (bytes <= 0)
            break;
        Request req = { WriteReq, offset, bytes, page };
        Request reply;
#if 1
        DeviceIO(req, reply);
#else
        reply.result = req.bytes;
#endif
        if (reply.result != bytes)
            break;
        int status = MemPool::Instance()->Deallocate(MemPool::Buffer, page);
        DOASSERT(status >= 0 && page, "Failed to deallocate buffer space\n");
        offset += bytes;
    }

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

FdIOTask::FdIOTask(int fd, int blockSize) :
	IOTask(blockSize), _fd(fd)
{
    _offset = lseek(fd, SEEK_CUR, 0);
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

TapeIOTask::TapeIOTask(TapeDrive &tape) :
	IOTask(tape.getBlockSize()), _tape(tape)
{
    _offset = _tape.tell();
}

WebIOTask::WebIOTask(char *url, Boolean isInput, int blockSize) :
	FdIOTask(OpenURL(url, isInput), blockSize)
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

    DOASSERT(_webfd < 0 && _cache >= 0, "Invalid parameters");

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
    unsigned long int sum = 0;
    unsigned long int xor = 0;
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
    printf("Writing cached HTTP file (%ld bytes) to %s\n",
           _cacheSize, _url);
#endif

    DOASSERT(!_isInput && _webfd < 0 && _cache > 0, "Invalid parameters");

    size_t totlen = _cacheSize;
    _webfd = open_http(_url, _isInput, &totlen);
    if (_webfd < 0) {
        fprintf(stderr, "Cannot write to %s\n", _url);
        perror("connect");
        return;
    }

    unsigned long int sent = 0;

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

MemPool *MemPool::_instance = 0;

MemPool::MemPool(int numPages, int pageSize, int &status) :
	_numPages(numPages), _pageSize(pageSize), _maxBuff(0.3)
{
    _instance = this;

    status = Initialize();
}

int MemPool::Initialize()
{
    int status;
    _sem = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_sem, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _sem->setValue(1);

    _free = new SemaphoreV(Semaphore::newKey(), status, 1);
    DOASSERT(_free, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _free->setValue(0);

    // We need space for page and also address in _freePage
    int size = _numPages * (_pageSize + sizeof(char *)) + 3 * sizeof(int);

#ifdef SHARED_MEMORY
    key_t _shmKey = SharedMemory::newKey();
    int created = 0;
    _shm = new SharedMemory(_shmKey, size, _buf, created);
    DOASSERT(_shm, "Out of memory");
    if (!created)
        printf("Warning: pre-existing shared memory initialized\n");
#if DEBUGLVL >= 1
    printf("Created a %d-byte shared memory segment at 0x%p\n", size, _buf);
#endif
#else
    _buf = new char [size];
#if DEBUGLVL >= 1
    printf("Created a %d-byte local memory area at 0x%p\n", size, _buf);
#endif
#endif

    DOASSERT(_buf, "Out of memory");

#if DEBUGLVL >= 1
    printf("Initializing memory pool\n");
#endif

    memset(_buf, 0, _numPages * _pageSize);

#if DEBUGLVL >= 1
    printf("Creating free, cache, and buffer lists\n");
#endif

    _freePage = (char **)(_buf + _numPages * _pageSize);
    _numFree = (int *)(_freePage + _numPages);
    _numCache = (int *)(_numFree + 1);
    _numBuffer = (int *)(_numCache + 1);

    for(int i = 0; i < _numPages; i++)
        _freePage[i] = _buf + i * _pageSize;

    *_numFree = _numPages;
    *_numCache = 0;
    *_numBuffer = 0;

    return 0;
}

MemPool::~MemPool()
{
#ifdef SHARED_MEMORY
    delete _shm;
#else
    delete _buf;
#endif

    _sem->destroy();
    delete _sem;

    _free->destroy();
    delete _free;
}

int MemPool::Allocate(PageType type, char *&page)
{
    AcquireMutex();

    while (!(*_numFree)) {
#if DEBUGLVL >= 5
        printf("Out of free pages: %d cache, %d buffer\n",
               *_numCache, *_numBuffer);
#endif
        ReleaseMutex();
        AcquireFree();
        AcquireMutex();
#if DEBUGLVL >= 5
        printf("Woke up from sleep, free pages: %d cache, %d buffer\n",
               *_numCache, *_numBuffer);
#endif
    }
           
    --(*_numFree);
    page = _freePage[*_numFree];
    DOASSERT(page, "Invalid page");
    _freePage[*_numFree] = 0;
    if (type == Buffer)
        (*_numBuffer)++;
    else
        (*_numCache)++;

#if DEBUGLVL >= 3
    printf("Allocated %s page 0x%p\n",
           (type == Cache ? "cache" : "buffer"), page);
#endif

    ReleaseMutex();

    return 0;
}

int MemPool::Release(PageType type, char *&page)
{
    // Make decision whether to reclaim page or not; decision
    // based on ratio of cache/buffer pages, for example

    if (1) {
        Deallocate(type, page);
        page = 0;
    }

    return 0;
}

int MemPool::Deallocate(PageType type, char *page)
{
    AcquireMutex();

    DOASSERT(!_freePage[*_numFree], "Invalid page");
    _freePage[(*_numFree)++] = page;
    if (type == Buffer) {
        (*_numBuffer)--;
#if DEBUGLVL >= 3
        printf("Now %d buffer pages remain in use\n", *_numBuffer);
#endif
    } else {
        (*_numCache)--;
#if DEBUGLVL >= 3
        printf("Now %d cache pages remain in use\n", *_numCache);
#endif
    }

    // If someone is waiting for a free page, signal it
    if (*_numFree == 1)
        ReleaseFree();

    ReleaseMutex();

    return 0;
}

int MemPool::Convert(char *page, PageType oldType, PageType &newType)
{
    AcquireMutex();

    page = page;

    if (oldType != newType) {
        if (oldType == Cache) {
            (*_numCache)--;
            (*_numBuffer)++;
        } else {
            (*_numCache)++;
            (*_numBuffer)--;
        }

#if DEBUGLVL >= 3
        printf("Memory pool now has %d cache pages, %d buffer pages\n",
               *_numCache, *_numBuffer);
#endif
    }

    ReleaseMutex();

    return 0;
}

SBufMgr *SBufMgr::_instance = 0;

SBufMgr::SBufMgr(MemPool &pool, int frames) :
	_pool(pool), _numFrames(frames),
        _pageSize(pool.PageSize())
#ifndef SHARED_MEMORY
        , _ht(103, AddrHash)
#endif
{
    _instance = this;

    (void)Initialize();
}

int SBufMgr::Initialize()
{
    int status;
    _mutex = new SemaphoreV(Semaphore::newKey(), status);
    DOASSERT(_mutex, "Out of memory");
    DOASSERT(status >= 0, "Cannot create semaphore");
    _mutex->setValue(1);

#ifdef SHARED_MEMORY
    printf("Creating buffer manager frame table in shared memory\n");
    key_t _frmShmKey = SharedMemory::newKey();
    int created = 0;
    char *buf;
    _frmShm = new SharedMemory(_frmShmKey, _numFrames * sizeof(PageFrame),
                               buf, created);
    DOASSERT(_frmShm, "Out of memory");
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
    DOASSERT(_frames, "Out of memory");
#endif
 
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        frame.Clear();
        frame.page = 0;
        frame.size = 0;
    }

    return 0;
}

SBufMgr::~SBufMgr()
{
    _DeallocMemory();

#ifdef SHARED_MEMORY
    delete _frmShm;
#else
    delete [] _frames;
#endif

    _mutex->destroy();
    delete _mutex;
}

void SBufMgr::_DeallocMemory()
{
    AcquireMutex();

    // deallocate pages and frames

    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.page)
            _pool.Deallocate(MemPool::Cache, frame.page);
    }

    ReleaseMutex();
}

int SBufMgr::_PinPage(IOTask *stream, int pageNo, char *&page, Boolean read)
{
    if (!stream) {
        printf("No stream\n");
        return -1;
    }

    int index;
    PageAddr addr = { stream, pageNo };
#ifdef SHARED_MEMORY
    for(index = 0; index < _numFrames; index++) {
        PageFrame &frame = _frames[index];
        if (frame.valid && frame.addr == addr)
            break;
    }
    int status = (index < _numFrames ? 0 : -1);
#else
    int status = _ht.lookup(addr, index);
#endif

    Boolean newPage = false;

    if (status >= 0) {                  // frame found
        PageFrame &frame = _frames[index];
        if (!frame.valid)
            printf("frame valid %d\n", frame.valid);
        DOASSERT(frame.valid, "Invalid page frame 1");
        frame.pinCnt++;
        if (!frame.page)
            newPage = true;
    } else {
        if ((index = AllocFrame()) < 0) {
            printf("No free frame\n");
            return index;
        }
        PageFrame &frame = _frames[index];
        assert(!frame.page);
        newPage = true;
#ifndef SHARED_MEMORY
        if ((status = _ht.insert(addr, index)) < 0) {
            printf("Cannot insert to hash table\n");
            return status;
        }
#endif
        frame.Set(stream, pageNo);
    }

    PageFrame &frame = _frames[index];
    DOASSERT(frame.valid, "Invalid page frame 2");
    frame.refbit = true;

    // When data is piped from I/O devices, the I/O tasks will not allocate
    // memory space for us; with shared or local memory, I/O tasks allocate
    // memory, so we have to do it only we're not going to call the
    // I/O task here

    if (!read || !newPage) {
        if (!frame.page) {
            if ((status = _pool.Allocate(MemPool::Cache, frame.page)) < 0) {
                printf("Cannot get free page from pool\n");
                return status;
            }
            frame.size = _pageSize;
        }
    }

    if (read && newPage) {
        frame.iopending = true;
        ReleaseMutex();
        if ((status = stream->Read(((unsigned long long)pageNo) *
                                   ((unsigned long long)_pageSize),
                                   _pageSize, frame.page)) < 0) {
            printf("Cannot read page\n");
            return status;
        }
        AcquireMutex();
        frame.iopending = false;
        frame.size = status;
    }

    page = frame.page;
    DOASSERT(page, "Invalid page address 2");

    if (!read && newPage)
        memset(page, 0, _pageSize);

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d pinned in frame %d, addr 0x%p, pinCnt is %d\n",
           stream, pageNo, index, frame.page, frame.pinCnt);
#endif

    return frame.size;
}

int SBufMgr::_UnPinPage(IOTask *stream, int pageNo, Boolean dirty,
                        int size, Boolean force)
{
    if (!stream) {
        printf("No stream\n");
        return -1;
    }

    int index;
    PageAddr addr = { stream, pageNo };
#ifdef SHARED_MEMORY
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
    DOASSERT(frame.valid, "Invalid page frame 3");

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
        DOASSERT(frame.page, "Invalid page address 3");
        if (!frame.dirty) {
            // Release unpinned non-dirty pages back to memory pool
#if DEBUGLVL >= 5
            printf("Releasing page 0x%p to memory pool\n", frame.page);
#endif
            _pool.Release(MemPool::Cache, frame.page);
        } else if (force) {
#if DEBUGLVL >= 5
            printf("Forcibly writing page %ld to device\n", frame.addr.pageNo);
#endif
            frame.iopending = true;
            ReleaseMutex();
            // Write dirty pages if forced
            int status = frame.addr.stream->Write(
                            ((unsigned long long)frame.addr.pageNo) *
                            ((unsigned long long)_pageSize),
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
            if (frame.page)
                _pool.Release(MemPool::Cache, frame.page);
#ifndef SHARED_MEMORY
            if ((status = _ht.remove(frame.addr)) < 0) {
                return status;
            }
#endif
            frame.Clear();
        }
    }

    frame.pinCnt--;

#if DEBUGLVL >= 3
    printf("Page 0x%p:%d unpinned from frame %d, pinCnt is %d\n",
           stream, pageNo, index, frame.pinCnt);
#endif

    return 0;
}

int SBufMgr::_UnPin(IOTask *stream, Boolean dirty)
{
    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (frame.valid && (!stream || frame.addr.stream == stream)) {
#if DEBUGLVL >= 5
            printf("Releasing frame %d\n", i);
#endif
            int status;
#ifndef SHARED_MEMORY
            if ((status = _ht.remove(frame.addr)) < 0)
                return status;
#endif
            if (dirty)
                frame.dirty = true;
            if (frame.dirty) {
#if DEBUGLVL >= 5
                printf("Flushing page %ld to device\n", frame.addr.pageNo);
#endif
                DOASSERT(frame.page, "Invalid page address 4");
                frame.iopending = true;
                ReleaseMutex();
                status = frame.addr.stream->Write(
                            ((unsigned long long)frame.addr.pageNo) *
                            ((unsigned long long)_pageSize),
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
            }
            if (frame.page)
                _pool.Release(MemPool::Cache, frame.page);
            frame.Clear();
        }
    }

    return 0;
}

int SBufMgr::_InMemory(IOTask *stream, int pageStart, int pageEnd,
                       PageRangeList *&inMemory)
{
    if (!stream) {
        printf("No stream\n");
        return -1;
    }

    inMemory = 0;
    pageStart = pageEnd;

    for(int i = 0; i < _numFrames; i++) {
        PageFrame &frame = _frames[i];
        if (!frame.valid || frame.addr.stream != stream)
            continue;
#if DEBUGLVL >= 5
        printf("Adding frame %d, page %ld to page list\n",
               i, frame.addr.pageNo);
#endif
    }

    return 0;
}

int SBufMgr::AllocFrame()
{
    int i = PickVictim();
    if (i < 0) {
        fprintf(stderr, "Could not find free frame for page\n");
        return i;
    }

    PageFrame &frame = _frames[i];
    if (frame.valid)
        Dump();
    DOASSERT(!frame.valid, "Invalid page frame 4");

    return i;
}

int SBufMgr::AddrHash(PageAddr &addr, int numBuckets)
{
    return (((long)addr.stream) + addr.pageNo) % numBuckets;
}

void SBufMgr::Dump()
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

SBufMgrLRU::SBufMgrLRU(MemPool &pool, int frames) :
	SBufMgr(pool, frames), _clockHand(_numFrames - 1)
{
}

int SBufMgrLRU::PickVictim()
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
            // If no pages available in memory pool, reuse pages
            // in frames that are not pinned; otherwise find an
            // unused frame and allocate page from memory pool.
            if (!freePool) {
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

#ifndef SHARED_MEMORY
        int status = _ht.remove(frame.addr);
        if (status < 0)
            return status;
#endif

        if (frame.dirty) {              // page has unwritten data on it
            DOASSERT(frame.page, "Invalid page address 5");
#if DEBUGLVL >= 5
            printf("Flushing page %ld from frame %d\n", frame.addr.pageNo,
                   selectedFrame);
#endif
            frame.iopending = true;
            ReleaseMutex();
            int status = frame.addr.stream->Write(
                            ((unsigned long long)frame.addr.pageNo) *
                            ((unsigned long long)_pageSize),
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
#if DEBUGLVL >= 5
            printf("Flushed page %ld from frame %d\n", frame.addr.pageNo,
                   selectedFrame);
#endif
        }

        if (frame.page)
            _pool.Release(MemPool::Cache, frame.page);

        frame.Clear();

        return selectedFrame;
    }
}
