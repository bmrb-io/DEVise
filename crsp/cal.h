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

  $Log$*/

#ifndef _CAL_H_
#define _CAL_H_

#include <iostream.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

class Calendar 
{
public:
  Calendar(int fdes);
  ~Calendar();

  // Loads the day info corr. to the specified record number
  // Returns 1 on success, 0 on failure
  int load_day(int recno);

  void print_day();

  // Type definition for the stuct for each day
  typedef struct day_t {
      int caldt;
      float vwretd;
      float vwretx;
      float ewretd;
      float ewretx;
      float totval;
      int totcnt;
      float usdval;
      int usdcnt;
      float spindx;
      float sprtrn;
    } day_t;

  // Constant declarations
  const int REC_LEN=130;

  // Variable declarations
  int fildes;

  day_t cal;		// stores info about a day

private:
  char currec[REC_LEN];

  int get_rec(int recno);

  void get_field(int pos, char type, int len, int pre, void *dst);
};

#endif
