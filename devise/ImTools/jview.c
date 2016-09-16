#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _RS6000
#include <sys/select.h>
#endif

#include "http_url.h"
#define IMLIST_CMD "imlist"
#define IMINFO_CMD "nph-cat"
#define IMGETSCAN_CMD "nph-jgscan"

/***
**** WAIT proc used by parents to wait for child to terminate
***/

static int ProcWait(pid_t pid)
{
  int procwpid, procstatus;

  if ( ((procwpid = waitpid(pid, &procstatus, 0)) == -1) ||
       (!WIFEXITED(procstatus)) ||
       (WEXITSTATUS(procstatus) != 0) ) return(0);
  return(1);
}


static int newlinefd(char s[], int fd)
{
  int i;
  char c;
  i = 0;
  while (read(fd,&c,1) == 1) {
    if (c == '\n') {
      s[i] = '\0';
      return (i+1);
    }
    s[i] = c;
    i++;
  }
  s[i] = '\0';
  return EOF;
}



static int MyRead(int fd, unsigned char *buff, int num)
{
  unsigned char *temp;
  int tocome = num;
  int thistime;
  
  temp = buff;

  while (tocome > 0) {
    thistime = read(fd,temp,tocome);
    if (thistime <= 0) return(num-tocome);
    tocome -= thistime;
    temp = temp + thistime;
  }

  return(num);
}


static int MyWrite(int fd, unsigned char *buff, int num)
{
  unsigned char *temp;
  int tocome = num;
  int thistime;
  
  temp = buff;

  while (tocome > 0) {
    thistime = write(fd,temp,tocome);
    if (thistime <= 0) return(num-tocome);
    tocome -= thistime;
    temp = temp + thistime;
  }

  return(num);
}

  


void Usage(void)
{
  fprintf(stderr,"Usage: jview [-display display] [-visual vclass] [-loc x y] [-ack] [-maxc <m>]\n");
  fprintf(stderr,"       [-remap] [-privremap] [-win <winid>] [-oldcmap]\n");
  fprintf(stderr,"       [-showafter <percent>] [-ccube n] [-dither] [-url <url_prefix>]\n");
  fprintf(stderr,"       [-parwin <winid>] [-silent] [-echo] [-tryXback] jpegfile\n");
  exit(1);
}


#define DATA_BUF_SIZE 4096 
#define MAXSCANS 100

#define CMD_NONE 0
#define CMD_NEXT 1
#define CMD_NEXTALL 2
#define CMD_ABORT 3
#define CMD_QUIT 4
#define CMD_NEW_MAP_DEF 5
#define CMD_NEW_MAP_PRIV 6

int num_to_get[MAXSCANS];
int cmd_list[MAXSCANS];
int which_cmd_last;
int cmd_till;

int PeepNextCmd(int fd, int *numtoget)
{
  char cmd_buff[100], foo[50];
  fd_set fds;
  struct timeval tout;
  int ret;

  if (cmd_till > which_cmd_last) {
    *numtoget = num_to_get[which_cmd_last];
    return(cmd_list[which_cmd_last]);
  }
  else {
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    tout.tv_sec = 0;
    tout.tv_usec = 0;

    if (select(1, &fds, (fd_set *) 0, (fd_set *) 0, &tout) > 0) {
      ret = newlinefd(cmd_buff, fd);
      cmd_till++;
      if (ret == EOF) cmd_list[cmd_till] = CMD_QUIT;
      else if (!strncmp(cmd_buff,"nextall",7)) cmd_list[cmd_till] = CMD_NEXTALL;
      else if (!strncmp(cmd_buff,"next",4)) {
	cmd_list[cmd_till] = CMD_NEXT;
	if (sscanf(cmd_buff,"%s%d",foo,&num_to_get[cmd_till]) != 2) {
	  num_to_get[cmd_till] = 1;
        }
	*numtoget = num_to_get[cmd_till];
      }
      else if (!strncmp(cmd_buff,"abort",5)) cmd_list[cmd_till] = CMD_ABORT;
      else if (!strncmp(cmd_buff,"quit",4)) cmd_list[cmd_till] = CMD_QUIT;
      else cmd_list[cmd_till] = CMD_NONE;
      return(cmd_list[cmd_till]);
    }
    else return(CMD_NONE);
  }
}

int GetNextCmd(int fd, int *numtoget)
{
  char cmd_buff[100], foo[50];
  int ret;

  if (cmd_till > which_cmd_last) {
    which_cmd_last++;
    *numtoget = num_to_get[which_cmd_last];
    return(cmd_list[which_cmd_last]);
  }
  else {
    ret = newlinefd(cmd_buff, fd);
    which_cmd_last++;
    cmd_till++;
    if (ret == EOF) cmd_list[cmd_till] = CMD_QUIT;
    else if (!strncmp(cmd_buff,"nextall",7)) cmd_list[cmd_till] = CMD_NEXTALL;
    else if (!strncmp(cmd_buff,"next",4)) {
      cmd_list[cmd_till] = CMD_NEXT;
      if (sscanf(cmd_buff,"%s%d",foo,&num_to_get[cmd_till]) != 2) {
        num_to_get[cmd_till] = 1;
      }
      *numtoget = num_to_get[cmd_till];
    }
    else if (!strncmp(cmd_buff,"abort",5)) cmd_list[cmd_till] = CMD_ABORT;
    else if (!strncmp(cmd_buff,"quit",4)) cmd_list[cmd_till] = CMD_QUIT;
    else cmd_list[cmd_till] = CMD_NONE;
    return(cmd_list[cmd_till]);
  }
}

static int ReadInfoFile(int fd, int *offsets, double *psnr, 
         double *gpsnr, double *bpsnr, int *nscansplus1, int echo)
{

    int l, i, from;
    char buff[100];
    char foo1[20], foo2[20], foo3[20], foo4[20], foo5[20], foo6[20];

    /* format of info file:
      scan offset <offset>
      scan offset <offset>
      ...
      end offset <offset>
      #if nplanes==1
      PSNR: <psnr> dB
      PSNR: <psnr> dB
      ..
      #elseif nplanes==3
      RPSNR: <rpsnr> dB GPSNR: <gpsnr> dB BPSNR: <bpsnr> dB
	or
      YPSNR: <rpsnr> dB CbPSNR: <gpsnr> dB CrPSNR: <bpsnr> dB
      ...
    */
    *nscansplus1 = 0;
    l = newlinefd(buff, fd);
    while ((l != EOF) && (strncmp(buff,"end",3))) {
      sscanf(buff,"%s%s%d",foo1,foo2,&offsets[*nscansplus1]);
      (*nscansplus1)++; 
      l = newlinefd(buff,fd);
    } 
    if (l==EOF) return(0);
    sscanf(buff,"%s%s%d",foo1,foo2,&offsets[*nscansplus1]);
    (*nscansplus1)++; 

    i=1;
    l = newlinefd(buff, fd);
    from = 0;

    while ((l != EOF) && (i < *nscansplus1)) { 
      if (!strncmp(buff,"RPSNR",5)) { 
        sscanf(buff, "%s%lf%s%s%lf%s%s%lf%s",
	  foo1, &psnr[i], foo2, foo3, &gpsnr[i], foo4, foo5,
	  &bpsnr[i], foo6);
	if (echo) printf("%d bytes, Red %lf dB, Green %lf dB, Blue %lf dB\n",
			   offsets[i] - from, psnr[i], gpsnr[i], bpsnr[i]);
      } 
      else if (!strncmp(buff,"YPSNR",5)) { 
        sscanf(buff, "%s%lf%s%s%lf%s%s%lf%s",
	  foo1, &psnr[i], foo2, foo3, &gpsnr[i], foo4, foo5,
	  &bpsnr[i], foo6);
	if (echo) printf("%d bytes, Y %lf dB, Cb %lf dB, Cr %lf dB\n",
			   offsets[i] - from, psnr[i], gpsnr[i], bpsnr[i]);
      } 
      else {
        sscanf(buff, "%s%lf%s", foo1, &psnr[i], foo2);
	if (echo) printf("%d bytes, Gray %lf dB\n",
	  offsets[i] - from, psnr[i]); 
      } 
      from = offsets[i]; 
      i++;
      l = newlinefd(buff, fd); 
    }

    if (i < *nscansplus1)  {
      if (echo) {
        while (i < *nscansplus1) { 
  	  printf("%d bytes\n", offsets[i] -from);
	  from = offsets[i];
	  i++;
	}
	printf("END\n");
	fflush(stdout);
      }
      return(1);
    }

    if (echo) {
      printf("END\n");
      fflush(stdout);
    }
    return(2); 

}



int main(int argc, char *argv[])
{
  int from, toread, offsets[MAXSCANS], i, nbytes, next_scan; 
  double psnr[MAXSCANS], gpsnr[MAXSCANS], bpsnr[MAXSCANS]; 
  char jsdirname[200]; 
  char buff[DATA_BUF_SIZE];
  int echo, silent, old_cmap;
  int ccube = 4;
  int tryback, maxc, dodither; 

  int piped[2];
  pid_t pid;

  FILE *fp;
  int fd;
  int next_cmd, peeped_cmd;
  char *xdjpeg_argv[40];
  char xdjpeg_name[10],xdjpeg_d[10], xdjpeg_loc[6],
    xdjpeg_wherex[10], xdjpeg_wherey[10], xdjpeg_map[10],
    xdjpeg_show[15], xdjpeg_showafter[10], xdjpeg_parwin[10],
    xdjpeg_title[10], xdjpeg_win[10], xdjpeg_oldcm[10], xdjpeg_maxc[10],
    xdjpeg_maxc_val[10], xdjpeg_vclass[10], xdjpeg_vclass_val[10],
    xdjpeg_ccube[10], xdjpeg_ccube_val[10], xdjpeg_tryback[10],
    xdjpeg_dither[10], xdjpeg_qfromstdin[20];


  int first, last, nloops, remaining, numscans;
  int isURL, l;
  char * imfilename;
  char URL_prefix[1024];
  char the_url[1024];
  int pargiven, wingiven;
  char *parwin;
  char *thewin;
  char *titlestr; 


  char *dname; /* display */
  int wherex, wherey;
  int remap, sperc, privremap;
  int numtoget;
  int ack_given;
  int vclass;

  
  vclass = -1;
  dname = (char *) 0;
  titlestr = (char *) 0;
  wherex = 0;
  wherey = 0;
  remap = 0;
  privremap = 0;
  sperc = 100;
  isURL = 0;
  pargiven = 0;
  echo = 0;
  silent = 0;
  ack_given = 0;
  wingiven = 0;
  old_cmap = 0;
  tryback = 0;
  maxc = 256;
  dodither = 0;

  /* parse switches */
  if (argc < 2) Usage();
  i = 1;
  while (i < (argc-1)) {
    if (!strncmp(argv[i],"-display", 4)) {
      i++;
      if (i >= argc) Usage();
      dname = argv[i];
    }
    else if (!strncmp(argv[i],"-title", 6)) {
      i++;
      if (i >= argc) Usage();
      titlestr = argv[i];
    }
    else if (!strncmp(argv[i],"-loc", 4)) {
      i++;
      if (i >= argc) Usage();
      wherex = atoi(argv[i]);
      i++;
      if (i >= argc) Usage();
      wherey = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-ccube", 3)) {
      i++;
      if (i >= argc) Usage();
      ccube = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-maxc", 3)) {
      i++;
      if (i >= argc) Usage();
      maxc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-visual", 4)) {
      i++;
      if (i >= argc) Usage();
      vclass = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-remap", 6)) {
      remap = 1;
    }
    else if (!strncmp(argv[i],"-dither", 4)) {
      dodither = 1;
    }
    else if (!strncmp(argv[i],"-echo", 5)) {
      echo = 1;
    }
    else if (!strncmp(argv[i],"-tryXback", 5)) {
      tryback = 1;
    }
    else if (!strncmp(argv[i],"-oldcmap", 5)) {
      old_cmap = 1;
    }
    else if (!strncmp(argv[i],"-silent", 7)) {
      silent = 1;
    }
    else if (!strncmp(argv[i],"-ack", 4)) {
      ack_given = 1;
    }
    else if (!strncmp(argv[i],"-privremap", 6)) {
      privremap = 1;
    }
    else if (!strncmp(argv[i],"-showafter", 7)) {
      i++;
      if (i >= argc) Usage();
      sperc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-parwin", 7)) {
      i++;
      if (i >= argc) Usage();
      parwin = argv[i];
      pargiven = 1;
    }
    else if (!strncmp(argv[i],"-win", 4)) {
      i++;
      if (i >= argc) Usage();
      thewin = argv[i];
      wingiven = 1;
    }
    else if (!strncmp(argv[i],"-url", 4)) {
      i++;
      if (i >= argc) Usage();
      if (strncmp(argv[i],"http://",7)) {
	fprintf(stderr,"Invalid URL prefix\n");
	exit(1);
      }
      strcpy(URL_prefix, argv[i] + 5); /* skip http: */
      isURL = 1;
    }
    else {
      Usage();
    }
    i++;
  }



  imfilename = argv[argc-1];


  /*** create viewer ***/

  if (pipe(piped) == -1) {
    fprintf(stderr,"Could not create pipe\n");
    exit(1);
  }

  if ((pid=fork()) == -1) {
    fprintf(stderr,"Too many processes: could not fork\n");
    exit(1);
  }

  if (pid==0) {
    /* child */
    close(piped[1]);
    if (dup2(piped[0],0) == -1) {
      fprintf(stderr,"xdjpeg could not dup2\n");
      exit(1);
    }

    i = 0;

    strcpy(xdjpeg_name,"xdjpeg");
    xdjpeg_argv[i++] = xdjpeg_name;
    
    if (dname) {
      strcpy(xdjpeg_d,"-display");
      xdjpeg_argv[i++]  = xdjpeg_d;
      xdjpeg_argv[i++] = dname;
    }

    if (vclass != -1) {
      strcpy(xdjpeg_vclass,"-visual");
      xdjpeg_argv[i++]  = xdjpeg_vclass;
      sprintf(xdjpeg_vclass_val,"%d\0",vclass);
      xdjpeg_argv[i++]  = xdjpeg_vclass_val;
    }

    if (titlestr) {
      strcpy(xdjpeg_title,"-title");
      xdjpeg_argv[i++]  = xdjpeg_title;
      xdjpeg_argv[i++] = titlestr;
    }

    strcpy(xdjpeg_ccube,"-ccube");
    xdjpeg_argv[i++] = xdjpeg_ccube;
    sprintf(xdjpeg_ccube_val,"%d\0",ccube);
    xdjpeg_argv[i++] = xdjpeg_ccube_val;

    if (maxc != 256) {
      strcpy(xdjpeg_maxc,"-maxc");
      xdjpeg_argv[i++] = xdjpeg_maxc;
      sprintf(xdjpeg_maxc_val,"%d\0",maxc);
      xdjpeg_argv[i++] = xdjpeg_maxc_val;
    }

    strcpy(xdjpeg_loc,"-loc");
    xdjpeg_argv[i++] = xdjpeg_loc;
    sprintf(xdjpeg_wherex,"%d\0", wherex);
    xdjpeg_argv[i++] = xdjpeg_wherex;
    sprintf(xdjpeg_wherey,"%d\0", wherey);
    xdjpeg_argv[i++] = xdjpeg_wherey;

    if (privremap) {
      strcpy(xdjpeg_map,"-privremap");
      xdjpeg_argv[i++] = xdjpeg_map;
    }
    else if (remap) {
      strcpy(xdjpeg_map,"-remap");
      xdjpeg_argv[i++] = xdjpeg_map;
    }

    if (wingiven) {
      strcpy(xdjpeg_win,"-win");
      xdjpeg_argv[i++] = xdjpeg_win;
      xdjpeg_argv[i++] = thewin;
    }
    else if (pargiven) {
      strcpy(xdjpeg_parwin,"-parwin");
      xdjpeg_argv[i++] = xdjpeg_parwin;
      xdjpeg_argv[i++] = parwin;
    }
    
    if (old_cmap) {
      strcpy(xdjpeg_oldcm,"-oldcmap");
      xdjpeg_argv[i++] = xdjpeg_oldcm;
    }

    if (dodither) {
      strcpy(xdjpeg_dither,"-dither");
      xdjpeg_argv[i++] = xdjpeg_dither;
    }

    if (tryback) {
      strcpy(xdjpeg_tryback,"-tryXback");
      xdjpeg_argv[i++] = xdjpeg_tryback;
    }
    
    strcpy(xdjpeg_qfromstdin,"-qfromstdink");
    xdjpeg_argv[i++] = xdjpeg_qfromstdin;

    strcpy(xdjpeg_show,"-showafter");
    xdjpeg_argv[i++] = xdjpeg_show;
    sprintf(xdjpeg_showafter,"%d\0",sperc);
    xdjpeg_argv[i++] = xdjpeg_showafter;

    xdjpeg_argv[i] = (char *) 0;

    execvp("xdjpeg", xdjpeg_argv);

  }

  /* parent */
  close(piped[0]);

  next_cmd = CMD_NONE;
  which_cmd_last = 0;
  cmd_till = 0;

  if (!isURL) {
    sprintf(jsdirname,"%s.jsdir\0",imfilename);
    if ((fd = open(jsdirname, O_RDONLY)) < 0) {
      fprintf(stderr,"Could not open %s\n",jsdirname);
      exit(1);
    }

    if (!ReadInfoFile(fd, offsets, psnr, gpsnr, bpsnr, &numscans, echo)) {
      fprintf(stderr,"Could not read %s\n",jsdirname);
      exit(1);
    }
      
    close(fd);

    if (numscans < 2) exit(1);
  

     if ((fd = open(imfilename,O_RDONLY, 0)) < 0) {
       fprintf(stderr,"Could not open %s\n",argv[1]);
       exit(1);
     }
     
     if (!silent) printf("JPEG image %s has %d scans\n", imfilename, numscans-1);
     fflush(stdout);
     from = 0;

     i = 1;
     while (i < numscans) { 
       toread = offsets[i] - from;
       from = offsets[i];
       nloops = toread/DATA_BUF_SIZE;
       remaining = toread - (nloops*DATA_BUF_SIZE);
       if (!silent) printf("\tScan # %d has %d bytes\n",i,toread);
       fflush(stdout);

       while (nloops > 0) {
         if (MyRead(fd, buff, DATA_BUF_SIZE) < DATA_BUF_SIZE) exit(1);
         if (MyWrite(piped[1], buff, DATA_BUF_SIZE) < DATA_BUF_SIZE) exit(1);
         nloops--;
       }
       if (MyRead(fd, buff, remaining) < remaining) exit(1);
       if (MyWrite(piped[1], buff, remaining) < remaining) exit(1);

       if (!silent) printf("\tDone with scan # %d\n",i);
       fflush(stdout);

       if (i==(numscans-1)) break;

       while ((next_cmd = GetNextCmd(0,&numtoget)) == CMD_NONE);

       if (next_cmd == CMD_QUIT) break;
       else if (next_cmd == CMD_ABORT) break;

       if (next_cmd == CMD_NEXTALL) i = (numscans-1);
       else {
	 i += numtoget;
	 if (i >= numscans) i = (numscans-1);
       }

    }
    close(fd);
  }
  else  {
    /* do http stuff */

    l = strlen(URL_prefix);
    if (URL_prefix[l-1] != '/') {
      URL_prefix[l++] = '/';
      URL_prefix[l] = '\0';
    }
    sprintf(the_url,"%s%s?%s.jsdir\0",URL_prefix,IMINFO_CMD,imfilename);
    if ((fd = open_http(the_url, &nbytes)) < 0) {
      fprintf(stderr,"Could not open URL %s\n",the_url);
      exit(1);
    }

    if (!ReadInfoFile(fd, offsets, psnr, gpsnr, bpsnr, &numscans, echo)) {
      fprintf(stderr,"Could not read URL %s\n",the_url);
      exit(1);
    }
    
    close(fd);

    if (numscans < 2) exit(1);
  


    if (!silent) printf("JPEG image %s has %d scans\n", imfilename, numscans-1);
    fflush(stdout);
    from = 0;

    i = 1;
    next_scan = 1; 
    while (i < numscans) { 
       sprintf(the_url,"%s%s?%s+%d+%d\0",URL_prefix,
	  IMGETSCAN_CMD, imfilename, next_scan, i);
       if ((fd = open_http(the_url, &nbytes)) < 0) {
	 fprintf(stderr,"Could not open URL %s\n",the_url);
	 exit(1);
       }
       toread = offsets[i] - from;
       if (nbytes < 0) nbytes = toread;
       if (toread != nbytes) {
	 fprintf(stderr, "Warning: unexpected scan length\n");
	 toread = nbytes;
       }
       from = offsets[i];
       nloops = toread/DATA_BUF_SIZE;
       remaining = toread - (nloops*DATA_BUF_SIZE);
       if (!silent) printf("\tScan # %d has %d bytes\n",i,toread);
       fflush(stdout);

       while (nloops > 0) {
         if (MyRead(fd, buff, DATA_BUF_SIZE) < DATA_BUF_SIZE) exit(1);
         if (MyWrite(piped[1], buff, DATA_BUF_SIZE) < DATA_BUF_SIZE) exit(1);
         nloops--;
       }
       if (MyRead(fd, buff, remaining) < remaining) exit(1);
       if (MyWrite(piped[1], buff, remaining) < remaining) exit(1);

       close(fd);
       if (!silent) printf("\tDone with scan # %d\n",i);
       fflush(stdout);


       if (i==(numscans-1)) break;

       while ((next_cmd = GetNextCmd(0,&numtoget)) == CMD_NONE);

       if (next_cmd == CMD_QUIT) break;
       else if (next_cmd == CMD_ABORT) break;

       next_scan = i+1;

       if (next_cmd == CMD_NEXTALL) i = (numscans-1);
       else {
	 i += numtoget;
	 if (i >= numscans) i = (numscans-1);
       }
    }
  }


  while (next_cmd != CMD_QUIT) {
    next_cmd = GetNextCmd(0, &numtoget);
  }

  MyWrite(piped[1],"q",1); /* get xdjpeg out of event-gathering loop */
  close(piped[1]);
  ProcWait(pid); /* avoid zombies */
  return(0);
}

