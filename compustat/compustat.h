/*
   $Id$

   $Log$
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
   1. starting offset of field
   2. length of field
   3. Number of digits after the decimal
   
   Eg: the first field has precision 10.3 - described by (0, 10, 3)
*/
#define COMP_NUM_PER_DAT 3

/* Description of fields to extract from the first part of data array -
   i.e. fields in the range 1 - 175
*/
#define COMP_DAT_FIELDS_1 7
int comp_dat_1[COMP_DAT_FIELDS_1 * COMP_NUM_PER_DAT] = {
    20, 10, 3,
    30, 10, 3,
    40, 10, 3,
    110, 10, 3,
    206, 8, 3,
    214, 8, 3,
    222, 8, 3
  };

/* Description of fields to extract from the second part of data array -
   i.e. fields in the range 176 - 350
*/
#define COMP_DAT_FIELDS_2 3
int comp_dat_2[COMP_DAT_FIELDS_2 * COMP_NUM_PER_DAT] = {
    30, 10, 3,
    156, 10, 3,
    268, 8, 3
  };

/* Offset to the start of the data array */
#define COMP_DARR_OFF 442

/* Length of data array portion corr. to values of one year */
#define COMP_DARR_LEN 1578

/* Max Length of string fields */
#define COMP_MAX_STR_LEN  50

/* Function prototypes */
int find_rec(FILE *idxfile, char fname[], char fvalue[], int *off, 
	      int *year, char *smbl_val);
void generate_dat(char *dat1, char *dat2, int year,
		  FILE *outfile);
int comp_get_pos(char fname[]);
int comp_compare(char *val1, char *val2, int pos);
double comp_get_val(char *str, int len, int pre);

#endif
