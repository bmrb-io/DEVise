/*
  This file taken from the ZOO project and modified locally.
  See attached notice.
*/

/*
  $Id$

  $Log$*/

#define MODIFIED

/*
 * lc.c
 * 
 * The purpose of this program is to count program lines, excluding any 
 * blank lines or comment lines.
 *
 * By Eben M Haber 2/5/92
 *
 * I image that it will check each line.  If the length is 0, or if the first
 * non-blank character is '/' or '*', then the line doesn't count.  Otherwise
 * it's ok, and counts as a program line.
 *
 */

#include <stdio.h>

#define BUFFERSIZE 255

#ifdef MODIFIED
int
#endif
main(int argc, char *argv[])
{
  char buffer[BUFFERSIZE];    /* buffer to hold characters from the file */
  int file_index;             /* loop index if we have > 1 file */
  int index;                  /* loop index to iterate through the buffer */
  int comment_lines = 0,      /* number of comment lines seen */
      program_lines = 0,      /* number of program lines seen */
      total_lines = 0,        /* total number of lines seen */
      blank_lines = 0;        /* number of blank lines seen */
  FILE *input_file;           /* where to read from */

  /*
   * the file names will be on the command line, loop through them in case
   * there is more than 1
   */

  for (file_index = 1; file_index <= argc; file_index++)
    {
      if (argc == 1)
	input_file = stdin;
      else if (file_index == argc)
	continue;

      /*
       * if they specified something, we will assume for now that it's a file
       * name, and we will only deal with 1 file at this time
       */
      
      else
	{
	  input_file = fopen(argv[file_index],"r");
	  if (input_file == NULL)
	    {
	      printf("error opening file to read: %s\n",argv[file_index]);
	      continue;
	    }
	}
      
      /*
       * now we are ready to loop through the file, reading into the buffer and
       * processing, counting all the different kinds of lines...
       */
      
      while (fgets(buffer,BUFFERSIZE,input_file) != NULL)
	{
	  /*
	   * loop through the buffer, and decide what kind of line this is
	   */
	  
	  total_lines++;
	  index = 0;
	  
	  while ((buffer[index] == ' ') || (buffer[index] == '\t'))
	    index++;
	  
	  /*
	   * we now have a non-whitespace character.  What is it?
	   */
	  
	  switch (buffer[index])
	    {
	    case '\n':
	    case '\r': blank_lines++; break;
	      
	    case '/': 
	      if ((buffer[index+1] == '*') || (buffer[index+1] == '/'))
		comment_lines++; break;
	      
	    case '*': comment_lines++; break;
	      
	    default: program_lines++;
	    } /* end case */
	} /* end while */
    } /* end for loop */

  /*
   * we've now reached the end of the file, print the results
   */

  printf("Total lines:    %8d\n program lines: %8d\n comment lines: %8d\n blank lines:   %8d\n",total_lines,program_lines,comment_lines,blank_lines);

  if (total_lines > 0)
    printf("Percentage wise: \n program lines: %7d%% \n comment lines: %7d%% \n blank lines:   %7d%% \n",(program_lines*100)/total_lines,(comment_lines*100)/total_lines,(blank_lines*100)/total_lines);

#ifdef MODIFIED
  return 1;
#endif
}
