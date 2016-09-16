#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "TasvirPatron.h"

extern int errno;

extern int TasvirPatron(char *servName, char *errmsg, int portnum)
{
  int fd, result;
  struct in_addr *ptr;
  struct hostent *servEnt;
  struct sockaddr_in servAddr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0){
    sprintf(errmsg,"Cannot create socket: %s",strerror(errno));
    return -1;
  }

  servEnt = gethostbyname(servName);
  if (!servEnt) {
    sprintf(errmsg,"Cannot translate server name: %s",strerror(errno));
    close(fd);
    return -1;
  }
  if (servEnt->h_addrtype != AF_INET) {
    sprintf(errmsg,"Unsupported address type: %s",strerror(errno));
    close(fd);
    return -1;
  }

  ptr = (struct in_addr *)*servEnt->h_addr_list;

  memset(&servAddr, 0, sizeof(struct sockaddr_in));
	
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(portnum);
  memcpy(&(servAddr.sin_addr),ptr,sizeof(struct in_addr));


  result = connect(fd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
  if (result < 0) {
    sprintf(errmsg,"Cannot connect to server: %s",strerror(errno));
    close(fd);
    return -1;
  }
  
  return fd;
}

