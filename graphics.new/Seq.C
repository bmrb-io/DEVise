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
  Revision 1.11  1996/12/03 20:33:22  jussi
  Moved readn() to Util.c.

  Revision 1.10  1996/10/02 15:23:50  wenger
  Improved error handling (modified a number of places in the code to use
  the DevError class).

  Revision 1.9  1996/05/31 15:39:31  jussi
  Minor changes to remove compiler warnings in Linux.

  Revision 1.8  1996/04/16 20:38:47  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.7  1996/01/13 03:19:13  jussi
  Moved platform-dependent network stuff to machdep.h.

  Revision 1.6  1996/01/12 18:30:00  jussi
  Network functions need not be defined in SGI because socket.h
  defines them properly.

  Revision 1.5  1996/01/12 17:59:03  jussi
  The extraction routine now periodically updates the Tcl display.

  Revision 1.4  1996/01/12 16:24:42  jussi
  Fixed comparison between signed and unsigned integer.

  Revision 1.3  1996/01/11 21:00:02  jussi
  Added parsing of range information and reduced data buffer
  size to 16 kB.

  Revision 1.2  1996/01/10 00:40:52  jussi
  Added computation of hi/lo values in schema. Improved data buffering
  so that data can be read in small (32 kB) chunks instead of reading
  into memory in one large chunk.

  Revision 1.1  1996/01/09 16:33:44  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream.h>
#include <ctype.h>

#include <tcl.h>

#include "machdep.h"
#include "Util.h"
#include "Parse.h"
#include "DevError.h"

//#define DEBUG
//#define DEBUG_IO

#define COMPUTE_MIN_MAX
#define USE_RANGE_INFO

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

static const char *resultHeader = "Result of Execution\n";
static const char *schemaFmt =
	"Schema ( < %[^>]> ) OrderAttr %d density %f, range %n";
static const char *dateAttr = "hmdy";
static const char *intAttr = "int";
static const char *doubleAttr = "double";
static const char *stringAttr = "string";
static const char *attrSep = ", ";
static const char attrTypeSep = ':';
static const char *rangeSep = " ... ";
static const char dataFieldSep = '\t';

static const char newline    = '\n';
static const char dateChar   = 'd';
static const char intChar    = 'i';
static const char doubleChar = 'f';
static const char stringChar = 's';

static char *schema = 0;
static const int maxNumAttrs = 16;
static char schemaDef[maxNumAttrs + 1];
static double minVal[maxNumAttrs];
static double maxVal[maxNumAttrs];
static int orderAttr = -1;
static float density = 0.0;

static int ParseSEQHeader(char *&buf, int &len)
{
  if (memcmp(buf, resultHeader, strlen(resultHeader))) {
    fprintf(stderr, "Unexpected header from SEQ server:\n");
    fprintf(stderr, "  %.256s\n", buf);
    return TCL_ERROR;
  }

  buf += strlen(resultHeader);
  len -= strlen(resultHeader);

  return TCL_OK;
}

static int ParseSEQSchema(char *&buf, int &len, char *schemafile,
			  char *schematype)
{
  // when this function is called the first time, space is allocated for
  // for schema and minVal/maxVal are initialized

  if (!schema) {
    char *eol = (char *)memchr((void *)buf, newline, len);
    if (!eol) {
      fprintf(stderr, "Unexpected schema from SEQ server:\n");
      fprintf(stderr, "  %.256s\n", buf);
      return TCL_ERROR;
    }

    *eol = 0;

#ifdef DEBUG
    printf("Initializing schema definition %s\n", buf);
#endif

    schema = CopyString(buf);
    memset(schemaDef, 0, maxNumAttrs + 1);
    for(int a = 0; a < maxNumAttrs; a++) {
      minVal[a] = 1.0;
      maxVal[a] = 0.0;
    }
    orderAttr = -1;
    density = 0.0;

    eol++;
    len -= (eol - buf);
    buf = eol;
  }

  DOASSERT(schema, "No schema");

  // allocate space that is large enough to hold the attribute
  // list; schema definition includes it so it must be enough
  // space
  char attrDef[strlen(schema) + 1];

  int bytes;
  if (sscanf(schema, schemaFmt, attrDef, &orderAttr, &density, &bytes) < 3) {
    fprintf(stderr, "Invalid schema: %s\n", schema);
    return TCL_ERROR;
  }

  // remove trailing blank from attribute definition
  attrDef[strlen(attrDef) - 1] = 0;

#ifdef USE_RANGE_INFO
  char *rangeDef = &schema[bytes];

  // isolate low and high values in the range definition
  char *rangeBegin = rangeDef;
  char *rangeEnd = strstr(rangeDef, rangeSep);
  if (!rangeEnd) {
    fprintf(stderr, "Range end not specified: %s\n", rangeDef);
    return TCL_ERROR;
  }
  rangeEnd += strlen(rangeSep);

  // attach range information to corresponding attribute; note that
  // the range information may be parsed incorrectly (as floats) the
  // first time the schema is parsed; in the second round, the attribute
  // types are known and dates will be parsed accordingly
  if (schemaDef[orderAttr] == dateChar) {
    minVal[orderAttr] = ParseSQLTimestamp(rangeBegin);
    maxVal[orderAttr] = ParseSQLTimestamp(rangeEnd);
  } else {
    minVal[orderAttr] = atof(rangeBegin);
    maxVal[orderAttr] = atof(rangeEnd);
  }
#endif

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

  char *attrName = attrDef;

  for(int i = 0;; i++) {
    if (i >= maxNumAttrs) {
      fprintf(stderr, "Too many attributes in schema: %s\n", attrDef);
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
      if (i == orderAttr)
	fprintf(fp, "sorted ");
      fprintf(fp, "attr %s ", attrName);
    }

    char *nextAttr = 0;
    if (!strncmp(colon, dateAttr, strlen(dateAttr))) {
      schemaDef[i] = dateChar;
      if (fp) {
	fprintf(fp, "date");
	if (maxVal[i] >= minVal[i])
	  fprintf(fp, " hi %ld lo %ld", (long)maxVal[i], (long)minVal[i]);
      }
      nextAttr = colon + strlen(dateAttr);
    } else if (!strncmp(colon, intAttr, strlen(intAttr))) {
      schemaDef[i] = intChar;
      if (fp) {
	fprintf(fp, "int");
	if (maxVal[i] >= minVal[i])
	  fprintf(fp, " hi %ld lo %ld", (long)maxVal[i], (long)minVal[i]);
      }
      nextAttr = colon + strlen(intAttr);
    } else if (!strncmp(colon, doubleAttr, strlen(doubleAttr))) {
      schemaDef[i] = doubleChar;
      if (fp) {
	fprintf(fp, "double");
	if (maxVal[i] >= minVal[i])
	  fprintf(fp, " hi %g lo %g", maxVal[i], minVal[i]);
      }
      nextAttr = colon + strlen(doubleAttr);
    } else if (!strncmp(colon, stringAttr, strlen(stringAttr))) {
      schemaDef[i] = stringChar;
      if (fp)
	fprintf(fp, "string");
      nextAttr = colon + strlen(stringAttr);
    } else {
      fprintf(stderr, "Invalid attribute type: %s\n", colon);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }

    if (fp)
      fprintf(fp, "\n");

    if (!*nextAttr)
      break;

    if (strncmp(nextAttr, attrSep, strlen(attrSep))) {
      fprintf(stderr, "Invalid attribute separator: %s\n", nextAttr);
      if (fp)
	fclose(fp);
      return TCL_ERROR;
    }

    attrName = nextAttr + strlen(attrSep);
  }
  
#ifdef DEBUG
  printf("Parsed schema as %s, ordered attribute %d\n", schemaDef,
	 orderAttr);
#endif

  if (fp && fclose(fp)) {
    fprintf(stderr, "Cannot close schema file %s: ", schemafile);
    perror("fclose");
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int WriteSEQRecord(char *rec, int len, FILE *fp, char *cachefile)
{
  DOASSERT(rec && fp && cachefile, "Invalid parameters");

#ifdef DEBUG_IO
  printf("Converting record (%d bytes): %s\n", len, rec);
#endif

  const int numAttrs = strlen(schemaDef);

  for(int i = 0; i < numAttrs; i++) {
    char *fieldSep = strchr(rec, dataFieldSep);
    if (!fieldSep) {
      fprintf(stderr, "Illegal data record at field %d: %s\n", i, rec);
      fclose(fp);
      return TCL_ERROR;
    }
    *fieldSep = 0;

#ifdef COMPUTE_MIN_MAX
    double value = 0.0;
#endif

    if (schemaDef[i] == dateChar) {
      time_t tim = ParseSQLTimestamp(rec);
      if (tim == (time_t)-1) {
	fprintf(stderr, "Invalid date field: %s\n", rec);
	fclose(fp);
	return TCL_ERROR;
      }
      fprintf(fp, "%ld", (long int)tim);
#ifdef COMPUTE_MIN_MAX
      value = (double)tim;
#endif
    } else {
      // write other attribute types as-is
      fwrite(rec, strlen(rec), 1, fp);
#ifdef COMPUTE_MIN_MAX
      value = atof(rec);
#endif
    }

#ifdef COMPUTE_MIN_MAX
    if (minVal[i] > maxVal[i])
      minVal[i] = maxVal[i] = value;
    else if (value < minVal[i])
      minVal[i] = value;
    else if (value > maxVal[i])
      maxVal[i] = value;
#endif

    rec = fieldSep + 1;

    if (i < numAttrs - 1)
      fprintf(fp, ",");
  }

  fprintf(fp, "\n");

  return TCL_OK;
}

int ParseSEQData(char *buf, int size, int len, int sock, char *cachefile)
{
  DOASSERT(buf && cachefile, "Invalid parameters");

  const char *resultTrailer = "\n";

  FILE *fp = fopen(cachefile, "w");
  if (!fp) {
    fprintf(stderr, "Cannot create cache file %s: ", cachefile);
    perror("fopen");
    return TCL_ERROR;
  }

  int pos = 0;

  printf("Transferring data.");
  fflush(stdout);
#ifdef DEBUG
  printf("\n");
#endif

  for(unsigned long num = 0;; num++) {
    if (num % 500 == 0)
      UPDATE_TCL;

    char *line = buf + pos;
    if (!memcmp(line, resultTrailer, strlen(resultTrailer)))
      break;

    char *eol = (char *)memchr((void *)line, newline, len);

    if (!eol || *(eol + 1) != newline) {
      if (line[len - 1] != 0) {
	// terminating null not received yet, so go fetch more data
#ifdef DEBUG
	printf("Partial record, fetching more data\n");
#else
	printf(".");
	fflush(stdout);
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
	eol = (char *)memchr((void *)line, newline, len);
      }
      if (!eol || *(eol + 1) != newline) {
	fprintf(stderr, "Unexpected data from SEQ server:\n");
	fprintf(stderr, "  %.256s\n", line);
	fclose(fp);
	return TCL_ERROR;
      }
    }

    *eol = 0;                           // replace newline with null
    int status = WriteSEQRecord(line, strlen(line), fp, cachefile);
    if (status != TCL_OK)
      return status;

    eol += 2;                           // skip over both newlines
    len -= (eol - line);
    pos += (eol - line);
  }

#ifndef DEBUG
  printf("\n");
#endif

  if (fclose(fp)) {
    fprintf(stderr, "Cannot close cache file %s: ", cachefile);
    perror("close");
    return TCL_ERROR;
  }

  return TCL_OK;
}

int seq_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  DOASSERT(argc == 7, "Invalid parameters");

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

  // send query plus terminating null to SEQ server
  int bytes = strlen(cmdbuf) + 1;
  if (write(sock, cmdbuf, bytes) != bytes) {
    reportErrSys("Error in SEQ server connection: write");
    return TCL_ERROR;
  }
  
  delete cmdbuf;

  char recvBuf[16 * 1024];
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

  status = ParseSEQSchema(buf, bytes, 0, 0);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  // shift data to beginning of buffer
  memcpy(recvBuf, buf, bytes);
  status = ParseSEQData(recvBuf, sizeof recvBuf, bytes, sock, cachefile);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  status = ParseSEQSchema(buf, bytes, schemafile, schematype);
  if (status != TCL_OK) {
    close(sock);
    return status;
  }

  delete schema;
  schema = 0;

  if (close(sock)) {
    perror("Error in SEQ server connection: close");
    return TCL_ERROR;
  }

  return TCL_OK;
}
