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
  Revision 1.5  1995/11/20 16:12:23  jussi
  Made tape file dependent offset part of data source definition,
  and not part of the offset of the securities. The first security
  starts at offset 0, and any extra offset (such as that for a
  tar header) is defined elsewhere.

  Revision 1.4  1995/11/17 04:06:55  ravim
  Extracts only first 6 chars of cusip number.

  Revision 1.3  1995/11/10 19:01:22  jussi
  Updated comments to reflect the new format of the index file.

  Revision 1.2  1995/11/09 22:44:44  jussi
  Converted to use tape drive instead of regular file. Changed
  output to comma-delimited with quotes surrounding company name
  because name may include spaces.

  Revision 1.1  1995/11/09 15:30:53  ravim
  Initial revision
*/

// File containing routine that generates the index file for a CRSP tape

// Format of a crsp index file
// The index file contains the following fields separated by commas:
//   Byte offset of record in tape file
//   Starting record number for this company - will be used for access later
//   PERMNO 
//   Latest CUSIP
//   HEXCD
//   HSICCD
//   Latest company name (enclosed in quotes)
//   Beginning date of data array
//   End date of data array

#include <iostream.h>
#include <string.h>
#include <stdlib.h>

#include "sec.h"
#include "tapedrive.h"

void crsp_index(char *tapeName, int fileno, unsigned long int offset,
		char *indexName);

main(int argc, char **argv)
{
  if (argc != 3) {
    cerr << "Usage: " << argv[0]
         << " <input tape device>[:fileno[:fileoffset]] <output index file>"
         << endl;
    exit(0);
  }

  int fileno = -1;
  unsigned long int offset = 0;
  char *colon = strchr(argv[1], ':');
  if (colon) {
    char *colon2 = strchr(colon + 1, ':');
    if (colon2) {
      offset = atol(colon2 + 1);
      *colon2 = 0;
    }
    fileno = atoi(colon + 1);
    *colon = 0;
  }

  crsp_index(argv[1], fileno, offset, argv[2]);
}

//***********************************************************************

void crsp_index(char *tapeName, int fileno, unsigned long int offset,
		char *indexName)
{
  // Open data file
  TapeDrive tape(tapeName, "r", fileno, 32768);
  if (!tape)  {
    cerr << "Error: could not open tape device " << tapeName << endl;
    exit(0);
  }
  if (tape.seek(offset) != offset) {
    cerr << "Cannot seek to offset " << offset << endl;
    perror("seek");
    exit(0);
  }

  // Open index file
  FILE *fdi = fopen(indexName, "w");
  if (!fdi)  {
    cerr << "Error: could not create index file " << indexName << endl;
    exit(0);
  }

  int recno = 0;
  unsigned long int tapepos = offset;
  Security *s = new Security(tape);

  while (s->reccount) 
  {
    fprintf(fdi, "%lu,", tapepos - offset);
    fprintf(fdi, "%d,", recno);
    fprintf(fdi, "%d,", s->header.permno);
    // Output only the first six chars of the cusip number since this is 
    // used in coral program to map with the compustat companies
    fprintf(fdi, "%.6s,", s->header.cusip);
    fprintf(fdi, "%d,", s->header.hexcd);
    fprintf(fdi, "%d,", s->header.hsiccd);
    fprintf(fdi, "\"%s\",", s->names[s->header.numnam - 1].comnam);
    fprintf(fdi, "%d,", s->header.begdat);
    fprintf(fdi, "%d\n", s->header.enddat);

    recno += s->reccount;
    delete s;

    tapepos = offset + CRSP_DATA_RLEN * recno;
    if (tape.seek(tapepos) != tapepos)
      break;
    s = new Security(tape);
  }

  fclose(fdi);
}
