/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.4  1995/09/19 16:42:52  jussi
  Modified for attaching to tape drive.

  Revision 1.3  1995/09/19 14:59:45  jussi
  Added log message.

  Revision 1.1  1995/09/18 18:30:35  jussi
  Initial revision of archive.
*/

#ifndef _INDEX_H_
#define _INDEX_H_

#include "tapedrive.h"

/* Some basic stuff */
#define TRUE 1
#define FALSE 0

/* Some useful macros */
#define COMP_REC_LENGTH 8332
#define COMP_REC_PER_COMP 8
#define COMP_FIRST_REC_SET 1
#define COMP_SECOND_REC_SET 5

/* Codes for data types */
#define COMP_INT 1
#define COMP_CHAR 2


/* A specific field is described by three parameters
   1. offset
   2. number of bytes = length of the field
   3. type - currently either integer or character string
*/
#define COMP_NUM_PER_FIELD 3


/* Fields to extract from the first record of a company */
#define COMP_NUM_FIELDS_1 6

int comp_idx_1[COMP_NUM_FIELDS_1 * COMP_NUM_PER_FIELD] =  {
    0, 4, COMP_INT,
    4, 6, COMP_CHAR,
    10, 3, COMP_CHAR,
    18, 28, COMP_CHAR,
    46, 8, COMP_CHAR,
    64, 2, COMP_INT
  };


/* Fields to extract from the fifth record of a company */
#define COMP_NUM_FIELDS_2 2

int comp_idx_2[COMP_NUM_FIELDS_2 * COMP_NUM_PER_FIELD] = {
    16, 2, COMP_INT,
    18, 3, COMP_INT
  };

/* prototype declarations for functions */
void create_index(TapeDrive &tape, FILE *outfile);

void extract_fields(int num_fields, int field_arr[], char recbuf[], 
		    FILE *outfile);
char *comp_str_format(char *str, int len);
#endif
