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
  Revision 1.14  1998/02/09 21:12:17  donjerko
  Added Bin by clause and implementation.

  Revision 1.13  1997/12/10 00:00:36  okan
  ODBC Interface Changes ...

  Revision 1.12  1997/12/02 23:26:20  donjerko
  Changes made by Kevin

  Revision 1.11  1997/11/23 21:23:30  donjerko
  Added ODBC stuff.

  Revision 1.10  1997/11/20 05:30:42  okan
  *** empty log message ***

  Revision 1.9  1997/11/12 15:42:52  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.8.4.1  1997/10/30 20:13:38  wenger
  Got DEVise to compile, link, and run on sun and linux; compiles but
  doesn't link on hp (can't find memory mapping functions).  This includes
  moving the unidata config.h files to separate copies in the unidata
  directory for each architecture, since the config.h's are architecture-
  dependent.

  Revision 1.8  1997/09/05 22:20:10  donjerko
  Made changes for port to NT.

  Revision 1.7  1997/08/22 23:13:03  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.6  1997/03/06 02:35:30  donjerko
  Undefined DEBUG

  Revision 1.5  1997/02/03 04:11:31  donjerko
  Catalog management moved to DTE

  Revision 1.4  1996/12/21 22:21:45  donjerko
  Added hierarchical namespace.

  Revision 1.3  1996/12/05 16:05:59  wenger
  Added standard Devise file headers.

 */
//**************************************************************************
//    SockStream.C
//
//    The SockStream Class - A buffer class that provides stream
//    access to sockets.
//
//**************************************************************************

//#include <stdlib.h>   erased for sysdep.h
//#include <unistd.h>   erased for sysdep.h
//#include <iostream.h>  erased for sysdep.h 
#include "sysdep.h"
#include <string>
// #include <netdb.h>
//#include "machdep.h"	// Could be removed in standalone case
//#include <sys/types.h>   erased for sysdep.h
//#include <sys/errno.h>   erased for sysdep.h
//#include <sys/socket.h>   erased for sysdep.h
//#include <netinet/in.h>   erased for sysdep.h
//#include <arpa/inet.h>   erased for sysdep.h

//#include <stdarg.h>   erased for sysdep.h
#include <assert.h>

#include "SockStream.h"
//#include "Identify.h"

#ifdef ASYN_SOCK_REL
//#include <sys/time.h>   erased for sysdep.h
#endif

// Ugly special case to get this to work for SunOS because the system
// include files are so screwed up.  RKW 29 Oct., 1997.
// We can't include netdb.h because it defines gethostbyname() with no
// arguments.
#if defined(SUN)
struct  hostent {
        char    *h_name;        /* official name of host */
        char    **h_aliases;    /* alias list */
        int     h_addrtype;     /* host address type */
        int     h_length;       /* length of address */
        char    **h_addr_list;  /* list of addresses from name server */
#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
};

extern "C" struct hostent  *gethostbyname(const char *);
extern "C" int socket(int, int, int);
extern "C" int bind(int, struct sockaddr *, int);
extern "C" int listen(int, int);
extern "C" int accept(int, struct sockaddr *, int *);
extern "C" int connect(int, struct sockaddr *, int);
extern "C" int shutdown(int, int);
#endif

extern int errno;

//#define DEBUG_SOCK_REL

// *************************************************************************
Cor_sockbuf::Cor_sockbuf(SOCKET_TP sockfd)
{
    init();
    _socketfd = sockfd;
}

// *************************************************************************
Cor_sockbuf::Cor_sockbuf(const char* _host, unsigned short port)
{
    init();
    connect(_host, port);
}

// *************************************************************************
Cor_sockbuf::Cor_sockbuf(unsigned short _port)
{
    init();

    port = _port;
    // gethostname(host,HOSTNAME_LENGTH);

    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if( _socketfd < 0 ) {
	   perror("SockStream socket failed");
	   assert(0);
    }
    sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(port);

    // allow the port to be re-bound
    int reuse = 1;
    if( setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, 
                   (char*)&reuse, sizeof(reuse))<0) {
      perror("error setting reuse in Socket::bind");
    }

    if( bind(_socketfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0) {
      perror("bind error\n");
      perror("SockStream bind failed");
      assert(0);
      ::close(_socketfd);
      _socketfd = -1;
      return;
    }

      // listen() specifies the queue size for pending connections
    listen(_socketfd, 3);
}

// *************************************************************************
Cor_sockbuf::~Cor_sockbuf()
{
    close();
    delete _buffer;
}

// *************************************************************************
void Cor_sockbuf::init()
{
    _socketfd = -1;
    _buffer = NULL;
    get_buffer_size = 4096;
    put_buffer_size = 1024;
#if defined(__GNUG__)
    allocate();
#else
    doallocate();
#endif
}

// *************************************************************************
int Cor_sockbuf::connect(const char* _host, unsigned short _port)
{
    port = _port;
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    /* Return type of inet_addr is unsigned long, but man page says it
     * returns -1 for error.  Hmm...  RKW 11/21/96. */

    serv_addr.sin_addr.s_addr = inet_addr(_host); // try dotted decimal
    if( serv_addr.sin_addr.s_addr == (unsigned long) -1 ) {
	// not dotted decimal
        hostent *hp = gethostbyname(_host);
        if( hp == NULL ) {	// error!
	        return -1;
        }
        memcpy(&serv_addr.sin_addr, hp->h_addr, hp->h_length);
    }

    _socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if( _socketfd < 0 ) {	// error!
	   perror("SockStream socket2 failed");
	   assert(0);
        return -1;
    }

    if( ::connect(_socketfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ){
        // error!
	   perror("SockStream connect failed");
        ::close(_socketfd);
        _socketfd = -1;
        return -1;
    }

    strcpy(host,_host);
    return(0);
}

// *************************************************************************
int Cor_sockbuf::close()
{
    int rc = shutdown();
    rc |= ::close(_socketfd);
    return(rc);
}

// *************************************************************************
int Cor_sockbuf::shutdown()
{
    int rc = sync();
    rc |= ::shutdown(_socketfd, 2);
    return rc;
}

// *************************************************************************
Cor_sockbuf *Cor_sockbuf::AcceptConnection()
{
    if (!valid())
        return(NULL);

    struct sockaddr Addr;
    int AddrLen = sizeof(Addr);

    SOCKET_TP sd = accept(_socketfd, &Addr, &AddrLen);
    if (sd < 0) {
	   perror("SockStream accept failed");
	   assert(0);
        return(NULL);
    }
    return new Cor_sockbuf(sd);
}

// *************************************************************************
void Cor_sockbuf::RefuseConnection()
{
    if (!valid())
        return;

    struct sockaddr Addr;
    int AddrLen = sizeof(Addr);

      // Temporially accepting, so we can tell them to get lost.
    SOCKET_TP sd = accept(_socketfd, &Addr, &AddrLen);
    if (sd < 0) {
        perror("CoralSock: accept");
        return;
    }

    char *mess = "Connection refused by Coral Server.\n";
    send(sd,mess,strlen(mess),0);

      // Disconnect
    ::shutdown(sd,2);
    ::close(sd);
}

// *************************************************************************
int Cor_sockbuf::doallocate()
{
    int buffer_size = get_buffer_size + put_buffer_size;
    char *cp = new char[buffer_size];
    if( cp == NULL ) {
        return(EOF);
    }
    _buffer = cp;
#if defined(__GNUG__)
    setb(cp, cp + buffer_size, 0);
#endif

        // Get buffer is empty
    setg(cp, cp, cp-1);
    setp(cp + get_buffer_size, cp + buffer_size);
    return 0;
}

// *************************************************************************
int Cor_sockbuf::overflow(int c)
{
    char *cp = pbase();

    streamsize len = pptr() - cp;

#ifdef DEBUG_SOCK_REL
    // cp[len] = '\0';cout << "overflow cp = " << cp;cout.flush();
    cout << "overflow cp = ";
    cout.write(cp,len);
    cout.flush();
#endif
    
    while( len > 0 ) {
        SOCKET_TP n = EOF;
	   do {
            n = send(_socketfd, cp, len,0);
#ifdef DEBUG_SOCK_REL
//		  cerr << "write:" << _socketfd << ' ' << n << ' ' << errno << endl;
#endif
	   } while( n == -1 && errno == EINTR );
        if ( n < 0 ) {
	       perror("SockStream write failed");
	       assert(0);
        }
        len -= n;
        cp += n;
    }
    setp(pbase(), epptr());
    if ( c != EOF ) {
        return sputc(c);
    }
    return 0;
}

// *************************************************************************
int Cor_sockbuf::underflow()
{
    char *cp = gptr();

    if( egptr() > cp ) {	// chars still in buffer
        gbump(1);
        return(*cp);
    }
    sync();         // make sure our output is flushed
                    // before we wait for input
    cp = eback();

    SOCKET_TP n;
    do {
    		n = recv(_socketfd, cp, get_buffer_size,0);
	} while(n == -1 && errno == EINTR);

    if(n == 0){
	   return(EOF);
    }
	 
    if ( n < 0 ) {
	   perror("SockStream read failed");
	   assert(0);
    } 

#ifdef DEBUG_SOCK_REL
    cp[n] = '\0';cout << "underflow cp = " << cp;cout.flush();
#endif
    
    setg(cp, cp, cp + n); // no putback (at least for now...)
    return(*cp);
}

// *************************************************************************
int Cor_sockbuf::sync()
{
    return overflow(EOF);
}

#ifdef ASYN_SOCK_REL
// *************************************************************************
int Cor_sockbuf::more()
{
    fd_set mask;

    FD_ZERO(&mask);
    FD_SET(_socketfd, &mask);
    struct timeval tv;
    tv.tv_sec = tv.tv_usec = 0;

#ifdef HPUX
    if (select(FD_SETSIZE, (int*) &mask, NULL, NULL, &tv) < 0) {
#else
    if (select(FD_SETSIZE, &mask, NULL, NULL, &tv) < 0) {
#endif
        perror("Coral: Select Error ");
        return 0;
    }

    if (FD_ISSET(_socketfd, &mask))
        return 1;
    else {
        if (egptr() > gptr())  
            return 1;
        else
            return 0;
    }
}
#endif    
// *************************************************************************
