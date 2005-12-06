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
  Revision 1.4.20.1  2005/09/06 21:20:24  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.4  1998/12/15 14:55:35  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.3  1995/11/02 16:52:44  jussi
  Updated copyright message.

  Revision 1.2  1995/09/22 15:49:45  jussi
  Added copyright message.

  Revision 1.1  1995/09/06 15:28:57  jussi
  Initial revision of archive.
*/

#ifndef ISSMDATA_H
#define ISSMDATA_H

typedef struct {
  char  symbol[12];
  char  cusip[8];
  char  pexch;
  int   n;
  int   nt;
  int   nq;
  int   ndays;
  int   ndenom;
  int   nbig;
  int   nprint;
  int   nseldy;
  int   nstop;
  int   ncor;
  int   ncon;
  int   nbnk;
  int   nmktmk;
  int   nshs;
  int   ndist;
  int   nterr;
  int   nqerr;
  int   hdr2;
  int   hdr3;
  float avgpri;
  float lowpri;
} Header1Type;

Header1Type header1;

int map1[] = { 28, 3, 12, 3, 8, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2 };

typedef struct {
  char name[30];
  char xcusip[8];
  char mof;
  char issued[15];
  char sic[4];
} Header2Type;

Header2Type header2;

int map2[] = { 11, 3, 30, 3, 8, 3, 1, 3, 15, 3, 4 };

const int bnmktm   =  48141;
const int bndist   =  120;
const int bnslst   =  20000;

float *drate, avgpri, lowpri, *shslst;

int   *shsout;
int   *idays, *iprint;
int   *imktmk, *icon;
int   *iseldy, *ibnk;
int   *istop, *idenom;
int   *icor, *ibig;
int   *big;

short *volbid, *priask;
short *cumsec, *days;
short *asksiz, *bidsiz;
short *cor, *denom;
short *exdate, *dadate;
short *dpdate, *drdate;
short *shsday, *seldy;

char  dflag[bndist][2];
char  *trdqt, *oexch, *print;
char  *bnk, *con;

#endif
