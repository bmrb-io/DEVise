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
  Revision 1.7  1997/11/23 21:23:30  donjerko
  Added ODBC stuff.

  Revision 1.6  1997/09/05 22:56:08  donjerko
  *** empty log message ***

  Revision 1.5  1997/09/05 22:20:10  donjerko
  Made changes for port to NT.

  Revision 1.4  1997/08/22 23:13:03  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.3  1996/12/21 22:21:45  donjerko
  Added hierarchical namespace.

  Revision 1.2  1996/12/05 16:06:00  wenger
  Added standard Devise file headers.

 */

//**************************************************************************
//    SockStream.h
//
//    The Cor_SockStream Class - A class for providing low-level
//    socket operations for Coral programs.
//
//**************************************************************************

#ifndef _SOCKSTREAM_H_
#define _SOCKSTREAM_H_

//#include <stdio.h>   erased for sysdep.h
//#include <stdlib.h>   erased for sysdep.h
//#include <unistd.h>   erased for sysdep.h
#include <string>

//#include <sys/types.h>   erased for sysdep.h
//#include <stdarg.h>   erased for sysdep.h

//#include <iostream.h>   erased for sysdep.h
//#include <fstream.h>   erased for sysdep.h
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

//**************************************************************************

            // default PORT, unless env var CORALPORT exists 
#define CORAL_PORT   6006

            // Port used by explain to talk to server 
#define EXPLAIN_PORT 6007

            // default SERVER, unless env var CORALSERVER exists 
#define CORAL_SERVER "fontina"

#define HOSTNAME_LENGTH 100

//**************************************************************************

class Cor_sockbuf : public streambuf {

  public:

      // used to create a local socket at a specific port
    Cor_sockbuf(unsigned short Port);

      // used to create a socket connected to a remote machine 
    Cor_sockbuf(const char *HostName, unsigned short Port = CORAL_PORT);

      // create a socket using defaults, or envirornment variables,
      // or the given file descriptor (connect to a remote machine)
    Cor_sockbuf(int _sockfd = -1);

   ~Cor_sockbuf();

    int connect(const char *HostName, unsigned short Port);
    int close();
    int shutdown();

    int   sync();	// only flushes output

      // Check if this socket is valid
    int   valid() { return(_socketfd >= 0); }

    int   socketfd() { return(_socketfd); }

      // Accept or Refuse a connection
    Cor_sockbuf *AcceptConnection();
    void RefuseConnection();

    char *hostname() { return(host); }
    unsigned short my_port() { return(port); }

#ifdef ASYN_SOCK_REL
    int more(); // more stuff on this socket for read?
#endif
    
  protected:

    int   overflow(int c = EOF);
    int   underflow();
    int   doallocate();

    void  init();
    
  private:

    int   _socketfd;               // The socket's file descriptor
    char  host[HOSTNAME_LENGTH];
    unsigned short port;         

    char* _buffer;
    int get_buffer_size;
    int put_buffer_size;

    Cor_sockbuf(const Cor_sockbuf& other);
    Cor_sockbuf& operator=(const Cor_sockbuf& other);
};

//**************************************************************************

#endif
