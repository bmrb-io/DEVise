/*
  This file contributed by the Condor project and modified locally.
  See attached copyright notice.
*/

/*
  $Id$

  $Log$
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
#include <assert.h>

#include <tcl.h>
#include <tk.h>

#include "machdep.h"

//#define DEBUG

extern int errno;

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

#define HTTP_PORT	80

#define HTTP_GET_FORMAT "GET %s HTTP/1.0"

#define HTTP_C_LENGTH "content-length"
#define HTTP_C_LENGTH_LEN strlen(HTTP_C_LENGTH)

#define HTTP_VERSION "HTTP/1.0"
#define HTTP_VERSION_LEN strlen(HTTP_VERSION)

#define FTP_PORT	21
#define FTP_LOGIN_RESP	220
#define FTP_PASSWD_RESP 331
#define FTP_PASV_RESP	227
#define FTP_TYPE_RESP	200

#define ASCII_CR ('M' - ' ')
#define ASCII_LF ('J' - ' ')
#define BUF_LENGTH 1024

static char *uname = "anonymous";
static char *passwd = "dummy@";

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

/* read line terminated by CRLF */

static int
readline(int fd, char *buf)
{
	int		count = 0;
	int		rval;

	for(;;) {
		if ((rval = read(fd, buf, 1)) < 0) {
			return rval;
		}
		if (*buf != '\n') {
			count++;
			buf++;
		} else {
			break;
		}
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

	string++;					/* skip the leading '<' */
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
*	always does transfers in binary mode.  We can also only read from ftp URL's
*/

static
char *get_ftpd_response(int sock_fd, int resp_val)
{
	static char	resp[1024];
	int		ftp_resp_num;

	do {
		if (readline(sock_fd, resp) < 0) {
			close(sock_fd);
			return 0;
		}
		sscanf(resp, "%d", &ftp_resp_num);
	} while (ftp_resp_num != resp_val);
	return resp;
}

static
int open_ftp( const char *name )
{
	struct sockaddr_in	sin;
	int		sock_fd;
	int		status;
	char		*port_sep;
	char		*end_of_addr;
	int		port_num = FTP_PORT;
	struct hostent *he;
	char		ftp_cmd[1024];
	int		read_count;
	char		*ftp_resp;
	int		ip_addr[4];
	int		port[2];
	int		rval;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		fprintf(stderr, "socket() failed, errno = %d\n", errno);
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
		return -1;
	}

	status = connect(sock_fd, (struct sockaddr *) &sin, sizeof(sin));
	if (status < 0) {
		fprintf(stderr, "http connect() FAILED, errno = %d\n", errno);
		return status;
	}

#ifdef DEBUG
	printf("ftp: connected to host %s, port %d\n", name, port_num);
#endif

	*end_of_addr = '/';
	name = end_of_addr;

	if (get_ftpd_response(sock_fd, FTP_LOGIN_RESP) == 0) {
		return -1;
	}
	
#ifdef DEBUG
	printf("ftp: sending username\n");
#endif

	sprintf(ftp_cmd, "USER %s\n", uname);
	write(sock_fd, ftp_cmd, strlen(ftp_cmd));
	if (get_ftpd_response(sock_fd, FTP_PASSWD_RESP) == 0) {
		return -1;
	}

#ifdef DEBUG
	printf("ftp: sending password\n");
#endif

	sprintf(ftp_cmd, "PASS %s\n", passwd);
	write(sock_fd, ftp_cmd, strlen(ftp_cmd));

	sprintf(ftp_cmd, "PASV\n");
	write(sock_fd, ftp_cmd, strlen(ftp_cmd));

	ftp_resp = get_ftpd_response(sock_fd, FTP_PASV_RESP);
	if (ftp_resp == 0) {
		return -1;
	}

#ifdef DEBUG
	printf("ftp: setting binary data transfer type\n");
#endif

	sprintf(ftp_cmd, "TYPE I\n");
	write(sock_fd, ftp_cmd, strlen(ftp_cmd));
	if (get_ftpd_response(sock_fd, FTP_TYPE_RESP) == 0) {
		return -1;
	}

#ifdef DEBUG
	printf("ftp: sending retrieve command\n");
#endif

	sprintf(ftp_cmd, "RETR %s\n", name);
	write(sock_fd, ftp_cmd, strlen(ftp_cmd));

	for ( ; *ftp_resp != '(' ; ftp_resp++) 
		;

	ftp_resp++;
	sscanf(ftp_resp, "%d,%d,%d,%d,%d,%d", &ip_addr[0], &ip_addr[1], 
		   &ip_addr[2], &ip_addr[3], &port[0], &port[1]);

	sprintf(ftp_cmd, "cbstp:<%d.%d.%d.%d:%d>", ip_addr[0], ip_addr[1], 
		   ip_addr[2], ip_addr[3], port[0] << 8 | port[1]);
	rval = condor_bytes_stream_open_ckpt_file( ftp_cmd );
	close(sock_fd);

#ifdef DEBUG
	printf("ftp: closed command socket\n");
#endif

	return rval;
}

extern
int open_http( char *name, size_t * bytes_in_body)
{

	struct sockaddr_in	sin;
	int	sock_fd;
	int	status;
	char	*port_sep;
	char	*end_of_addr;
	char    *clength_start;
	char    *stat_code_start, *reason_phrase_start;
	int	stat_code, l;
	int	port_num = HTTP_PORT;
	struct hostent *he;
	char	http_cmd[BUF_LENGTH];

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		fprintf(stderr, "socket() failed, errno = %d\n", errno);
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
		return status;
	}

	name = end_of_addr;
	sprintf(http_cmd, HTTP_GET_FORMAT, name);
	l = strlen(http_cmd);
	http_cmd[l++] = ASCII_CR;
	http_cmd[l++] = ASCII_LF;
	http_cmd[l++] = ASCII_CR;
	http_cmd[l++] = ASCII_LF;
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
	*bytes_in_body = 0;
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

  int fd;
  size_t len = 0;
  if (!strncmp(url, "ftp://", 6))
    fd = open_ftp(url);
  else
    fd = open_http(url, &len);
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
  
  while((len = read(fd, url_data, sizeof url_data)) > 0) {
    UPDATE_TCL;
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
