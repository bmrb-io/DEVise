/*
  This file contributed by the Condor project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$*/

/* 
** Copyright 1995 by Miron Livny and Jim Pruyne
** 
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation, and that the names of the University of
** Wisconsin and the copyright holders not be used in advertising or
** publicity pertaining to distribution of the software without specific,
** written prior permission.  The University of Wisconsin and the 
** copyright holders make no representations about the suitability of this
** software for any purpose.  It is provided "as is" without express
** or implied warranty.
** 
** THE UNIVERSITY OF WISCONSIN AND THE COPYRIGHT HOLDERS DISCLAIM ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE UNIVERSITY OF
** WISCONSIN OR THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT
** OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
** OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
** OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
** OR PERFORMANCE OF THIS SOFTWARE.
** 
** Author:  Jim Pruyne
**
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <tcl.h>
#include <tk.h>

#include "machdep.h"

extern int errno;

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

#define HTTP_PORT	80

#define HTTP_GET_FORMAT "GET %s HTTP/1.0"
#define INT_CR 13
#define INT_LF 10

#define HTTP_C_LENGTH "content-length"
#define HTTP_C_LENGTH_LEN strlen(HTTP_C_LENGTH)

#define HTTP_VERSION "HTTP/1.0"
#define HTTP_VERSION_LEN strlen(HTTP_VERSION)

#define BUF_LENGTH 1024

static void
tolowcase(char *s)
{
  int c,i=0,delta;
  while ((c = s[i]) != '\0') {
    if (((delta = (c - 'A')) >= 0) && (delta < 26)) s[i] = 'a' + delta;
    i++;
    if (i == BUF_LENGTH) break;
  }
}

/* read lines terminated by CRLF, replacing CR, LF by \0 */
static int
readline(int fd, char *buf)
{
	int		count = 0;
	int		rval;

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
			  if (count == BUF_LENGTH) {
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
	return	count;
}


extern
int open_http( char *name, int * bytes_in_body)
{

	struct sockaddr_in	sin;
	int		sock_fd;
	int		status;
	char	*port_sep;
	char	*end_of_addr;
	char    *clength_start;
	char    *stat_code_start, *reason_phrase_start;
	int stat_code, l;
	int		port_num = HTTP_PORT;
	struct hostent *he;
	char	http_cmd[BUF_LENGTH];
	

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		fprintf(stderr, "socket() failed, errno = %d\n", errno);
		fflush(stderr);
		return sock_fd;
	}

	if (strncmp(name,"http://",7)) {
		return -1;
	}
	name += 7;

	port_sep = strchr(name, ':');
	end_of_addr = strchr(name, '/');
	if (end_of_addr == 0) {
		return -1;
	}
	*end_of_addr = '\0';

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

	*end_of_addr = '/';

	status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
	if (status < 0) {
		fprintf(stderr, "http connect() FAILED, errno = %d\n", errno);
		fflush(stderr);
		return status;
	}

	name = end_of_addr;
	sprintf(http_cmd, HTTP_GET_FORMAT, name);
	l = strlen(http_cmd);
	http_cmd[l++] = INT_CR;
	http_cmd[l++] = INT_LF;
	http_cmd[l++] = INT_CR;
	http_cmd[l++] = INT_LF;
	http_cmd[l] = '\0';
	write(sock_fd, http_cmd, l);

	/* get status line */
	if (readline(sock_fd, http_cmd) <= 0) {
	  fprintf(stderr,"Bad http connection\n");
	  close(sock_fd);
	  return(-1);
	}

	if (strncmp(http_cmd, HTTP_VERSION, HTTP_VERSION_LEN)) {
	  fprintf(stderr,"Bad http header received\n");
	  close(sock_fd);
	  return(-1);
	}

	if (!(stat_code_start = strchr(http_cmd,' '))) { 
	  fprintf(stderr,"Bad http header received\n");
	  close(sock_fd);
	  return(-1);
	}
	stat_code_start++;

	if (!(reason_phrase_start = strchr(stat_code_start,' '))) { 
	  fprintf(stderr,"Bad http header received\n");
	  close(sock_fd);
	  return(-1);
	}
	*reason_phrase_start = '\0';
	reason_phrase_start++;

	stat_code = atoi(stat_code_start);
	if (stat_code < 200) {
	  fprintf(stderr,"Unused status code (%d) received\n", stat_code);
	  close(sock_fd);
	  return(-1);
	}
	else if (stat_code < 300) {
	  /* cool.. */
	}
	else if (stat_code < 400) {
	  fprintf(stderr,"Redirection error: %s\n", reason_phrase_start);
	  close(sock_fd);
	  return(-1);
	}
	else if (stat_code < 500) {
	  fprintf(stderr,"Client error: %s\n", reason_phrase_start);
	  close(sock_fd);
	  return(-1);
	}
	else if (stat_code < 600) {
	  fprintf(stderr,"Server error: %s\n", reason_phrase_start);
	  close(sock_fd);
	  return(-1);
	}
	else {
	  fprintf(stderr,"Strange status code (%d) received\n", stat_code);
	  close(sock_fd);
	  return(-1);
	}


	/* Skip the header part, but extract Content-Length */
	*bytes_in_body = -1;
	while (readline(sock_fd, http_cmd) > 0) {
	  tolowcase(http_cmd);
	  if (!strncmp(http_cmd, HTTP_C_LENGTH, HTTP_C_LENGTH_LEN)) {
	    if ((clength_start = strchr(http_cmd,':'))) {
	      clength_start++;
	      while (*clength_start == ' ') clength_start++;
	      *bytes_in_body = atoi(clength_start);
	    }
	  }  
	}
	return sock_fd;
}

int www_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  assert(argc == 5);

  char *url = argv[1];
  char *cachefile = argv[2];
  char *schemafile = argv[3];
  char *schematype = argv[4];

  int len;
  int fd = open_http(url, &len);
  if (fd < 0) {
    fprintf(stderr, "Could not open URL %s\n", url);
    return TCL_ERROR;
  }

  FILE *fp = fopen(cachefile, "w");
  if (!fp) {
    fprintf(stderr, "Cannot create cache file %s: ", cachefile);
    perror("fopen");
    close(fd);
    return TCL_ERROR;
  }

  char url_data[BUF_LENGTH];
  
  while((len = read(fd, url_data, BUF_LENGTH)) > 0) {
    if (fwrite(url_data, len, 1, fp) != 1) {
      fprintf(stderr, "Cannot write to cache file %s: ", cachefile);
      perror("fwrite");
      close(fd);
      return TCL_ERROR;
    }
  }

  close(fd);

  if (fclose(fp)) {
    fprintf(stderr, "Cannot close cache file %s: ", cachefile);
    perror("close");
    return TCL_ERROR;
  }

  return TCL_OK;
}
