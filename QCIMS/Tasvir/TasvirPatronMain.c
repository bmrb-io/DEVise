#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "TasvirPatron.h"
#include "DataSrcDest.h"



int main(int argc, char *argv[])
{
  char sendmsg[TASVIR_MAX_STR_LENGTH], retmsg[TASVIR_MAX_STR_LENGTH];
  int fd, portnum = TASVIR_PORT;
  int nret, i;
  DataSrc dsrc, *ds = &dsrc;
  DataSrc dsrc2, *ds2 = &dsrc2;
  DataDest ddest, *dd = &ddest;

  if (argc < 2) {
    fprintf(stderr,"Usage: TasvirPatron <servername> [portnum]\n");
    exit(1);
  }
  
  if (argc > 2) portnum = atoi(argv[2]); 

  DS_Init(ds, 0);
  DS_OpenStream(ds, stdin, 0);

  DS_Init(ds2, 0);
  DD_Init(dd);

  fprintf(stdout,"> ");
  fflush(stdout);

  DS_ReadLine(ds, sendmsg, TASVIR_MAX_STR_LENGTH);
  while (1) {
    if (!strncasecmp(sendmsg,"close",4)) break;
    fd = TasvirPatron(argv[1],retmsg,portnum);
    if (fd < 0) {
      fprintf(stdout,"Error: %s\n",retmsg);
      exit(1);
    }

    DD_OpenFd(dd, fd);
    DD_Write(dd,sendmsg,strlen(sendmsg));
    DD_Write(dd,"\n",1);

    DS_OpenFd(ds2, fd, 0);
    DS_ReadLine(ds2, retmsg, TASVIR_MAX_STR_LENGTH);

    nret = 0;
    if (!strncasecmp(sendmsg,"apply",3) || !strncasecmp(sendmsg,"wapply",3)) {
      if (!strncmp(retmsg,"OK",2)) {
	sscanf(retmsg, "OK %d",&nret);
      }
    }
    fprintf(stdout,"Tasvir returned: %s\n> ",retmsg);
    fflush(stdout);
    for (i=0; i<nret;i++) {
      DS_ReadLine(ds2, retmsg, TASVIR_MAX_STR_LENGTH);
      fprintf(stdout,"Tasvir returned: %s\n> ",retmsg);
      fflush(stdout);
    }
    close(fd);
    DS_ReadLine(ds, sendmsg, TASVIR_MAX_STR_LENGTH);
  }


  return(0);




}

