/*
   $Id$

   $Log$*/

#ifndef ISSMDATA_H
#define ISSMDATA_H

struct {
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
} header1;

int map1[] = { 28, 3, 12, 3, 8, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2 };

struct {
  char name[30];
  char xcusip[8];
  char mof;
  char issued[15];
  char sic[4];
} header2;

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

char  mktmkr[bnmktm][4];
char  dflag[bndist][2];
char  *trdqt, *oexch, *print;
char  *bnk, *con;

#endif
