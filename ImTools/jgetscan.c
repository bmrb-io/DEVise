#include <stdio.h>
#include <stdlib.h>


#define BUF_SIZE 512 

#define M_EOI 0xD9

int 
main (int argc, char *argv[])
{
  int from, toread, offsets[200]; 
  char jsdirname[200], foo[50], boo[50]; 
  char buff[BUF_SIZE];
  FILE *fp;
  int first, last, nloops, remaining, numscans;
  int send_eoi = 0;

  /* usage jgetscan jpeg_file scan_num [end_scan_num] [-term]
     scan_num starts at 1. on -term, EOI mark will be sent
     instead of SOS, if not all scans are needed. -term must be
     after end_scan_num, if both are provided.
  */
  
  if (argc < 3) exit(1);

  first =  atoi(argv[2]);

  last = first;

  if (argc > 3) {
    if ((argc >4) || strncmp(argv[3],"-term",2)) {
      last =  atoi(argv[3]);
    }
    if (!strncmp(argv[argc-1],"-term",2)) send_eoi = 1;
  }

  sprintf(jsdirname,"%s.jsdir\0",argv[1]);
  if ((fp = fopen(jsdirname,"r")) == NULL) {
    fprintf(stderr,"Could not open %s\n",jsdirname);
    exit(1);
  }

  numscans = 0;
  while ((fscanf(fp,"%s%s%d",foo,boo,&offsets[numscans]) == 3) &&
	 (!strncmp(boo,"offset",3))) {
    numscans++;
  }
  fclose(fp);

  if (numscans < 2) exit(1);
  
  if (first < 1) first = 1;
  if (first >= numscans) first = (numscans-1);
  if (last < first) last = first;
  if (last >= numscans) last = (numscans-1);

  if (last == (numscans-1)) send_eoi = 0;

  if (first==1) from = 0;
  else from = offsets[first-1];
  toread = offsets[last] - from;

  if ((fp = fopen(argv[1],"rb")) == NULL) {
    fprintf(stderr,"Could not open %s\n",argv[1]);
    exit(1);
  }

  fseek(fp, from, 0);


  nloops = toread/BUF_SIZE;
  remaining = toread - (nloops*BUF_SIZE);



  while (nloops > 0) {
    fread(buff, 1, BUF_SIZE, fp);
    if (send_eoi && (nloops==1) && (remaining==0))
      buff[BUF_SIZE-1] = M_EOI;
    fwrite(buff, 1, BUF_SIZE, stdout);
    nloops--;
  }

  fread(buff, 1, remaining, fp);

  if (send_eoi && (remaining > 0)) 
    buff[remaining-1] = M_EOI;

  fwrite(buff, 1, remaining, stdout);

  fclose(fp);
  return(0);
}

