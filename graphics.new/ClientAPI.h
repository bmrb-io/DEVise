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

#ifndef ClientAPI_h
#define ClientAPI_h

#include <sys/types.h>

#ifdef SUN
#include "missing.h"
#endif

#include "ParseAPI.h"

extern int DeviseOpen(char *hostName, int port, int control);
extern int DeviseReceive(int fd, char *result, u_short &flag, char *errorMsg);
extern int DeviseSend(int fd, char **argv, int num);
extern int DeviseClose(int fd);

const int DefaultDevisePort = 6100;

#endif
