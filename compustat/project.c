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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include "compustat.h"
#include "tapedrive.h"

static int fieldNum[] = { 35, 52, 21, 22, 198 };
const int extractFields = sizeof fieldNum / sizeof fieldNum[0];
static int pos[extractFields];
static int len[extractFields];
static int pre[extractFields];

/*-------------------------------------------------------------------*/

/* This function returns the floating point number which is represented as
   a character string with the given length and precision. */

double comp_get_val(char *str, int len, int pre)
{
#if 1
  char tmp = str[len];
  str[len] = 0;
  double d = atof(str);
  str[len] = tmp;

  double denom = 1;
  if (pre == 3)
    denom = 1e3;
  else if (pre == 4)
    denom = 1e4;
  else if (pre == 6)
    denom = 1e6;
  else {
    printf("Error : precision %d is not supported\n", pre);
    exit(0);
  }

  return d / denom;
#else
  int intval, decval;
  double denom;

  /* Extract the first (len - pre) characters out - integer part */
  char tmp = str[len - pre];
  str[len - pre] = 0;
  intval = atoi(str);
  str[len - pre] = tmp;

  /* Extract the last pre characters out - decimal part */
  tmp = str[len];
  str[len] = 0;
  decval = atoi(str[len - pre]);
  str[len] = tmp;

  /* Now add up the two parts of the float number adjusting for the 
     implied decimal point */
  /* We will optimize for the case pre=3, 4 and 6*/
  if (pre == 3)
    denom = 1e3;
  else if (pre == 4)
    denom = 1e4;
  else if (pre == 6)
    denom = 1e6;
  else {
    printf("Error : precision %d is not supported\n", pre);
    exit(0);
  }

  return (double)intval + (double)decval / denom;
#endif
}

/*-------------------------------------------------------------------*/

/* This function generates data corr. to the passed year.
   dat1 represents the start of the data array for the first set of 
   attrs (1-175) and dat2 is the start of the data array for the second 
   set (176-350). */

void generate_dat(char *dat1, char *dat2, unsigned long int recid,
		  unsigned long int firm)
{
  printf("%lu,%lu", recid, firm);

  for(int i = 0; i < extractFields; i++) {
    double val = 0;
    if (fieldNum[i] < COMP_DAT_FIELDS_1) {
      val = comp_get_val(dat1 + pos[i], len[i], pre[i]);
    } else {
      val = comp_get_val(dat2 + pos[i], len[i], pre[i]);
    }
    printf(",%g", val);
  }

  printf("\n");
}

/*-------------------------------------------------------------------*/

int create_comp_dat(TapeDrive &tape, unsigned long int recoffset,
		    unsigned long int firm)
{
  static unsigned long int recid = 0;

  /* Loop through sets of five years-
     First loop looks at record 1 - first five years for 1 - 175 and
                         record 5 -                      176 - 350
     and so on */

  for(int i = 0; i < 4; i++) {
    char recbuf1[COMP_REC_LENGTH];
    char recbuf2[COMP_REC_LENGTH];

    /* Read record for first record into memory */
    tape.seek(recoffset);
    tape.read(recbuf1, (size_t)COMP_REC_LENGTH);

    /* Read record for the other set of attrs eg : (1,5), (2,6), .. */
    tape.seek(recoffset + 4*COMP_REC_LENGTH);
    tape.read(recbuf2, (size_t)COMP_REC_LENGTH);

    /* Loop  for five years - pass in pointers to data arrays for each of
       the two sets of attrs */
    for(int j = 0; j < 5; j++) {
      generate_dat(recbuf1 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   recbuf2 + COMP_DARR_OFF + j*COMP_DARR_LEN,
		   recid++, firm);
    }

    /* Increment recoffset - pass on to next set of five years */
    recoffset += COMP_REC_LENGTH;
  }

  return 0;
}

/*-------------------------------------------------------------------*/

int comp_create(char *tapeDrive, char *tapeFile, char *tapeOff,
		char *tapeBsize, char *idxFile)
{
  FILE *idxfile;

  /* Get the index file pointer */
  if (!(idxfile = fopen(idxFile, "r"))) {
    printf("Error: could not open index file: %s\n", idxFile);
    return -1;
  }
  
  TapeDrive tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape) {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return -1;
  }

  unsigned long int offset;
  unsigned long int firm = 0;

  while(firm++ < 10 && fscanf(idxfile, "%lu,%*[^\n]", &offset) == 1) {
    if (create_comp_dat(tape, offset, firm) < 0) {
      fprintf(stderr, "Error extracting at offset %lu\n", offset);
    }
  }

  fclose(idxfile);

  return 0;
}

/*-------------------------------------------------------------------*/

void findAttrInfo(int field, int &pos, int &len, int &pre)
{
  pos = 0;

  for(int i = 0; i < COMP_DAT_FIELDS_1; i++) {
    assert(pos < COMP_REC_LENGTH);
    len = comp_dat_1[i * COMP_NUM_PER_DAT];
    pre = comp_dat_1[i * COMP_NUM_PER_DAT + 1];
    if (i == field)
      return;
    pos += len;
  }

  pos = 0;

  for(int j = 0; j < COMP_DAT_FIELDS_2; j++) {
    assert(pos < COMP_REC_LENGTH);
    len = comp_dat_2[j * COMP_NUM_PER_DAT];
    pre = comp_dat_2[j * COMP_NUM_PER_DAT + 1];
    if (j == field - COMP_DAT_FIELDS_2)
      return;
    pos += len;
  }

  printf("Could not find field %d in tables!\n", field);
  exit(1);
}

/*-------------------------------------------------------------------*/

int main(int argc, char **argv)
{
  if (argc < 6) {
    printf("Usage: %s tapedrive filenum offset blocksize indexfile\n",
	   argv[0]);
    exit(1);
  }

  char *tapeDrive = argv[1];
  char *tapeFile = argv[2];
  char *tapeOff = argv[3];
  char *tapeBsize = argv[4];
  char *idxFile = argv[5];

  for(int i = 0; i < extractFields; i++) {
    findAttrInfo(fieldNum[i], pos[i], len[i], pre[i]);
    printf("Field %d is at %d, len %d, precision %d\n",
	   fieldNum[i], pos[i], len[i], pre[i]);
  }

  return comp_create(tapeDrive, tapeFile, tapeOff, tapeBsize, idxFile);
}
