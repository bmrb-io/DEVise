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
  Revision 1.6  1996/04/16 20:57:11  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.5  1995/12/28 18:22:28  jussi
  Removed warnings related to for loop variable scope.

  Revision 1.4  1995/11/15 21:12:21  ravim
  Fix bug in constructor - init pointers to NULL.

  Revision 1.3  1995/11/15 07:01:25  ravim
  Generates data file in print_security method.

  Revision 1.2  1995/11/09 22:42:35  jussi
  Converted to use tape drive instead of regular file. Added assertions
  just after each 'new' call. Modified get_field; dynamic memory allocation
  caused core dumps in Solaris/Pentium. The real cause may be somewhere
  else however.

  Revision 1.1  1995/11/09 15:30:56  ravim
  Initial revision
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "sec.h"
#include "cal.h"
#include "Exit.h"

#define CALFILE "dsi94.dat"

// ******************* PUBLIC FUNCTIONS ******************

Security::Security(TapeDrive &drive) : tape(drive)
{
  // Initialize pointers
  reccount = 0;
  names = NULL;
  dists = NULL;
  shares = NULL;
  delist = NULL;
  nasdin = NULL;
  bidlo = NULL;
  askhi = NULL;
  prc = NULL;
  vol = NULL;
  ret = NULL;
  sxret = bxret =  NULL;
  yrval = NULL;

  // Generate all the arrays in order - header has to be extracted first
  // since the foll. arrays use the header info in determining the size.
  if (get_header() == 0)
    return;

  get_names();
  get_dists();
  get_shares();
  get_delist();
  get_nasdin();
  get_bidlo();
  get_askhi();
  get_prc();
  get_vol();
  get_ret();
  get_sxret();
  get_bxret();
  get_yrval();
}

Security::~Security()
{
  // deallocate all the arrays
  delete [] names;
  delete [] dists;
  delete [] shares;
  delete [] delist;
  delete [] nasdin;
  delete [] bidlo;
  delete [] askhi;
  delete [] prc;
  delete [] vol;
  delete [] ret;
  delete [] sxret;
  delete [] bxret;
  delete [] yrval;
}

void Security::print_security(ostream& out)
{
  // Currently will print out only the fields of the DDATA array of crsp
  // The fields in each record are :
  // Date - in some form
  // bidlo at that date
  // askhi
  // prc
  // vol
  // ret

  FILE *calfile = 0;
  Calendar *calendar = 0;

  // Get the calendar file pointer
  if (!(calfile = fopen(CALFILE, "r")))
    printf("Error: could not open calendar file: %s\n", CALFILE);
  else
    calendar = new Calendar(calfile);

  int sdate = header.begdat;
  while (sdate <= header.enddat) {
    // if calendar file is available, translate trading date to
    // real date
    int realdate = sdate;
    if (calendar) {
      if (calendar->load_day(sdate))
	realdate = calendar->cal.caldt;
    }

    // DATE
    out << realdate << " ";
    // BIDLO and ASKHI
    if ((header.begsp == 0) || 
	(sdate < header.begsp) || (sdate > header.endsp))
      out << 0 << " "<< 0 << " ";
    else
      out << bidlo[sdate - header.begsp] << " "
	<< askhi[sdate - header.begsp] << " ";
    // PRC
    if ((header.begprc == 0) || 
	(sdate < header.begprc) || (sdate > header.endprc))
      out << 0 << " ";
    else
      out << prc[sdate - header.begprc] << " ";
    // VOL
    if ((header.begvol == 0) || 
	(sdate < header.begvol) || (sdate > header.endvol))
      out << 0 << " ";
    else
      out << vol[sdate - header.begvol] << " ";
    // RET
    if ((header.begret == 0) || 
	(sdate < header.begret) || (sdate > header.endret))
      out << 0 << " ";
    else
      out << ret[sdate - header.begret] << " ";
    
    // NEWLINE
    out << endl;
    sdate++;
  }

  if (calfile)
    fclose(calfile);
  delete calendar;
}

//******************** PRIVATE FUNCTIONS ***************

// Returns 1 on success, 0 if eof
int Security::get_next_rec()
{
  // Read CRSP_DATA_RLEN bytes into currec
  if (tape.read((void *)currec, (size_t)CRSP_DATA_RLEN) == 0)
    return 0;
  reccount++;
  return 1;
}

// Removes trailing white space from string

void Security::trim_string(char *str)
{
  int i;
  for(i = strlen(str) - 1; i >= 0; i--)
    if (!isspace(str[i]))
      break;
  str[i + 1] = 0;
}

void Security::get_field(int pos, char type, int len, int pre, void *dst)
{
  // Offset into array is pos-1
  char *from = &(currec[pos-1]);
  char oldchar;

  switch (type) {
  case 'A': 
    memcpy((char *)dst, from, len);
    ((char *)dst)[len] = '\0';
    break;

  case 'I':
    oldchar = from[len];
    from[len] = 0;
    *((int *)dst) = atoi(from);
    from[len] = oldchar;
    break;

  case 'F':
    // Here I am assuming that the decimal point is present in the 
    // field. If the point is implied, different actions need to be done.
    oldchar = from[len];
    from[len] = 0;
    *((float *)dst) = atof(from);
    from[len] = oldchar;
    break;

  default:  
    cout << "Unknown type of field"<<endl;
    break;
  }
}

int Security::get_header()
{
  if (!get_next_rec()) {
    cout << "Reached end of data file" << endl;
    return 0;
  }

  get_field(21, 'A', 8, 0, header.cusip);
  get_field(29, 'I', 6, 0, &header.permno);
  get_field(35, 'I', 6, 0, &header.compno);
  get_field(41, 'I', 6, 0, &header.issuno);
  get_field(47, 'I', 2, 0, &header.hexcd);
  get_field(49, 'I', 4, 0, &header.hsiccd);
  get_field(53, 'I', 3, 0, &header.numnam);
  get_field(56, 'I', 3, 0, &header.numdis);
  get_field(59, 'I', 3, 0, &header.numshr);
  get_field(62, 'I', 3, 0, &header.numdel);
  get_field(65, 'I', 3, 0, &header.numndi);
  get_field(68, 'I', 4, 0, &header.begdat);
  get_field(72, 'I', 4, 0, &header.enddat);
  get_field(76, 'I', 4, 0, &header.begprc);
  get_field(80, 'I', 4, 0, &header.endprc);
  get_field(84, 'I', 4, 0, &header.begsp);
  get_field(88, 'I', 4, 0, &header.endsp);
  get_field(92, 'I', 4, 0, &header.begvol);
  get_field(96, 'I', 4, 0, &header.endvol);
  get_field(100, 'I', 4, 0, &header.begret);
  get_field(104, 'I', 4, 0, &header.endret);
  get_field(108, 'I', 4, 0, &header.begsxs);
  get_field(112, 'I', 4, 0, &header.endsxs);
  get_field(116, 'I', 4, 0, &header.begbxs);
  get_field(120, 'I', 4, 0, &header.endbxs);
  get_field(124, 'I', 2, 0, &header.begyr);
  get_field(126, 'I', 2, 0, &header.endyr);
  
  return 1;
}

void Security::get_names()
{
  int len = 66;
  int numitems = header.numnam;
  int numperrec = 5;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;

  // Allocate array
  names = new names_t[numitems];
  DOASSERT(names, "Out of memory");

  for (i=0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 6, 0, &names[tot].namedt);
      get_field(j*len + 27, 'A', 8, 0, names[tot].ncusip);
      get_field(j*len + 35, 'A', 8, 0, names[tot].ticker);
      get_field(j*len + 43, 'A', 32, 0, names[tot].comnam);
      trim_string(names[tot].comnam);
      get_field(j*len + 75, 'A', 4, 0, names[tot].shrcls);
      get_field(j*len + 79, 'I', 2, 0, &names[tot].shrcd);
      get_field(j*len + 81, 'I', 2, 0, &names[tot].exchcd);
      get_field(j*len + 83, 'I', 4, 0, &names[tot].siccd);

      j++;
      tot++;
    }
  }
}

void Security::get_dists()
{
  int len = 67;
  int numitems = header.numdis;
  int numperrec = 5;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;

  // Allocate array
  dists = new dists_t[numitems];
  DOASSERT(dists, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 4, 0, &dists[tot].distcd);
      get_field(j*len + 25, 'F', 13, 6, &dists[tot].divamt);
      get_field(j*len + 38, 'F', 13, 6, &dists[tot].facpr);
      get_field(j*len + 51, 'F', 13, 6, &dists[tot].facshr);
      get_field(j*len + 64, 'I', 6, 0, &dists[tot].dclrdt);
      get_field(j*len + 70, 'I', 6, 0, &dists[tot].exdt);
      get_field(j*len + 76, 'I', 6, 0, &dists[tot].rcrddt);
      get_field(j*len + 82, 'I', 6, 0, &dists[tot].paydt);

      j++;
      tot++;
    }
  }
}

void Security::get_shares()
{
  int len = 17;
  int numitems = header.numshr;
  int numperrec = 22;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;

  // Allocate array
  shares = new shares_t[numitems];
  DOASSERT(shares, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 10, 0, &shares[tot].shrout);
      get_field(j*len + 31, 'I', 6, 0, &shares[tot].shrsdt);
      get_field(j*len + 37, 'I', 1, 0, &shares[tot].shrflg);

      j++;
      tot++;
    }
  }
}

void Security::get_delist()
{
  int len = 82;
  int numitems = header.numdel;
  int numperrec = 4;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;

  // Allocate array
  delist = new delist_t[numitems];
  DOASSERT(delist, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 6, 0, &delist[tot].dlstdt);
      get_field(j*len + 27, 'I', 3, 0, &delist[tot].dlstcd);
      get_field(j*len + 30, 'I', 6, 0, &delist[tot].nwperm);
      get_field(j*len + 36, 'I', 6, 0, &delist[tot].nextdt);
      get_field(j*len + 42, 'F', 13, 6, &delist[tot].dlbid);
      get_field(j*len + 55, 'F', 13, 6, &delist[tot].dlask);
      get_field(j*len + 68, 'F', 13, 6, &delist[tot].dlprc);
      get_field(j*len + 81, 'I', 9, 0, &delist[tot].dlvol);
      get_field(j*len + 90, 'F', 13, 6, &delist[tot].dlret);

      j++;
      tot++;
    }
  }
}

void Security::get_nasdin()
{
  int len = 14;
  int numitems = header.numndi;
  int numperrec = 27;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;

  // Allocate array
  nasdin = new nasdin_t[numitems];
  DOASSERT(nasdin, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 6, 0, &nasdin[tot].trtsdt);
      get_field(j*len + 27, 'I', 2, 0, &nasdin[tot].trtscd);
      get_field(j*len + 29, 'I', 2, 0, &nasdin[tot].nmsind);
      get_field(j*len + 31, 'I', 2, 0, &nasdin[tot].mmcnt);
      get_field(j*len + 33, 'I', 2, 0, &nasdin[tot].nsdinx);

      j++;
      tot++;
    }
  }
}

void Security::get_bidlo()
{
  int len = 13;
  int numitems = header.endsp - header.begsp  + 1;
  int numperrec = 29;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endsp <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  bidlo = new float[numitems];
  DOASSERT(bidlo, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &bidlo[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_askhi()
{
  int len = 13;
  int numitems = header.endsp - header.begsp  + 1;
  int numperrec = 29;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endsp <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  askhi = new float[numitems];
  DOASSERT(askhi, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &askhi[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_prc()
{
  int len = 13;
  int numitems = header.endprc - header.begprc  + 1;
  int numperrec = 29;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endprc <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  prc = new float[numitems];
  DOASSERT(prc, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &prc[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_vol()
{
  int len = 9;
  int numitems = header.endvol - header.begvol  + 1;
  int numperrec = 42;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endvol <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  vol = new int[numitems];
  DOASSERT(vol, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'I', 9, 0, &vol[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_ret()
{
  int len = 13;
  int numitems = header.endret - header.begret  + 1;
  int numperrec = 29;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endret <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  ret = new float[numitems];
  DOASSERT(ret, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &ret[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_sxret()
{
  int len = 13;
  int numitems = header.endsxs - header.begsxs  + 1;
  int numperrec = 29;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endsxs <= 0) return;
  if (numitems <= 0) return;

  // Allocate array
  sxret = new float[numitems];
  DOASSERT(sxret, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &sxret[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_bxret()
{
  int len = 13;
  int numitems = header.endbxs - header.begbxs  + 1;
  int numperrec = 29;
  if (numitems <= 0) return;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endbxs <= 0) return;

  // Allocate array
  bxret = new float[numitems];
  DOASSERT(bxret, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 13, 6, &bxret[tot]);

      j++;
      tot++;
    }
  }
}

void Security::get_yrval()
{
  int len = 47;
  int numitems = header.endyr - header.begyr + 1;
  int numperrec = 8;

  int i, j, tot = 0;
  int numrec = numitems/numperrec;
  if (numitems % numperrec) numrec++;
  if (header.endyr <= 0) return;

  // Allocate array
  yrval = new yrval_t[numitems];
  DOASSERT(yrval, "Out of memory");

  for(i = 0; i < numrec; i++) {
    if (!get_next_rec()) {
      cout << "Error: inconsistency in data file" << endl;
      exit(0);
    }

    j = 0;
    while ((j < numperrec) && (tot < numitems)) {
      get_field(j*len + 21, 'F', 15, 8, &yrval[tot].yrcap);
      get_field(j*len + 36, 'I', 2, 0, &yrval[tot].prcap);
      get_field(j*len + 38, 'F', 13, 6, &yrval[tot].yrsdev);
      get_field(j*len + 51, 'I', 2, 0, &yrval[tot].prsdev);
      get_field(j*len + 53, 'F', 13, 6, &yrval[tot].yrbeta);
      get_field(j*len + 66, 'I', 2, 0, &yrval[tot].prbeta);

      j++;
      tot++;
    }
  }
}
