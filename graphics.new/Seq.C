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

  $Log$
  Revision 1.1  1996/01/09 16:33:44  jussi
  Initial revision.
*/

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
#ifdef DEBUG
  char *orig = buf;
#endif

  int nleft = nbytes;
  while(nleft > 0) {
    int nread = read(fd, buf, nleft);
    if (nread < 0)                      // error?
      return nread;
    if (nread == 0)                     // EOF?
      break;
    nleft -= nread;
    buf   += nread;
    if (buf[-1] == 0)
      break;
  }

#ifdef DEBUG
  printf("readn: got %d bytes: %.*s\n", nbytes - nleft, nbytes - nleft, orig);
#endif

  return nbytes - nleft;
}
  
int ParseSEQHeader(char *&buf, int &len)
{
  const char *resultHeader = "Result of Execution\n";

  if (memcmp(buf, resultHeader, strlen(resultHeader))) {
    fprintf(stderr, "Unexpected header from SEQ server:\n");
    fprintf(stderr, "  %.256s\n", buf);
    return TCL_ERROR;
  }

  buf += strlen(resultHeader);
  len -= strlen(resultHeader);

  return TCL_OK;
}

int ParseSEQSchema(char *&buf, int &len, char *schemafile, char *schematype,
		   char *&schema, double *&minVal, double *&maxVal,
		   char *&sdef)
{
  const char *schemaHdr = "Schema ( < ";
  const char *orderAttr = " >) OrderAttr ";
  const char *dateField = "hmdy";
  const char *intField = "int";
  const char *floatField = "float";
  const char *stringField = "string";
  const char *attrSep = ", ";
  const char attrTypeSep = ':';

  // when this function is called the first time, space is allocated for
  // for sdef and the schema is stored there in SEQ language;
  // upon the next call, the schema is used from sdef

  if (!sdef) {
    char *eol = (char *)memchr(buf, '\n', len);
    if (!eol) {
      fprintf(stderr, "Unexpected schema from SEQ server:\n");
      fprintf(stderr, "  %.256s\n", buf);
      return TCL_ERROR;
    }

    *eol = 0;

    sdef = new char [strlen(buf) + 1];
    if (!sdef) {
      fprintf(stderr, "Cannot allocate schema definition.\n");
      return TCL_ERROR;
    }
    strcpy(sdef, buf);
    
    eol++;
    len -= (eol - buf);
    buf = eol;
  }

  if (strncmp(sdef, schemaHdr, strlen(schemaHdr))) {
    fprintf(stderr, "Invalid schema header: %s\n", sdef);
    return TCL_ERROR;
  }
  char *attrName = &sdef[strlen(schemaHdr)];

  int sortedAttr = -1;
  char *order = strchr(sdef, orderAttr[1]);
  if (order && !strncmp(order - 1, orderAttr, strlen(orderAttr))
      && isdigit(*(order - 1 + strlen(orderAttr)))) {
    sortedAttr = atoi(order - 1 + strlen(orderAttr));
    *(order - 1) = 0;
  } else {
    fprintf(stderr, "Invalid schema definition: %s\n", sdef);
    return TCL_ERROR;
  }

  FILE *fp = 0;

  if (schemafile) {
    fp = fopen(schemafile, "w");
    if (!fp) {
      fprintf(stderr, "Cannot create schema file %s: ", schemafile);
      perror("fopen");
      return TCL_ERROR;
    }
    
    fprintf(fp, "type %s ascii\n", schematype);
    fprintf(fp, "comment //\n");
    fprintf(fp, "separator ','\n");
  }

  const int maxNumAttrs = 16;
  static char schemaDef[maxNumAttrs + 1];
  static double minVals[maxNumAttrs];
  static double maxVals[maxNumAttrs];

  if (!schema) {
    assert(!minVal && !maxVal);
    schema = schemaDef;
    minVal = minVals;
    maxVal = maxVals;
    memset(schemaDef, 0, maxNumAttrs + 1);
    for(int a = 0; a < maxNumAttrs; a++) {
      minVal[a] = 1.0;
      maxVal[a] = 0.0;
    }
  } else {
    assert(minVal && maxVal);
  }

  char *beginDef = attrName;

  for(int i = 0;; i++) {
    if (i >= maxNumAttrs) {
      fprintf(stderr, "Too many attributes in schema: %s\n", beginDef);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }
    char *colon = strchr(attrName, attrTypeSep);
    if (!colon)  {
      fprintf(stderr, "Invalid attribute definition: %s\n", attrName);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }

    *colon = 0;
    colon++;

    if (fp) {
      if (i == sortedAttr)
	fprintf(fp, "sorted ");
      fprintf(fp, "attr %s ", attrName);
    }

    char *nextAttr = 0;
    if (!strncmp(colon, dateField, strlen(dateField))) {
      if (schema[i] == 0)
	schema[i] = 'd';
      if (fp)
	fprintf(fp, "date hi %ld lo %ld\n", (long)maxVal[i], (long)minVal[i]);
      nextAttr = colon + strlen(dateField);
    } else if (!strncmp(colon, intField, strlen(intField))) {
      if (schema[i] == 0)
	schema[i] = 'i';
      if (fp)
	fprintf(fp, "int hi %ld lo %ld\n", (long)maxVal[i], (long)minVal[i]);
      nextAttr = colon + strlen(intField);
    } else if (!strncmp(colon, floatField, strlen(floatField))) {
      if (schema[i] == 0)
	schema[i] = 'f';
      if (fp)
	fprintf(fp, "float hi %g lo %g\n", minVal[i], maxVal[i]);
      nextAttr = colon + strlen(floatField);
    } else if (!strncmp(colon, stringField, strlen(stringField))) {
      if (schema[i] == 0)
	schema[i] = 's';
      if (fp)
	fprintf(fp, "string");
      nextAttr = colon + strlen(stringField);
    } else {
      fprintf(stderr, "Invalid field type: %s\n", attrName);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }

    *(colon - 1) = attrTypeSep;

    if (!*nextAttr)
      break;

    if (strncmp(nextAttr, attrSep, strlen(attrSep))) {
      fprintf(stderr, "Invalid field separator: %s\n", nextAttr);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }

    attrName = nextAttr + strlen(attrSep);
  }
  
  *(order - 1) = orderAttr[0];

#ifdef DEBUG
  printf("Parsed schema as %s, ordered attribute %d\n", schema,
	 sortedAttr);
#endif

  if (fp && fclose(fp)) {
    fprintf(stderr, "Cannot close schema file %s: ", schemafile);
    perror("fclose");
    return TCL_ERROR;
  }

  return TCL_OK;
}

int WriteSEQRecord(char *rec, int len, FILE *fp, char *cachefile,
		   char *schema, double *minVal, double *maxVal)
{
  assert(rec && fp && cachefile && schema && minVal && maxVal);

#ifdef DEBUG
  printf("Converting record (%d bytes): %s\n", len, rec);
#endif

  const char attrSep = '\t';

  for(int i = 0; i < strlen(schema); i++) {
    char *fieldSep = strchr(rec, attrSep);
    if (!fieldSep) {
      fprintf(stderr, "Illegal data record: %s\n", rec);
      fclose(fp);
      return TCL_ERROR;
    }
    *fieldSep = 0;

    if (schema[i] == 'd') {
      // convert SQL time format to Unix time format
      struct tm t;
      if (sscanf(rec, "%d-%d-%d %d:%d:%d", &t.tm_year, &t.tm_mon,
		 &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) != 6) {
	fprintf(stderr, "Invalid date field: %s\n", rec);
	fclose(fp);
	return TCL_ERROR;
      }
      // HACK HACK until seq try1 relation fixed
#if 1
      t.tm_year += 1896;
#endif
      t.tm_year -= 1900;
      t.tm_mon--;
      t.tm_isdst = -1;
      time_t tim = mktime(&t);
#ifdef DEBUG
      printf("Converting time %04d-%02d-%02d %02d:%02d:%02d\n", t.tm_year,
	     t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#endif
      if (tim == (time_t)-1) {
	fprintf(stderr, "Invalid date field: %s\n", rec);
	fclose(fp);
	return TCL_ERROR;
      }
      fprintf(fp, "%ld", (long int)tim);
      if (minVal[i] > maxVal[i])
	minVal[i] = maxVal[i] = (double)tim;
      else if (tim < minVal[i])
	minVal[i] = (double)tim;
      else if (tim > maxVal[i])
	maxVal[i] = (double)tim;
    } else if (schema[i] == 'i' || schema[i] == 'd') {
      // write other attribute types as-is
      fwrite(rec, strlen(rec), 1, fp);
      double value = atof(rec);
      if (minVal[i] > maxVal[i])
	minVal[i] = maxVal[i] = value;
      else if (value < minVal[i])
	minVal[i] = value;
      else if (value > maxVal[i])
	maxVal[i] = value;
    } else if (schema[i] == 's') {
      // write string attributes types as-is
      fwrite(rec, strlen(rec), 1, fp);
    } else {
      assert(0);
    }

    rec = fieldSep + 1;

    if (i < strlen(schema) - 1)
      fprintf(fp, ",");
  }

  fprintf(fp, "\n");

  return TCL_OK;
}

int ParseSEQData(char *buf, int size, int len, int sock,
                 char *cachefile, char *schema, double *minVal,
		 double *maxVal)
{
  assert(buf && cachefile && schema && minVal && maxVal);

  const char *resultTrailer = "\n";

  FILE *fp = fopen(cachefile, "w");
  if (!fp) {
    fprintf(stderr, "Cannot create cache file %s: ", cachefile);
    perror("fopen");
    return TCL_ERROR;
  }

  int pos = 0;

  while(1) {
    char *line = buf + pos;
    if (!memcmp(line, resultTrailer, strlen(resultTrailer)))
      break;

    char *eol = (char *)memchr(line, '\n', len);

#ifdef DEBUG
    printf("Pos %d, len %d, eol %d\n", pos, len, (eol - buf));
#endif

    if (!eol || *(eol + 1) != '\n') {
      if (line[len - 1] != 0) {
	// terminating null not received yet, so go fetch more data
#ifdef DEBUG
	printf("Partial record, fetching more data\n");
#endif
	// shift partial line to beginning of buffer
	memcpy(buf, line, len);
	pos = 0;
	line = buf;
	int bytes = readn(sock, buf + len, size - len);
	if (bytes < 0) {
	  perror("Error in SEQ server connection: read");
	  fclose(fp);
	  return TCL_ERROR;
	}
	len += bytes;
	eol = (char *)memchr(line, '\n', len);
      }
      if (!eol || *(eol + 1) != '\n') {
	fprintf(stderr, "Unexpected data from SEQ server:\n");
	fprintf(stderr, "  %.256s\n", line);
	fclose(fp);
	return TCL_ERROR;
      }
    }

    *eol = 0;                           // replace newline with null
    int status = WriteSEQRecord(line, strlen(line), fp, cachefile, schema,
				minVal, maxVal);
    if (status != TCL_OK)
      return status;

    eol += 2;                           // skip over both newlines
    len -= (eol - line);
    pos += (eol - line);
  }

  if (fclose(fp)) {
    fprintf(stderr, "Cannot close cache file %s: ", cachefile);
    perror("close");
    return TCL_ERROR;
  }

  return TCL_OK;
}

int seq_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  assert(argc == 7);

  char *hostname = argv[1];
  int port = atoi(argv[2]);
  char *cachefile = argv[4];
  char *schemafile = argv[5];
  char *schematype = argv[6];

  struct hostent *hostp;
  if (!(hostp = gethostbyname(hostname))) {
    fprintf(stderr, "Unknown host: %s", hostname);
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

  printf("Connecting to SEQ server %s (%s:%d)\n", hostname,
	 inet_ntoa(addr.sin_addr), port);

  if (connect(sock, (struct sockaddr *)&addr, sizeof addr) < 0) {
    perror("Error in SEQ server connection: connect");
    return TCL_ERROR;
  }

  printf("Transferring data...\n");

  char *cmdbuf = new char [4 + strlen(argv[3]) + 1];
  if (!cmdbuf) {
    fprintf(stderr, "Cannot allocate command buffer\n");
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

  char *buf = recvBuf;
  int status = ParseSEQHeader(buf, bytes);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  char *sdef = 0;                       // schema in SEQ
  char *schema = 0;                     // attribute characters
  double *minVal = 0;                   // minimum values
  double *maxVal = 0;                   // maximum values
  status = ParseSEQSchema(buf, bytes, 0, 0, schema, minVal, maxVal, sdef);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  // shift data to beginning of buffer
  memcpy(recvBuf, buf, bytes);
  status = ParseSEQData(recvBuf, sizeof recvBuf, bytes, sock,
			cachefile, schema, minVal, maxVal);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  status = ParseSEQSchema(buf, bytes, schemafile, schematype, schema,
			  minVal, maxVal, sdef);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  delete sdef;

  if (close(sock)) {
    perror("Error in SEQ server connection: close");
    return TCL_ERROR;
  }

  return TCL_OK;
}
