/*
 * -- 
 * -- CS739 : Distributed Systems, Spring '98, UW-Madison
 * -- Term Project : Worms For Reliable Computation
 * -- Sanjay Agrawal & Paul Salmon, {sanj,pdsalmon}@cs.wisc.edu
 * --
 * -- network.h  -- describes the networking functions.
 * --
 * --
 */

#ifndef _NETWORK_INCL_
#define _NETWORK_INCL_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <sys/wait.h>

int CreateSocket(int PortNumber);
int CreateServerTCPSocket(int PortNumber);
int CreateClientTCPSocket(int PortNumber,char *server_name);

int ReadFromSocket(int sock,char *msg, int bufferlength);
int ReadFromTCPSocket(int sock,char *msg, int bufferlength);

int WriteToSocket(int sock,int portnumber,char *hostname,
                   char *msg,int msgsize);
int WriteToTCPSocket(int sock, char *msg,int msgsize);

int IsMessageAvailableOnSocket(int sock,int timeout_sec);
int AcceptConnectOnTCPSocket(int sock);
     
void CloseSocket(int sock);
     
#endif // _NETWORK_INCL
