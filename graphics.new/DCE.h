/*
  This file contributed by the Tape Join project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$
  Revision 1.12  1997/12/11 04:25:41  beyer
  Shared memory and semaphores are now released properly when devise
  terminates normally.

  Revision 1.11  1997/01/14 20:05:47  wenger
  Fixed some compile warnings; fixed relative positions of OK/Cancel
  buttons in link GUI; removed some debug code I accidentally left
  in place.

  Revision 1.10  1996/12/20 16:25:44  jussi
  Improved support for multiple semaphore vectors.

  Revision 1.9  1996/12/18 15:32:51  jussi
  Replaced ~SharedMemory() with destroy().

  Revision 1.8  1996/12/13 21:34:02  jussi
  Replaced assert() calls with error return codes.

  Revision 1.7  1996/12/12 22:04:34  jussi
  Added support for private semaphores and shared memory (default).

  Revision 1.6  1996/12/03 20:35:38  jussi
  Added debugging message.

  Revision 1.5  1996/09/26 20:28:44  jussi
  Made code compile in Linux.

  Revision 1.4  1996/09/26 19:00:21  jussi
  Added virtual semaphore class SemaphoreV which tries to get
  around the limited number of semaphore vectors allowed by
  the operating system. Various bug fixes.

  Revision 1.3  1996/09/08 05:35:20  jussi
  Added semaphore number as parameter to Semaphore::test().

  Revision 1.2  1996/07/18 02:13:20  jussi
  Added protective ifdef's for Ultrix.

  Revision 1.1  1996/07/12 03:52:28  jussi
  Initial revision.
*/

#define MODIFIED

/*
  Copyright 1993-1996 by Jussi Myllymaki
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name(s) of the copyright holder(s)
  not be used in advertising or publicity pertaining to distribution of
  the software without specific, written prior permission. The copyright
  holder(s) make no representations about the suitability of this
  software for any purpose.  It is provided "as is" without express
  or implied warranty.
  
  Author: Jussi Myllymaki
*/

// Low level operating system primitives

#ifndef DCE_H
#define DCE_H

#ifndef ULTRIX
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#endif

#include <errno.h>
#include <assert.h>
#include <stdio.h>

#ifdef HPUX
#undef MIN
#undef MAX
#endif
#include <limits.h>

#ifdef MODIFIED
#include <sys/types.h>
#endif
#include <sys/stat.h>

#include "machdep.h"

// Choose to either use private (per-process) keys, in which case
// processes (except forked ones) cannot share semaphores and memory.
// Or, use shared keys, in which case the system allocates keys
// through a table which is stored in shared memory. This approach
// may fail at run-time if several copies of the application are
// running at the same time.

#define PRIVATE_KEYS
//#define SHARED_KEYS

#if defined(PRIVATE_KEYS) && defined(SHARED_KEYS)
#error "Cannot use both private and shared keys"
#endif

#if !defined(PRIVATE_KEYS) && !defined(SHARED_KEYS)
#define PRIVATE_KEYS
#endif

class Semaphore {
public:
  Semaphore(key_t key, int &status, int nsem = 1);
  int destroy();                        // destroy semaphore
#if defined(SHARED_KEYS)
  static int destroyAll();              // destroy all semaphores
#endif

  int acquire(int num = 1,
	      int sem = 0) {            // acquire num units of resource
    struct sembuf sop;
    sop.sem_num = sem;                  // semaphore sem
    sop.sem_op = -num;                  // acquire/get num units (P)
    sop.sem_flg = 0;
    while(1) {
      int result = semop(id, &sop, 1);
      if (result < 0 && errno == EINTR) // signal received?
	continue;
      if (result < 0) {
        printf("num %d, op %d\n", sop.sem_num, sop.sem_op);
	perror("semop");
      }
      assert(result >= 0);
      break;
    }
    return num;
  }

  int release(int num = 1,
	      int sem = 0) {            // release num units of resource
    struct sembuf sop;
    sop.sem_num = sem;                  // semaphore sem
    sop.sem_op = num;                   // release num units (V)
    sop.sem_flg = 0;
    int result = semop(id, &sop, 1);
    if (result < 0) {
      printf("num %d, op %d\n", sop.sem_num, sop.sem_op);
      perror("semop");
    }
    assert(result >= 0);
    return num;
  }

  int test(int num = 1,
           int sem = 0) {               // try to acquire num units of resource
    struct sembuf sop;
    sop.sem_num = sem;                  // semaphore sem
    sop.sem_op = -num;                  // acquire/get num units (P)
    sop.sem_flg = IPC_NOWAIT;
    int result = semop(id, &sop, 1);
    if (result < 0 && errno == EAGAIN)  // failed to get num units?
      return 0;
    if (result < 0)
      perror("semop");
    assert(result >= 0);
    return num;
  }

  int getValue(int sem = 0) {           // get value of semaphore
#ifdef __linux
    static const union semun NullSemUnion = { 0 };
    int result = semctl(id, sem, GETVAL, NullSemUnion);
#else
    int result = semctl(id, sem, GETVAL, 0);
#endif
    if (result < 0)
      perror("semctl");
    assert(result >= 0);
    return result;
  }

  int setValue(int num, int sem = 0);   // set num units of resource
  int getId() { return id; }            // return id

  static key_t newKey();                // create new key

  operator int() {                      // get current number of units
    return getValue(0);                 // get current number of units
  }

private:
  key_t key;                            // identifying key
  int   id;                             // semaphore id
};

class SemaphoreV {
public:
  SemaphoreV(key_t key, int &status, int nsem = 1);
  ~SemaphoreV();

#if defined(SHARED_KEYS)
  static int destroyAll() {             // destroy all semaphores
    return Semaphore::destroyAll();
  }
#endif

  int acquire(int num = 1,
	      int sem = 0) {            // acquire num units of resource
    return _sem[_semvec[sem]]->acquire(num, _semnum[sem]);
  }

  int release(int num = 1,
	      int sem = 0) {            // release num units of resource
    return _sem[_semvec[sem]]->release(num, _semnum[sem]);
  }

  int test(int num = 1,
           int sem = 0) {               // try to acquire num units of resource
    return _sem[_semvec[sem]]->test(num, _semnum[sem]);
  }

  int getValue(int sem = 0) {           // get value of semaphore
    return _sem[_semvec[sem]]->getValue(_semnum[sem]);
  }

  int setValue(int num, int sem = 0) {  // set num units of resource
    return _sem[_semvec[sem]]->setValue(num, _semnum[sem]);
  }

  operator int() {                      // get current number of units
    return getValue(0);
  }

  int getId() {                         // return id
    return _sem[_semvec[0]]->getId();
  }

  static key_t newKey() {               // create new key
    return Semaphore::newKey();
  }

  static int numAvailable() {           // number of available semaphores
    if (!_sem) {
      if (create() < 0)
        return 0;
    }
    return _semUnused;
  }

protected:
  static int create();                  // create real semaphore vectors

  static Semaphore **_sem;              // real semaphore vectors
  static int _semVectors;               // # vectors allocated
  static int _semsPerVector;            // # semaphores per vector
  static int **_semUsed;                // non-zero if semaphore used
  static int _semUnused;                // number of unused semaphores
  int _nsem;                            // # semaphores
  int *_semvec;                         // vector number
  int *_semnum;                         // semaphore number
};

class SharedMemory {
public:
  SharedMemory(key_t key, int size, char *&addr, int &created);
  int destroy();                        // destroy shared memory segment
#if defined(SHARED_KEYS)
  static int destroyAll();              // destroy all shared mem segments
#endif

  int getSize() { return size; }        // return size of segment
  operator char*() { return addr; }     // return local address
  static key_t newKey();                // create new key
  int getId() { return id; }            // return id

private:
  key_t key;                            // identifying key
  int   id;                             // shared memory id
  int   size;                           // size of shared memory segment
  char  *addr;                          // caller's mapped address
};

class IOBuffer {
public:
  IOBuffer(int size = 32768);
  ~IOBuffer();

  void attach();                        // attach to communication area
  void init();                          // initialize comm area

  int reserveW(int minUnits, int maxUnits,
	       int bytes,
	       int &n, char *&buf);     // reserve write space
  int reserveR(int minUnits, int maxUnits,
	       int bytes,
	       int &n, char *&buf);     // reserve read space
  int releaseW(int origUnits,
	       int units, int bytes);   // release write space
  int releaseR(int units, int bytes);   // release read space

  int write(char *buf, int bytes);      // write to pipe
  int read(char *buf, int bytes);       // read from pipe

  void setEOF();                        // set EOF flag

protected:
  key_t semKey;                         // key for semaphores
  key_t shmKey;                         // key for shared memory
  int size;                             // size of shared memory

  struct CommBuffer {
    int size;                           // size of buffer
    int bytes;                          // number of bytes in buffer
    int head;                           // pointer to head of data
    int tail;                           // pointer to tail of data
    int atEOF;                          // set to 1 when EOF flagged
    int wReserve;                       // bytes reserved for writing
    int rReserve;                       // bytes reserved for reading
    char buf[1];                        // start of buffer
  } *comm;                              // communication area

  SemaphoreV *sems;                     // semaphores
  static const int lock = 0;            // protects comm area
  static const int hasData = 1;         // set when comm area has data
  static const int hasSpace = 2;        // set when comm area has space
  SharedMemory *shmem;                  // shared memory area
};

#endif
