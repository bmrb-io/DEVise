/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$*/

#ifdef HPUX
#undef __P                              // defined by libio.h and netdb.h
#endif

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <tcl.h>
#include <tk.h>

#ifndef SUN
#include <arpa/inet.h>
#else
extern "C" char *inet_ntoa(struct in_addr);
#endif
#ifdef ALPHA
extern "C" struct hostent *gethostbyname(char *);
#endif

#if !defined(HPUX) && !defined(ALPHA)
extern "C" {
  extern int socket(int, int, int);
  extern int bind(int, struct sockaddr *, int);
  extern int listen(int, int);
  extern int accept(int, struct sockaddr *, int *);
  extern int connect(int, struct sockaddr *, int);
  extern int shutdown(int, int);
}
#endif

//#define DEBUG

static int readn(int fd, char *buf, int nbytes)
{
  int nleft = nbytes;
  while(nleft > 0) {
    int nread = read(fd, buf, nleft);
    if (nread < 0)                      // error?
      return nread;
    if (nread == 0)                     // EOF?
      break;
    nleft -= nread;
    buf   += nread;
  }

  return nbytes - nleft;
}
  
int ParseSEQHeader(char *&buf, int &len)
{
  const char *resultHeader = "Result of Execution\n";

  if (memcmp(buf, resultHeader, strlen(resultHeader))) {
    printf("Unexpected header from SEQ server:\n");
    printf("  %.256s\n", buf);
    return TCL_ERROR;
  }

  buf += strlen(resultHeader);
  len -= strlen(resultHeader);

  return TCL_OK;
}

int ParseSEQSchema(char *&buf, int &len, char *schemafile, char *&schema)
{
  const char *schemaHdr = "Schema ( < ";
  const char *orderAttr = " >) OrderAttr ";
  const char *dateField = "hmdy";
  const char *intField = "int";
  const char *floatField = "float";
  const char *attrSep = ", ";

  char *eol = (char *)memchr(buf, '\n', len);
  if (!eol) {
    printf("Unexpected schema from SEQ server:\n");
    printf("  %.256s\n", buf);
    return TCL_ERROR;
  }

  *eol = 0;
  char *s = buf;

  eol++;
  len -= (eol - buf);
  buf = eol;

  if (strncmp(s, schemaHdr, strlen(schemaHdr))) {
    printf("Invalid schema header: %s\n", s);
    return TCL_ERROR;
  }
  char *attrName = &s[strlen(schemaHdr)];

  int sortedAttr = -1;
  char *order = strchr(s, '>');
  if (order && !strncmp(order - 1, orderAttr, strlen(orderAttr))
      && isdigit(*(order - 1 + strlen(orderAttr)))) {
    sortedAttr = atoi(order - 1 + strlen(orderAttr));
    *(order - 1) = 0;
  } else {
    printf("Invalid schema definition: %s\n", s);
    return TCL_ERROR;
  }

  FILE *fp = fopen(schemafile, "w");
  if (!fp) {
    printf("Cannot create schema file %s: ", schemafile);
    perror("fopen");
    return TCL_ERROR;
  }

  static int schemaSeqNum = 1;

  fprintf(fp, "type SEQ-%d ascii\n", schemaSeqNum);
  fprintf(fp, "comment //\n");
  fprintf(fp, "separator ','\n");

  const int maxNumAttrs = 16;
  static char schemaDef[maxNumAttrs + 1];
  *(schema = schemaDef) = 0;
  s = attrName;

  for(int i = 0;; i++) {
    if (i >= maxNumAttrs) {
      printf("Too many attributes in schema: %s\n", s);
      fclose(fp);
      return TCL_ERROR;
    }
    char *colon = strchr(attrName, ':');
    if (!colon)  {
      printf("Invalid attribute definition: %s\n", attrName);
      fclose(fp);
      return TCL_ERROR;
    }

    *colon = 0;
    colon++;
    if (i == sortedAttr)
      fprintf(fp, "sorted ");

    fprintf(fp, "attr %s ", attrName);
    char *nextAttr = 0;
    if (!strncmp(colon, dateField, strlen(dateField))) {
      strcat(schema, "d");
      fprintf(fp, "date\n");
      nextAttr = colon + strlen(dateField);
    } else if (!strncmp(colon, intField, strlen(intField))) {
      strcat(schema, "i");
      fprintf(fp, "int\n");
      nextAttr = colon + strlen(intField);
    } else if (!strncmp(colon, floatField, strlen(floatField))) {
      strcat(schema, "f");
      fprintf(fp, "float\n");
      nextAttr = colon + strlen(floatField);
    } else {
      printf("Invalid field type: %s\n", attrName);
      fclose(fp);
      return TCL_ERROR;
    }

    if (!*nextAttr)
      break;

    if (strncmp(nextAttr, attrSep, strlen(attrSep))) {
      printf("Invalid field separator: %s\n", nextAttr);
      fclose(fp);
      return TCL_ERROR;
    }

    attrName = nextAttr + strlen(attrSep);
  }
  
#ifdef DEBUG
  printf("Parsed schema as: %s\n", schema);
#endif

  if (fclose(fp)) {
    printf("Cannot close schema file %s: ", schemafile);
    perror("fclose");
    return TCL_ERROR;
  }

  schemaSeqNum++;

  return TCL_OK;
}

int WriteSEQRecord(char *rec, int len, FILE *fp,
		   char *cachefile, char *schema)
{
  for(int i = 0; i < strlen(schema); i++) {
    char *fieldSep = strchr(rec, '\t');
    if (!fieldSep) {
      printf("Illegal data record: %s\n", rec);
      fclose(fp);
      return TCL_ERROR;
    }
    *fieldSep = 0;

    if (schema[i] == 'd') {
      // convert SQL time format to Unix time format
      struct tm t;
      if (sscanf(rec, "%d-%d-%d %d:%d:%d", &t.tm_year, &t.tm_mon,
		 &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) != 6) {
	printf("Invalid date field: %s\n", rec);
	fclose(fp);
	return TCL_ERROR;
      }
      t.tm_year -= 1900;
      t.tm_mon--;
      t.tm_isdst = -1;
      time_t tim = mktime(&t);
      if (tim == (time_t)-1) {
	printf("Invalid date field: %s: ", rec);
	perror("mktime");
	fclose(fp);
	return TCL_ERROR;
      }
      fprintf(fp, "%ld", (long int)tim);
    } else if (schema[i] == 'i' || schema[i] == 'f') {
      // write integers and floats as-is
      fwrite(rec, strlen(rec), 1, fp);
    } else {
      assert(0);
    }

    rec = fieldSep + 1;
  }

  return TCL_OK;
}

int ParseSEQData(char *buf, int size, int len, int sock,
                 char *cachefile, char *schema)
{
  const char *resultTrailer = "\nClockTime";

  FILE *fp = fopen(cachefile, "w");
  if (!fp) {
    printf("Cannot create cache file %s: ", cachefile);
    perror("fopen");
    return TCL_ERROR;
  }

  int pos = 0;

  while(1) {
    char *line = buf + pos;
    if (!memcmp(line, resultTrailer, strlen(resultTrailer)))
      break;

    char *eol = (char *)memchr(line, '\n', len);

    if (!eol || *(eol + 1) != '\n') {
      // shift partial line to beginning of buffer
      memcpy(buf, line, len);
      pos = 0;
      line = buf;
      int bytes = readn(sock, buf + len, size - len);
      if (bytes < 0) {
	perror("Erorr in SEQ server connection: read");
	fclose(fp);
	return TCL_ERROR;
      }
      len += bytes;
      eol = (char *)memchr(line, '\n', len);
      if (!eol || *(eol + 1) != '\n') {
	printf("Unexpected data from SEQ server:\n");
	printf("  %.256s\n", line);
	fclose(fp);
	return TCL_ERROR;
      }
    }

    *eol = 0;                           // replace newline with null
    int status = WriteSEQRecord(line, strlen(line), fp, cachefile, schema);
    if (status != TCL_OK)
      return status;

    eol++;
    len -= (eol - buf);
    pos += (eol - buf);
  }

  if (fclose(fp)) {
    printf("Cannot close cache file %s: ", cachefile);
    perror("close");
    return TCL_ERROR;
  }

  return TCL_OK;
}

int seq_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  assert(argc == 6);

  char *hostname = argv[1];
  int port = atoi(argv[2]);
  char *cachefile = argv[4];
  char *schemafile = argv[5];

  struct hostent *hostp;
  if (!(hostp = gethostbyname(hostname))) {
    sprintf(interp->result, "Unknown host: %s", hostname);
    return TCL_ERROR;
  }

  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Error in SEQ server connection: socket");
    return TCL_ERROR;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr, hostp->h_addr, hostp->h_length);
  addr.sin_port = htons((unsigned short)port);

  if (connect(sock, (struct sockaddr *)&addr, sizeof addr) < 0) {
    perror("Error in SEQ server connection: connect");
    return TCL_ERROR;
  }

  printf("Connected to SEQ server %s (%s:%d)\n", hostname,
	 inet_ntoa(addr.sin_addr), port);

  char *cmdbuf = new char [4 + strlen(argv[3]) + 1];
  if (!cmdbuf) {
    printf("Cannot allocate command buffer\n");
    return TCL_ERROR;
  }

  sprintf(cmdbuf, "SEQ:%s", argv[3]);

#ifdef DEBUG
  printf("Sending SEQ command (%d bytes) \"%s\"\n", (int)strlen(cmdbuf),
	 cmdbuf);
#endif

  if (write(sock, cmdbuf, strlen(cmdbuf)) != strlen(cmdbuf)) {
    perror("Error in SEQ server connection: write");
    return TCL_ERROR;
  }
  
  delete cmdbuf;

  char recvBuf[32 * 1024];
  int bytes;
  if ((bytes = readn(sock, recvBuf, sizeof recvBuf)) < 0) {
    perror("Error in SEQ server connection: read");
    return TCL_ERROR;
  }

#ifdef DEBUG
  printf("Received reply (%d bytes) \"%s\"\n", bytes, buf);
#endif

  char *buf = recvBuf;
  int status = ParseSEQHeader(buf, bytes);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  char *schema;
  status = ParseSEQSchema(buf, bytes, schemafile, schema);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  // shift data to beginning of buffer
  memcpy(recvBuf, buf, bytes);
  status = ParseSEQData(recvBuf, sizeof recvBuf, bytes, sock,
			cachefile, schema);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  if (close(sock)) {
    perror("Erorr in SEQ server connection: close");
    return TCL_ERROR;
  }

  return TCL_OK;
}
