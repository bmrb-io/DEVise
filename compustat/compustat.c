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
#include <tcl.h>
#include <tk.h>

#include "compustat.h"
#include "tapedrive.h"

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

#define IDXFILE_NAME "compustat.idx"
static char *outfile_path = 0;
static char *tapeDrive = 0;
static char *tapeFile = 0;
static char *tapeBsize = 0;

/*-------------------------------------------------------------------*/

/* This function interfaces the Compustat extraction routines to
   TCL/TK. The path name for output files is taken from TCL. */

int comp_create(char **, int);
int create_comp_dat(char *, char *);

int comp_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  /* Allow other functions to UPDATE_TCL */

  globalInterp = interp;

  /* Get parameter values from TCL script */

  tapeDrive = Tcl_GetVar(interp, "cstat_tapeDrive", TCL_GLOBAL_ONLY);
  tapeFile = Tcl_GetVar(interp, "cstat_tapeFile", TCL_GLOBAL_ONLY);
  tapeBsize = Tcl_GetVar(interp, "cstat_tapeBsize", TCL_GLOBAL_ONLY);
  outfile_path = Tcl_GetVar(interp, "cstat_diskPath", TCL_GLOBAL_ONLY);
  if (!tapeDrive || !tapeFile || !tapeBsize || !outfile_path) {
    fprintf(stderr, "One of cstat_tapeDrive, cstat_tapeFile,\n");
    fprintf(stderr, "cstat_tapeBsize, or cstat_diskPath is undefined.\n");
    fprintf(stderr, "Define these values in $(DEVISE_LIB)/cstat.tk.\n");
    return TCL_ERROR;
  }

  printf("Reading from %s:%s (%s) to %s\n", tapeDrive, tapeFile,
	 tapeBsize, outfile_path);

  /* do not pass argv[0] (name of TCL command) */

  return comp_create(&argv[1], argc - 1);
}

/*-------------------------------------------------------------------*/

/* This is a wrapper function that makes the assumption that data files
   are to be created based on the ticker symbol. This function takes
   as parameter a set of ticker symbols (character strings). It reads the
   index file and arranges these symbols in the increasing order of offsets
   of the corresponding records (since the records are on tape) and calls
   create_comp_dat for every successive symbol. */
/* syms is the array of character strings and num is the number of strings
   passed */
int comp_create(char **syms, int num)
{
  FILE *idxfile;
  int *offset_arr;
  int *spos_arr;
  int tmp, i, j;
  char tmpp[COMP_MAX_STR_LEN];

  /* Get the index file pointer */
  if ((idxfile = fopen(IDXFILE_NAME, "r")) == NULL)
  {
    printf("Error: could not open index file\n");
    return TCL_ERROR;
  }
  
  /* We will retrieve num offsets and sort them */
  offset_arr = (int *)malloc(num*sizeof(int));
  spos_arr = (int *)malloc(num*sizeof(int));

  for (i = 0; i < num; i++)
  {
    spos_arr[i] = i;
    find_rec(idxfile, "SMBL", syms[i], &offset_arr[i], &tmp, tmpp);
    rewind(idxfile);
  }

  /* Now sort offset_arr - bubble sort for now.*/
  for (i = 0; i < num; i++)
    for (j = i+1; j < num; j++)
      if (offset_arr[i] > offset_arr[j])
      {
	tmp = offset_arr[i];
	offset_arr[i] = offset_arr[j];
	offset_arr[j] = tmp;
	tmp = spos_arr[i];
	spos_arr[i] = spos_arr[j];
	spos_arr[j] = tmp;
      }

  /* Call create_comp_dat for every symbol in turn */
  for (i = 0; i < num; i++)
  {
    char tclCmd[255];
    if (create_comp_dat("SMBL", syms[spos_arr[i]]) != TCL_OK) {
      fprintf(stderr, "Error in extracting %s\n", syms[spos_arr[i]]);
    } else {
      sprintf(tclCmd, "cstat_tapeToDisk %s", syms[spos_arr[i]]);
      if (Tcl_Eval(globalInterp, tclCmd) != TCL_OK)
	fprintf(stderr, "Error: %s\n", globalInterp->result);
      UPDATE_TCL;
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
int create_comp_dat(char *fname, char *fvalue)
{
  FILE *idxfile;
  FILE *outfile;
  int i, j;
  int recoffset, year;
  char recbuf1[COMP_REC_LENGTH];
  char recbuf2[COMP_REC_LENGTH];
  char smbl_val[COMP_MAX_STR_LEN];
  char tmpval[COMP_MAX_STR_LEN];

  TapeDrive tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return TCL_ERROR;
  }

  /* Get the index file pointer */
  if ((idxfile = fopen(IDXFILE_NAME, "r")) == NULL)
  {
    printf("Error: could not open index file\n");
    return TCL_ERROR;
  }

  /* Find the record for the company in the index file and open an output
     file with the appropriate name */
  if (find_rec(idxfile, fname, fvalue, &recoffset, &year, smbl_val) == FALSE)
  {
    printf("Error: Invalid field name, value combination\n");
    fclose(idxfile);
    return TCL_ERROR;
  }

  /* Create a name of the file based on the value of SMBL field.
     Create the output file and return the file pointer. */
  /* Create name for the data file to be generated */
  sprintf(tmpval, "%s/%s.dat", outfile_path, smbl_val);

  printf("Creating Compustat file %s\n", tmpval);

  /* Open file pointer for the data file */
  if ((outfile = fopen(tmpval, "w")) == NULL)
  {
    printf("Error: could not create file for writing data\n");
    return TCL_ERROR;
  }

  /* Loop through sets of five years-
     First loop looks at record 1 - first five years for 1 - 175 and
                         record 5 -                      176 - 350
     and so on */

  for (i = 0; i < 4; i++)
  {
    /* Read record for first record into memory */
    tape.seek(recoffset);
    tape.read(recbuf1, (size_t)COMP_REC_LENGTH);

    /* Read record for the other set of attrs eg : (1,5), (2,6), .. */
    tape.seek(recoffset + 4*COMP_REC_LENGTH);
    tape.read(recbuf2, (size_t)COMP_REC_LENGTH);

    /* Loop  for five years - pass in pointers to data arrays for each of
       the two sets of attrs */
    for (j = 0; j < 5; j++)
      generate_dat(recbuf1 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   recbuf2 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   year++, outfile);

    /* Increment recoffset - pass on to next set of five years */
    recoffset += COMP_REC_LENGTH;
  }

  /* Close files */
  if (fclose(idxfile) == EOF)
    perror("fclose");
  if (fclose(outfile) == EOF)
    perror("fclose");

  return TCL_OK;
}

/*-------------------------------------------------------------------*/

/* This function scans the index file and finds the record corr. to the
   passed fname and fvalue.
   In that record, return the OFFSET, YEAR and SMBL fields. */

int find_rec(FILE *idxfile, char fname[], char fvalue[], int *off, 
	     int *year, char *smbl_val)
{
  int i;
  int offset_pos, year_pos, smbl_pos, fname_pos;
  int offset_val;
  int year_val;
  char *fname_val;
  char tmpval[COMP_MAX_STR_LEN];

  /* First do some preprocessing : find the field numbers for OFFSET, YEAR,
     SMBL and fname fields. */
  
  offset_pos = comp_get_pos("OFFSET");
  year_pos = comp_get_pos("YEAR");
  smbl_pos = comp_get_pos("SMBL");
  fname_pos = comp_get_pos(fname);
  if (fname_pos == -1)		/* Invalid field name */
  {
    printf("Error : Invalid field name %s\n", fname);
    return FALSE;
  }
  /* If fname is also SMBL, we should read the field only once from the
     stream - so make fname_val point to smbl_val */
  if (fname_pos == smbl_pos)
    fname_val = (char *)&(smbl_val[0]);
  else
    fname_val = (char *)malloc(COMP_MAX_STR_LEN);

  /* Go in a loop - In every record save the values of OFFSET, YEAR, SMBL
     and fname fields. At the end of the loop check if the value of fname
     field is fvalue. If so, quit the loop. */

  do
  {
    for (i=0; i < COMP_NUM_IDX_FIELDS; i++)
    {
      if (i == fname_pos)
	fscanf(idxfile, "%s", fname_val);
      else if (i == offset_pos)
	fscanf(idxfile, "%d", &offset_val);
      else if (i == year_pos)
	fscanf(idxfile, "%d", &year_val);
      else if (i == smbl_pos)
	fscanf(idxfile, "%s", smbl_val);
      else 
	fscanf(idxfile, "%s", tmpval);
    }
    
  }while ((!comp_compare(fvalue, fname_val, fname_pos)) &&
	  (!feof(idxfile)));

  if (comp_compare(fvalue, fname_val, fname_pos))
  {
    *off = offset_val;
    *year = year_val;
    free(fname_val);
    return TRUE;
  }
  
  printf("Error: field value %s not found for field name %s\n",
	 fvalue, fname);
  free(fname_val);
  return FALSE;
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
  for (i = 0; i < COMP_DAT_FIELDS_1; i++)
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
  for (i = 0; i < COMP_DAT_FIELDS_2; i++)
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

/* This function returns the field position in the index file record for
   the field with the passed name. This info is got by scanning through the
   comp_idx_form struct. */
int comp_get_pos(char fname[])
{
  int pos = 0;
  
  while (pos < COMP_NUM_IDX_FIELDS)
    if (!strcmp(comp_idx_form[2*pos], fname))
      return pos;
    else
      pos++;

  return -1;
}
	
/*-------------------------------------------------------------------*/

/* This function checks if val1 and val2 are identical. Returns 1 if
   identical, 0 otherwise.
   val1 and val2 are passed as strings - but they might be either string
   values or integers. So, comparison might be using strcmp or "==".
   Use pos to check in comp_idx_form struct and find out the type. */
int comp_compare(char *val1, char *val2, int pos)
{
  if (!strcmp(comp_idx_form[2*pos + 1], "I"))	/* Integer field */
    if (atoi(val1) == atoi(val2))
      return 1;
    else return 0;
  else						/* String field */
    if (!strcmp(val1, val2))
      return 1;
    else return 0;
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

