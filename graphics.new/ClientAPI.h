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

extern int DeviseHost(char *hostName, int &port);
extern int DeviseOpen(char *hostName, int port, int control);
extern int DeviseReceive(char *result, int &flag, char *errorMsg);
extern int DeviseSend(char **argv, int num);
extern int DeviseClose();

#endif
