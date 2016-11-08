#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "TasvirPatron.h"


/********************************************************
   read a \n-terminated line from fd into s, replacing
   \n by \0. do not read more than maxc-1 characters
   return the number of characters read (excluding \n). 
*********************************************************/
static int
newlinefd(char *s, int fd, int maxc)
{
  int i, lim = maxc-1; 
  char c;
  i = 0;
  while ((i < lim) && (read(fd,&c,1) == 1)) {
    if (c == '\n') {
      s[i] = '\0';
      return (i);
    }
    s[i++] = c;
  }
  s[i] = '\0';
  return(i); 
}


/******************************************************
  Read num bytes from fd into buff. Return
  # bytes actually read, which may be less only when
  EOF is prematurely reached.
*******************************************************/ 
static int
MyRead(int fd, unsigned char *buff, int num)
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


/******************************************************
  Write num bytes into fd from buff. Return
  # bytes actually written, which may be less only when
  write(fd,..,..) returns <=0 at some point. 
*********************************************************/ 
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

int main(int argc, char *argv[])
{
  char sendmsg[TASVIR_MAX_STR_LENGTH], retmsg[TASVIR_MAX_STR_LENGTH];
  char *tmp;
  int fd, imfd,l,i;
  char cmd[50], fname[200];
  int portnum = TASVIR_PORT; 

  if (argc < 2) {
    fprintf(stderr,"Usage: TasvirPatronSend <servername> [portnum]\n");
    exit(1);
  }
 
  if (argc > 2) portnum = atoi(argv[2]); 

  fprintf(stdout,"> ");
  fflush(stdout);
  newlinefd(sendmsg,0,TASVIR_MAX_STR_LENGTH); 
  while (1) {
    fd = TasvirPatron(argv[1],retmsg, portnum);
    if (fd < 0) {
      fprintf(stdout,"Error: %s\n",retmsg);
      exit(1);
    }
    imfd = -1;
    if (!strncmp(sendmsg,"show",2)) {
      sscanf(sendmsg,"%s%s",cmd,fname);
      l = strlen(fname);
      tmp = &sendmsg[0] + 1;
      while (strncmp(tmp,fname,l) != 0) tmp++;
      *tmp++ = '-';
      for (i=0;i<(l-1);i++) *tmp++ = ' ';
      if ((imfd = open(fname,O_RDONLY)) < 0) {
        fprintf(stdout,"Error: %s non-existant\n",fname);
	close(fd);
	continue;
      }
      fprintf(stdout,"%s\n",sendmsg);
    }
    MyWrite(fd,sendmsg,strlen(sendmsg));
    MyWrite(fd,"\n",1);
    if (imfd >= 0) {
      while ((l=MyRead(imfd,sendmsg,TASVIR_MAX_STR_LENGTH))==TASVIR_MAX_STR_LENGTH) {
	MyWrite(fd,sendmsg,TASVIR_MAX_STR_LENGTH);
      }
      if (l > 0) MyWrite(fd,sendmsg,l);
      close(imfd);
    }
    newlinefd(retmsg,fd,TASVIR_MAX_STR_LENGTH);
    close(fd);
    fprintf(stdout,"Tasvir returned: %s\n> ",retmsg);
    fflush(stdout);
    newlinefd(sendmsg,0,TASVIR_MAX_STR_LENGTH);
  }


  return(0);




}

