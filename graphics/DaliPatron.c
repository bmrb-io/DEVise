/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.3.48.1  2005/10/20 20:29:35  wenger
  Some errno-related fixes; added RH 7.3 stuff; clarifying note
  in linux setup script.

  Revision 1.3  1996/12/30 23:51:05  andyt
  First version with support for Embedded Tcl/Tk windows. WindowRep classes
  now have member functions for creating and destroying Tk windows.
  Interface to the EmbeddedTk server is in ETkIfc.h

  Revision 1.2  1996/11/21 19:13:46  wenger
  Fixed more compile warnings; updated devise.dali to match current
  command-line flags.

 */

#include "machdep.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "Dali.h"

int DaliPatron(char *servName, char *errmsg, int port)
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
  servAddr.sin_port   = htons(port);
  memcpy(&(servAddr.sin_addr), ptr, sizeof(struct in_addr));


  result = connect(fd,(struct sockaddr *)&servAddr,
		       sizeof(struct sockaddr));
  if (result < 0) {
    sprintf(errmsg,"Cannot connect to server: %s",strerror(errno));
    close(fd);
    return -1;
  }
  
  return fd;
}
