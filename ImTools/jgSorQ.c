#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


#define BUF_SIZE 512 

#define M_EOI 0xD9

#define INT_CR 13
#define INT_LF 10


int 
main (int argc, char *argv[])
{
  int toread, offsets[200]; 
  char jsdirname[1024], foo[50], boo[50], imfilename[1024]; 
  char val_str[100]; 
  char buff[BUF_SIZE];
  FILE *fp;
  int i,l, last, nloops, remaining, numscans;
  float target; 
  float psnrs[200]; 
  int get_qual; 

  /* usage jgSorQ jpeg_file T [-psnr] 
     get at least 1 scan of jpeg_file, and at most 
     as many scans as to exceed T bytes (or quality (PSNR) T dB if
     -psnr is specified). This will always terminate with M_EOI
  */
  
  if (argc < 3) exit(1);

  get_qual = 0; 
  target =  (float) atof(argv[2]);
  if ((argc > 3) && (!strncmp(argv[3],"-psnr",2))) {
    get_qual = 1;
  }
  
  sprintf(imfilename,"%s\0",argv[1]);
  sprintf(jsdirname,"%s.jsdir\0",imfilename);

  if ((fp = fopen(jsdirname,"r")) == NULL) {
    fprintf(stderr,"Could not open %s\n",jsdirname);
    exit(1);
  }

  numscans = 0;
  while ((fscanf(fp,"%s%s%s",foo,boo,val_str)==3) &&
	 (!strncmp(boo,"offset",3))) { 
    offsets[numscans] = atoi(val_str); 
    numscans++;
  }

  if (numscans < 2) exit(1);

  if (get_qual) { 
    if (!strncmp(foo,"PSNR",1)) {
      /* grayscale PSNRs */
      psnrs[1] = atof(boo); 
      for (i=2;i<numscans;i++) fscanf(fp,"%s%f%s",foo,&psnrs[i],boo); 
    } else {
      /* ypsnr, cb psnr, cr psnr, psnr */ 
      fscanf(fp,"%s%s%s",foo,val_str,boo); /* cb */
      fscanf(fp,"%s%s%s",foo,val_str,boo); /* cr */ 
      fscanf(fp,"%s%s%s",foo,val_str,boo); 
      psnrs[1] = atof(val_str); 
      for (i=2;i<numscans;i++) {
        fscanf(fp,"%s%s%s",foo,val_str,boo); /* y */
        fscanf(fp,"%s%s%s",foo,val_str,boo); /* cb */
        fscanf(fp,"%s%s%s",foo,val_str,boo); /* cr */ 
        fscanf(fp,"%s%f%s",foo,&psnrs[i],boo); 
      } 
    } 
  }

  fclose(fp);

  last = 1;
  if (!get_qual) { 
    while ((last < numscans) && 
	   ((float) offsets[last] < target)) last++;
    if (last==numscans) last--;
    if (((float) offsets[last] > target) && (last > 1)) last--;
  } else {
    while ((last < numscans) && 
	   (psnrs[last] < target)) last++;
    if (last==numscans) last--;
  }


  toread = offsets[last];

  if ((fp = fopen(imfilename,"rb")) == NULL) {
    fprintf(stderr,"Could not open %s\n",argv[1]);
    exit(1);
  }

  fseek(fp, 0, SEEK_SET);
  nloops = toread/BUF_SIZE;
  remaining = toread - (nloops*BUF_SIZE);
  

  while (nloops > 0) {
    fread(buff, 1, BUF_SIZE, fp);
    if (!remaining && (nloops==1)) buff[BUF_SIZE-1] = M_EOI;
    write(1,buff,BUF_SIZE);
    nloops--;
  }
  fread(buff, 1, remaining, fp);
  if (remaining>0) buff[remaining-1] = M_EOI;
  write(1,buff, remaining);

  fclose(fp);
  return(0); 
}


