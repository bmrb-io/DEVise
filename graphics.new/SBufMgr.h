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

//
// Configuration of the buffer manager
// -----------------------------------
//
// Choose one of the following feature combinations by uncommenting
// the corresponding #define's.
//
//   Processes, shared memory, with or without buffering
//   Processes, pipes
//   Threads, shared memory, with or without buffering
//   Threads, local memory, with or without buffering
//

//#define PROCESS_TASK
//#define THREAD_TASK
//#define SHARED_MEMORY
//#define PIPE_DATA
//#define BUFFER

#if defined(SOLARIS)
#define THREAD_TASK
#elif defined(SUN) || defined(HPUX) || defined(LINUX)
#define PROCESS_TASK
#define SHARED_MEMORY
#else
#define PROCESS_TASK
#define PIPE_DATA
#endif

#if !defined(PROCESS_TASK) && !defined(THREAD_TASK)
#error "Must use either process or thread tasks"
#endif

#if defined(PROCESS_TASK) && defined(THREAD_TASK)
#error "Cannot use both process and thread tasks"
#endif

#if defined(SHARED_MEMORY) && defined(PIPE_DATA)
#error "Cannot use both shared memory and pipes"
#endif

#if defined(PIPE_DATA) && defined(BUFFER)
#error "Cannot use pipes for read-ahead/write-behind buffering"
#endif

#if defined(PROCESS_TASK) && !defined(SHARED_MEMORY) && !defined(PIPE_DATA)
#error "Must have shared memory or pipes for process tasks"
#endif

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#ifdef THREAD_TASK
#include <pthread.h>
#endif

#include "tapedrive.h"
#include "DeviseTypes.h"
#include "HashTable.h"
#include "DCE.h"
#include "DList.h"

struct PageRange {
    int start;
    int end;
};

DefineDList(PageRangeList, PageRange);
DefinePtrDList(PageList, char *)

// Memory pool

class MemPool {
  public:
    MemPool(int numPages, int pageSize, int &status);
    ~MemPool();

    // Page types
    enum PageType { Cache, Buffer };

    // Request types
    enum ReqType { AllocateReq, ReleaseReq, DeallocateReq, ConvertReq };

    // Request structure
    struct Request {
        ReqType type;                   // request type
        PageType ptype;                 // page type
        PageType nptype;                // new page type (for ConvertReq)
        char *addr;                     // page address
    };

    // Direct (same process) interface to memory pool
    int Allocate(PageType type, char *&page) {
        AcquireMutex();
        int result = _Allocate(type, page);
        ReleaseMutex();
        return result;
    }
    int Release(PageType type, char *&page) {
        AcquireMutex();
        int result = _Release(type, page);
        ReleaseMutex();
        return result;
    }
    int Deallocate(PageType type, char *page) {
        AcquireMutex();
        int result = _Deallocate(type, page);
        ReleaseMutex();
        return result;
    }
    int Convert(char *page, PageType oldType, PageType &newType) {
        AcquireMutex();
        int result = _Convert(page, oldType, newType);
        ReleaseMutex();
        return result;
    }

    // IPC interface to memory pool
    int AllocateP(PageType type, char *&page) {
#ifdef PROCESS_TASK
        Request req = { AllocateReq, type, type, page };
        Request reply;
        int status = SendReq(req, reply);
        if (status < 0)
            return status;
        page = reply.addr;
        return 0;
#endif
#ifdef THREAD_TASK
        return Allocate(type, page);
#endif
    }
    int ReleaseP(PageType type, char *&page) {
#ifdef PROCESS_TASK
        Request req = { ReleaseReq, type, type, page };
        Request reply;
        int status = SendReq(req, reply);
        if (status < 0)
            return status;
        page = reply.addr;
        return 0;
#endif
#ifdef THREAD_TASK
        return Release(type, page);
#endif
    }
    int DeallocateP(PageType type, char *page) {
#ifdef PROCESS_TASK
        Request req = { DeallocateReq, type, type, page };
        Request reply;
        return SendReq(req, reply);
#endif
#ifdef THREAD_TASK
        return Deallocate(type, page);
#endif
    }
    int ConvertP(char *page, PageType oldType, PageType &newType) {
#ifdef PROCESS_TASK
        Request req = { ConvertReq, oldType, newType, page };
        Request reply;
        int status = SendReq(req, reply);
        if (status < 0)
            return status;
        newType = reply.nptype;
        return 0;
#endif
#ifdef THREAD_TASK
        return Convert(page, oldType, newType);
#endif
    }

    int PageSize() { return _pageSize; }
    int NumPages() { return _numPages; }
    int FreeLeft() { return _free.Size(); }

    static MemPool *Instance() { return _instance; }

  protected:
    // Initialization
    int Initialize();

#ifdef PROCESS_TASK
    // Request interface
    int SendReq(Request &req, Request &reply);

    // Request interrupt handler
    static void ProcessReq(int caller);
    void ProcessReq();
#endif

    // Acquire and release mutex
    void AcquireMutex() { _sem->acquire(1); }
    void ReleaseMutex() { _sem->release(1); }

    // Allocate, release and deallocate pages
    int _Allocate(PageType type, char *&page);
    int _Release(PageType type, char *&page);
    int _Deallocate(PageType type, char *page);
    int _Convert(char *page, PageType oldType, PageType &newType);

    // Hash function for page address hash table
    static int AddrHash(char *&addr, int numBuckets);

    // Number of memory pages and their size
    const int _numPages;
    const int _pageSize;

    // Base address of memory
    char *_buf;

    // List of free pages; list of cache and buffer pages
    PageList _free;
    HashTable<char *, int> *_cache;
    HashTable<char *, int> *_buffer;
    
    // Maximum ratio of buffer pages to total pages
    float _maxBuff;

    // An instance of this class
    static MemPool *_instance;

#ifdef PROCESS_TASK
    // Fd's of pipes for requests
    int _reqFd[2];
    int _replyFd[2];
    pid_t _pid;
#endif

    // Mutex for synchronization
    SemaphoreV *_sem;

#ifdef SHARED_MEMORY
    // Shared memory data structures
    key_t _shmKey;
    SharedMemory *_shm;
#endif
};

// I/O task

class IOTask {
  public:
    IOTask(int blockSize = -1) : _blockSize(blockSize) {}
    virtual int Initialize() { return 0; }
    virtual int WriteEOF() { return 0; }
    virtual ~IOTask() {}

    // Read specified byte range
    virtual int Read(unsigned long long offset,
                     unsigned long bytes,
                     char *&addr) = 0;

    // Read specified block range
    int ReadP(int blockOffset, int blocks, char *&addr) {
        int result = Read(blockOffset * _blockSize, blocks * _blockSize, addr);
        if (result < 0 || (result % _blockSize) != 0)
            return -1;
        return result / _blockSize;
    }

    // Write specified byte range
    virtual int Write(unsigned long long offset,
                      unsigned long bytes,
                      char *&addr) = 0;

    // Write specified block range
    int WriteP(int blockOffset, int blocks, char *&addr) {
        int result = Read(blockOffset * _blockSize, blocks * _blockSize, addr);
        if (result < 0 || (result % _blockSize) != 0)
            return -1;
        return result / _blockSize;
    }

    // Set buffering mode
    virtual int SetBuffering(Boolean readAhead,
                             Boolean writeBehind,
                             unsigned int buffBytes) = 0;

    // Set buffering mode
    int SetBufferingP(Boolean readAhead,
                      Boolean writeBehind,
                      int buffBlocks) {
        return SetBuffering(readAhead, writeBehind, buffBlocks * _blockSize);
    }

    // Return busy/idle status
    virtual Boolean IsBusy() = 0;

  protected:
    int _blockSize;
};

class UnixIOTask : public IOTask {
  public:
    // Create, initialize and destroy Unix I/O task
    UnixIOTask(int blockSize = -1);
    virtual int Initialize();
    virtual ~UnixIOTask();

    // Read specified byte range from file
    virtual int Read(unsigned long long offset,
                     unsigned long bytes,
                     char *&addr);

    // Write specified byte range to file
    virtual int Write(unsigned long long offset,
                      unsigned long bytes,
                      char *&addr);

    // Set buffering mode
    virtual int SetBuffering(Boolean readAhead,
                             Boolean writeBehind,
                             unsigned int buffBytes);

    // Return busy/idle status
    virtual Boolean IsBusy();

  protected:
    // Request types
    enum ReqType { ReadReq, WriteReq, SetBufferingReq, TerminateReq };

    // Request structure
    struct Request {
        ReqType type;
        unsigned long long offset;
        unsigned long bytes;
        char *addr;
        int result;
    };

    // Child process for communicating with parent
    static void *ProcessReq(void *arg);
    void *ProcessReq();

    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply) = 0;

    // Return current offset
    virtual unsigned long long Offset() = 0;

    // Set fd to blocking or nonblocking mode
    static int SetBlockMode(int fd);
    static int SetNonBlockMode(int fd);

#ifdef BUFFER
    // Perform read-ahead/write-behind buffering
    int Buffer();
    int BufferFlush(int num);
    int BufferCheck(Request &req, Request &reply);
    int BufferConvert(char *page,
                      MemPool::PageType oldType,
                      MemPool::PageType &newType);
#endif

    int BufferDealloc(MemPool::PageType type, char *page);

#ifdef SHARED_MEMORY
    // Attach and detach to/from shared memory segment
    SharedMemory *AttachSharedMemory(key_t key);
    void DetachSharedMemory(SharedMemory *shm);
#endif

    // Set/clear device busy flag
    void SetDeviceBusy();
    void SetDeviceIdle();

    // Fd's of pipes for requests
    int _reqFd[2];
    int _replyFd[2];

    Boolean _readAhead;                 // read-ahead buffering enabled
    Boolean _writeBehind;               // write-behind buffering enabled
    Boolean _readBuff;                  // true if buffer is for reads
    Boolean _buffPause;                 // a pause in buffering
    int _buffPages;                     // max # pages to buffer
    int _buffNext;                      // next page to buffer

    DefinePtrDList(ReqList, Request *);
    ReqList _buffer;                    // read/write buffer

    unsigned long int _totalCount;      // total # requests processed
    unsigned long int _buffReadCount;   // # buffered read requests
    unsigned long int _buffWriteCount;  // # buffered write requests
    unsigned long long int _readBytes;  // # bytes read
    unsigned long long int _writeBytes; // # bytes written
    unsigned long long int _seekBytes;  // total seek distance in bytes

#ifdef PROCESS_TASK
    pid_t _child;                       // pid of child process
#endif
#ifdef THREAD_TASK
    pthread_t _child;                   // thread id of child
#endif

    int _pageSize;                      // size of memory pool pages

    Semaphore *_isBusy;                 // flag indicating busy device
};

// I/O task for Unix file descriptors

class UnixFdIOTask : public UnixIOTask {
  public:
    // Create Unix I/O task using file descriptors
    UnixFdIOTask(int fd, int blockSize = -1);

  protected:
    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply);

    // Return current offset
    virtual unsigned long long Offset() { return _offset; }

    int _fd;                            // fd of device
    unsigned long long _offset;         // current offset on device
};

// I/O task for Unix Web connections

class UnixWebIOTask : public UnixFdIOTask {
  public:
    // Create Unix Web I/O task
    UnixWebIOTask(char *url, Boolean isInput, int blockSize = -1);
    virtual ~UnixWebIOTask();

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
    unsigned long int _cacheSize;       // current size of cache file
};

// I/O task for TapeDrive objects

class UnixTapeIOTask : public UnixIOTask {
  public:
    // Create, initialize and destroy Unix tape I/O task
    UnixTapeIOTask(TapeDrive &tape);

  protected:
    // Function for handling actual I/O
    virtual void DeviceIO(Request &req, Request &reply);

    // Return current offset
    virtual unsigned long long Offset() { return _offset; }

    TapeDrive &_tape;                   // tape device
    unsigned long long _offset;         // current offset on tape
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
  friend class SBufMgr;
  friend class SBufMgrLRU;

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

  // Initialize frame for page

  void Clear() {
      pinCnt = 0;
      addr.stream = 0;
      addr.pageNo = -1;
      valid = dirty = refbit = false;
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

class SBufMgr {
  public:
    SBufMgr(MemPool &pool, int frames);
    virtual ~SBufMgr();  

    // Allocate, fix and release pages; clear pages
    int AllocPage(IOTask *stream, int pageNo, char *&page) {
        return PinPage(stream, pageNo, page, false);
    }
    int PinPage(IOTask *stream, int pageNo, char *&page, Boolean read = true);
    int UnPinPage(IOTask *stream, int pageNo, Boolean dirty,
                  int size = -1, Boolean force = false);
    int UnPin(IOTask *stream, Boolean dirty);

    int InMemory(IOTask *stream, int pageStart, int pageEnd,
                 PageRangeList *&inMemory);

 protected:
    // Acquire and release mutex
    void AcquireMutex() { _mutex->acquire(); }
    void ReleaseMutex() { _mutex->release(); }

    // Pick a free frame; pick victim page if necessary
    int AllocFrame();
    virtual int PickVictim() = 0;

    // Hash function for page address hash table
    static int AddrHash(PageAddr &addr, int numBuckets);

    // Print diagnostic information
    virtual void Dump();

    // Memory pool
    MemPool &_pool;

    // Number of frames and page size
    const int _numFrames;
    const int _pageSize;

    // Page frames
    PageFrame *_frames;

    // Hash table of page addresses
    HashTable<PageAddr, int> _ht;

    // Mutex for synchronization
    SemaphoreV *_mutex;
};

// LRU buffer manager

class SBufMgrLRU : public SBufMgr {
  public:
    SBufMgrLRU(MemPool &pool, int frames);
    virtual ~SBufMgrLRU() {}

 protected:
    // Pick victim for replacement
    virtual int PickVictim();

    // Clock hand for Clock page replacement algorithm
    int _clockHand;
};

#endif
