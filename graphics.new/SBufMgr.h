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
  Revision 1.12  1996/12/03 20:42:16  jussi
  Moved memory management stuff to MemMgr.C. Introduced better types
  for stream position (streampos_t), byte count (bytecount_t) and
  request size (iosize_t).

  Revision 1.11  1996/11/23 21:05:01  jussi
  MemMgr now manages multi-page chunks instead of single pages.

  Revision 1.10  1996/11/15 20:34:02  jussi
  Removed MemMgr::Release() method. IOTask::Write() no longer
  deallocates the page it gets from the caller.

  Revision 1.9  1996/11/12 17:23:41  jussi
  Renamed SBufMgr class to CacheMgr and MemPool to MemMgr. This is
  to reflect the new terms (cache manager, memory manager) used in
  the documentation.

  Revision 1.8  1996/11/11 15:50:57  jussi
  Removed IOTask virtual base class and substituted UnixIOTask for
  it. When SBufMgr accesses an IOTask, no separate process is
  created. A process is created only when a ReadStream() or WriteStream()
  function of IOTask is called. In Solaris, switched to using processes
  instead of threads.

  Revision 1.7  1996/11/08 15:42:24  jussi
  Removed IOTask::Initialize() and SetBuffering(). Added support
  for streaming via ReadStream() and WriteStream().

  Revision 1.6  1996/11/07 17:37:13  jussi
  Memory pool is no longer a separate process but a reentrant
  function library.

  Revision 1.5  1996/11/04 20:21:00  jussi
  Improved support for parallel writes.

  Revision 1.4  1996/11/01 20:11:59  jussi
  MemPool object now forks a separate memory pool process. Got rid
  of the option of having processes sharing data via pipes. (Pipes
  still used for sharing control information.) SBufMgr object now
  compatible with process/shared memory environment.

  Revision 1.3  1996/10/02 19:48:33  jussi
  Added support for writing data through an HTTP connection.
  Also made it possible to use simple POST/GET transactions
  using a single IOUnixWebTask.

  Revision 1.2  1996/09/26 20:29:07  jussi
  Made code compile in Linux without threads.

  Revision 1.1  1996/09/26 19:02:33  jussi
  Renamed file from ExtBufMgr -> SBufMgr. Added Web I/O task and
  changed buffer manager to allow more concurrency between I/Os.

  Revision 1.2  1996/09/07 01:43:31  jussi
  Method BufferDealloc() is used also when BUFFER is not defined.

  Revision 1.1  1996/08/01 22:45:27  jussi
  Initial revision.
*/

#ifndef SBufMgr_h
#define SBufMgr_h

// These flags are now defined in the appropriate architecture-specific
// makefile (e.g., solaris/Makfile.config) so that we can configure
// this class differently depending on the architecture.
//#define SBM_PROCESS
//#define SBM_THREAD
//#define SBM_SHARED_MEMORY

#if !defined(SBM_PROCESS) && !defined(SBM_THREAD)
#error "Must use either process or thread tasks"
#endif

#if defined(SBM_PROCESS) && defined(SBM_THREAD)
#error "Cannot use both process and thread tasks"
#endif

#if defined(SBM_PROCESS) && !defined(SBM_SHARED_MEMORY)
#error "Must have shared memory for process tasks"
#endif

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#ifdef SBM_THREAD
#include <pthread.h>
#endif

#include "DeviseTypes.h"
#include "MemMgr.h"

#ifndef ATTRPROJ
#include "tapedrive.h"
#include "HashTable.h"
#endif

// I/O task

class IOTask {
  public:
    // Create and destroy I/O task
    IOTask(int &status, int blockSize = -1);
    virtual int WriteEOF() { return 0; }
    virtual ~IOTask();

    // Read specified byte range from file
    int Read(streampos_t offset, iosize_t bytes, char *&addr);

    // Read specified block range
    int ReadP(int blockOffset, int blocks, char *&addr) {
        int result = Read(blockOffset * _blockSize, blocks * _blockSize, addr);
        if (result < 0 || (result % _blockSize) != 0)
            return -1;
        return result / _blockSize;
    }

    // Write specified byte range to file
    int Write(streampos_t offset, iosize_t bytes, char *addr);

    // Write specified block range
    int WriteP(int blockOffset, int blocks, char *&addr) {
        int result = Read(blockOffset * _blockSize, blocks * _blockSize, addr);
        if (result < 0 || (result % _blockSize) != 0)
            return -1;
        return result / _blockSize;
    }

    // Read stream
    int ReadStream(streampos_t offset, iosize_t bytes, int pipeSize);
    int Consume(char *&buf, streampos_t &offset, iosize_t &bytes) {
        return _dataPipe->Consume(buf, offset, bytes);
    }

    // Write stream
    int WriteStream(int pipeSize);
    int Produce(char *buf, streampos_t offset, iosize_t bytes) {
        return _dataPipe->Produce(buf, offset, bytes);
    }

    // Terminate I/O stream
    int Terminate();

    // Return busy/idle status
    Boolean IsBusy();

  protected:
    int StartChild(int pipeSize);

    // Request types
    enum ReqType { ReadReq, WriteReq };

    // Request structure
    struct Request {
        ReqType type;
        streampos_t offset;
        iosize_t bytes;
        char *addr;
        int result;
    };

    // Child process for streaming data
    static void *DoStream(void *arg);
    void *DoStream();

    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply) = 0;

    // Return current offset
    virtual streampos_t Offset() = 0;

    // Read stream
    void _ReadStream(streampos_t offset, iosize_t bytes);

    // Write stream
    void _WriteStream();

    // Set/clear device busy flag
    void SetDeviceBusy() { _isBusy->acquire(1); }
    void SetDeviceIdle() { _isBusy->release(1); }

    const int _pageSize;                // size of memory manager pages
    int _blockSize;                     // (user) data block size

    DataPipe *_dataPipe;                // data pipe
    Boolean _readStream;                // read streaming enabled
    Boolean _writeStream;               // write streaming enabled
    streampos_t _streamOffset;          // offset of data stream
    iosize_t _streamLength;             // length of data stream

    bytecount_t _readBytes;             // # bytes read
    bytecount_t _writeBytes;            // # bytes written
    bytecount_t _seekBytes;             // total seek distance in bytes

#ifdef SBM_PROCESS
    pid_t _child;                       // pid of child process
#endif
#ifdef SBM_THREAD
    pthread_t _child;                   // thread id of child
#endif

    SemaphoreV *_isBusy;                // flag indicating busy device
};

// I/O task for Unix file descriptors

class FdIOTask : public IOTask {
  public:
    // Create I/O task using file descriptors
    FdIOTask(int &status, int fd, int blockSize = -1);

  protected:
    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply);

    // Return current offset
    virtual streampos_t Offset() { return _offset; }

    int _fd;                            // fd of device
    streampos_t _offset;                // current offset on device
};

#ifndef ATTRPROJ
// I/O task for Web connections

class WebIOTask : public FdIOTask {
  public:
    // Create Web I/O task
    WebIOTask(int &status, char *url, Boolean isInput, int blockSize = -1);
    virtual ~WebIOTask();

    // Finish writing to HTTP
    virtual int WriteEOF();

  protected:
    // Open connection to Web resource
    int OpenURL(char *url, Boolean isInput);

    // Write cached HTTP file to Web site
    void WriteHTTP();

    // Make unique cache file name for URL
    char *MakeCacheName(char *url);

    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply);

    int _webfd;                         // file descriptor for Web connection
    char *_url;                         // URL of Web resource
    Boolean _isInput;                   // true if input Web resource
    FILE *_cache;                       // cache file pointer
    char *_cacheName;                   // name of cache file
    bytecount_t _cacheSize;             // current size of cache file
};

// I/O task for TapeDrive objects

class TapeIOTask : public IOTask {
  public:
    // Create tape I/O task
    TapeIOTask(int &status, TapeDrive &tape);

  protected:
    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply);

    // Return current offset
    virtual streampos_t Offset() { return _offset; }

    TapeDrive &_tape;                   // tape device
    streampos_t _offset;                // current offset on tape
};

// Page address information

struct PageAddr {
    IOTask *stream;                     // stream pointer
    long pageNo;                        // page number within a stream
};

inline int operator==(PageAddr &addr1, PageAddr &addr2)
{
    if (addr1.stream == addr2.stream && addr1.pageNo == addr2.pageNo)
        return 1;
    return 0;
}

// Page frame information

class PageFrame {
  friend class CacheMgr;
  friend class CacheMgrLRU;

  public:
    PageFrame() {
        Clear();
        page = 0;
        size = 0;
    }
    
  protected:
    PageAddr addr;                      // address of page in frame
    char *page;                         // memory image of page
    int size;                           // page length (_pageSize for all
                                        // except last page in file)
    int pinCnt;                         // pin count of page
    unsigned valid: 1;                  // true if frame information is valid
    unsigned dirty: 1;                  // true if page is dirty
    unsigned refbit: 1;                 // true if page referenced recently
    unsigned iopending: 1;              // true if I/O pending for page

  // Initialize frame for page

  void Clear() {
      pinCnt = 0;
      addr.stream = 0;
      addr.pageNo = -1;
      valid = dirty = refbit = iopending = false;
  }

  // Set frame fields

  void Set(IOTask *stream, int pageNo) { 
      addr.stream = stream;
      addr.pageNo = pageNo;
      pinCnt = 1;
      valid = true;
      dirty = false;
      refbit = true;
  }
};

// Streaming buffer manager

class CacheMgr {
  public:
    CacheMgr(MemMgr &mgr, int frames, int &status);
    virtual ~CacheMgr();  

    // Request types
    enum ReqType { PinPageReq, UnPinPageReq, UnPinReq, TerminateReq };

    // Request structure
    struct Request {
        ReqType type;                   // request type
        IOTask *stream;                 // I/O stream
        int pageNo;                     // page number
        char *page;                     // page address
        int size;                       // size of page
        Boolean read;                   // true if allocate new page
        Boolean dirty;                  // true if page is dirty
        Boolean force;                  // true if page must be forced out
    };

    // Direct (same process) interface to memory manager
    int AllocPage(IOTask *stream, int pageNo, char *&page) {
        return PinPage(stream, pageNo, page, false);
    }
    int PinPage(IOTask *stream, int pageNo,
                char *&page, Boolean read = true) {
        AcquireMutex();
        int result = _PinPage(stream, pageNo, page, read);
        ReleaseMutex();
        return result;
    }
    int UnPinPage(IOTask *stream, int pageNo, Boolean dirty,
                  int size = -1, Boolean force = false) {
        AcquireMutex();
        int result = _UnPinPage(stream, pageNo, dirty, size, force);
        ReleaseMutex();
        return result;
    }
    int UnPin(IOTask *stream, Boolean dirty) {
        AcquireMutex();
        int result = _UnPin(stream, dirty);
        ReleaseMutex();
        return result;
    }
    static CacheMgr *Instance() { return _instance; }

 protected:
    // Initialization
    virtual int Initialize();

    // Acquire and release mutex
    void AcquireMutex() { _mutex->acquire(); }
    void ReleaseMutex() { _mutex->release(); }

    int _PinPage(IOTask *stream, int pageNo,
                 char *&page, Boolean read = true);
    int _UnPinPage(IOTask *stream, int pageNo, Boolean dirty,
                   int size = -1, Boolean force = false);
    int _UnPin(IOTask *stream, Boolean dirty);

    // An instance of this class
    static CacheMgr *_instance;

    void _AllocMemory();
    void _DeallocMemory();

#ifdef NEWSTUFF
    // Request interface
    int SendReq(Request &req, Request &reply);

    // Child process for communicating with parent
    static void *ProcessReq(void *arg);
    void *ProcessReq();

    // Fd's of pipes for requests
    int _reqFd[2];
    int _replyFd[2];

#ifdef SBM_PROCESS
    pid_t _child;                       // pid of child process
#endif
#ifdef SBM_THREAD
    pthread_t _child;                   // thread id of child
#endif
#endif

    // Pick a free frame; pick victim page if necessary
    int AllocFrame();
    virtual int PickVictim() = 0;

    // Hash function for page address hash table
    static int AddrHash(PageAddr &addr, int numBuckets);

    // Print diagnostic information
    virtual void Dump();

    // Memory manager
    MemMgr &_mgr;

    // Number of frames and page size
    const int _numFrames;
    const int _pageSize;

    // Page frames
    PageFrame *_frames;
#ifdef SBM_SHARED_MEMORY
    SharedMemory *_frmShm;
#else
    // Hash table of page addresses
    HashTable<PageAddr, int> _ht;
#endif

    // Mutexes for synchronization
    SemaphoreV *_mutex;
};

// LRU buffer manager

class CacheMgrLRU : public CacheMgr {
  public:
    CacheMgrLRU(MemMgr &mgr, int frames, int &status);
    virtual ~CacheMgrLRU() {}

 protected:
    // Pick victim for replacement
    virtual int PickVictim();

    // Clock hand for Clock page replacement algorithm
    int _clockHand;
};
#endif

#endif
