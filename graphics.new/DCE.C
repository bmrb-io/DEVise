/*
  This file contributed by the Tape Join project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$
  Revision 1.13  1997/12/11 04:25:41  beyer
  Shared memory and semaphores are now released properly when devise
  terminates normally.

  Revision 1.12  1997/11/05 00:23:18  donjerko
  Made changes for RTree.

  Revision 1.11  1996/12/31 17:57:29  jussi
  Corrected if condition to handle failing semget().

  Revision 1.10  1996/12/20 16:25:38  jussi
  Improved support for multiple semaphore vectors.

  Revision 1.9  1996/12/18 15:32:50  jussi
  Replaced ~SharedMemory() with destroy().

  Revision 1.8  1996/12/13 21:34:02  jussi
  Replaced assert() calls with error return codes.

  Revision 1.7  1996/12/12 22:04:34  jussi
  Added support for private semaphores and shared memory (default).

  Revision 1.6  1996/12/03 20:35:37  jussi
  Added debugging message.

  Revision 1.5  1996/09/26 19:00:20  jussi
  Added virtual semaphore class SemaphoreV which tries to get
  around the limited number of semaphore vectors allowed by
  the operating system. Various bug fixes.

  Revision 1.4  1996/08/01 22:48:22  jussi
  Fixed problem with semop() in SunOS and HP-UX.

  Revision 1.3  1996/07/29 21:40:16  wenger
  Fixed various compile errors and warnings.

  Revision 1.2  1996/07/18 02:12:52  jussi
  Added #include <sys/types.h>, needed by <sys/stat.h> in Ultrix.

  Revision 1.1  1996/07/12 03:52:27  jussi
  Initial revision.
*/

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

#include <iostream.h>
#include <stdio.h>
#include <assert.h>

#include "DCE.h"

//#define DEBUG

#ifdef MODIFIED
#include "Exit.h"
#endif

#ifdef SHARED_KEYS
struct ShmKeyTable {
  int semNextKey;
  int shmNextKey;
};

static const key_t ShmKeyTableKey = 1999;
static const key_t SemaphoreBase = 1000;
static const key_t SharedMemoryBase = 2000;
static struct ShmKeyTable *shmKeyTable = 0;
#endif

#ifdef __linux
static const union semun NullSemUnion = { 0 };
#endif

Semaphore::Semaphore(key_t key, int &status, int nsem)
{
  // assume creation of semaphore fails

  status = -1;

  // possibly create and then attach to semaphore

  if ((id = semget(key, nsem, SEM_R | SEM_A)) < 0) {
    if (errno != ENOENT) {
      perror("semget");
      return;
    }
#ifdef DEBUG
    cerr << "%%  Creating semaphore (" << nsem << " sems)" << endl;
#endif
    id = semget(key, nsem, SEM_R | SEM_A | IPC_CREAT);
    if (id < 0) {
      perror("semget");
      return;
    }
  } else {
#ifdef DEBUG
    cerr << "%%  Attached to existing semaphore (" << nsem << " sems)" << endl;
#endif
  }

#ifdef DEBUG
  cerr << "%%  Semaphore id " << id << endl;
#endif

  status = 0;
}

int Semaphore::destroy()
{
#ifdef __linux
  if (semctl(id, 0, IPC_RMID, NullSemUnion) < 0)
#else
  if (semctl(id, 0, IPC_RMID) < 0)
#endif
    perror("semctl");
  return 0;
}

#if defined(SHARED_KEYS)
int Semaphore::destroyAll()
{
  key_t maxKey = SemaphoreBase + 500;
  if (shmKeyTable)
    maxKey = shmKeyTable->semNextKey + 100;

  for(key_t key = SemaphoreBase; key < maxKey; key++) {
    int id;
    if ((id = semget(key, 1, SEM_R | SEM_A)) < 0) {
      if (errno != ENOENT)
        perror("semget");
    } else {
#ifdef DEBUG
      cerr << "%%  Removing semaphore " << id << endl;
#endif
#ifdef __linux
      if (semctl(id, 0, IPC_RMID, NullSemUnion) < 0)
#else
      if (semctl(id, 0, IPC_RMID) < 0)
#endif
	perror("semctl");
    }
  }

  if (shmKeyTable)
    shmKeyTable->semNextKey = SemaphoreBase;

  return 0;
}
#endif

int Semaphore::setValue(int num, int sem)
{
#if defined(__sun) || defined(__solaris)
  union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
  };
#endif
#if defined(__linux) || defined(__sun) || defined(__solaris)
  union semun param;
  param.val = num;
  int result = semctl(id, sem, SETVAL, param);
#else
  int result = semctl(id, sem, SETVAL, num);
#endif
  if (result < 0) {
    perror("semctl");
    return result;
  }

#ifdef DEBUG
  cerr << "%%  Semaphore " << sem << " set to value " << num << endl;
#endif

  return num;
}

#if defined(SHARED_KEYS)
static void attachTable()
{
  shmKeyTable = 0;
  char *segment = 0;
  int created = 0;
  SharedMemory *shm = new SharedMemory(ShmKeyTableKey,
				       sizeof(struct ShmKeyTable),
				       segment, created);
  // avoid compiler warnings (unused var)
  shm = shm;
  if (!segment) {
    cerr << "Cannot attach to shared memory key table" << endl;
    return;
  }
  shmKeyTable = (struct ShmKeyTable *)segment;
  if (created) {
    shmKeyTable->semNextKey = SemaphoreBase;
    shmKeyTable->shmNextKey = SharedMemoryBase;
  }
}

key_t Semaphore::newKey()
{
  if (!shmKeyTable)
    attachTable();
  assert(shmKeyTable);
  key_t key = shmKeyTable->semNextKey++;
#ifdef DEBUG
  cerr << "Next semaphore key is " << key << endl;
#endif
  return key;
}
#endif

#if defined(PRIVATE_KEYS)
key_t Semaphore::newKey()
{
  return IPC_PRIVATE;
}
#endif

Semaphore **SemaphoreV::_sem = NULL;
int SemaphoreV::_semVectors = 0;
int SemaphoreV::_semsPerVector = 16;
int **SemaphoreV::_semUsed = NULL;
int SemaphoreV::_semUnused = 0;

SemaphoreV::SemaphoreV(key_t key, int &status, int nsem)
{
  key = key;

  _nsem = 0;
  status = -1;

  if (!_sem) {
    if (create() < 0)
      return;
  }

  DOASSERT(_sem, "Inconsistent state");

  if (_semUnused < nsem) {
    fprintf(stderr, "Cannot allocate %d semaphores (%d left)\n",
            nsem, _semUnused);
    return;
  }

  _semvec = new int [nsem];
  _semnum = new int [nsem];
  if (!_semvec || !_semnum) {
    fprintf(stderr, "Out of memory\n");
    return;
  }

  int semcnt = 0;

  for(int i = 0; i < _semVectors && semcnt < nsem; i++) {
    for(int j = 0; j < _semsPerVector && semcnt < nsem; j++) {
      if (!_semUsed[i][j]) {
        _semvec[semcnt] = i;
        _semnum[semcnt] = j;
        _semUsed[i][j] = 1;
        _semUnused--;
        _nsem++;
#ifdef DEBUG
        printf("%%  Assigned %d/%d semaphore to [%d,%d]\n",
               semcnt, nsem, i, j);
#endif
        ++semcnt;
      }
    }
  }

  if (semcnt != nsem) {
    fprintf(stderr, "Could not find %d free semaphores\n", nsem);
    return;
  }

  status = 0;
}

SemaphoreV::~SemaphoreV()
{
  for(int s = 0; s < _nsem; s++)
    _semUsed[_semvec[s]][_semnum[s]] = 0;

  _semUnused += _nsem;

  delete [] _semvec;
  delete [] _semnum;

#ifdef DEBUG
  printf("%%  Semaphores left unused %d\n", _semUnused);
#endif

  if (_semUnused < _semVectors * _semsPerVector)
    return;

#ifdef DEBUG
  printf("%%  Destroying %dx%d semaphores\n", _semVectors, _semsPerVector);
#endif

  for(int i = 0; i < _semVectors; i++) {
    _sem[i]->destroy();
    delete _sem[i];
    delete [] _semUsed[i];
  }
  delete [] _sem;
  delete [] _semUsed;

  _sem = NULL;
  _semUsed = NULL;
  _semVectors = 0;
}

int SemaphoreV::create()
{
  DOASSERT(!_sem, "Inconsistent state");

  const int maxSems = 2 * 16;
  _semVectors = maxSems / _semsPerVector;
  if (maxSems % _semsPerVector != 0)
    _semVectors++;

  _sem = new Semaphore * [_semVectors];
  _semUsed = new int * [_semVectors];
  if (!_sem || !_semUsed)
    return -1;

#ifdef DEBUG
  printf("Created %dx%d semaphores\n", _semVectors, _semsPerVector);
#endif

  for(int i = 0; i < _semVectors; i++) {
    int status;
    _sem[i] = new Semaphore(Semaphore::newKey(), status, _semsPerVector);
    if (!_sem[i] || status < 0)
      return -1;
    _semUsed[i] = new int [_semsPerVector];
    if (!_semUsed)
      return -1;
    for(int j = 0; j < _semsPerVector; j++)
      _semUsed[i][j] = 0;
  }

  _semUnused = _semVectors * _semsPerVector;

  return 0;
}

SharedMemory::SharedMemory(key_t key, int size, char *&address, int &created) :
	key(key), size(size), addr(0)
{
#if defined(DEBUG)
  printf("SharedMemory::SharedMemory(%d, %d)\n", key, size);
#endif

  // assume creation of shared memory segment fails

  address = 0;
  created = 0;
  id = -1;

  // see if we can attach to an existing shared memory segment

  if (key != IPC_PRIVATE) {
    if ((id = shmget(key, 0, SHM_R | SHM_W)) < 0) {
      if (errno != ENOENT) {
        perror("shmget");
        return;
      }
    }
  }

  if (id >= 0) {
      // successfully attached -- now do a consistency check
    struct shmid_ds sbuf;
    int result = shmctl(id, IPC_STAT, &sbuf);
    if (result < 0) {
      perror("shmctl");
      return;
    }
#ifdef DEBUG
    cerr << "%%  Attached to existing shared memory (" << sbuf.shm_segsz
         << " bytes, " << sbuf.shm_nattch << " attachments)" << endl;
#endif
#ifdef MODIFIED
    if (size > 0 && (int)sbuf.shm_segsz != size) {
#else
    if ((int)sbuf.shm_segsz != size) {
#endif
      cerr << "Existing shared memory segment has incorrect size: "
           << sbuf.shm_segsz << " vs. " << size << endl;
      cerr << "Deleting old segment" << endl;
      result = shmctl(id, IPC_RMID, 0);
      if (result < 0) {
        perror("shmctl");
        return;
      }
      id = -1;
    }
  }

  if (id < 0) {
#ifdef DEBUG
    cerr << "%%  Creating shared memory segment (" << size << " bytes)"
         << endl;
#endif
    id = shmget(key, size, SHM_R | SHM_W | IPC_CREAT);
  }

  if (id < 0) {
    perror("shmget");
    return;
  }

  created = 1;

#ifdef DEBUG
  cerr << "%%  Shared memory id " << id << endl;
#endif

  char *ptr = (char *)shmat(id, 0, 0);
  if ((long)ptr == -1) {
    perror("shmat");
    return;
  }

  address = addr = ptr;
}

int SharedMemory::destroy()
{
  if (!addr)
    return 0;

  if (shmdt(addr) < 0)
    perror("shmdt");
#ifdef DEBUG
  cerr << "%%  Removing shared memory segment " << id << endl;
#endif
  if (shmctl(id, IPC_RMID, 0) < 0)
    perror("shmctl");

  addr = 0;

  return 0;
}

#if defined(SHARED_KEYS)
int SharedMemory::destroyAll()
{
  key_t maxKey = ShmKeyTableKey + 500;
  if (shmKeyTable)
    maxKey = shmKeyTable->shmNextKey + 100;

  for(key_t key = ShmKeyTableKey; key < maxKey; key++) {
    int id;
    if ((id = shmget(key, 0, SHM_R | SHM_W)) < 0) {
      if (errno != ENOENT)
        perror("shmget");
    } else {
#ifdef DEBUG
      cerr << "%%  Removing shared memory segment " << id << endl;
#endif
      if (shmctl(id, IPC_RMID, 0) < 0)
	perror("semctl");
    }
  }

  if (shmKeyTable)
    shmKeyTable->shmNextKey = SharedMemoryBase;

  return 0;
}

key_t SharedMemory::newKey()
{
  if (!shmKeyTable)
    attachTable();
  assert(shmKeyTable);
  key_t key = shmKeyTable->shmNextKey++;
#ifdef DEBUG
  cerr << "Next shared memory key is " << key << endl;
#endif
  return key;
}
#endif

#if defined(PRIVATE_KEYS)
key_t SharedMemory::newKey()
{
  return IPC_PRIVATE;
}
#endif

#define RELEASE(sem) sems->release(1, sem)
#define ACQUIRE(sem) sems->acquire(1, sem)

IOBuffer::IOBuffer(int size) :
	size(size), sems(0), shmem(0)
{
  semKey = Semaphore::newKey();
  shmKey = SharedMemory::newKey();
}

IOBuffer::~IOBuffer()
{
  delete sems;
  delete shmem;
}

void IOBuffer::attach()
{
  // attach to semaphores

  int status;
  sems = new SemaphoreV(semKey, status, 3);
  assert(status >= 0);
  
  // attach to shared memory segment

  char *buf = 0;
  int created = 0;
  shmem = new SharedMemory(shmKey, size + 7 * sizeof(int), buf, created);
  assert(shmem && buf);
  comm = (CommBuffer *)buf;
  if (created)
    init();
}

void IOBuffer::init()
{
  assert(comm && sems);

  comm->size = shmem->getSize() - 7 * sizeof(int);
  assert(comm->size == size);
  comm->bytes = 0;
  comm->head = 0;
  comm->tail = 0;
  comm->atEOF = 0;
  comm->wReserve = 0;
  comm->rReserve = 0;
  sems->setValue(1, lock);
  sems->setValue(0, hasData);
  sems->setValue(1, hasSpace);
}

int IOBuffer::reserveW(int minUnits, int maxUnits, int bytes,
		       int &n, char *&buf)
{
  if (!minUnits)
    minUnits = 1;
  if (!maxUnits)
    maxUnits = 1;

  assert(comm && sems);
  assert(minUnits > 0 && maxUnits >= minUnits && bytes > 0);

  while(1) {
    ACQUIRE(hasSpace);                  // wait until space available
    ACQUIRE(lock);
    int space = comm->size - comm->bytes - comm->wReserve;
    if (!space) {                       // false alarm, no space?
      RELEASE(lock);
      continue;
    }

    int b = (maxUnits * bytes < space ? maxUnits * bytes : space);
    b = (b < comm->size - comm->tail ? b : comm->size - comm->tail);
    n = b / bytes;
    assert(n > 0);
    if (n < minUnits) {                 // too little space?
#ifdef DEBUG
      cerr << "reserveW asking for " << minUnits << " units, got only "
	   << n << endl;
#endif
      RELEASE(lock);
      continue;
    }

    buf = &comm->buf[comm->tail];
    b = n * bytes;
#ifdef DEBUG
    cerr << "%%  Reserved " << b << " bytes to buffer at "
         << comm->tail << ", now " << comm->wReserve << "/"
	 << comm->bytes << " bytes" << endl;
#endif
    comm->tail += b;
    if (comm->tail >= comm->size)
      comm->tail -= comm->size;
    assert(comm->tail >= 0 && comm->tail < comm->size);
    comm->wReserve += b;
    if (comm->bytes + comm->wReserve < comm->size)
      RELEASE(hasSpace);                // still space left
    RELEASE(lock);
    break;
  }

  return 0;
}

int IOBuffer::reserveR(int minUnits, int maxUnits, int bytes,
		       int &n, char *&buf)
{
  if (!minUnits)
    minUnits = 1;
  if (!maxUnits)
    maxUnits = 1;

  assert(comm && sems);
  assert(minUnits > 0 && maxUnits >= minUnits && bytes > 0);

  ACQUIRE(lock);
  if (!comm->bytes && comm->atEOF) {
    n = 0;
    buf = 0;
    RELEASE(lock);
    return 0;
  }
  RELEASE(lock);

  while(1) {
    ACQUIRE(hasData);                   // wait until data available
    ACQUIRE(lock);
    int data = comm->bytes - comm->rReserve;
    if (!data) {                        // false alarm, no data?
      int eof = comm->atEOF;
      RELEASE(lock);
      if (eof) {
	n = 0;
	buf = 0;
	break;
      }
      continue;
    }

    int b = (maxUnits * bytes < data ? maxUnits * bytes : data);
    b = (b < comm->size - comm->head ? b : comm->size - comm->head);
    n = b / bytes;
    assert(n > 0);
    if (!comm->atEOF && n < minUnits) { // too little data?
#ifdef DEBUG
      cerr << "reserveR asking for " << minUnits << " units, got only "
	   << n << endl;
#endif
      RELEASE(lock);
      continue;
    }

    buf = &comm->buf[comm->head];
    b = n * bytes;
#ifdef DEBUG
    cerr << "%%  Reserved " << b << " bytes from buffer at "
         << comm->head << ", now " << comm->rReserve << "/"
	 << comm->bytes << " bytes" << endl;
#endif
    comm->head += b;
    if (comm->head >= comm->size)
      comm->head -= comm->size;
    assert(comm->head >= 0 && comm->head < comm->size);
    comm->rReserve += b;
    if (comm->bytes - comm->rReserve > 0)
      RELEASE(hasData);                 // still data left
    RELEASE(lock);
    break;
  }

  return 0;
}

int IOBuffer::releaseW(int origUnits, int units, int bytes)
{
  assert(origUnits > 0 && units >= 0 && bytes > 0);
  ACQUIRE(lock);
  assert(comm->wReserve >= origUnits * bytes);
  comm->wReserve -= origUnits * bytes;  // move bytes from reserved state
  comm->bytes += units * bytes;         // .. to actual state
#ifdef DEBUG
  cerr << "%%  Released " << units * bytes << " of "
       << origUnits * bytes << " bytes as written" << endl;
#endif
  RELEASE(lock);
  RELEASE(hasData);                     // notify reader that data is available
  return 0;
}

int IOBuffer::releaseR(int units, int bytes)
{
  if (!units)
    return 0;
  assert(units > 0 && bytes > 0);
  ACQUIRE(lock);
  assert(comm->rReserve >= units * bytes);
  comm->rReserve -= units * bytes;      // move bytes from reserved state
  comm->bytes -= units * bytes;         // .. to actual state
#ifdef DEBUG
  cerr << "%%  Released " << units * bytes << " bytes as read" << endl;
#endif
  RELEASE(lock);
  RELEASE(hasSpace);                    // notify writer that space exists
  return 0;
}

int IOBuffer::write(char *buf, int bytes)
{
  assert(comm && sems);
  assert(comm->wReserve == 0);

  int left = bytes;
  while(left > 0) {
    ACQUIRE(hasSpace);                  // wait until space available
    ACQUIRE(lock);
    int space = comm->size - comm->bytes;
    if (!space) {                       // false alarm, no space?
      RELEASE(lock);
      continue;
    }

    int b = (left < space ? left : space);
#ifdef DEBUG
    cerr << "%%  Writing " << b << " bytes to buffer at "
         << comm->tail << ", now " << comm->bytes << " bytes" << endl;
#endif
    int atend = (b < comm->size - comm->tail ? b : comm->size - comm->tail);
    int atbeg = b - atend;
    if (atend) {
      memcpy(&comm->buf[comm->tail], &buf[bytes - left], atend);
      comm->tail += atend;
      if (comm->tail >= comm->size)     // wrap around the buffer?
	comm->tail -= comm->size;
    }      
    if (atbeg) {
      memcpy(comm->buf, &buf[bytes - left + atend], atbeg);
      comm->tail = atbeg;
    }
    assert(comm->tail >= 0 && comm->tail < comm->size);
    left -= b;
    comm->bytes += b;
    if (comm->bytes < comm->size)
      RELEASE(hasSpace);                // still space left
    RELEASE(lock);
    RELEASE(hasData);                   // notify reader that data is available
  }

  return bytes;
}

int IOBuffer::read(char *buf, int bytes)
{
  assert(comm && sems);
  assert(comm->rReserve == 0);

  ACQUIRE(lock);
  if (!comm->bytes && comm->atEOF) {
    RELEASE(lock);
    return 0;
  }
  RELEASE(lock);

  int left = bytes;
  while(left > 0) {
    ACQUIRE(hasData);                   // wait until data available
    ACQUIRE(lock);
    int data = comm->bytes;
    if (!data) {                        // false alarm, no data?
      int eof = comm->atEOF;
      RELEASE(lock);
      if (eof)
	break;
      continue;
    }

    int b = (left < data ? left : data);
#ifdef DEBUG
    cerr << "%%  Reading " << b << " bytes from buffer at "
         << comm->head << ", now " << comm->bytes << " bytes" << endl;
#endif
    int atend = (b < comm->size - comm->head ? b : comm->size - comm->head);
    int atbeg = b - atend;
    if (atend) {
      memcpy(&buf[bytes - left], &comm->buf[comm->head], atend);
      comm->head += atend;
      if (comm->head >= comm->size)     // wrap around the buffer?
	comm->head -= comm->size;
    }
    if (atbeg) {
      memcpy(&buf[bytes - left + atend], comm->buf, atbeg);
      comm->head = atbeg;
    }
    assert(comm->head >= 0 && comm->head < comm->size);
    left -= b;
    comm->bytes -= b;
    if (comm->bytes > 0)
      RELEASE(hasData);                 // still data left
    RELEASE(lock);
    RELEASE(hasSpace);                  // notify writer that space exists
  }

  return bytes - left;
}

void IOBuffer::setEOF()
{
  ACQUIRE(lock);
  assert(comm->wReserve == 0); 
  comm->atEOF = 1;
  RELEASE(lock);
  RELEASE(hasData);
}
