#include <memory.h>
#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/time.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "common.h"

#define max_seg 1024

#ifdef CLIENT_DEBUG
        #define DBCLIENT(a) {cout << __FILE__ << ':' << __LINE__ << ' ' << a << endl;}

#else
        #define DBCLIENT(a) {}
#endif

int main(int argc, char** argv)
{
  int cl;
  char *outbuf, *inbuf;
  int sock, nfound;
  //static struct timeval timeout = { 0, 0 };
  struct hostent *hostp;
  fd_set rmask;
  struct sockaddr_in server;
  
  printf("Content-type: text/plain%c%c",10,10);
  cl = atoi(getenv("CONTENT_LENGTH"));
  outbuf = new char[cl];
  cin.read(outbuf,cl);

  // debugging message;
  // cout << "cl=" << cl << "\n";
  // cout.write(outbuf, cl); 
  // cout << "\n";
  // end of debugging message;

  // the following segment contains socket stuff;
  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    {
      cerr << "socket";
      delete[] outbuf;
      exit(1);
    }
  
  hostp = gethostbyname(DTEserver);
  
  memset((void *) &server, 0, sizeof server);
  server.sin_family = AF_INET;
  memcpy((void *) &server.sin_addr, hostp->h_addr, hostp->h_length);
  server.sin_port =htons((unsigned short) DTEserverport);
  
  if (connect(sock, (struct sockaddr *)&server, sizeof server) < 0)
    {
      (void) close(sock);
      cerr << "connect";
      delete[] outbuf;
      exit(1);
    }
  
  if ((write(sock, &cl, sizeof(int)))<0)
    {
      cerr << "*** write cl ***";
      (void) close(sock);
      delete[] outbuf;
      exit(1);
    }
  
  if ((write(sock, outbuf, cl))<0)
    {
      cerr << "*** write outbuf ***";
      delete[] outbuf;
      (void) close(sock);
      exit(1);
    }
  delete[] outbuf;

/*  
  FD_ZERO(&rmask);
  FD_SET(sock, &rmask);
  nfound = select(FD_SETSIZE, &rmask, (fd_set *)0, (fd_set *)0, &timeout);
  while ( (nfound<=0) || (!FD_ISSET(sock, &rmask) ))
    {
      FD_SET(sock, &rmask);
      nfound = select(FD_SETSIZE, &rmask, 0, 0, &timeout);
    }
 */

  inbuf=new char[max_seg];
  int bytesread = read(sock, inbuf, max_seg);
  
  while (bytesread>0)
    {
      cout.write(inbuf, bytesread);
      bytesread = read(sock, inbuf, max_seg);
    }
  
  delete[] inbuf;
  close(sock);
}
