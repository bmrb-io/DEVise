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
  Revision 1.11  1995/11/30  01:55:51  ravim
  Fixed a bug with computing tape offsets.

  Revision 1.10  1995/11/20 22:39:21  jussi
  Base tape offset received from calling program.

  Revision 1.9  1995/11/17 03:57:43  ravim
  New format of index file used. Uses CUSIP number to search for company.

  Revision 1.8  1995/11/14 22:55:06  jussi
  Changed interface with Tcl. Tcl scripts now pass both company
  name and cache file where data should be extracted to.

  Revision 1.7  1995/10/18 21:03:36  ravim
  Extracts all the attrs from compustat database.

  Revision 1.6  1995/09/29 21:23:31  ravim
  Added support for precision 6.

  Revision 1.5  1995/09/22 22:36:41  jussi
  Added tape block size parameter.

  Revision 1.4  1995/09/22 15:52:14  jussi
  Added copyright message.

  Revision 1.3  1995/09/19 14:59:41  jussi
  Added log message.

  Revision 1.1  1995/09/18 18:30:32  jussi
  Initial revision of archive.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <tcl.h>
#include <tk.h>

#include "compustat.h"
#include "tapedrive.h"

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

/*-------------------------------------------------------------------*/

/* This function interfaces the Compustat extraction routines to
   TCL/TK. The path name for output files is taken from TCL. */

int comp_create(char *tapeDrive, char *tapeFile, char *tapeOff,
		char *tapeBsize, char *idxFile, char **, int);
int create_comp_dat(TapeDrive &tape, unsigned long int offset, char *file);

int comp_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  /* Allow other functions to UPDATE_TCL */

  globalInterp = interp;

  assert(argc >= 8);

  /* Get parameter values from TCL script */

  char *tapeDrive = argv[1];
  char *tapeFile = argv[2];
  char *tapeOff = argv[3];
  char *tapeBsize = argv[4];
  char *idxFile = argv[5];

  printf("Reading from %s:%s:%s (%s)\n",
	 tapeDrive, tapeFile, tapeOff, tapeBsize);

  /* do not pass argv[0] (name of TCL command) */

  return comp_create(tapeDrive, tapeFile, tapeOff, tapeBsize, idxFile,
		     &argv[6], argc - 6);
}

/*-------------------------------------------------------------------*/

/* This is a wrapper function that makes the assumption that data files
   are to be created based on the ticker symbol. This function takes
   as parameter pairs of <ticker symbol, cachefilename>. It reads the
   index file and arranges these symbols in the increasing order of offsets
   of the corresponding records (since the records are on tape) and calls
   create_comp_dat for every successive symbol. */

int comp_create(char *tapeDrive, char *tapeFile, char *tapeOff,
		char *tapeBsize, char *idxFile, char **argv, int argc)
{
  FILE *idxfile;

  assert(argc % 2 == 0);
  int num = argc / 2;

  /* Get the index file pointer */
  if ((idxfile = fopen(idxFile, "r")) == NULL)
  {
    printf("Error: could not open index file: %s\n", idxFile);
    return TCL_ERROR;
  }
  
  /* We will retrieve num offsets and sort them */
  unsigned long int *offset_arr = (unsigned long int *)
                                  malloc(num*sizeof(unsigned long int));
  int *spos_arr = (int *)malloc(num*sizeof(int));

  int i;
  for(i = 0; i < num; i++)
  {
    spos_arr[i] = i * 2;
    offset_arr[i] = find_rec(idxfile, argv[i * 2]);
    offset_arr[i] += atol(tapeOff);
    rewind(idxfile);
  }

  /* Now sort offset_arr - bubble sort for now.*/
  for(i = 0; i < num; i++)
    for(int j = i+1; j < num; j++)
      if (offset_arr[i] > offset_arr[j])
      {
	unsigned long int tmp = offset_arr[i];
	offset_arr[i] = offset_arr[j];
	offset_arr[j] = tmp;
	tmp = spos_arr[i];
	spos_arr[i] = spos_arr[j];
	spos_arr[j] = tmp;
      }

  TapeDrive tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return TCL_ERROR;
  }

  /* Call create_comp_dat for every symbol in turn */
  for(i = 0; i < num; i++)
  {
    if (create_comp_dat(tape, offset_arr[i],
			argv[spos_arr[i] + 1]) != TCL_OK) {
      fprintf(stderr, "Error in extracting %s\n", argv[spos_arr[i]]);
    }
  }

  free(offset_arr);
  free(spos_arr);
  fclose(idxfile);

  return TCL_OK;
}

/*-------------------------------------------------------------------*/

/* This function extracts the fields in the data and outputs them into
   a DeVise style file.
*/
int create_comp_dat(TapeDrive &tape, unsigned long int recoffset, char *file)
{
  FILE *outfile;
  int i, j;
  int year;
  char yrbuf[3];
  char recbuf1[COMP_REC_LENGTH];
  char recbuf2[COMP_REC_LENGTH];

  printf("Creating Compustat file %s \n",file);

  /* Open file pointer for the data file */
  if ((outfile = fopen(file, "w")) == NULL)
  {
    printf("Error: could not create file for writing data\n");
    return TCL_ERROR;
  }

  /* Loop through sets of five years-
     First loop looks at record 1 - first five years for 1 - 175 and
                         record 5 -                      176 - 350
     and so on */

  for(i = 0; i < 4; i++)
  {
    /* Read record for first record into memory */
    tape.seek(recoffset);
    tape.read(recbuf1, (size_t)COMP_REC_LENGTH);

    /* Read record for the other set of attrs eg : (1,5), (2,6), .. */
    tape.seek(recoffset + 4*COMP_REC_LENGTH);
    tape.read(recbuf2, (size_t)COMP_REC_LENGTH);

    /* Extract the data year value */
    memcpy(yrbuf, recbuf1+ 64, 2);
    yrbuf[2] = '\0';
    year = atoi(yrbuf);

    /* Loop  for five years - pass in pointers to data arrays for each of
       the two sets of attrs */
    for(j = 0; j < 5; j++)
      generate_dat(recbuf1 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   recbuf2 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   year++, outfile);

    /* Increment recoffset - pass on to next set of five years */
    recoffset += COMP_REC_LENGTH;
  }

  /* Close files */
  if (fclose(outfile) == EOF)
    perror("fclose");

  return TCL_OK;
}

/*-------------------------------------------------------------------*/

/* This function scans the index file and finds the record corr. to the
   passed cusip number.
   In that record, return the OFFSET field. */

unsigned long int find_rec(FILE *idxfile, char cval[])
{
  int tmpval;
  char tmpbuf[200];     // large enough to hold one line of index file
  unsigned long int offval;
  char fval[10];	// stores cusip here

  /* Go in a loop - In every record save the values of OFFSET, YEAR, SMBL
     fields. At the end of the loop check if the value of smbl
     field is smblval. If so, quit the loop. */

  do
  {
    /* get the cusip */
    fscanf(idxfile, "%lu,%d,%6s", &offval, &tmpval, fval);

    /* Ignore rest of line */
    fgets(tmpbuf, 200, idxfile);

  } while ((strcmp(cval, fval)) && (!feof(idxfile)));

  if (!strcmp(cval, fval))
    return offval;
  
  printf("ERROR: could not find CUSIP %s in the index file\n", cval);
  return 0;
}

/*-------------------------------------------------------------------*/

/* This function generates data corr. to the passed year.
   dat1 represents the start of the data array for the first set of 
   attrs (1-175) and dat2 is the start of the data array for the second 
   set (176-350). */
void generate_dat(char *dat1, char *dat2, int year,
		  FILE *outfile)
{
  int i, pos, len, pre;
  float val;

  /* Output year */
  fprintf(outfile, "%d ", year);

  /* output values of fields from the first data array */
  pos = 0;
  for(i = 0; i < COMP_DAT_FIELDS_1; i++)
  {
    len = comp_dat_1[i*COMP_NUM_PER_DAT];
    pre = comp_dat_1[i*COMP_NUM_PER_DAT + 1];

    val = comp_get_val(dat1+pos, len, pre);
    fprintf(outfile, "%f ", val);
    /* Increment pos based on length of this attr */
    pos += len;
  }

  /* output values of fields from the second data array */
  pos = 0;
  for(i = 0; i < COMP_DAT_FIELDS_2; i++)
  {
    len = comp_dat_2[i*COMP_NUM_PER_DAT];
    pre = comp_dat_2[i*COMP_NUM_PER_DAT + 1];

    val = comp_get_val(dat2+pos, len, pre);
    fprintf(outfile, "%f ", val);
    /* Increment pos based on length of this attr */
    pos += len;
  }

  /* Output new line - end of record for this year */
  fprintf(outfile, "\n");
}

/*-------------------------------------------------------------------*/

/* This function returns the floating point number which is represented as
   a character string with the given length and precision. */
double comp_get_val(char *str, int len, int pre)
{
  int intval, decval;
  char *intpart, *decpart;
  double denom;

  /* Extract the first (len - pre) characters out - integer part */
  intpart = (char *)malloc(len - pre +1);
  memcpy(intpart, str, len - pre);
  intpart[len - pre] = '\0';
  intval = atoi(intpart);
  free(intpart);

  /* Extract the last pre characters out - decimal part */
  decpart = (char *)malloc(pre + 1);
  memcpy(decpart, str+len-pre, pre);
  decpart[pre] = '\0';
  decval = atoi(decpart);
  free(decpart);

  /* Now add up the two parts of the float number adjusting for the 
     implied decimal point */
  /* We will optimize for the case pre=3, 4 and 6*/
  if (pre == 3)
    denom = 1000.0;
  else if (pre == 4)
    denom = 10000.0;
  else if (pre == 6)
    denom = 1000000.0;
  else
  {
    printf("Error : precision %d is not supported\n", pre);
    exit(0);
  }

  return ((double)intval + (double)decval/denom);
}

/*-------------------------------------------------------------------*/
