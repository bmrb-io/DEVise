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
  Revision 1.1  1995/11/09 15:30:50  ravim
  Initial revision
*/

#include <stdio.h>
#include <stdlib.h>
#include "cal.h"

// PUBLIC FUNCTIONS
Calendar::Calendar(int fdes)
{
  fildes = fdes;
  cal.caldt = cal.totcnt = cal.usdcnt = 0;
  cal.vwretd = cal.vwretx = cal.ewretd = cal.ewretx = cal.usdval = 0;
  cal.spindx = cal.sprtrn = 0;
}

Calendar::~Calendar()
{

}

int Calendar::load_day(int recno)
{
  // Get the correct record into buffer
  if (get_rec(recno) == 0)
  {
    cout << "Could not load record for current day. "<<endl;
    return 0;
  }

  // Now load all the fields
  get_field(2, 'I', 6, 0, &cal.caldt);
  get_field(9, 'F', 13, 6, &cal.vwretd);
  get_field(23, 'F', 13, 6, &cal.vwretx);
  get_field(37, 'F', 13, 6, &cal.ewretd);
  get_field(51, 'F', 13, 6, &cal.ewretx);
  get_field(65, 'F', 13, 6, &cal.totval);
  get_field(81, 'I', 5, 0, &cal.totcnt);
  get_field(87, 'F', 15, 8, &cal.usdval);
  get_field(103, 'I', 5, 0, &cal.usdcnt);
  get_field(109, 'F', 7, 2, &cal.spindx);
  get_field(117, 'F', 13, 6, &cal.sprtrn);

  return 1;
}

void Calendar::print_day()
{
  cout << "Current day information" <<endl;
  cout << "caldt : "<<cal.caldt <<endl;
  cout <<"vwretd , vwretx: "<<cal.vwretd<<cal.vwretx<<endl;
  cout << "ewretd, ewretx : "<<cal.ewretd<<cal.ewretx<<endl;
  cout << "totval, totcnt : "<<cal.totval<<cal.totcnt<<endl;
  cout << "usdval, usdcnt : "<<cal.usdval<<cal.usdcnt<<endl;
  cout << "spindx, sprtrn : "<<cal.spindx<<cal.sprtrn<<endl;
}


// PRIVATE FUNCTIONS

// Returns 0 if eof reached ,else 1
int Calendar::get_rec(int recno) 
{
  // Seek to the appropriate record
  if (lseek(fildes, REC_LEN*recno, SEEK_SET) == -1)
  {
    cout << "Error in seeking to the record in calendar file"<< endl;
    exit(0);
  }

  // Read REC_LEN bytes into currec
  if (read(fildes, (void *)currec, (size_t)REC_LEN) == 0)
    return 0;
  else return 1;
}


void Calendar::get_field(int pos, char type, int len, int pre, void *dst)
{
  char *buf;
  // Position p means offset of (p-1)
  char *from = &(currec[pos -1]);

  switch (type) {
    case 'A': 
      memcpy((char *)dst, from, len);
      ((char *)dst)[len] = '\0';
      break;

    case 'I':
      buf = new char(len+1);
      memcpy(buf, from, len);
      buf[len] = '\0';
      *((int *)dst) = atoi(buf);
      delete buf;
      break;

    case 'F':
      // Here I am assuming that the decimal point is present in the 
      // field. If the point is implied, different actions need to be done.
      buf = new char(len+1);
      memcpy(buf, from, len);
      buf[len] = '\0';
      *((float *)dst) = atof(buf);
      delete buf;
      break;

    default:  
      cout << "Unknown type of field"<<endl;
      break;

    };
}
