#ifndef _DTETHREAD_H_
#define _DTETHREAD_H_

// (C) 1998- by Kevin S. Beyer

#define DTE_PTHREADS

#if defined(DTE_PTHREADS)
#include <pthread.h>
#elif defined(DTE_STHREADS)
#error sthreads nyi
#include "sthread.h"
#else
#error need to choose thread implementation
#endif    


//---------------------------------------------------------------------------


class DteThread
{
public:

  enum { default_stack_size = 64*1024 };

  DteThread(unsigned stack_size = default_stack_size);

  virtual ~DteThread();

  void fork();

  void wait();                  // for completion

protected:

  virtual void run() = 0;

private:

  unsigned _stack_size;

#if defined(DTE_PTHREADS)
  pthread_t _tid;
  static void* DteThreadRun(void* t);
#elif defined(DTE_STHREADS)
#endif

  DteThread(const DteThread& x);
  DteThread& operator=(const DteThread& x);
};


inline
DteThread::DteThread(unsigned stack_size)
: _stack_size(stack_size)
{
#if defined(DTE_PTHREADS)
#elif defined(DTE_STHREADS)
#endif
}


inline
DteThread::~DteThread()
{
#if defined(DTE_PTHREADS)

#elif defined(DTE_STHREADS)
#endif
}

#if defined(DTE_PTHREADS)
inline
void* DteThread::DteThreadRun(void* t)
{
  try {
    ((DteThread*)t)->run();
  } catch(...) {
    cerr << "uncaught exception!\n";
  }
  return NULL;
}
#endif

inline
void DteThread::fork()
{
#if defined(DTE_PTHREADS)
  typedef void* (*THREAD_FUNC)(void*);
  if( pthread_create(&_tid, NULL, DteThreadRun, this) != 0 ) {
    cerr << "error creating pthread\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif
}


inline
void DteThread::wait()
{
#if defined(DTE_PTHREADS)
  void* status;
  if( pthread_join(_tid, &status) != 0 ) {
    cerr << "error joining thread\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif
}



//---------------------------------------------------------------------------

class DteMutex
{
public:

  DteMutex();

  ~DteMutex();

  // timeout nyi
  void acquire();

  void release();

protected:

private:

#if defined(DTE_PTHREADS)
  pthread_mutex_t _mutex;
  //pthread_mutexattr_t _mattr;
#elif defined(DTE_STHREADS)
#endif    

  DteMutex(const DteMutex& x);
  DteMutex& operator=(const DteMutex& x);

  friend class DteCondVar;
};



inline
DteMutex::DteMutex()
{
#if defined(DTE_PTHREADS)
  if( pthread_mutex_init(&_mutex, NULL) != 0 ) {
    cerr << "error creating mutex\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
DteMutex::~DteMutex()
{
#if defined(DTE_PTHREADS)
  if( pthread_mutex_destroy(&_mutex) != 0 ) {
    cerr << "error destroying mutex\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
void DteMutex::acquire()
{
#if defined(DTE_PTHREADS)
  if( pthread_mutex_lock(&_mutex) != 0 ) {
    cerr << "error locking mutex\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
  W_COERCE(_mutex.acquire())
#endif    
}


inline
void DteMutex::release()
{
#if defined(DTE_PTHREADS)
  if( pthread_mutex_unlock(&_mutex) != 0 ) {
    cerr << "error unlocking mutex\n";
    exit(1);
  }

#elif defined(DTE_STHREADS)
#endif    
}



//---------------------------------------------------------------------------

class DteCondVar
{
public:

  DteCondVar();

  ~DteCondVar();

  void wait(DteMutex& m); // timeout nyi

  void signal();

  void broadcast();

protected:

private:

#if defined(DTE_PTHREADS)
  pthread_cond_t _cond;
#elif defined(DTE_STHREADS)
#endif    

  DteCondVar(const DteCondVar& x);
  DteCondVar& operator=(const DteCondVar& x);
};



inline
DteCondVar::DteCondVar()
{
#if defined(DTE_PTHREADS)
  if( pthread_cond_init(&_cond,NULL) != 0 ) {
    cerr << "error creating condition variable\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
DteCondVar::~DteCondVar()
{
#if defined(DTE_PTHREADS)
  if( pthread_cond_destroy(&_cond) != 0 ) {
    cerr << "error destroying condition variable\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
void DteCondVar::wait(DteMutex& m)
{
#if defined(DTE_PTHREADS)
  if( pthread_cond_wait(&_cond, &(m._mutex)) != 0 ) {
    cerr << "error waiting on condition variable\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
void DteCondVar::signal()
{
#if defined(DTE_PTHREADS)
  if( pthread_cond_signal(&_cond) != 0 ) {
    cerr << "error signaling condition variable\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}


inline
void DteCondVar::broadcast()
{
#if defined(DTE_PTHREADS)
  if( pthread_cond_broadcast(&_cond) != 0 ) {
    cerr << "error broadcasting condition variable\n";
    exit(1);
  }
#elif defined(DTE_STHREADS)
#endif    
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // _DTETHREAD_H_
