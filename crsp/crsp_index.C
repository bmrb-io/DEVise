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

// File containing routine that generates the index file for a CRSP tape

// Format of a crsp index file
// The index file contains the foll. fields separated by blanks
// starting record number for this company - will be used for access later
// PERMNO 
// Latest CUSIP
// HEXCD
// HSICCD
// Latest company name
// Beginning date of data array
// end date of data array

#include <stdio.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sec.h"

#define CRSP_DATA "/afs/cs.wisc.edu/p/miron/miron/DEVise/crsp/dsm94.dat"
#define CRSP_INDEX "crsp.index"

void crsp_index();

main()
{
  crsp_index();
}

//***********************************************************************

void crsp_index()
{
  int recno = 0;
  Security *s;
  // Open data file
  int fd = open(CRSP_DATA, O_RDONLY);
  // Open index file
  FILE *fdi = fopen(CRSP_INDEX, "w");

  s = new Security(fd);
  while (s->reccount) 
  {
    fprintf(fdi, "%d ", recno);
    fprintf(fdi, "%d ", s->header.permno);
    fprintf(fdi, "%s ", s->header.cusip);
    fprintf(fdi, "%d ", s->header.hexcd);
    fprintf(fdi, "%d ", s->header.hsiccd);
    fprintf(fdi, "%s ", s->names[s->header.numnam - 1].comnam);
    fprintf(fdi, "%d ", s->header.begdat);
    fprintf(fdi, "%d\n", s->header.enddat);

    recno += s->reccount;
    delete s;

    if (lseek(fd, CRSP_DATA_RLEN * recno, SEEK_SET) == -1)
      break;
    s = new Security(fd);
  }

  close(fd);
  fclose(fdi);
}
