// (C) 1997- by Kevin S. Beyer

#include "Socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>		// for perror
#include <fcntl.h>
#include <string.h>		// for strstr
#include <iostream.h>
#include <stdlib.h>		// for atoi
#include <assert.h>

#if defined(__cygwin)
#define htonl(x) __ntohl(x)
#define htons(x) __ntohs(x)
#endif

/*
   RString Socket::local_addr(int port)
   {
   char hostname[256];
   assert(gethostname(hostname, sizeof(hostname)) == 0);
   char addr[300];
   sprintf(addr, "%s:%d", hostname, port);
   return RString(addr);
   }
   */


static int myconnect(sockaddr_in* serv_addr)
{
  int sockd;
  sockd = socket(AF_INET, SOCK_STREAM, 0);
  if( sockd < 0 ) {	// error!
    perror("couldn't create socket in Socket::connect");
    return -1;
  }
  // set close on exec for this sockd
  fcntl(sockd, F_SETFD, 1);
  if( ::connect(sockd, (sockaddr*)serv_addr, sizeof(sockaddr_in)) < 0 ){
    // error!
    ::close(sockd);
    return -1;
  }
  return sockd;
}


int Socket::connect(const char* host, int port)
{
  sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(host); // try dotted decimal
  if( int(addr.sin_addr.s_addr) == -1 ) { // not dotted decimal
    hostent* hp = gethostbyname(host);
    if( hp == NULL ) {	// error!
      return -1;
    }
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
  }
  return myconnect(&addr);
}


int Socket::connect(const char* addr)
{
  char host[128];
  const char* port = strstr(addr, ":");
  if( port == NULL ) {
    cerr << "malformed addr in Socket::connect: " << addr << '\n';
    return -1;
  }
  unsigned host_len = port - addr;
  if( host_len >= sizeof(host) ) {
    cerr << "addr too long in Socket::connect: " << addr << '\n';
    return -1;
  }
  strncpy(host, addr, host_len);
  host[host_len] = '\0';
  port++;
  return connect(host, atoi(port));
}


int Socket::bind(int& port)
{
  int sockd = socket(AF_INET, SOCK_STREAM, 0);
  if( sockd < 0 ) {
    return -1;
  }
  // set close on exec for this sockd
  fcntl(sockd, F_SETFD, 1);
  sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  // allow the port to be re-bound
  int reuse = 1;
  if( setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, 
                 (char*)&reuse, sizeof(reuse))<0) {
    perror("error setting reuse in Socket::bind");
  }
  

  if( ::bind(sockd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind error in Socket::bind");
    ::close(sockd);
    return -1;
  }

#if defined(USE_SOCKET_LINGER)&&0
  // set linger
  linger l;
  l.l_onoff = 1;
  l.l_linger = 60;
  if( setsockopt(sockd, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l)) < 0 ) {
    perror("error setting linger in Socket::bind");
  }
#endif

  if(port == 0) {
    int len = sizeof(addr);
    if( ::getsockname(sockd, (struct sockaddr*)&addr, &len) < 0 ) {
      perror("getsockname error in Socket::bind");
      ::close(sockd);
      return -1;
    }
    assert(len == sizeof(addr));
    port = addr.sin_port;
  }

  listen(sockd,SOMAXCONN);

  return sockd;
}


int Socket::accept(int sockd)
{
  sockaddr_in cli_addr;
  int cli_len = sizeof(cli_addr);
  int cli_sd = ::accept(sockd, (sockaddr*)&cli_addr, &cli_len);
  if( cli_sd < 0 ) {
    perror("accept error during Socket::accept");
    return -1;
  }
  return cli_sd;
}


int Socket::serve(int sockd, bool do_fork)
{
  if( do_fork ) {

    for(;;) {
      int cli_sd = accept(sockd);
      if( cli_sd < 0 ) return cli_sd;
      int child_pid = fork();
      if( child_pid < 0 ) {
        perror("fork error during Socket::serve");
        return -1;
      }
      if( child_pid == 0 ) {	// child process
        ::close(sockd);	// should the child close this??
        return cli_sd;
      }

      // parent process
      ::close(cli_sd);
    }
    assert(0);
    return 0;			// never gets here!

  } else {

    int cli_sd = accept(sockd);
    return cli_sd;

  }
}

int Socket::port_by_name(char* service)
{
  servent* sp = getservbyname(service, "tcp");
  if( sp == NULL ) {		// error!
    return -1;
  }
  return sp->s_port;
}


int Socket::close(int sockd)
{
  int rc = 0;
  if( (rc = ::shutdown(sockd, 2)) < 0 ) {
    perror("shutdown error");
  }
  if( (rc = ::close(sockd)) < 0 ) {
    perror("close error");
  }
  return rc;
}


int Socket::connect_samehost(int sd, int port)
{
  sockaddr_in addr = {0};
  int len = sizeof(addr);
  int rc = ::getpeername(sd, (sockaddr*)&addr, &len);
  if( rc < 0 ) {
    perror("getpeername");
    return rc;
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  return myconnect(&addr);
}
