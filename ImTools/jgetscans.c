#include <stdio.h>
#include <stdlib.h>


#define BUF_SIZE 512 

int 
main (int argc, char *argv[])
{
  int from, toread, offsets[100], i; 
  char jsdirname[200], foo[10], boo[10]; 
  char buff[BUF_SIZE];
  FILE *fp;
  int first, last, nloops, remaining, numscans;
  unsigned long delay_list[100];

  /* usage jgetscans jpeg_file [delay] [delay] ...
     scan_num starts at 1
  */
  

  if (argc < 2) exit(1);

  i = 2;
  while (i < argc) {
    delay_list[i-2] = (unsigned long) atoi(argv[i]);
    i++;
  }
  last = i-2;


  sprintf(jsdirname,"%s.jsdir\0",argv[1]);
  if ((fp = fopen(jsdirname,"r")) == NULL) {
    fprintf(stderr,"Could not open %s\n",jsdirname);
    exit(1);
  }

  numscans = 0;
  while (fscanf(fp,"%s%s%d",foo,boo,&offsets[numscans]) == 3) {
    numscans++;
  }
  fclose(fp);

  if (numscans < 2) exit(1);
  
  for (i=last;i<numscans;i++) delay_list[i] = 0;

  if ((fp = fopen(argv[1],"rb")) == NULL) {
    fprintf(stderr,"Could not open %s\n",argv[1]);
    exit(1);
  }

  from = 0;
  for (i=1;i<numscans;i++) {
    sleep(delay_list[i-1]);
    toread = offsets[i] - from;
    from = offsets[i];
    nloops = toread/BUF_SIZE;
    remaining = toread - (nloops*BUF_SIZE);

    while (nloops > 0) {
      fread(buff, 1, BUF_SIZE, fp);
      fwrite(buff, 1, BUF_SIZE, stdout);
      nloops--;
    }
    fread(buff, 1, remaining, fp);
    fwrite(buff, 1, remaining, stdout);
  }

  fclose(fp);
  return(0);
}

