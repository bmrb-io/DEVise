/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DataSeg (data segment class).
 */

/*
  $Id$

  $Log$
  Revision 1.5  2000/03/14 17:05:27  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.4  1999/10/18 15:36:41  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.3  1998/03/04 19:11:01  wenger
  Fixed some more dynamic memory errors.

  Revision 1.2  1996/08/05 19:48:53  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.1  1996/06/04 14:21:40  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

 */

#define _DataSeg_C_

//#define DEBUG

#include <string.h>

#include "DataSeg.h"
#include "Util.h"

/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static const char *	srcFile = __FILE__;

char *		DataSeg::_label = NULL;
char *		DataSeg::_filename = NULL;
long		DataSeg::_offset = 0;
long		DataSeg::_length = 0;

/*------------------------------------------------------------------------------
 * function: DataSeg::Set
 * Set the information about a data segment.
 */
void
DataSeg::Set(const char *label, const char *filename, long offset, long length)
{
	if (label == NULL) label = "";
	if (filename == NULL) filename = "";

	DO_DEBUG(printf("DataSeg::Set(%s, %s, %ld, %ld)\n", label, filename,
		offset, length));

	if (_label != NULL)
	{
		FreeString(_label);
		_label = NULL;
	}
	if (label != NULL) _label = CopyString(label);

	if (_filename != NULL)
	{
		FreeString(_filename);
		_filename = NULL;
	}
	if (filename != NULL) _filename = CopyString(filename);

	_offset = offset;
	_length = length;

	return;
}

/*------------------------------------------------------------------------------
 * function: DataSeg::Get
 * Get the information about a data segment.
 */
void
DataSeg::Get(char *&label, char *&filename, long &offset, long &length)
{
	DO_DEBUG(printf("DataSeg::Get()\n"));

	label = _label;
	filename = _filename;
	offset = _offset;
	length = _length;

	return;
}

/*============================================================================*/
