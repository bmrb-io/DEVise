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

  $Log$
  Revision 1.1  1995/11/09 15:30:52  ravim
  Initial revision
*/

#ifndef _CAL_H_
#define _CAL_H_

#include <stdio.h>

class Calendar
{
public:
  Calendar(FILE *fp);
  ~Calendar();

  // Loads the day info corr. to the specified record number
  // Returns 1 on success, 0 on failure
  int load_day(int recno);

  void print_day();

  // Type definition for the stuct for each day
  typedef struct day_t {
    int caldt;      // DAILY TRADING DATE.
    float vwretd;   // VALUE-WEIGHTED MARKET RETURN, INCLUDING DIVIDENDS.
    float vwretx;   // VALUE-WEIGHTED MARKET RETURN, EXCLUDING DIVIDENDS.
    float ewretd;   // EQUAL-WEIGHTED MARKET RETURN, INCLUDING DIVIDENDS.
    float ewretx;   // EQUAL-WEIGHTED MARKET RETURN, EXCLUDING DIVIDENDS.
    float totval;   // TOTAL VALUE OF STOCKS WITH NON-MISSING DATA.
    int totcnt;     // TOTAL COUNT OF STOCKS WITH NON-MISSING DATA.
    float usdval;   // TOTAL VALUE OF STOCKS USED FOR VALUE-WEIGHTING.
    int usdcnt;     // TOTAL COUNT OF STOCKS USED FOR VALUE-WEIGHTING.
    float spindx;   // LEVEL OF STANDARD AND POORS 500 COMPOSITE INDEX.
    float sprtrn;   // RETURN OF STANDARD AND POORS 500 COMPOSITE INDEX.
  } day_t;

  const int REC_LEN = 130;

  // Variable declarations
  FILE *fp;

  day_t cal;		// stores info about a day

private:
  char currec[REC_LEN];

  int get_rec(int recno);

  void get_field(int pos, char type, int len, int pre, void *dst);
};

#endif
