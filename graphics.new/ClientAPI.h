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

extern char *DevisePaste(int argc, char **argv);
extern int DeviseNonBlockMode(int fd);
extern int DeviseBlockMode(int fd);
extern int DeviseOpen(char *hostName, int port);
extern int DeviseReceive(int fd, int block, u_short &flag,
			 int &argc, char **&argv);
extern int DeviseSend(int fd, u_short flag, u_short bracket,
		      int argc, char **argv);
extern int DeviseClose(int fd);

const int DefaultDevisePort = 6100;

#endif
