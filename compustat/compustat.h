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
  Revision 1.9.46.1  2005/09/12 19:41:51  wenger
  Got DEVise to compile on basslet.bmrb.wisc.edu (AMD 64/gcc
  4.0.1).

  Revision 1.9  1995/11/20 22:39:42  jussi
  Changed type of file offset to unsigned long int.

  Revision 1.8  1995/11/17 04:05:07  ravim
  New form of index file.

  Revision 1.7  1995/11/02 16:57:15  jussi
  Updated copyright message.

  Revision 1.6  1995/10/18 21:03:06  ravim
  Extracts all the attrs from compustat database.

  Revision 1.5  1995/09/22 15:52:15  jussi
  Added copyright message.

  Revision 1.4  1995/09/19 14:59:42  jussi
  Added log message.

  Revision 1.2  1995/09/18 18:32:36  jussi
  Dummy change. Ignore.

  Revision 1.1  1995/09/18 18:30:33  jussi
  Initial revision of archive.
*/

#ifndef _COMPUSTAT_H_
#define _COMPUSTAT_H_

#include "index.h"

/* Format of the index file - array of field names and field types.
   The only field types currently are Int and Character string. 
*/

#define COMP_NUM_IDX_FIELDS (COMP_NUM_FIELDS_1 + COMP_NUM_FIELDS_2 + 1)

char *comp_idx_form[COMP_NUM_IDX_FIELDS*2] = {
    "OFFSET", "I",
    "DNUM",   "I",
    "CNUM",   "C",
    "CIC",    "C",
    "NAME",   "C",
    "SMBL",   "C",
    "YEAR",   "I",
    "STATE",  "I",
    "COUNTY", "I"
  };

/* Description of data fields to extract - each field is described through 
   three numbers
   1. length of field
   2. Number of digits after the decimal

   The fields start from offset 0 and the offsets of successive fields are
   determined by the lengths of the previous fields.

   Eg: the first field has precision 10.3 - described by (10, 3)
*/
#define COMP_NUM_PER_DAT 2

/* Description of fields to extract from the first part of data array -
   i.e. fields in the range 1 - 175
*/
#define COMP_DAT_FIELDS_1 175
int comp_dat_1[COMP_DAT_FIELDS_1 * COMP_NUM_PER_DAT] = {
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    10, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 6,
    10, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,

    8, 3,
    8, 3,
    10, 4,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    10, 3,

    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,

    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3
  };

/* Description of fields to extract from the second part of data array -
   i.e. fields in the range 176 - 350
*/
#define COMP_DAT_FIELDS_2 175
int comp_dat_2[COMP_DAT_FIELDS_2 * COMP_NUM_PER_DAT] = {
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    10, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 6,
    10, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,

    8, 3,
    8, 3,
    10, 4,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,

    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,
    10, 3,

    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    10, 3,
    10, 3,
    10, 3,
    8, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,

    10, 3,
    10, 3,
    10, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,

    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,

    8, 3,
    8, 3,
    10, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    8, 3,
    10, 3,

    8, 3,
    10, 3,
    8, 3,
    8, 3,
    10, 3
  };

/* Offset to the start of the data array */
#define COMP_DARR_OFF 442

/* Length of data array portion corr. to values of one year */
#define COMP_DARR_LEN 1578

/* Max Length of string fields */
#define COMP_MAX_STR_LEN  50

/* Function prototypes */
unsigned long int find_rec(FILE *idxfile, const char cval[]);
void generate_dat(char *dat1, char *dat2, int year,
		  FILE *outfile);
double comp_get_val(char *str, int len, int pre);

#endif
