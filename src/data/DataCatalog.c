/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DataCatalog class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/11/19 21:13:07  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h> // for MAXPATHLEN

#include "DataCatalog.h"
#include "Util.h"
#include "Init.h"
#include "DevError.h"

#define DEBUG 0

static DataCatalog *catInstance = NULL;
static const int maxEntryLen = 3 * MAXPATHLEN;

static const char *directoryType = "Directory";
static const char *unixfileType = "UNIXFILE";

/*------------------------------------------------------------------------------
 * function: ParseQuotedString
 * Find the first string enclosed in quotes in inBuf (the quotes are not
 * included in the returned string); returns the index of the first charater
 * *after* the string (0 if no string found).
 *
 * Note: outBuf can be NULL if you just want to find the start of the next
 * string.
 */
static int
ParseQuotedString(const char *inBuf, char *outBuf, int outBufSize)
{
#if (DEBUG >= 3)
  printf("ParseQuotedString(%s)\n", inBuf);
#endif

  // TEMP -- report error and return -1 if buffer overflows and size > 0

  if (outBuf == NULL) outBufSize = 0;

  const char *inChar = inBuf;
  char *outEnd = outBuf;
  int nextIndex = 0;

  // Bypass all characters before first ".
  while (*inChar != '\0' && *inChar != '"') {
    inChar++;
  }
  if (*inChar == '"') {
    inChar++;

    // Remember where string started.
    const char *inBegin = inChar;

    // Now find closing quote.
    while (*inChar != '\0' && *inChar != '"') {
      inChar++;
    }
    if (*inChar == '"') {
      nextIndex = inChar - inBuf + 1;

      // Copy the string to the output buffer, if the buffer is big enough
      // (the output buffer is not yet terminated).
      int length = (inChar - 1) - inBegin + 1;
      if (length < outBufSize) {
        strncpy(outBuf, inBegin, length);
        outEnd += length;
      }
    }
  }

  // Terminate the output string.
  if (outEnd - outBuf < outBufSize) *outEnd = '\0';

#if (DEBUG >= 3)
  if (outBuf != NULL) printf("  found string {%s}\n", outBuf);
#endif

  return nextIndex;
}

/*------------------------------------------------------------------------------
 * function: ParseSpaceString
 * Find the first whitespace-delimited string in inBuf; returns the index
 * of the first charater *after* the string (0 if no string found).
 *
 * Note: outBuf can be NULL if you just want to find the start of the next
 * string.
 */
static int
ParseSpaceString(const char *inBuf, char *outBuf, int outBufSize)
{
#if (DEBUG >= 3)
  printf("ParseSpaceString(%s)\n", inBuf);
#endif

  // TEMP -- report error and return -1 if buffer overflows and size > 0

  if (outBuf == NULL) outBufSize = 0;

  const char *inChar = inBuf;
  char *outChar = outBuf;
  int nextIndex = 0;

  // Bypass leading whitespace.
  while (*inChar != '\0' && isspace(*inChar)) {
    inChar++;
  }
  if (*inChar != '\0') {

    // Copy from input to output until we hit whitespace, making sure we
    // don't overflow the output buffer.  If the buffer overflows, we keep
    // going without copying so the return value is correct.
    while (*inChar != '\0' && !isspace(*inChar)) {
      if (outChar - outBuf < outBufSize - 1) {
        *outChar = *inChar;
        outChar++;
      }
      inChar++;
    }
    nextIndex = inChar - inBuf;
  }

  // Terminate the output string.
  if (outChar - outBuf < outBufSize) *outChar = '\0';

#if (DEBUG >= 3)
  if (outBuf != NULL) printf("  found string {%s}\n", outBuf);
#endif

  return nextIndex;
}

/*------------------------------------------------------------------------------
 * function: ParseCatEntry
 * Parse a catalog entry.
 *
 * Note: buffers can be NULL if you don't need all fields.
 */
static void
ParseCatEntry(char *catEntry, char *nameBuf, int nameBufSize, char *typeBuf,
  int typeBufSize, char *fileBuf, int fileBufSize)
{
#if (DEBUG >= 3)
  printf("ParseCatEntry(%s)\n", catEntry);
#endif

  // Get the name.
  int nextIndex = ParseQuotedString(catEntry, nameBuf, nameBufSize);

  // Get the type.
  nextIndex += ParseSpaceString(&catEntry[nextIndex], typeBuf, typeBufSize);

  // Get the file name.
  nextIndex += ParseSpaceString(&catEntry[nextIndex], fileBuf, fileBufSize);

#if (DEBUG >= 3)
  if (nameBuf != NULL) printf("  name: {%s}\n", nameBuf);
  if (typeBuf != NULL) printf("  type: {%s}\n", typeBuf);
  if (fileBuf != NULL) printf("  file: {%s}\n", fileBuf);
#endif
}

/*------------------------------------------------------------------------------
 * function: ReadEntry
 * Read a data source catalog entry (entries may span more than one line;
 * however, there cannot be more than one entry on a given line).
 */
static fgets_result
ReadEntry(char *buf, int bufSize, FILE *fp)
{
#if (DEBUG >= 3)
  printf("ReadEntry()\n");
#endif

  fgets_result result = fgets_ok;

  char *currChar = buf;
  int spaceLeft = bufSize;

  //
  // Keep reading lines into the buffer until we've read a line with a ';'
  // (the entry terminating character).
  //
  Boolean done = false;
  while (!done) {
    fgets_result tmpResult = nice_fgets(currChar, spaceLeft, fp);
    if (tmpResult != fgets_ok) {
      done = true;
      result = tmpResult;
    } else {
      if (strchr(currChar, ';')) {
        done = true;
      } else {
        int length = strlen(currChar);
	currChar += length;
	spaceLeft -= length;

	// Put a space at the end to make sure there's whitespace before the
	// chars from the next line.
	if (spaceLeft > 1) {
	  *currChar = ' ';
	  currChar++;
	  spaceLeft--;
	} else {
	  done = true;
	  reportErrNosys("Buffer overflow");
	  result = fgets_bufoverflow;
	}
      }
    }
  }

#if (DEBUG >= 3)
  printf("Read entry: {%s}\n", buf);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::Instance
 * Get an instance of the class (create if necessary).
 */
DataCatalog *
DataCatalog::Instance()
{
#if (DEBUG >= 1)
  printf("DataCatalog::Instance()\n");
#endif

 if (catInstance == NULL) {
   catInstance = new DataCatalog;
   DOASSERT(catInstance, "Out of memory");
 }
 return catInstance;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::DataCatalog
 * Constructor.
 */
DataCatalog::DataCatalog()
{
#if (DEBUG >= 1)
  printf("DataCatalog::DataCatalog()\n");
#endif

  char errBuf[2 * MAXPATHLEN];

  const char *defaultCatFile = "./catalog.dte";
  const char *envVar = "DEVISE_HOME_TABLE";
  const char *catFile = getenv(envVar);
  if (catFile == NULL) {
    sprintf(errBuf,
      "Environment variable %s not defined; using %s for catalog file",
      envVar, defaultCatFile);
    reportErrNosys(errBuf);
    catFile = defaultCatFile;
  }
  _catFile = CopyString(catFile);
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::~DataCatalog
 * Destructor.
 */
DataCatalog::~DataCatalog()
{
#if (DEBUG >= 1)
  printf("DataCatalog::~DataCatalog()\n");
#endif

  delete [] _catFile;
  _catFile = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::ShowEntry
 * Show the given catalog entry.
 */
char *
DataCatalog::ShowEntry(char *entryName)
{
#if (DEBUG >= 1)
  printf("DataCatalog::ShowEntry(%s)\n", entryName);
#endif

  char *entry = FindEntry(entryName, _catFile);

#if (DEBUG >= 1)
  printf("  found entry {%s}\n", entry ? entry : "NULL");
#endif

  return entry;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::ListCatalog
 * List the given data source catalog.
 */
char *
DataCatalog::ListCatalog(char *catName)
{
#if (DEBUG >= 1)
  printf("DataCatalog::ListCatalog(%s)\n", catName);
#endif

  char *list = NULL;
  char errBuf[2 * MAXPATHLEN];

  //
  // Find the right catalog file.
  //
  const int catBufSize = MAXPATHLEN;
  char catBuf[catBufSize];
  char *catFile = FindCatFile(catName, catBuf, catBufSize);

  //
  // Find the size of the catalog file so we have a guess as to how big
  // the output buffer should be.
  //
  int listSize;
  {
    struct stat catStat;
    if (stat(catFile, &catStat) != 0) {
      sprintf(errBuf, "Can't get status of file %s", catFile);
      reportErrSys(errBuf);
      listSize = 100;
    } else {
      listSize = catStat.st_size / 3; // a guess
    }
  }

  //
  // Now scan that catalog file and get all of the entries.
  //
  if (catFile != NULL) {
    FILE *fp = fopen(catFile, "r");
    if (fp == NULL) {
      sprintf(errBuf, "Unable to open data source catalog file %s\n", catFile);
      reportErrSys(errBuf);
    } else {
      list = new char[listSize];
      list[0] = '\0';
      char *outChar = list;

      Boolean done = false;
      while (!done) {
        const int bufSize = maxEntryLen;
        char buf[bufSize];

	fgets_result tmpResult = ReadEntry(buf, bufSize, fp);

	if (tmpResult == fgets_eof) {
          done = true;
        } else if (tmpResult == fgets_ok) {

	  // Got an entry -- parse it and print the name and type to the
	  // output buffer.
	  int lastChar = strlen(buf) - 1;
	  if (buf[lastChar] == '\n') buf[lastChar] = '\0';

	  char nameBuf[bufSize], typeBuf[bufSize], fileBuf[bufSize];
	  ParseCatEntry(buf, nameBuf, bufSize, typeBuf, bufSize, NULL, 0);

	  const char *format = "{\"%s\" \"%s\"} ";

	  // Increase output list size if necessary.
          if ((outChar + strlen(format) + strlen(nameBuf) +
	      strlen(typeBuf) + 1) >= (list + listSize)) {
	    listSize *= 2;
	    char *newList = new char[listSize];
	    DOASSERT(newList, "Out of memory");
	    strcpy(newList, list);
	    delete [] list;
	    list = newList;
	    outChar = list + strlen(list);
	  }

	  sprintf(outChar, format, nameBuf, typeBuf);
	  outChar += strlen(outChar);
	  DOASSERT(outChar < list + listSize, "Output list overflow");
	} else {
	  reportErrNosys("Error reading data catalog entry");
	  done = true;
	  delete [] list;
	  list = NULL;
	}
      }

      if (fclose(fp) != 0) {
        sprintf(errBuf, "Error closing data source catalog file %s\n", catFile);
        reportErrSys(errBuf);
      }
    }
  }

  return list;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::AddEntry
 * Add an entry to the given catalog.
 */
int
DataCatalog::AddEntry(char *catName, char *entry)
{
#if (DEBUG >= 1)
  printf("DataCatalog::AddEntry(%s, %s)\n", catName, entry);
#endif

  int result = 0;
  char errBuf[2 * MAXPATHLEN];

  //TEMP -- this should probably check to make sure that an entry with
  // the given name doesn't already exist, even though the GUI seems to
  // check for this

  {
    const int bufSize = 1024;
    char nameBuf[bufSize], typeBuf[bufSize];
    (void) ParseQuotedString(entry, nameBuf, bufSize);
    (void) ParseCatEntry(entry, nameBuf, bufSize, typeBuf, bufSize, NULL, 0);

    //
    // Check for illegal characters in entry name.
    //
    char *inChar = nameBuf;
    Boolean error = false;
    while(*inChar != '\0' && !error) {
      if (isspace(*inChar)) {
	reportErrNosys("Data source name cannot contain whitespace");
        error = true;
	result = -1;
      } else if (*inChar == '.') {
	reportErrNosys("Data source name cannot contain periods");
        error = true;
	result = -1;
      }
      inChar++;
    }

    //
    // Check for illegal/unsupported data source types.
    //
    if (strcmp(typeBuf, directoryType) && strcmp(typeBuf, unixfileType)) {
      sprintf(errBuf, "Data source type %s is not currently supported by "
        "this version of DEVise", typeBuf);
      reportErrNosys(errBuf);
      result = -1;
    }
  }

  if (result == 0) {
    //
    // Find the right catalog file.
    //
    const int catBufSize = MAXPATHLEN;
    char catBuf[catBufSize];
    char *catFile = FindCatFile(catName, catBuf, catBufSize);

    FILE *fp = fopen(catFile, "a");
    if (fp == NULL) {
      sprintf(errBuf, "Unable to open data source catalog file %s\n", catFile);
      reportErrSys(errBuf);
      result = -1;
    } else {
      // Note: the space after the semicolon is critical if the DTE is ever
      // going to read the table.  RKW 1998-11-18.
      if (fprintf(fp, "%s ; \n", entry) < 0) {
        reportErrSys("Error writing to data source catalog file");
        result = -1;
      }

      if (fclose(fp) != 0) {
        sprintf(errBuf, "Error closing data source catalog file %s\n", catFile);
        reportErrSys(errBuf);
        result = -1;
      }
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::DeleteEntry
 * Delete the given entry.
 */
int
DataCatalog::DeleteEntry(char *entryName)
{
#if (DEBUG >= 1)
  printf("DataCatalog::DeleteEntry(%s)\n", entryName);
#endif

  int result = 0;
  char errBuf[2 * MAXPATHLEN];

  //
  // Split the source name into the directory name and the non-directory
  // part.
  //
  char *dirName = CopyString(entryName);
  char *partName = NULL;
  char *delim = strrchr(dirName, '.');
  if (delim != NULL) {
    *delim = '\0';
    partName = delim + 1;
  } else {
    sprintf(errBuf, "Illegal data source name (no '.'): %s", entryName);
    result = -1;
  }

  //
  // Now find the catalog file for that directory, and the size of the
  // catalog file.
  //
  char *catFile = NULL;
  int catSize;

  const int catBufSize = MAXPATHLEN;
  char catBuf[catBufSize];
  if (result == 0) {
    catFile = FindCatFile(dirName, catBuf, catBufSize);

    struct stat catStat;
    if (stat(catFile, &catStat) != 0) {
      sprintf(errBuf, "Can't get status of file %s", catFile);
      reportErrSys(errBuf);
      result = -1;
    } else {
      catSize = catStat.st_size;
    }
  }

  //
  // Allocate a buffer to copy the non-deleted entries into.
  //
  char *outBuf = NULL;
  if (result == 0) {
    outBuf = new char[catSize];
    DOASSERT(outBuf, "Out of memory");
  }

  //
  // Read all but the deleted entry into the temporary buffer.
  //
  int newCatSize = 0;
  if (result == 0) {

    FILE *catfp = fopen(catFile, "r");

    if (catfp == NULL) {
      sprintf(errBuf,
        "Unable to open data source catalog file %s\n", catFile);
      reportErrSys(errBuf);
      result = -1;
    } else {

      char *outChar = outBuf;
      Boolean found = true;
      Boolean done = false;
      while (!done) {
        const int bufSize = maxEntryLen;
        char buf[bufSize];

	fgets_result tmpResult = ReadEntry(buf, bufSize, catfp);

	if (tmpResult == fgets_eof) {
          done = true;
        } else if (tmpResult == fgets_ok) {
          const int nameBufSize = 1024;
          char nameBuf[nameBufSize];

          (void) ParseQuotedString(buf, nameBuf, nameBufSize);

          if (!strcmp(partName, nameBuf)) {
	    found = true; // Found the entry to delete.
	  } else {
	    strcpy(outChar, buf);
	    outChar += strlen(outChar);
	    *outChar = '\n';
	    outChar++;
	    DOASSERT(outChar < outBuf + catSize, "Buffer overflow");
	  }
	} else {
	  reportErrNosys("Error reading data catalog entry");
	  done = true;
	  result = -1;
	}
      }
      newCatSize = outChar - outBuf;

      if (fclose(catfp) != 0) {
        sprintf(errBuf,
          "Error closing data source catalog file %s\n", catFile);
        reportErrSys(errBuf);
        result = -1;
      }

      if (!found) {
	sprintf(errBuf, "Did not find data source catalog entry %s", entryName);
	reportErrNosys(errBuf);
        result = -1;
      }
    }
  }

  //
  // Now write the temporary buffer over the catalog file.
  //
  if (result == 0) {
    FILE *catfp = fopen(catFile, "w");
    if (catfp == NULL) {
      sprintf(errBuf,
        "Unable to open data source catalog file %s", catFile);
      reportErrSys(errBuf);
      result = -1;
    } else {
      if (newCatSize > 0 && fwrite(outBuf, newCatSize, 1, catfp) == 0) {
        sprintf(errBuf, "Error writing to data source catalog file %s",
	  catFile);
        reportErrSys(errBuf);
        result = -1;
      }

      if (fclose(catfp) != 0) {
        sprintf(errBuf,
          "Error closing data source catalog file %s\n", catFile);
        reportErrSys(errBuf);
        result = -1;
      }
    }
  }

  delete [] outBuf;
  delete [] dirName;

  return result;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*------------------------------------------------------------------------------
 * function: DataCatalog::FindEntry
 * Find the given entry in the given catalog file.
 */
char *
DataCatalog::FindEntry(char *entryName, char *catFile)
{
#if (DEBUG >= 2)
  printf("DataCatalog::FindEntry(%s, %s)\n", entryName, catFile);
#endif

  char *entry = NULL;
  char errBuf[2 * MAXPATHLEN];

  char *partName;

  // Get rid of leading '.'s, if any.
  {
    char *tmpChar = entryName;
    while (*tmpChar == '.') { tmpChar++; }
    partName = CopyString(tmpChar);
  }

  // Figure out whether name contains directories.
  Boolean hasDir = false;
  char *nextName = NULL;

  char *delim = strchr(partName, '.');
  if (delim != NULL) {
    hasDir = true;
    *delim = '\0';
    nextName = delim + 1;
  }

  //
  // Now go look for an entry matching <name> in the given catalog.
  //
  FILE *fp = fopen(catFile, "r");
  if (fp == NULL) {
    sprintf(errBuf, "Unable to open data source catalog file %s\n", catFile);
    reportErrSys(errBuf);
  } else {

    Boolean done = false;
    while (!done) {
      const int bufSize = maxEntryLen;
      char buf[bufSize];

      fgets_result tmpResult = ReadEntry(buf, bufSize, fp);

      if (tmpResult == fgets_eof) {
        done = true;
      } else if (tmpResult == fgets_ok) {

	// Got an entry...
        const int nameBufSize = 1024;
	char nameBuf[nameBufSize];
        (void) ParseQuotedString(buf, nameBuf, nameBufSize);
        if (!strcmp(partName, nameBuf)) {

	  // ...entry matches the name we're looking for...
	  done = true;
	  if (hasDir) {

	    // We're looking for a directory -- go down one more level.
	    char typeBuf[bufSize];
	    char fileBuf[bufSize];
            ParseCatEntry(buf, NULL, 0, typeBuf, bufSize, fileBuf, bufSize);
	    if (strcmp(typeBuf, directoryType)) {
	      sprintf(errBuf, "%s is not a data source directory", partName);
	      reportErrNosys(errBuf);
	      entry = NULL;
	    } else {
	      entry = FindEntry(nextName, fileBuf);
	    }
	  } else {

	    // ...not a directory -- we're done.
	    int lastChar = strlen(buf) - 1;
	    if (buf[lastChar] == '\n') buf[lastChar] = '\0';
	    entry = CopyString(buf);
	  }
	}
      } else {
        reportErrNosys("Error reading data catalog entry");
        done = true;
      }
    }

    if (fclose(fp) != 0) {
      sprintf(errBuf, "Error closing data source catalog file %s\n", catFile);
      reportErrSys(errBuf);
    }
  }

  delete [] partName;

#if (DEBUG >= 2)
  printf("  found entry {%s}\n", entry ? entry : "NULL");
#endif

  return entry;
}

/*------------------------------------------------------------------------------
 * function: DataCatalog::FindCatFile
 * Find the catalog file for the given directory name.
 */
char *
DataCatalog::FindCatFile(char *catName, char *buf, int bufSize)
{
#if (DEBUG >= 2)
  printf("DataCatalog::FindCatFile(%s)\n", catName);
#endif

  char *catFile;

  if (!strcmp(catName, ".") || !strcmp(catName, "")) {
    catFile = _catFile;
  } else {
    char *catEntry = FindEntry(catName, _catFile);

    const int typeBufSize = 1024;
    char typeBuf[typeBufSize];
    ParseCatEntry(catEntry, NULL, 0, typeBuf, typeBufSize, buf, bufSize);

    if (strcmp(typeBuf, directoryType)) {
      reportErrNosys("Name requested is not a directory");
      catFile = NULL;
    } else {
      catFile = buf;
    }
  }

  return catFile;
}

/*============================================================================*/
