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
  Implementation of DataSource class.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1996/05/22 17:51:55  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.3  1996/05/07 22:28:14  jussi
  Reverted the changes made in the previous check-in because I
  found a better way to fix the problem where only the tail
  part of a schema file name is returned when a session is saved.

  Revision 1.2  1996/05/07 22:13:44  jussi
  Added virtual method getName() to DataSourceFile which returns
  the filename, not the regular name or alias. The filename is
  needed when a session file is saved.

  Revision 1.1  1996/05/07 16:04:17  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.
 */

#define _DataSource_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>

#include "DataSource.h"
#include "Util.h"
#include "DevError.h"
#include "tapedrive.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSource::DataSource
 * DataSource constructor.
 */
DataSource::DataSource(char *label)
{
	DO_DEBUG(printf("DataSource::DataSource(%s)\n",
		(label != NULL) ? label : "<null>"));

	if (label != NULL)
	{
		_label = strdup(label);
	}
	else
	{
		_label = NULL;
	}
}

/*------------------------------------------------------------------------------
 * function: DataSource::~DataSource
 * DataSource destructor.
 */
DataSource::~DataSource()
{
	DO_DEBUG(printf("DataSource::~DataSource()\n"));

	if (_label != NULL)
	{
		delete [] _label;
	}
}

/*------------------------------------------------------------------------------
 * function: DataSource::getLabel
 * Get the label of a DataSource object.
 */
char *
DataSource::getLabel()
{
	DO_DEBUG(printf("DataSource::getLabel()\n"));

	return _label;
}

/*------------------------------------------------------------------------------
 * function: DataSource::Fgets
 * This is a dummy function to catch calls to Fgets() for derived classes
 * that don't implement it.
 */
char *
DataSource::Fgets(char *buffer, int size)
{
	DO_DEBUG(printf("DataSource::Fgets()\n"));

	fprintf(stderr, "Fgets method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSource::Fread
 * This is a dummy function to catch calls to Fread() for derived classes
 * that don't implement it.
 */
size_t
DataSource::Fread(char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSource::Fread()\n"));

	fprintf(stderr, "Fread method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSource::Read
 * This is a dummy function to catch calls to Read() for derived classes
 * that don't implement it.
 */
size_t
DataSource::Read(char *buf, int byteCount)
{
	DO_DEBUG(printf("DataSource::Read()\n"));

	fprintf(stderr, "Read method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSource::Fwrite
 * This is a dummy function to catch calls to Fwrite() for derived classes
 * that don't implement it.
 */
size_t
DataSource::Fwrite(const char *buf, size_t size, size_t itemCount)
{
	DO_DEBUG(printf("DataSource::Fwrite()\n"));

	fprintf(stderr, "Fwrite method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSource::Write
 * This is a dummy function to catch calls to Write() for derived classes
 * that don't implement it.
 */
size_t
DataSource::Write(const char *buf, size_t byteCount)
{
	DO_DEBUG(printf("DataSource::Write()\n"));

	fprintf(stderr, "Write method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSource::append
 * This is a dummy function to catch calls to append() for derived classes
 * that don't implement it.
 */
int
DataSource::append(void *buf, int recSize)
{
	DO_DEBUG(printf("DataSource::append()\n"));

	fprintf(stderr, "append method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return -1;
}

/*------------------------------------------------------------------------------
 * function: DataSource::printStats
 * This is a dummy function to catch calls to printStats() for derived classes
 * that don't implement it.
 */
void
DataSource::printStats()
{
	DO_DEBUG(printf("DataSource::printStats()\n"));

	fprintf(stderr, "printStats method not implemented for class %s\n",
		objectType());
	DOASSERT(false, "");

	return;
}

/*============================================================================*/
