/**************************************************************************
*                         Copyright (c) 1996                              *
* by Sovereign Hill Software, Inc., Amherst, Massachusetts, U.S.A.        *
*        (413-256-6916;   http://www.sovereign-hill.com)                  *
*                          All Rights Reserved.                           *
*                                                                         *
* The SOFTWARE was developed in part at the Center for Intelligent        *
* Information Retrieval (CIIR) at the University of Massachusetts at      *
* Amherst (For more information, contact 413-545-0463 or                  *
* http://ciir.cs.umass.edu).  The SOFTWARE was acquired by Sovereign      *
* Hill Software Inc. and is the property of Sovereign Hill Software.      *
* ACSIOM is authorized, under an agreement with Sovereign Hill Software,  *
* to license the SOFTWARE for non-commercial, non-revenue-producing       *
* prototyping and internal evaluation purposes only.                      *
*                                                                         *
* This SOFTWARE was developed in significant part with private funding.   *
* The INQUERY SOFTWARE is proprietary to Sovereign Hill Software and may  *
* not be distributed in whole or in part or used in any way without       *
* authorized permission in writing from Sovereign Hill Software.          *
* Possession, use, duplication or dissemination of the INQUERY SOFTWARE   *
* and media is authorized only pursuant to a valid written license from   *
* ACSIOM or Sovereign Hill Software, Inc.                                 *
*                                                                         *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.  *
* Government is subject to restrictions as set forth in subparagraph      *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.              *
**************************************************************************/ 


/*
 * IPC_COMMON is a header file defining prototypes for:
 *            select, bzero, sleep 
 *            socket, connect, send, recv, setsockopt
 * 
 * Note:  These prototypes may vary by architecture.  By placing
 *        the prototypes in this file, maintenance is improved.
 *  
 * Companion C-file:   sys_network.c
 */

#if !defined(IPC_COMMON_H)
#define IPC_COMMON_H
/*
 *  RCS identifier for module:
 */
static char ipc_common_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WATCOM
#include <unistd.h>
#endif

/*
**  The following include files define the socket function prototypes
**  for the supported INQUERY Architectures...  
**
**  _WATCOM is assumed defined when compiling for the PC platform.
**  _WATCOM #ifdefs are usually equivalent to INQ_ARCH_pc
**
**  For the UNIX-based systems, we will re-define bzero() to be memset().
**    bzero is non ANSI-C and non-POSIX.
*/

#ifdef _WATCOM   
/*
**  WIN SOCK include file
**  WIN SOCK include file
*/
# include <winsock.h>
  extern void bzero(char *b1, int length);
#elif defined(__MWERKS__)

/* nothing yet */

#else
# include <memory.h>
# define bzero(a,b) memset(a,0,b)
# include <fcntl.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>

#ifdef INQ_ARCH_rs6000
# include <sys/select.h>
#endif

# include <netdb.h>

#ifdef INQ_ARCH_solaris
struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyaddr(const char *addr, int len,int type);
#endif /* SOLARIS */

/*
 *  TCP/IP HEADER FILES
 */
# include <netinet/in.h>
# include <arpa/inet.h>

/*----------------------------------------------*
 *    INADDR_NONE not found on solaris, so....
 *    if it's not in the inet header file, 
 *    define it ourselves
 *----------------------------------------------*/
# ifndef INADDR_NONE
#   define INADDR_NONE (u_long)0xffffffff
# endif  /* INADDR_NONE */

#endif  /* _WATCOM */

/*
 *  DECNET HEADER FILES
 */
#if defined(INQ_ARCH_mips) || defined(INQ_ARCH_alpha)
# include <netdnet/dn.h>
# include <netdnet/dnetdb.h>
#endif  /* DECNET HEADER FILES (INQ_ARCH_mips/INQ_ARCH_alpha) */

/*
**  This should be included by sys/types.h....  Let's try without!
**
#ifdef INQ_ARCH_rs6000
# include <sys/select.h>
#endif
*/

#include "doctypes.h"   /* needed for feedback_function_t definition */
#include "net_ui.h"     /* definitions for INQUERY clients */

/*
**   This block sets up the INQUERY standards for checking socket
**   status.  Not all network packages return socket errors in 
**   the same manner.
*/
#ifdef _WATCOM  /* aka WINSOCK */
# define INQ_WOULDBLOCK          WSAEWOULDBLOCK
# define INQ_ERRNO               WSAGetLastError()
# define WS_VERSION_REQD         0x0101
# define WS_VERSION_MAJOR        LOBYTE(WS_VERSION_REQD)
# define WS_VERSION_MINOR        HIBYTE(WS_VERSION_REQD)
# define MIN_SOCKETS_REQD        6
#else
# define INQ_WOULDBLOCK EWOULDBLOCK
# define INQ_ERRNO errno
/*------------------------------------------------*
 *  These are defined in the winsock header file
 *    and should be used for error checking
 *    for all socket calls!
 *------------------------------------------------*/
# define SOCKET int
# define SOCKET_ERROR -1
# define INVALID_SOCKET -1
#endif /* aka WINSOCK */

/*
 *  Define the network types here ...
 *   In a central header file, in lieu of in each specific header
 *   file.
 */

#if !defined(DECNET)
#define DECNET 1
#endif
#if !defined(TCPIP)
#define TCPIP  2
#endif
#if !defined(LOCAL)
#define LOCAL  3
#endif

#define MAX_NET_BUFSIZE 8192    /* Maximum bytes to send/recv at one time */

/*
**  Re-define the O_NDELAY on suns as we are including
**   the "wrong" fcntl.h </usr/5include/fcntl.h>
*/
#if defined(sparc)
# define strerror(a) ""
# undef O_NDELAY
#  define O_NDELAY 0x04      
#endif


/*
 *  External(system, library) Function Prototypes:
 */

#ifndef _WATCOM
#if !defined(__MWERKS__) && !defined (INQ_ARCH_rs6000) && !defined(INQ_ARCH_solaris) && !defined(INQ_ARCH_alpha) && !defined(INQ_ARCH_sgi) && !defined(INQ_ARCH_dgux)

extern int send(int s, char *msg, int len, int flags);
extern int select (int nfds, fd_set *readfds, fd_set *writefds, 
                   fd_set *exceptfds, struct timeval *timeout);
extern int socket(int af, int type, int protocol);
extern int recv(int s, char *buf, int len, int flags);
extern int connect(int s, struct sockaddr *name, int namelen);
extern int bind(int s, struct sockaddr *name, int namelen);
extern int listen(int s, int backlog);
extern int accept(int s, struct sockaddr *addr, int *addrlen);
extern int setsockopt(int s, int level, int optname, char *optval, 
                       int optlen);

/*
**  The sun4 operating system does not provide prototypes
**    for the socket functions....
**    In addition, the prototypes for the various functions
**    parameters depends on the version of the compiler,
**    the version of the OS, and the day of the week.....
**    Therefore, no parameters are specified in these prototypes.
*/
#ifdef INQ_ARCH_sun4
 extern int send();
 extern int select();
 extern int socket();
 extern int recv();
 extern int connect();
 extern int bind();
 extern int listen();
 extern int accept();
 extern int setsockopt();
#endif  /* INQ_ARCH_sun4 */

#endif  /* USE_IPC_PROTOS ... !solaris && !alpha  */

/*
 *  DECNET specific prototypes
 ******************************************************
 */
#if defined(INQ_ARCH_mips)
extern int dnet_eof(int s);
extern struct dn_naddr *dnet_addr (char *cp);
#endif /* USE DECNET PROTOTYPES */

extern int  sigblock(int mask);
extern int sigsetmask(int mask);
#endif /* undefined _WATCOM */
/*
 * Function Prototypes for sys_network.c 
 */

void make_conn_async(void *connection);
void make_conn_cloexec(void *connection);

/*
** Common Connection Handle 
** ------------------------
** A common connection handle used by all the protocols.
** sockaddr is a void pointer which points to either 
**        sockaddr_dn (DECNET)
**     OR sockaddr_in (TCPIP)
**     OR sockaddr_un (IPC)
** depending on the protocol.
*/
typedef struct _conn_handle_t {
  int type;          /* network connection type */
  void *conn;        /* connection */
  void *sockaddr;    /* address (sockaddr) of the client */
  char *server_name;
  char *conn_id;
  feedback_function_t feedback_function;
  } conn_handle_t;

/*---------------------------------------------------*
 *  There is no equivalent for strerror() for 
 *    winsock WSA error returns.
 *    Therefore, have written sock_strerror() 
 *    for the PC
 *---------------------------------------------------*/
#ifdef _WATCOM
  char *sock_strerror(int err_no);
#else
# define sock_strerror strerror
#endif

/*---------------------------------------------------*
 *   The WINSOCK library is non-POSIX in that it 
 *       *does not* support read() and write().
 *   Therefore, these system calls are redefined 
 *       for the CIIR UI using the winsock library.
 *--------------------------------------------------*/
#ifdef _WATCOM
# define read(x,y,z) recv(x,y,z,0)
# define write(x,y,z) send(x,y,z,0)
#endif

void make_conn_async(void *connection);
void make_conn_cloexec(void *connection);
int sys_net_init(void);
int sys_net_term(void);
/*
**  connection handle required functions 
*/
int feedback_function_exists(void *connection);
int check_request(void *connection);
int peek_data(void *connection, char *buff, int nbytes);
Int_t write_data(void *connection, Char_t *buff, Int_t nbytes);
Int_t read_data(void *connection, Char_t *buff, Int_t nbytes);

#endif
