/*
 * -- 
 * -- CS739 : Distributed Systems, Spring '98, UW-Madison
 * -- Term Project : Worms For Reliable Computation
 * -- Sanjay Agrawal & Paul Salmon, {sanj,pdsalmon}@cs.wisc.edu
 * --
 * -- network.cpp  -- describes the networking functions.
 * --
 * --
 */

#include "network.h"

int CreateSocket(int PortNumber)
{
  int    sock;
  struct sockaddr_in my_addr;

  sock=socket(AF_INET,SOCK_DGRAM,0);
  if (sock < 0 )
     printf("socket error: Couldn't create socket");

  bzero((char *)&my_addr,sizeof(my_addr));
  my_addr.sin_family     =AF_INET;
  my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  my_addr.sin_port       =htons(PortNumber);

  if (bind(sock,(struct sockaddr *)&my_addr,sizeof(my_addr))<0)
    {      
      perror("network error: could not bind socket");
      close(sock);
      return -2;      
    }
  
  return sock;   
}

int CreateServerTCPSocket(int PortNumber)
{
  int    sock;
  struct sockaddr_in my_addr;
  struct linger socklinger;

  sock=socket(AF_INET,SOCK_STREAM,0);
  if (sock < 0 ){
     printf("socket error: Couldn't create socket");
     exit(0);
  }

 /*
  socklinger.l_onoff =  1;
  socklinger.l_linger = 0;

  setsockopt(sock,SOL_SOCKET,SO_LINGER,(char *)&socklinger,sizeof(socklinger));
  */
  bzero((char *)&my_addr,sizeof(my_addr));
  my_addr.sin_family     =AF_INET;
  my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  my_addr.sin_port       =htons(PortNumber);

  if (bind(sock,(struct sockaddr *)&my_addr,sizeof(my_addr))<0)
    {      
      perror("network error: could not bind socket");
      return -1;      
    }
  
  listen(sock,5);

  return sock;   
}

int CreateClientTCPSocket(int PortNumber, char *server_name)
{
  int    sock;
  struct sockaddr_in my_addr,serv_addr;
  struct hostent *hp;
  struct linger socklinger;

  sock=socket(AF_INET,SOCK_STREAM,0);
  if (sock < 0 ){
     printf("socket error: Couldn't create socket");
     exit(0);
  }

  /*
  socklinger.l_onoff =  1;
  socklinger.l_linger = 0;

  setsockopt(sock,SOL_SOCKET,SO_LINGER,(char *)&socklinger,sizeof(socklinger));
   */

  bzero((char *)&my_addr,sizeof(my_addr));
  my_addr.sin_family     =AF_INET;
  my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  my_addr.sin_port       = 0;


  if (bind(sock,(struct sockaddr *)&my_addr,sizeof(my_addr))<0)
    {      
      perror("network error: could not bind socket");
      return -1;      
    }
  
   hp = (struct hostent *) gethostbyname(server_name);
   if (!hp)
    {
     perror ("CreateClientTCPSocket: gethostbyname");
      return -1;
    }


   bzero(&serv_addr,sizeof(serv_addr));
   serv_addr.sin_family = hp->h_addrtype;
   bcopy(hp->h_addr, (caddr_t)&serv_addr.sin_addr,hp->h_length);
   serv_addr.sin_port=htons(PortNumber);

   if (connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
    {
     perror ("CreateClientTCPSocket: connect");
     close(sock);
     return -1;
    }

   return sock;   
}

int ReadFromTCPSocket(int sock,char *msg, int bufferlength)
{

  int nbytes;  
  int nleft = bufferlength;

 // nbytes = recv( sock, msg, bufferlength, 0);
  while( nleft > 0 )
    {
      nbytes = recv( sock, msg, nleft, 0 );
      nleft -= nbytes;
      msg += nbytes;
    }

  return nbytes;
}

int WriteToTCPSocket(int sock,char *msg, int bufferlength)
{

 int left, nwritten;

  nwritten = send( sock, msg, bufferlength, 0 );
  printf("nwritten = %d\n",nwritten);

  return nwritten;
  /*
  left = bufferlength;
  while( left > 0 )
    {
      nwritten = send( sock, msg, left, 0 );
      left -= nwritten;
      msg += nwritten;
    }
  return bufferlength;
   */
}

int AcceptConnectOnTCPSocket(int sock)
{
  struct sockaddr_in cli_addr;
  int nsock;
  
  int clilen = sizeof(cli_addr);
  nsock  = accept(sock,(struct sockaddr *)&cli_addr,&clilen);

  if (nsock < 0) 
       {
              perror("network error: accept refused");
              exit (0);
       }
  
  return nsock;
}

int ReadFromSocket(int sock,char *msg, int bufferlength)
{

  int nbytes,  
      clilen;
  struct sockaddr_in cli_addr;
 
  clilen=sizeof(cli_addr);
  nbytes = recvfrom( 
                     sock,
                     msg,
                     bufferlength,
                     0,
                     (struct sockaddr *)&cli_addr,
                     &clilen
                   );
  if (nbytes < 0) printf("network error: recvfrom failed on socket");
  return nbytes;
}

int  WriteToSocket(int sock,int portnumber,
                         char *hostname,char *msg,int msgsize)
{
  struct sockaddr_in addr;
  struct hostent *hp;
  int nbytes;
  
  bzero((char *)&addr,sizeof(addr));
  addr.sin_port=htons(portnumber);

  /* THIS IS THE NON REENTRANT gethostbyname. CHANGE IN THE FUTURE */
  hp = (struct hostent *) gethostbyname(hostname);
  if (!hp)
     printf("network error: unknown host %s",hostname);

  addr.sin_family = hp->h_addrtype;
  bcopy(hp->h_addr, (caddr_t)&addr.sin_addr,hp->h_length);

  nbytes = sendto(sock,msg,msgsize,0,(struct sockaddr *)&addr,
                  sizeof(addr));
  if (nbytes<0)
    printf("network error: error in sending message");
  
  return nbytes;  
} 

int IsMessageAvailableOnSocket(int sock,int timeout_sec)
{
  struct timeval timeout;
  fd_set rfds;

  timeout.tv_sec=timeout_sec;
  timeout.tv_usec=0;

  FD_ZERO(&rfds);
  FD_SET(sock,&rfds);

  if (select(sock+1,&rfds,NULL,NULL,&timeout)==-1)
             printf("network error: select failed on a socket");

  return FD_ISSET(sock,&rfds);  
}


void CloseSocket(int sock)
{
  struct sockaddr name;
  int i = sizeof(name);
  if(close(sock) < 0 ) 
              printf( "network error: Couldn't close a socket" );
  getsockname(sock,(struct sockaddr *)&name,&i);
  unlink((char *)&name);

  return;
}
