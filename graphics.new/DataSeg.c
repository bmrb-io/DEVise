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
  Implementation of DataSeg (data segment class).
 */

/*
  $Id$

  $Log$
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

static char *	srcFile = __FILE__;

char *		DataSeg::_label = NULL;
char *		DataSeg::_filename = NULL;
long		DataSeg::_offset = 0;
long		DataSeg::_length = 0;

/*------------------------------------------------------------------------------
 * function: DataSeg::Set
 * Set the information about a data segment.
 */
void
DataSeg::Set(char *label, char *filename, long offset, long length)
{
	if (label == NULL) label = "";
	if (filename == NULL) filename = "";

	DO_DEBUG(printf("DataSeg::Set(%s, %s, %ld, %ld)\n", label, filename,
		offset, length));

	if (_label != NULL)
	{
		delete _label;
		_label = NULL;
	}
	if (label != NULL) _label = strdup(label);

	if (_filename != NULL)
	{
		delete _filename;
		_filename = NULL;
	}
	if (filename != NULL) _filename = strdup(filename);

	_offset = offset;
	_length = length;
}

/*------------------------------------------------------------------------------
 * function: DataSeg::Get
 * Get the information about a data segment.
 */
void
DataSeg::Get(char *&label, char *&filename, long &offset, long &length)
{
	label = _label;
	filename = _filename;
	offset = _offset;
	length = _length;
}

/*============================================================================*/
