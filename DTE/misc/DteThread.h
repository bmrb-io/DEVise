#ifndef _DTETHREAD_H_
#define _DTETHREAD_H_

// (C) 1999- by Kevin S. Beyer

#define DTE_PTHREADS

#include <iostream.h>

//===========================================================================
#if defined(DTE_PTHREADS)
#include <pthread.h>


class DteThread
{
public:

  enum { DEFAULT_STACK_SIZE = 64*1024 };

  //kb: Maybe I should initialize and allocate a mutex here
  DteThread(unsigned stackSize = DEFAULT_STACK_SIZE)
    : stackSize(stackSize) { }

  //kb: destroy the mutex here
  virtual ~DteThread() { }

  void fork() {
    typedef void* (*THREAD_FUNC)(void*);
    if( pthread_create(&tid, NULL, pthreadRun, this) != 0 ) {
      cerr << "error creating pthread\n";
      exit(1);
    }
  }

  // waits for the completion of this thread
  void wait() {
    //kb: wait on the mutex here, and release the mutex at the end of run()
    void* status;
    if( pthread_join(tid, &status) != 0 ) {
      cerr << "error joining thread\n";
      exit(1);
    }
  }

protected:

  virtual void run() = 0;

private:

  unsigned stackSize;

  pthread_t tid;
  static void* pthreadRun(void* t); // eliminates warnings

  DteThread(const DteThread& x);
  DteThread& operator=(const DteThread& x);
};

//kb: place this in a .C file?
inline
void* DteThread::pthreadRun(void* t)
{
  ((DteThread*)t)->run();
  return NULL;
}

//---------------------------------------------------------------------------

class DteMutex
{
public:

  DteMutex() {
    if( pthread_mutex_init(&mutex, NULL) != 0 ) {
      cerr << "error creating mutex\n";
      exit(1);
    }
  }

  ~DteMutex() {
    if( pthread_mutex_destroy(&mutex) != 0 ) {
      cerr << "error destroying mutex\n";
      exit(1);
    }
  }

  // timeout nyi
  void acquire() {
    if( pthread_mutex_lock(&mutex) != 0 ) {
      cerr << "error locking mutex\n";
      exit(1);
    }
  }

  void release() {
    if( pthread_mutex_unlock(&mutex) != 0 ) {
      cerr << "error unlocking mutex\n";
      exit(1);
    }
  }

private:

  pthread_mutex_t mutex;

  DteMutex(const DteMutex& x);
  DteMutex& operator=(const DteMutex& x);

  friend class DteMonitor;
};

//---------------------------------------------------------------------------

class DteMonitor
{
public:

  DteMonitor() {
    if( pthread_cond_init(&condVar,NULL) != 0 ) {
      cerr << "error creating condition variable\n";
      exit(1);
    }
  }

  ~DteMonitor() {
    if( pthread_cond_destroy(&condVar) != 0 ) {
      cerr << "error destroying condition variable\n";
      exit(1);
    }
  }

  void acquire() { mutex.acquire(); }

  // timeout nyi
  void wait() {
    if( pthread_cond_wait(&condVar, &(mutex.mutex)) != 0 ) {
      cerr << "error waiting on condition variable\n";
      exit(1);
    }
  }

  //kb: should signal automatically release??
  void signal() {
    if( pthread_cond_signal(&condVar) != 0 ) {
      cerr << "error signaling condition variable\n";
      exit(1);
    }
    mutex.release();
  }

  void broadcast() {
    if( pthread_cond_broadcast(&condVar) != 0 ) {
      cerr << "error broadcasting condition variable\n";
      exit(1);
    }
    mutex.release();
  }

protected:

private:

  DteMutex mutex;

  pthread_cond_t condVar;

  DteMonitor(const DteMonitor& x);
  DteMonitor& operator=(const DteMonitor& x);
};



//===========================================================================
#elif defined(DTE_STHREADS)
#error sthreads nyi
#include "sthread.h"


//===========================================================================
#else
#error need to choose thread implementation
#endif    

//===========================================================================

#endif // _DTETHREAD_H_
