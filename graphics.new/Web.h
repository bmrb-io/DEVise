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

  $Log$*/

#ifndef Web_h
#define Web_h

extern int open_ftp(char *name);
extern int open_http(char *name, size_t *length);
extern int condor_bytes_stream_open_ckpt_file(char *name);

#endif
