/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1996
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
  Revision 1.4  1995/12/28 17:31:12  jussi
  Added SGI specific code.

  Revision 1.3  1995/09/22 15:43:32  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:55  jussi
  Added CVS header.
*/

#ifndef MACHDEP_H
#define MACHDEP_H

/*
   Make sure macros indicating the platform are there.
*/

#if !defined(__aix) && defined(_AIX)
  #define __aix
#endif
  
#if !defined(__sgi) && defined(sgi)
  #define __sgi
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

/*
   Check that any application-defined platform macros
   are consistent with compiler-defined macros.
*/

#if defined(AIX) && !defined(__aix)
#error "Are you sure this is an AIX machine?"
#endif

#if defined(ALPHA) && !defined(__alpha)
#error "Are you sure this is an Alpha machine?"
#endif

#if defined(HPUX) && !defined(__hpux)
#error "Are you sure this is an HP machine?"
#endif

#if defined(PENTIUM) && !defined(__solaris)
#error "Are you sure this is a Pentium machine?"
#endif

#if defined(SGI) && !defined(__sgi)
#error "Are you sure this is an SGI machine?"
#endif

#if defined(SUN) && !defined(__sun)
#error "Are you sure this is a Sun machine?"
#endif

#if defined(ULTRIX) && !defined(__ultrix)
#error "Are you sure this is an Ultrix machine?"
#endif

#if defined(WINNT) && !defined(__winnt)
#error "Are you sure this is a Windows NT machine?"
#endif

/*
   Define general routines and data structures that are typically
   needed in low-level programming.
*/

#if defined(__hpux) && !defined(_INCLUDE_HPUX_SOURCE)
#define _INCLUDE_HPUX_SOURCE
#endif

#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>

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

/*
   Definitions related to time.
*/

#include <sys/time.h>
#include <sys/wait.h>

#ifdef __aix
  #include <time.h>
#endif

#ifdef __sun
  EXTERNC int gettimeofday(struct timeval *,
			   struct timezone *);
#endif

/*
   Definitions related to pipes, shared memory, and semaphores.
*/

#ifdef __hpux
  #if PIPE_MAX > 1048576
    #undef PIPE_MAX
    #define PIPE_MAX 1048576
  #endif
#endif

#if defined(__sun) || defined(__aix) || defined(__alpha)
  #define PIPE_MAX PIPE_BUF
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#if defined(__sun)
  #define SHM_R 0400
  #define SHM_W 0200

/*  EXTERNC int shmget(key_t, u_int, u_int); */
  EXTERNC int shmget(key_t, int, int);
  EXTERNC int shmctl(int, int, struct shmid_ds *);
  EXTERNC char *shmat(int, char *, int);
  EXTERNC int shmdt(char *);
  EXTERNC int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif

#if defined(__alpha) || defined(__sun)
  EXTERNC int semget(key_t, int, int);
  EXTERNC int semctl(int, int, int, ...);
  EXTERNC int semop(int, struct sembuf *, unsigned int);

  #ifndef SEM_A
  #define SEM_A 0200                    // alter permission
  #endif
  #ifndef SEM_R
  #define SEM_R 0400                    // read permission
  #endif
#endif

#if defined(__hpux) || defined(__alpha)
union semun {
  int val;
  struct semid_ds *buf;
  u_short *array;
};
#endif

/*
   Definitions related to signals.
*/

#if !defined(__hpux) && !defined(__solaris) && !defined(__sgi)
  EXTERNC int sigpause(int sigmask);
#endif

#if defined(__sun) || defined(__solaris)
  EXTERNC int sigblock(int);
  EXTERNC int sigsetmask(int);
#endif

/*
   Definitions related to magnetic tapes and other device I/O.
*/

#include <sys/ioctl.h>

#ifdef __sun
#ifdef _cplusplus
  EXTERNC int aioread(int, char *, int, int, int, struct aio_result_t *);
  EXTERNC int aiowrite(int, char *, int, int, int, struct aio_result_t *);
  EXTERNC aio_result_t *aiowait(struct timeval *);
#endif
#endif

#ifdef __ultrix
  #include <ansi_compat.h>
  #include <sys/devio.h>
#endif

#ifndef __aix
#ifdef __alpha
  #include "osf_mtio.h"
#else
  #include <sys/mtio.h>
#endif
#endif

#ifdef __aix
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

#if !defined(__alpha) && !defined(__hpux) && !defined(__solaris) \
    && !defined(__sgi)
  EXTERNC int readv(int, struct iovec *, int);
  EXTERNC int writev(int, struct iovec *, int);
#endif

#ifdef __sun
  #define SEEK_SET 0
  #ifndef __cplusplus
    EXTERNC int printf(char *, ...);
    EXTERNC int fprintf(FILE *, char *, ...);
  #endif
#endif

/*
   Definitions related to network I/O.
*/

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef __sun
#include <arpa/inet.h>
#else
EXTERNC char *inet_ntoa(struct in_addr);
#endif

#ifdef __alpha
EXTERNC struct hostent *gethostbyname(char *);
#endif

#if !defined(__hpux) && !defined(__alpha) && !defined(__sgi)
EXTERNC {
  extern int socket(int, int, int);
  extern int bind(int, struct sockaddr *, int);
  extern int listen(int, int);
  extern int accept(int, struct sockaddr *, int *);
  extern int connect(int, struct sockaddr *, int);
  extern int shutdown(int, int);
}
#endif

/*
   Miscellaneous definitions.
*/

#if defined(__sun) || defined(__hpux) || defined(__solaris)
  EXTERNC void perror(const char *);
#endif

#if !defined(__hpux) && !defined(__solaris) && !defined(__sgi)
  EXTERNC key_t ftok(char *, char);
#endif
  
EXTERNC int fsync(int);
EXTERNC unsigned sleep(unsigned);

#endif
