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
  Revision 1.1  1995/11/10 18:49:48  jussi
  Initial revision.
*/

// File containing routine that extracts a security from a CRSP tape

#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <tcl.h>
#include <tk.h>

#include "sec.h"
#include "tapedrive.h"

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

/*-------------------------------------------------------------------*/

/*
   This is a wrapper function that makes the assumption that data files
   are to be created based on the CUSIP code. This function takes
   as parameter pairs of <CUSIP, cachefilename>. It reads the
   index file and arranges these symbols in the increasing order of offsets
   of the corresponding records (since the records are on tape) and extracts
   every security in turn.
*/

int crsp_create(char *tapeDrive, char *tapeFile, char *tapeBsize,
		char *idxFile, char **argv, int argc)
{
  FILE *idxfile;
  int *offset_arr;
  int *spos_arr;
  int tmp, i, j;
  int num = argc / 2;

  assert(argc % 2 == 0);

  // Get the index file pointer
  if ((idxfile = fopen(idxFile, "r")) == NULL)
  {
    printf("Error: could not open index file: %s\n", idxFile);
    return TCL_ERROR;
  }
  
  // We will retrieve num offsets and sort them
  offset_arr = (int *)malloc(num*sizeof(int));
  spos_arr = (int *)malloc(num*sizeof(int));

  for (i = 0; i < num; i++) {
    spos_arr[i] = i * 2;
    // FIX: find offset matching CUSIP number from index file
    offset_arr[i] = i * 400;
  }

  // Now sort offset_arr - bubble sort for now
  for (i = 0; i < num; i++) {
    for (j = i+1; j < num; j++) {
      if (offset_arr[i] > offset_arr[j]) {
	tmp = offset_arr[i];
	offset_arr[i] = offset_arr[j];
	offset_arr[j] = tmp;
	tmp = spos_arr[i];
	spos_arr[i] = spos_arr[j];
	spos_arr[j] = tmp;
      }
    }
  }

  fclose(idxfile);

  TapeDrive tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return TCL_ERROR;
  }

  // Extract every security in turn
  for (i = 0; i < num; i++) {
    if (tape.seek(offset_arr[spos_arr[i]]) != offset_arr[spos_arr[i]]) {
      cerr << "Error in seeking to tape position " << offset_arr[spos_arr[i]]
	   << endl;
      perror("seek");
    } else {
      Security s(tape);
      ofstream outfile(argv[spos_arr[i] + 1], ios::out);
      if (!outfile) {
	cerr << "Cannot create file " << argv[spos_arr[i] + 1] << endl;
	perror("create");
      } else {
	s.print_security(outfile);
	outfile.close();
      }
    }
  }

  free(offset_arr);
  free(spos_arr);

  return TCL_OK;
}

/*-------------------------------------------------------------------*/

/*
   This function interfaces the CRSP extraction routines to TCL/TK.
*/

int crsp_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  assert(argc >= 7);

  // Get parameter values from TCL script

  char *tapeDrive = argv[1];
  char *tapeFile = argv[2];
  char *tapeBsize = argv[3];
  char *idxFile = argv[4];

  printf("Reading from %s:%s (%s)\n", tapeDrive, tapeFile, tapeBsize);

  // pass pairs of <CUSIP, cachefilename> to extraction routine

  return crsp_create(tapeDrive, tapeFile, tapeBsize, idxFile,
		     &argv[5], argc - 5);
}
