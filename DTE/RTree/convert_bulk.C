/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.2  1997/03/28 16:07:48  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include"tools/correct_math.C"

#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))

main()
{
  char line[160], line2[160];
  char extra[30000];
  char entry_val[10000];
  int point_data, num_elem, num_dim, data_size, new_data_size;

  gets(line);
  sscanf(line, "%d %d %d", &num_dim, &data_size, &point_data);
  new_data_size = data_size;
  for(int Rep=0; Rep<=(1-point_data); Rep++)
    {
      char *type_info = strpbrk(line,"ivfds");
      for(int dim=0; dim<num_dim; dim++)
	{
	  switch (type_info[0]) 
	    {
	    case 'v': case 'd':
	      new_data_size += 4;
	      break;
	    case 's':
	      int len;
	      sscanf(type_info+2, "%d", &len);
	      type_info = strpbrk(type_info, "]");
	      new_data_size += maximum(0, len-4);
	      break;
	    default:
	      break;
	    }
	  type_info++;
	}
    }
  int size=sprintf(line2, "%d %d %d", num_dim, new_data_size, point_data);
  strncat(line2, "          ", (8-((size+1)%8))%8);
  strncat(line2, "\n", 1);
  fwrite(line2, size+1+(8-((size+1)%8))%8, 1, stdout);
  
  while(1)
    {
      int place_in_extra = 0;
      for(int Rep=0; Rep<=(1-point_data); Rep++)
	{
	  char *type_info = strpbrk(line,"ivfds");
	  for(int dim=0; dim<num_dim; dim++)
	    {
	      switch (type_info[0]) 
		{
		case 'i':
		  fread(entry_val, 4, 1, stdin);
		  if (feof(stdin))
		    exit(0);
		  fwrite(entry_val, 4, 1, stdout);
		  break;
		case 'f':
		  fread(entry_val, 4, 1, stdin);
		  if (feof(stdin))
		    exit(0);
		  flip_bits(*((float *)entry_val));
		  fwrite(entry_val, 4, 1, stdout);
		  break;
		case 'v':
		  fread(entry_val, 8, 1, stdin);
		  if (feof(stdin))
		    exit(0);
		  fwrite((char *)&FIRST_WORD(*entry_val), 4, 1, stdout);
		  memcpy(extra+place_in_extra, (char *)&LAST_WORD(*entry_val), 4);
		  place_in_extra += 4;
		  break;
		case 'd':
		  fread(entry_val, 8, 1, stdin);
		  if (feof(stdin))
		    exit(0);
		  flip_bits(*((double *)entry_val));
		  fwrite((char *)&FIRST_WORD(*entry_val), 4, 1, stdout);
		  memcpy(extra+place_in_extra, (char *)&LAST_WORD(*entry_val), 4);
		  place_in_extra += 4;
		  break;
		case 's':
		  int len;
		  sscanf(type_info+2, "%d", &len);
		  type_info = strpbrk(type_info, "]");
		  fread(entry_val, len, 1, stdin);
		  if (feof(stdin))
		    exit(0);
		  if (len > 4)
		    memcpy(extra+place_in_extra, entry_val+4, len);
		  else
		    fwrite("    ", 4-len, 1, stdout);
		  fwrite(entry_val, minimum(4, len), 1, stdout);
		  place_in_extra += maximum(0, len-4);
		  break;
		default:
		  printf("error\n");
		}
	      type_info++;
	    }
	}
      fread(entry_val, data_size, 1, stdin);
      fwrite(entry_val, data_size, 1, stdout);
      fwrite(extra, place_in_extra, 1, stdout);
      if (feof(stdin))
	exit(0);
    }
}
