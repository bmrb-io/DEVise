#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include"correct_math.h"

#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))

main()
{
  char line[160], line2[160];
  char extra[30000];
  char entry_val[10000];
  int point_data, num_elem, num_dim, data_size, new_data_size;

  gets(line);
  sscanf(line, "%d %d %d\n", &num_dim, &data_size, &point_data);
  new_data_size = data_size;
  int max_reps;
  if (point_data)
    max_reps=1;
  else
    max_reps=2;
  for(int Rep=0; Rep<max_reps; Rep++)
    {
      char *type_info = strpbrk(line,"ivfds");
      for(int dim=0; dim<num_dim; dim++)
	{
	  switch (type_info[0]) 
	    {
	    case 's':
	      int len;
	      sscanf(type_info+2, "%d", &len);
	      type_info = strpbrk(type_info, "]");
	      new_data_size += maximum(0, len-8);
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
      for(int Rep=0; Rep<max_reps; Rep++)
	{
	  char *type_info = strpbrk(line,"ivfds");
	  for(int dim=0; dim<num_dim; dim++)
	    {
	      switch (type_info[0]) 
		{
		case 'i':
		  {
		    fread(entry_val, 4, 1, stdin);
		    long long long_entry_val = *((int *)entry_val);
		    if feof(stdin)
			     exit(0);
		    fwrite(&long_entry_val, 8, 1, stdout);
		  }
		  break;
		case 'f':
		  {
		    fread(entry_val, 4, 1, stdin);
		    if feof(stdin)
			     exit(0);
		    double double_entry_val = *((float *)entry_val);
		    flip_bits(double_entry_val);
		    fwrite(&double_entry_val, 8, 1, stdout);
		  }
		  break;
		case 'v':
		  fread(entry_val, 8, 1, stdin);
		  if feof(stdin)
		    exit(0);
		  fwrite(entry_val, 8, 1, stdout);
		  break;
		case 'd':
		  fread(entry_val, 8, 1, stdin);
		  if feof(stdin)
		    exit(0);
		  flip_bits(*((double *)entry_val));
		  fwrite(entry_val, 8, 1, stdout);
		  break;
		case 's':
		  int len;
		  sscanf(type_info+2, "%d", &len);
		  type_info = strpbrk(type_info, "]");
		  fread(entry_val, len, 1, stdin);
		  if feof(stdin)
		    exit(0);
		  if (len > 8)
		    memcpy(extra+place_in_extra, entry_val+8, len);
		  else
		    fwrite("        ", 8-len, 1, stdout);
		  fwrite(entry_val, minimum(8, len), 1, stdout);
		  place_in_extra += maximum(0, len-8);
		  break;
		default:
		  fprintf(stderr, "error while converting\n");
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
