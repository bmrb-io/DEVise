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
  Revision 1.7  1995/12/28 18:25:32  jussi
  Removed warnings related to for loop variable scope.

  Revision 1.6  1995/11/20 22:39:58  jussi
  Base tape offset received from calling program.

  Revision 1.5  1995/11/17 04:41:02  ravim
  Fix bug.

  Revision 1.4  1995/11/17 04:05:57  ravim
  Extracts only first 6 chars of cusip for comparison.

  Revision 1.3  1995/11/15 21:11:39  ravim
  Fix bug in extract routine.

  Revision 1.2  1995/11/15 07:00:56  ravim
  Outputs crsp data file.

  Revision 1.1  1995/11/10 18:49:48  jussi
  Initial revision.
*/

// File containing routine that extracts a security from a CRSP tape

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <tk.h>

#include "sec.h"
#include "tapedrive.h"
#include "Exit.h"

//#define DEBUG

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

unsigned long int find_offset(FILE *idxfile, char cval[]);

/*-------------------------------------------------------------------*/

/*
   This is a wrapper function that makes the assumption that data files
   are to be created based on the CUSIP code. This function takes
   as parameter pairs of <CUSIP, cachefilename>. It reads the
   index file and arranges these symbols in the increasing order of offsets
   of the corresponding records (since the records are on tape) and extracts
   every security in turn.
*/

int crsp_create(char *tapeDrive, char *tapeFile, char *tapeOff,
		char *tapeBsize, char *idxFile, char **argv, int argc)
{
  FILE *idxfile;

  DOASSERT(argc % 2 == 0, "Invalid parameters");
  int num = argc / 2;

  // Get the index file pointer
  if ((idxfile = fopen(idxFile, "r")) == NULL)
  {
    printf("Error: could not open index file: %s\n", idxFile);
    return TCL_ERROR;
  }
  
  // We will retrieve num offsets and sort them
  unsigned long int *offset_arr = (unsigned long int *)
                                  malloc(num*sizeof(unsigned long int));
  int *spos_arr = (int *)malloc(num*sizeof(int));

  int i;
  for(i = 0; i < num; i++) {
    spos_arr[i] = i * 2;
    // find offset for this security by looking through the index file
    offset_arr[i] = find_offset(idxfile, argv[i * 2]);
    // add offset of beginning of tape file
    offset_arr[i] += atol(tapeOff);
    rewind(idxfile);
  }

  fclose(idxfile);

  // Now sort offset_arr - bubble sort for now
  for(i = 0; i < num; i++) {
    for(int j = i+1; j < num; j++) {
      if (offset_arr[i] > offset_arr[j]) {
	unsigned long int tmp = offset_arr[i];
	offset_arr[i] = offset_arr[j];
	offset_arr[j] = tmp;
	tmp = spos_arr[i];
	spos_arr[i] = spos_arr[j];
	spos_arr[j] = tmp;
      }
    }
  }

  TapeDrive tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return TCL_ERROR;
  }

  // Extract every security in turn
  for(i = 0; i < num; i++) {
#ifdef DEBUG
    cout << "Seeking to offset " << offset_arr[spos_arr[i]] << endl;
#endif
    if ((unsigned long)tape.seek(offset_arr[spos_arr[i]])
	!= offset_arr[spos_arr[i]]) {
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
#ifdef DEBUG
	cout << "Extracting " << argv[spos_arr[i] + 1] << endl;
#endif
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

  DOASSERT(argc >= 8, "Invalid parameters");

  // Get parameter values from TCL script

  char *tapeDrive = argv[1];
  char *tapeFile = argv[2];
  char *tapeOff = argv[3];
  char *tapeBsize = argv[4];
  char *idxFile = argv[5];

  printf("Reading from %s:%s:%s (%s)\n",
	 tapeDrive, tapeFile, tapeOff, tapeBsize);

  // pass pairs of <CUSIP, cachefilename> to extraction routine

  return crsp_create(tapeDrive, tapeFile, tapeOff, tapeBsize, idxFile,
		     &argv[6], argc - 6);
}

// Searches through the index file and finds the entry for the given 
// CUSIP number. Then, returns the tape offset for this security.

unsigned long int find_offset(FILE *idxfile, char cval[])
{
  int tmpval1, tmpval2;
  char tmpbuf[200];	// large enough to hold one line of index file
  unsigned long int offval;
  char fval[10];	// stores CUSIP in this

  do 
  {
    // get offset
    fscanf(idxfile, "%lu,%d,%d,%6s", &offval, &tmpval1, &tmpval2, fval);

    // Ignore rest of line
    fgets(tmpbuf, 200, idxfile);

  } while ((strcmp(cval, fval)) && (!feof(idxfile)));

  if (!strcmp(cval, fval))
    return offval;

  printf("ERROR: could not find CUSIP %s in the index file\n", cval);
  return 0;
}
