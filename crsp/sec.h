/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.3  1995/11/15 07:02:03  ravim
  Minor change in print_security() prototype.

  Revision 1.2  1995/11/09 22:43:18  jussi
  Converted to use tape drive instead of regular file. Added trim_string
  function to trim extra white space off of company names.

  Revision 1.1  1995/11/09 15:30:58  ravim
  Initial revision
*/

#ifndef _SEC_H_
#define _SEC_H_

#include <iostream.h>

#include "tapedrive.h"

// Constant declarations
const int CRSP_DATA_RLEN = 400;

class Security
{
public:
  // The file descriptor points to the start of the records corr. to the 
  // current security.
  Security(TapeDrive &tape);
  ~Security();

  void print_security(ostream& out);

  // data members - initially declared as pointers - will be dynamically
  // allocated at load time

  // TYPE definitions for the structures in arrays
  typedef struct header_t {
    char cusip[9];
    int permno;
    int compno;
    int issuno;
    int hexcd;
    int hsiccd;
    int numnam;
    int numdis;
    int numshr;
    int numdel;
    int numndi;
    int begdat;
    int enddat;
    int begprc;
    int endprc;
    int begsp;
    int endsp;
    int begvol;
    int endvol;
    int begret;
    int endret;
    int begsxs;
    int endsxs;
    int begbxs;
    int endbxs;
    int begyr;
    int endyr;
  } header_t;
  
  typedef struct names_t {
    int namedt;
    char ncusip[9];
    char ticker[9];
    char comnam[33];
    char shrcls[5];
    int shrcd;
    int exchcd;
    int siccd;
  } names_t;

  typedef struct dists_t {
    int distcd;
    float divamt;
    float facpr;
    float facshr;
    int dclrdt;
    int exdt;
    int rcrddt;
    int paydt;
  } dists_t;

  typedef struct shares_t {
    int shrout;
    int shrsdt;
    int shrflg;
  } shares_t;

  typedef struct delist_t {
    int dlstdt;
    int dlstcd;
    int nwperm;
    int nextdt;
    float dlbid;
    float dlask;
    float dlprc;
    int dlvol;
    float dlret;
  } delist_t;

  typedef struct nasdin_t {
    int trtsdt;
    int trtscd;
    int nmsind;
    int mmcnt;
    int nsdinx;
  } nasdin_t;
  
  typedef struct yrval_t {
    float yrcap;
    int prcap;
    float yrsdev;
    int prsdev;
    float yrbeta;
    int prbeta;
  } yrval_t;
  
  // Variable declarations
  TapeDrive &tape;

  header_t header;
  names_t *names;
  dists_t *dists;
  shares_t *shares;
  delist_t *delist;
  nasdin_t *nasdin;
  float *bidlo;
  float *askhi;
  float *prc;
  int *vol;
  float *ret;
  float *sxret;
  float *bxret;
  yrval_t *yrval;

  // Total Number of records for this security
  int reccount;

private:
  // private member functions and associated data
  char currec[CRSP_DATA_RLEN];
  int get_next_rec();
  void trim_string(char *str);
  void get_field(int pos, char type, int len, int pre, void *dst);

  int get_header();
  void get_names();
  void get_dists();
  void get_shares();
  void get_delist();
  void get_nasdin();
  void get_bidlo();
  void get_askhi();
  void get_prc();
  void get_vol();
  void get_ret();
  void get_sxret();
  void get_bxret();
  void get_yrval();
};

#endif
