#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include"correct_math.h"

#define minimum(A, B) ((A) < (B) ? (A) : (B))

#define maximum(A, B) ((A) > (B) ? (A) : (B))

#include <sys/types.h>
#include <netinet/in.h> 

inline long long htonll(long long x)
{
#if defined(_LITTLE_ENDIAN)
  int* p = (int*)&x;
  int t = htonl(p[0]);
  p[0] = htonl(p[1]);
  p[1] = t;
  x = *(long long*)p;
#elif defined(_BIG_ENDIAN)
  // nada
#else
# error "what endian??"
#endif;
  return x;
}


main()
{
  char line[160], line2[160];
  char extra[30000];
  char entry_val[10000];
  int point_data, num_dim, data_size, new_data_size;
  int recs = 0;

  gets(line);
  sscanf(line, "%d %d %d\n", &num_dim, &data_size, &point_data);
  //fprintf(stderr, "convert schema: %s\n", line);
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

  int Rep;
  int dim;
  
  while(1)
    {
      int place_in_extra = 0;
      for(Rep=0; Rep<max_reps; Rep++)
	{
	  char *type_info = strpbrk(line,"ivfds");
	  for(dim=0; dim<num_dim; dim++)
	    {
	      switch (type_info[0]) 
		{
		case 'i':
		  {
                    int i;
		    fread(&i, 4, 1, stdin);
		    long long long_entry_val = i;
		    if (feof(stdin))
                      goto done;
		    fwrite(&long_entry_val, 8, 1, stdout);
                    //fprintf(stderr, "i: %d\n", i);
		  }
		  break;
		case 'f':
		  {
                    int f;
		    fread(&f, 4, 1, stdin);
		    if (feof(stdin))
                      goto done;
		    double double_entry_val = f;
		    flip_bits(double_entry_val);
		    fwrite(&double_entry_val, 8, 1, stdout);
		  }
		  break;
		case 'v':
		  fread(entry_val, 8, 1, stdin);
		  if (feof(stdin))
		    goto done;
		  fwrite(entry_val, 8, 1, stdout);
		  break;
		case 'd':
                  {
                    double d;
                    fread(&d, 8, 1, stdin);
                    if (feof(stdin))
                      goto done;
                    flip_bits(d);
                    fwrite(&d, 8, 1, stdout);
                  }
                  break;
		case 's':
                  {
                    long long x = 0;
                    int len;
                    sscanf(type_info+2, "%d", &len);
                    type_info = strpbrk(type_info, "]");
                    fread(&x, minimum(len,8), 1, stdin);
                    if (feof(stdin))
                      goto done;
                    //fprintf(stderr, "s[%d]: '%s' %llx", len, &x, x);
                    x = htonll(x);
                    //fprintf(stderr, " %llx\n", x);
                    fwrite(&x, 8, 1, stdout);

                    if (len > 8) {
                      int rest = len - 8;
                      fread(extra+place_in_extra, rest, 1, stdin);
                      if (feof(stdin))
                        goto done;
                      place_in_extra += rest;
                    }
                  }
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
      recs++;
      if (feof(stdin))
	goto done;
    }

 done:

   if( Rep != 0 || dim != 0 ) {
     fprintf(stderr, "warning convert_bulk input was not properly formatted\n");
   }
   fprintf(stderr, "converted %d records\n", recs);
}
