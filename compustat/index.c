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

  if (argc != 3)
  {
    printf("Usage: %s <input data file> <output index file>\n", argv[0]);
    exit(0);
  }

  /* Get the input file pointer */
  TapeDrive tape(argv[1], "r", -1, 8332);
  if (!tape)
  {
    fprintf(stderr, "Error: could not open tape device %s\n", argv[1]);
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
   This function scans through the entire compustat database once and
   creates an index file containing records with the following fields.

   starting offset
   industry classification code
   ...

*/
void create_index(TapeDrive &tape, FILE *outfile)
{
  char comp_rec[COMP_REC_LENGTH];	/* buffer to hold a record */
  int offset = 0;			/* Offset of first company is 0 */

  for(;;) {
    if (tape.seek(offset) != offset)
    {
      printf("Cannot seek to offset %d\n", offset);
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
    fprintf(outfile,"%i ",offset);

    /* Extract fields from first record */
    extract_fields(COMP_NUM_FIELDS_1, comp_idx_1, comp_rec, outfile);

    /* Seek to the fifth record of company */
    int cur_pos = tape.tell();
    if (tape.seek(cur_pos + 4*COMP_REC_LENGTH) != cur_pos + 4*COMP_REC_LENGTH)
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
    offset += COMP_REC_PER_COMP * COMP_REC_LENGTH;
    
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

    /* If it is a string field, we need to make some reformatting
       so that there are no spaces in it because we want to
       write out the entire field as a single string and read it back
       as a single string */
    if (field_arr[i*COMP_NUM_PER_FIELD + 2] == COMP_CHAR)
      fprintf(outfile,"%s ", comp_str_format(tmpbuf, len));
    else
      fprintf(outfile,"%i ", atoi(tmpbuf));
    free(tmpbuf);
  }
}


/* This function reformats the passed string so that the entire buffer can be
   written and read back as a single string */
char *comp_str_format(char *str, int len)
{
  int i = 0;

  /* If there is one space between two chars, change to -.
     If more that one blank, terminate the string */

  while (i < len -1)
  {
    if (isspace(str[i]))
      if (isspace(str[i+1]))
      {
	str[i]='\0';
	break;
      }
      else
	str[i]='-';
    i++;
  }

  return str;
}
