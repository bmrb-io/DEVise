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
  Revision 1.1  1995/11/09 15:30:53  ravim
  Initial revision
*/

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
#include "tapedrive.h"

void crsp_index(char *tapeName, char *indexName);

main(int argc, char **argv)
{
  if (argc != 3) {
    cerr << "Usage: " << argv[0]
         << " <input tape device> <output index file>" << endl;
    exit(0);
  }

  crsp_index(argv[1], argv[2]);
}

//***********************************************************************

void crsp_index(char *tapeName, char *indexName)
{
  // Open data file
  TapeDrive tape(tapeName, "r", -1, 32768);
  if (!tape)  {
    cerr << "Error: could not open tape device " << tapeName << endl;
    exit(0);
  }
  tape.readTarHeader();
  unsigned long int beginning = tape.tell();

  // Open index file
  FILE *fdi = fopen(indexName, "w");
  if (!fdi)  {
    cerr << "Error: could not create index file " << indexName << endl;
    exit(0);
  }

  int recno = 0;
  unsigned long int tapepos = tape.tell();
  Security *s = new Security(tape);

  while (s->reccount) 
  {
    fprintf(fdi, "%lu,", tapepos);
    fprintf(fdi, "%d,", recno);
    fprintf(fdi, "%d,", s->header.permno);
    fprintf(fdi, "%s,", s->header.cusip);
    fprintf(fdi, "%d,", s->header.hexcd);
    fprintf(fdi, "%d,", s->header.hsiccd);
    fprintf(fdi, "\"%s\",", s->names[s->header.numnam - 1].comnam);
    fprintf(fdi, "%d,", s->header.begdat);
    fprintf(fdi, "%d\n", s->header.enddat);

    recno += s->reccount;
    delete s;

    tapepos = beginning + CRSP_DATA_RLEN * recno;
    if (tape.seek(tapepos) != tapepos)
      break;
    s = new Security(tape);
  }

  fclose(fdi);
}
