/*
  This file contributed by the Condor project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$
  Revision 1.6  1996/04/16 20:38:49  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.5  1996/03/24 17:19:36  jussi
  Fixed bugs in open_ftp and open_http.

  Revision 1.4  1996/02/01 18:00:11  jussi
  Added detection of 'no such file' response from ftp server.
  Also added error messages in case of network or protocol
  errors.

  Revision 1.3  1996/01/30 22:44:24  jussi
  Added ftp and cbstp protocol support.

  Revision 1.2  1996/01/26 19:45:38  jussi
  Added TCL_UPDATE statement.

  Revision 1.1  1996/01/13 20:51:39  jussi
  Initial revision.
*/

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

#include <tcl.h>
#include <tk.h>

#include "Exit.h"
#include "machdep.h"

//#define DEBUG

extern int errno;

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

#define HTTP_PORT	80

#define HTTP_GET_FORMAT "GET %s HTTP/1.0\n\n"

#define HTTP_C_LENGTH "content-length"
#define HTTP_C_LENGTH_LEN strlen(HTTP_C_LENGTH)

#define HTTP_VERSION "HTTP/1.0"
#define HTTP_VERSION_LEN strlen(HTTP_VERSION)

#define FTP_PORT	21
#define FTP_LOGIN_RESP	220
#define FTP_PASSWD_RESP 331
#define FTP_PASV_RESP	227
#define FTP_TYPE_RESP	200
#define FTP_NOFILE_RESP 550
#define FTP_OPEN_CONN   150

#define BUF_LENGTH 1024

static char *uname = "anonymous";
static char *passwd = "dummy@";

static char buffer[BUF_LENGTH];

static void
tolowcase(char *s)
{
  int c, i = 0, delta;
  while ((c = s[i]) != '\0') {
    if (((delta = (c - 'A')) >= 0) && (delta < 26))
      s[i] = 'a' + delta;
    i++;
    if (i == BUF_LENGTH)
      break;
  }
}

/* read line terminated by CRLF */

static int
readline(int fd, char *buf)
{
	int		count = 0;
	int		rval;

	for(;;) {
		if ((rval = read(fd, buf, 1)) < 0)
			return rval;
		if (!rval || *buf == '\n')
			break;
		count++;
		buf++;
	}
	return	count;
}

/* Convert a string of the form "<xx.xx.xx.xx:pppp>" to a sockaddr_in  JCP */

static void
string_to_sin(char *string, struct sockaddr_in *sin)
{
	int             i;
	char    *cur_byte;
	char    *end_string;

	string++;				/* skip the leading '<' */
	cur_byte = (char *) &(sin->sin_addr);
	for(end_string = string; end_string != 0; ) {
		end_string = strchr(string, '.');
		if (end_string == 0) {
			end_string = strchr(string, ':');
		}
		if (end_string) {
			*end_string = '\0';
			*cur_byte = atoi(string);
			cur_byte++;
			string = end_string + 1;
			*end_string = '.';
		}
	}
	
	end_string = strchr(string, '>');
	if (end_string) {
		*end_string = '\0';
	}
	sin->sin_port = htons(atoi(string));
	if (end_string) {
		*end_string = '>';
	}
}

static
int condor_bytes_stream_open_ckpt_file( char *name )
{
	struct sockaddr_in	sin;
	int		sock_fd;
	int		status;
	
	if (strncmp(name,"cbstp:",6)) {
		return -1;
	}
	name += 6;

	string_to_sin(name, &sin);
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		fprintf(stderr, "socket() failed, errno = %d\n", errno);
		return sock_fd;
	}
	sin.sin_family = AF_INET;
	status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
	if (status < 0) {
		fprintf(stderr, "cbstp connect() FAILED, errno = %d\n", errno);
		return status;
	}

#ifdef DEBUG
	printf("cbstp: connected to %s\n", name);
#endif

	return sock_fd;
}

/*
*	A full fledged ftp url looks like:
*	ftp://[username[:password]@]host/path/file
*	The current implementation does not support username or password, and
*	always does transfers in binary mode.  We can also only read from ftp
*	URL's
*/

static
char *get_ftpd_response(int sock_fd, int resp_val)
{
  int		ftp_resp_num = -1;
  
  do {
    if (readline(sock_fd, buffer) < 0) {
      fprintf(stderr, "Premature end of data from ftp server.\n");
      return 0;
    }
    sscanf(buffer, "%d", &ftp_resp_num);
  } while (ftp_resp_num != resp_val);

  return buffer;
}

static
int open_ftp( char *name )
{
  struct sockaddr_in	sin;
  int		sock_fd;
  int		status;
  char		*port_sep;
  char		*end_of_addr;
  int		port_num = FTP_PORT;
  struct hostent *he;
  int		read_count;
  char		*ftp_resp;
  int		ip_addr[4];
  int		port[2];
  int		rval;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    return sock_fd;
  }

  if (strncmp(name,"ftp://",6)) {
    return -1;
  }
  name += 6;

  end_of_addr = strchr(name, '/');
  if (end_of_addr == 0) {
    return -1;
  }
  *end_of_addr = '\0';

  sin.sin_port = htons(port_num);

  he = gethostbyname( name );
  if ( he ) {
    sin.sin_family = he->h_addrtype;
    sin.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
  } else {
    fprintf(stderr, "Cannot translate hostname %s.\n", name);
    return -1;
  }
  
  status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
  if (status < 0) {
    fprintf(stderr, "Cannot connect to ftp server %s: ", name);
    perror("connect");
    return status;
  }

#ifdef DEBUG
  printf("ftp: connected to host %s, port %d\n", name, port_num);
#endif

  *end_of_addr = '/';
  name = end_of_addr;

  if (get_ftpd_response(sock_fd, FTP_LOGIN_RESP) == 0) {
    fprintf(stderr, "No login message from ftp server.\n");
    goto error;
  }
	
#ifdef DEBUG
  printf("ftp: sending username\n");
#endif

  sprintf(buffer, "USER %s\n", uname);
  write(sock_fd, buffer, strlen(buffer));
  if (get_ftpd_response(sock_fd, FTP_PASSWD_RESP) == 0) {
    fprintf(stderr, "No password message from ftp server.\n");
    goto error;
  }

#ifdef DEBUG
  printf("ftp: sending password\n");
#endif

  sprintf(buffer, "PASS %s\n", passwd);
  write(sock_fd, buffer, strlen(buffer));

#ifdef DEBUG
  printf("ftp: setting binary data transfer type\n");
#endif

  sprintf(buffer, "TYPE I\n");
  write(sock_fd, buffer, strlen(buffer));
  if (get_ftpd_response(sock_fd, FTP_TYPE_RESP) == 0) {
    fprintf(stderr, "Cannot set ftp server to binary mode.\n");
    goto error;
  }

  sprintf(buffer, "PASV\n");
  write(sock_fd, buffer, strlen(buffer));

  ftp_resp = get_ftpd_response(sock_fd, FTP_PASV_RESP);
  if (ftp_resp == 0) {
    fprintf(stderr, "No network address message from ftp server.\n");
    goto error;
  }
  if (sscanf(ftp_resp, "%*[^(](%d,%d,%d,%d,%d,%d)",
	     &ip_addr[0], &ip_addr[1], &ip_addr[2], &ip_addr[3],
	     &port[0], &port[1]) != 6) {
    fprintf(stderr, "Cannot parse network address: %s.\n", ftp_resp);
    goto error;
  }

  sprintf(buffer, "cbstp:<%d.%d.%d.%d:%d>", ip_addr[0], ip_addr[1], 
	  ip_addr[2], ip_addr[3], port[0] << 8 | port[1]);
  rval = condor_bytes_stream_open_ckpt_file( buffer );
  if (rval < 0) {
    fprintf(stderr, "Cannot connect to ftp secondary port.\n");
    close(sock_fd);
    return rval;
  }

#ifdef DEBUG
  printf("ftp: sending retrieve command\n");
#endif

  sprintf(buffer, "RETR %s\n", name);
  write(sock_fd, buffer, strlen(buffer));

  if (readline(sock_fd, buffer) < 0) {
    fprintf(stderr, "Premature end of data from ftp server.\n");
    goto error;
  }
  if (atoi(buffer) == FTP_NOFILE_RESP) {
    fprintf(stderr, "File not found at ftp server: %s.\n", name);
    goto error;
  }
  if (atoi(buffer) != FTP_OPEN_CONN) {
    fprintf(stderr, "Unknown error at ftp server: %s.\n", buffer);
    goto error;
  }

  close(sock_fd);

#ifdef DEBUG
  printf("ftp: closed command socket\n");
#endif

  return rval;

 error:
  close(sock_fd);
  return -1;
}

extern
int open_http( char *name, size_t * bytes_in_body)
{
  struct sockaddr_in sin;
  int	sock_fd;
  int	status;
  char	*port_sep;
  char	*end_of_addr;
  char  *clength_start;
  char  *stat_code_start;
  char  *reason_phrase_start;
  int	stat_code, l;
  int	port_num = HTTP_PORT;
  struct hostent *he;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    return sock_fd;
  }

  if (strncmp(name,"http://", 7))
    return -1;

  name += 7;

  port_sep = strchr(name, ':');
  end_of_addr = strchr(name, '/');
  if (!end_of_addr)
    return -1;

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
    fprintf(stderr, "Cannot translate host name %s.\n", name);
    return -1;
  }

  status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
  if (status < 0) {
    fprintf(stderr, "Cannot connect to http server %s: ", name);
    perror("connect");
    return status;
  }

  *end_of_addr = '/';
  sprintf(buffer, HTTP_GET_FORMAT, end_of_addr);

#ifdef DEBUG
  printf("http: sending get command\n");
#endif

  write(sock_fd, buffer, strlen(buffer));

  /* get status line */
  if (readline(sock_fd, buffer) <= 0) {
    fprintf(stderr, "Bad http connection\n");
    goto error;
  }

  if (strncmp(buffer, HTTP_VERSION, HTTP_VERSION_LEN)) {
    fprintf(stderr, "Bad http header received\n");
    goto error;
  }

  if (!(stat_code_start = strchr(buffer,' '))) { 
    fprintf(stderr, "Bad http header received\n");
    goto error;
  }
  stat_code_start++;

  if (!(reason_phrase_start = strchr(stat_code_start,' '))) { 
    fprintf(stderr, "Bad http header received\n");
    goto error;
  }
  *reason_phrase_start = '\0';
  reason_phrase_start++;

  stat_code = atoi(stat_code_start);
  if (stat_code < 200) {
    fprintf(stderr, "Unused status code (%d) received\n", stat_code);
    goto error;
  }
  else if (stat_code < 300) {
    /* cool.. */
  }
  else if (stat_code < 400) {
    fprintf(stderr, "Redirection error: %s\n", reason_phrase_start);
    goto error;
  }
  else if (stat_code < 500) {
    fprintf(stderr, "Client error: %s\n", reason_phrase_start);
    goto error;
  }
  else if (stat_code < 600) {
    fprintf(stderr, "Server error: %s\n", reason_phrase_start);
    goto error;
  }
  else {
    fprintf(stderr, "Strange status code (%d) received\n", stat_code);
    goto error;
  }

  /* Skip the header part, but extract Content-Length */
  *bytes_in_body = 0;
  while (readline(sock_fd, buffer) > 0) {
    tolowcase(buffer);
    if (!strncmp(buffer, HTTP_C_LENGTH, HTTP_C_LENGTH_LEN)) {
      sscanf(buffer, "%*s %d", bytes_in_body);
      break;
    }  
  }

  return sock_fd;

 error:
  close(sock_fd);
  return -1;
}

int www_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  DOASSERT(argc == 5, "Invalid parameters");

  char *url = argv[1];
  char *cachefile = argv[2];
  char *schemafile = argv[3];
  char *schematype = argv[4];

  int fd;
  size_t len = 0;
  if (!strncmp(url, "ftp://", 6))
    fd = open_ftp(url);
  else
    fd = open_http(url, &len);
  if (fd < 0) {
    fprintf(stderr, "Could not open URL %s.\n", url);
    return TCL_ERROR;
  }

  FILE *fp = fopen(cachefile, "w");
  if (!fp) {
    fprintf(stderr, "Cannot create cache file %s: ", cachefile);
    perror("fopen");
    close(fd);
    return TCL_ERROR;
  }

  while((len = read(fd, buffer, sizeof buffer)) > 0) {
    UPDATE_TCL;
    if (fwrite(buffer, len, 1, fp) != 1) {
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
