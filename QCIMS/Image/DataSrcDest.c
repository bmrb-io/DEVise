#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataSrcDest.h"

#ifndef NO_UNIX

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define HTTP_PORT	80

#define HTTP_GET_FORMAT "GET %s HTTP/1.0"
#define INT_CR 13
#define INT_LF 10

#define HTTP_C_LENGTH "content-length"
#define HTTP_C_LENGTH_LEN strlen(HTTP_C_LENGTH)

#define HTTP_VERSION "HTTP/1.0"
#define HTTP_VERSION_LEN strlen(HTTP_VERSION)

#define HTTP_BUF_LENGTH 1024

static void
tolowcase(char *s)
{
  int c,i=0,delta;
  while ((c = s[i]) != '\0') {
    if (((delta = (c - 'A')) >= 0) && (delta < 26)) s[i] = 'a' + delta;
    i++;
    if (i == HTTP_BUF_LENGTH) break;
  }
}

/* read lines terminated by CRLF, replacing CR, LF by \0 */
static int
http_readline(int fd, char *buf)
{
  int    count = 0;
  int    rval;

  for(;;) {
    if ((rval = read(fd, buf, 1)) < 0) {
      return rval;
    }
    else if (rval == 0) {
      /* shouldn't really get here */
      *buf = '\0';
      break;
    }

    if (*buf != INT_LF) {
      if (*buf == INT_CR) *buf = '\0';
      else {
          count++;
        if (count == HTTP_BUF_LENGTH) {
          /* hack */
          *buf = '\0';
          count--;
          break;
        }  
          buf++;
      }
    } else {
      *buf = '\0';
      break;
    }
  }
  return  count;
}


static
int open_http(const char *name, long * bytes_in_body) 
{
  struct sockaddr_in  sin;
  int    sock_fd;
  int    status;
  char  *port_sep;
  char  *end_of_addr;
  char    *clength_start;
  char    *stat_code_start, *reason_phrase_start;
  int stat_code, l;
  int    port_num = HTTP_PORT;
  struct hostent *he;
  char  http_cmd[HTTP_BUF_LENGTH];
  

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    return sock_fd;
  }

  if (!strncmp(name,"http://",7)) {
    name += 7; 
  }

  port_sep = strchr(name, ':');
  end_of_addr = strchr(name, '/');
  if (end_of_addr) {
    *end_of_addr = '\0'; 
  }

  if (port_sep) {
    *port_sep = '\0';
    port_sep++;
    port_num = atoi(port_sep);
  }
  sin.sin_port = htons(port_num);

  he = gethostbyname( name );
  if ( he ) {
    sin.sin_family = he->h_addrtype;
    sin.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
  } else {
    return -1;
  }


  status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
  if (status < 0) {
    return status;
  }

  if (end_of_addr) {
    *end_of_addr = '/'; 
  }
  sprintf(http_cmd, HTTP_GET_FORMAT, 
    end_of_addr ? end_of_addr : "/");
  l = strlen(http_cmd);
  http_cmd[l++] = INT_CR;
  http_cmd[l++] = INT_LF;
  http_cmd[l++] = INT_CR;
  http_cmd[l++] = INT_LF;
  http_cmd[l] = '\0';
  write(sock_fd, http_cmd, l);

  /* get status line */
  if (http_readline(sock_fd, http_cmd) <= 0) {
    close(sock_fd);
    return(-1);
  }

  if (strncmp(http_cmd, HTTP_VERSION, HTTP_VERSION_LEN)) {
    close(sock_fd);
    return(-1);
  }

  if (!(stat_code_start = strchr(http_cmd,' '))) { 
    close(sock_fd);
    return(-1);
  }
  stat_code_start++;

  if (!(reason_phrase_start = strchr(stat_code_start,' '))) { 
    close(sock_fd);
    return(-1);
  }
  *reason_phrase_start = '\0';
  reason_phrase_start++;

  stat_code = atoi(stat_code_start);
  if (stat_code < 200) {
    close(sock_fd);
    return(-1);
  }
  else if (stat_code < 300) {
    /* cool.. */
  }
  else if (stat_code < 400) {
    close(sock_fd);
    return(-1);
  }
  else if (stat_code < 500) {
    close(sock_fd);
    return(-1);
  }
  else if (stat_code < 600) {
    close(sock_fd);
    return(-1);
  }
  else {
    close(sock_fd);
    return(-1);
  }


  /* Skip the header part, but extract Content-Length */
  *bytes_in_body = -1;
  while (http_readline(sock_fd, http_cmd) > 0) {
    tolowcase(http_cmd);
    if (!strncmp(http_cmd, HTTP_C_LENGTH, HTTP_C_LENGTH_LEN)) {
      if ((clength_start = strchr(http_cmd,':')) != 0) {
        clength_start++;
        while (*clength_start == ' ') clength_start++;
        sscanf(clength_start,"%ld",bytes_in_body);
      }
    }  
  }
  return sock_fd;
}



#endif /* !NO_UNIX */ 


static long DS_NoSrcReadFunc(struct DataSrcStruct *ds, char *buff, long nbytes)
{
  return (long) 0;
}


extern void DS_Init(DataSrc *ds, long total_in_src)
{
  ds->state = DS_NO_SRC; 
  ds->total_count = 0; 
  if (total_in_src > 0) ds->total_in_src = total_in_src;
  else ds->total_in_src = 0; 
  ds->bytes_to_read = 0;
  ds->bytes_read = 0; 
  ds->src_kind = DS_KIND_NONE; 
  ds->read_func = DS_NoSrcReadFunc; 
} 

#define DS_ShouldCloseSrc(ds) \
 ( ( (ds)->state != DS_NO_SRC) &&  \
    ( DS_web_src(ds) || DS_filter_src(ds) || \
        ( DS_file_src(ds) && ( (ds)->srcname[0] != '<') ) ) ) 

#ifndef NO_UNIX
#define DS_CloseSrc(ds) \
  { if ( DS_file_src(ds) && (ds)->fp ) { fclose((ds)->fp); \
       (ds)->fp = (FILE *) 0; } \
    else if ( (DS_web_src(ds) || DS_filter_src(ds)) && ((ds)->fd >2)) {\
       close((ds)->fd); (ds)->fd = -1; } }
#else
#define DS_CloseSrc(ds) \
  { if ( DS_file_src(ds) && (ds)->fp ) { fclose((ds)->fp); \
       (ds)->fp = (FILE *) 0; }}
#endif


#ifndef NO_UNIX

static long DS_ReadFromFd(struct DataSrcStruct *ds, char *buff, long nbytes)
{ 
  char *temp;
  long tocome; 
  long thistime, ntoread;
  
  if (ds->state != DS_SRC_ACTIVE) return(0); 
  if ((ntoread = nbytes) <= 0) return(0);  
  if ((ds->bytes_to_read > 0) &&
      ((ds->bytes_read + ntoread) > ds->bytes_to_read))
	ntoread = ds->bytes_to_read - ds->bytes_read;
  if ((ds->total_in_src > 0) &&
      ((ds->total_count + ntoread) > ds->total_in_src))
	ntoread = ds->total_in_src - ds->total_count;
  tocome = ntoread; 
  temp = buff; 
  while (tocome > 0) {
    /* loop needed cos we might be reading off a pipe */ 
    thistime = read(ds->fd,temp,tocome);
    if (thistime <= 0) {
      ds->file_eof = 1; 
      break; 
    } 
    tocome -= thistime;
    temp = temp + thistime;
  }
  
  ntoread -= tocome; 
  ds->bytes_read += ntoread; 
  ds->total_count += ntoread; 
  if ((ds->file_eof) || 
    ((ds->bytes_to_read > 0) && (ds->bytes_read >= ds->bytes_to_read))) 
      ds->state = DS_SRC_DONE; 
  return(ntoread); 
}

#endif /* !NO_UNIX */

static long DS_ReadFromStream(struct DataSrcStruct *ds, char *buff, long nbytes)
{ 
  char *temp;
  long tocome; 
  long thistime, ntoread;
  
  if (ds->state != DS_SRC_ACTIVE) return(0); 
  if ((ntoread = nbytes) <= 0) return(0);  
  if ((ds->bytes_to_read > 0) &&
      ((ds->bytes_read + ntoread) > ds->bytes_to_read))
	ntoread = ds->bytes_to_read - ds->bytes_read;
  if ((ds->total_in_src > 0) &&
      ((ds->total_count + ntoread) > ds->total_in_src))
	ntoread = ds->total_in_src - ds->total_count;
  tocome = ntoread; 
  temp = buff; 
  while (tocome > 0) {
    /* loop needed cos we might be reading off a pipe */ 
    thistime = fread(temp,1,tocome,ds->fp);
    if (thistime <= 0) {
      ds->file_eof = 1; 
      break; 
    } 
    tocome -= thistime;
    temp = temp + thistime;
  }
  
  ntoread -= tocome; 
  ds->bytes_read += ntoread; 
  ds->total_count += ntoread; 
  if ((ds->file_eof) || 
    ((ds->bytes_to_read > 0) && (ds->bytes_read >= ds->bytes_to_read))) 
      ds->state = DS_SRC_DONE; 
  return(ntoread); 
}

extern int DS_OpenFile(DataSrc *ds, char *fname, long bytes_to_read)
{ 
  FILE *fp = 0; 
  int kind, fd = -1; 
  long bytes_in_file = 0; 
  if (ds->state == DS_DEAD) return(0); 

  if (!fname || (!strcmp(fname,"")) || (!strcmp(fname,"-"))) {
    fp = stdin; 
    kind = DS_KIND_STREAM; 
  #ifndef NO_UNIX
  } else if (!strncmp(fname,"http://",7)) {
    fd = open_http(fname, &bytes_in_file); 
    kind = DS_KIND_WEB; 
  #endif
  } else { 
    fp = fopen(fname, "rb"); 
    kind = DS_KIND_FILE; 
  } 
  
  if (kind == DS_KIND_WEB) {
    if (fd < 0) return 0; 
  } else {
    if (!fp) return 0; 
  }

  if (DS_ShouldCloseSrc(ds)) {
    DS_CloseSrc(ds); 
  } 

  if (kind == DS_KIND_WEB) {
    ds->fd = fd;
    if (fd==0) strcpy(ds->srcname,DSD_NAME_STDIN);
    else strcpy(ds->srcname, fname); 
    ds->read_func = DS_ReadFromFd; 
  } else { 
    ds->fp = fp;
    if (fp==stdin) strcpy(ds->srcname,DSD_NAME_STDIN);
    else strcpy(ds->srcname, fname); 
    ds->read_func = DS_ReadFromStream; 
  }

  ds->src_kind = kind; 
  ds->file_eof = 0; 
  ds->web_bytes = bytes_in_file; 
  if ((bytes_in_file > 0) && 
      ((bytes_to_read == 0) || (bytes_to_read > bytes_in_file))) 
    ds->bytes_to_read = bytes_in_file; 
  else { 
    if (bytes_to_read >= 0)
      ds->bytes_to_read = bytes_to_read;
    else if (ds->state == DS_NO_SRC)
      ds->bytes_to_read = 0;
  }
  if ((ds->state == DS_NO_SRC) || (bytes_to_read >= 0))
    ds->bytes_read = 0; 
  if ((ds->total_in_src > 0) && (ds->bytes_to_read > 0) &&
      ((ds->bytes_to_read -ds->bytes_read + ds->total_count)
	  > ds->total_in_src))
	ds->bytes_to_read = ds->total_in_src - ds->total_count
	  + ds->bytes_read; 
  if (ds->bytes_to_read < 0) ds->bytes_to_read = 0; 
  if ((ds->bytes_to_read != 0) &&
      (ds->bytes_to_read == ds->bytes_read))
	ds->bytes_to_read = 0; 
  ds->state = DS_SRC_ACTIVE; 
  
  return(1); 
} 



extern int DS_OpenFd(DataSrc *ds, int fd, long bytes_to_read) 
{ 
  #ifndef NO_UNIX
  if (ds->state == DS_DEAD) return(0); 
  
  if (fd < 0) return(0); 

  if (DS_ShouldCloseSrc(ds)) {
    DS_CloseSrc(ds); 
  } 

  ds->fd = fd;
  ds->src_kind = DS_KIND_FD; 
  ds->file_eof = 0; 
  if (fd != 0) strcpy(ds->srcname,DSD_NAME_FD);
  else strcpy(ds->srcname,DSD_NAME_STDIN);
  if (bytes_to_read >= 0)
    ds->bytes_to_read = bytes_to_read;
  else if (ds->state == DS_NO_SRC)
    ds->bytes_to_read = 0;
  if ((ds->state == DS_NO_SRC) || (bytes_to_read >= 0))
    ds->bytes_read = 0; 
  if ((ds->total_in_src > 0) && (ds->bytes_to_read > 0) &&
      ((ds->bytes_to_read -ds->bytes_read + ds->total_count)
	  > ds->total_in_src))
	ds->bytes_to_read = ds->total_in_src - ds->total_count
	  + ds->bytes_read; 
  if (ds->bytes_to_read < 0) ds->bytes_to_read = 0; 
  if ((ds->bytes_to_read != 0) &&
      (ds->bytes_to_read == ds->bytes_read))
	ds->bytes_to_read = 0; 
  ds->state = DS_SRC_ACTIVE; 
  ds->read_func = DS_ReadFromFd; 
  
  return 1; 

  #else /* !NO_UNIX */
  return 0; 
  #endif

} 

extern int DS_OpenStream(DataSrc *ds, FILE *fp, long bytes_to_read) 
{ 
  if (ds->state == DS_DEAD) return(0); 
  
  if (!fp || (fp==stdout) || (fp==stderr) ) return(0); 

  if (DS_ShouldCloseSrc(ds)) {
    DS_CloseSrc(ds); 
  } 

  ds->fp = fp;
  ds->src_kind = DS_KIND_STREAM; 
  ds->file_eof = 0; 
  if (fp != stdin) strcpy(ds->srcname,DSD_NAME_STREAM);
  else strcpy(ds->srcname,DSD_NAME_STDIN);
  if (bytes_to_read >= 0)
    ds->bytes_to_read = bytes_to_read;
  else if (ds->state == DS_NO_SRC)
    ds->bytes_to_read = 0;
  if ((ds->state == DS_NO_SRC) || (bytes_to_read >= 0))
    ds->bytes_read = 0; 
  if ((ds->total_in_src > 0) && (ds->bytes_to_read > 0) &&
      ((ds->bytes_to_read -ds->bytes_read + ds->total_count)
	  > ds->total_in_src))
	ds->bytes_to_read = ds->total_in_src - ds->total_count
	  + ds->bytes_read; 
  if (ds->bytes_to_read < 0) ds->bytes_to_read = 0; 
  if ((ds->bytes_to_read != 0) &&
      (ds->bytes_to_read == ds->bytes_read))
	ds->bytes_to_read = 0; 
  ds->state = DS_SRC_ACTIVE; 
  ds->read_func = DS_ReadFromStream; 
  
  return 1; 
} 

static long DS_ReadFromMemory(struct DataSrcStruct *ds, char *buff, long nbytes)
{ 
  long ntoread;
  
  if (ds->state != DS_SRC_ACTIVE) return(0); 
  if ((ntoread = nbytes) <= 0) return(0);  
  if ((ds->bytes_read + ntoread) > ds->bytes_to_read)
	ntoread = ds->bytes_to_read - ds->bytes_read;
  if ((ds->total_in_src > 0) &&
      ((ds->total_count + ntoread) > ds->total_in_src))
	ntoread = ds->total_in_src - ds->total_count;
  memcpy(buff, ds->next_mem_byte, ntoread);
  ds->next_mem_byte += ntoread; 
  ds->bytes_read += ntoread;
  ds->total_count += ntoread; 
  
  if (ds->bytes_read >= ds->bytes_to_read) 
      ds->state = DS_SRC_DONE; 
  return(ntoread); 
}

extern int DS_OpenMemory(DataSrc *ds, char *mem, long bytes_to_read) 
{ 

  if (ds->state == DS_DEAD) return(0); 
  if (!mem) return(0); 
  if (bytes_to_read == 0) return(0); 
  else if (bytes_to_read < 0) { 
    if (ds->state == DS_NO_SRC) return(0); 
    bytes_to_read = ds->bytes_to_read - ds->bytes_read;
  }
  if ((ds->total_in_src > 0) && 
      ((bytes_to_read + ds->total_count) > ds->total_in_src))
	bytes_to_read = ds->total_in_src - ds->total_count; 
  if (bytes_to_read <= 0) return(0); 
	

  if (DS_ShouldCloseSrc(ds)) {
    DS_CloseSrc(ds); 
  } 

  ds->state = DS_SRC_ACTIVE; 
  ds->next_mem_byte = mem; 
  ds->src_kind = DS_KIND_MEMORY; 
  strcpy(ds->srcname,DSD_NAME_MEMORY);
  ds->read_func = DS_ReadFromMemory; 
  ds->bytes_read = 0;
  ds->bytes_to_read = bytes_to_read; 
  
  return(1); 
} 

extern int DS_ExtendSrc(DataSrc *ds, long nbytes_more)
{
  if ((ds->state == DS_NO_SRC) ||
      (ds->state == DS_DEAD) ||
      !DS_src_has_file(ds) || 
      DS_file_eof(ds) ) return(0);
  if (ds->bytes_to_read == 0) return(1);
    /* was anyway reading till eof */
  if (nbytes_more > 0) {
    ds->bytes_to_read += nbytes_more; 
    if ((ds->total_in_src > 0) && 
        ((ds->bytes_to_read - ds->bytes_read + ds->total_count)
	     > ds->total_in_src))
  	ds->bytes_to_read = ds->total_in_src - ds->total_count +
				   ds->bytes_read; 
  } else {  
    /* extend till eof */ 
    ds->bytes_to_read = 0; 
  } 
  ds->state = DS_SRC_ACTIVE;
  return(1); 
}


extern int DS_ResetSrcBytes(DataSrc *ds, long nbytes)
{
  if ((ds->state == DS_NO_SRC) ||
      (ds->state == DS_DEAD) ||
      !DS_src_has_file(ds) || 
      ds->file_eof ) return(0);
  if (nbytes > 0) {
    if (nbytes < ds->bytes_read) return(0);
    ds->bytes_to_read = nbytes; 
    if ((ds->total_in_src > 0) && 
        ((ds->bytes_to_read -ds->bytes_read + ds->total_count)
		 > ds->total_in_src))
  	ds->bytes_to_read = ds->total_in_src - ds->total_count
		 + ds->bytes_read; 
    if (ds->bytes_to_read <= ds->bytes_read) ds->state = DS_SRC_DONE;
    else ds->state = DS_SRC_ACTIVE;
  } else {  
    /* extend till eof */ 
    ds->bytes_to_read = 0; 
    ds->state = DS_SRC_ACTIVE;
  } 
  return(1); 
}

extern int DS_ResetTotalSrcBytes(DataSrc *ds, long nbytes)
{
  if (ds->state == DS_DEAD) return(0);

  if (nbytes < 0) return(0);
  else if (nbytes==0) ds->total_in_src = 0;
  else /* if (nbytes > 0) */ {
    if (nbytes < ds->total_count) return(0);
    ds->total_in_src = nbytes; 
    if (ds->bytes_to_read > 0) {
      if ((ds->bytes_to_read -ds->bytes_read + ds->total_count)
	  > ds->total_in_src)
  	ds->bytes_to_read = ds->total_in_src - ds->total_count 
	     + ds->bytes_read; 
      if (ds->bytes_to_read <= ds->bytes_read)
        ds->state = DS_SRC_DONE;
    }
  } 
  return(1); 
}


extern void DS_Close(DataSrc *ds)
{
  if (ds->state == DS_DEAD) return; 
  if (DS_ShouldCloseSrc(ds)) {
    DS_CloseSrc(ds); 
  } 
  ds->fd = -1; 
  ds->fp = (FILE *) 0; 
  ds->state = DS_DEAD;
  ds->src_kind = DS_KIND_NONE;
  ds->read_func = DS_NoSrcReadFunc; 
  return; 
}

extern int DS_ReadLine(DataSrc *ds, char *buff, long buffsize)
{
  int i = 0, lastloc = buffsize-2;
  long ret; 
  char * locbuff = buff;
  
  ret = DS_Read(ds, locbuff, 1);
  while ((ret==1) && (*locbuff != '\n') && (i <= lastloc)) {
    locbuff++;
    i++;
    ret = DS_Read(ds, locbuff, 1);
  }
  if ((ret != 1) || (*locbuff == '\n')) *locbuff = '\0';
  else *(++locbuff) = '\0'; 
  if ((i==0) && !ret) return 0;
  else return 1; 
}

#ifndef NO_UNIX

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

#define FILTER_BUF_SIZE 4096
	    
#ifndef SA_NOCLDWAIT
#define SA_NOCLDWAIT 0
#endif

#endif /* !NO_UNIX */

extern int DS_UseFilter(DataSrc *ds, char *filter, 
	    int NumFirstBytes, char *FirstBytes) 
{
  #ifndef NO_UNIX
  int pipetofilter[2], pipefromfilter[2]; 
  int pid, subpid;
  long len; 
  char buf[FILTER_BUF_SIZE]; 
  static int ForkFiltDidSigAct = 0;

  #ifdef _DEC
  struct sigaction act = {0,0,SA_NOCLDWAIT | SA_NOCLDSTOP};
  #else
  #ifdef _LINUX
  struct sigaction act = {0,0,SA_NOCLDWAIT | SA_NOCLDSTOP,0};
  #else
  struct sigaction act = {0,0,0,SA_NOCLDWAIT | SA_NOCLDSTOP};
  #endif
  #endif
  
  char *FilterArgs[3];

  if (ds->state != DS_SRC_ACTIVE) return(0); 

  if (ds->total_count != NumFirstBytes) return(0); 

  if (!ForkFiltDidSigAct) {
    /* do not want to create zombies */
    sigaction(SIGCHLD, &act, 0);
    ForkFiltDidSigAct = 1;
  }

  if (ds->src_kind == DS_KIND_FILE) { 

    /* no need for subfilter */ 
    if (pipe(pipefromfilter) == -1) return(0);

    if ((pid=fork()) == -1) {
      close(pipefromfilter[0]);
      close(pipefromfilter[1]);
      return(0);
    } 

    if (pid==0) {
      /* child */
      close(pipefromfilter[0]);
      if (dup2(pipefromfilter[1],1) == -1) {
        _exit(1);
      } 
      FilterArgs[0] = filter; 
      FilterArgs[1] = ds->srcname; 
      FilterArgs[2] = (char *) 0;
      execvp(FilterArgs[0], FilterArgs); 
      _exit(1); 
    } else {
      /* parent */
      close(pipefromfilter[1]);
      if (DS_ShouldCloseSrc(ds)) {
        DS_CloseSrc(ds); 
      } 
      ds->total_count = 0;
      ds->total_in_src = 0; 
      ds->bytes_read = 0;
      ds->bytes_to_read = 0; 
      ds->fd = pipefromfilter[0]; 
      ds->read_func = DS_ReadFromFd; 
      ds->file_eof = 0; 
      ds->src_kind = DS_KIND_FILTER; 
      return(1);
    }

  } else {

    /* filter must read off existing ds */ 
    if (pipe(pipefromfilter) == -1) {
      return(0);
    }

    if (pipe(pipetofilter) == -1) {
      close(pipefromfilter[0]);
      close(pipefromfilter[1]);
      return(0);
    }


    if ((pid=fork()) == -1) {
      close(pipefromfilter[0]);
      close(pipefromfilter[1]);
      close(pipetofilter[0]);
      close(pipetofilter[1]);
      return(0);
    } 

    if (pid==0) {
      /* child */
      close(pipefromfilter[0]);
      close(pipetofilter[1]);
      if (dup2(pipefromfilter[1],1) == -1) {
        _exit(1);
      } 
      if (dup2(pipetofilter[0],0) == -1) {
        _exit(1);
      } 
      FilterArgs[0] = filter; 
      FilterArgs[1] = (char *) 0; 

      execvp(FilterArgs[0], FilterArgs); 
      _exit(1); 
    } 
    else {
      /* parent */
      close(pipefromfilter[1]);
      close(pipetofilter[0]);

      if ((subpid=fork()) == -1) {
        close(pipefromfilter[0]);
        close(pipetofilter[1]);
        return(0);
      } 

      if (subpid==0) { 
	/* child */
	close(pipefromfilter[0]); 
	if (NumFirstBytes > 0) {
	  MyWrite(pipetofilter[1], FirstBytes, NumFirstBytes); 
	}
	/* read all the bytes in previous source */
	if (ds->total_in_src > 0)
	  ds->bytes_to_read = 0;
	  /* ds->total_in_src is still the same*/ 
	while ((len=DS_Read(ds, buf, FILTER_BUF_SIZE)) > 0) { 
	  MyWrite(pipetofilter[1], buf, len); 
	}
	close(pipetofilter[1]);
	DS_Close(ds); 
	_exit(0); 

      } else { 
        /* parent */
        close(pipetofilter[1]);
        if (DS_ShouldCloseSrc(ds)) {
          DS_CloseSrc(ds); 
        } 
        ds->total_count = 0; 
        ds->total_in_src = 0; 
        ds->bytes_read = 0;
        ds->bytes_to_read = 0; 
        ds->fd = pipefromfilter[0]; 
        ds->read_func = DS_ReadFromFd; 
	ds->file_eof = 0; 
        ds->src_kind = DS_KIND_FILTER; 
        return(1);
      }
    }
  }

  return 0;

  #else /* !NO_UNIX */
  return 0;
  #endif

}



extern long DS_SrcBytes(DataSrc *ds)
{
  long curr_pos, end_off; 

  if ((ds->state == DS_NO_SRC) ||
      (ds->state == DS_DEAD) ) return ((long) 0);
  
  if (DS_web_src(ds)) return ds->web_bytes;

  if (DS_file_src(ds)) {
    curr_pos = ftell(ds->fp);
    if (fseek(ds->fp, (long) 0, SEEK_END) < 0) return 0L;
    end_off = ftell(ds->fp);
    if (fseek(ds->fp, curr_pos, SEEK_SET) < 0) return 0L;
    return end_off;
  }
  return 0L;

}


/********* DataDest functions ************/

#define DD_ShouldCloseDest(dd) \
 ( ( (dd)->state != DD_NO_DEST) &&  \
        DD_file_dest(dd) && ( (dd)->destname[0] != '<') ) 

#define DD_CloseDest(dd) \
  { if ( DD_file_dest(dd) && (dd)->fp ) { fclose((dd)->fp); \
       (dd)->fp = (FILE *) 0; }}


static long DD_NoDestWriteFunc(struct DataDestStruct *dd, char *buff, long nbytes)
{
  return (long) 0;
}

static long DD_WriteToAutoMemory(struct DataDestStruct *dd,
    char *buff, long nbytes)
{ 
  long ntowrite, new_size;
  char *tbuff;
  
  if (dd->state != DD_DEST_ACTIVE) return(0); 
  if ((ntowrite = nbytes) <= 0) return(0);  
  if ((dd->bytes_written + ntowrite) > dd->auto_mem_size) {
    new_size = dd->bytes_written + ntowrite + dd->auto_mem_step; 
    if ((tbuff = (char *) malloc(new_size)) != 0) {
      memcpy(tbuff, dd->auto_mem, dd->bytes_written);
      dd->next_mem_byte = tbuff + dd->bytes_written;
      free(dd->auto_mem); 
      dd->auto_mem = tbuff; 
      dd->auto_mem_size = new_size; 
    } else {
      ntowrite = dd->auto_mem_size - dd->bytes_written; 
    } 
  }

  if (ntowrite) {
    memcpy(dd->next_mem_byte, buff, ntowrite);
    dd->next_mem_byte += ntowrite; 
    dd->bytes_written += ntowrite;
    dd->total_count += ntowrite; 
  } 

  return(ntowrite); 
}

static int DD_OpenAutoMemory(DataDest *dd, long bytes_to_write) 
{ 
  char *buff; 

  if (dd->state == DD_DEAD) return(0); 
  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  if (bytes_to_write <= 0) return(0); 
  if (!dd->use_auto_mem) return(0); 
  if (!(buff = (char *) malloc(bytes_to_write))) return(0);

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 

  dd->state = DD_DEST_ACTIVE; 
  dd->next_mem_byte = buff; 
  dd->auto_mem = buff;
  dd->auto_mem_size = bytes_to_write; 
  dd->dest_kind = DD_KIND_AUTO_MEMORY; 
  strcpy(dd->destname,DSD_NAME_AUTO_MEMORY);
  dd->write_func = DD_WriteToAutoMemory; 
  dd->bytes_written = 0;
  
  return(1); 
} 

#define DD_AUTO_MEM_STEP 8192

extern void DD_Init(DataDest *dd) 
{
  dd->state = DD_NO_DEST; 
  dd->total_count = 0; 
  dd->bytes_written = 0; 
  dd->dest_kind = DD_KIND_NONE; 
  dd->write_func = DD_NoDestWriteFunc; 
  dd->use_auto_mem = 0;
  dd->auto_mem_size = 0;
  dd->mem_size = 0;
  dd->auto_mem = (char *) 0; 
  dd->auto_mem_step = DD_AUTO_MEM_STEP; 
} 

#ifndef NO_UNIX

static long DD_WriteToFd(struct DataDestStruct *dd, char *buff, long nbytes)
{ 
  char *temp;
  long tocome; 
  long thistime, ntowrite;
  
  if (dd->state != DD_DEST_ACTIVE) return(0); 
  if ((ntowrite = nbytes) <= 0) return(0);  
  tocome = ntowrite; 
  temp = buff; 
  while (tocome > 0) {
    /* loop needed cos we might be writing to a pipe */ 
    thistime = write(dd->fd,temp,tocome);
    if (thistime <= 0) {
      dd->file_eof = 1; 
      break; 
    } 
    tocome -= thistime;
    temp = temp + thistime;
  }
  
  ntowrite -= tocome; 
  dd->bytes_written += ntowrite; 
  dd->total_count += ntowrite; 

  if ((tocome > 0) && (dd->use_auto_mem)) {
    /* try switching to auto memory */
    if (!DD_OpenAutoMemory(dd, tocome)) return(ntowrite);
    else return(ntowrite + DD_Write(dd, buff + ntowrite, tocome)); 
  } else return(ntowrite); 
}

#endif


static long DD_WriteToStream(struct DataDestStruct *dd, char *buff, long nbytes)
{ 
  char *temp;
  long tocome; 
  long thistime, ntowrite;
  
  if (dd->state != DD_DEST_ACTIVE) return(0); 
  if ((ntowrite = nbytes) <= 0) return(0);  
  tocome = ntowrite; 
  temp = buff; 
  while (tocome > 0) {
    /* loop needed cos we might be writing to a pipe */ 
    thistime = fwrite(temp,1,tocome,dd->fp);
    if (thistime <= 0) {
      dd->file_eof = 1; 
      break; 
    } 
    tocome -= thistime;
    temp = temp + thistime;
  }
  
  ntowrite -= tocome; 
  dd->bytes_written += ntowrite; 
  dd->total_count += ntowrite; 

  if ((tocome > 0) && (dd->use_auto_mem)) {
    /* try switching to auto memory */
    if (!DD_OpenAutoMemory(dd, tocome)) return(ntowrite);
    else return(ntowrite + DD_Write(dd, buff + ntowrite, tocome)); 
  } else return(ntowrite); 
}

extern int DD_OpenFile(DataDest *dd, char *fname) 
{ 
  int  kind; 
  FILE *fp;

  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  
  if (dd->state == DD_DEAD) return(0); 

  if (!fname || (!strcmp(fname,"")) || (!strcmp(fname,"-"))) {
    fp = stdout;
    kind = DD_KIND_STREAM; 
  } else { 
    fp = fopen(fname,"wb"); 
    kind = DD_KIND_FILE; 
  } 
  
  if (!fp) return(0); 

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 

  dd->fp = fp;
  dd->dest_kind = kind; 
  dd->file_eof = 0; 
  if (fp==stdout) strcpy(dd->destname,DSD_NAME_STDOUT);
  else strcpy(dd->destname, fname); 
  
  dd->bytes_written = 0; 
  dd->write_func = DD_WriteToStream; 
  dd->state = DD_DEST_ACTIVE; 
  
  return(1); 
} 

static long DD_WriteToNull(struct DataDestStruct *dd, char *buff, long nbytes)
{ 
  
  if (dd->state != DD_DEST_ACTIVE) return(0); 
  if (nbytes <= 0) return(0);  
  
  dd->bytes_written += nbytes; 
  dd->total_count += nbytes; 

  return nbytes; 
} 

extern int DD_OpenNull(DataDest *dd) 
{ 

  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  
  if (dd->state == DD_DEAD) return(0); 

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 
  dd->dest_kind = DD_KIND_NULL; 
  strcpy(dd->destname,DSD_NAME_NULL);
  
  dd->bytes_written = 0; 
  dd->write_func = DD_WriteToNull; 
  dd->state = DD_DEST_ACTIVE; 
  
  return(1); 
} 




extern int DD_OpenFd(DataDest *dd, int fd) 
{ 
  #ifndef NO_UNIX

  if (dd->state == DD_DEAD) return(0); 
  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  
  if (fd < 0) return(0); 

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd);
  } 


  dd->fd = fd;
  dd->dest_kind = DD_KIND_FD; 
  dd->file_eof = 0; 
  if (fd == 1) strcpy(dd->destname,DSD_NAME_STDOUT); 
  else if (fd == 2) strcpy(dd->destname,DSD_NAME_STDERR);
  else strcpy(dd->destname,DSD_NAME_FD); 
  dd->bytes_written = 0; 
  dd->state = DD_DEST_ACTIVE; 
  dd->write_func = DD_WriteToFd; 
  
  return(1); 

  #else /* !NO_UNIX */
  return 0
  #endif

} 


extern int DD_OpenStream(DataDest *dd, FILE *fp) 
{ 

  if (dd->state == DD_DEAD) return(0); 
  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  
  if (!fp || (fp==stdin) ) return 0; 

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 


  dd->fp = fp;
  dd->dest_kind = DD_KIND_STREAM; 
  dd->file_eof = 0; 
  if (fp == stdout) strcpy(dd->destname,DSD_NAME_STDOUT); 
  else if (fp == stderr) strcpy(dd->destname,DSD_NAME_STDERR);
  else strcpy(dd->destname,DSD_NAME_STREAM); 
  dd->bytes_written = 0; 
  dd->state = DD_DEST_ACTIVE; 
  dd->write_func = DD_WriteToStream; 
  
  return(1); 

} 

static long DD_WriteToMemory(struct DataDestStruct *dd, char *buff, long nbytes)
{ 
  long ntowrite, rem;
  
  if (dd->state != DD_DEST_ACTIVE) return(0); 
  if ((ntowrite = nbytes) <= 0) return(0);  
  if ((dd->bytes_written + ntowrite) > dd->mem_size)  
	ntowrite = dd->mem_size - dd->bytes_written;
  if (ntowrite) {
    memcpy(dd->next_mem_byte, buff, ntowrite);
    dd->next_mem_byte += ntowrite; 
    dd->bytes_written += ntowrite;
    dd->total_count += ntowrite; 
  } 

  if (((rem = (nbytes - ntowrite)) > 0) && (dd->use_auto_mem)) { 
    /* try switching to auto memory */
    if (!DD_OpenAutoMemory(dd, rem)) return(ntowrite);
    else return(ntowrite + DD_Write(dd, buff + ntowrite, rem)); 
  } else return(ntowrite); 

}

extern int DD_OpenMemory(DataDest *dd, char *mem, long mem_bytes) 
{ 

  if (dd->state == DD_DEAD) return(0); 
  if (dd->dest_kind == DD_KIND_AUTO_MEMORY) return(0); 
  if (mem_bytes < 0) return(0); 
  if (!mem && (mem_bytes != 0)) return(0); 

  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 

  dd->state = DD_DEST_ACTIVE; 
  dd->next_mem_byte = mem; 
  dd->dest_kind = DD_KIND_MEMORY; 
  strcpy(dd->destname,DSD_NAME_MEMORY);
  dd->write_func = DD_WriteToMemory; 
  dd->bytes_written = 0;
  dd->mem_size = mem_bytes; 
  
  return(1); 
} 



extern void DD_Close(DataDest *dd)
{
  if (dd->state == DD_DEAD) return; 
  if (DD_ShouldCloseDest(dd)) {
    DD_CloseDest(dd); 
  } 
  dd->fd = -1; 
  dd->fp = (FILE *) 0; 
  dd->state = DD_DEAD;
  dd->dest_kind = DD_KIND_NONE;
  dd->write_func = DD_NoDestWriteFunc; 
  return; 
}

extern long DD_printf(DataDest *dd, char *format, ...)
{
  char argbuff[DD_STRLENMAX];
  int l; 
  va_list pvar;
  va_start(pvar, format);
  vsprintf(argbuff, format, pvar);
  l = strlen(argbuff); 
  return DD_Write(dd, argbuff, (long) l); 
}




