/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:31:55  jussi
  Added CVS header.
*/

#ifndef MACHDEP_H
#define MACHDEP_H

#if defined(__hpux) && !defined(_INCLUDE_HPUX_SOURCE)
#define _INCLUDE_HPUX_SOURCE
#endif

#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>

#if !defined(__aix) && defined(_AIX)
  #define __aix
#endif
  
#if !defined(__sun)
  #if defined(sun2) || defined(sun3) || defined(sun3x) || defined(sun4) \
      || defined(sun) || defined(sparc)
    #define __sun
  #endif
#endif

#ifdef PENTIUM
#define __solaris
#endif

#if defined(__solaris) && defined(__sun)
  #undef __sun
#endif

#ifdef __ultrix
  #include <ansi_compat.h>
  #include <sys/devio.h>
#endif

#ifdef __hpux
  #if PIPE_MAX > 1048576
    #undef PIPE_MAX
    #define PIPE_MAX 1048576
  #endif
#endif

#if defined(__sun) || defined(__aix) || defined(__alpha)
  #define PIPE_MAX PIPE_BUF
#endif

#ifndef __aix
#ifdef __alpha
  #include "osf_mtio.h"
#else
  #include <sys/mtio.h>
#endif
#endif

#ifdef __aix
  #include <time.h>

  #include <sys/tape.h>
  /* need to define mapping from mtio #defines to AIX ones */
  #define MTIOCTOP   STIOCTOP
  #define MTIOCGET   /* unsupported */
  #define mtop       stop
  #define mt_op      st_op
  #define mt_count   st_count
  #define MTOFFL     STOFFL
  #define MTREW      STREW
  #define MTRETEN    STRETEN
  #define MTWEOF     STWEOF
  #define MTFSF      STFSF
  #define MTRSF      STRSF
  #define MTBSF      STRSF
  #define MTFSR      STFSR
  #define MTRSR      STRSR
  #define MTBSR      STRSR
  #define MTLOAD     STINSRT
  #define MTUNLOAD   STEJECT

  struct mtget {
    daddr_t mt_resid;
    daddr_t mt_fileno;
    daddr_t mt_blkno;
  };
#endif

#ifdef __hpux
#define FILE2FD(f) (f->__fileL + (f->__fileH << 8))
#else
#define FILE2FD(f) (f->_file)
#endif

#ifndef MIN
  #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
  
#ifndef MAX
  #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __cplusplus
  #define EXTERNC extern "C"
#else
  #define EXTERNC extern
#endif

#ifdef __cplusplus
  EXTERNC {
    #include <sys/uio.h>
  }
#else
  #include <sys/uio.h>
#endif

#if defined(__ultrix) || defined(__sun) || \
    (!defined(__cplusplus) && (defined(__aix_was_sun) || defined(__aix)))
  EXTERNC int ioctl(int, int, char *);
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#if defined(__sun)
/*  EXTERNC int shmget(key_t, u_int, u_int); */
  EXTERNC int shmget(key_t, int, int);
  EXTERNC int shmctl(int, int, struct shmid_ds *);
  EXTERNC char *shmat(int, char *, int);
  EXTERNC int shmdt(char *);
  EXTERNC int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#ifdef _cplusplus
  EXTERNC int aioread(int, char *, int, int, int, struct aio_result_t *);
  EXTERNC int aiowrite(int, char *, int, int, int, struct aio_result_t *);
  EXTERNC aio_result_t *aiowait(struct timeval *);
#endif
#endif

#if defined(__sun) || defined(__solaris)
  EXTERNC int sigblock(int);
  EXTERNC int sigsetmask(int);
#endif

#if defined(__alpha) || defined(__sun)
  EXTERNC int semget(key_t, int, int);
  EXTERNC int semctl(int, int, int, ...);
  EXTERNC int semop(int, struct sembuf *, unsigned int);

#ifndef SEM_A
#define SEM_A 0200                      // alter permission
#endif
#ifndef SEM_R
#define SEM_R 0400                      // read permission
#endif
#endif

#if defined(__hpux) || defined(__alpha)
union semun {
  int val;
  struct semid_ds *buf;
  u_short *array;
};
#endif

#if !defined(__alpha) && !defined(__hpux) && !defined(__solaris)
  EXTERNC int readv(int, struct iovec *, int);
  EXTERNC int writev(int, struct iovec *, int);
#endif

#ifdef __sun
  #define SEEK_SET 0
  #define SHM_R 0400
  #define SHM_W 0200
  #ifndef __cplusplus
    EXTERNC int printf(char *, ...);
    EXTERNC int fprintf(FILE *, char *, ...);
  #endif
#endif

#if defined(__sun) || defined(__hpux) || defined(__solaris)
  EXTERNC void perror(const char *);
#endif

#if !defined(__hpux) && !defined(__solaris)
  EXTERNC int sigpause(int sigmask);
  EXTERNC key_t ftok(char *, char);
#endif
  
#ifdef __sun
  EXTERNC int gettimeofday(struct timeval *,
			   struct timezone *);
#endif

EXTERNC int fsync(int);
EXTERNC unsigned sleep(unsigned);

#endif
