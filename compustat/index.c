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
  Revision 1.8  1995/11/20 22:40:46  jussi
  Offset of company excludes base tape offset.

  Revision 1.7  1995/11/17 04:07:20  ravim
  New form of index file.

  Revision 1.6  1995/11/02 16:57:16  jussi
  Updated copyright message.

  Revision 1.5  1995/09/22 15:52:16  jussi
  Added copyright message.

  Revision 1.4  1995/09/19 16:42:34  jussi
  Added tape drive functionality.

  Revision 1.3  1995/09/19 14:59:44  jussi
  Added log message.

  Revision 1.1  1995/09/18 18:30:34  jussi
  Initial revision of archive.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "index.h"

main(int argc, char **argv)
{
  FILE *outfile;
  int fileno = -1;
  long int offset = 0;
  char *colon = 0;

  if (argc != 3)
  {
    printf("Usage: %s <input tape device>[:fileno[:fileoffset]] <output index file>\n", argv[0]);
    exit(0);
  }

  colon = strchr(argv[1], ':');
  if (colon) {
    char *colon2 = strchr(colon + 1, ':');
    if (colon2) {
      offset = atol(colon2 + 1);
      *colon2 = 0;
    }
    fileno = atoi(colon + 1);
    *colon = 0;
  }

  /* Get the input file pointer */
  TapeDrive tape(argv[1], "r", fileno, 32768);
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", argv[1]);
    exit(0);
  }

  if (tape.seek(offset) != offset) {
    fprintf(stderr, "Cannot seek to offset %lu\n", offset);
    perror("seek");
    exit(0);
  }

  /* Get the output file pointer */
  /* Create a new output file for writing index */
  if ((outfile = fopen(argv[2], "w")) == NULL)
  {
    printf("Error: could not create output file %s\n", argv[2]);
    exit(0);
  }

  /* Call function to create the index file */
  create_index(tape, outfile);

  /* Close files */
  if (fclose(outfile) == EOF)
    perror("fclose");
}

/*
   This function trims a string by removing trailing whitespace.
*/

static void trim_string(char *str)
{
  int i;
  for(i = strlen(str) - 1; i >= 0; i--)
    if (!isspace(str[i]))
      break;
  str[i + 1] = 0;
}

/* 
   This function scans through the entire compustat database once and
   creates an index file containing records with the following fields.

   starting offset
   industry classification code
   ...
*/

void create_index(TapeDrive &tape, FILE *outfile)
{
  char comp_rec[COMP_REC_LENGTH];	/* buffer to hold a record */
  long int offset = tape.tell(); /* Beginning of database */
  long int tapepos = offset;     /* Offset of first company */

  for(;;) {
    if (tape.seek(tapepos) != tapepos)
    {
      printf("Cannot seek to offset %lu\n", tapepos);
      break;
    }
    int bytes = tape.read((void *)comp_rec, (size_t)COMP_REC_LENGTH);
    if (!bytes)
      break;
    if (bytes != COMP_REC_LENGTH)
    {
      printf("Incomplete first record found: %d bytes\n", bytes);
      break;
    }

    /* Output the first field (offset) in the output record */
    fprintf(outfile, "%lu", tapepos - offset);

    /* Extract fields from first record */
    extract_fields(COMP_NUM_FIELDS_1, comp_idx_1, comp_rec, outfile);

    /* Seek to the fifth record of company */
    if (tape.seek(tapepos + 3*COMP_REC_LENGTH) != tapepos + 3*COMP_REC_LENGTH)
    {
      printf("Error: could not access the fifth record\n");
      break;
    }

    /* Read fifth record into memory */
    if (tape.read((void *)comp_rec, (size_t)COMP_REC_LENGTH)
	!= COMP_REC_LENGTH)
    {
      printf("Incomplete fifth record found\n");
      break;
    }    

    /* Extract fields from fifth record */
    extract_fields(COMP_NUM_FIELDS_2, comp_idx_2, comp_rec, outfile);

    /* Update offset */
    tapepos += COMP_REC_PER_COMP * COMP_REC_LENGTH;
    
    /* Put new line to denote end of one record */
    fprintf(outfile, "\n");
  }
}       


/* This function extracts the specified fields from the given record.
   Based on the type of the field, writes out appropriately to the 
   outfile.
*/
void extract_fields(int num_fields, int field_arr[], char recbuf[], 
		    FILE *outfile)
{
  int i;
  int len;
  char *tmpbuf;

  for (i=0; i < num_fields; i++)
  {
    len = field_arr[i*COMP_NUM_PER_FIELD + 1];

    /* +1 needed to put \0 at end */
    if (!(tmpbuf = (char *)malloc(len + 1)))
    {
      printf("Error in malloc\n");
      exit(0);
    }

    memcpy(tmpbuf, &recbuf[field_arr[i*COMP_NUM_PER_FIELD]],
	   len);

    tmpbuf[len] = '\0';
    trim_string(tmpbuf);

    /* A hack to ensure that the name appears in quotes */
    if ((field_arr == comp_idx_1) && 
	(field_arr[i*COMP_NUM_PER_FIELD] == 18))
      fprintf(outfile,",\"%s\"", tmpbuf);
    else if (field_arr[i*COMP_NUM_PER_FIELD + 2] == COMP_CHAR)
      fprintf(outfile,",%s", tmpbuf);
    else
      fprintf(outfile,",%d", atoi(tmpbuf));
    free(tmpbuf);
  }
}


