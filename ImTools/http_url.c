#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

extern int errno;

#define HTTP_PORT	80

#define HTTP_GET_FORMAT "GET %s HTTP/1.0"
#define INT_CR 13
#define INT_LF 10

#define HTTP_C_LENGTH "content-length"
#define HTTP_C_LENGTH_LEN strlen(HTTP_C_LENGTH)

#define HTTP_VERSION "HTTP"
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
int open_http( const char *name, int * bytes_in_body)
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
		fprintf(stderr, "http connect() FAILED, errno = %d\n", errno);
		fflush(stderr);
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
	    if (clength_start = strchr(http_cmd,':')) {
	      clength_start++;
	      while (*clength_start == ' ') clength_start++;
	      *bytes_in_body = atoi(clength_start);
	    }
	  }  
	}
	return sock_fd;
}


#ifdef _CAT_URL
main(int argc, char *argv[])
{
  char url_data[BUF_LENGTH];
  int fd, len;
  
  if (argc < 2) {
    fprintf(stderr,"No URL given\n");
    exit(1);
  }
  if (strncmp(argv[1],"http://",7)) {
    fd = open_http(argv[1],&len);
  } else { 
    fd = open_http(argv[1]+7,&len);
  }

  if (fd < 0) {
    fprintf(stderr,"Could not open URL\n");
    exit(1);
  }
  while ((len = read(fd,url_data,BUF_LENGTH)) > 0)
	  write(1,url_data,len);
  close(fd);
  return(0);
}

#endif

