/*
  This file contributed by the Tape Join project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$*/

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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "machdep.h"

class Semaphore {
public:
  Semaphore(key_t key, int &status, int nsem = 1);
  int destroy();                        // destroy semaphore
  static int destroyAll();              // destroy all semaphores

  int acquire(int num = 1,
	      int sem = 0) {            // acquire num units of resource
    static struct sembuf sop;
    sop.sem_num = sem;                  // semaphore sem
    sop.sem_op = -num;                  // acquire/get num units (P)
    sop.sem_flg = 0;
    while(1) {
      int result = semop(id, &sop, 1);
      if (result < 0 && errno == EINTR) // signal received?
	continue;
      if (result < 0)
	perror("semop");
      assert(result >= 0);
      break;
    }
    return num;
  }

  int release(int num = 1,
	      int sem = 0) {            // release num units of resource
    static struct sembuf sop;
    sop.sem_num = sem;                  // semaphore sem
    sop.sem_op = num;                   // release num units (V)
    sop.sem_flg = 0;
    int result = semop(id, &sop, 1);
    if (result < 0)
      perror("semop");
    assert(result >= 0);
    return num;
  }

  int test(int num = 1) {               // try to acquire num units of resource
    static struct sembuf sop;
    sop.sem_num = 0;                    // semaphore 0
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

  int setValue(int num, int sem = 0);   // set num units of resource
  int getId() { return id; }            // return id

  static key_t newKey();                // create new key

  operator int();                       // get current number of units

private:
  key_t key;                            // identifying key
  int   id;                             // semaphore id
};

class SharedMemory {
public:
  SharedMemory(key_t key, int size, char *&addr, int &created);
  ~SharedMemory();
  static int destroyAll();              // destroy all shared mem segments

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

  Semaphore *sems;                      // semaphores
  const int lock = 0;                   // protects comm area
  const int hasData = 1;                // set when comm area has data
  const int hasSpace = 2;               // set when comm area has space
  SharedMemory *shmem;                  // shared memory area
};

#endif
