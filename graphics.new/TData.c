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
  Revision 1.3  1996/01/27 00:17:27  jussi
  Added copyright notice and cleaned up a bit.

  Revision 1.2  1995/09/05 22:15:44  jussi
  Added CVS header.
*/

//#define DEBUG

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "TData.h"
#include "Exit.h"
#include "Util.h"
#include "DevFileHeader.h"
#include "DevError.h"
#include "AttrList.h"

/*------------------------------------------------------------------------------
 * function: TData::~TData
 * TData destructor.
 */
TData::~TData()
{
}

/*------------------------------------------------------------------------------
 * function: TData::WriteRecs
 * For writing records. Default: not implemented.
 */

void TData::WriteRecs(RecId startId, int numRecs, void *buf)
{
  fprintf(stderr, "TData::WRiteRecs: not implemented\n");
  Exit::DoExit(2);
}

/*------------------------------------------------------------------------------
 * function: TData::Save
 * Save this TData (schema(s) and actual data) to the given file.
 */
DevStatus
TData::Save(char *filename)
{
  DO_DEBUG(printf("TData::Save(%s)\n", filename));

  DevStatus result = StatusOk;

  int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (fd == -1)
  {
    reportError("Can't open file", errno);
    result = StatusFailed;
  }
  else
  {
    if (WriteHeader(fd) == StatusFailed) result = StatusFailed;
    if (WriteLogSchema(fd) == StatusFailed) result = StatusFailed;
    if (WritePhysSchema(fd) == StatusFailed) result = StatusFailed;
    if (WriteData(fd) == StatusFailed) result = StatusFailed;

    if (close(fd) == -1)
    {
      reportError("Can't close file", errno);
      result = StatusFailed;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteHeader
 * Write the appropriate file header to the given file descriptor.
 */
DevStatus
TData::WriteHeader(int fd)
{
  DevStatus result = StatusOk;

  char *header = DevFileHeader::Get(FILE_TYPE_TDATA);
  int headerLen = strlen(header);

  if (write(fd, header, headerLen) != headerLen)
  {
    reportError("Error writing header", errno);
    result = StatusFailed;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteLogSchema
 * Write the logical schema, if any, to the given file descriptor.
 */
DevStatus
TData::WriteLogSchema(int fd)
{
  DevStatus result = StatusOk;





  char *endMark = "endSchema\n";
  int endMarkLen = strlen(endMark);
  if (write(fd, endMark, endMarkLen) != endMarkLen)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WritePhysSchema
 * Write the physical schema to the given file descriptor.
 */
DevStatus
TData::WritePhysSchema(int fd)
{
  DevStatus result = StatusOk;

  char *type = "\ntype <name> ascii|binary\n";//TEMPTEMP
  int typeLen = strlen(type);
  if (write(fd, type, typeLen) != typeLen)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }

  char *commSep = "comment //\nseparator ','\n";//TEMPTEMP
  int commSepLen = strlen(commSep);
  if (write(fd, commSep, commSepLen) != commSepLen)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }

  AttrList *attrListP = GetAttrList();
  if (attrListP == NULL)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }
  else
  {
    attrListP->Write(fd);
  }

  char *endMark = "endSchema\n";
  int endMarkLen = strlen(endMark);
  if (write(fd, endMark, endMarkLen) != endMarkLen)
  {
    reportError("Error writing schema", errno);
    result = StatusFailed;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TData::WriteData
 * Write the actual data to the given file descriptor.
 */
DevStatus
TData::WriteData(int fd)
{
  DevStatus result = StatusOk;

  return result;
}

